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
 * \file vt100_processInput.c
 * 
 * This file contains routines and data for handling characters input either 
 *  from the FPUI or from a given file of characters.
 *
 * � Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief  getNextChar(), processEndLineChar(), processInChar(), processTabChar()
 *         get_ESC_sequence(), process_ESC_sequence() 
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
#include   <ctype.h>        // for isdigit()
#include   <string.h>		// for strlen() 
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
 * Extern Data   declared in vt100.h 
 */

/*************
extern VIRTUAL_DISPLAY vDisplay;    // the global Virtual Display 
extern int16_t numRows;
extern int16_t numCols;

extern int16_t tabStops[DEFAULT_DISPLAY_COLS * 3];
extern int16_t tabCol;  			// current tab column setting

extern CURSOR_POS cursorPos;
extern uint8_t currentAttribs;      //  current setting of Attribute bit flags 

extern int16_t errorCode;           //  global stored & saved error code result 
**************/

//=============================================================================
/**
 * Local Defines - see include file vt100.h
 */
char getNextChar(int16_t fd, char **pBuffPtr);
void processInputChar(char inChar, VIRTUAL_DISPLAY *VD_ptr);
void processEndLineChar(char inChar, VIRTUAL_DISPLAY *VD_ptr);
void process_ESC_Seq(int16_t fd, VIRTUAL_DISPLAY *VD_ptr, char **pBuffPtr);
void processTabChar();

//=============================================================================
/**
 * Global Data Variables 
 */



//=============================================================================
/**
 * Module Private Variables 
 */



//=============================================================================
/**
 * \brief vt100_processInput(fd, line_buffer, pVirtualDisplay ) 
 *                     Process characters either from  characters 
 *                     from serial port and build a vt100-like representation or 
 *                     take chars from a line buffer (if fd is not present)
 *                     and build a vt100-like representation,
 *                     handling <ESC> sequences etc.                    
 *
 * \params[in]	      fd   or zero 
 *                    if fd is zero - use line buffer 
 *                    
 *        
 * \param[out]	Virtual DISPLAY is populated with text and attributes for each 
 *               character position.
 *               
 * \return		0 = Okay, otherwise an Error code - could be serial port problem
 *               Error Code must be preserved for future calls 
 */

void	
vt100_processInput( int16_t fd, char *lineBuffer, VIRTUAL_DISPLAY *pDisplay)
{
char nextChar;					// input character from Serial port
static char *bufferPointer = NULL;
// VIRTUAL_DISPLAY *pDisplay;		    // pointer to virtual Display
// int16_t result = 0;

	bufferPointer = lineBuffer; 
	
#if DEBUG_ON & DEBUG_LINE
	if( fd ==  0 )
	 printf("\n vt100_processInput: line: %s ", lineBuffer);
#endif
	
	while(!vt100_allDone )      // wait for vt100_end()  or end of line 
	{
		usleep (3);     // 300 millisecs delay  
		 
		nextChar = getNextChar(fd, &bufferPointer);
		if( nextChar == '\0' )
		{
			break;    // end of line of text input 
		}
		
#if DEBUG_ON
		if( debugFlags & Z_FOR_EXIT ) {
		// printf("\n read # %d: %c", ++loop_counter, nextChar);
			if(nextChar == 'z') break;   //  quick & dirty out for testing 
		}
#endif
		
		if(nextChar == ESC_CHAR ) {
			
#if DEBUG_ON & DEBUG_ESC_SEQ
			printf("\n processInput: <ESC> Char:  line is : %s", lineBuffer);
#endif 			
			process_ESC_Seq(fd, pDisplay, &bufferPointer );
		}
		else if (nextChar == TAB_CHAR) {
			processTabChar();
		}
		else if( (nextChar == NL_CHAR) || (nextChar == CR_CHAR)) {
			processEndLineChar(nextChar, pDisplay );
		}			
		else {
			processInputChar(nextChar, pDisplay );
		}
		
	}  // end forever while loop 
	


	return;


}  // end vt100_processInput()


//=============================================================================
/**
 * \brief getNextChar(fd) - read next character from serial port or file buffer
 * 
 *                                            
 *
 * \params[in]	fd - file descriptor from opening the serial port, or 0
 *              pBufferPtr - Pointer to the char string pointer for file buffer 
 *        
 * \param[out]	
 *               
 * \return		ASCII byte character 
 */

char
getNextChar(int16_t fd, char **pBufferPtr)
{
char nextChar = 0;
char *buffPtr = *pBufferPtr;

	if( fd != 0 ) 
	{
		nextChar = readSerialPort(fd);  // need to make polling/non-block
	}
	else
	{
		nextChar = *buffPtr++;
		*pBufferPtr = buffPtr;
	}
	
	return( nextChar);
	
}  //  end - getNextChar() 

//=============================================================================
/**
 * \brief processEndLineChar(char inChar, VIRTUAL_DISPLAY *VD_ptr )  
 *                                        handle new line and carriage return  
 *                                          
 *
 * \params[in]	char inChar             - last character read from Serial Port 
 *              VIRTUAL_DISPLAY *VD_ptr - pointer to the text and attributes array
 *        
 * \param[out]	
 *               
 * \return		nothing (DISPLAY array is updated) 
 */

void 
processEndLineChar(char inChar, VIRTUAL_DISPLAY *VD_ptr)
{
int16_t j=0;
char **pTextArray = VD_ptr->pText;            // set up the malloc'd data area pointers 
char **pAttribsArray = VD_ptr->pAttribs;
char *pRowText = NULL;
char *pRowAttribs = NULL;

	if( inChar == NL_CHAR )   // must have room on display to go to next line
	{
		if( cursorPos.cp_row < (numRows - 1) ) // is there still room on DISPLAY for another line?
		{ 
			cursorPos.cp_row++;
			cursorPos.cp_col = 0;
		}
		else      //  We must be at the end of the screen ... check AutoScroll Function 
		{
			if( vt100_get_autoScroll() == TRUE )
			{   
				if( vt100_get_autoWrap() == TRUE )   // Simultaneous Wrap & Scroll (like Rock 'n Roll?)
				{
					vt100_scrollVD( -1 );   // per ATC 5.2b, page 7-6 
					cursorPos.cp_row = 0;   // put cursor to top right column 
					cursorPos.cp_col = numCols -1; 
					//  already wrote blanks to whole top line - don't need to write a blank at last col again
				}
				else
				{
				// only autoScroll is on - Move display up one line; leave cursor position on the bottom line
				vt100_scrollVD(1);   
				}
			}
			else  
			{       //  it is off - clear the top line - set cursor position to top row; leave column alone 
			    pRowText = pTextArray[0];
				pRowAttribs = pAttribsArray[0];
				for(j = 0; j < numCols; j++)
				{
					*pRowText++  = ' ';
					*pRowAttribs = '0';
				}
				cursorPos.cp_row = 0; 
				// cursorPos.cp_col = 0; 
			}
		}
	}
	
	if( inChar == CR_CHAR )
	{
#if DEBUG_ON
		printf("\n processEndLineChar:  got a <CR>  char %d    ", inChar);
#endif
		cursorPos.cp_col = 0;     // Carriage Return goes back to start of line
		//  Question - do we blank out the line or just overtype???
	}

	return;
	
}  // end - processEndLineChar() 

//=============================================================================
/**
 * \brief processInputChar(char inChar, VIRTUAL_DISPLAY *VD_ptr )  
 *                                        place next character int16_to the text 
 *                                        array for the DISPLAY and update its  
 *                                        attributes as well 
 *                                          
 *
 * \params[in]	char inChar             - last character read from Serial Port 
 *              VIRTUAL_DISPLAY *VD_ptr - pointer to the text and attributes array
 *        
 * \param[out]	
 *               
 * \return		nothing (DISPLAY array is updated) 
 */

void 
processInputChar(char inChar, VIRTUAL_DISPLAY *VD_ptr)
{
int16_t fillPosition = TRUE;
int16_t j            = 0;                   // for column looping 
char **pTextArray    = VD_ptr->pText;       // set up the malloc'd data area pointers 
char **pAttribsArray = VD_ptr->pAttribs;
char *pRowText       = NULL;
char *pRowAttribs    = NULL;
char tempSpecChar    = '\0';
	
    // must check for end of row and bottom of DISPLAY using cursorPos cursor position
    //  also check for interaction of autoWrap and autoScroll features 

	if( cursorPos.cp_col > (numCols - 1) ) { // adjust since arrays count from 0, # of cols starts with 1 
		if(vt100_get_autoWrap() == TEXT_WRAP_OFF) {    // overwrite last char on line 
			cursorPos.cp_col = numCols -1;  //  starts from col = 0, set to last col position  
		}
		else if (vt100_get_autoWrap() == TEXT_WRAP_ON){
		    cursorPos.cp_col = 0;         // set to start of line and overwrite 
		}
		else  
		{   // should be = TEXT_WRAP NL  - advance to next line
			if( cursorPos.cp_row < (numRows - 1) ) { // is there still room on DISPLAY for another line?
				cursorPos.cp_row++;
				cursorPos.cp_col = 0;
			}
			else {    // at bottom right of DISPLAY - overwrite the last char position on the display
				cursorPos.cp_col = numCols - 1;  //  starts from col = 0, set to last col position 
				if( vt100_get_autoScroll() == (int16_t) TRUE){
					// interaction between Text Wrap and Auto Scroll
					// ON - scroll screen up one line - see also
					//  TBD
					;
				}
				else{
					// autoScroll is OFF - jump to top line and clear it!
					cursorPos.cp_row = 0;
					cursorPos.cp_col = 0; 
				    pRowText = pTextArray[cursorPos.cp_row];
					pRowAttribs = pAttribsArray[cursorPos.cp_row];
					for(j = 0; j < numCols; j++ ) {
						*pRowText++ = ' ';
						*pRowAttribs++ = '0';
					}
					
				}
				
			}
				
		} // end else 

	}
	
	pRowText = pTextArray[cursorPos.cp_row];
	pRowAttribs = pAttribsArray[cursorPos.cp_row];

	if( inChar & SPECIAL_CHAR_BIT)
	{
#if DEBUG_ON & DEBUG_SPECIAL
		printf("\n SET SPECIAL CHAR for 0x%02x", inChar);
#endif
		fillPosition = FALSE;                        // might be invalid spec char 
		tempSpecChar = inChar &= ~SPECIAL_CHAR_BIT;  // Turn off the special char bit 
		if( isdigit( tempSpecChar) ) 
		{
			tempSpecChar -= '0';          // convert to binary 1 to 8 
			if( (tempSpecChar > 0) && (tempSpecChar <= NUMBER_OF_SPEC_CHARS) )
			{
				// is this particular Special Character defined?? - YES if it has columns > 0 
				if( specialCharColumns[tempSpecChar - 1] > 0)
				{
					fillPosition = TRUE;      // good Special Character - well defined!!
					pRowText[cursorPos.cp_col] = tempSpecChar + '0';        
					pRowAttribs[cursorPos.cp_col] = currentAttribs | ATTRIB_SPECIAL;	// show in the attributes	

#if DEBUG_ON & DEBUG_SPECIAL
					printf("\n current Attribs is %d  [0x%0x]", currentAttribs, currentAttribs);
					printf(" ... output char is '%c' and Attribs is '%d' [0x%02x]   \n", 
								pRowText[cursorPos.cp_col], pRowAttribs[cursorPos.cp_col],
								pRowAttribs[cursorPos.cp_col] );
#endif
				// otherwise - we do absolutely NOTHING - do not load the character if defective Special Char 	
				}
			}
		}
	}
	else
	{   // normal character 
		pRowText[cursorPos.cp_col] = inChar;
		pRowAttribs[cursorPos.cp_col] = currentAttribs;
	}
		
	if( fillPosition ) 
	{
		//  now move the cursor position 
		cursorPos.cp_col++;
		if( cursorPos.cp_col > (numCols - 1) ) { // adjust since arrays count from 0, # of cols starts with 1 
			if(vt100_get_autoWrap() == TEXT_WRAP_OFF) {     // just overwrite the last column position  
				cursorPos.cp_col = numCols - 1;             //  starts at col = 0, so set to last col position 
			}
			else if (vt100_get_autoWrap() == TEXT_WRAP_ON){
			    cursorPos.cp_col = 0;         // set to start of line and next char will overwrite 
			}
			else  {  // should be = TEXT_WRAP NL  - advance to next line
				if( cursorPos.cp_row < (numRows - 1) ) { // is there still room on DISPLAY for another line?
					cursorPos.cp_row++;
					cursorPos.cp_col = 0;
				}
				else {    // at bottom right of DISPLAY -  - do nothing
					;
				}
			}
		}
	}
	
	
	return;
	
}    // end - processInputChar()

//=============================================================================
/**
 * \brief processTabChar( )  
 *                                   adjust the cursor based on tab stops if any
 *                                          
 *
 * \params[in]	
 *        
 * \param[out]	
 *               
 * \return		nothing (cursor position is updated) 
 */

void 
processTabChar()
{
int16_t nextCol;

#if DEBUG_ON & DEBUG_TABS
	printf("\nprocessTabChar() at row %d col %d ", cursorPos.cp_row + 1,
	       cursorPos.cp_col + 1);
#endif

		// see if there is a tab stop set from next col to end of the line 
    /*****************   use dataAccess routine instead  12/23/2010
	for(i = cursorPos.cp_col + 1;  i < numCols ; i++ )
	{
		if(tabStops[i] == '1' )    // tab stop at this col ?
		{
			cursorPos.cp_col = i;  // update the cursor position
			break;
		}
	}
	******************************/
	
	if( (nextCol = vt100_get_nextTabStop( cursorPos.cp_col) ) > cursorPos.cp_col )
	{
		cursorPos.cp_col = nextCol;
	}
	
	return;
	
}   // end - processTabChar()

//=============================================================================
/**
 * \brief get_ESC_Seq(int16_t fd, char * buffer, char **pBuffPtr)
 *                                        we've received and <ESC> char and '['; 
 *                                        get the rest of the ESC sequence 
 *                                            
 *
 * \params[in]	char * buffer           - buffer to load read of sequence 
 *        
 * \param[out]	
 *               
 * \return		 Action code to be taken for ESC sequence  
 */



int16_t 
get_ESC_Seq(int16_t fd, char * buffer, char **pBuffPtr)
{
char nextChar = 0;
int16_t seqIndex = 2;          //  already have <ESC> and '[' loaded into positions 0 and 1
int16_t term_index = 0;		   //  index into the ESC_terminators array 
int16_t action_code = 0; 
int16_t i = 0;

static char ESC_terminators [] = { 
		CURSOR_UP, CURSOR_DOWN, CURSOR_RIGHT, CURSOR_LEFT, 0, 0, 0, CURSOR_POSITION,   //  A - H
		0, CLEAR_DISPLAY, CLEAR_LINE, 0, 0, 0, 0, COMPOSE_SPECIAL,                     //  I - P
		0, 0, BACKLIGHT_TIMEOUT, 0, 0, SPECIAL_CHAR, 0, 0, 0, 0,                       //  Q - Z
		0, 0, 0, 0, 0, 0,                                                              //  : - @
		0, 0, 0, 0, 0, CURSOR_POSITION, CLEAR_TABS, SET_AUTO_MODES,                    //  a - h
		0, 0, 0, CLEAR_AUTO_MODES, CHAR_ATTRIBUTES, STATUS_QUERY, 0, 0, 0,             //  i - q
		0, 0, 0, 0, 0, 0, 0, 0, 0,               									   //  r - z
		0, 0, 0, 0, 0                                                                  //  { - DEL 
};

//   first - clear out the buffer 
	for( i = seqIndex;  i < MAX_ESC_SEQ_LEN; i++ )
		buffer[i] = 0; 
			
//  we have to look at next characters coming in to see if we have the whole 
//   ESC sequence.  Load the chars into the ESC sequence buffer until a 
//   valid ESC Sequence terminating character is read

	while ( 1 ) {
		//read the sequence characters and and compare to known terminators from array
		
		nextChar = getNextChar(fd, pBuffPtr);
		//  Special case processing if there are an 'A'  and 'B' - possible conflicts:
		//  
		//  Command inquiries are <ESC>[An  and <ESC>[Bn
		//   but Cursor Move Up and Down are <ESC>[ValueA  and <ESC>[ValueB (vt100 standards)
		//  If we have an A or B - have to check if previous char is '[' versus a Value
		//  If it is a '[', then we need to get the trailing 'n' character to complete
		//  the sequence. 
		//   Or we can just check the sequence index number.  If == 2, then we continue 
		//   else if it is > 2, we can proceed to look at the terminators
		
		
		
		buffer[seqIndex++] = nextChar;           // load character int16_to ESC sequence
		
		if( (nextChar == 'A')  || (nextChar == 'B') )
		{
			//  seqIndex has already autoIncremented from 2 to 3 
			if( (seqIndex == 3) && (buffer[1] == '[') ) 
			{
				// this should be an <ESC>[An type sequence 
				// don't need to do any more - wait for next Char 
			}
			else
			{
				term_index = (int16_t) (nextChar - 'A');  // likely a Cursor Pos command
				if( (action_code = ESC_terminators[term_index]) )
				{
				    break;
				}
			}
		}
		else if( (nextChar >= 'A') && (nextChar <= 'z')  ) // look for ESC seq terminators 
		{
			term_index = (int16_t) (nextChar - 'A');     // index starts with 0 for letter 'A'
			if( (action_code = ESC_terminators[term_index]) )
			{
				
				
			    break;
			}
		}
		
		if( seqIndex > (MAX_ESC_SEQ_LEN -1 )  ){
			//  very big problem somewhere !!! 
			action_code = -1; 
			break; 
		}  // end if 

		
	} // end while 
	
	return (action_code);

}//  end - get_ESC_Seq()
	

//=============================================================================
/**
 * \brief process_ESC_Seq(int16_t fd, VIRTUAL_DISPLAY *VD_ptr, char **pBuffPtr )
 *                                        we've received an <ESC> char; 
 *                                        get the rest of the ESC sequence and process 
 *                                        it.
 *                                            
 *
 * \params[in]	fd                      - file descriptor from opening the serial port
 *              VIRTUAL_DISPLAY DISPLAY   - the DISPLAY text and attributes array
 *        
 * \param[out]	
 *               
 * \return		 nothing  (DISPLAY array is updated) 
 */



void 
process_ESC_Seq(int16_t fd, VIRTUAL_DISPLAY *VD_ptr, char **pBuffPtr)
{
char ESC_sequence[MAX_ESC_SEQ_LEN];         // holds the <ESC> sequence chars 
char outputSeqBuffer[MAX_ESC_SEQ_LEN];      // output buffer to make string for serial port write
int16_t outBufLen = 0;                      // length of the output string 
int16_t numBytesWritten = 0;                // bytes written to the Serial Port 
char nextChar = 0;
int     tempIntValue = 0;
int16_t seqCode = 0;
int16_t seqValue = 0;
int16_t initCol = 0;                        // starting column (from cursorPos) in a line
int16_t i=0, j=0;
static int ESC_line = 0, ESC_col = 0;       // for Py;Px conversions
char *pESC = ESC_sequence;                  // ptr for parsing through ESC sequence 
char **pTextArray = VD_ptr->pText;          // set up the malloc'd data area pointers 
char **pAttribsArray = VD_ptr->pAttribs;
char *pRowText = NULL;
char *pRowAttribs = NULL;


	ESC_sequence[0] = ESC_CHAR;    //  init the start of the sequence
	ESC_sequence[1] = 0; 
	ESC_sequence[2] = 0;
	ESC_sequence[3] = 0;

	pRowText        = pTextArray[cursorPos.cp_row];
	pRowAttribs     = pAttribsArray[cursorPos.cp_row];

	  
	     //read the ESC sequence 
	nextChar = getNextChar(fd, pBuffPtr);
	switch (nextChar) {
	
	case 'D':
		// scroll window up one line
		vt100_scrollVD( 1 );
		break;
	case 'E':
		// move to next line  ? start of line or just next row below?
		//   if ( (cursorPos.cp_row += 1) >  numRows - 1 )
		if( cursorPos.cp_row < (numRows - 1) )
				cursorPos.cp_row++ ;   // rows start with 0 
		// cursorPos.cp_col = 0;       // comment out for now - just move down 1 row
		break;
				
	case 'H':
		// set tab at current column 
		tabCol = cursorPos.cp_col;
		tabStops[tabCol] = 'T';    // show an ASCII 'T' instead of ' '
		seqCode = 0;
		break; 
		
	case 'M':    // scroll window down one line
		vt100_scrollVD( -1 ); 
		break;
			
	case 'P':     // Compose Special Character
        // sequence is <ESC> P P1 [ Pn ; Pn ; Pn ; .... f
		ESC_sequence[1] = 'P';
		
		seqCode = get_ESC_Seq(fd, ESC_sequence, pBuffPtr);
		
		if( seqCode == CURSOR_POSITION )  // should have found an 'f' terminator
			seqCode = COMPOSE_SPECIAL;    // this should occur
		
#if DEBUG_ON & DEBUG_SPECIAL
		else
			printf("\n process_ESC_seq(): Compose SPECIAL CHAR # %d: seqCode was %d, should be %d ",
					    ESC_sequence[2], seqCode, CURSOR_POSITION );
#endif
		
		break;
			
	case '[':      //  <ESC> [  sequence 
		// get rest of sequence and process
		ESC_sequence[1] = '[';
		seqCode = get_ESC_Seq(fd, ESC_sequence, pBuffPtr);
		break;
		
	case '6':      //  <ESC> 6 n  sequence - CURSOR_POSITION request 
		//  this is an older VT-100 original query 
		//    CursorPos,  Parameters/Attributes,  auxSwitch status 
		if( (nextChar = getNextChar(fd, pBuffPtr)) == 'n' ) {
			// dummy this in to be the same as the new STATUS QUERY COMMANDS for RESPONSES
			ESC_sequence[1] = '[';
			ESC_sequence[2] = '6';
			ESC_sequence[3] = 'n';
			seqCode = STATUS_QUERY;
		}
		else
		{
			ESC_sequence[1] = '6';
			ESC_sequence[2] = nextChar;
			//  don't know really what to do here - this is not defined 
			//  no further processing will happen without the 'seqCode' being set 
		}
		break;
		
	case '7':      //  <ESC> 7 sequence - SAVE_CURSOR_POS
		ESC_sequence[1] = '7';
		seqCode = SAVE_CURSOR_POS;
		
		break;
		
	case '8':      //  <ESC> 8 sequence - REST_CURSOR_POS
		ESC_sequence[1] = '8';
		seqCode = REST_CURSOR_POS;
		
		break;
		
	default:
		//  unknown or not-handled ESC sequence 
		break;
			
	}
		

	if( seqCode > 0 )   // valid ESC Sequence that needs processing?
	{
		seqValue = 0;
		if( isdigit ( ESC_sequence[2]))
		{
			if( sscanf( &ESC_sequence[2], "%d", (int *) &seqValue) != 1 )
			{
				seqValue = 0;
			}
		}
		else if( isdigit ( ESC_sequence[3]))    // is this <ESC>[?NN or something similar 
		{
			if( sscanf( &ESC_sequence[3], "%d", (int *) &seqValue) != 1 )
			{
				seqValue = 0;
			}
		}
	
		//  now have valid sequence code and numeric value if present
				
		
#if DEBUG_ON & DEBUG_ESC_SEQ
		printf("\nprocess_ESC_sequence for <ESC> [ %c and seqCode = %d", seqValue, seqCode);
#endif
				
		switch( seqCode) {
		case CURSOR_UP:
			if ( (cursorPos.cp_row -= seqValue) < 0 )
				cursorPos.cp_row = 0;
			break;
			
		case CURSOR_DOWN:
			if ( (cursorPos.cp_row += seqValue) >=  (numRows - 1) )  // cursorPos values start with 0
				cursorPos.cp_row = numRows - 1;   // rows start with 0 
			break;
			
		case CURSOR_RIGHT:
			if ( (cursorPos.cp_col += seqValue) >= (numCols - 1) )    // cursorPos values start with 0
				cursorPos.cp_col = numCols - 1;     // force to last position 
			break;
			
		case CURSOR_LEFT:
			if ( (cursorPos.cp_col -= seqValue) < 0 )
				cursorPos.cp_col = 0;              // force to start of line position 
			break;
			
		case CURSOR_POSITION:
			cursorPos.cp_row = 0;
			cursorPos.cp_col = 0;
			// check for row;column position as opposed to top left 
			if( isdigit( ESC_sequence[2]) )  // if no number - then home to top left
			{ 
				if( sscanf(&ESC_sequence[2], "%d;%d", &ESC_line, &ESC_col) >= 2 )
				{	// ESC sequence starts row and cols with a 1 to 8 row # and 1 to 40 col #
					//  first - confirm within boundaries and set to boundaries if be
					if( ESC_line < 1 ) ESC_line = 1; 
					if( ESC_col < 1 )  ESC_col  = 1;
					if( ESC_line > numRows) ESC_line = numRows;
					if( ESC_col  > numCols) ESC_col  = numCols;
					  // internally we use cols from 0 to numCols and
					  //   row numbers from 0 to numRows 
					  // so we will subtract 1 from ESC_line & ESC_col 
					cursorPos.cp_row = ESC_line - 1;
					cursorPos.cp_col = ESC_col  - 1;
				}
			}
			break;
			
			
		case SET_AUTO_MODES:
			switch (seqValue)
			{
			case 5:    
				if( ESC_sequence[2] == '?') // set Reverse Video 				
					vt100_set_reverseVideo((int16_t) ON );
				else if( ESC_sequence[2] == '<')
					vt100_set_backLight( (int16_t) ON );
				else
#if DEBUG_ON
					printf("\n process_ESC_Seq() - [ ??? 5 h - SET_AUTO_MODES ");
#else

					;   // do nothing - maybe non-implemented <ESC> sequence 
#endif
				break;
			case 7: 
				vt100_set_autoWrap((int16_t) TEXT_WRAP_NL);
				break;
			case 8:
				vt100_set_autoRepeat( (int16_t) ON );
				break;
			case 24:       // UNDERLINE MODE ON 
				currentAttribs |= ATTRIB_UNDER;   // bit flags "OR"
				break;
			case 25:       // could be 25 or 025 in <ESC>[ sequence - 2 different actions 
				if(ESC_sequence[2] == '0')
					vt100_set_cursorState(ON);
				else					
					currentAttribs |= ATTRIB_BLINK;   // bit flags "OR"
				break;
			case 27:     // set Reverse Video 
				vt100_set_reverseVideo((int16_t) ON );
				break;
			case 33:       // Cursor Blink ON 
				vt100_set_cursorBlink(ON);
				break;
			case 47:       // AutoScroll ON 
				vt100_set_autoScroll( (int16_t) ON );
				break;
			default:
				break;
			}
			break;
			
		case CLEAR_AUTO_MODES:
			switch (seqValue)
			{
			case 5:     
				if( ESC_sequence[2] == '?') // turn off Reverse Video 				
					vt100_set_reverseVideo((int16_t) OFF );
				else if( ESC_sequence[2] == '<')
					vt100_set_backLight( (int16_t) OFF );
				else
#if DEBUG_ON
					printf("\n process_ESC_Seq() - [ ??? 5 l - CLEAR_AUTO_MODES ");
#else

					;   // do nothing - maybe non-implemented <ESC> sequence 
#endif
				break;
			case 7: 
				vt100_set_autoWrap((int16_t) TEXT_WRAP_OFF);
				break;
			case 8:
				vt100_set_autoRepeat( (int16_t) OFF );
				break;
			case 24:       // UNDERLINE MODE OFF 
				currentAttribs &= ~ATTRIB_UNDER;
				break;
			case 25:      // could be 25 or 025 in <ESC>[ sequence - 2 different actions   
				if(ESC_sequence[2] == '0')
					vt100_set_cursorState(OFF);
				else					   
					currentAttribs &= ~ATTRIB_BLINK;  // clear the Blink bit 
				break;
			case 27:     // Turn Off Reverse Video 
				vt100_set_reverseVideo((int16_t) OFF );
				break;
			case 33:       // Cursor Blink OFF 
				vt100_set_cursorBlink(OFF);
				break;
			case 47:       // AutoScroll OFF 
				vt100_set_autoScroll( (int16_t) OFF );
				break;
			default:
				break;
			}
			break;
			
		case CLEAR_DISPLAY:      //  have to do the rest of the codes - clear is only 
			switch (seqValue) 
			{
			case 0:      // Clear screen from cursor down 
				i = cursorPos.cp_row;
				initCol = cursorPos.cp_col;
				for(i = cursorPos.cp_row; i < numRows ; i++ ){
					pRowText = pTextArray[i];
					pRowAttribs = pAttribsArray[i];
					for(j = initCol; j < numCols; j++ ) {
						*pRowText++    = ' ';     // set to blanks for Text and 
						*pRowAttribs++ = '0';	  //  ASCII Zeroes for the attributes 	
						// VD_ptr->text[i][j] = ' ';   // blanks for cleared text 
						// VD_ptr->attribs[i][j] = '0'; // zero for cleared attributes 
					}
					initCol = 0;     // clear all the rest of the rows going down 
				}
				break;
				
			case 1:    // Clear screen from cursor up
				i = cursorPos.cp_row;
				initCol = cursorPos.cp_col;
				for(i = cursorPos.cp_row; i >= 0 ; i-- ){
					pRowText = pTextArray[i];
					pRowAttribs = pAttribsArray[i];
					for(j = initCol; j >= 0; j-- ) {
						*pRowText++    = ' ';     // set to blanks for Text and 
						*pRowAttribs++ = '0';	  //  ASCII Zeroes for the attributes 	
						// VD_ptr->text[i][j] = ' ';   // blanks for cleared text 
						// VD_ptr->attribs[i][j] = '0'; // zero for cleared attributes 
					}
					initCol = numCols - 1;     // clear rest of rows going up, starting from last column
				}
				break;
				
			case 2:    // Clear entire screen 
				vt100_clearVD();
				break;
				
			default:
				break;
			}
			
			break;
			
		case CLEAR_LINE:
			// cursorPos has line number 
			switch (seqValue) 
			{
			case 0:  // Clear from cursor to the right 
				
 				for(j = cursorPos.cp_col; j < numCols; j++ ) 
				    {
 					pRowText[j]    = ' ';   // blanks for cleared text 
 					pRowAttribs[j] = '0';   // zero for cleared attributes 
				    }
				break;
				
			case 1:  // Clear line from cursor to the left 
 				for(j = cursorPos.cp_col; j >= 0; j-- ) 
				    {
 					pRowText[j]    = ' ';   // blanks for cleared text 
 					pRowAttribs[j] = '0';   // zero for cleared attributes 
				    }
				break;
				
			case 2:  // Clear entire line  
 				for(j = 0; j < numCols; j++ ) 
				    {
 					pRowText[j]    = ' ';   // blanks for cleared text 
 					pRowAttribs[j] = '0';   // zero for cleared attributes 
				    }
				break;
			default:
				break;
			}
			break;
			
		case CLEAR_TABS:
			switch (seqValue) 
			{
			case 0:    // Clear a tab at the current column 
				tabStops[cursorPos.cp_col] = '0';
				break;
			case 3:    // Clear all tab stops 
				for(j = 0; j < numCols; j++ ) 
				{
					tabStops[j] = '0';
				}
				break;
			default:
				// unknown tabstop seqValue 
				break;
			}
			break;
			
		case CHAR_ATTRIBUTES:
			switch (seqValue)
			{
			case RESET_ATTRIBUTES:  // clear all attributes
				// what about the bit 8 for the Special Character???
				// it can't be kept in currentAttribs because it only applies to one character at a time
				currentAttribs = '0';
				break;
			case BLINK_ON:
				currentAttribs |= ATTRIB_BLINK;   // bit flags "OR"
				break;
			case REVERSE_ON:
				currentAttribs |= ATTRIB_REVERSE;   // bit flags "OR"
				break;
			case UNDER_ON:
				currentAttribs |= ATTRIB_UNDER;   // bit flags "OR"
				break;
			default:
				break;
				
			}
			break;
			
		case BACKLIGHT_TIMEOUT:
			vt100_set_backLightTimeout(seqValue);   // sequence value sets timeout in seconds
			                    // NOTE:  These routines currently have no implementation of  
			                    //        this timeout countdown between key strokes by user 
			                    //        at the Front Panel.
			break;
			
		case COMPOSE_SPECIAL:      // Compose a Special bit-mapped character - seqValue has which one
			                       // sequence is <ESC> P P1 [ Pn ; Pn ; Pn ; .... f
			pESC = &ESC_sequence[4];   // should be the first 8-bit # 
			
#if DEBUG_ON & DEBUG_SPECIAL
            printf("\n Composing SpecChar # %d [%c] : %s ", seqValue, ESC_sequence[2], &ESC_sequence[1] );
#endif
            
			for(i=0; i < BYTES_PER_SPEC_CHAR; i++)   // up to 8 elements before the 'f' delimiter
			{
				sscanf(pESC, "%d", &tempIntValue);
				specialChars[seqValue-1][i] = (uint8_t) tempIntValue;
				while( isdigit( *pESC) )    // bypass the number just converted 
					pESC++;
				
				if( *pESC == ';' ) {
					pESC++;   // skip the ';' delimiter to get to the next bit-map byte 
				}
				
				if( *pESC == 'f' ){  // this will allow a ";f"  sequence even though it might be wrong
					i++;      // account for this Pn Value before breaking the loop
					break;    // special chars can be 5 - 8 columns long; Pn = P1 - P8
				}
			}
#if DEBUG_ON & DEBUG_SPECIAL
			if( *pESC != 'f' ) {
				printf("\n vt100_processInput error: loading Special Char # %d: terminator '%c' should be 'f'",
							 seqValue, *pESC);
			// NOTE:  This could be caused it there are more than 8 Pn values, which would make a very wide 
			//         Special Character, which would be invalid and cleared out 
			}
#endif

			if( (i >= MIN_SPEC_CHAR_COLS )  && (i <= MAX_SPEC_CHAR_COLS ) )
			{
				specialCharColumns[seqValue-1] = i;    // should be a number in range 5 - 8 
			}
			else
			{    // should clear out this invalid attempt 
#if DEBUG_ON & DEBUG_SPECIAL
				printf("\n vt100_processInput:  Invalid attempt to set up Special Character # %d - only %d columns",
						           seqValue, i);
#endif 
				
				for(i=0; i < BYTES_PER_SPEC_CHAR; i++)   // up to 8 elements before the 'f' delimiter
				{
					specialChars[seqValue-1][i] = 0;
				}
	
			}
			
			break;
				
		case SPECIAL_CHAR:    // display one of 8 Special Characters as a ASCII Digit '1' thru '8' 
			// and put the Atrribute Bit On for Speciall Char for that character only
			// The special character number is in the "seqValue" variable 
			processInputChar((char) ((uint8_t) ( (0x30 + seqValue) | 0x80) ), pVD);  // ASCII '0' + seq + bit 7

			break;

		case STATUS_QUERY: 
			// one of three possible queries - cursorPos, attribs/parameters, auxSwitch state 
			// all need to have responses generated and written to the serial port fd if open 
			//  have to send (transmit) cursor pos message onto Serial Port (fd)
			//  sequence is:  <ESC>[<row>;<col>R or similar 
			// clear and load an output buffer for response 
#if DEBUG_ON & DEBUG_SERIAL
			printf("\n vt100_process_ESC_Seq:  STATUS QUERY # %c ", ESC_sequence[2]	);
#endif
			
			for(i = 0; i < MAX_ESC_SEQ_LEN; i++ ){
				outputSeqBuffer[i] = '\0';
			}
			outputSeqBuffer[0] = ESC_CHAR;
			outputSeqBuffer[1] = '[';
			
			switch ( ESC_sequence[2] ) {
			
			case '6':    // return Cursor Position 
				sprintf(&outputSeqBuffer[2], "%d;%dR", cursorPos.cp_row + 1, cursorPos.cp_col + 1);
				break;
				
			case 'A':    // return Auxillary Switch Status
				sprintf(&outputSeqBuffer[2], "%dR", vt100_get_auxSwitch() );
				break;
				
			case 'B':    // return status of various attributes and parameters 
				sprintf(&outputSeqBuffer[2], "%d;%d;%d;%d;%d;%dR", 
						vt100_get_autoWrap(),  vt100_get_autoScroll(),       vt100_get_autoRepeat(),
						vt100_get_backLight(), vt100_get_backLightTimeout(), vt100_get_auxSwitch()
						);
				break;
				
			default:
				break;
			}
			
			outBufLen = strlen(outputSeqBuffer);

			if( fdSerialPort > 0 )
			{
				numBytesWritten = writeSerialPort(fdSerialPort, outputSeqBuffer, outBufLen ); 
				if( numBytesWritten == outBufLen )
				{
					;  // good command written out 
				}
				else
				{
					;      // null statement - placeholder
#if DEBUG_ON & DEBUG_SERIAL
					//     ERR_07_PORT_WRITE  if there was a return value 
					printf("\n vt100_process_ESC_Seq:  Only wrote %d of %d bytes to Serial port for CMD/RESP",
							    numBytesWritten, outBufLen); 
#endif
				}
			}
			else 
			{
#if DEBUG_ON & DEBUG_SERIAL
				printf("\n vt100_process_ESC_Seq: write out to serial port %d chars:\n  %s ", 
						     outBufLen, outputSeqBuffer);
#endif 
			}

			break;

		case SAVE_CURSOR_POS:
			vt100_save_cursorPos();
			break;
			
		case REST_CURSOR_POS:
			vt100_rest_cursorPos();
			break;
			
		default:
			break;
						
		}   // end switch
	
	}   // end if 
	
}  // end process_ESC_Seq
