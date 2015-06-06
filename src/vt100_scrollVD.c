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
 * \file vt100_scrollVD.c
 * 
 *   Scroll the Virtual Display up or down by 1 line 
 *
 * � Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief  vt100_scrollVD() 
 * 
 * \date   12/26/2010 
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
extern int16_t numRows;
extern int16_t numCols;

//=============================================================================
/**
 * Local Defines
 */

//=============================================================================
/**
 * \brief  vt100_scrollVD(int16_t scrollUp) - scroll up (+) or down (-) 1 Lines 
 *      Note: could scroll multiple lines if we saved the whole pointer arrays first
 *            Also - we could use the saved pointers to do a wrap around type buffer,
 *            but we can not use the saved pointers to restore previous text since 
 *            these are overwritten with blanks.  If we wanted to be able to "undo"
 *            a scroll up or down, we'd have to allocate text & attrib line buffers
 *            and save the line or lines that would be overwritten.
 * 
 *
 * \params[in]	int16_t    upDownFlag -  scroll up (+) or down(-): use 1 or -1
 *        
 * \param[out]	
 *               
 * \return		none
 * 
 */

void
vt100_scrollVD(int16_t scrollUp )
{
char **pTextArray = vDisplay.pText;         // set up the malloc'd data area pointers 
char **pAttribsArray = vDisplay.pAttribs;
char *pSaveRowText = NULL;                  // have to save current text & attrib pointers  
char *pSaveRowAttribs = NULL;               //     while shifting others onto current place 
int16_t i=0, j=0;                           // loop counters for rows(i) and columns (j)
int16_t scrollFlag = TRUE;					// did we scroll okay?

//   Concept - since the Virtual Display is an array of pointers to dynamically allocated
//      memory, we will just move pointers around and clear the new line at the top or 
//      bottom of the Virtual Display.  
//   NOTE:   The CursorPosition is not changed by this action.
//
	
	
	if( scrollUp == 1)
	{   //   scroll up  (leave a bottom line open & clear it)
		pSaveRowText = pTextArray[0];
		pSaveRowAttribs = pAttribsArray[0];
		for( i = 1;  i < numRows; i++ )
		{
			pTextArray[i-1] = pTextArray[i];
			pAttribsArray[i-1] = pAttribsArray[i];
		}
		pTextArray[numRows - 1] = pSaveRowText;
		pAttribsArray[numRows - 1] = pSaveRowAttribs;
		// now clear the new row at the bottom 
	}
	else if( scrollUp == -1) 
	{   // scroll Down - leave an open line at the top  
		pSaveRowText = pTextArray[numRows - 1];
		pSaveRowAttribs = pAttribsArray[numRows - 1];
		for( i = numRows - 1;  i > 0; i-- )
		{
			pTextArray[i] = pTextArray[i-1];
			pAttribsArray[i] = pAttribsArray[i-1];
		}
		pTextArray[0] = pSaveRowText;
		pAttribsArray[0] = pSaveRowAttribs;
		// now clear the new row at the top 
	}
	else {
		scrollFlag = FALSE;
	}


	// if we actually scrolled, then do the line clearing
	if ( scrollFlag )    
	{
	//  now clear the new row's text and attributes which are
	//   still being pointed to by the saved Text & Attribs pointers
		for( j= 0; j< numCols; j++ )
		{
			pSaveRowText[j] = ' ';
			pSaveRowAttribs[j] = '0';
		}
	}	

	return;
	
}   //  end  vt100_scrollVD() 
	
