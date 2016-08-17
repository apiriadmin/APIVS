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
 * \file vt100_noFPUI.c
 * 
 * Routines to support functioning without the FPUI connected 
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
#include    <string.h>
#include    "vt100.h"



//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */

#define LINE_BUFF_SIZE (DEFAULT_DISPLAY_COLS * 5)

static char vtInBuffer[LINE_BUFF_SIZE * 2];   // converted from file 


//=============================================================================
/**
 * \brief       int16_t vt100_fileToVD(char *pFileName)
 *                send a Human Readable file format to VD (convert to ASCII/HEX 
 *                bytes and send to the Virtual Display)
 *
 * \param[in]	pFileName - something like "screen1.input"
 *              
 * 
 * \param[out]	Description
 * 
 * \return		= 0  -  good 
 *              < 0  -  error number
 * 
 */
int16_t
vt100_fileToVD (const char *pFileName)
{
static char lineBuffer[LINE_BUFF_SIZE];     //  leave plenty of extra room for other messages
VIRTUAL_DISPLAY *pDisplay = &vDisplay;   // 
FILE *filePtr;
int16_t lineCounter = 0;

	if( (filePtr = fopen(pFileName, "r" )) == NULL)
		return( vt100_set_errorCode(ERR_01_FILE_OPEN) );

#if DEBUG_ON
	printf("\n vt100_fileToVD: Opened file %s for VD input processing: \n", pFileName);
#endif
	
	while ( fgets(lineBuffer, LINE_BUFF_SIZE, filePtr) != NULL )    // read file to end 
	{	
		++lineCounter;	
		
		if( lineBuffer[0] != '#')   // only process data lines, not comments
		{
			int ascii_len = vt100_convertToASCII(lineBuffer, vtInBuffer);   // convert from visible ASCII to straight ASCII		
		
#if DEBUG_ON & DEBUG_LINE
			printf("%03d: %d : %s \n Out: ", lineCounter, strlen(lineBuffer), lineBuffer);
			int i, j;              // print Hex output 
			for(j=0; j < 4; j++ ){
				for(i=0; i < 32 ; i++ )
					if( vtInBuffer[(j*32)+i] ){
						if( i == 16 ) printf("    ");
						printf("%02x ", vtInBuffer[(j*32)+i]);   // show the hex???
					}
					else
						break;
				if(!vtInBuffer[(j*32)+i]) break;
				printf("\n      ");
			}
				 
#endif
		
			//vt100_processInput( (int16_t) 0 , vtInBuffer, pDisplay );    // no fd available - sending line by line 
			// process input line by line to Virtual Display 
                        if (fdSerialPort > 0) {
                                writeSerialPort(fdSerialPort, vtInBuffer, ascii_len); 
                        }
		}
				
	}   // end for()  - moving text out to buffer
	
	fclose(filePtr);
	
	return(OKAY);
	
	
}  // end - vt100_fileToVD() 

//=============================================================================
/**
 * \brief       void vt100_stringToVD(char *pString )
 *                given a string pointer; process the 
 *                characters out to the Virtual Display
 *
 * \param[in]	pString  - ASCII String to send to VT 
 *              
 * 
 * \param[out]	Description
 * 
 * \return		nothing  
 *             
 * 
 */
void
vt100_stringToVD (char *pString )
{
VIRTUAL_DISPLAY *pDisplay = &vDisplay;   // 


			// force this to process the string by setting fd = 0 
		vt100_processInput( (int16_t) 0 , pString, pDisplay );     
		
		return;
	
	
}  // end - vt100_stringToVD() 


//=============================================================================
/**
 * \brief       void  vt100_vtInputBuffer(char *returnBuffer, int16_t bufferLen)
 *                returns the converted ASCII from the vt100_fileToVD()
 *
 * \param[in]	char * ptr to buffer for converted string 
 * 
 *              
 * 
 * \param[out]	Description
 * 
 * \return		= 0  -  good 
 *              < 0  -  error number
 * 
 */

void
vt100_vtInputBuffer(char *returnBuffer, int16_t bufferLen)
{
char *toPtr;
char *fromPtr;
int16_t downCounter = bufferLen;

	toPtr   = (char *) returnBuffer;
	fromPtr = vtInBuffer;
	
	while (downCounter--)
	{
		if( (*toPtr++ = *fromPtr++) == '\0' )   // string terminator?
		{
			break;      // all done
		}
	}
	
	//  always jam a '\0' byte terminator at the end of the buffer
	
	returnBuffer[bufferLen - 1] = '\0';
 
	
	return;
	
	
}  // end - vt100_vtInputBuffer() 

