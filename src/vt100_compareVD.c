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
 * \file vt100_compareVD.c
 * 
 *   Compares a Virtual Display text & attributes with a given filename's copy 
 *
 * \brief  vt100_compareVD() 
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
#include <unistd.h>
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
#define LINE_BUFFER_SIZE   200     // to allow for > 80 char lines 
#define LINE_LENGTH        100     // max size line for read line command 

static int16_t compareRow(const char *textRow, const char *compareString, int16_t numChars);


//=============================================================================
/**
 * \brief  vt100_compareVD(uint8_t *pFileName) - compare a file to internal VD  
 * 
 *                                            
 *
 * \params[in]	uint8_t    *pFileName    - file name string pointer 
 *        
 * \param[out]	
 *               
 * \return		0      compares OKAY  
 *              1      does not match or compare 
 *              < 0    error codes (file problems and such) 
 */

int16_t 
vt100_compareVD(const char *pFileName)
{
int16_t i=0, j=0;                             // for loops parsing strings of text, etc.
char lineBuffer[LINE_BUFFER_SIZE];            // Vir. Display (40 chars wide) allocate extra	
char *ptrSC = NULL;                           // pointer to play with special chars arrays 
int16_t numSpecChars = 0;                     // count # of defined Special Chars with non-zero bitmaps
int16_t numSpecCols  = 0;                     // number of vertical bitmap columns in this special character 
int16_t specCharNumber = 0;                   // which special character are we processing 
int valueSC = 0;                              // pull value from the array for compare 
int16_t errorSC = 0;                          // flag an SC error 
int16_t compareStage = COMPARE_TEXT;          // 8 stages of compares: 1st one: text, then attribs, tabs, parms, cursor, Spec chars
int16_t compareResult = 0;
int row = 0;                                  // for text and attribute compares 
int col = 0;                                  //   "   "       "          "
int err = 0;                                  // for file I/O error returns 
static FILE *pFile;                           // FILE Handle 
static VIRTUAL_DISPLAY *VD_ptr;               // our VIRTUAL DISPLAY 
int16_t lineCounter = 0; 
char **pTextArray = vDisplay.pText;           // set up the malloc'd data area pointers 
char **pAttribsArray = vDisplay.pAttribs;
char *pRowText = NULL;
char *pRowAttribs = NULL;
USERINT_PARAM *pOtherParms = NULL;
int   fileParmValue = 0;
char errorBuffer[100] = { 
		'\0','\0','\0','\0','\0','\0' };  	  // buffer for error string generation - put a few zero bytes to show not used yet


	VD_ptr = &vDisplay;
	
	if( (pFile = fopen(pFileName, "r" )) == NULL)
	{
		return( vt100_set_errorCode(ERR_01_FILE_OPEN) );
	}
	
	//  Concept - like a state machine with 5 stages:
	//     1.  Compare the text array 
	//     2.  Compare the attributes array   
	//     3.  Compare the Tabs array 
	//     4.  Compare the other data parameters
    //     5.  Compare the Cursor Position (row and column)
	//     6.  Compare the Special Char # of columns per SC array
	//     7.  Compare the Special Character Bitmaps per each SC
	//     8.  Compare Completed OKAY!
	//
	//pthread_mutex_lock(&vDisplay.mutex);
	usleep(11000);
	while ( compareStage < COMPARE_COMPLETE  )
	{
		    // read a line at a time to compare with the VD
		if( fgets(lineBuffer, LINE_LENGTH, pFile ) == (char *) 0 )
		{    // end of file
			break;
		}
 
		if( lineBuffer[0] == '#' )   // bypass comment lines 
		{
#if DEBUG_ON & DEBUG_PARM_MATCH
			if( compareStage == COMPARE_PARMS )
				printf("\n %s", lineBuffer);
#endif
			continue;     // bypass comments - get next line into buffer 
		}
		
		lineCounter++;   //  only counting good data lines and not comments 

#if DEBUG_ON & DEBUG_PARM_MATCH
// #if DEBUG_ON & DEBUG_SPECIAL
		printf("\n %d: %s", lineCounter, lineBuffer);
#endif
		
		if( (compareStage == COMPARE_TEXT)  || (compareStage == COMPARE_ATTRIBS) )
		{   //  Compare the Text are and the Attributes row by row first 
			// compare the file's line to the display row (text or attributes)
			if( compareStage == COMPARE_TEXT )
			{
				pRowText = pTextArray[row];
				compareResult = compareRow(pRowText, lineBuffer, numCols);
			
			}
			else   // compare stage == COMPARE_ATTRIBS
			{
				pRowAttribs = pAttribsArray[row];
				compareResult = compareRow(pRowAttribs, lineBuffer, numCols);
			}

			if( compareResult != 0 )
			{   // this row did not compare 
				vt100_set_errorCode(ERR_11_VD_MATCH_ERR);
				if(compareStage == COMPARE_TEXT){
				   sprintf(errorBuffer,"ERR_11: VD Text differs: row # %d: %s ", row, 
						   pRowText);
				}
				else {
				   sprintf(errorBuffer,"ERR_11: VD Attrib differs: row # %d: %s ", row, 
						   pRowAttribs);
				}
#if DEBUG_ON
				printf("\n %s \n", errorBuffer);
#endif
				vt100_set_errorText(errorBuffer);
				break;
			}
			
				// set up for next row if not at end of text 
			if( row == numRows - 1) 
			{   // just processed the last row - if text, switch to attribs
				if( compareStage == COMPARE_ATTRIBS )
				{   // all done processing attributes - now compare other data items 
					compareStage = COMPARE_TABS;
				}
				else 
				{  // switch over to the attributes array compare for rows & columns  
					compareStage = COMPARE_ATTRIBS;
					row = 0;
				}
			}
			else if( row  < (numRows -1))
			{  // do next row 
				row++;
			}
			else 
			{   /// should never ever hit here 
				;
#if DEBUG_ON
				printf("\n vt100_compareVD:  out of bounds error - row # %d, compareStage = %d \n",
						      row, compareStage);
#endif
			}
			
		}  //  end - text & attributes area - stages 1 & 2 
		
		else if( (compareStage > COMPARE_ATTRIBS) & (compareStage <= COMPARE_CURSOR) )
		{	       // Compare Tabs and other parameters - stages 3, 4 & 5
			if(compareStage == COMPARE_TABS)  
			{          //  compare Tabs array 
#if DEBUGON & DEBUG_TABS
				printf("\n TABS: file %s \n     : VD   %s \n", lineBuffer, tabStops);
#endif
				for(col = 0; col < numCols ; col++ )
				{
					if(lineBuffer[col] != tabStops[col])
					{
						
						vt100_set_errorCode(ERR_11_VD_MATCH_ERR);
						sprintf(errorBuffer,"ERR_11: VD TabStops Differ at col %d: %s ", col, 
									tabStops);
						vt100_set_errorText(errorBuffer);
						compareResult = 1;     // mismatch 
						break;
					}
				}
				
				if( compareResult == 0 ){   // good compare on Tabs 
					pOtherParms = vt100_get_otherParameters();
					compareStage = COMPARE_PARMS;
					
				}
				else {
					break;
				}

			}    // endif - compare Tabs
							
			else if(compareStage == COMPARE_PARMS )
			{    //  OTHER PARAMETERS COMPARE 
				 // compare the other parameters of the VD against the file values 
				if( pOtherParms->pUI_value == NULL )   // at the end of the list? 
				{
					//     compareResult = 0;      // should already be = 0 ; good result - all values compare okay
					compareStage = COMPARE_CURSOR;				}
				else
				{
					sscanf(lineBuffer, "%d", &fileParmValue);
#if DEBUG_ON & DEBUG_PARM_MATCH
					printf("\nPARAM %s: VD value: %d, file value: %d", 
							pOtherParms->pUI_name, (int) pOtherParms->pUI_value[0], (int) fileParmValue);
#endif	
					if(pOtherParms->pUI_value[0] != fileParmValue ) 
					{
						vt100_set_errorCode(ERR_11_VD_MATCH_ERR);
						sprintf(errorBuffer,"ERR_11: VD Param %s differs: VD: %d ; file value: %d:", 
								pOtherParms->pUI_name,  (int) pOtherParms->pUI_value[0], (int) fileParmValue);
						
#if DEBUG_ON & DEBUG_PARM_MATCH
						printf("\n PARAM miscompare on %s : errorBuffer: %s \n", pOtherParms->pUI_name,
								       errorBuffer);
#endif
						vt100_set_errorText(errorBuffer);
						
						compareResult = 1;        // mismatch in parameter compare 
						break;
					}
					else {
#if DEBUG_ON & DEBUG_PARM_MATCH
						printf("\n vt100_compare: VD parameter: %s value %d matches file value: %d \n", 
										pOtherParms->pUI_name, (int) pOtherParms->pUI_value[0],
										     fileParmValue);
#endif
						pOtherParms++;    // go to next parameter 
					}
					
				}  // end - else 
				
			}   // end if - compare other Parameters 
			

#if DEBUG_ON & DEBUG_PARM_MATCH
			printf("\nvt100_compare - lineCounter = %d , compareStage = %d, lineBuffer: %s ", 
					       lineCounter, compareStage, lineBuffer );
#endif
			
			
			//  go right into compare cursor because the line has already been read 
			//    and we ran out of parameters (reached the "NULL, NULL" entry! 
			if (compareStage == COMPARE_CURSOR)    
			{
				sscanf(lineBuffer, "%d,%d", &row, &col);
				if( (row  != cursorPos.cp_row +1 ) || (col != cursorPos.cp_col +1) ) 
				{
					vt100_set_errorCode(ERR_11_VD_MATCH_ERR);
					sprintf(errorBuffer,"ERR_11: VD Cursor Position differs: VD: %d,%d ; file value: %d, %d ", 
							cursorPos.cp_row + 1, cursorPos.cp_col + 1, row, col);
#if DEBUG_ON & DEBUG_PARM_MATCH
					printf("\n vt100_compareVD: %s ", errorBuffer);
					printf("\n     lineBuffer is: %s", lineBuffer);
#endif

					vt100_set_errorText(errorBuffer);

#if DEBUG_ON 
					printf("\n vt100_compareVD: get ErrorText returns: %s \n", vt100_get_errorText() );
#endif
					
					compareResult = 1;        // mismatch in parameter compare 
					break;                    // stop the insanity - get out now!
				}
				else
				{
					compareStage = COMPARE_SPEC_SIZES;  // have to absorb the "Saved Cursor Pos" line 
					                                    // not a compared value 
				}
				
			}  // endif - compare cursor position 
			
		}   // endelse - Tabs, Parms & Cursor 
		
		else if( compareStage > COMPARE_CURSOR )
		{
		
			if(compareStage == COMPARE_SPEC_SIZES )
			{
				// have to absorb the first line (saved cursor position)
				// our lines start out with a space character 
				if( lineBuffer[0] == ' ')    // first time through will see a 1,1 saved cursor and will bypass
				{
					ptrSC        = &lineBuffer[3];
					errorSC      = 0; 
					numSpecChars = 0;
				
#if  DEBUG_ON & DEBUG_SPECIAL
					printf("\n Compare Special array column sizes:  %s", lineBuffer);
#endif
					
					for(i = 0; i < NUMBER_OF_SPEC_CHARS; i++)
					{
						sscanf(ptrSC, "%1d,", &valueSC);
						if( valueSC != specialCharColumns[i] )   // are they the same size (# of vertical columns?)
						{
							errorSC = TRUE;
							vt100_set_errorCode(ERR_11_VD_MATCH_ERR);
							sprintf(errorBuffer,"ERR_11: VD SpecChar %d differs in # of cols.  VD: %d ; file value: %d", 
									i+1, specialCharColumns[i], valueSC );
						
#if DEBUG_ON & DEBUG_SPECIAL
							printf("\n vt100_compareVD: %s ", errorBuffer);
							printf("\n     lineBuffer is: %s", lineBuffer);
#endif
	
							vt100_set_errorText(errorBuffer);
							break;
						}
						else
						{
							if( specialCharColumns[i] > 0 ) {
								numSpecChars++;
							}
							ptrSC += 3;        /// move to next col value - 1 digit, 1 comma, 1 space char
							
						}
					}    // end - for loop on sizes (# of cols) 
					
					if( errorSC)
					{
						compareResult = 1;
						break;
					}
					else
					{
						if( numSpecChars > 0 ) {
							compareStage = COMPARE_SPEC_BITMAPS;
						}
						else {
							compareStage = COMPARE_GRAPHIC_MODE;
						}
							
					}
				}
#if DEBUG_ON & DEBUG_SPECIAL
				printf("\n COMPARE:  - number of special characters found = %d : %s ",
							numSpecChars, lineBuffer);
#endif
			
			
			}   // endif - COMPARE_SPEC_SIZES
			
			else if( compareStage == COMPARE_SPEC_BITMAPS )     // COMPARE the Bit-Mapped Special Chars
			{
				errorSC = FALSE;     
				specCharNumber = (int16_t) (lineBuffer[3]  - (uint8_t) 0x30);
				numSpecCols    = (int16_t) (lineBuffer[9] - (uint8_t) 0x30);
				ptrSC = &lineBuffer[13];
#if DEBUG_ON & DEBUG_SPECIAL
				printf("\n BITMAP SC # %d, # cols %d:\n line:  %s ", specCharNumber, numSpecCols, lineBuffer);
#endif	

				for(j = 0; j < numSpecCols; j++)
				{
					sscanf(ptrSC, "%d", &valueSC);
					if( valueSC != specialChars[specCharNumber - 1][j] )
					{
						errorSC = TRUE;
						vt100_set_errorCode(ERR_11_VD_MATCH_ERR);
						sprintf(errorBuffer,"ERR_11: VD SpecChar %d bitmap mismatch; Pn col# %d.  VD: %d ; file value: %d", 
								specCharNumber, j+1, specialChars[specCharNumber - 1][j], valueSC );
						
#if DEBUG_ON & DEBUG_SPECIAL
						printf("\n :::::::::::::::::::::::::::::::::::::::::::");
						printf("\n Bitmap miscompare on col # %d: err msg: %s ", j+1, errorBuffer);
						printf("\n Special Char # %d col value is %d [0x%02x] ", specCharNumber, 
								specialChars[specCharNumber - 1][j], specialChars[specCharNumber - 1][j] );
						printf("\n     lineBuffer is: %s", lineBuffer);
						printf("\n :::::::::::::::::::::::::::::::::::::::::::");
#endif

						vt100_set_errorText(errorBuffer);
						break;
					
					}
					else
					{
						ptrSC += 4;        /// move to next col value - 3 digits plus 1 space char 
						
					}
				}   // end for loop on bitmaps 
				
				if( errorSC )
				{
					compareResult = 1;
					break;
				}
				else
				{
					numSpecChars--;
					if( numSpecChars <= 0 )
					{
						compareStage = COMPARE_GRAPHIC_MODE;
					}
				}
			}
			else if (compareStage == COMPARE_GRAPHIC_MODE) {
				sscanf(lineBuffer, "%x", &fileParmValue);
				if( graphicModeFlags != fileParmValue ) {
					vt100_set_errorCode(ERR_11_VD_MATCH_ERR);
					sprintf(errorBuffer,"ERR_11: VD Graphic Mode differs: VD: %02x ; file value: %02x:", 
						(int) graphicModeFlags, (int) fileParmValue);
					vt100_set_errorText(errorBuffer);
					compareResult = 1;        // mismatch in parameter compare 
#if DEBUG_ON 
					printf("\n vt100_compareVD: get ErrorText returns: %s \n", vt100_get_errorText() );
#endif
					break;
				} else {
					compareStage = COMPARE_COMPLETE;
				}
			}
		}   // endif - compareStage > COMPARE_CURSOR  
	
	}   // end - while ( 1) 
	//pthread_mutex_unlock(&vDisplay.mutex);
	
	if( compareStage != COMPARE_COMPLETE)   // check for incomplete "compare file" without all the "stages"
	{
		if( errorBuffer[0] == '\0')         // not here because of a break on a prior error 
		{
			vt100_set_errorCode(ERR_12_COMPARE_FILE);
			sprintf(errorBuffer,"ERR_12: compare file '%s' problem - premature termination of compares.  File may be incomplete.",
					     pFileName);
			vt100_set_errorText(errorBuffer);
			compareResult = 1;        // mismatch in parameter compare 
		}
	}
	
	if( (err = fclose(pFile ))!= 0 )
	{
#if DEBUG_ON & DEBUG_FILE_IO
		printf("\n vt100_compareVD - file close error %d \n", err );
#endif
		return( vt100_set_errorCode(ERR_04_FILE_CLOSE) );
	}

	return(compareResult);
	
}   //  end  vt100_compareVD() 
	
//=============================================================================
/**
 * \brief  compareRow( textRow, compareString, numChars )  
 * 
 *                                            
 *
 * \params[in]	char       *textRow       - pointer to row in the array 
 *              char       *compareString - one line from the file 
 *              int16_t     numChars      - number of chars to compare 
 *        
 * \param[out]	
 *               
 * \return		0 if okay, 1 (non-zero) if miscompare 
 */
static int16_t
compareRow(const char *textRow, const char *compareString, int16_t numChars)
{
int16_t result = 0;
int16_t col = 0;

	for (col = 0; col < numChars; col++ ){ 
//		if(textRow[col] != compareString[col])  return ( 1 ); 
		if(*textRow++ != *compareString++ )   // optimization 
		{
			result = 1;  // fails compare  
			break;
		}
	}
	
	return(result);    	 // = 0 if compare was okay, = 1 if compare failed 
	
}   // end - compareRow() 
