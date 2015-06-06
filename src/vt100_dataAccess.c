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
 * \file vt100_dataAccess.c
 * 
 * This file contains the routines that access the global data variables except
 *  for the VIRTUAL DISPLAY structure.
 *
 * � Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief   vt100 data access routines - set & get for autoWrap, autoScroll
 *          autoRepeat and backLight global variables 
 *
 * \date    11/15/2010 
 *
 * \author  Patrick Acer 
 *
 * \version 1.0
 *
 *****************************************************************************/

//=============================================================================
/**
 * Includes
 */
// #include	<stdint.h>		// STD IEEE Type Definitions - int16_t, uint8_t, etc.
#include "vt100.h"

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */

      // Error data - Global Data - all modules can set, use the get routines for library
static int16_t errorCode = 0;         // stored global error code, = 0 if OKAY
static char *pErrorText = "  initial test error text message    ";
static char *pErrorData[VT100_MAX_ERRNUM + 2] = {
        "0 = vt100 lib: Okay - no error", 
        "ERR_01_FILE_OPEN      -1",
        "ERR_02_FILE_READ      -2",
        "ERR_03_FILE_WRITE     -3",
        "ERR_04_FILE_CLOSE     -4",
        "ERR_05_PORT_OPEN      -5",
        "ERR_06_PORT_READ      -6",
        "ERR_07_PORT_WRITE     -7",
        "ERR_08_PORT_CLOSE     -8",
        "ERR_09_MEM_ALLOC      -9",
        "ERR_10_THREAD_CREATE -10",
        "ERR_11_VD_MATCH_ERR  -11",
        "vt100 lib - end of error list"
};

      //  Mode Status Words  - only visible to the routines in this module  
static int16_t autoWrap         = TEXT_WRAP_OFF;   //  treatment at end of line (rightmost column)
static int16_t autoRepeat       = OFF;
static int16_t autoScroll       = OFF;
static int16_t backLight        = OFF;
static int16_t backLightTimeout = 0;
static int16_t cursorState      = ON;
static int16_t cursorBlink      = ON;
static int16_t saved_cursorState = OFF;
static int16_t saved_cursorBlink = OFF;
static int16_t reverseVideo     = OFF;
static int16_t auxSwitch        = OFF;

static USERINT_PARAM otherParameters[] = {
		{ &autoWrap,    "autoWrap"  },
		{ &autoRepeat,  "autoRepeat"  },
		{ &autoScroll,  "autoScroll"  },
		{ &backLight,  "backLight"  },
		{ &backLightTimeout, "backLightTimeout"  },
		{ &cursorState,  "cursorState"  },
		{ &cursorBlink,  "cursorBlink"  },
		{ &reverseVideo, "reverseVideo"  },
		{ &auxSwitch,    "auxSwitch"  },
		// { &cursorPos.cp_row,  "cursorRow "  },                 // can't compare because internal starts with zero and 
		// { &cursorPos.cp_col,  "cursorCol "  },                 //  stored file values start with 1 for row and column 
		{  NULL ,      NULL }       //  must end list with a NULL 
};


//=============================================================================
/**
 * \brief       USERINT_PARAM *get_otherParameters()
 * 
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
USERINT_PARAM *
vt100_get_otherParameters( )
{
		return(otherParameters);

}

//=============================================================================
/**
 * \brief       void vt100_set_auxSwitch( int16_t newValue)
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
int16_t
vt100_set_auxSwitch( int16_t newValue)
{
static char commandStrings[2][4] = {  // Aux Switch status sequences: 
         { 0x1b, 'O', 'U', '\0' },    // Aux Switch OFF
         { 0x1b, 'O', 'T', '\0' }     // Aux Switch ON 
};
#define COMMAND_LENGTH  3
char *bufPtr = NULL;
int16_t numBytesWritten = 0;
int16_t returnValue = 0;

		auxSwitch = newValue;
		bufPtr = commandStrings[newValue];   // write out to port before changing the value
		//   write a 3 byte command/response to the serial port if active 
		if( fdSerialPort > 0 )
		{
			numBytesWritten = writeSerialPort(fdSerialPort, bufPtr, COMMAND_LENGTH ); 
			if( numBytesWritten != COMMAND_LENGTH)
			{
				returnValue = ERR_07_PORT_WRITE; 
			}
			
		}
		else
		{
#if DEBUG_ON & DEBUG_SERIAL
int16_t i = 0;
			printf("\n vt100_set_auxSwitch: Write Reply to Serial Port %d bytes: ", COMMAND_LENGTH);
			for( i= 0; i < COMMAND_LENGTH;  i++ ){
				printf("%c [0x%02x]  ", bufPtr[i], bufPtr[i]);
			}
			printf("\nDEBUG ONLY:  auxSwitch changed to new value %d \n", newValue);
			auxSwitch = newValue;
#endif

		}
			
		return (returnValue);
		
}

//=============================================================================
/**
 * \brief       int16_t vt100_get_auxSwitch( )
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
int16_t
vt100_get_auxSwitch( )
{
		return(auxSwitch);

}



//=============================================================================
/**
 * \brief       void vt100_set_cursorState( int16_t newValue)
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
void
vt100_set_cursorState( int16_t newValue)
{
		cursorState = newValue;
	
		return;

}

//=============================================================================
/**
 * \brief       int16_t vt100_get_cursorState( )
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
int16_t
vt100_get_cursorState( )
{
		return(cursorState);

}


//=============================================================================
/**
 * \brief       void vt100_set_cursorPos( int16_t row, int16_t col)
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
void
vt100_set_cursorPos( int16_t row, int16_t col)
{
		//  first - confirm within boundaries and set to boundaries 
	    //    rows and cols run from 1 to numRows and numCols 
		if( row < 1 ) row = 1; 
		if( col < 1 ) col = 1;
		if( row > numRows )
		{	
			row = numRows;
		}
		if( col > numCols )
		{
			col = numCols;
		}

		// internally rows and cols go from 0 to (num - 1)
		cursorPos.cp_row = row - 1;
		cursorPos.cp_col = col - 1;
		
		return;

}

//=============================================================================
/**
 * \brief       void vt100_get_cursorPos( int16_t *pRow, int16_t *pCol)
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
void
vt100_get_cursorPos( int16_t *pRow, int16_t *pCol)
{
		*pRow = cursorPos.cp_row + 1;     // change to a 1 to numRows based system to report back
		*pCol = cursorPos.cp_col + 1;     // change to a 1 to numCols based system to report back
		
		return;

}

//=============================================================================
/**
 * \brief       void vt100_save_cursorPos( int16_t row, int16_t col)
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
void
vt100_save_cursorPos()
{
		saved_cursorPos.cp_row = cursorPos.cp_row;
		saved_cursorPos.cp_col = cursorPos.cp_col;
		saved_cursorState      = cursorState;
		saved_cursorBlink      = cursorBlink;
		
		return;

}

//=============================================================================
/**
 * \brief       void vt100_rest_cursorPos( int16_t *pRow, int16_t *pCol)
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
void
vt100_rest_cursorPos( )
{
		cursorPos.cp_row  = saved_cursorPos.cp_row;
		cursorPos.cp_col  = saved_cursorPos.cp_col;
		cursorState       = saved_cursorState;
		cursorBlink       = saved_cursorBlink;
		
		return;

}


//=============================================================================
/**
 * \brief       void vt100_setCursorBlink( int16_t newValue)
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
void
vt100_set_cursorBlink( int16_t newValue)
{
		cursorBlink = newValue;
	
		return;

}

//=============================================================================
/**
 * \brief       int16_t vt100_getCursorBlink( )
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
int16_t
vt100_get_cursorBlink( )
{
		return(cursorBlink);

}


//=============================================================================
/**
 * \brief       uint8_t vt100_get_currentAttribs() 
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
uint8_t
vt100_get_currentAttribs()
{
		
		return ( currentAttribs );

}

//=============================================================================
/**
 * \brief       void vt100_set_currentAttribs(uint8_t newAttribs) 
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
void
vt100_set_currentAttribs(uint8_t newAttribs)
{
		currentAttribs = newAttribs;
		return;

}

//=============================================================================
/**
 * \brief       void vt100_clear_currentAttribs() 
 *
 * \param[in]	none 

 * \param[out]	none 

 * \return		none   
 */
void
vt100_clear_currentAttribs()
{
		currentAttribs = (uint8_t) NO_ATTRIBUTES;
		return;

}

//=============================================================================
/**
 * \brief       char *vt100_get_tabStops() 
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		pointer to array of ASCII chars ' ' or 'T';  'T' = tab stop at that col  
 *              Note:  the array is always visible, printable characters to make 
 *                     confirmation and output very simple 
 */
char *
vt100_get_tabStops()
{
		
		return ( tabStops );

}

//=============================================================================
/**
 * \brief       uint16_t vt100_get_nextTabStop(uint16_t currentCol ) 
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
int16_t
vt100_get_nextTabStop(uint16_t currentCol )
{
uint16_t i = 0;
int16_t nextTabCol = 0;
char *tabs;

		tabs = vt100_get_tabStops();
		
		for( i=currentCol+1;  i < numCols; i++)
			if( (tabs[i] == 'T') ||  (tabs[i] == 't') )
			{
				nextTabCol = i;
				break;
			}
		
		return ( nextTabCol );   // should never have a tab stop in the first column - it is meaningless

}

//=============================================================================
/**
 * \brief       void vt100_set_tabStop(uint16_t currentCol) 
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
void
vt100_set_tabStop(uint16_t currentCol )
{
		tabStops[currentCol] = 'T';
		
		return;

}

//=============================================================================
/**
 * \brief       void vt100_clear_tabStop(uint16_t currentCol) 
 *
 * \param[in]	none 

 * \param[out]	none 

 * \return		none   
 */
void
vt100_clear_tabStop(uint16_t currentCol)
{
		tabStops[currentCol] = ' ';
		
		return;

}

//=============================================================================
/**
 * \brief       void vt100_clear_allTabStops() 
 *
 * \param[in]	none 

 * \param[out]	none 

 * \return		none   
 */
void
vt100_clear_allTabStops()
{
uint16_t i = 0; 
char *tabs;

			tabs = vt100_get_tabStops();
			
			for( i= 0;  i < numCols; i++)
				tabs[i] = ' ';
				
			
			return;   

}

//=============================================================================
/**
 * \brief       uint8_t *vt100_get_specialChar( whichChar ) 
 *
 * \param[in]	int16_t whichChar  - number from 1 - 8 to select which special Char 

 * \param[out]	none 

 * \return		pointer to the 8-byte array of uint8_t bit-mapping bytes
 *               for that special character    
 */
uint8_t *
vt100_get_specialChar(int16_t whichChar)
{
		if( (whichChar > 0) && (whichChar <= NUMBER_OF_SPEC_CHARS) )
		{    // should we also check if this special char # has been loaded? 
			return ( specialChars[whichChar] );
		}
		else
			return( NULL );    // this is an error condition 

}

//=============================================================================
/**
 * \brief       void vt100_set_specialChars(uint8_t whichChar) 
 *
 * \param[in]	int16_t   whichChar - which of the 8 chars (1 - 8)

 * \param[out]	none 

 * \return		none   
 */

/******************  this is done in-line in vt100_processInput.c 
void
vt100_set_specialChar(uint8_t whichChar)
{
		for( i= 0; i < 8;  i++ ) {
		  specialChars[whichChar][i] = Pn[i];
		}
		
		return;

}
*******************  end of vt100_set_specialChar()  ******************/



//=============================================================================
/**
 * \brief       void vt100_clear_specialChars() 
 *
 * \param[in]	none 

 * \param[out]	none 

 * \return		none   
 */
void
vt100_clear_specialChars()
{
int16_t i=0, j=0;

		for( i = 0; i < NUMBER_OF_SPEC_CHARS; i++)
		{
			for(j=0; j < BYTES_PER_SPEC_CHAR; j++)
			{
				specialChars[i][j] = 0;      // set all bits to 0 = a blank character
				specialCharColumns[i] = 0;   //  indicate that this Special Char is not loaded 
			}
		}

		return;

}



//=============================================================================
/**
 * \brief       int16_t vt100_get_errorCode()
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		errorCode value  
 */
int16_t
vt100_get_errorCode()
{
		return(errorCode);
}

//=============================================================================
/**
 * \brief       void vt100_set_errorCode(int16_t newErrorCode)
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
int16_t
vt100_set_errorCode(int16_t newErrorCode)
{
		errorCode  = newErrorCode;
		
		//  NOTE:  Error codes are all < = 0  (negative or zero!)

		pErrorText = pErrorData[-errorCode];
		
#if DEBUG_ON & DEBUG_ERRORS
		printf("\n set_errorCode: %d, errorText = %s", newErrorCode, vt100_get_errorText() );
#endif
		
		return( errorCode );

}


//=============================================================================
/**
 * \brief       char *vt100_get_errorText()
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		errorText value  
 */
char *
vt100_get_errorText()
{
int16_t errorIndex = 0;

		errorIndex = vt100_get_errorCode();

		if( pErrorText == NULL)
		{     // this should never happen 
#if DEBUG_ON & DEBUG_ERRORS
			printf("\n get_errorText 01: NULL .. for errorIndex: %d", errorIndex);
#endif
			//  Note:  Error codes are all negative or zero, i.e. <= 0 
			pErrorText = pErrorData[-errorIndex];
		}
		
		return(pErrorText);      // set with call to vt100_set_errorCode
}

//=============================================================================
/**
 * \brief       void vt100_set_errorText(char * newErrorText)
 *
 * \param[in]	char * newErrorText

 * \param[out]	none 

 * \return		nothing  
 */
void
vt100_set_errorText(char *newErrorText)
{
	
#if DEBUG_ON & DEBUG_ERRORS
	    printf("\n setErrorText with: %s", newErrorText);
#endif
	    
		pErrorText = newErrorText;
		return;
}

//=============================================================================
/**
 * \brief       void vt100_clear_errorText()
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		nothing  
 */
void
vt100_clear_errorText()
{
		
		pErrorText = NULL;
		return;
}


//=============================================================================
/**
 * \brief       void vt100_setAutoWrap(int16_t newValue)
 *
 * \param[in]	int16_t newValue = 0 for OFF,  1 for Same Line, 2 for Next Line

 * \param[out]	none 

 * \return		none 
 */
void
vt100_set_autoWrap(int16_t newValue)
{
		autoWrap = newValue;
		return;
}

//=============================================================================
/**
 * \brief       int16_t vt100_getAutoWrap()
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		autoWrap value  
 */
int16_t
vt100_get_autoWrap()
{
		return(autoWrap);
}

//=============================================================================
/**
 * \brief       void vt100_setAutoWrap(int16_t newValue)
 *
 * \param[in]	int16_t newValue = 0 for OFF,  1 for Same Line, 2 for Next Line

 * \param[out]	none 

 * \return		none 
 */
void
vt100_set_reverseVideo(int16_t newValue)
{
		reverseVideo = newValue;
		return;
}

//=============================================================================
/**
 * \brief       int16_t vt100_getAutoWrap()
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		reverseVideo value  
 */
int16_t
vt100_get_reverseVideo()
{
		return(reverseVideo);
}



//=============================================================================
/**
 * \brief       void vt100_set_autoRepeat(int16_t newValue)
 *
 * \param[in]	int16_t newValue = 0 for OFF,  = 1 for ON

 * \param[out]	none 

 * \return		none 
 */
void
vt100_set_autoRepeat(int16_t newValue)
{
		autoRepeat = newValue;
		return;
}

//=============================================================================
/**
 * \brief       int16_t vt100_get_autoRepeat()
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		autoRepeat value  
 */
int16_t
vt100_get_autoRepeat()
{
		return(autoRepeat);
}


//=============================================================================
/**
 * \brief       void vt100_set_autoScroll(int16_t newValue)
 *
 * \param[in]	int16_t newValue = 0 for OFF,  = 1 for ON

 * \param[out]	none 

 * \return		none 
 */
void
vt100_set_autoScroll(int16_t newValue)
{
		autoScroll = newValue;
		return;
}

//=============================================================================
/**
 * \brief       int16_t vt100_get_autoScroll()
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		autoScroll value  
 */
int16_t
vt100_get_autoScroll()
{
		return(autoScroll);
}


//=============================================================================
/**
 * \brief       void vt100_set_backLight(int16_t newValue)
 *
 * \param[in]	int16_t newValue = 0 for OFF,  = 1 for ON

 * \param[out]	none 

 * \return		none 
 */
void
vt100_set_backLight(int16_t newValue)
{
		backLight = newValue;
		return;
}

//=============================================================================
/**
 * \brief       int16_t vt100_get_backLight()
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		backLight value  
 */
int16_t
vt100_get_backLight()
{
		// can't really tell if the backlight is on or off unless we can time the timeout
	    //   in real time ... and know how the screens keep flipping on the Front Panel
	    //   with various user actions and inputs 
	
	    // every user keystroke resets the timeout value, and libVT100 routines have no 
	    //  knowledge of keystrokes 
		return(backLight);
}

//=============================================================================
/**
 * \brief       void vt100_set_backLightTimeout( int16_t newValue )
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
void
vt100_set_backLightTimeout( int16_t newValue )
{
		backLightTimeout = newValue;
		
		return;

}

//=============================================================================
/**
 * \brief       void vt100_get_backLightTimeout( int16_t *pRow, int16_t *pCol)
 *
 * \param[in]	none

 * \param[out]	none 

 * \return		none   
 */
int16_t 
vt100_get_backLightTimeout( )
{
		
		return(backLightTimeout); 

}

