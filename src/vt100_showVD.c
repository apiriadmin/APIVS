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
 * \file vt100_showVD.c
 *  Output the Virtual Display - text & attributes - to the screen
 * NOTE:  This routine must be exactly synchronized with vt100_dumpVD()
 *          and vt100_fromDumpToVD()
 *
 * � Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief vt100_showVD(char * commentText) 
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
#include    <ctype.h>        // for the isprint() macro/function
#include    "vt100.h"

//=============================================================================
/**
 * Extern
 */

//==================================Text===========================================
/**
 * Local Defines
 */


//=============================================================================
/**
 * \brief       void vt100_showVD()  - output the VD to console display 
 *
 * \param[in]	FILE *pFile         - pointer to file or stdout for screen prints
 *              char *commentText   - optional comment to appear in heading or ""
 * 
 * \param[out]	Description
 * 
 * \return		None 
 * 
 * 
 */
void
vt100_showVD (FILE *pFile, const char *commentText)
{
// int16_t fd = 0;
int16_t i = 0, j=0, k=0;                       // loop counters
int16_t numBytes = 0;                          // num of bytes per Special Character (variable: 5, 6, 7, or 8 bytes)
// char commentBuffer [82];
char lineBuffer[DEFAULT_DISPLAY_COLS * 4];     // temp - should be malloc'd with numCols + 5 to 10 chars for EOL stuff
char outBuffer[DEFAULT_DISPLAY_COLS * 4];      // for Special Character disolay
char *outPtr = outBuffer;
int16_t specCols = 0;  	                       // # of columns in this special character 
const char *commentPtr = " ";
char tempChar = ' ';
int16_t countSpecChars = 0;                    // count the actual number of special characters defined correctly
time_t rawtime;
char **pTextArray    = vDisplay.pText;         // set up the malloc'd data area pointers 
char **pAttribsArray = vDisplay.pAttribs;
char *pRowText       = NULL;                   // local pointer variables for looping data moves 
char *pRowAttribs    = NULL;                   //   "
char *p_tabStops     = NULL;			       //   "

	if( commentText != (char *)NULL )
	{
		commentPtr = commentText;
	}
	
		//   get current date and time and put into the file heading 
	// time ( &rawtime );
	strftime(lineBuffer,30 , "%Y%m%d %H:%M:%S",localtime(&rawtime));
	fprintf(pFile,"\n -- Virtual Display and Global Variable Dump -- \n");
	fprintf(pFile,"\n# Date: %s - %s   \n#", lineBuffer, commentPtr);
	fprintf(pFile,"\n Display Rows\n0        ");
	// fprintf(pFile,"\n0        1         2         3         4");
	// fprintf(pFile,"\n1234567890123456789012345678901234567890\n");
	//  12/20/2010 - change to dynamic line length headers 
	for(j = 10; j <= numCols;  j += 10){
		fprintf(pFile,"%d         ", j/10 );
	}
	fprintf(pFile,"\n123456789");
	for(j = 10; j <= numCols;  j += 10){
		fprintf(pFile,"0123456789");
	}
	fprintf(pFile,"\n");
 
	//   TEXT DISPLAY 
	for( i = 0; i < numRows; i++)
	{	
		// move display to Line Buffer - one line at a time, and check the characters 
	    pRowText = pTextArray[i];

		for( j=0; j < numCols ; j++ )
		{
			tempChar = pRowText[j];
			if(!isprint(tempChar)) tempChar = '?';
			lineBuffer[j] = tempChar;
		}
		lineBuffer[j++] = '\n';    //  put end of line and string terminator
		lineBuffer[j++] = '\0'; 
		
		//    sfprintf(pFile,lineBuffer, "%40.40s\n", vDisplay.text[i]);
		// length = strlen(lineBuffer);
		// bytesWritten = write(fd, lineBuffer, length);
		fprintf(pFile,"%s", lineBuffer);

		/***********************
#if DEBUG_ON
		fprintf(pFile,"\n writing row # %d:  %d text characters attempted,  %d written \n", i+1, length, bytesWritten);
#endif
		if( bytesWritten <= 0 )
		{
			fprintf(pFile,"\nFile Write error # %d \n", bytesWritten );
		}
		***********************/
	}
	// put a solid delimiting line under the text to be sure where it ends
	fprintf(pFile,"------------------------------------------------------------");

	
	fprintf(pFile,"\n Attribute Rows\n0        ");
	// fprintf(pFile,"\n0        1         2         3         4");
	// fprintf(pFile,"\n1234567890123456789012345678901234567890\n");
	for(j = 10; j <= numCols;  j += 10){
		fprintf(pFile,"%d         ", j/10 );
	}
	fprintf(pFile,"\n123456789");
	for(j = 10; j <= numCols;  j += 10){
		fprintf(pFile,"0123456789");
	}
	fprintf(pFile,"\n");
 
	//   ATTRIBUTE DISPLAY 
	for( i = 0; i < numRows; i++)
	{	
	
		// move attributes to Line Buffer 1 line at a time - check the characters 
		pRowAttribs = pAttribsArray[i];
		for( j=0; j < numCols ; j++ ){
			tempChar = pRowAttribs[j];
			if(!isprint(tempChar)) tempChar = '?';
			lineBuffer[j] = tempChar;
		}
		lineBuffer[j++] = '\n';    //  put end of line and string terminator
		lineBuffer[j++] = '\0'; 
		
		//   sfprintf(pFile,lineBuffer, "%40.40s\n", vDisplay.text[i]);
		// length = strlen(lineBuffer);
		// bytesWritten = write(fd, lineBuffer, length);
		fprintf(pFile,"%s", lineBuffer);
	
	/***********
#if DEBUG_ON
	fprintf(pFile,"\n writing row # %d:  %d attrib characters attempted,  %d written \n", i+1, length, bytesWritten);
#endif
	
	if( bytesWritten <= 0 )
	{
		fprintf(pFile,"\nFile Write error # %d \n", bytesWritten );
	}
	*************************/
	}
	
	// put a solid delimiting line under the attributes to be sure where they end
	fprintf(pFile,"------------------------------------------------------------");
	
	
	// TAB STOPS:
	fprintf(pFile,"\n Tab Stops array: \n0        ");
	// fprintf(pFile,"\n0        1         2         3         4");
	// fprintf(pFile,"\n1234567890123456789012345678901234567890\n");
	for(j = 10; j <= numCols;  j += 10){
		fprintf(pFile,"%d         ", j/10 );
	}
	fprintf(pFile,"\n123456789");
	for(j = 10; j <= numCols;  j += 10){
		fprintf(pFile,"0123456789");
	}
	fprintf(pFile,"\n");
 
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
	
	//   sfprintf(pFile,lineBuffer, "%40.40s\n", vDisplay.text[i]);
	// length = strlen(lineBuffer);
	// bytesWritten = write(fd, lineBuffer, length);
	fprintf(pFile,"%s", lineBuffer);
	
	/***********
#if DEBUG_ON
	fprintf(pFile,"\n writing row # %d:  %d attrib characters attempted,  %d written \n", i+1, length, bytesWritten);
#endif
	
	if( bytesWritten <= 0 )
	{
		fprintf(pFile,"\nFile Write error # %d \n", bytesWritten );
	}
	*************************/
	
	
	fprintf(pFile,"\n#===========================================================\n");
	fprintf(pFile,"\n Global Variables:\n");
	// fprintf(pFile,"backLight %d,  autoWrap %d, autoScroll %d, autoRepeat %d,\n Cursor Pos %d:%02d, Blink %d \n\n",
	// 		   vt100_get_backLight(), vt100_get_autoWrap(), vt100_get_autoScroll(), vt100_get_autoRepeat(),
	// 		   cursorPos.cp_row, cursorPos.cp_col, vt100_get_cursorBlink() );
	
	fprintf(pFile, "\n autoWrap: ");
	fprintf(pFile, "\n%d", vt100_get_autoWrap() );
	
	fprintf(pFile, "\n autoRepeat: ");
	fprintf(pFile, "\n%d", vt100_get_autoRepeat() );
	
	fprintf(pFile, "\n autoScroll: ");
	fprintf(pFile, "\n%d", vt100_get_autoScroll() );
	
	fprintf(pFile, "\n backLight: ");
	fprintf(pFile, "\n%d", vt100_get_backLight() );
	
	fprintf(pFile, "\n backLightTimeout: ");
	fprintf(pFile, "\n%d", vt100_get_backLightTimeout() );
	
	fprintf(pFile, "\n cursorState: ");
	fprintf(pFile, "\n%d", vt100_get_cursorState() );
	
	fprintf(pFile, "\n cursorBlink: ");
	fprintf(pFile, "\n%d", vt100_get_cursorBlink() );
	
	fprintf(pFile, "\n reverseVideo: ");
	fprintf(pFile, "\n%d", vt100_get_autoWrap() );
	
	fprintf(pFile, "\n auxSwitch: ");
	fprintf(pFile, "\n%d", vt100_get_auxSwitch() );
	
	fprintf(pFile, "\n_cursorPos: ");
	fprintf(pFile, "\n%d,%d", cursorPos.cp_row + 1, cursorPos.cp_col + 1);    // internal representation starts with 0 
	
	fprintf(pFile, "\n saved_cursorPos: ");
	fprintf(pFile, "\n%d,%d", saved_cursorPos.cp_row + 1, saved_cursorPos.cp_col + 1);
	fprintf(pFile, "\n");
	
	
	fprintf(pFile,"\n#===========================================================\n");
	fprintf(pFile,"\n SPECIAL Characters Definitions:\n");
	
	countSpecChars = 0;
	for(i = 0;  i < NUMBER_OF_SPEC_CHARS;  i++)
	{
		if( (numBytes = specialCharColumns[i]) > 0 ){
			countSpecChars++;
			fprintf(pFile, "\n  # %d: %d columns: ", i+1, numBytes);
			for(j=0;  j < specialCharColumns[i] ; j++)
			{
				fprintf(pFile, "%3.3d ", specialChars[i][j] );
			}
		}
	}
	
	if(countSpecChars == 0) 
	{
		fprintf(pFile,"\n No Special Characters defined yet \n");
	}
	else 
	{
		fprintf(pFile, "\n\n");
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
		fprintf(pFile, "\n# %s| \n#",  outBuffer);
		
		
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
			fprintf(pFile, "\n# %s| ",  outBuffer);
		}
		
		fprintf(pFile,"\n\n");
	}
	
	// Graphic Mode Flags
	fprintf(pFile,"\n#===========================================================\n");
	fprintf(pFile, "\nGraphic Mode Flags:\n");
	fprintf(pFile, "0x%02x\n\n", graphicModeFlags );

	return;
	
	
}  // end - vt100_showVD() 
