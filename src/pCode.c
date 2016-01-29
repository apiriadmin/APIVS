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
 * \file pCode.c
 *
 * This file contains the definition of functions for processing the APIVSXML
 * pCode (parsing only)
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief pCode XML processing
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
#include	"vse_stnd.h"	// STD VSE Definitions
#include	"outputXML.h"	// Output XML Processor Definitions
#include	"pCode.h"		// pCode Processor Definitions

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
/** \brief TOP Element definition */
static	P_TOP	s_pTop;

/** \brief Pointer to active TS being parsed */
static	P_TS	*s_pTSActive = NULL;

/** \brief Current nesting depth */
static	uint16_t	s_nestingDepth = 0;

/** \brief Pointer where to link next P_CODE */
static	P_CODE	**s_ppCode[MAX_NESTING];
static	P_CODE	**s_ppCodeSave[MAX_NESTING];

/** \brief Pointer to current pCode being parsed */
static	P_CODE	*s_pCodeSave[MAX_NESTING];

/** \brief Pointer where to link Then P_CODE */
static	P_CODE	**s_ppThen[MAX_NESTING];

/** \brief Pointer where to link Else P_CODE */
static	P_CODE	**s_ppElse[MAX_NESTING];

/** \brief Point to current text buffer */
static	char	**s_ppText = NULL;

//=============================================================================
/**
 * \brief This function returns the pointer to the TS linked listed
 *
 * \return		NULL if no Test Suites
 * 				otherwise, first Test Suite in linked list
 *
 */
P_TS *
pCode_getTopTS()
{
	return(s_pTop.pTS);		// Return point to Test Suite list
}

//=============================================================================
/**
 * \brief This function returns a pointer to the named testSuite
 *
 * \param[in]	testSuite - Pointer to name of testSuite to find
 * \return		NULL if testSuite not found
 * 				otherwise, pointer to the named testSuite
 *
 */
P_TS *
pCode_findTestSuite(const char *testSuite)
{
	P_TS	*pTS = s_pTop.pTS;
	while (NULL != pTS)
	{
		if (!strcmp(testSuite, pTS->common.pName))
		{
			// This TS matched
			break;
		}

		// This TS did not match
		pTS = pTS->pTS;		// Advance to next entry in list
	}

	return(pTS);			// Return point to found entry
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_TESTSUITE> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseTestSuiteOpen(XML_Parser pParser, const char **pAttr)
{
	P_TS		*pTS;			// Pointer for the new Test Suite
	P_TS		*pTS_next;		// Pointer to next TS
	P_SU		*pSU;			// Pointer to SetUp
	P_TD		*pTD;			// Pointer to TearDown
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		nameFound = FALSE;	// Name attribute is required

	// Process a Test Suite
	if ((0 != s_nestingDepth) || (NULL != s_ppCode[s_nestingDepth]))
	{
		// We aren't at the top level
		// This is a problem
		// Can only be defined at Top Level
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseTestSuiteOpen(): Can only be defined at top level",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the TS instance
	if (!(pTS = malloc(sizeof(P_TS))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseTestSuiteOpen(): Failed to malloc space for TS size [%d]",
				sizeof(P_TS));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pTS, 0, sizeof(P_TS));

	// Save the line number
	pTS->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	
	// Show as the Active TS
	s_pTSActive = pTS;

	// Link it into the TS List
	pTS_next = s_pTop.pTS;
	if (NULL == pTS_next)
	{
		// This is the first TS in the list
		s_pTop.pTS = pTS;
	}
	else
	{
		while (pTS_next->pTS)
		{
			pTS_next = pTS_next->pTS;
		}

		pTS_next->pTS = pTS;
	}

	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_NAME))
		{
			// ATTR: Name
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestSuiteOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_NAME,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pTS->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pTS->common.pName = ptr;

			// Make sure this name is unique
			pTS_next = s_pTop.pTS;
			while (pTS_next != pTS)
			{
				if (!strcmp(ptr, pTS_next->common.pName))
				{
					// We found the name, not unique
					char	string[OUTPUT_STRING_MAX];

					sprintf(string,
							"pCode_parseTestSuiteOpen(): Test Suite name [%s] is not unique",
							ptr);
					OUTPUT_ERR(pTS->common.lineNumber, string, NULL, NULL);
					return(STATUS_FAIL);
				}
				pTS_next = pTS_next->pTS;
			}

			nameFound = TRUE;	// Show name attribute found and processed
		}
		else if (!strcmp(pAttr_name, A_SETUP))
		{
			// ATTR: TestSuite SetUp
			// Make sure this reference is found
			pSU = s_pTop.pSU;
			while (NULL != pSU)
			{
				if (!strcmp(pSU->common.pName, pAttr_value))
				{
					// We found the referenced SetUp
					break;
				}
				pSU = pSU->pSU;
			}

			if (NULL == pSU)
			{
				// We did not find referenced Set Up
				char string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestSuiteOpen(): Referenced Set Up [%s] not defined",
						pAttr_value);
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   NULL,
						   NULL);
						   
				return(STATUS_FAIL);
			}

			// Link the found Set Up into TS
			pTS->pSU = pSU;
		}
		else if (!strcmp(pAttr_name, A_TEARDOWN))
		{
			// ATTR: TestSuite TearDown
			// Make sure this reference is found
			pTD = s_pTop.pTD;
			while (NULL != pTD)
			{
				if (!strcmp(pTD->common.pName, pAttr_value))
				{
					// We found the referenced Tear Down
					break;
				}
				pTD = pTD->pTD;
			}

			if (NULL == pTD)
			{
				// We did not find referenced Set Up
				char string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestSuiteOpen(): Referenced Tear Down [%s] not defined",
						pAttr_value);
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   NULL,
						   NULL);
						   
				return(STATUS_FAIL);
			}

			// Link the found Set Up into TS
			pTS->pTD = pTD;
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestSuiteOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pTS->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pTS->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseTestSuiteOpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pTS->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present
	if (!nameFound)
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseTestSuiteOpen(): Required attribute[%s] not found",
				A_NAME);
		OUTPUT_ERR(pTS->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_TESTSUITE> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseTestSuiteClose(XML_Parser pParser)
{
	s_pTSActive = NULL;	// Show no TS active
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_TESTCASE> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseTestCaseOpen(XML_Parser pParser, const char **pAttr)
{
	P_TC		*pTC;			// Pointer for the new Test Case
	P_TC		*pTC_next;		// Pointer to next TC
	P_SU		*pSU;			// Pointer to SetUp
	P_TD		*pTD;			// Pointer to TearDown
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		nameFound = FALSE;	// Name attribute is required

	// Process a Test Case
	if ((0 != s_nestingDepth) || (NULL != s_ppCode[s_nestingDepth]))
	{
		// We aren't at the top level
		// This is a problem
		// TC can only be defined at Top Level
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseTestCaseOpen(): Can only be defined at top level",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// First malloc the TC instance
	if (!(pTC = malloc(sizeof(P_TC))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseTestCaseOpen(): Failed to malloc space for TC size [%d]",
				sizeof(P_TC));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pTC, 0, sizeof(P_TC));

	// Save the line number
	pTC->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	
	// Link it into the TC List
	pTC_next = s_pTop.pTC;
	if (NULL == pTC_next)
	{
		// This is the first TC in the list
		s_pTop.pTC = pTC;
	}
	else
	{
		while (pTC_next->pTC)
		{
			pTC_next = pTC_next->pTC;
		}

		pTC_next->pTC = pTC;
	}

	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_NAME))
		{
			// ATTR: Name
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestCaseOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_NAME,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pTC->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pTC->common.pName = ptr;

			// Make sure this name is unique
			pTC_next = s_pTop.pTC;
			while (pTC_next != pTC)
			{
				if (!strcmp(ptr, pTC_next->common.pName))
				{
					// We found the name, not unique
					char	string[OUTPUT_STRING_MAX];

					sprintf(string,
							"pCode_parseTestCaseOpen(): Test Case name [%s] is not unique",
							ptr);
					OUTPUT_ERR(pTC->common.lineNumber, string, NULL, NULL);
					return(STATUS_FAIL);
				}
				pTC_next = pTC_next->pTC;
			}

			nameFound = TRUE;	// Show name attribute found and processed
		}
		else if (!strcmp(pAttr_name, A_SETUP))
		{
			// ATTR: TestCase SetUp
			// Make sure this reference is found
			pSU = s_pTop.pSU;
			while (NULL != pSU)
			{
				if (!strcmp(pSU->common.pName, pAttr_value))
				{
					// We found the referenced Test Case
					break;
				}
				pSU = pSU->pSU;
			}

			if (NULL == pSU)
			{
				// We did not find referenced Set Up
				char string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestCaseOpen(): Referenced Set Up [%s] not defined",
						pAttr_value);
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   NULL,
						   NULL);
						   
				return(STATUS_FAIL);
			}

			// Link the found Set Up into TC
			pTC->pSU = pSU;
		}
		else if (!strcmp(pAttr_name, A_TEARDOWN))
		{
			// ATTR: TestCase TearDown
			// Make sure this reference is found
			pTD = s_pTop.pTD;
			while (NULL != pTD)
			{
				if (!strcmp(pTD->common.pName, pAttr_value))
				{
					// We found the referenced Tear Down
					break;
				}
				pTD = pTD->pTD;
			}

			if (NULL == pTD)
			{
				// We did not find referenced Set Up
				char string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestCaseOpen(): Referenced Tear Down [%s] not defined",
						pAttr_value);
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   NULL,
						   NULL);
						   
				return(STATUS_FAIL);
			}

			// Link the found Set Up into TC
			pTC->pTD = pTD;
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestCaseOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pTC->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pTC->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseTestCaseOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pTC->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present
	if (!nameFound)
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseTestCaseOpen(): Required attribute[%s] not found",
				A_NAME);
		OUTPUT_ERR(pTC->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	// Show where to start linking code
	s_ppCode[s_nestingDepth] = &pTC->pCode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_TESTCASE> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseTestCaseClose(XML_Parser pParser)
{
	s_ppCode[s_nestingDepth] = NULL;	// Show done with P_CODE

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_TESTCASEREF> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseTestCaseRefOpen(XML_Parser pParser, const char **pAttr)
{
	P_TC		*pTC = NULL;	// Pointer for the referenced Test Case
	P_TC		*pTC_new;		// For mallocing TC
	P_TC		*pTC_next;		// Next TC in TS TC list
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	boolean		refFound = FALSE;	// Required attribute found flag

	// Process a Test Case Reference
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_REF))
		{
			// ATTR: Ref
			refFound = TRUE;

			// Make sure this reference is found
			pTC = s_pTop.pTC;
			while (NULL != pTC)
			{
				if (!strcmp(pTC->common.pName, pAttr_value))
				{
					// We found the referenced Test Case
					break;
				}
				pTC = pTC->pTC;
			}

			if (NULL == pTC)
			{
				// We did not find referenced Test Case
				char string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestCaseRefOpen(): Referenced Test Case [%s] not defined",
						pAttr_value);
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   NULL,
						   NULL);
						   
				return(STATUS_FAIL);
			}

			// We found the test case, now link it into the Test Suite
			// Test Case List
			if (NULL == s_pTSActive)
			{
				// There is no Test Suite being processed, error
				char string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestCaseRefOpen(): No Test Suite active for referenced Test Case [%s]",
						pAttr_value);
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}

			// Link at end of TC list as a new TC
			// We need to make a copy because this single
			// Test Case could be referenced by multiple Test Suites
			// And the TC linkage needs to be specific to this TS
			// Just copy as a whole, since all of the pointers
			// can be used as is.
			if (!(pTC_new = malloc(sizeof(P_TC))))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTestCaseRefOpen(): Failed to malloc space for TC size [%d]",
						sizeof(P_TC));
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			// Copy found TC into new buffer
			memcpy(pTC_new, pTC, sizeof(P_TC));

			// Finally link it in!
			// to the Active test suite
			pTC_next = s_pTSActive->pTC;
			if (NULL == pTC_next)
			{
				s_pTSActive->pTC = pTC_new;	// Head of list
			}
			else
			{
				while (NULL != pTC_next->pTC)
				{
					pTC_next = pTC_next->pTC;
				}
				pTC_next->pTC = pTC_new;
			}
			pTC_new->pTC = NULL;		// New tail
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseTestCaseRefOpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
					   string,
					   NULL,
					   NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present and processed
	if (!refFound)
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseTestCaseRefOpen(): Required attribute[%s] not found",
				A_REF);
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_TESTCASEREF> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseTestCaseRefClose(XML_Parser pParser)
{
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_PRINT> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parsePrintOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer for the new P_CODE
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs

	// Process a Print
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parsePrintOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parsePrintOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parsePrintOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_PRINT;
	pCode->code.cPrint.level = LEVEL_SUMMARY;	// Default level of print
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parsePrintOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else if (!strcmp(pAttr_name, A_LEVEL))
		{
			if (!strcmp(pAttr_value, LVL_0))
			{
				pCode->code.cPrint.level = LEVEL_ALWAYS;
			}
			else if (!strcmp(pAttr_value, LVL_1))
			{
				pCode->code.cPrint.level = LEVEL_CONFORMANCE;
			}
			else if (!strcmp(pAttr_value, LVL_2))
			{
				pCode->code.cPrint.level = LEVEL_SUMMARY;
			}
			else if (!strcmp(pAttr_value, LVL_3))
			{
				pCode->code.cPrint.level = LEVEL_TRACE;
			}
			else
			{
				// Unknown level
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parsePrintOpen(): Failed to parse %s=%s, unknown value",
						A_LEVEL,
						pAttr_value);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parsePrintOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	s_ppText = &pCode->code.cPrint.pText;	// Set up for text of Print
	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_PRINT> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parsePrintClose(XML_Parser pParser)
{
	s_ppText = NULL;							// Show text no longer allowed
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_DEFINE> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseDefineOpen(XML_Parser pParser, const char **pAttr)
{
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	boolean		varFound = FALSE;	// var is required
	boolean		typeFound = FALSE;	// varType is required
	VAR_TYPE	varType = 0;	// Parsed varType
	uint16_t	size = DEFAULT_SIZE;	// Size of array if needed
	const char	*pSizeGiven = NULL;		// Was size provided
	ARG_P		*sizeArg;		// For dereferencing the size
	P_COMMON	common;			// Common information
	common.pName = NULL;		// Parsed variable name
	common.pDesc = NULL;		// Parsed description

	// Process a Define
	// Not linked in, define a variable RIGHT NOW!
	
	// Save the line number
	common.lineNumber = XML_GetCurrentLineNumber(pParser);
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_VAR))
		{
			// ATTR: Var
			// Parse the variable
			if (STATUS_FAIL == argValidateVarSyntax(common.lineNumber,
													pAttr_value))
			{
				// Bad syntax
				return(STATUS_FAIL);
			}

			common.pName = pAttr_value;
			varFound = TRUE;
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			common.pDesc = pAttr_value;
		}
		else if (!strcmp(pAttr_name, A_TYPE))
		{
			// ATTR: type
			if (STATUS_FAIL == argParseType(pAttr_value, &varType))
			{
				// Unknown variable type
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseDefineOpen(): Failed to parse [%s=%s], unknown value",
						A_TYPE,
						pAttr_value);
				OUTPUT_ERR(common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}

			typeFound = TRUE;
		}
		else if (!strcmp(pAttr_name, A_SIZE))
		{
			// ATTR: size
			if (STATUS_FAIL == argValidateArgSyntax(common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(common.lineNumber,
												 pAttr_value,
												 &sizeArg))
			{
				return(STATUS_FAIL);
			}
			if (VAR_INT != sizeArg->varType)
			{
				// Needs to be an INT
				char string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseDefineOpen(): attribute [%s] must be of type [%s]",
						A_SIZE,
						argVarStringGet(VAR_INT));
				OUTPUT_ERR(common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
			size = sizeArg->arg.data.value.intValue;
			pSizeGiven = pAttr_value;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseDefineOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	if (!varFound || !typeFound)
	{
		// Required items not found
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseDefineOpen(): required attributes [%s,%s] missing",
				A_VAR,
				A_TYPE);
		OUTPUT_ERR(common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	// Define the variable
	if (STATUS_FAIL == argDefineVar(common.lineNumber,
									common.pName,
									varType,
									size))
	{
		return(STATUS_FAIL);
	}

	// We want to add the attributes
	outputXmlAttrAddCommon(LEVEL_TRACE, &common, A_VAR);
	outputXmlAttrAdd(A_TYPE, argVarStringGet(varType));
	if (pSizeGiven)
	{
		outputXmlAttrAdd(A_SIZE, pSizeGiven);
	}
	outputXmlTagCurrent(LEVEL_TRACE, P_DEFINE, outputXmlAttrGet());

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_DEFINE> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseDefineClose(XML_Parser pParser)
{
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_SET> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSetOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer for the new Init
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		varFound = FALSE;	// var is required
	boolean		valFound = FALSE;	// value or file is required
	boolean		fileFound = FALSE;	// file or value is required

	// Process a Set
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseSetOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseSetOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseSetOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_SET;
	pCode->code.cSet.operation = OP_EQ;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_VAR))
		{
			// Make sure correct variable definition
			if (STATUS_FAIL == argValidateVarSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				// Bad syntax
				return(STATUS_FAIL);
			}

			// ATTR: Var
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSetOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pName = ptr;
			varFound = TRUE;
		}
		else if (!strcmp(pAttr_name, A_VALUE))
		{
			// ATTR: value
			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cSet.pVal))
			{
				return(STATUS_FAIL);
			}
			valFound = TRUE;
		}
		else if (!strcmp(pAttr_name, A_FILE))
		{
			// ATTR: file
			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cSet.pFile))
			{
				return(STATUS_FAIL);
			}

			if (pCode->code.cSet.pFile->varType != VAR_PCHAR)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseSetOpen(): Invalid variable specified, [%s] required [%s] passed, for ATTR [%s]",
						argVarStringGet(VAR_PCHAR),
						argVarStringGet(pCode->code.cSet.pFile->varType),
						A_FILE);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}
			fileFound = TRUE;
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSetOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else if (!strcmp(pAttr_name, A_MEMBER))
		{
			// ATTR: member
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSetOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->code.cSet.pMember = ptr;
		}
		else if (!strcmp(pAttr_name, A_OP))
		{
			// ATTR: operation
			if (!strcmp(pAttr_value, OP_S_ADD))
			{
				pCode->code.cSet.operation = OP_ADD;
			}
			else if (!strcmp(pAttr_value, OP_S_SUB))
			{
				pCode->code.cSet.operation = OP_SUB;
			}
			else
			{
				// The parse failed
				char string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseSetOpen(): value [%s] of attribute [%s] not recognized",
						pAttr_value,
						pAttr_name);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseSetOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	if (!varFound || (!valFound && !fileFound))
	{
		// Required items not found
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseSetOpen(): required attributes [%s] or [%s or %s] missing",
				A_VAR,
				A_VALUE,
				A_FILE);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	// Set the pCode->pVar with variable being set
	// We have already made sure that it was a variable reference
	if (STATUS_FAIL == argDereferenceVar(pCode->common.lineNumber,
										 pCode->common.pName,
										 &pCode->code.cSet.pVar))
	{
		return(STATUS_FAIL);
	}

	// Link it in
	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_SET> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSetClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_FUNCTION> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseFunctionOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer for the new Function
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		nameFound = FALSE;	// funcName is required

	// Process a Function

	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseFunctionOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseFunctionOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseFunctionOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_FUNC;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_FNAME))
		{
			// ATTR: funcName
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseFunctionOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pName = ptr;
			nameFound = TRUE;

			if (STATUS_FAIL == funcDereferenceFunc(pCode->common.lineNumber,
												   pAttr_value,
												   &pCode->code.cFunc.pFunc))
			{
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseFunctionOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else if (!strcmp(pAttr_name, A_RETURN))
		{
			// ATTR: return
			// A return MUST be placed in a variable (not a constant)
			if (STATUS_FAIL == argValidateVarSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFunc.returnValue))
			{
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_ERRNO))
		{
			// ATTR: errno
			// An errno MUST be placed in a variable (not a constant)
			if (STATUS_FAIL == argValidateVarSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFunc.errnoValue))
			{
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_P1))
		{
			// ATTR: p1
			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFunc.arg[0]))
			{
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_P2))
		{
			// ATTR: p2
			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFunc.arg[1]))
			{
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_P3))
		{
			// ATTR: p3
			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFunc.arg[2]))
			{
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_P4))
		{
			// ATTR: p4
			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFunc.arg[3]))
			{
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_P5))
		{
			// ATTR: p5
			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFunc.arg[4]))
			{
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_P6))
		{
			// ATTR: p6
			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFunc.arg[5]))
			{
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_P7))
		{
			// ATTR: p7
			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFunc.arg[6]))
			{
				return(STATUS_FAIL);
			}
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseFunctionOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	if (!nameFound)
	{
		// Required items not found
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseFunctionOpen(): required attributes [%s] missing",
				A_FNAME);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	// Verify the arguments
	if (STATUS_FAIL == funcVerifyArgs(pCode->common.lineNumber,
									  &pCode->code.cFunc))
	{
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_FUNCTION> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseFunctionClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_IF> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseIfOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer for the new If
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		exprFound = FALSE;	// expression is required

	// Process a If
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseIfOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseIfOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseIfOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_IF;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_EXPR))
		{
			// ATTR: Expression
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseIfOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->code.cIf.expr.pExpr = ptr;
			exprFound = TRUE;
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseIfOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseIfOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	if (!exprFound)
	{
		// Required items not found
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseIfOpen(): required attributes [%s] missing",
				A_EXPR);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}
	
	if (STATUS_FAIL == exprParse(pCode))
	{
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	s_ppThen[s_nestingDepth] = &pCode->code.cIf.pThen;	// Set up <P_THEN> Block
	s_ppElse[s_nestingDepth] = &pCode->code.cIf.pElse;	// Set up <P_ELSE> Block

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_IF> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseIfClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	s_ppThen[s_nestingDepth] = NULL;
	s_ppElse[s_nestingDepth] = NULL;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_WHILE> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseWhileOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer for the new While
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		exprFound = FALSE;	// expression is required

	// Process a While
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseWhile(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseWhile(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseWhile(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_WHILE;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_EXPR))
		{
			// ATTR: Expression
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseWhileOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->code.cWhile.expr.pExpr = ptr;
			exprFound = TRUE;
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseWhileOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseWhileOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	if (!exprFound)
	{
		// Required items not found
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseWhileOpen(): required attributes [%s] missing",
				A_EXPR);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}
	
	if (STATUS_FAIL == exprParse(pCode))
	{
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];

	// Show we are "diving" in
	s_nestingDepth++;
	if (s_nestingDepth >= MAX_NESTING)
	{
		// We are nested too deep
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"pCode_parseWhileOpen(): nesting too deep, maximum [%d]",
				MAX_NESTING);
		OUTPUT_ERR(pCode->common.lineNumber,
				   string,
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
											// Show we are linking While code
	s_ppCode[s_nestingDepth] = &pCode->code.cWhile.pWhile;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_WHILE> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseWhileClose(XML_Parser pParser)
{
	s_ppCode[s_nestingDepth] = NULL;	// Clear for next use
	s_nestingDepth--;					// Show backing out of While nest
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_SETUP> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSetUpOpen(XML_Parser pParser, const char **pAttr)
{
	P_SU		*pSU;			// Pointer for the new Set Up
	P_SU		*pSU_next;		// Point to next SU
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		nameFound = FALSE;	// Name attribute is required

	// Process a Set Up
	if ((0 != s_nestingDepth) || (NULL != s_ppCode[s_nestingDepth]))
	{
		// We aren't at the top level
		// This is a problem
		// Can only be defined at Top Level
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseSetUpOpen(): Can only be defined at top level",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the SU instance
	if (!(pSU = malloc(sizeof(P_SU))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseSetUpOpen(): Failed to malloc space for SU size [%d]",
				sizeof(P_SU));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pSU, 0, sizeof(P_SU));

	// Save the line number
	pSU->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	
	// Link it into the SU List
	pSU_next = s_pTop.pSU;
	if (NULL == pSU_next)
	{
		// This is the first SU in the list
		s_pTop.pSU = pSU;
	}
	else
	{
		while (pSU_next->pSU)
		{
			pSU_next = pSU_next->pSU;
		}

		pSU_next->pSU = pSU;
	}

	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_NAME))
		{
			// ATTR: Name
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSetUpOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_NAME,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pSU->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pSU->common.pName = ptr;

			// Make sure this name is unique
			pSU_next = s_pTop.pSU;
			while (pSU_next != pSU)
			{
				if (!strcmp(ptr, pSU_next->common.pName))
				{
					// We found the name, not unique
					char	string[OUTPUT_STRING_MAX];

					sprintf(string,
							"pCode_parseSetUpOpen(): SetUp name [%s] is not unique",
							ptr);
					OUTPUT_ERR(pSU->common.lineNumber, string, NULL, NULL);
					return(STATUS_FAIL);
				}
				pSU_next = pSU_next->pSU;
			}

			nameFound = TRUE;	// Show name attribute found and processed
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSetUpOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pSU->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pSU->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseSetUpOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pSU->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present
	if (!nameFound)
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseSetUpOpen(): Required attribute[%s] not found",
				A_NAME);
		OUTPUT_ERR(pSU->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	// Show where to start linking code
	s_ppCode[s_nestingDepth] = &pSU->pCode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_SETUP> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSetUpClose(XML_Parser pParser)
{
	s_ppCode[s_nestingDepth] = NULL;	// Show done with P_CODE

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_TEARDOWN> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseTearDownOpen(XML_Parser pParser, const char **pAttr)
{
	P_TD		*pTD;			// Pointer for the new Tear Down
	P_TD		*pTD_next;		// Point to next TD
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		nameFound = FALSE;	// Name attribute is required

	// Process a Tear Down
	if ((0 != s_nestingDepth) || (NULL != s_ppCode[s_nestingDepth]))
	{
		// We aren't at the top level
		// This is a problem
		// Can only be defined at Top Level
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseTearDownOpen(): Can only be defined at top level",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the TD instance
	if (!(pTD = malloc(sizeof(P_TD))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseTearDownOpen(): Failed to malloc space for TD size [%d]",
				sizeof(P_TD));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pTD, 0, sizeof(P_TD));

	// Save the line number
	pTD->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	
	// Link it into the TD List
	pTD_next = s_pTop.pTD;
	if (NULL == pTD_next)
	{
		// This is the first TD in the list
		s_pTop.pTD = pTD;
	}
	else
	{
		while (pTD_next->pTD)
		{
			pTD_next = pTD_next->pTD;
		}

		pTD_next->pTD = pTD;
	}

	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_NAME))
		{
			// ATTR: Name
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTearDownOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_NAME,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pTD->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pTD->common.pName = ptr;

			// Make sure this name is unique
			pTD_next = s_pTop.pTD;
			while (pTD_next != pTD)
			{
				if (!strcmp(ptr, pTD_next->common.pName))
				{
					// We found the name, not unique
					char	string[OUTPUT_STRING_MAX];

					sprintf(string,
							"pCode_parseTearDownOpen(): TearDown name [%s] is not unique",
							ptr);
					OUTPUT_ERR(pTD->common.lineNumber, string, NULL, NULL);
					return(STATUS_FAIL);
				}
				pTD_next = pTD_next->pTD;
			}

			nameFound = TRUE;	// Show name attribute found and processed
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseTearDownOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pTD->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pTD->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseTearDownOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pTD->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present
	if (!nameFound)
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseTearDownOpen(): Required attribute[%s] not found",
				A_NAME);
		OUTPUT_ERR(pTD->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	// Show where to start linking code
	s_ppCode[s_nestingDepth] = &pTD->pCode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_TEARDOWN> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseTearDownClose(XML_Parser pParser)
{
	s_ppCode[s_nestingDepth] = NULL;	// Show done with P_CODE

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_SUBR> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSubroutineOpen(XML_Parser pParser, const char **pAttr)
{
	P_SUB		*pSUB;			// Pointer for the new Subroutine
	P_SUB		*pSUB_next;		// Point to next SUB
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		nameFound = FALSE;	// Name attribute is required

	// Process a Set Up
	if ((0 != s_nestingDepth) || (NULL != s_ppCode[s_nestingDepth]))
	{
		// We aren't at the top level
		// This is a problem
		// Can only be defined at Top Level
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseSubroutineOpen(): Can only be defined at top level",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the SU instance
	if (!(pSUB = malloc(sizeof(P_SUB))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseSubroutineOpen(): Failed to malloc space for SUB size [%d]",
				sizeof(P_SUB));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pSUB, 0, sizeof(P_SUB));

	// Save the line number
	pSUB->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	
	// Link it into the SU List
	pSUB_next = s_pTop.pSUB;
	if (NULL == pSUB_next)
	{
		// This is the first SUB in the list
		s_pTop.pSUB = pSUB;
	}
	else
	{
		while (pSUB_next->pSUB)
		{
			pSUB_next = pSUB_next->pSUB;
		}

		pSUB_next->pSUB = pSUB;
	}

	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_NAME))
		{
			// ATTR: Name
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSubroutineOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pSUB->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pSUB->common.pName = ptr;

			// Make sure this name is unique
			pSUB_next = s_pTop.pSUB;
			while (pSUB_next != pSUB)
			{
				if (!strcmp(ptr, pSUB_next->common.pName))
				{
					// We found the name, not unique
					char	string[OUTPUT_STRING_MAX];

					sprintf(string,
							"pCode_parseSubroutineOpen(): Subroutine name [%s] is not unique",
							ptr);
					OUTPUT_ERR(pSUB->common.lineNumber, string, NULL, NULL);
					return(STATUS_FAIL);
				}
				pSUB_next = pSUB_next->pSUB;
			}

			nameFound = TRUE;	// Show name attribute found and processed
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSubroutine(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pSUB->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pSUB->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseSubroutineOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pSUB->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present
	if (!nameFound)
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseSubroutineOpen(): Required attribute[%s] not found",
				A_NAME);
		OUTPUT_ERR(pSUB->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	// Show where to start linking code
	s_ppCode[s_nestingDepth] = &pSUB->pCode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_SUBR> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSubroutineClose(XML_Parser pParser)
{
	s_ppCode[s_nestingDepth] = NULL;	// Show done with P_CODE

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_SH> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSignalHandlerOpen(XML_Parser pParser, const char **pAttr)
{
	P_SigHand	*pSH;			// Pointer for the new SignalHandler
	P_SigHand	*pSH_next;		// Point to next SH
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		nameFound = FALSE;	// Name attribute is required

	// Process a SignalHandler
	if ((0 != s_nestingDepth) || (NULL != s_ppCode[s_nestingDepth]))
	{
		// We aren't at the top level
		// This is a problem
		// Can only be defined at Top Level
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseSignalHandlerOpen(): Can only be defined at top level",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the SU instance
	if (!(pSH = malloc(sizeof(P_SigHand))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseSignalhandlerOpen(): Failed to malloc space for SH size [%d]",
				sizeof(P_SigHand));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pSH, 0, sizeof(P_SigHand));

	// Save the line number
	pSH->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	
	// Link it into the SU List
	pSH_next = s_pTop.pSH;
	if (NULL == pSH_next)
	{
		// This is the first SH in the list
		s_pTop.pSH = pSH;
	}
	else
	{
		while (pSH_next->pSH)
		{
			pSH_next = pSH_next->pSH;
		}

		pSH_next->pSH = pSH;
	}

	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_NAME))
		{
			// ATTR: Name
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSignalHandlerOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pSH->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pSH->common.pName = ptr;

			// Make sure this name is unique
			pSH_next = s_pTop.pSH;
			while (pSH_next != pSH)
			{
				if (!strcmp(ptr, pSH_next->common.pName))
				{
					// We found the name, not unique
					char	string[OUTPUT_STRING_MAX];

					sprintf(string,
							"pCode_parseSignalHandler(): SignalHandler name [%s] is not unique",
							ptr);
					OUTPUT_ERR(pSH->common.lineNumber, string, NULL, NULL);
					return(STATUS_FAIL);
				}
				pSH_next = pSH_next->pSH;
			}

			nameFound = TRUE;	// Show name attribute found and processed
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSignalHandler(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pSH->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pSH->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseSignalHandlerOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pSH->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present
	if (!nameFound)
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseSignalHandlerOpen(): Required attribute[%s] not found",
				A_NAME);
		OUTPUT_ERR(pSH->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	// Show where to start linking code
	s_ppCode[s_nestingDepth] = &pSH->pCode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_SH> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSignalHandlerClose(XML_Parser pParser)
{
	s_ppCode[s_nestingDepth] = NULL;	// Show done with P_CODE

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_THEN> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseThenOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer to then pCode
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	P_CODE		**ppThen;		// Where we are suppoed to link this too

	// Process a Then
	if (NULL == s_ppThen[s_nestingDepth])
	{
		// We aren't expecting a Then right now
		// This is a problem
		// Can only be defined when expected
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseThenOpen(): <Then> Not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// First malloc the Then
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseThenOpen(): Failed to malloc space for THEN size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_THEN;

	// Save for later update
	ppThen = s_ppThen[s_nestingDepth];
	
	// Show we are "diving" in
	s_nestingDepth++;
	if (s_nestingDepth >= MAX_NESTING)
	{
		// We are nested too deep
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseThenOpen(): nesting too deep, maximum [%d]",
				MAX_NESTING);
		OUTPUT_ERR(pCode->common.lineNumber,
				   string,
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseThenOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseThenOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Show where to start linking code
	*ppThen = pCode;
	s_ppCode[s_nestingDepth] = &pCode->pCode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_THEN> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseThenClose(XML_Parser pParser)
{
	s_ppCode[s_nestingDepth] = NULL;	// Clear for next use
	s_nestingDepth--;					// Show we are backing out a nesting lvl
	s_ppThen[s_nestingDepth] = NULL;	// Show then is no longer currently valid
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_ELSE> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseElseOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer to Else pCode
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	P_CODE		**ppElse;		// Where we are suppoed to link this too

	// Process a Else
	if (NULL == s_ppElse[s_nestingDepth])
	{
		// We aren't expecting a Else right now
		// This is a problem
		// Can only be defined when expected
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseElseOpen(): <Else> Not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Save for later update
	ppElse = s_ppElse[s_nestingDepth];
	
	// First malloc the Else
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseElseOpen(): Failed to malloc space for Else size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_ELSE;

	// Show we are "diving" in
	s_nestingDepth++;
	if (s_nestingDepth >= MAX_NESTING)
	{
		// We are nested too deep
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseElseOpen(): nesting too deep, maximum [%d]",
				MAX_NESTING);
		OUTPUT_ERR(pCode->common.lineNumber,
				   string,
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseElseOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseElseOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Show where to start linking code
	*ppElse = pCode;
	s_ppCode[s_nestingDepth] = &pCode->pCode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_ELSE> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseElseClose(XML_Parser pParser)
{
	s_ppCode[s_nestingDepth] = NULL;	// Clear for next use
	s_nestingDepth--;					// Show we are backing out a nesting lvl
	s_ppElse[s_nestingDepth] = NULL;	// Show Else is no longer currently valid
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_CALL> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseCallOpen(XML_Parser pParser, const char **pAttr)
{
	P_SUB		*pSUB = NULL;	// Pointer to referenced Subroutine
	P_SU		*pSU = NULL;	// Pointer to referenced SetUp
	P_TD		*pTD = NULL;	// Pointer to referenced TearDown
	P_CODE		*pCode;			// Pointer to Call pCode
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Temporary malloc
	boolean		refFound = FALSE;	// Required attribute found flag

	// Process a Call
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseCallOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseCallOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseCallOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_CALL;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_REF))
		{
			// ATTR: Ref
			refFound = TRUE;

			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseCallOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_REF,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pName = ptr;

			// Make sure this reference is found
			pSUB = s_pTop.pSUB;
			while (NULL != pSUB)
			{
				if (!strcmp(pSUB->common.pName, pAttr_value))
				{
					// We found the referenced Subroutine
					break;
				}
				pSUB = pSUB->pSUB;
			}

			if (NULL == pSUB)
			{
				// We did not find referenced Subroutine
				char string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseCallOpen(): Referenced Subroutine [%s] not defined",
						pAttr_value);
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   NULL,
						   NULL);
						   
				return(STATUS_FAIL);
			}

			pCode->code.cCall.pSUB = pSUB;	// Link in Subroutine
		}
		else if (!strcmp(pAttr_name, A_SETUP))
		{
			// ATTR: SetUp
			// Make sure this reference is found
			pSU = s_pTop.pSU;
			while (NULL != pSU)
			{
				if (!strcmp(pSU->common.pName, pAttr_value))
				{
					// We found the referenced SetUp
					break;
				}
				pSU = pSU->pSU;
			}

			if (NULL == pSU)
			{
				// We did not find referenced Set Up
				char string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseCallOpen(): Referenced Set Up [%s] not defined",
						pAttr_value);
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   NULL,
						   NULL);
						   
				return(STATUS_FAIL);
			}

			// Link the found Set Up
			pCode->code.cCall.pSU = pSU;
		}
		else if (!strcmp(pAttr_name, A_TEARDOWN))
		{
			// ATTR: TearDown
			// Make sure this reference is found
			pTD = s_pTop.pTD;
			while (NULL != pTD)
			{
				if (!strcmp(pTD->common.pName, pAttr_value))
				{
					// We found the referenced Tear Down
					break;
				}
				pTD = pTD->pTD;
			}

			if (NULL == pTD)
			{
				// We did not find referenced Set Up
				char string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseCallOpen(): Referenced Tear Down [%s] not defined",
						pAttr_value);
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   NULL,
						   NULL);
						   
				return(STATUS_FAIL);
			}

			// Link the found TearDown
			pCode->code.cCall.pTD = pTD;
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseCallOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseCallOpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
					   string,
					   NULL,
					   NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present and processed
	if (!refFound)
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseCallOpen(): Required attribute[%s] not found",
				A_REF);
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_CALL> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseCallClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_SIG> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSignalOpen(XML_Parser pParser, const char **pAttr)
{
	P_SigHand	*pSH = NULL;	// Pointer to referenced Signal Handler
	P_CODE		*pCode;			// Pointer to Sig pCode
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Temporary malloc
	boolean		refFound = FALSE;	// Required attribute found flag
	boolean		sigFound = FALSE;	// Required attribute found flag
	boolean		actFound = FALSE;	// Required attribute found flag

	// Process a Signal
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseSignalOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseSignalOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseSignalOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_SIG;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_REF))
		{
			// ATTR: Ref
			refFound = TRUE;

			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSignalOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_REF,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pName = ptr;

			// Make sure this reference is found
			pSH = s_pTop.pSH;
			while (NULL != pSH)
			{
				if (!strcmp(pSH->common.pName, pAttr_value))
				{
					// We found the referenced Subroutine
					break;
				}
				pSH = pSH->pSH;
			}

			if (NULL == pSH)
			{
				// We did not find referenced Subroutine
				char string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseSignalOpen(): Referenced Signal Handler [%s] not defined",
						pAttr_value);
				OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}

			pCode->code.cSig.pSH = pSH;	// Link in Signal Handler
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseSignalOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else if (!strcmp(pAttr_name, A_SIG))
		{
			// ATTR: signal
			sigFound = TRUE;

			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseSignalOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);

			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cSig.pSignal))
			{
				return(STATUS_FAIL);
			}

			pCode->code.cSig.pSignal->pName = ptr;

			if (pCode->code.cSig.pSignal->varType != VAR_INT)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseSignalOpen(): Invalid variable specified, [%s] required [%s] passed",
						argVarStringGet(VAR_INT),
						argVarStringGet(pCode->code.cSig.pSignal->varType));
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_ACTION))
		{
			// ATTR: action
			actFound = TRUE;
			if (!strcmp(pAttr_value, VALUE_ENABLE))
			{
				pCode->code.cSig.enable = TRUE;
			}
			else if (!strcmp(pAttr_value, VALUE_DISABLE))
			{
				pCode->code.cSig.enable = FALSE;
			}
			else
			{
				// illegal action value
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseSignalOpen(): Illegal value [%s] for attribute [%s]",
						pAttr_value,
						pAttr_name);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];
			sprintf(string,
					"pCode_parseSignalOpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
					   string,
					   NULL,
					   NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present and processed
	// ref required if action is enable
	if (   !actFound
		|| !sigFound
		|| ((pCode->code.cSig.enable == TRUE) && !refFound))
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];
		sprintf(string,
				"pCode_parseSignalOpen(): Required attribute[%s or %s or %s] not found",
				A_REF,
				A_ACTION,
				A_SIG);
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser), string, NULL, NULL);
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_SIG> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSignalClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_AUX> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseAuxOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer to Aux pCode
	char		*ptr;			// Temporary malloc
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	boolean		flipFound = FALSE;	// Required attribute found flag

	// Process a Aux
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseAuxOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseAuxOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseAuxOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_AUX;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseAuxOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else if (!strcmp(pAttr_name, A_FLIP))
		{
			// ATTR: flip
			flipFound = TRUE;
			if (!strcmp(pAttr_value, NC_ON))
			{
				pCode->code.cAux.flip = TRUE;
			}
			else if (!strcmp(pAttr_value, NC_OFF))
			{
				pCode->code.cAux.flip = FALSE;
			}
			else
			{
				// illegal action value
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseAuxOpen(): Illegal value [%s] for attribute [%s]",
						pAttr_value,
						pAttr_name);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];
			sprintf(string,
					"pCode_parseAuxOpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
					   string,
					   NULL,
					   NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present and processed
	// ref required if action is enable
	if (!flipFound)
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];
		sprintf(string,
				"pCode_parseAuxOpen(): Required attribute[%s] not found",
				A_FLIP);
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser), string, NULL, NULL);
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_AUX> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseAuxClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_DUMP> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseDumpOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer to Dump pCode
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Temporary malloc
	boolean		fileFound = FALSE;	// Required attribute found flag
	boolean		dumpFound = FALSE;	// Required attribute found flag
	boolean		frameFound = FALSE;	// Required attribute found flag

	// Process a dump
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseDumpOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseDumpOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"pCode_parseDumpOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_DUMP;
	pCode->code.cDump.level = LEVEL_SUMMARY;	// Default level of dump
	pCode->code.cDump.force = FALSE;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_FILE))
		{
			// ATTR: File
			fileFound = TRUE;

			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseDumpOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pName = ptr;

			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cDump.pFile))
			{
				return(STATUS_FAIL);
			}

			if (pCode->code.cDump.pFile->varType != VAR_PCHAR)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseDumpOpen(): Invalid variable specified, [%s] required [%s] passed, for ATTR [%s]",
						argVarStringGet(VAR_PCHAR),
						argVarStringGet(pCode->code.cDump.pFile->varType),
						A_FILE);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_SEQ))
		{
			// ATTR: Sequence Number
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseDumpOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);

			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cDump.pSeq))
			{
				return(STATUS_FAIL);
			}
			pCode->code.cDump.pSeq->pName = ptr;

			if (pCode->code.cDump.pSeq->varType != VAR_INT)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseDumpOpen(): Invalid variable specified, [%s] required [%s] passed, for ATTR [%s]",
						argVarStringGet(VAR_INT),
						argVarStringGet(pCode->code.cDump.pSeq->varType),
						A_SEQ);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_FRAME))
		{
			// ATTR: Frame Number
			frameFound = TRUE;
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseDumpOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);

			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cDump.pFrame))
			{
				return(STATUS_FAIL);
			}
			pCode->code.cDump.pFrame->pName = ptr;

			if (pCode->code.cDump.pFrame->varType != VAR_INT)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseDumpOpen(): Invalid variable specified, [%s] required [%s] passed, for ATTR [%s]",
						argVarStringGet(VAR_INT),
						argVarStringGet(pCode->code.cDump.pFrame->varType),
						A_FRAME);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_DUMP))
		{
			// ATTR: Dump
			dumpFound = TRUE;
			
			if (!strcmp(pAttr_value, D_VD))
			{
				pCode->code.cDump.type = DUMP_VD;
			}
			else if (!strcmp(pAttr_value, D_CMDMSG))
			{
				pCode->code.cDump.type = DUMP_CMDMSG;
			}
			else
			{
				// Unrecognized value
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseDumpOpen(): Unknown [%s] value [%s]",
						pAttr_name,
						pAttr_value);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_LEVEL))
		{
			if (!strcmp(pAttr_value, LVL_0))
			{
				pCode->code.cDump.level = LEVEL_ALWAYS;
			}
			else if (!strcmp(pAttr_value, LVL_1))
			{
				pCode->code.cDump.level = LEVEL_CONFORMANCE;
			}
			else if (!strcmp(pAttr_value, LVL_2))
			{
				pCode->code.cDump.level = LEVEL_SUMMARY;
			}
			else if (!strcmp(pAttr_value, LVL_3))
			{
				pCode->code.cDump.level = LEVEL_TRACE;
			}
			else
			{
				// Unknown level
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseDumpOpen(): Failed to parse %s=%s, unknown value",
						pAttr_name,
						pAttr_value);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseDumpOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else if (!strcmp(pAttr_name, A_FORCE))
		{
			if (!strcmp(pAttr_value, NC_TRUE))
			{
				pCode->code.cDump.force = TRUE;
			}
			else
			{
				// The parse failed
				char string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseDumpOpen(): Attribute [%s] value [%s] not recognized",
						pAttr_name,
						pAttr_value);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseDumpOpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present and processed
	if (   !fileFound
		|| !dumpFound
		|| (   (pCode->code.cDump.type == DUMP_CMDMSG)
			&& !frameFound))
	{
		// Required attributes not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseDumpOpen(): Required attributes[%s or %s or %s] not found",
				A_FILE,
				A_DUMP,
				A_FRAME);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_DUMP> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseDumpClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_LOAD> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseLoadOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer to Dump pCode
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Temporary malloc
	boolean		fileFound = FALSE;	// Required attribute found flag
	boolean		loadFound = FALSE;	// Required attribute found flag
	boolean		frameFound = FALSE;	// Required attribute found flag

	// Process a dump
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseLoadOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseLoadOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"pCode_parseLoadOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_LOAD));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_LOAD;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_FILE))
		{
			// ATTR: File
			fileFound = TRUE;

			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseLoadOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pName = ptr;

			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cLoad.pFile))
			{
				return(STATUS_FAIL);
			}

			if (pCode->code.cLoad.pFile->varType != VAR_PCHAR)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseLoadOpen(): Invalid variable specified, [%s] required [%s] passed",
						argVarStringGet(VAR_PCHAR),
						argVarStringGet(pCode->code.cLoad.pFile->varType));
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_LOAD))
		{
			// ATTR: Load
			loadFound = TRUE;
			
			if (!strcmp(pAttr_value, D_VD))
			{
				pCode->code.cLoad.type = DUMP_VD;
			}
			else if (!strcmp(pAttr_value, D_CMDMSG))
			{
				pCode->code.cLoad.type = DUMP_CMDMSG;
			}
			else
			{
				// Unrecognized value
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseLoadOpen(): Unknown [%s] value [%s]",
						pAttr_name,
						pAttr_value);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_FRAME))
		{
			// ATTR: Frame Number
			frameFound = TRUE;
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseLoadOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);

			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cLoad.pFrame))
			{
				return(STATUS_FAIL);
			}
			pCode->code.cLoad.pFrame->pName = ptr;

			if (pCode->code.cLoad.pFrame->varType != VAR_INT)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseLoadOpen(): Invalid variable specified, [%s] required [%s] passed, for ATTR [%s]",
						argVarStringGet(VAR_INT),
						argVarStringGet(pCode->code.cLoad.pFrame->varType),
						A_FRAME);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseLoadOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseLoadOpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present and processed
	if (   !fileFound
		|| !loadFound
		|| (   (pCode->code.cLoad.type == DUMP_CMDMSG)
			&& !frameFound))
	{
				pCode->code.cLoad.type = DUMP_CMDMSG;
		// Required attributes not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseLoadOpen(): Required attributes[%s or %s or %s] not found",
				A_FILE,
				A_LOAD,
				A_FRAME);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_LOAD> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseLoadClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_FORMAT> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseFormatOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer to Format pCode
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Temporary malloc

	// Process a Format
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseFormatOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseFormatOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"pCode_parseFormatOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_FORMAT;
	pCode->code.cFormat.level = LEVEL_TRACE;	// Default level of Format
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_VAR))
		{
			// ATTR: Var
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseFormatOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pName = ptr;

			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFormat.pArg))
			{
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_LEVEL))
		{
			if (!strcmp(pAttr_value, LVL_0))
			{
				pCode->code.cFormat.level = LEVEL_ALWAYS;
			}
			else if (!strcmp(pAttr_value, LVL_1))
			{
				pCode->code.cFormat.level = LEVEL_CONFORMANCE;
			}
			else if (!strcmp(pAttr_value, LVL_2))
			{
				pCode->code.cFormat.level = LEVEL_SUMMARY;
			}
			else if (!strcmp(pAttr_value, LVL_3))
			{
				pCode->code.cFormat.level = LEVEL_TRACE;
			}
			else
			{
				// Unknown level
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseFormatOpen(): Failed to parse %s=%s, unknown value",
						pAttr_name,
						pAttr_value);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseFormatOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseFormatOpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_FORMAT> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseFormatClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_ABORT> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseAbortOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer for the new P_CODE
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Used for temporary mallocs
	boolean		statusFound = FALSE;	// For required attr

	// Process a Abort
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseAbortOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseAbortOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseAbortOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_ABORT;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseAbortOpen(): Failed to malloc space for attribute [%s] size [%d]",
						A_DESC,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else if (!strcmp(pAttr_name, A_STATUS))
		{
			if (!strcmp(pAttr_value, VALUE_PASS))
			{
				pCode->code.cAbort.status = STATUS_PASS;
			}
			else if (!strcmp(pAttr_value, VALUE_FAIL))
			{
				pCode->code.cAbort.status = STATUS_FAIL;
			}
			else
			{
				// Unknown status
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseAbortOpen(): Failed to parse %s=%s, unknown value",
						A_STATUS,
						pAttr_value);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}
			
			statusFound = TRUE;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseAbortOpen(): attribute[%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present
	if (!statusFound)
	{
		// Required name not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseAbortOpen(): Required attribute[%s] not found",
				A_STATUS);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_ABORT> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseAbortClose(XML_Parser pParser)
{
	s_ppText = NULL;							// Show text no longer allowed
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_SLEEP> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSleepOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer to Sleep pCode
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Temporary malloc
	boolean		timeFound = FALSE;	// Required attribute found flag

	// Process a sleep
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseSleepOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseSleepOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"pCode_parseSleepOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_SLEEP;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_TIME))
		{
			// ATTR: time
			timeFound = TRUE;

			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSleepOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pName = ptr;

			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cSleep.pArg))
			{
				return(STATUS_FAIL);
			}

			if (pCode->code.cSleep.pArg->varType != VAR_INT)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseSleepOpen(): Invalid variable specified, [%s] required [%s] passed",
						argVarStringGet(VAR_INT),
						argVarStringGet(pCode->code.cSleep.pArg->varType));
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseSleepOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseSleepOpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present and processed
	if (!timeFound)
	{
		// Required attributes not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseSleepOpen(): Required attributes [%s] not found",
				A_TIME);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_SLEEP> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseSleepClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_FPUII> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseFPUIIOpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer to FPUII pCode
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Temporary malloc
	boolean		fileFound = FALSE;	// Required attribute found flag

	// Process a FPUII
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseFPUIIOpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseFPUIIOpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"pCode_parseFPUIIOpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_FPUII;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_FILE))
		{
			// ATTR: File
			fileFound = TRUE;

			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseFPUIIOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pName = ptr;

			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFPUII.pFile))
			{
				return(STATUS_FAIL);
			}

			if (pCode->code.cFPUII.pFile->varType != VAR_PCHAR)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseFPUIIOpen(): Invalid variable specified, [%s] required [%s] passed",
						argVarStringGet(VAR_PCHAR),
						argVarStringGet(pCode->code.cFPUII.pFile->varType));
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseFPUIIOpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseFPUIIOpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present and processed
	if (!fileFound)
	{
		// Required attributes not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseFPUIIOpen(): Required attribute [%s] not found",
				A_FILE);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_FPUII> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseFPUIIClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a <P_FIOR> element
 *
 * \param[in]	pParser - expat private data
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseFIOROpen(XML_Parser pParser, const char **pAttr)
{
	P_CODE		*pCode;			// Pointer to FIOR pCode
	const char	*pAttr_name;	// Pointer to ATTR name
	const char	*pAttr_value;	// Pointer to ATTR value
	char		*ptr;			// Temporary malloc
	boolean		fileFound = FALSE;	// Required attribute found flag
	boolean		frameFound = FALSE;	// Required attribute found flag

	// Process a FIOR
	
	// See if P_CODE is expected
	if (NULL == s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseFIOROpen(): P_CODE not allowed at this point",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}

	// Make sure we have not linked something here yet
	if (NULL != *s_ppCode[s_nestingDepth])
	{
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   "pCode_parseFIOROpen(): Program error, P_CODE exists",
				   NULL,
				   NULL);
		return(STATUS_FAIL);
	}
	
	// First malloc the P_CODE instance
	if (!(pCode = malloc(sizeof(P_CODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"pCode_parseFIOROpen(): Failed to malloc space for PC size [%d]",
				sizeof(P_CODE));
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set it up to be filled in
	memset(pCode, 0, sizeof(P_CODE));

	// Save the line number
	pCode->common.lineNumber = XML_GetCurrentLineNumber(pParser);
	pCode->type = PC_E_FIOR;
	
	// Parse the attributes
	pAttr_name = *pAttr++;
	while (pAttr_name)
	{
		pAttr_value = *pAttr++;

		if (!strcmp(pAttr_name, A_FILE))
		{
			// ATTR: File
			fileFound = TRUE;

			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseFIOROpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pName = ptr;

			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFIOR.pFile))
			{
				return(STATUS_FAIL);
			}

			if (pCode->code.cFIOR.pFile->varType != VAR_PCHAR)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseFIOROpen(): Invalid variable specified, [%s] required [%s] passed",
						argVarStringGet(VAR_PCHAR),
						argVarStringGet(pCode->code.cFIOR.pFile->varType));
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_FRAME))
		{
			// ATTR: Sequence Number
			frameFound = TRUE;
			if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber,
													pAttr_value))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber,
												 pAttr_value,
												 &pCode->code.cFIOR.pFrame))
			{
				return(STATUS_FAIL);
			}

			if (pCode->code.cFIOR.pFrame->varType != VAR_INT)
			{
				// Illegal variable type specified
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"pCode_parseFIOROpen(): Invalid variable specified, [%s] required [%s] passed, for ATTR [%s]",
						argVarStringGet(VAR_INT),
						argVarStringGet(pCode->code.cFIOR.pFrame->varType),
						A_FRAME);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   NULL,
						   NULL);
				return(STATUS_FAIL);
			}
		}
		else if (!strcmp(pAttr_name, A_DESC))
		{
			// ATTR: Description
			if (!(ptr = malloc(strlen(pAttr_value) + 1)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"pCode_parseFIOROpen(): Failed to malloc space for attribute [%s] size [%d]",
						pAttr_name,
						strlen(pAttr_value) + 1);
				OUTPUT_ERR(pCode->common.lineNumber,
						   string,
						   strerror(errno),
						   NULL);
				return(STATUS_FAIL);
			}

			strncpy(ptr, pAttr_value, strlen(pAttr_value) + 1);
			pCode->common.pDesc = ptr;
		}
		else
		{
			// The parse failed
			char string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_parseFIOROpen(): attribute [%s] not recognized",
					pAttr_name);
			OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Work on next attribute
		pAttr_name = *pAttr++;
	}

	// Make sure required parameters are present and processed
	if (!fileFound || !frameFound)
	{
		// Required attributes not found
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseFIOROpen(): Required attributes [%s,%s] not found",
				A_FILE,
				A_FRAME);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	s_pCodeSave[s_nestingDepth] = pCode;	// Show the current pCode
											// Save the current linkage
	s_ppCodeSave[s_nestingDepth] = s_ppCode[s_nestingDepth];
	s_ppCode[s_nestingDepth] = NULL;		// Show no pCode allowed

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used for parsing a </P_FIOR> element
 *
 * \param[in]	pParser - expat private data
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseFIORClose(XML_Parser pParser)
{
												// Link in this P_CODE
	*s_ppCodeSave[s_nestingDepth] = s_pCodeSave[s_nestingDepth];
												// Set up for next P_CODE
	s_ppCode[s_nestingDepth] = &s_pCodeSave[s_nestingDepth]->pCode;
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function parses a PCODE opening tag
 *
 * \param[in]	pData - expat private data (not used)
 * \param[in]	pElement - The element tag
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_parseOpen(void *pData, const char *pElement, const char **pAttr)
{
	XML_Parser	pParser = (XML_Parser)pData;
	int16_t		status = STATUS_PASS;

	// Parse the elements one by one
	if (!strcmp(pElement, P_TESTSUITE))
	{
		// It's a Test Suite
		status = pCode_parseTestSuiteOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_TESTCASE))
	{
		// It's a Test Case
		status = pCode_parseTestCaseOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_TESTCASEREF))
	{
		// It's a Test Case Reference
		status = pCode_parseTestCaseRefOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_PRINT))
	{
		// It's a Print
		status = pCode_parsePrintOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_SETUP))
	{
		// It's a SetUp
		status = pCode_parseSetUpOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_TEARDOWN))
	{
		// It's a TearDown
		status = pCode_parseTearDownOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_SUBR))
	{
		// It's a Subroutine
		status = pCode_parseSubroutineOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_SH))
	{
		// It's a SignalHandler
		status = pCode_parseSignalHandlerOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_AUX))
	{
		// It's a Aux
		status = pCode_parseAuxOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_DEFINE))
	{
		// It's a Define
		status = pCode_parseDefineOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_SET))
	{
		// It's a Set
		status = pCode_parseSetOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_FUNCTION))
	{
		// It's a Function
		status = pCode_parseFunctionOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_IF))
	{
		// It's a If
		status = pCode_parseIfOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_WHILE))
	{
		// It's a While
		status = pCode_parseWhileOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_THEN))
	{
		// It's a Then
		status = pCode_parseThenOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_ELSE))
	{
		// It's a Else
		status = pCode_parseElseOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_CALL))
	{
		// It's a Call
		status = pCode_parseCallOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_SIG))
	{
		// It's a Signal
		status = pCode_parseSignalOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_ABORT))
	{
		// It's a Abort
		status = pCode_parseAbortOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_DUMP))
	{
		// It's a Dump
		status = pCode_parseDumpOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_LOAD))
	{
		// It's a Load
		status = pCode_parseLoadOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_FORMAT))
	{
		// It's a Format
		status = pCode_parseFormatOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_SLEEP))
	{
		// It's a Sleep
		status = pCode_parseSleepOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_FPUII))
	{
		// It's a FPUII
		status = pCode_parseFPUIIOpen(pParser, pAttr);
	}
	else if (!strcmp(pElement, P_FIOR))
	{
		// It's a FIOR
		status = pCode_parseFIOROpen(pParser, pAttr);
	}
	else
	{
		// The parse failed
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseOpen() pElement[%s] not recognized",
				pElement);
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   NULL,
				   NULL);
		status = STATUS_FAIL;
	}

	// See if the parse has failed
	if (STATUS_FAIL == status)
	{
		// Yes, go no further
		XML_SetElementHandler(pParser, NULL, NULL);
		XML_SetCharacterDataHandler(pParser, NULL);
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function parses a PCODE closing tag
 *
 * \param[in]	pData - expat private data (not used)
 * \param[in]	pElement - The element tag
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
pCode_parseClose(void *pData, const char *pElement)
{
	XML_Parser	pParser = (XML_Parser)pData;
	int16_t		status = STATUS_PASS;

	// Parse the elements one by one
	if (!strcmp(pElement, P_APIVSXML))
	{
		OUTPUT_TRACE("APIVSXML");
	}
	else if (!strcmp(pElement, P_TESTSUITE))
	{
		// It's a Test Suite
		status = pCode_parseTestSuiteClose(pParser);
	}
	else if (!strcmp(pElement, P_TESTCASE))
	{
		// It's a Test Case
		status = pCode_parseTestCaseClose(pParser);
	}
	else if (!strcmp(pElement, P_TESTCASEREF))
	{
		// It's a Test Case Reference
		status = pCode_parseTestCaseRefClose(pParser);
	}
	else if (!strcmp(pElement, P_PRINT))
	{
		// It's a Print
		status = pCode_parsePrintClose(pParser);
	}
	else if (!strcmp(pElement, P_SETUP))
	{
		// It's a SetUp
		status = pCode_parseSetUpClose(pParser);
	}
	else if (!strcmp(pElement, P_TEARDOWN))
	{
		// It's a TearDown
		status = pCode_parseTearDownClose(pParser);
	}
	else if (!strcmp(pElement, P_SUBR))
	{
		// It's a Subroutine
		status = pCode_parseSubroutineClose(pParser);
	}
	else if (!strcmp(pElement, P_SH))
	{
		// It's a SignalHandler
		status = pCode_parseSignalHandlerClose(pParser);
	}
	else if (!strcmp(pElement, P_DEFINE))
	{
		// It's a Define
		status = pCode_parseDefineClose(pParser);
	}
	else if (!strcmp(pElement, P_SET))
	{
		// It's a Set
		status = pCode_parseSetClose(pParser);
	}
	else if (!strcmp(pElement, P_FUNCTION))
	{
		// It's a Function
		status = pCode_parseFunctionClose(pParser);
	}
	else if (!strcmp(pElement, P_IF))
	{
		// It's a If
		status = pCode_parseIfClose(pParser);
	}
	else if (!strcmp(pElement, P_WHILE))
	{
		// It's a While
		status = pCode_parseWhileClose(pParser);
	}
	else if (!strcmp(pElement, P_THEN))
	{
		// It's a Then
		status = pCode_parseThenClose(pParser);
	}
	else if (!strcmp(pElement, P_ELSE))
	{
		// It's a Else
		status = pCode_parseElseClose(pParser);
	}
	else if (!strcmp(pElement, P_CALL))
	{
		// It's a Call
		status = pCode_parseCallClose(pParser);
	}
	else if (!strcmp(pElement, P_SIG))
	{
		// It's a Signal
		status = pCode_parseSignalClose(pParser);
	}
	else if (!strcmp(pElement, P_AUX))
	{
		// It's a Aux
		status = pCode_parseAuxClose(pParser);
	}
	else if (!strcmp(pElement, P_ABORT))
	{
		// It's a Abort
		status = pCode_parseAbortClose(pParser);
	}
	else if (!strcmp(pElement, P_DUMP))
	{
		// It's a Dump
		status = pCode_parseDumpClose(pParser);
	}
	else if (!strcmp(pElement, P_LOAD))
	{
		// It's a Load
		status = pCode_parseLoadClose(pParser);
	}
	else if (!strcmp(pElement, P_FORMAT))
	{
		// It's a Format
		status = pCode_parseFormatClose(pParser);
	}
	else if (!strcmp(pElement, P_SLEEP))
	{
		// It's a Sleep
		status = pCode_parseSleepClose(pParser);
	}
	else if (!strcmp(pElement, P_FPUII))
	{
		// It's a FPUII
		status = pCode_parseFPUIIClose(pParser);
	}
	else if (!strcmp(pElement, P_FIOR))
	{
		// It's a FIOR
		status = pCode_parseFIORClose(pParser);
	}
	else
	{
		// The parse failed
		char string[OUTPUT_STRING_MAX];

		sprintf(string,
				"pCode_parseClose() pElement[%s] not recognized",
				pElement);
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   NULL,
				   NULL);
		status = STATUS_FAIL;
	}

	// See if Parse succeeded
	if (STATUS_FAIL == status)
	{
		// Fail, go no further
		XML_SetElementHandler(pParser, NULL, NULL);
		XML_SetCharacterDataHandler(pParser, NULL);
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function parses text between an open and close tag
 *
 * \param[in]	pData - expat private data
 * \param[in]	pText - pointer to text to parse
 * \param[in]	len - length of text to parse
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
void
pCode_text(void *pData, const char *pText, uint16_t len)
{
	XML_Parser 	pParser = (XML_Parser)pData;
	char		*ptr;			// Used for temporary mallocs

	// See if we have an active text collector point
	if (NULL == s_ppText)
	{
		return;
	}

	// See if we have have already processed text
	if (NULL != *s_ppText)
	{
		// Yes, we have to concatenate the strings together
		// Get the length of the first string
		uint16_t	oldLen = strlen(*s_ppText);

		// Malloc space big enough for the 2 strings
		if (!(ptr = malloc(len + oldLen + 1)))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_text(): Failed to malloc space for text size [%d]",
					len + oldLen + 1);
			OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
					   string,
					   strerror(errno),
					   NULL);
			XML_SetElementHandler(pParser, NULL, NULL);
			XML_SetCharacterDataHandler(pParser, NULL);
			return;
		}

		// Copy the first string into new buffer
		memcpy(ptr, *s_ppText, oldLen);
		free(*s_ppText);
		*s_ppText = ptr;			// Save the address of new text buffer

		// Now concatenate new string
		ptr += oldLen;
		while (len)
		{
			*ptr++ = *pText++;
			len--;
		}
	}
	else
	{
		// Allocate space for text, +1 for a "\0'
		if (!(ptr = malloc(len + 1)))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];

			sprintf(string,
					"pCode_text(): Failed to malloc space for text size [%d]",
					len + 1);
			OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
					   string,
					   strerror(errno),
					   NULL);
			XML_SetElementHandler(pParser, NULL, NULL);
			XML_SetCharacterDataHandler(pParser, NULL);
			return;
		}

		// Move to buffer
		// NULL Terminate string being moved
		memcpy(ptr, pText, len);
		ptr[len] = '\0';
		*s_ppText = ptr;			// Save the address of the text buffer
	}
}

//=============================================================================
/**
 * \brief This function starts the pCode processing
 *
 * \param[in]	pData - expat private data (not used)
 * \param[in]	pElement - The element tag
 * \param[in]	pAttr - Attribute list
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 *
 */
int16_t
pCode_start(void *pData, const char *pElement, const char **pAttr)
{
	XML_Parser	pParser = (XML_Parser)pData;

	if (strcmp(pElement, P_APIVSXML))
	{
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"Parsing error: First element is not [%s]",
				P_APIVSXML);
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   NULL,
				   NULL);
		XML_SetElementHandler(pParser, NULL, NULL);
		XML_SetCharacterDataHandler(pParser, NULL);
		return(STATUS_FAIL);
	}

	if (NULL != pAttr[0])
	{
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"Parsing error: pAttr list for [%s] is not empty",
				P_APIVSXML);
		OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
				   string,
				   NULL,
				   NULL);
		XML_SetElementHandler(pParser, NULL, NULL);
		XML_SetCharacterDataHandler(pParser, NULL);
		return(STATUS_FAIL);
	}

	// Everything is as it should be
	// Ready the TOP element for the future
	memset(&s_pTop, 0, sizeof(P_TOP));
	
	memset(&s_ppCode[0], 0, sizeof(s_ppCode));
	memset(&s_ppThen[0], 0, sizeof(s_ppThen));
	memset(&s_ppElse[0], 0, sizeof(s_ppElse));

	// Return success
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function function ends the parse processing
 *
 * \param[in]	pData - expat private data (not used)
 * \param[in]	pElement - The element tag
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
pCode_end(void *pData, const char *pElement)
{
	XML_Parser pParser = (XML_Parser)pData;
	pParser = pParser;					// Stop compiler warning

	return(STATUS_PASS);
}
