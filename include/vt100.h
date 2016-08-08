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
 * \file vt100.h
 * This file defines the structures and constants for the vt100 emulator lib
 *
 * � Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief include file
 *
 * \author Patrick Acer
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef vt100_h
#define vt100_h

//=============================================================================
/**
 * Includes
 */
#include	<stdio.h>		// STD I/O Definitions
#include	<stdint.h>		// STD IEEE Type Definitions - int16_t, uint8_t, etc.
// #include	<unistd.h>		// STD Unix Definitions 
// #include	<stdlib.h>		// STD Library Definitions
// #include	<string.h>		// STD String Definitions
// #include	<errno.h>		// STD errno Definitions

//=============================================================================
/**
 * Local Defines
 */

#define DEBUG_ON        0x000     //  >= 1 (bitFlags) for DEBUG mode, = 0 for production 
                                  //  do a bitwise OR with the following to select/add outputs 
#define DEBUG_GENERAL    0x01     //  non-specific debug messages 
#define DEBUG_MALLOC     0x02	  //  dump out malloc() result messages
#define DEBUG_LINE       0x04     //  dump lines in vt100_noFPUI()
#define DEBUG_ESC_SEQ    0x08     //  dump ESC Sequence msgs 
#define DEBUG_PARM_MATCH 0x10     //  dump other parameter names as they compare
#define DEBUG_TABS       0x20     //  dump tab related things 
#define DEBUG_FILE_IO    0x40     //  dump file related messages 
#define DEBUG_ERRORS     0x80     //  dump msgs related to error handling 
#define DEBUG_SPECIAL   0x100     //  dump Special Character processing messages 
#define DEBUG_SERIAL    0x200     //  dump Serial Port related messages
#define DEBUG_THREAD    0x400     //  dump any thread related messages 

#undef  TRUE                      // prevent conflicts with other include files
#define TRUE                1      
#undef  FALSE                     // prevent conflicts with other include files 
#define FALSE               0

#define OFF                 0     // used for AutoScroll, AutoRepeat, and
#define ON                  1     //   Reverse Video modes, and the auxSwitch 

#define ESC_CHAR          033
#define BACKSLASH        0134     //  backslash character  - 0x5C      
#define TAB_CHAR          011     //  tab                  - 0x09
#define CR_CHAR           015     //  carriage return      - 0x0D
#define NL_CHAR           012     //  new line (line feed) - 0x0A

#define MAX_ESC_SEQ_LEN    40     //  for size of buffers 
#define MAX_DUMP_LINE_LEN 200     //  should be less than 50 - 60 chars max 

#define DEFAULT_DISPLAY_ROWS  8
#define DEFAULT_DISPLAY_COLS 40 

//  Attribute Bit Flags settings- Bitwise OR with Ascii Zero 0x30 
#define ATTRIB_BLINK      0x01     // Blinking - 
#define ATTRIB_REVERSE    0x02     // Reverse Video 
#define ATTRIB_UNDER      0x04     // Underline
#define ATTRIB_SPECIAL    0x08     // Special Bitmapped Character - 1 of 8 possible chars
#define SPECIAL_CHAR_BIT  0x80     // Bitwise 'OR' in text char on top of ASCII '1' through '8' to designate Special Char
#define NO_ATTRIBUTES      '0'     // Visible ASCII char - "OR" in the bits to turn on attribs

//  Text Wrap modes
#define TEXT_WRAP_OFF        0	  // no wrap - drop off end of line
#define TEXT_WRAP_ON         1     // wrap to beginning of same line
#define TEXT_WRAP_NL         2     // wrap to next line

//   Thread constants
#define NUM_THREADS          1

//  Compare stages for internal VD to given file comparison:
#define COMPARE_TEXT         1
#define COMPARE_ATTRIBS      2
#define COMPARE_TABS         3
#define COMPARE_PARMS        4
#define COMPARE_CURSOR       5
#define COMPARE_SPEC_SIZES   6
#define COMPARE_SPEC_BITMAPS 7
#define COMPARE_GRAPHIC_MODE 8
#define COMPARE_COMPLETE     9

//   ESC Sequence Action Codes
#define CURSOR_UP            1
#define CURSOR_DOWN          2
#define CURSOR_RIGHT         3
#define CURSOR_LEFT          4
#define CURSOR_POSITION      5
#define SET_AUTO_MODES       6
#define CLEAR_AUTO_MODES     7
#define CLEAR_DISPLAY        8
#define CLEAR_LINE           9
#define CLEAR_TABS          10
#define CHAR_ATTRIBUTES     11 
#define BACKLIGHT_TIMEOUT   12
#define COMPOSE_SPECIAL     13
#define SPECIAL_CHAR        14
#define STATUS_QUERY        15
#define GET_CURSOR_POS      16
#define SAVE_CURSOR_POS     17
#define REST_CURSOR_POS     18
#define HOME_CURSOR         19
#define PANEL_TYPE          20


// Special Characters Constants 
#define NUMBER_OF_SPEC_CHARS 8     // max number of 8 different Special Characters defined
#define MIN_SPEC_CHAR_COLS   5     // each Special Char can have from 5 - 8 columns of 8-bit pixels 
#define MAX_SPEC_CHAR_COLS   8     //  (and each column is one of the BYTES_PER_SPECIAL CHARACTER )
#define BYTES_PER_SPEC_CHAR  8     // max number of 8-bit binary bit-map bytes per Special Character

// Graphic Mode Constants
#define GRAPHIC_UNDERLAY_1   0x01
#define GRAPHIC_UNDERLAY_2   0x02
#define GRAPHIC_OVERLAY_1    0x04
#define GRAPHIC_OVERLAY_2    0x08
#define GRAPHIC_CURSOR_U     0x10
#define GRAPHIC_CURSOR_O     0x20

//   Set & Clear Attributes Action Codes - not all are used for ITE VSE 
//   ITE also has used other special <ESC> sequences for some of the same actions
#define RESET_ATTRIBUTES     0
#define BOLD_ON              1
#define LOW_ON               2
#define UNDER_ON             4
#define BLINK_ON             5
#define REVERSE_ON           7
#define INVISIBLE_ON         8


//    Error Codes 
#define OKAY                 0
#define ERR_01_FILE_OPEN    -1
#define ERR_02_FILE_READ    -2
#define ERR_03_FILE_WRITE   -3
#define ERR_04_FILE_CLOSE   -4
#define ERR_05_PORT_OPEN    -5
#define ERR_06_PORT_READ    -6
#define ERR_07_PORT_WRITE   -7
#define ERR_08_PORT_CLOSE   -8
#define ERR_09_MEM_ALLOC    -9
#define ERR_10_THREAD_CREATE -10
#define ERR_11_VD_MATCH_ERR -11
#define ERR_12_COMPARE_FILE -12
#define VT100_LAST_ERRNUM   -12
#define VT100_MAX_ERRNUM  (VT100_LAST_ERRNUM * (-1) )   //  gives a positive value for array length

//   Run-time debug flags to put in global word debugFlags
#define Z_FOR_EXIT        0x01		   // lower case z ends screen input instead of waiting  


//=============================================================================
/**
 * Application Structures 
 */

    // NOTE:  12/18/2010 - remove this static structure and convert to 
    //    a malloc'd structure to allow for different sized displays 
	//
    //   typedef struct vtdisplay {
	//       char text[DEFAULT_DISPLAY_ROWS][DEFAULT_DISPLAY_COLS];
    //   	 char attribs[DEFAULT_DISPLAY_ROWS][DEFAULT_DISPLAY_COLS];
    //   } VIRTUAL_DISPLAY;


typedef struct vtdisplay {
	char **pText;            // list of pointers to Rows of text
	char **pAttribs;         // list of pointers to Rows of attributes
	char *pTextBuf;          // char buffer for text that row pointers point into 
	char *pAttribsBuf;       // char buffer for attribs that row pointers point into
	int   ptrsBufSize;       // size of the malloc'd pointer lists - for later free() calls
	int   charsBufSize;      // size of the malloc'd character buffers (text & attribs)  
	pthread_mutex_t mutex;
} VIRTUAL_DISPLAY;

typedef struct cursor {
	int16_t cp_row;
	int16_t cp_col;
} CURSOR_POS;

typedef struct threadArgs {   // arguments and data passed to the main thread by vt100_start()
	int16_t thr_threadId;
	VIRTUAL_DISPLAY *thr_DisplayPtr;
	int16_t thr_DisplayRows;
	int16_t thr_DisplayCols;
	const char *thr_portName;
} THREAD_ARGS ;

typedef struct userIntParameter {   // use to create array of User Interface Parameters
	int16_t *pUI_value;       // pointer to user interface parameter value
	char    *pUI_name;        // name of this parameter - ASCII string
}  USERINT_PARAM;


//=============================================================================

/**
 * External Global Routines 
 */

extern int16_t  openSerialPort(const char *portName);
extern char     readSerialPort(int16_t fd);
extern int16_t  writeSerialPort(int16_t fd, char *buffer, int16_t count);
extern int16_t  closeSerialPort(int16_t fd);

extern int16_t  vt100_start(const char *portName, int16_t screenRows, int16_t screenCols);
extern void     vt100_end();
extern void     vt100_clearVD();
extern int16_t  vt100_dumpVD(const char *pFileName, uint16_t fileSeqNum, const char *commentText);
extern void     vt100_showVD(FILE *filePtr, const char *commentText);
extern int16_t  vt100_compareVD(const char *pFileName);
extern int16_t  vt100_getPortFd(char *returnPortName);
extern void     vt100_processInput(int16_t fd, char *lineBuffer, VIRTUAL_DISPLAY *pDisplay);
extern int16_t  vt100_fileToVD (const char *pFileName);
extern void     vt100_vtInputBuffer(char *returnBuffer, int16_t bufferLen);
extern void     vt100_stringToVD( char *pString );
extern int      vt100_convertToASCII ( char *inPtr, char *outPtr );
extern int16_t  vt100_fromDumpToVD (const char *pFileName);
extern void     vt100_scrollVD(int16_t upOrDownFlag );

   // Global Data Access Routines 
extern USERINT_PARAM *vt100_get_otherParameters();
extern void     vt100_resetParms();
extern void     vt100_set_cursorState(int16_t newValue);
extern int16_t  vt100_get_auxSwitch();
extern int16_t  vt100_set_auxSwitch(int16_t newValue);
extern int16_t  vt100_get_cursorState();
extern void     vt100_set_cursorPos( int16_t row, int16_t col);
extern void     vt100_get_cursorPos( int16_t *pRow, int16_t *pCol);
extern void     vt100_set_cursorBlink(int16_t newValue);
extern int16_t  vt100_get_cursorBlink();
extern void 	vt100_save_cursorPos();
extern void     vt100_rest_cursorPos();
extern void     vt100_set_currentAttribs(uint8_t newAttribs);
extern uint8_t  vt100_get_currentAttribs();
extern void     vt100_clear_currentAttribs();
extern uint8_t *vt100_get_specialChar(int16_t whichChar);
extern void     vt100_clear_specialChars();

extern void     vt100_set_tabStop(uint16_t currentCol);
extern char    *vt100_get_tabStops();
extern int16_t  vt100_get_nextTabStop(uint16_t currentCol);
extern void     vt100_clear_tabStop(uint16_t currentCol);
extern void     vt100_clear_allTabStops();

extern int16_t  vt100_get_errorCode();
extern int16_t  vt100_set_errorCode(int16_t newErrorCode);
extern char    *vt100_get_errorText();
extern void     vt100_set_errorText(char *newErrorText);
extern void     vt100_clear_errorText();
extern void     vt100_set_autoWrap(int16_t newValue);
extern int16_t  vt100_get_autoWrap();
extern void     vt100_set_reverseVideo(int16_t newValue);
extern int16_t  vt100_get_reverseVideo();
extern void     vt100_set_autoRepeat(int16_t newValue);
extern int16_t  vt100_get_autoRepeat();
extern void     vt100_set_autoScroll(int16_t newValue);
extern int16_t  vt100_get_autoScroll();
extern void     vt100_set_backLight(int16_t newValue);
extern int16_t  vt100_get_backLight();
extern void     vt100_set_backLightTimeout (int16_t newValue);
extern int16_t  vt100_get_backLightTimeout();
 

//=============================================================================

/** 
 * Global Data used by all modules 
 */

extern VIRTUAL_DISPLAY vDisplay;    // this is the local DISPLAY image used by everything
extern VIRTUAL_DISPLAY *pVD;        // pointer to the Virtual Display 

extern int16_t numRows; 
extern int16_t numCols;
extern int16_t specialCharColumns[NUMBER_OF_SPEC_CHARS];
extern uint8_t specialChars[NUMBER_OF_SPEC_CHARS][BYTES_PER_SPEC_CHAR];
extern uint8_t graphicModeFlags;

extern char *fileNamePtr;           // store the passed File Name Pointer for access by all 

extern int16_t fdSerialPort;

//   NOTE:  12/18/2010 - this should be changed to a dynamic malloc'd array, but can use very large for now
extern char tabStops[DEFAULT_DISPLAY_COLS * 3];   // temp - just use a big 120 col array for now
extern char *pTabStops; 

extern int16_t tabCol;  			// current tab column setting

extern CURSOR_POS cursorPos;
extern CURSOR_POS saved_cursorPos;

extern uint8_t currentAttribs;      //  current setting of Attribute bit flags 

extern int16_t debugFlags;    		//   set to 0 for production - turns on some debug features 

//   12/18/2010 - autoIncrement removed per teg - change in interface - extension # will be passed as argument
// extern int16_t autoIncrement;     	// for multiple repeated calls to dumpVD, showVD, etc - increment file # 

extern int16_t vt100_allDone;       //  wait for vt100_end() 

#endif
