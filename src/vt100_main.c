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
 * This file contains the test framework for the vt100 emulator library
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

#include    "vt100.h"

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */

#define BUFFER_LEN ( (DEFAULT_DISPLAY_COLS * 5) * 2 )     // make same as in vt100_noFPUI.c
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
static char returnBuffer[BUFFER_LEN] = {'\0','\0','\0','\0','\0','\0','\0' };  
int16_t result;
int16_t fd = 0; 
int16_t autoIncrement = 0;
    // size of Front Panel Display - must be int variables for scanf() function
int frontPanelRows = 0;       
int frontPanelCols = 0;
// char *portName = "/dev/tty";  // use keyboard tty input for serial I/O
char *portName = NULL;

	printf("\n\n  Test Framework for vt100 emulator library ");
	printf("\n        (command syntax:  './vt filename')\n");
	
	  // get the working port name and fd 
	fd = vt100_getPortFd(portName);
	// portName = "/dev/ttyS3";
	
	   // Initial setup of arrays in memory 
	if( strcmp(fileName, "") == 0)  
			fileName = "disp_dumpVD.txt";
	
	printf("\n main: fileName is %s, portName is %s, port fd is %d \n", 
			fileName, portName, fd);
	 
	//  Due to Dynamic Memory Allocation - can not call any routines until vt100_start() is called 
	// result = vt100_dumpVD(fileName, 0, "Initial State");   // show original data array - with no number extenison
	// printf("\n\n Result of vt100_dumpVT = %d \n", result);
	// vt100_showVD(stdout, "Initial State");
	// do compare on intial state 
	// result = vt100_compareVD("display.8.40.start");
	// printf("\n Default compareVD: display.8.40.start = %d \n", result);
	
	printf("\n\n Number of rows in the Front Panel Display: ");
	scanf("%d", &frontPanelRows );
	printf("\n Number of columns in the Front Panel Display: ");
	scanf("%d", &frontPanelCols );
		
	  //  vt100_start() 

	printf("\n\n=================  vt100_start() =============\n");
	
	printf("\n - calling vt100_start() for port %s: with %d rows and %d columns\n", portName,
			     frontPanelRows, frontPanelCols);
	
    //	result = vt100_start(portName);   // modify array 
	result = vt100_start(NULL , frontPanelRows, frontPanelCols);   // modify array 
	if( result != 0 ) printf("\n  ... returned error code = %d ", result);
	printf("\n Initial states of errorCode: %d and errorText  %s ", 
			     vt100_get_errorCode(), vt100_get_errorText() );
	
	vt100_dumpVD("dump.vt100_start", ++autoIncrement, "after vt100_start()"); 
	vt100_showVD(stdout, "after vt100_start()");
	
	
	vt100_clearVD();     // initialize or clear the display rows & columns from malloc garbage
	printf("\n\n=========  vt100_clear - compare clear with display.clear =============\n");
	vt100_dumpVD("dump.after_1st_clear", ++autoIncrement, "after 1st clear"); 
	vt100_showVD(stdout, "after 1st vt100_clear(0 after the start()");
	result = vt100_compareVD("display.clear");
	printf("\n\n Result of vt100_compareVD = %d, for file: %s \n", result, 
			"display.clear");
	if( result != 0 )
		printf("\n  errorText = %s", vt100_get_errorText() );

	printf("\n\n=================  vt100_set_auxSwitch() ON ===========================\n");
	
	result = vt100_set_auxSwitch( 1 );	
	vt100_dumpVD("auxSwitch.out", ++autoIncrement, "set_auxSwitch ON");     
	printf("\n\n Result vt100_set_auxSwitch = %d \n", result);
	vt100_showVD(stdout, "fileToVD");
	sleep(1);

	printf("\n\n=================  vt100_set_auxSwitch() OFF ==========================\n");
	
	result = vt100_set_auxSwitch( 0 );	
	vt100_dumpVD("auxSwitch.out", ++autoIncrement, "set_auxSwitch OFF");     
	printf("\n\n Result vt100_set_auxSwitch = %d \n", result);
	vt100_showVD(stdout, "fileToVD");
	sleep(1);
	
	printf("\n\n=================  vt100_fileToVD - short file '1234.in'  =============\n");
	
	result = vt100_fileToVD("1234.in");
	
	vt100_dumpVD("1234.out", ++autoIncrement, "fileToVD");     
	printf("\n\n Result vt100_fileToVD = %d \n", result);
	vt100_showVD(stdout, "fileToVD");
	sleep(1);
	printf("\n ------------- return buffer: ------------");
	vt100_vtInputBuffer(returnBuffer, sizeof (returnBuffer));
	printf("\n returnBuffer starts on next line: \n%s\n-------------------\n", returnBuffer);
	sleep(2);

	printf("\n\n=================  Command/Response sequences  ========\n");
	
	result = vt100_fileToVD("commands.in");
	sleep(1);
	vt100_dumpVD("cmdResponses.out", ++autoIncrement, "CMD Responses");     
	printf("\n\n Result of cmdResponses using vt100_fileToVD = %d \n", result);
	vt100_showVD(stdout, "CMD_Repsonses ");
	sleep(1);
	
	printf("\n\n=================  vt100_stringToVD  # 1 - very short =============\n");
	
	vt100_stringToVD("1234");
	
	result = vt100_dumpVD("displayString.txt", ++autoIncrement, "short stringToVD");     
	printf("\n\n Result of vt100_dumpVD = %d \n", result);
	vt100_showVD(stdout, "short vt100_stringToVD");

	
	printf("\n\n=================  vt100_stringToVD  # 2 - long multi-line  ========\n");
	
	vt100_stringToVD("This is a test \n#9999: of the emergency broadcasting system 0123456789ABC\n\n\n\tTAKE 2 \n\n\tThe END\t17abc\t25def\t33ghi\n\t\t\t7777");
	
	result = vt100_dumpVD("displayString.txt", ++autoIncrement, "long stringToVD");     
	printf("\n\n Result of vt100_dumpVD = %d \n", result);
	vt100_showVD(stdout, "long vt100_stringToVD");
    sleep(2);

	
	printf("\n\n============  Special Chars create (vt100_fileToVD) =============\n");
	
	vt100_clearVD();     // initialize or clear the display rows & columns 
	vt100_resetParms();  // restore all PARMs etc before doing Special Chars testing
	
	result = vt100_fileToVD("specialChars.in");
	
	vt100_dumpVD("specialChars.out", ++autoIncrement, "Special Chars");     
	printf("\n\n Result of Special Chars vt100_fileToVD = %d \n", result);
	vt100_showVD(stdout, "Special Chars Compose ");
	sleep(1);
	
	printf("\n\n=========== Special Chars compare  - display.special OK ============\n");

	result = vt100_compareVD("display.special");
	printf("\n\n Result of vt100_compareVD = %d, for file: %s \n", result, 
			"display.special");

	if( result != 0 )
		printf("\n  errorText = %s ", vt100_get_errorText() );

	printf("\n\n======= Special Chars compare  - display.special.bad - FAIL ========\n");

	result = vt100_compareVD("display.special.bad");
	printf("\n\n Result of vt100_compareVD = %d, for file: %s \n", result, 
			"display.special.bad");

	if( result != 0 )
		printf("\n  errorText = %s ", vt100_get_errorText() );

	printf("\n\n=============  vt100_fileToVD - junk screen =============\n");
	
	result = vt100_fileToVD("testFileToVD.in");
	
	vt100_dumpVD("testFileToVD.out", ++autoIncrement, "fileToVD");     
	printf("\n\n Result vt100_fileToVD = %d \n", result);
	vt100_showVD(stdout, "fileToVD");
	printf("\n - compare the testFileToVD.in with the display.testIput ");
	result = vt100_compareVD("display.testInput");
	printf("\n\n Result  = %d, for file: %s \n", result, "display.testIput");
	sleep(1);
	printf("\n ------------- return buffer: ------------");
	vt100_vtInputBuffer(returnBuffer, sizeof (returnBuffer));
	printf("\n returnBuffer starts on next line: \n%s\n-------------------\n", returnBuffer);
	sleep(1);

	printf("\n\n=============  Command/Response sequences  # 2    =======\n");
	
	result = vt100_fileToVD("commands.in");
	sleep(1);
	vt100_dumpVD("cmdResponses.out", ++autoIncrement, "CMD Responses");     
	printf("\n\n Result of cmdResponses using vt100_fileToVD = %d \n", result);
	vt100_showVD(stdout, "CMD_Repsonses ");
	sleep(1);

	
	printf("\n\n=================  vt100_fromDumptoVD - display.fpm =============\n");
	
	vt100_resetParms();      // restore to power up condition
	result = vt100_fromDumpToVD("display.fpm");
	
	vt100_dumpVD("test_fromDumpToVD.out", ++autoIncrement, "fromDumpToVD");     
	printf("\n\n Result vt100_fromDumpToVD = %d \n", result);
	vt100_showVD(stdout, "fromDumpToVD");
	
	printf("\n\n=================  vt100_compareVD - display.fpm =============\n");

	result = vt100_compareVD("display.fpm");
	printf("\n\n Result of vt100_compareVD = %d, for file: %s \n", result, 
			"display.fpm");

	if( result != 0 )
		printf("\n  errorText = %s ", vt100_get_errorText() );

/*************************	don't need to test this any more .....	
	printf("\n\n=================  vt100_fromDumptoVD - bad file =======\n");
	
	result = vt100_fromDumpToVD("display.fail");
	
	vt100_dumpVD("test_fromDumpToVD.out", ++autoIncrement, "fromDumpToVD_FAIL");     
	printf("\n\n Result vt100_fromDumpToVD = %d \n", result);
	vt100_showVD(stdout, "fromDumpToVD_FAIL");
****************************************************/

	printf("\n\n=================  vt100_fromDumptoVD - display.config =============\n");
	
	// vt100_resetParms();      // restore to power up condition
	result = vt100_fromDumpToVD("display.config");

	vt100_dumpVD("disp_dumpVD.tst", ++autoIncrement, "fromDumpToVD - config");     
	printf("\n\n Result vt100_fromDumpToVD = %d \n", result);
	// printf("\n errorCode: %d, errorText: %s ", vt100_get_errorCode(),vt100_get_errorText() );
	vt100_showVD(stdout, "fromDumpToVD - config");

	
	printf("\n\n=================  vt100_compareVD - display.config =============\n");

	result = vt100_compareVD("display.config");
	printf("\n\n Result of vt100_compareVD = %d, for file: %s \n", result, 
			"display.config");

	if( result != 0 )
		printf("\n  errorText = %s ", vt100_get_errorText() );


	printf("\n\n=================  vt100_scrollVD - UP  =======\n");
	
	vt100_scrollVD(1); 
	
	vt100_dumpVD("test_scrollVD_UP.out", ++autoIncrement, "scrollVD-UP!");     
	vt100_showVD(stdout, "scrollVD-UP!");

	printf("\n\n=================  vt100_scrollVD - DOWN  =======\n");
	
	vt100_scrollVD(-1); 
	
	vt100_dumpVD("test_scrollVD_DOWN.out", ++autoIncrement, "scrollVD-DOWN!");     
	vt100_showVD(stdout, "scrollVD-DOWN!");
		
	/* ****************** don't need this test anymore ************
	printf("\n\n=================  vt100_scrollVD - Bad arg =======\n");
	
	vt100_scrollVD(5); 
	
	vt100_dumpVD("test_scrollVD_badArg.out", ++autoIncrement, "scrollVD-badArg!");     
	vt100_showVD(stdout, "scrollVD-badArg!");
	*********************************************/
		
	/*************************
	 usleep(3);  // 300 msecs ?

	printf("\n\n\n\n  SYSTEM CALL to show process ID:");
	system( "ps -e | grep vt");
	printf("\n--------------------------------\n\n");
	***********************************/	
	
	vt100_end();
	
	
	sleep (1);      // give time for last calls to output to screen?

	
	exit(0);
	
	
}  // end - main () 


