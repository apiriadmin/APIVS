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
 * \file vt100.c
 * 
 * This file contains routines and data for handling the vt100 Virtual Display 
 * emulation and comparison tasks/features.
 *
 * \brief  vt100_start(), vt100_thread(), vt100_end() 
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
#include   <stdio.h>		// STD I/O Definitions
#include   <stdlib.h>		// for the malloc() function
#include   <pthread.h>
//   #include   <termios.h>		// terminal I/O structures for ioctl calls
#include   "vt100.h"

//=============================================================================
/**
 * Extern Subroutines 
 */
extern void sleep();
extern void usleep();
extern void wait();

//=============================================================================
/**
 * Extern Data   - now declared in vt100.h
 */

/*****************************
extern VIRTUAL_DISPLAY vDisplay;    // the global Virtual Display 
extern int16_t numRows;
extern int16_t numCols;

extern int16_t tabStops[DEFAULT_DISPLAY_COLS * 3];
extern int16_t tabCol;  			// current tab column setting

extern CURSOR_POS cursorPos;
extern uint8_t currentAttribs;      //  current setting of Attribute bit flags 

extern int16_t errorCode;           //  global stored & saved error code result 
**********************/

//=============================================================================
/**
 * Local Defines - see include file vt100.h - maybe some should be static defines
 *                                             in vt100_processInput.c
 */
// char getNextChar(int16_t fd);
// void processInputChar(char inChar, VIRTUAL_DISPLAY *VD_ptr);
// void processEndLineChar(char inChar, VIRTUAL_DISPLAY *VD_ptr);
// void process_ESC_Seq(int16_t fd, VIRTUAL_DISPLAY *VD_ptr);
// void processTabChar();

//=============================================================================
/**
 * Global Data Variables 
 */



//=============================================================================
/**
 * Module Private Variables 
 */

static int threadErrorCode = 0;
static pthread_t thread_descriptor;		    // thread descriptor
static THREAD_ARGS thread_data;                // data passed to the thread 
static int16_t thread_result = 0;		// returned result from thread 
static int16_t *pThreadResult = &thread_result; // pointer for the pthread_join() 
static int16_t threadNum = 0;


//=============================================================================
/**
 * \brief vt100_thread() - get characters from serial port and build a vt100-like 
 *                        representation, handling <ESC> sequences etc.
 *                        Have to malloc() a vDisplay area of pointer arrays                    
 *
 * \params[in]	pointer to a THREAD_ARGS structure, which contains:
 *                              int16_t thr_threadId;
 *                              VIRTUAL_DISPLAY *thr_DISPLAYPtr;  
 *                              int16_t thr_DISPLAYRows;
 *                              int16_t thr_DISPLAYCols;
 *                              char *thr_portName;
 *                                                          
 *        
 * \param[out]	Virtual DISPLAY is populated with text and attributes for each 
 *               character position.
 * \return		0 = Okay, otherwise an Error code - could be serial port problem
 *               Error Code must be preserved for future calls 
 */

int16_t						// no return value - provided by vt100_getError()
vt100_thread(THREAD_ARGS *args )
{
// char nextChar;					// input character from Serial port
// int16_t fd = 0;						// file descriptor for Serial Port open, read, write
const char *portName;
VIRTUAL_DISPLAY *pVDisplay;		    // pointer to virtual Display
char *pTextBuffer = NULL;			//  malloc'd text area
char *pAttribsBuffer = NULL; 		//  malloc'd attribs area
char **pTextPtrsBuffer = NULL;      //  malloc'd text pointers (rows) area
char **pAttribsPtrsBuffer = NULL;   //  malloc'd attribs pointers (rows) area 
int16_t rows         = 0;
int16_t cols         = 0;
int16_t bumpUpCols   = 0;
int16_t numBytes     = 0;
int16_t numPtrBytes  = 0;
int16_t rowLength    = 0;
int16_t i            = 0;           // array loop variable 

	//   unpack the two most crucial parameters - rows & cols - from args 
	rows = args[0].thr_DisplayRows;
	cols = args[0].thr_DisplayCols;
	//  have to allocate space on even pointer boundaries for rows 
	bumpUpCols = cols % sizeof( char *);    // see if number of columns is on a pointer boundary
	if( bumpUpCols) 
		cols += ( sizeof(char *) - bumpUpCols);  // if not (non-zero remainder), force to next boundary

	numBytes = rows * cols;
	rowLength = cols * (sizeof(char));

#if DEBUG_ON
	printf("\n\nvt100_THREAD %d is STARTING", args[0].thr_threadId);
	printf("\n  - on port %s with display rows %d and cols %d, pointer size is %d bytes", 
			  args[0].thr_portName, rows, cols, sizeof( char *) ); 
	printf("\n malloc() %d bytes each for Virtual Display text & data arrays",
			     numBytes );
#endif
	
    // 
	//   Concept - since screen size can vary in number of row or columns,
	//      we have dynamic arrays for the text display and the attributes display
	//      The Virtual Display for text is a list of pointers - 1 per line, and 
	//       a similar list of pointers for the attributes - 1 pointer per line 
	// 
	
	if (pthread_mutex_init(&vDisplay.mutex, NULL) != 0) {
		return( vt100_set_errorCode(ERR_10_THREAD_CREATE) );
	}
	
	/*    
	 *   First - malloc() all Virtual Display data area and set up pointer arrays   
	 *   
	 */
	pTextBuffer    = (char *) malloc(numBytes); 
	pAttribsBuffer = (char *) malloc(numBytes); 

	numPtrBytes    = numRows * (sizeof (char *));
	pTextPtrsBuffer = (char **) malloc(numPtrBytes );
	pAttribsPtrsBuffer = (char **) malloc(numPtrBytes );
	if( (pTextBuffer == NULL)  || (pAttribsBuffer == NULL) || (pTextPtrsBuffer == NULL) 
			|| (pAttribsPtrsBuffer == NULL) ) 
	{
#if DEBUG_ON & DEBUG_MALLOC
		printf("\n\nvt100_thread: malloc probs: numBytes %d,  numPtrBytes %d ", numBytes,
				   numPtrBytes );
		printf("\n Ptrs are: %0x, %0x, %0x, %0x", (unsigned int) pTextBuffer, 
				   (unsigned int) pAttribsBuffer, (unsigned int) pTextPtrsBuffer,
				   (unsigned int) pAttribsPtrsBuffer );
#endif
		return( vt100_set_errorCode(ERR_09_MEM_ALLOC) );
	}
	
#if DEBUG_ON & DEBUG_THREAD
	printf("\n\nvt100_thread: malloc results: numBytes %d,  numPtrBytes %d ", numBytes,
			   numPtrBytes );
#endif
	
	//  init the Virtual Display with the buffer addresses & row pointer arrays
	//   buffer & array addresses used for later free() calls 
	pthread_mutex_lock(&vDisplay.mutex);
	vDisplay.pText        = pTextPtrsBuffer;
	vDisplay.pAttribs     = pAttribsPtrsBuffer;
	vDisplay.pTextBuf     = pTextBuffer;
	vDisplay.pAttribsBuf  = pAttribsBuffer;
	vDisplay.ptrsBufSize  = numPtrBytes;
	vDisplay.charsBufSize = numBytes;
	
	//  set up the pointer arrays from malloc'd memory 
	for( i = 0; i < rows ; i++ )   // distribute the memory into pointer arrays 
	{    
		pTextPtrsBuffer[i] = pTextBuffer;
		pTextBuffer += rowLength;
		pAttribsPtrsBuffer[i] = pAttribsBuffer;
		pAttribsBuffer += rowLength;
#if DEBUG_ON & DEBUG_MALLOC 
		printf("\n row %d: TextPtr: %0x, Attribs Ptr: %0x,", i+1, (unsigned int) pTextPtrsBuffer[i], 
				(unsigned int)  pAttribsPtrsBuffer[i] );
#endif
		
	}
	
	vt100_clearVD();        // start clean & clear before text input 
	pthread_mutex_unlock(&vDisplay.mutex);
	
#if DEBUG_ON & DEBUG_MALLOC
		printf("\n vDisplay struct: TextPtrBuffer: %0x, AttribsPtrBuffer: %0x,", 
				(unsigned int) pTextPtrsBuffer, (unsigned int) pAttribsPtrsBuffer );
		printf("\n==========================================================");
		for( i = 0; i < rows; i++ ) 
		{
			printf("\n %d: text %0x,  attrib %0x ", i, (unsigned int) vDisplay.pText[i], 
					(unsigned int) vDisplay.pAttribs[i]);
		}
#endif
	
	
	// check if fpui port is present and can be opened 
	portName  = args[0].thr_portName; 
	pVDisplay = args[0].thr_DisplayPtr;
	
	if( portName == NULL )   // no fpui present - can't read any chars
	{
		while(!vt100_allDone)
		{
#if DEBUG_ON & DEBUG_THREAD
			printf("T%d, ", ++counter); 
			if( counter % 10) printf("\n");
#endif 
			sleep(1);    // wait 1secs - allow asynch call to process file input 
		}
	}
	else     
	{        
		// we are given port to open like "/dev/fpi" 
		// we will open the port and process characters received to the VT 
#if DEBUG_ON
    //  clear the display and show the results 
	// do compare on the display that was just cleared with the local dump file 
	//  this must be removed for delivery - should not be accessed from the library routines
		int16_t result = 0;
		result = vt100_compareVD("display.clear");
		printf("\nvt100_thread: compareVD: display.clear = %d \n", result);
		vt100_showVD(stdout, "After call to vt100_clearVD");
#endif
	
		if( (fdSerialPort = openSerialPort(portName) ) <= 0 ){   // Error on open
#if DEBUG_ON
			printf("\n ERR_05_PORT_OPEN; received %d trying to open %s \n", fdSerialPort, portName);
#endif
			vt100_set_errorCode(ERR_05_PORT_OPEN); 
			threadErrorCode = ERR_05_PORT_OPEN;
			vt100_allDone = TRUE;
		} else {
			// Send a "Power UP" sequence on startup
			char outputSeqBuffer[MAX_ESC_SEQ_LEN];
			int outBufLen = sprintf(outputSeqBuffer, "\x1b[PU");
			if (writeSerialPort(fdSerialPort, outputSeqBuffer, outBufLen) != outBufLen) {
				vt100_set_errorCode(ERR_07_PORT_WRITE); 
				threadErrorCode = ERR_07_PORT_WRITE;
				vt100_allDone = TRUE;
			}
		}

#if DEBUG_ON
		printf("\nTHREAD %d ready to read input from %s ... ", 
					args[0].thr_threadId, portName);
		printf("\n Please start entering input:\n  (terminate with a 'z' on a new line}\n");
#endif
	
		while(!vt100_allDone ) 
		{
			vt100_processInput( fdSerialPort, NULL, pVDisplay );     //  read Serial Port and process forever - no line buffer here
		}  // end forever while loop 
		
		// all done - close the serial port and exit
		
		closeSerialPort(fdSerialPort);
	
		// vt100_dumpVD("dump.thread.after", "thread after key inputs");
	
	}  // end - good serial port 
	
	
#if DEBUG_ON
	printf("\n\n vt100 - exiting thread # %d", args[0].thr_threadId );
#endif
	
	sleep(1);
	
	pthread_mutex_lock(&vDisplay.mutex);
	//  free the malloc'd memory in reverse order
	free( vDisplay.pAttribs );
	free( vDisplay.pText );
	free( vDisplay.pAttribsBuf );	
	free( vDisplay.pTextBuf  );
	pthread_mutex_unlock(&vDisplay.mutex);
	pthread_mutex_destroy(&vDisplay.mutex);
	
	pthread_exit(&threadErrorCode);   // return error code to the original start function
	
	
	//   return (errorCode);
	
	
}  // end vt100_thread()

//=============================================================================
/**
 * \brief vt100_resetParms() - put parameters and Tabs to power up state 
 * 
 *                                            
 *
 * \params[in]	none
 *        
 * \param[out]	parameters and tabs set to power up state
 * 
 * \return		none
 */

void
vt100_resetParms()
{
	vt100_set_errorCode(0);                     // init to zero at run-time
	vt100_clear_errorText();
	
	//  set the "Reset/Power-Up Conditions" from the ATC 5.2b spec, page 7-4
	vt100_set_autoRepeat( 0 );
	vt100_set_autoWrap( 0 );
	vt100_set_autoScroll( 0 );
	vt100_set_currentAttribs( NO_ATTRIBUTES );  // clear Reverse Video, Character Blink and Underline Attribs, + Special Char
	
	vt100_set_cursorBlink( 1 );      // Turn on Cursor and its blinking state 
	vt100_set_cursorState( 1 ); 
	vt100_set_cursorPos( 1, 1);    // cursor at top left 
	
	vt100_clear_specialChars();    // clear all down to 0 bits which should be a blank char
	
	// set the initial tab stops at 9, 17, 25, 33 per ATC 5.2b spec, page 7-4 
	vt100_set_tabStop( (int16_t) ( 9 - 1) );      // our array starts with 0 
	vt100_set_tabStop( (int16_t) (17 - 1) ); 
	vt100_set_tabStop( (int16_t) (25 - 1) ); 
	vt100_set_tabStop( (int16_t) (33 - 1) ); 

	vt100_set_backLightTimeout( (int16_t) 6 );   // set timeout to 60 secs 
	vt100_set_backLight( (int16_t) 0);           // turn off the backlight 

	return;
	
}   // end - vt100_resetParms()


//=============================================================================
/**
 * \brief vt100_start(char * portName, int16_t screenRows, int16_t screenCols )  
 *                        get characters from serial port and build a vt100-like 
 *                        representation, handling <ESC> sequences etc. 
 *                        Accesses the global data VIRTUAL_DISPLAY vDisplay of rows & columns                   
 *
 * \params[in]	*portName - pointer to port name like "/dev/c60" or "/dev/fpi" 
 *                        NOTE:  if portName == NULL - then no fpui is present 
 *                            cannot open serial port - just loop - and run in the
 *                            no_fpui mode
 *                            
 *               screenRows - number of rows in Front Panel Display
 *               
 *               screenCols - number of columns in Front Panel Display
 *        
 * \param[out]	vDisplay is populated with text and attributes for each  character 
 *                 position.
 *                 
 * \return		0 = Okay, otherwise an Error code - could be serial port problem
 *               Error Code must be preserved for future calls 
 */

int16_t 								// no return value - provided by vt100_get_error()
vt100_start(const char *portName, int16_t screenRows, int16_t screenCols )
{
int16_t res = 0;                        // holds results from thread create call 

#if DEBUG_ON
	printf("\n vt100_start for port name %s \n ", portName);
#endif
	
	numRows = screenRows;
	numCols = screenCols;
	
	vt100_set_cursorPos((int16_t) 1, (int16_t) 1);   // set to home position (external home is 1, 1)
	
	vt100_resetParms();            // reset all parameters and error codes, etc.
	
    //   vt100_clearVD();               // can't clear display until malloc'd - do in the thread after malloc 

	
	thread_data.thr_threadId     = ++threadNum;   //  notice if it is called more than once
	thread_data.thr_DisplayPtr   = &vDisplay;
	thread_data.thr_DisplayRows  = numRows;
	thread_data.thr_DisplayCols  = numCols;
	thread_data.thr_portName     = portName;
	
	
	//  do all work in a thread so control can return to calling program
	res = pthread_create(&thread_descriptor, NULL, (void *) vt100_thread, &thread_data);
	if( res != 0 )
	{
		vt100_set_errorCode( ERR_10_THREAD_CREATE );
#if DEBUG_ON
		printf("\n\n vt100_lib: Thread Create error %d\n", res);
#endif
	}
	
	sleep(1);
	
	
	return( vt100_get_errorCode() );
	
}  // end vt100_start()

//=============================================================================
/**
 * \brief vt100_end() - stop processing input and force the "clean-up" 
 * 
 *                                            
 *
 * \params[in]	Point16_ter to a Virtual_DISPLAY of R rows and C Columns
 *        
 * \param[out]	Virtual DISPLAY is populated with text and attributes for each 
 *               character position.
 * \return		0 = Okay, otherwise an Error code - could be serial port problem
 */

void
vt100_end()
{
	// printf( "\nvt100_end() ");
	vt100_allDone = TRUE;
	
	sleep(1);
	
#if DEBUG_ON 
	printf("\n vt100_end - looking for thread to exit");
#endif
	
	pthread_join( thread_descriptor, (void **) &pThreadResult );   // wait for it to finish 

	sleep(1); 
	
#if	DEBUG_ON
	printf("\n Thread returned a value of %d", *pThreadResult);
#endif
	
	sleep(1);  
	

	return;
}

