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
 * \file vt100_clearVD.c
 * 
 *   Clears the internal copy of the Virtual Display text & attributes 
 *
 * \brief  vt100_clearDisplay() 
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

extern VIRTUAL_DISPLAY vDisplay;

//=============================================================================
/**
 * Local Defines
 */


//=============================================================================
/**
 * \brief  vt100_clearDisplay(VIRTUAL_DISPLAY *VD_Ptr) - clear a Virtual Display  
 *             NOTE:  This does not affect the CURSOR_POSITION 
 *                    Cursor remains where it was!
 * 
 *                                            
 *
 * \params[in]	VIRTUAL_DISPLAY *VD_ptr - pointer to a Virtual Display 
 *        
 * \param[out]	
 *               
 * \return		nothing 
 */

void 
vt100_clearVD()
{
int16_t row = 0;
int16_t col = 0;
char **pTextArray = vDisplay.pText;
char **pAttribsArray = vDisplay.pAttribs;
char *pRowText = NULL;
char *pRowAttribs = NULL;

#if DEBUG_ON
	printf("\n vt100_clearDisplay \n");
#endif

	for ( row = 0; row < numRows;  row++ ) {
		pRowText = pTextArray[row];
		pRowAttribs = pAttribsArray[row];
		for (col = 0; col < numCols; col++ ){ 
			*pRowText++    = ' ';     // set to blanks for Text and 
			*pRowAttribs++ = '0';	  //  ASCII Zeroes for the attributes 	
		}
	}
	
	return;
	
}  // end - vt100_clearVD() 
