/*
 * Copyright 2015 AASHTO/ITE/NEMA.
 * American Association of State Highway and Transportation Officials,
 * Institute of Transportation Engineers and
 * National Electrical Manufacturers Association.
 *
 * This file is part of the Advanced Transportation Controller (ATC)
 * Application Programming Interface Validation Suite (APIVS).
 *
 * The APIVS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * The APIVS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the APIVS.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 *  Virtual loopback driver for synchronous serial ports as defined in
 *  the Advanced Transportation Controller (ATC) standards.
 *
 */

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/kfifo.h>
#include <linux/poll.h>
#include <asm/ioctls.h>

#include <linux/atc_spxs.h>

#define FLAG_OPEN       0x00000001
#define IS_OPEN(port)	(port->flags & FLAG_OPEN)

/* Module information */
#define VL_SYNC_MINOR   0	/* base of sequential minor numbers */
#define VL_SYNC_MINORS  6	/* number devices (even) */
int     vl_sync_major = 0;      /* dynamic assignment or module param */

struct vl_sync_port {
        struct cdev cdev;
        int line;
        pid_t pid;
	spinlock_t ctrl_lock;
        unsigned int flags;
	struct n_hdlc *n_hdlc;
	atc_spxs_config_t atc_config;
	wait_queue_head_t write_wait;
	wait_queue_head_t read_wait;
	struct fasync_struct *fasync_queue;
        struct vl_sync_port *paired_port;
};

static struct vl_sync_port vlport[6];

static int vl_sync_request_port(struct vl_sync_port *port);
static void vl_sync_release_port(struct vl_sync_port *port);
static int vl_sync_do_ioctl(struct vl_sync_port *port, unsigned int cmd, unsigned long arg);
static int vl_sync_set_iface(struct vl_sync_port *port, atc_spxs_config_t *config);
static int vl_sync_fasync(int fd, struct file *filp, int on);

#define MAX_HDLC_FRAME_SIZE     1024
  
#define DEFAULT_RX_BUF_COUNT 10
#define MAX_RX_BUF_COUNT 60
#define DEFAULT_TX_BUF_COUNT 3
#define HDLC_MAGIC 0x239e
struct n_hdlc_buf {
	struct n_hdlc_buf *link;
	int		  count;
	char		  buf[1];
};

#define	N_HDLC_BUF_SIZE	(sizeof(struct n_hdlc_buf) + MAX_HDLC_FRAME_SIZE)

struct n_hdlc_buf_list {
	struct n_hdlc_buf *head;
	struct n_hdlc_buf *tail;
	int		  count;
	struct kfifo	fifo;
	spinlock_t		spinlock;
};

struct n_hdlc {
	int			magic;
	__u32			flags;
	int			tbusy;
	int			woke_up;
	struct n_hdlc_buf	*tbuf;
	struct n_hdlc_buf_list	tx_buf_list;
	struct n_hdlc_buf_list	rx_buf_list;
	struct n_hdlc_buf_list	tx_free_buf_list;
	struct n_hdlc_buf_list	rx_free_buf_list;
};

/**
 * n_hdlc_buf_list_init - initialize specified HDLC buffer list
 * @list - pointer to buffer list
 */
static void n_hdlc_buf_list_init(struct n_hdlc_buf_list *list)
{
	memset(list, 0, sizeof(*list));
	spin_lock_init(&list->spinlock);
}	/* end of n_hdlc_buf_list_init() */

/**
 * n_hdlc_buf_put - add specified HDLC buffer to tail of specified list
 * @list - pointer to buffer list
 * @buf	- pointer to buffer
 */
static void n_hdlc_buf_put(struct n_hdlc_buf_list *list,
			   struct n_hdlc_buf *buf)
{
	unsigned long flags;
	spin_lock_irqsave(&list->spinlock,flags);

	buf->link=NULL;
	if (list->tail)
		list->tail->link = buf;
	else
		list->head = buf;
	list->tail = buf;
	(list->count)++;

	spin_unlock_irqrestore(&list->spinlock,flags);

}	/* end of n_hdlc_buf_put() */

/**
 * n_hdlc_buf_get - remove and return an HDLC buffer from list
 * @list - pointer to HDLC buffer list
 *
 * Remove and return an HDLC buffer from the head of the specified HDLC buffer
 * list.
 * Returns a pointer to HDLC buffer if available, otherwise %NULL.
 */
static struct n_hdlc_buf* n_hdlc_buf_get(struct n_hdlc_buf_list *list)
{
	unsigned long flags;
	struct n_hdlc_buf *buf;
	spin_lock_irqsave(&list->spinlock,flags);

	buf = list->head;
	if (buf) {
		list->head = buf->link;
		(list->count)--;
	}
	if (!list->head)
		list->tail = NULL;

	spin_unlock_irqrestore(&list->spinlock,flags);
	return buf;

}	/* end of n_hdlc_buf_get() */

/**
 * n_hdlc_alloc - allocate an n_hdlc instance data structure
 *
 * Returns a pointer to newly created structure if success, otherwise %NULL
 */
static struct n_hdlc *n_hdlc_alloc(void)
{
	struct n_hdlc_buf *buf;
	int i;
	struct n_hdlc *n_hdlc = kmalloc(sizeof(*n_hdlc), GFP_KERNEL);

	if (!n_hdlc)
		return NULL;

	memset(n_hdlc, 0, sizeof(*n_hdlc));

	n_hdlc_buf_list_init(&n_hdlc->rx_free_buf_list);
	n_hdlc_buf_list_init(&n_hdlc->tx_free_buf_list);
	n_hdlc_buf_list_init(&n_hdlc->rx_buf_list);
	n_hdlc_buf_list_init(&n_hdlc->tx_buf_list);

	/* allocate free rx buffer list */
	for(i=0;i<DEFAULT_RX_BUF_COUNT;i++) {
		buf = kmalloc(N_HDLC_BUF_SIZE, GFP_KERNEL);
		if (buf)
			n_hdlc_buf_put(&n_hdlc->rx_free_buf_list,buf);
		else
			pr_debug("%s(%d)n_hdlc_alloc(), kalloc() failed for rx buffer %d\n",
					__FILE__,__LINE__, i);
	}

	/* allocate free tx buffer list */
	for(i=0;i<DEFAULT_TX_BUF_COUNT;i++) {
		buf = kmalloc(N_HDLC_BUF_SIZE, GFP_KERNEL);
		if (buf)
			n_hdlc_buf_put(&n_hdlc->tx_free_buf_list,buf);
		else
			pr_debug("%s(%d)n_hdlc_alloc(), kalloc() failed for tx buffer %d\n",
					__FILE__,__LINE__, i);
	}

	/* Initialize the control block */
	n_hdlc->magic  = HDLC_MAGIC;
	n_hdlc->flags  = 0;

	return n_hdlc;

}	/* end of n_hdlc_alloc() */

static void n_hdlc_release(struct n_hdlc *n_hdlc)
{
	struct n_hdlc_buf *buf;
	
	/* Release transmit and receive buffers */
	for(;;) {
		buf = n_hdlc_buf_get(&n_hdlc->rx_free_buf_list);
		if (buf) {
			kfree(buf);
		} else
			break;
	}
	for(;;) {
		buf = n_hdlc_buf_get(&n_hdlc->tx_free_buf_list);
		if (buf) {
			kfree(buf);
		} else
			break;
	}
	for(;;) {
		buf = n_hdlc_buf_get(&n_hdlc->rx_buf_list);
		if (buf) {
			kfree(buf);
		} else
			break;
	}
	for(;;) {
		buf = n_hdlc_buf_get(&n_hdlc->tx_buf_list);
		if (buf) {
			kfree(buf);
		} else
			break;
	}
	kfree(n_hdlc->tbuf);
	kfree(n_hdlc);
	
}	/* end of n_hdlc_release() */


/*
 * ATC FIO API functions.
 */
static void *vs_sdlc_kernel_open( int channel )
{
	struct vl_sync_port *port;
	int retval;

	pr_debug("%s(%d)vs_sdlc_kernel_open() called\n",__FILE__,__LINE__);

	/* look up which uart_port instance from passed channel */
	if (channel == ATC_LKM_SP3S)
		port = &vlport[0];
	else if (channel == ATC_LKM_SP5S)
		port = &vlport[2];
	else if (channel == ATC_LKM_SP8S)
		port = &vlport[4];
	else
		return ERR_PTR(-ENODEV);

	/* ensure exclusive access */
	if (IS_OPEN(port))
		return ERR_PTR(-EBUSY);

	retval = vl_sync_request_port(port);
	if (retval)
		return ERR_PTR(retval);

	return (void *)port;
}
EXPORT_SYMBOL(vs_sdlc_kernel_open);

static int vs_sdlc_kernel_close( void *context )
{
	struct vl_sync_port *port = (struct vl_sync_port *)context;

	pr_debug("%s(%d)vs_sdlc_kernel_close() called\n",__FILE__,__LINE__);
	/* close down port */
	vl_sync_release_port(port);

	return 0;
}
EXPORT_SYMBOL(vs_sdlc_kernel_close);

static int vs_sdlc_kernel_read( void *context, void *buf, size_t count)
{
	struct vl_sync_port *port = (struct vl_sync_port *)context;
	struct n_hdlc *n_hdlc = port->n_hdlc;
	struct n_hdlc_buf *rbuf;

	pr_debug("%s(%d)vs_sdlc_kernel_read() called\n",__FILE__,__LINE__);

	/* Validate the pointers */
	if(!port || !IS_OPEN(port) || !n_hdlc)
		return -EBADF;

	rbuf = n_hdlc_buf_get(&n_hdlc->rx_buf_list);
	if (!rbuf) {
		/* no data */
		return 0;
	}

	/* Copy the data to the caller's buffer */
	if (count > rbuf->count)
		count = rbuf->count;
	memcpy(buf, rbuf->buf, count);
pr_debug("vs_sdlc_kernel_read: %d bytes: %02x\n", count, rbuf->buf[0]);
	/* return HDLC buffer to free list unless the free list */
	/* count has exceeded the default value, in which case the */
	/* buffer is freed back to the OS to conserve memory */
	if (n_hdlc->rx_free_buf_list.count > DEFAULT_RX_BUF_COUNT)
		kfree(rbuf);
	else
		n_hdlc_buf_put(&n_hdlc->rx_free_buf_list,rbuf);

	// Wake up paired_port write wait queue
        if (port->paired_port && IS_OPEN(port->paired_port))
                /* wake up sleeping writers */
                wake_up_interruptible(&port->paired_port->write_wait);

	return count;
}
EXPORT_SYMBOL(vs_sdlc_kernel_read);

static size_t vs_sdlc_kernel_write(void *context, const void *buf, size_t count)
{
	struct vl_sync_port *port = (struct vl_sync_port *)context;
        struct vl_sync_port *paired_port = NULL;
	struct n_hdlc *n_hdlc = NULL;
	struct n_hdlc_buf *tbuf;
	//unsigned long flags;

	/* Validate the pointers */
	if(!port || !IS_OPEN(port))
		return -ENODEV;

        paired_port = port->paired_port;
        if (!paired_port || !IS_OPEN(paired_port))
                return -EINVAL;

        // Write into paired_port receive buffer, if available
	n_hdlc = paired_port->n_hdlc;
        if (!n_hdlc)
                return -EIO;
                
	/* Verify frame size */
	if (count > MAX_HDLC_FRAME_SIZE) {
		pr_debug("vs_sdlc_kernel_write: truncating user packet from %lu to %d\n",
				(unsigned long) count, MAX_HDLC_FRAME_SIZE);
		count = MAX_HDLC_FRAME_SIZE;
	}


        // Lock paired_port
	//spin_lock_irqsave(&paired_port->ctrl_lock, flags);

	/* Allocate transmit buffer */
	if (!(tbuf = n_hdlc_buf_get(&n_hdlc->rx_free_buf_list))) {
		//spin_unlock_irqrestore(&paired_port->ctrl_lock, flags);
		return -EAGAIN;
	}

	/* Copy the user's buffer */
	memcpy(tbuf->buf, buf, count);

pr_debug("vs_sdlc_kernel_write: %d bytes: %02x\n", count, tbuf->buf[0]);
	/* Send the data */
	tbuf->count = count;
	n_hdlc_buf_put(&n_hdlc->rx_buf_list,tbuf);

	//spin_unlock_irqrestore(&paired_port->ctrl_lock, flags);

        /* Wake up any blocked reads on paired port and perform async signalling */
        wake_up_interruptible (&paired_port->read_wait);
        if (paired_port->fasync_queue != NULL)
                kill_fasync (&paired_port->fasync_queue, SIGIO, POLL_IN);
        
	return count;

}
EXPORT_SYMBOL(vs_sdlc_kernel_write);

static int vs_sdlc_kernel_ioctl(void *context, int command, unsigned long parameters)
{
	struct vl_sync_port *port = (struct vl_sync_port *)context;
	pr_debug("%s(%d)vs_sdlc_kernel_ioctl() called\n",__FILE__,__LINE__);

	return vl_sync_do_ioctl(port, command, parameters);
}
EXPORT_SYMBOL(vs_sdlc_kernel_ioctl);


static int vl_sync_request_port(struct vl_sync_port *port)
{
	struct n_hdlc *n_hdlc;
	unsigned long flags;
	int ret;

	pr_debug("VL_SYNC[%d]:request port\n", port->line);

	spin_lock_irqsave(&port->ctrl_lock,flags);
	if (IS_OPEN(port)) {
		pr_err("VL_SYNC[%d]:request port: busy", port->line);
		ret = -EBUSY;
		goto err_exit;
	}
	/* Initialize rx/tx frame queues */
	n_hdlc = n_hdlc_alloc();
	if (!n_hdlc) {
		pr_err("VL_SYNC[%d]:n_hdlc_alloc failed\n", port->line);
		ret = -ENFILE;
		goto err_exit;
	}

	port->n_hdlc = n_hdlc;

	/* Set transmit clock parameters */
	vl_sync_set_iface(port, &port->atc_config);

	port->flags |= FLAG_OPEN;
	port->pid = current->pid;

	spin_unlock_irqrestore(&port->ctrl_lock, flags);
	return 0;

err_exit:
	spin_unlock_irqrestore(&port->ctrl_lock, flags);
	return ret;
}

static int vl_sync_open(struct inode *inode, struct file *filp)
{
        struct vl_sync_port *port = container_of(inode->i_cdev, struct vl_sync_port, cdev);
        int retval = 0;
        
        filp->private_data = port;
 	retval = vl_sync_request_port(port);
	if (retval)
		return retval;
       
	return 0;
}

static void vl_sync_release_port(struct vl_sync_port *port)
{
	unsigned long flags;

	pr_debug("VL_SYNC[%d]:release\n", port->line);

	/* Ensure that the process is not hanging on select()/poll() */
	wake_up_interruptible (&port->read_wait);
	wake_up_interruptible (&port->write_wait);

//	if (port->wait_closing)
//		vl_sync_wait_until_send(port);

	n_hdlc_release(port->n_hdlc);
	port->n_hdlc = NULL;
	
	/* clear open flag */
	spin_lock_irqsave(&port->ctrl_lock, flags);
	port->flags &= ~FLAG_OPEN;
	spin_unlock_irqrestore(&port->ctrl_lock, flags);

	return;
}

static int vl_sync_release(struct inode *inode, struct file *filp)
{
	struct vl_sync_port *port = (struct vl_sync_port *)filp->private_data;

	vl_sync_fasync(-1, filp, 0);

	vl_sync_release_port(port);

	return 0;
}

static ssize_t vl_sync_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct vl_sync_port *port = (struct vl_sync_port *)filp->private_data;
	struct n_hdlc *n_hdlc = port->n_hdlc;
	int ret;
	struct n_hdlc_buf *rbuf;

	pr_debug("%s(%d)vl_sync_read() called\n",__FILE__,__LINE__);

	/* Validate the pointers */
	if (!n_hdlc)
		return -EIO;

	/* verify user access to buffer */
	if (!access_ok(VERIFY_WRITE, buf, count)) {
		pr_err("%s(%d) vl_sync_read() can't verify user buffer\n",
                        __FILE__, __LINE__);
		return -EFAULT;
	}

	/*spin_lock_irqsave(&port->read_ctrl_lock, flags);*/

	for (;;) {
		rbuf = n_hdlc_buf_get(&n_hdlc->rx_buf_list);
		if (rbuf)
			break;

		/* no data */
		pr_debug("%s(%d): no data to read\n", __FILE__, __LINE__);
		if (filp->f_flags & O_NONBLOCK) {
			return -EAGAIN;
		}

		interruptible_sleep_on (&port->read_wait);
		if (signal_pending(current)) {
			return -EINTR;
		}
	}

	if (rbuf->count > count)
		/* frame too large for caller's buffer (discard frame) */
		ret = -EOVERFLOW;
	else {
		/* Copy the data to the caller's buffer */
		if (copy_to_user(buf, rbuf->buf, rbuf->count))
			ret = -EFAULT;
		else
			ret = rbuf->count;
	}

	/* return HDLC buffer to free list unless the free list */
	/* count has exceeded the default value, in which case the */
	/* buffer is freed back to the OS to conserve memory */
	if (n_hdlc->rx_free_buf_list.count > DEFAULT_RX_BUF_COUNT)
		kfree(rbuf);
	else
		n_hdlc_buf_put(&n_hdlc->rx_free_buf_list,rbuf);

	// Wake up paired_port write wait queue
        if (port->paired_port && IS_OPEN(port->paired_port))
                /* wake up sleeping writers */
                wake_up_interruptible(&port->paired_port->write_wait);

	return ret;
}

static ssize_t vl_sync_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	struct vl_sync_port *port = (struct vl_sync_port *)filp->private_data;
        struct vl_sync_port *paired_port = port->paired_port;
	register struct n_hdlc *n_hdlc = NULL;
        register struct n_hdlc_buf *tbuf;
	int error = 0;
	DECLARE_WAITQUEUE(wait, current);

	/* Verify pointers */
        if (!port)
		return -ENODEV;

        if (!paired_port || !IS_OPEN(port))
                return -EINVAL;

        n_hdlc = paired_port->n_hdlc;
        
	if ((!n_hdlc) || (n_hdlc->magic != HDLC_MAGIC))
		return -EIO;

        	/* verify frame size */
	if (count > MAX_HDLC_FRAME_SIZE ) {
		pr_err("vl_sync_write: truncating user packet "
				"from %lu to %d\n", (unsigned long) count,
				MAX_HDLC_FRAME_SIZE );
		count = MAX_HDLC_FRAME_SIZE;
	}

	add_wait_queue(&port->write_wait, &wait);
	set_current_state(TASK_INTERRUPTIBLE);

//Write the data buffer into the receive buffer for the paired port

	/* Allocate transmit buffer */
	/* sleep until transmit buffer available */
	while (!(tbuf = n_hdlc_buf_get(&n_hdlc->rx_free_buf_list))) {
		if (filp->f_flags & O_NONBLOCK) {
			error = -EAGAIN;
			break;
		}
		schedule();

		n_hdlc = paired_port->n_hdlc;
		if (!n_hdlc || n_hdlc->magic != HDLC_MAGIC ) {
			pr_err("vl_sync_write: %p invalid after wait!\n", n_hdlc);
			error = -EIO;
			break;
		}

		if (signal_pending(current)) {
			error = -EINTR;
			break;
		}
	}

	set_current_state(TASK_RUNNING);
	remove_wait_queue(&port->write_wait, &wait);

	if (!error) {
		/* Retrieve the user's buffer */
		if (copy_from_user(tbuf->buf, buf, count))
			error = -EFAULT;
		else {
			/* Send the data */
			tbuf->count = error = count;
			n_hdlc_buf_put(&n_hdlc->rx_buf_list,tbuf);
                        /* Wake up any blocked reads on paired port and perform async signalling */
                        wake_up_interruptible (&paired_port->read_wait);
                        if (paired_port->fasync_queue != NULL)
                                kill_fasync (&paired_port->fasync_queue, SIGIO, POLL_IN);
                }
        }
		
	return error;
}


static unsigned int vl_sync_poll(struct file *filp, poll_table *wait)
{
	struct vl_sync_port *port = (struct vl_sync_port *)filp->private_data;
	struct n_hdlc *n_hdlc = port->n_hdlc;
	unsigned int mask = 0;

	if (n_hdlc && n_hdlc->magic == HDLC_MAGIC) {
		/* queue current process into any wait queue that */
		/* may awaken in the future (read and write) */

		poll_wait(filp, &port->read_wait, wait);
		poll_wait(filp, &port->write_wait, wait);

		/* set bits for operations that won't block */
		if (n_hdlc->rx_buf_list.head)
			mask |= POLLIN | POLLRDNORM;	/* readable */
		if (n_hdlc->tx_free_buf_list.head)
			mask |= POLLOUT | POLLWRNORM;	/* writable */
	}
	pr_debug("%s(%d)vl_sync_poll() called, mask=%x\n",__FILE__,__LINE__,mask);
	return mask;
}

static int vl_sync_fasync(int fd, struct file *filp, int on)
{
	struct vl_sync_port *port = (struct vl_sync_port *)filp->private_data;

	return fasync_helper(fd, filp, on, &port->fasync_queue);
}

static int vl_sync_set_iface(struct vl_sync_port *port, atc_spxs_config_t *config)
{
	unsigned int baud = 0; /*protocol, tx_clk_src, tx_clk_mode;*/

	pr_debug("VL_SYNC[%d]:set_iface\n", port->line);

	switch (config->protocol) {
	case ATC_SDLC:
	case ATC_SYNC:
	case ATC_HDLC:
		break;
	default:
		goto err_exit;
	}

	if (config->baud > ATC_B614400)
		goto err_exit;
		
	baud = ATC_B[config->baud];

   	switch (config->transmit_clock_source) {
   	case ATC_CLK_INTERNAL:
		break;
   	case ATC_CLK_EXTERNAL:
		goto err_exit;
   		break;
   	default:
   		goto err_exit;
   	}

   	switch (config->transmit_clock_mode) {
   	case ATC_GATED:
   	case ATC_CONTINUOUS:
   		break;
   	default:
   		goto err_exit;
   	}

   	pr_debug("VL_SYNC[%d]:set_iface: baud=%d\n", port->line, baud);
   	port->atc_config.baud = config->baud;

	return 0;

err_exit:
	return -EINVAL;
}

static int vl_sync_do_ioctl(struct vl_sync_port *port, unsigned int cmd, unsigned long arg)
{
	struct n_hdlc *n_hdlc = port->n_hdlc;
        struct vl_sync_port *paired_port = port->paired_port;
	void *argp = (void *)arg;
	unsigned long flags;
	atc_spxs_config_t tmp_config;
	int count = 0;
	int ret = 0;

	switch (cmd) {
	case ATC_SPXS_READ_CONFIG:
		spin_lock_irqsave(&port->ctrl_lock, flags);
		memcpy(argp,&port->atc_config,sizeof(atc_spxs_config_t));
		spin_unlock_irqrestore(&port->ctrl_lock, flags);
		break;
	case ATC_SPXS_WRITE_CONFIG:
		memcpy(&tmp_config,argp,sizeof(atc_spxs_config_t));
		spin_lock_irqsave(&port->ctrl_lock, flags);
		ret = vl_sync_set_iface(port, &tmp_config);
		spin_unlock_irqrestore(&port->ctrl_lock, flags);
		break;
		
	case FIONREAD:
		/* report count of read data available */
		/* in next available frame (if any) */
		spin_lock_irqsave(&n_hdlc->rx_buf_list.spinlock,flags);
		if (n_hdlc->rx_buf_list.head)
			count = n_hdlc->rx_buf_list.head->count;
		spin_unlock_irqrestore(&n_hdlc->rx_buf_list.spinlock,flags);
		ret = put_user(count, (int __user *)arg);
		break;

	case TIOCOUTQ:
		/* get size of next output frame in queue */
		if (!paired_port || !IS_OPEN(port))
			return -EINVAL;

		n_hdlc = paired_port->n_hdlc;
        
		if ((!n_hdlc) || (n_hdlc->magic != HDLC_MAGIC))
			return -EIO;

		spin_lock_irqsave(&n_hdlc->rx_buf_list.spinlock,flags);
		if (n_hdlc->rx_buf_list.head)
			count = n_hdlc->rx_buf_list.head->count;
		spin_unlock_irqrestore(&n_hdlc->rx_buf_list.spinlock,flags);
		ret = put_user(count, (int __user *)arg);
		break;

	default:
		ret = -ENOIOCTLCMD;
	}

	return ret;
}

static long vl_sync_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct vl_sync_port *port = filp->private_data;

    pr_debug("vl_sync: port[%d]:ioctl:cmd=%0x\n", port->line, cmd&0xffff);

    return vl_sync_do_ioctl(port, cmd, arg);
}


static struct file_operations vl_sync_fops = {
	.owner		= THIS_MODULE,
	.llseek		= no_llseek,
	.open		= vl_sync_open,
	.read		= vl_sync_read,
	.write		= vl_sync_write,
	.poll		= vl_sync_poll,
	.unlocked_ioctl	= vl_sync_ioctl,
	.release	= vl_sync_release,
	.fasync		= vl_sync_fasync,
};
         
static struct class *vl_sync_class;
static char *vl_sync_devnode(struct device *dev, umode_t *mode)
{
	return kasprintf(GFP_KERNEL, "%s", dev_name(dev));
}

static void vl_sync_init_dev(struct vl_sync_port *port, int index)
{
        int retval = 0;

        port->line = index;
	spin_lock_init(&port->ctrl_lock);
	init_waitqueue_head(&port->write_wait);
	init_waitqueue_head(&port->read_wait);

	port->atc_config.protocol = ATC_SDLC;
	port->atc_config.baud = ATC_B614400;
	port->atc_config.transmit_clock_source = ATC_CLK_INTERNAL;
	port->atc_config.transmit_clock_mode = ATC_CONTINUOUS;
        if (index%2)
                port->paired_port = &vlport[index-1];
        else
                port->paired_port = &vlport[index+1];

        cdev_init(&port->cdev, &vl_sync_fops);
        port->cdev.owner = THIS_MODULE;
        port->cdev.ops = &vl_sync_fops;
        retval = cdev_add(&port->cdev, MKDEV(vl_sync_major, VL_SYNC_MINOR + index), 1);
        if (retval)
                pr_err("VL_SYNC: cdev_add #%d failed", index);

	device_create(vl_sync_class, NULL, MKDEV(vl_sync_major,
                                VL_SYNC_MINOR + index), NULL, "vlsync%d", index);
}

static int __init vl_sync_init(void)
{
	int retval = 0;
        int i;
        dev_t dev;
 
	retval = alloc_chrdev_region(&dev, VL_SYNC_MINOR, VL_SYNC_MINORS, "vlsync");
	if (retval) {
		pr_err("VL_SYNC: Cannot alloc char device region");
		return retval;
	}
        vl_sync_major = MAJOR(dev);

	vl_sync_class = class_create(THIS_MODULE, "vlsync");
	if (IS_ERR(vl_sync_class)) {
		printk(KERN_ERR "Error creating vl_sync class.\n");
		retval = PTR_ERR(vl_sync_class);
		return retval;
	}
	vl_sync_class->devnode = vl_sync_devnode;

        for(i=0;i<VL_SYNC_MINORS;i++) {
                vl_sync_init_dev(&vlport[i], i);
        }

	return retval;
}

static void __exit vl_sync_exit(void)
{
        int i;
        
        for(i=0;i<VL_SYNC_MINORS;i++) {
                cdev_del(&vlport[i].cdev);
                device_destroy(vl_sync_class, MKDEV(vl_sync_major, VL_SYNC_MINOR + i));
        }
        class_destroy(vl_sync_class);
        unregister_chrdev_region(MKDEV(vl_sync_major, VL_SYNC_MINOR), VL_SYNC_MINORS);
}

module_init(vl_sync_init);
module_exit(vl_sync_exit);

MODULE_AUTHOR("J. Michael Gallagher, Intelight Inc. <support@intelight-its.com>");
MODULE_DESCRIPTION("Virtual loopback sync serial driver for ATC Standard ports");
MODULE_LICENSE("GPL");
MODULE_ALIAS_CHARDEV(VL_SYNC_MAJOR, VL_SYNC_MINOR);

