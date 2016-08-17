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
 * \file vt100_convertToASCII.c
 * 
 * Convert visible ASCII input data string to true ASCII  (hex) chars
 *   (remove 0x03 or <ESC>  or <CR>  or <NL>
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


//=============================================================================
/**
 * \brief       void vt100_convertToASCII(char *inptr, char *outptr)
 *
 * \param[in]	char *inPtr - input - Visible ASCII string
 *              char *outBuf - output - true (hex) ASCII string
 *              
 * 
 * \param[out]	Description
 * 
 * \return		size of conversion
 * 
 */
int
vt100_convertToASCII ( char *inPtr, char *outBuf )
{
char  nextChar; 
static int   conversionValue = 0;
uint8_t convertedChar = 0;
char *outPtr = outBuf;

#if DEBUG_ON
	// printf("\n vt100_convertToASCII: %s  \n", inPtr);
#endif
	
	while ( (nextChar = *inPtr++)   )    // read string till end 
	{	
		if(  (nextChar == CR_CHAR)  || (nextChar == NL_CHAR) )
			continue;    //  do not move CR or LF to the output buffer
		
		if( (nextChar < ' ') || ( ((uint8_t) nextChar)) > 0x7F )    // make sure it is a visible character 
		{
			*outPtr++ = '?';
		}
		else
		{
			if( (nextChar == '0')  && ( (*inPtr == 'x')  || (*inPtr == 'X') ) )  // Hex Char 0xNN
			{				
				sscanf(inPtr+1, "%2x", &conversionValue);
				convertedChar = (uint8_t) (conversionValue & 0xFF);   
 				//  convertedChar = (*(inPtr+1) * 16) + *(inptr+2);   //   0x73 = (7*16)  + 3 
				*outPtr++ = convertedChar;
				inPtr  += 3;
			}
			else if( nextChar == BACKSLASH )   // escaping special characters <, >, and backSlash 
			{
					if( (*inPtr == '<')  || (*inPtr == '>') || (*inPtr == BACKSLASH ) )  
					{
						*outPtr++ = *inPtr++;     // move the 'escaped char out unharmed 
					}
			}
			else if (nextChar == '<' ) 
			{
#if DEBUG_ON 
				// printf("\n Converting: %s", (inPtr-1) );
#endif
					if( strncmp( (const char *) (inPtr-1), "<ESC>", (size_t) 5) == 0)
						{
							*outPtr++ = ESC_CHAR;
							inPtr += 4;
						}
					else if( strncmp( (const char *) (inPtr-1), "<NL>", (size_t) 4) == 0 ) 
					{
						*outPtr++ = NL_CHAR;
						inPtr+= 3;
					}
					else if( strncmp( (const char *) (inPtr-1), "<CR>", (size_t) 4) == 0)
					{
						*outPtr++ = CR_CHAR;
						inPtr+= 3;
					}
					else if(  strncmp( (const char *) (inPtr-1), "<TAB>", (size_t) 5) == 0)
					{
						*outPtr++ = TAB_CHAR;
						inPtr+= 4;
					}
			} 
			//else 
			//{       // ordinary character 
			//	*outPtr++ = nextChar;
			//}
		}
			
	}     //   end while 
	
	*outPtr = '\0';     // terminate the converted string 

	return (outPtr - outBuf);
	
}  // end - vt100_convertToASCII() 

