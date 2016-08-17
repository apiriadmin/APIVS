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
 *
 * \brief This file contains a main function used to drive the FIO emulator library.
 *
 * \date   12/22/2010
 *
 * \author Steve Kapp
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

#include    "emfio.h"

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
 * \brief The application entry point.
 *
 * \param[in] argc The number of command line arguments.
 * \param[in] argv A pointer to an array of command line arguments.
 * \return Always exits with a code of \c 0.
 */
int
main (int argc, char *argv[])
{
	//  arg0 is the actual command line that invoked this program!
	char *fileName = argv[1];
	int16_t result;
	int16_t fd = 0;
	int16_t autoIncrement = 0;
		// size of Front Panel Display - must be int variables for scanf() function
	// char *portName = "/dev/tty";  // use keyboard tty input for serial I/O
	char *portName = NULL;
	
	printf("\n\n  Test Framework for emfio emulator library ");
	printf("\n        (command syntax:  './emfio filename')\n");
	
	result = emfio_start(NULL);
	if( result != 0 ) printf("\n  ... returned error code = %d ", result);
	
	emfio_end();
	
	sleep (1);      // give time for last calls to output to screen?
	
	exit(0);
}  // end - main () 
