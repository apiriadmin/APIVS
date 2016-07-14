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
 * \file vt100_globalData.c
 * 
 * This file contains the global Data used by the vt100 routines
 *
 * � Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief 
 * 
 * \date   11/01/2010 
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
#include "vt100.h"

//=============================================================================
/**
 * Extern
 */


//=============================================================================
/**
 * Local Defines - see include file vt100.h
 */
 

//=============================================================================
/**
 * Global Data Variables 
 */


/*********************                   // 12/19/2010 - replaced by dynamic malloc()'d array for variable sizes
VIRTUAL_DISPLAY vDisplay =               // this is the local DISPLAY image used by everything
{   
  {
	 {'1', '1', '1', '1', '1', '1', '1', '1', '1', '1',       '1', '1', '1', '1', '1', '1', '1', '1', '1', '1',
	  '1', '1', '1', '1', '1', '1', '1', '1', '1', '1',       '1', '1', '1', '1', '1', '1', '1', '1', '1', '1'},
		   
	 {'2', '2', '2', '2', '2', '2', '2', '2', '2', '2',       '2', '2', '2', '2', '2', '2', '2', '2', '2', '2',
      '2', '2', '2', '2', '2', '2', '2', '2', '2', '2',       '2', '2', '2', '2', '2', '2', '2', '2', '2', '2'},
      
     {'3', '3', '3', '3', '3', '3', '3', '3', '3', '3',       '3', '3', '3', '3', '3', '3', '3', '3', '3', '3',
      '3', '3', '3', '3', '3', '3', '3', '3', '3', '3',       '3', '3', '3', '3', '3', '3', '3', '3', '3', '3'},
      
     {'4', '4', '4', '4', '4', '4', '4', '4', '4', '4',       '4', '4', '4', '4', '4', '4', '4', '4', '4', '4',
      '4', '4', '4', '4', '4', '4', '4', '4', '4', '4',       '4', '4', '4', '4', '4', '4', '4', '4', '4', '4'},
   
     {'5', '5', '5', '5', '5', '5', '5', '5', '5', '5',       '5', '5', '5', '5', '5', '5', '5', '5', '5', '5',
      '5', '5', '5', '5', '5', '5', '5', '5', '5', '5',       '5', '5', '5', '5', '5', '5', '5', '5', '5', '5'},
   
     {'6', '6', '6', '6', '6', '6', '6', '6', '6', '6',       '6', '6', '6', '6', '6', '6', '6', '6', '6', '6',
      '6', '6', '6', '6', '6', '6', '6', '6', '6', '6',       '6', '6', '6', '6', '6', '6', '6', '6', '6', '6'},
      
     {'7', '7', '7', '7', '7', '7', '7', '7', '7', '7',       '7', '7', '7', '7', '7', '7', '7', '7', '7', '7',
      '7', '7', '7', '7', '7', '7', '7', '7', '7', '7',       '7', '7', '7', '7', '7', '7', '7', '7', '7', '7'},
      
     {'8', '8', '8', '8', '8', '8', '8', '8', '8', '8',       '8', '8', '8', '8', '8', '8', '8', '8', '8', '8',
      '8', '8', '8', '8', '8', '8', '8', '8', '8', '8',       '8', '8', '8', '8', '8', '8', '8', '8', '8', '8'}
  },
  {
	 {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
      '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
      
  	 {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
      '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},

     {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
      '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
  
     {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
      '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
   
 	 {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
      '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
       
   	 {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
      '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},

     {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
      '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'},
   
     {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', 
      '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',       '0', '0', '0', '0', '0', '0', '0', '0', '0', '0'}

  }
   
};
********************************/
VIRTUAL_DISPLAY vDisplay = {
		NULL,        //   *pText[]
		NULL,        //   *pAttribs[]
		NULL,        //   *pTextBuf[]
		NULL,        //   *pAttribsBuf[]
		0, 	         //    ptrsBufSize;    
		0,            //    charsBufSize;
};

VIRTUAL_DISPLAY *pVD = &vDisplay;       // intialize with default display - will be replaced by malloc'd array 


int16_t specialCharColumns[NUMBER_OF_SPEC_CHARS] =  // the number of Pn values in the "Compose" string
      {0, 0, 0, 0, 0, 0, 0, 0};                     // a zero means it has not yetbeen defined
                                                            
uint8_t specialChars[NUMBER_OF_SPEC_CHARS][BYTES_PER_SPEC_CHAR] =   // 8 chars(rows) - each 8 bytes
{     { 0,0,0,0, 0,0,0,0}, {0,0,0,0, 0,0,0,0}, {0,0,0,0, 0,0,0,0}, {0,0,0,0,0,0,0,0},
      { 0,0,0,0, 0,0,0,0}, {0,0,0,0, 0,0,0,0}, {0,0,0,0, 0,0,0,0}, {0,0,0,0,0,0,0,0}
};

uint8_t graphicModeFlags = 0;  

int16_t numRows = DEFAULT_DISPLAY_ROWS;
int16_t numCols = DEFAULT_DISPLAY_COLS;

char   *fileNamePtr = "testing.txt";

int16_t fdSerialPort = 0;            // file descriptor from opening the serial port 


      // allow for at least 120 cols MAX width for now - make dynamic later
char tabStops[DEFAULT_DISPLAY_COLS * 3] = {    
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',       ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',       ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',

	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',       ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',       ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
	
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',       ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',       ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '
};

char *pTabStops = tabStops;  // pointer to tabStops array

int16_t tabCol = 0;  			// current tab column setting

CURSOR_POS cursorPos = {0, 0 };
CURSOR_POS saved_cursorPos = {0, 0};

uint8_t currentAttribs = '0';   //  current setting of Attribute bit flags - must be visible ASCII char 

//  global debug flags = use bits to turn on and off various behaviors 
int16_t debugFlags = Z_FOR_EXIT;    //   set to 0 for production

//  pja - removed autoIncrement - new interface - fileSeqNum passed to dumpVD 
// int16_t autoIncrement = 0;     // for multiple repeated calls to dumpVD, showVD, etc - increment file # 

// TEG int16_t vt100_allDone = TRUE;  //  waiting for vt100_end() call 
int16_t vt100_allDone = FALSE;  //  waiting for vt100_end() call 

