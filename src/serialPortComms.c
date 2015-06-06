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
 * \file serialPortComms.c
 * This file contains the routines to handle comms over the serial port
 *
 * � Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
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

//=============================================================================
/**
 * \brief       int openSerialPort(portName)
 *
 * \param[in]	portName - something like "/dev/ttyS0"
 * 
 * \param[out]	Description
 * 
 * \return		file descriptor number of the opened serial port if good
 *              if error, returns a negative number indicating the problem
 * 
 */
int
openSerialPort(char *portName)
{
int fd = 0;

	fd = open(portName , O_RDWR ) ; //     | O_NOCTTY);
	
	return(fd);
	
	
}

//=============================================================================
/**
 * \brief       int readSerialPort(fd )
 *
 * \param[in]	int fd - file descriptor number returned from open
 * 
 * \param[out]	Description
 * 
 * \return		character read if okay or zero if EOF or error
 * 
 */
char
readSerialPort(int fd)
{
int res = 0;
char buf[20];    // leave a little extra space for future - maybe <Esc> sequences

	if ((res = read(fd,buf,1)) < 0)
		buf[0] = 0;    //  indicate EOF or error 
	
	return(buf[0]);
	
	
}
//=============================================================================
/**
 * \brief       int writeSerialPort(fd, buffer, count )
 *
 * \param[in]	int fd       - file descriptor number returned from open
 *              char *buffer - character string buffer
 *              int count    - number of characters to be written
 * 
 * \param[out]	Description
 * 
 * \return		count of characters written or < 0 if error
 * 
 */
char
writeSerialPort(int fd, char *buffer, int count)
{
int res = 0;

	if ((res = write(fd, buffer, count)) < 0)
		;    //  indicate EOF or error 
	
	return(res);
	
	
}

//=============================================================================
/**
 * \brief       int closeSerialPort(int fd)
 *
 * \param[in]	fd - file descriptor number returned when it was opened
 * 
 * \param[out]	Description
 * 
 * \return		0 if okay;  < 0 if error 
 * 
 */
int
closeSerialPort(int fd)
{
int res = 0;

	res = close(fd);
	
	
	return(res);
	
	
}
