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
 * \file atcmain.c
 * This file contains the test framework for the vt100 emulator library
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
#include    <fcntl.h>       // STD file control defs 
#include    <termios.h>     // terminal IO & serial port stuff

#include    "vt100.h"       // for libvt100 functions
#include    "fpui.h"        // for libfpui functions 

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
 * \brief       int16_t main() 
 *
 * \param[in]	pFileName - something like "dump_01.txt"
 * 
 * \param[out]	Description
 * 
 * \return		test framework main() routine 
 *              
 * 
 */
int
main (int argc, char *argv[])
{
	//  arg0 is the actual command line that invoked this program!
char *fileName = argv[1];
int16_t result = 0;
int16_t fd = 0; 
int     fpui_fd = 0;
char *portName = "/dev/tty";  // use keyboard tty input for serial I/O

	printf("\n\n  FPUI & VT100 Test Framework for vt100 emulator library ");
	printf("\n        (command syntax:  './vt filename')\n");
	
	  // get the working port name and fd 
	fd = vt100_getPortFd(portName);
	// portName = "/dev/ttyS3";
	
	fpui_fd = fpui_open("dev/fpi", O_RDWR, (const char *) "User 27" ); 
	if( fpui_fd <= 0 ) {
		printf("\n\natcmain: fpui_open returned error = %d \n", fpui_fd);
		exit( 1001 );
	}
	else
		printf("\natcmain: fpui_open() okay;  fd = %d ]\n", fpui_fd);
	
		
	
	   // Initial setup of arrays in memory 
	if( (strcmp(fileName,"") == 0) || (strcmp(fileName, " ") == 0) ) 
		fileName = "vtDump";   // default base output file name
	 
	result = vt100_dumpVD(fileName, "Initial State");   // show original data array 
	printf("\n\n Result of vt100_dumpVT = %d \n", result);
	vt100_showVD(stdout, "Initial State");
	
		  // do compare on intial state 
	result = vt100_compareVD("display.start");
	printf("\n 1st compareVD: display.start = %d \n", result);
		
	  //  call fpui_clear() to clear the display 
	fpui_clear( fpui_fd );
	vt100_showVD(stdout, "atcmain: After fpui_clear() ");
	
	vt100_clearVD();        // start clean & clear before text input 
	result = vt100_compareVD("display.clear");
	printf("\natcmain: compareVD: after display.clear = %d \n", result);
	vt100_showVD(stdout, "atcmain: After call to vt100_clearVD");

	
	  //  vt100_start() and vt100_end()
	printf("\n - calling vt100_start() for port %s:\n", portName);
	result = vt100_start(portName);   // modify array 
	if( result != 0 ) printf("\n  ... returned error code = %d ", result);
	vt100_end();
	
	sleep(1);
	
	strcat(fileName,  ".after");
	printf("\n new fileName is %s", fileName);
	
	printf("\nvt100_dump 2 (after start) to  %s\n", fileName);
	
	result = vt100_dumpVD(fileName, "After vt100_start()");
	printf("\n\n Result of vt100_dumpVT = %d \n", result);
	vt100_showVD(stdout, "After vt100_start()");
	
	/*************************
	 usleep(3);  // 300 msecs ?

	printf("\n\n\n\n  SYSTEM CALL to show process ID:");
	system( "ps -e | grep vt");
	printf("\n--------------------------------\n\n");
	***********************************/	
	
	
	if( (result = (int16_t) fpui_close( fpui_fd ) ) != 0 )
		printf("\natcmain: fpui_close() error # %d \n", result);
	
	sleep (1);      // give time for last co

	
	exit(0);
	
	
}  // end - main () 


