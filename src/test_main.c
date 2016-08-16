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
 * \file main.c
 * This file contains the definition of the \c #template
 *
 * \brief template
 *
 * \author Pat Acer
 *  
 * \date   11/01/2010
 *
 * \version 1.0
 *
 *****************************************************************************/

//=============================================================================
/**
 * Includes
 */
#include	<stdio.h>		// STD I/O Definitions
#include	<unistd.h>		// STD Unix Definitions
#include	<stdint.h>		// STD IEEE Type Definitions
#include	<stdlib.h>		// STD Library Definitions
#include	<string.h>		// STD String Definitions
#include   	<errno.h>		// STD errno Definitions
#include    <fcntl.h>
#include    <termios.h>     // terminal IO & serial port stuff

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */
#define BAUDRATE B9600


//=============================================================================
/**
 * \brief template Description
 *
 * \param[in]	Description
 * \param[out]	Description
 * \return		Description
 */

#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 
     

main()
{
int fd = -5, c, res;
struct termios oldtio, newtio;
char buf[1024];
char port_name[64] = "/dev/tty                                                ";
char *tty_names[] = {
		   "S0", "S1", "S2", "S3",   "",
		    "0",  "1",  "2",  "3",  "4",  "5",  "6",  "7",  "8",  "9",
		   "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
		   "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
		   "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
		   "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
		   "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
		   "60", "61", "62", "63"};
char *test_results[sizeof(tty_names)/sizeof( char *)] = { "  ", "    ", "   "};
char good_write[] = "                                 good write";
char fail_open[] = "fail open";
char bad_write[] = "bad write";
char fail_close[] = "fail_close";

		
int i=0, j=0, k=0;
const char sleep_cmd[] = "sleep 1;";;
int read_counter = 0;
int read_failures = 0; 

	   printf("\r\n Some strings: \nport_name: %s \ntty_names: %s , %s , %s \n", 
			   port_name, tty_names[23], tty_names [37], tty_names[49] );
	   
	   printf("\r\n sizeof(tty_names) = %d", sizeof(tty_names)/sizeof(char *)  );
	   
       // pause(); 

      for( i = 0; i < (sizeof(tty_names)/sizeof( char *)) ; i++)
      {
		  // i = 3;    //  try COM3 = ttyS3 to start 
		  sprintf(port_name, "/dev/tty%s",tty_names[i]);
		  printf ("\r\n: ------------------ ");
		  printf ("\r\n %02d: MSG01: Trying out serial port # %s --------:", i+1, port_name);
	
		  
		  fd = open(port_name , O_RDWR);     //  | O_NOCTTY);
		  
		  printf("\r\nMSG02: Open attempt on serial port # %s returned a fd = %d", port_name, fd);
		  //printf("\n------------------------------------------\n");
		   
		  if( fd >= 0) {
			  write (fd, "\n crazy  like a fox \n", 20);
			  break;
		  }
		  
		  system( sleep_cmd );   // for loop testing of all serial ports 
	  
      }
      
	  if( fd < 0 ) 
		  {perror(port_name); test_results[i] = fail_open; }  // exit(3); }  // QUIT if we can't open the port
	  else
	  {
		  printf("\r\n MSG03: -  starting with a write to the serial port ------ ");
		  if( (res = write(fd, "MSG04: Test 0 is starting     \n\n", 32)) < 0 )  // send msg out to the other serial port 
		 {
			 printf("\r\nMSG05a: Initial Write failed to distant serial port with code: %d ", res);
			 test_results[i] = bad_write;
			 close(fd);
			 exit(4);
		 }
			 
		  printf(" res = %d for port %s \n", res, port_name);
		  if( (res = write(fd, port_name, strlen(port_name)) < 0 ) ) { // );   // send over the port name 
				 printf("\r\nMSG05b: Second Write failed to distant serial port with code: %d ", res);
				 test_results[i] = bad_write;
				 close(fd);
				 exit(5);
			  
		  }
		  
		  tcgetattr( fd, &oldtio);  // save old port settings
		  bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
		  // memcpy( &newtio, &oldtio, sizeof(oldtio));     // initialize with old port setttings
		 
		  /* 
			BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
			CRTSCTS : CTS and RTS output hardware flow control (only used if the cable has
					  all necessary lines. See sect. 7 of Serial-HOWTO)
			CS8     : 8n1 (8bit,no parity,1 stopbit)
			CLOCAL  : local connection, no modem contol
			CREAD   : enable receiving characters
		  */
	
		  newtio.c_cflag = BAUDRATE | CS8 | CRTSCTS | CLOCAL | CREAD;   // 
		  
		  newtio.c_iflag = IGNPAR  | ICRNL;
		  
		  newtio.c_oflag = 0; 
		  
		  newtio.c_lflag = ICANON;   // canonical - line at a time - waits for <CR> or num chars requested
		  
		  newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
		  newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
		  newtio.c_cc[VERASE]   = 0;     /* del */
		  newtio.c_cc[VKILL]    = 0;     /* @ */
		  newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
		  newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
		  newtio.c_cc[VMIN]     = 80;     /* blocking read until 80 characters arrive */
		  newtio.c_cc[VSWTC]    = 0;     /* '\0' */
		  newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
		  newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
		  newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
		  newtio.c_cc[VEOL]     = 0;     /* '\0' */
		  newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
		  newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
		  newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
		  newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
		  newtio.c_cc[VEOL2]    = 0;     /* '\0' */
		 
		 /* 
		   now clean the modem line and activate the settings for the port
		 */
		  tcflush(fd, TCIFLUSH);
		  tcsetattr(fd,TCSANOW,&newtio);
		 
		 /*
		   terminal settings done, now handle input
		   In this example, inputting a 'z' at the beginning of a line will 
		   exit the program.
		 */ 
		 printf("\r\n %2d: MSG06: --   beginning reads & writes ----- ", i+1);
		 if( (res = write(fd, "\n\rTest 1 is starting", 22)) < 0 )  // send msg out to the other serial port 
		 {
			 printf("\r\nMSG07: Third Write failed to distant serial port with code: %d", res);
			 test_results[i] = bad_write;
			 close(fd);
			 exit(7);
		 }
		 printf("res = %d \n", res);
		 test_results[i] = good_write;
		 
		 while (STOP==FALSE) {     /* loop until we have a terminating condition */
		  /* read blocks program execution until a line terminating character is 
			 input, even if more than 255 chars are input. If the number
			 of characters read is smaller than the number of chars available,
			 subsequent reads will return the remaining chars. res will be set
			 to the actual number of characters actually read */
		
		 
		 
			 read_counter++; 
			 printf("\r\n MSG08: reading character string # %d", read_counter);
			 
			 if( (res = read(fd,buf,80 )) < 0 )
			 {   //   read failure  
				 read_failures++;
				 printf("\n MSG09: Read %d failed:  %d \n", read_counter, res); 
				 if( (read_failures > 50) )
					 exit(-5);   // quit if just not working
				 continue;   // try again from the top 
			 } 
			 buf[res]=0;             // set end of string, so we can printf 
			 
			 // printf ("\r\n %d MSG10: read %d characters in the string:", read_counter, res);
			 printf("\r\n%3d:%3d:[%s]", read_counter, res, buf);
			 // now echo back to the port with encased in brackets [] 
			 write (fd, "[", 1);
			 write( fd, buf, res);
			 write(fd, "]", 1);
			 
			 if (buf[0]=='z') STOP=TRUE;
					 
		  }   // end - while loop for reading chars
		 
		  
		 
		  if( (res = close (fd)) < 0 ) {
			  test_results[i] = fail_close;
		  }
		 
		  /* restore the old port settings */
		  tcsetattr(fd,TCSANOW,&oldtio);
		  
		  
	  }  // end - else - good open 
	  
    //  }  // end - for loop 
 
  /******************
   
  printf("\r\n\n =========== RESULTS =======================\r\n\n");
	  
  for( i = 0; i < (sizeof(tty_names)/sizeof( char *)) ; i++)
  {
	  // i = 3;    //  try COM3 to start 
	  sprintf(port_name, "/dev/tty%s",tty_names[i]);
	  printf("\r\n %d: %s: %s ", i+1, port_name, test_results[i]);
  }

  ************************************************************/
	  
	  exit(0);
	  
}
