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
/*****************************************************************************
 * \file vt100_serialPortComms.c
 * This file contains the routines to handle comms over the serial port
 *  and also a routine to locate the working serial port name /dev/tty??
 *
 * \brief template
 *
 * \author Patrick Acer
 *
 * \version 1.0
 *
 *****************************************************************************/

//=============================================================================
/**
 * Includes
 */
#include <fcntl.h>       // STD file control defs -ORDWR bits, etc.
#include <termios.h>     // terminal IO & serial port stuff
#include "vt100.h"       // ITE project include 

//=============================================================================
/**
 * Extern
 */
// extern int open(const char *file, int oflag);
extern int open (__const char *__file, int __oflag, ...) __nonnull ((1));
extern int16_t read();
extern int16_t write();
extern int16_t close();

extern void usleep();


//=============================================================================
/**
 * Local Defines
 */

//=============================================================================
/**
 * \brief       int16_t openSerialPort(portName)
 *
 * \param[in]	portName - something like "/dev/ttyS0"
 * 
 * \param[out]	none
 * 
 * \return		file descriptor number of the opened serial port if good
 *              if error, returns a negative number indicating the problem
 * 
 */
int16_t
openSerialPort(const char *portName)
{
int16_t fd = 0;

	fd = (int16_t) open(portName , O_RDWR ) ; //     | O_NOCTTY);
	
	return(fd);
	
	
}

//=============================================================================
/**
 * \brief       char readSerialPort(fd )
 *
 * \param[in]	int16_t fd - file descriptor number returned from open
 * 
 * \param[out]	none 
 * 
 * \return		character read if okay or zero if EOF or error
 * 
 */
char
readSerialPort(int16_t fd)  
{
int16_t res = 0;
char buf[20];    // leave a little extra space for future - maybe <Esc> sequences

	// need non-blocking/poll read here - to release CPU control 

	if ((res = read(fd,buf,1)) < 0)
		buf[0] = '\0';    //  indicate EOF or error 
	
	return(buf[0]);
	
	
}
//=============================================================================
/**
 * \brief       int16_t writeSerialPort(fd, buffer, count )
 *
 * \param[in]	int16_t fd       - file descriptor number returned from open
 *              char *buffer     - character string buffer
 *              int16_t count    - number of characters to be written
 * 
 * \param[out]	none
 * 
 * \return		count of characters written or < 0 if error
 * 
 */
int16_t
writeSerialPort(int16_t fd, char *buffer, int16_t count)
{
int16_t res = 0;

	if ((res = write(fd, buffer, count)) < 0)
		;    //  indicate EOF or error 
	
#if DEBUG_ON & DEBUG_SERIAL 
int16_t i = 0;
	printf("\n Writing %d bytes to Serial Port fd # %d:\n", count, fd );
	for( i= 0; i < count; i++ ){
		printf("%c [0x%02x]", buffer[i], buffer[i]);
	}
#endif
	
	return(res);
	
	
}

//=============================================================================
/**
 * \brief       int16_t closeSerialPort(int16_t fd)
 *
 * \param[in]	fd - file descriptor number returned when it was opened
 * 
 * \param[out]	none
 * 
 * \return		0 if okay;  < 0 if error 
 * 
 */
int16_t
closeSerialPort(int16_t fd)
{
int16_t res = 0;

	res = close(fd);
	
	
	return(res);
	
	
}

//=============================================================================
/**
 * \brief       uint16_t vt100_getPortFd() - locate a serial port to connect to
 *                                            of the form "/dev/ttyS3" 
 * 
 * \param[in]	none
 * 
 * \param[out]	none  
 * 
 * \return		file descriptor for opened port,  <= 0 if none or error 
 * 
 */

int16_t
vt100_getPortFd(char *returnPortName)
{
	int fd = -5;
	// struct termios oldtio, newtio;
	// char buf[1024];
	static char portName[64] = "/dev/tty                                                ";
	static char *tty_names[] = {
			   "S0", "S1", "S2", "S3",   "",
			    "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",
			   "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
			   "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
			   "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
			   "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
			   "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
			   "60", "61", "62", "63"};
			
	int i=0;

		   
		   printf("\r\n Number of potential ports to scan [sizeof(tty_names)] = %d", 
				      sizeof(tty_names)/sizeof(char *)  );
		   
	       // pause(); 

	      for( i = 0; i < (sizeof(tty_names)/sizeof( char *)) ; i++)
	      {
			  // i = 3;    //  try COM3 = ttyS3 to start 
			  sprintf(portName, "/dev/tty%s",tty_names[i]);
			  
			  printf("\n trying port name: %s ", portName );
			  
			  
			  fd = open(portName , O_RDWR);     //  | O_NOCTTY);
			   
			  if( fd >= 0) {    //  make a quick test write out to the port if okay 
				  printf("\n Opened serial port %s with fd = %d", portName, fd);
				  write (fd, "\n Test write to serial \n", 24);
				  close( fd );
				  break;
			  }
			 
			  usleep(1);   // 100 millisecs delay 
		  
	      }  // end for loop - searching for ports in all the wrong places!
	    
	      if ( fd <= 0 ) {
				  printf("\n could not open a serial port \n");
	      }
	      
	returnPortName = portName;
	
	return(fd);   // returns good fd or < 0 for a problem
	
	
}   //  end - vt100_getPortFd() 
