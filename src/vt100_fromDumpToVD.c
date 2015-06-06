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
 * \file vt100_fromDumpToVD.c
 * 
 * Dump the Virtual Display to a given filename 
 *
 * � Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief vt100_fromDumpToVD()
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
#include    <string.h>
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
 * \brief       int16_t vt100_fromDumpToVD(char *pFileName)
 *               Given a VT 100 dump file -read it's contents back to create
 *               the VD from scratch 
 *               
 *
 * \param[in]	pFileName - something like "dump_01.txt"
 *              
 * 
 * \param[out]	Description
 * 
 * \return		= 0  -  good 
 *              < 0  -  error number
 * 
 */
int16_t
vt100_fromDumpToVD (const char *pFileName)
{
int16_t rowNum      = 0;
int16_t colNum      = 0; 
int16_t lineCounter = 0;
static char lineBuffer[MAX_DUMP_LINE_LEN];  //  leave plenty of extra room for other messages
FILE *filePtr;
size_t rowLength = 0;
char *bufPtr;
char **pTextArray = vDisplay.pText;            // set up the malloc'd data area pointers 
char **pAttribsArray = vDisplay.pAttribs;
char *pRowText = NULL;
char *pRowAttribs = NULL;

// 	pDisplay = &vDisplay;			//  init pointer to the global VD 
	vt100_clearVD();   				//  clear the Virtual Display 

	if( (filePtr = fopen(pFileName, "r" )) == NULL)
		return( vt100_set_errorCode(ERR_01_FILE_OPEN) );
	
#if DEBUG_ON
	printf("\n vt100_fromDumpToVD: Opened file %s for VD input processing: \n", pFileName);
#endif
	
		//  process the text rows first 
	for( rowNum = 0; rowNum < numRows; rowNum++)
	{	
		//  bypass comments until we find a real row of text - comment lines start with '#'	
		while ( !feof(filePtr)  )    // read file to end 
		{
			fgets(lineBuffer, MAX_DUMP_LINE_LEN, filePtr); 
			++lineCounter;	
			if( lineBuffer[0] != '#')
			{
				break;   // must be at a text line 
			}
		}
		
		rowLength = strlen(lineBuffer);
#if DEBUG_ON
		// printf("%d: %s \n", lineCounter, lineBuffer);
		// check that the line has at least DISPLAY_COLS characters
		if( rowLength < numCols ) 
			printf("\n[txtrow %d had only %d characters instead of width of %d ]\n", 
					rowNum+1, rowLength, numCols);
#endif
		if(rowLength > numCols) 
		{
			rowLength = numCols;
		}
		
			// copy over text row to VD 
	    pRowText = pTextArray[rowNum];
		for(bufPtr = &lineBuffer[0], colNum = 0; colNum < rowLength; colNum++ )
		{
			pRowText[colNum] = *bufPtr++;
		}
		
		
	}    //  end of the text rows 
	
	
		// now do the attribute rows 
	for( rowNum = 0; rowNum < numRows; rowNum++)
	{	
		//  bypass comments until we find a real row of attributes - comment lines start with '#'	
		while ( !feof(filePtr)  )    // read file to end 
		{
			fgets(lineBuffer, MAX_DUMP_LINE_LEN, filePtr); 
			++lineCounter;	
			if( lineBuffer[0] != '#')
			{
				break;   // must be at a text line 
			}
		}
		
		rowLength = strlen(lineBuffer);
#if DEBUG_ON
		// printf("%d: %s \n", lineCounter, lineBuffer);
		// check that the line has at least DISPLAY_COLS characters
		if( rowLength < numCols ) 
			printf("\n[attrow %d had only %d characters instead of width of %d ]\n", 
					rowNum+1, rowLength, numCols);
#endif
		if(rowLength > numCols) 
		{
			rowLength = numCols;
		}
		
			// copy over attribute row to VD 
		pRowAttribs = pAttribsArray[rowNum];
		for(bufPtr = &lineBuffer[0], colNum = 0; colNum < rowLength; colNum++ )
		{
			pRowAttribs[colNum] = *bufPtr++;
		}
		
		
	}    //  end of the attribute rows 
	
	
	//  need to process the other global info - cursor pos, cursor blink, backlight, etc.
	
	// close(fd);
	fclose(filePtr);
	
	cursorPos.cp_row = numRows -1;
	cursorPos.cp_col = numCols -1;
	
	return(OKAY);
	
	
}  // end - vt100_fromDumpToVD() 

