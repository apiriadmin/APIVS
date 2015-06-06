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
 * \file vse.c
 * This file contains the definition of the \c #Validation Suite Engine (VSE)
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief VSE processor
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/

//=============================================================================
/**
 * Includes
 */
#include	<stdio.h>		// STD I/O Definitions
#include	<unistd.h>		// STD Unix Definitions
#include	<stdint.h>		// STD IEEE Type Definitions
#include	<stdlib.h>		// STD Library Definitions
#include	<string.h>		// STD String Definitions
#include	<errno.h>		// STD errno Definitions
#include	"vse.h"			// Validation Suite Engine Definitions
#include	"outputXML.h"	// Output APIOUTXML processor definitions
#include	"inputXML.h"	// Input APIVSXML processor definitions
#include	"pCode.h"		// pCode processor definitions
#include	"pInterpret.h"	// pInterpret processor definitions

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
 * Local Variables
 */

//=============================================================================
/**
 * \brief This function processes the parsed Test Suite(s)
 *
 * \param[in]	pTS - A Specific Test Suite, or NULL for all Test Suites
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
processTS(P_TS *pTS)
{
	// Process Test Suites
	if (NULL != pTS)
	{
		// Process a specific TS
		return(interpretTS(pTS));
	}
	else
	{
		// Process all TSs
		pTS = pCode_getTopTS();
		while(NULL != pTS)
		{
			if (STATUS_FAIL == interpretTS(pTS))
			{
				return(STATUS_FAIL);
			}

			pTS = pTS->pTS;		// Next TS
		}
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This is the main VSE processing function
 * This function starts by setting up the input XML file
 * and setting up the APIVSXML language interpreter.
 * Then it transfers control to the language interpreter.
 *
 * \param[in]	testSuite - The specific test suite to run, NULL if all suites
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int16_t
vse(char *testSuite)
{
	P_TS	*pTS;					// Test suite to process
	int16_t	status;					// Returned status

	// Start by parsing the APIVSXML input file
	if (STATUS_FAIL == inputXmlParse())
	{
		return(STATUS_FAIL);
	}

	outputXmlNewLine(LEVEL_SUMMARY);

	// Find the specific test suite if requested
	if (NULL != testSuite)
	{
		// A specific Test Suite is requested
		if (!(pTS = pCode_findTestSuite(testSuite)))
		{
			// Could not find Test Suite
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"vse(): Requested Test Suite [%s] not found",
					testSuite);
			OUTPUT_ERR(0, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Process a single test suite
		status = processTS(pTS);
	}
	else
	{
		// All Test Suites requested
		status = processTS(NULL);
	}

	return(status);
}
