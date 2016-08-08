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
 * \file vt100_dumpVD.c
 * 
 * Dump the Virtual Display to a given filename 
 * NOTE:  This routine must be exactly synchronized with vt100_showVD()
 *          and vt100_fromDumpToVD()
 *
 * � Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief vt100_dumpVD()
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
#include    <time.h>
#include    "vt100.h"

//=============================================================================
/**
 * Extern
 */
extern int isprint();

//=============================================================================
/**
 * Local Defines
 */

//=============================================================================
/**
 * \brief       int16_t vt100_dumpVD(char *pFileName, uint16_t fileSeqNum, char *commentText)
 *               dump the contents of the VD to file in human readable format
 *               including the text and the attributes
 *
 * \param[in]	pFileName - something like "dump_01.txt"
 *              commentText - optional text or "" - will appear in file comments
 * 
 * \param[out]	Description
 * 
 * \return		= 0  -  good 
 *              < 0  -  error number
 * 
 */
int16_t
vt100_dumpVD (const char *pFileName, uint16_t fileSeqNum, const char *commentText)
{
// int16_t fd = 0;
int16_t errNum = 0;
//  static int16_t autoIncrement = 0;   // moved to global data 
int16_t i = 0, j = 0, k = 0;                   // loopo control variables 
char fileNameBuffer [100];
char outBuffer[DEFAULT_DISPLAY_COLS * 4];      // for Special Character disolay
char *outPtr = outBuffer;
int16_t specCols = 0;  	                       // # of columns in this special character 
const char *commentPtr = " ";
char tempChar = ' ';
int16_t countSpecChars = 0;
int16_t numBytes = 0;
char lineBuffer[DEFAULT_DISPLAY_COLS * 2];     //  leave plenty of extra room for other messages
FILE *filePtr;
time_t rawtime;
char **pTextArray    = vDisplay.pText;         // set up the malloc'd data area pointers 
char **pAttribsArray = vDisplay.pAttribs;
char *pRowText       = NULL;				   // ptrs for looping buffer moves 
char *pRowAttribs    = NULL;                   //  "
char *p_tabStops     = NULL;                   //  "


	if( commentText != (char *)NULL )
	{
		commentPtr = commentText;
	}

	//  autoIncrement++;     // replaced by fileSeqNum
	//  sprintf(fileNameBuffer, "%s.%03d", pFileName, autoIncrement);
	//  if fileSeqNum == 0 then do not add a sequence number to the filename
	if( fileSeqNum > 0 )
		sprintf(fileNameBuffer, "%s.%03d", pFileName, fileSeqNum);
	else
		sprintf(fileNameBuffer, "%s", pFileName);
		

	if( (filePtr = fopen(fileNameBuffer, "w+" )) == NULL)
		return(vt100_set_errorCode(ERR_01_FILE_OPEN) );
	
	//   TEXT DISPLAY 
	//   get current date and time and put into the file heading 
    time ( &rawtime );
    strftime(lineBuffer,30 , "%Y%m%d %H:%M:%S",localtime(&rawtime));
	fprintf(filePtr, "#\n# -- Virtual Display and Global Variable Dump --");
	fprintf(filePtr,  "\n# Date: %s - %s   \n#", lineBuffer, commentPtr);
	fprintf(filePtr,  "\n# Display Rows: \n#0       ");
	// fprintf(pFile,"\n");
	// fprintf(filePtr,  "\n#0       1         2         3         4");
	// fprintf(filePtr,  "\n#234567890123456789012345678901234567890\n#\n");
	//  12/20/2010 - change to dynamic line length headers 
	for(j = 10; j <= numCols;  j += 10){
		fprintf(filePtr, "%d         ", j/10 );
	}
	fprintf(filePtr, "\n#23456789");
	for(j = 10; j <= numCols;  j += 10){
		fprintf(filePtr, "0123456789");
	}
	fprintf(filePtr, "\n");
		
	
		// move text display to Line Buffer, 1 line at a time, & check if visible chars 
	for( i = 0; i < numRows; i++)
	{	
		pRowText = pTextArray[i];
		for( j=0; j < numCols ; j++ ){
			tempChar = pRowText[j];
			if(!isprint(tempChar)) tempChar = '?';  // don't try to show unprintable chars 
			lineBuffer[j] = tempChar;
		}
		lineBuffer[j++] = '\n';    //  put end of line and string terminator
		lineBuffer[j++] = '\0'; 
		
		// length = strlen(lineBuffer);

		if( (errNum = fprintf(filePtr, "%s", lineBuffer)) <= 0 )
		{
			printf("\n ER01 - file write error %d \n", errNum);
			return( vt100_set_errorCode(ERR_03_FILE_WRITE) );
		}
				
	}   // end for()  - moving text out to buffer
	
	// delineate the data with a line of dashes:
	if( (errNum = fprintf(filePtr, "#----------------------------------------\n#")) <= 0 )
	{
		printf("\n ERR LOC 02: File Write Err # %d", errNum);
		return( vt100_set_errorCode(ERR_03_FILE_WRITE) );
	}
	
	//   ATTRIBUTE DISPLAY 
	fprintf(filePtr, "\n#\n# Attribute Rows: \n#0       ");
	// fprintf(filePtr, "\n#0       1         2         3         4");
	// fprintf(filePtr," \n#234567890123456789012345678901234567890\n#\n");
	//  12/20/2010 - change to dynamic line length headers 
	for(j = 10; j <= numCols;  j += 10){
		fprintf(filePtr, "%d         ", j/10 );
	}
	fprintf(filePtr, "\n#23456789");
	for(j = 10; j <= numCols;  j += 10){
		fprintf(filePtr, "0123456789");
	}
	fprintf(filePtr, "\n");

		// move attributes to Line Buffer 1 line at a time - check the characters 
	for( i = 0; i < numRows; i++)
	{
		pRowAttribs = pAttribsArray[i];
		for( j=0; j < numCols ; j++ ){
			tempChar = pRowAttribs[j];
			if(!isprint(tempChar)) tempChar = '?';
			lineBuffer[j] = tempChar;
		}
		lineBuffer[j++] = '\n';    //  put end of line and string terminator
		lineBuffer[j++] = '\0'; 
		
		if( fprintf(filePtr, "%s", lineBuffer) <= 0 )
		{
			printf("\n ERR LOC 03: File Write \n");
			return( vt100_set_errorCode(ERR_03_FILE_WRITE) );
		}	
		
	}  // end for() loop - moving attributes out to buffer 
	
	fprintf(filePtr, "#----------------------------------------\n#");  // delineate the data
	

	//   TAB STOPS DISPLAY 
	fprintf(filePtr, "\n#\n# Tab Stops: \n#0       ");
	// fprintf(filePtr, "\n#0       1         2         3         4");
	// fprintf(filePtr, " \n#234567890123456789012345678901234567890\n#\n");
	//  12/20/2010 - change to dynamic line length headers 
	for(j = 10; j <= numCols;  j += 10){
		fprintf(filePtr, "%d         ", j/10 );
	}
	fprintf(filePtr, "\n#23456789");
	for(j = 10; j <= numCols;  j += 10){
		fprintf(filePtr, "0123456789");
	}
	fprintf(filePtr, "\n");

	//   Tabs stops row display  
	// move tab chars to Line Buffer 1 line at a time - check the characters 
	p_tabStops = tabStops;
	for( j=0; j < numCols ; j++ ){
		tempChar = p_tabStops[j];
		if(!isprint(tempChar)) tempChar = '?';
		lineBuffer[j] = tempChar;
	}
	
	lineBuffer[j++] = '\n';    //  put end of line and string terminator
	lineBuffer[j++] = '\0'; 
	
	if( fprintf(filePtr, "%s", lineBuffer) <= 0 )
	{
		printf("\n ERR LOC 03: File Write \n");
		return(vt100_set_errorCode(ERR_03_FILE_WRITE) );
	}	

	
	//  PARAMETERS DISPLAY 
	fprintf( filePtr, "#\n#\n#"); 
	fprintf( filePtr, "\n#==================================================\n#");
	fprintf( filePtr, "\n# Global Variables:");
	// fprintf( filePtr, "\n# backlight %d,  autoWrap %d, autoScroll %d autoRepeat %d \n# Cursor Pos %d:%02d, Blink %d \n#\n",
	// 		    vt100_get_backLight(), vt100_get_autoWrap(), vt100_get_autoScroll(), vt100_get_autoRepeat(),
	// 		    cursorPos.cp_row, cursorPos.cp_col,  vt100_get_cursorBlink() );

	
	fprintf(filePtr, "\n# autoWrap: ");
	fprintf(filePtr, "\n%d", vt100_get_autoWrap() );
	
	fprintf(filePtr, "\n# autoRepeat: ");
	fprintf(filePtr, "\n%d", vt100_get_autoRepeat() );
	
	fprintf(filePtr, "\n# autoScroll: ");
	fprintf(filePtr, "\n%d", vt100_get_autoScroll() );
	
	fprintf(filePtr, "\n# backLight: ");
	fprintf(filePtr, "\n%d", vt100_get_backLight() );
	
	fprintf(filePtr, "\n# backLightTimeout: ");
	fprintf(filePtr, "\n%d", vt100_get_backLightTimeout() );
	
	fprintf(filePtr, "\n# cursorState: ");
	fprintf(filePtr, "\n%d", vt100_get_cursorState() );
	
	fprintf(filePtr, "\n# cursorBlink: ");
	fprintf(filePtr, "\n%d", vt100_get_cursorBlink() );
	
	fprintf(filePtr, "\n# reverseVideo: ");
	fprintf(filePtr, "\n%d", vt100_get_reverseVideo() );
	
	fprintf(filePtr, "\n# auxSwitch: ");
	fprintf(filePtr, "\n%d", vt100_get_auxSwitch() );
	
	fprintf(filePtr, "\n#_cursorPos: ");
	fprintf(filePtr, "\n%d,%d", cursorPos.cp_row + 1, cursorPos.cp_col + 1);   // internal representation starts with 0 
	
	fprintf(filePtr, "\n# saved_cursorPos: ");
	fprintf(filePtr, "\n%d,%d", saved_cursorPos.cp_row + 1, saved_cursorPos.cp_col + 1);
	fprintf(filePtr, "\n");
	
	//  SPECIAL CHARACTERS ARRAYS  
	fprintf( filePtr, "#\n#\n#"); 
	fprintf( filePtr, "\n#==================================================\n#");
	fprintf( filePtr, "\n# Special Character Definitions:");
	fprintf( filePtr, "\n#  # of column bytes for the 8 special characters:");
	fprintf( filePtr, "\n#  1  2  3  4  5  6  7  8  ");
	fprintf( filePtr, "\n#  -  -  -  -  -  -  -  -  ");
	fprintf( filePtr, "\n   %d, %d, %d, %d, %d, %d, %d, %d ", 
			   specialCharColumns[0], specialCharColumns[1], specialCharColumns[2], specialCharColumns[3], 
			   specialCharColumns[4], specialCharColumns[5], specialCharColumns[6], specialCharColumns[7]
			   );
	fprintf( filePtr, "\n# ");
	fprintf( filePtr, "\n# SCh  numb  Data Column bytes");
	fprintf( filePtr, "\n#  #   cols  (min 5, max 8)");
	fprintf( filePtr, "\n# ---  ----  --------------------------------------\n#");
	
	countSpecChars = 0;
	for(i = 0;  i < NUMBER_OF_SPEC_CHARS;  i++)
	{
		if( (numBytes = specialCharColumns[i]) > 0 ){
			countSpecChars++;
			fprintf(filePtr, "\n   %1.1d     %1.1d   ", i+1, numBytes);
			for(j=0;  j < specialCharColumns[i] ; j++)
			{
				fprintf(filePtr, "%3.3d ", specialChars[i][j] );
			}
		}
	}
	
	if(countSpecChars == 0) {
		fprintf(filePtr, "\n# No Special Characters defined \n#\n");
	}
	else 
	{
		fprintf(filePtr, "\n#\n#");
		//   Put out a dump of the special characters as bitmaps
		//   First put put the headings of the Spec Character Numbers 1 - 8 
		outPtr = &outBuffer[0];
		for(k = 0; k < NUMBER_OF_SPEC_CHARS; k++)
		{
			if( specialCharColumns[k] > 0 )  // we have a special character for this # 
			{
				specCols = specialCharColumns[k];
				*outPtr++ = '|';
				for(i = 0; i < (specCols -1) / 2 ; i++ )
					*outPtr++ = ' ';
				if( specCols & 0x01){    // odd # of cols in Special char 
					sprintf(outPtr++, "%d", k + 1);
				}
				else {
					sprintf(outPtr, "%2d", k + 1);
					outPtr +=2;					
				}
				for(i = 0; i < (specCols - 1)/ 2 ; i++ )
					*outPtr++ = ' ';
			}
			
		}
		*outPtr++ = '\0';    // string terminator 
		fprintf(filePtr, "\n# %s| \n#",  outBuffer);
		
		
		//   Now do the bitmaps of the Special Characters 
		for(j = 0x80; j != 0;  j >>= 1 )
		{
			outPtr = &outBuffer[0];
			for(k = 0; k < NUMBER_OF_SPEC_CHARS; k++)
			{
				if( specialCharColumns[k] > 0 )  // we have a special character for this # 
				{
					*outPtr++ = '|';
					for(i = 0; i < specialCharColumns[k]; i++)
					{
						if(specialChars[k][i] & j ) // is this bit a '1' ?
							*outPtr++ = '*';
						else
							*outPtr++ = ' ';
					}
				}
			}
			
			*outPtr++ = '\0';    // string terminator 
			fprintf(filePtr, "\n# %s| ",  outBuffer);
		}
		
		fprintf(filePtr, "\n#\n");
	}

	fprintf(filePtr, "# Graphic Mode Flags: ");
	fprintf(filePtr, "\n0x%02x\n", graphicModeFlags );
	
	// wait(1);
	
	// close(fd);
	fclose(filePtr);
	
	return(OKAY);
	
	
}  // end - vt100_dumpVD() 

