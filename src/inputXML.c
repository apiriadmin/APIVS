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
 * \file inputXML.c
 *
 * This file contains the definition of functions for processing the input
 * XML
 *
 * \brief input XML processing
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
#include	"inputXML.h"	// input XMl Processor Definitions
#include	"outputXML.h"	// input XMl Processor Definitions
#include	"pCode.h"		// PCODE Processor Definitions

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
/** \brief Where input is read from */
static	FILE	*s_inputFile = NULL;	// FILE* Where input is read from
/** \brief Name of input file */
static	char	*s_inputXml = NULL;		// Name of input file

/** \brief Input Parse processing variables */
static	char	s_inputBuffer[INPUT_BUFFER_MAX];

/** \brief Current Depth of input parsing */
static	int		s_depth = 0;

//=============================================================================
/**
 * \brief This function is the constructor to start the input XML generator
 *
 * \param[in]	inputXml - Path to input file to use
 * \return		STATUS_FAIL - Failed to open input XML file
 * 				STATUS_PASS - Ready to process input XML
 */
int16_t
inputXmlStart(char *inputXml)
{
	s_inputFile = NULL;		// Show nothing open yet

	s_inputXml = inputXml;	// Save file input

	// Prepare the input stream
	if (NULL != s_inputXml)
	{
		// A specific input stream was requested, open it
		if (NULL == (s_inputFile = fopen(inputXml, "r")))
		{
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"Failed to open input file [%s]",
					inputXml);
			OUTPUT_ERR(0, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
	}
	else
	{
		s_inputFile = stdin;	// Default to STDIN
		s_inputXml = "stdin";	// For reporting
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function cleans up the input XML processor
 *
 * \param[in]	cfile - File that called function
 * \parma[in]	cline - Line number called from
 * \return		void
 */
void
inputXmlEnd()
{
	// Close the input stream
	if (s_inputFile && (stdin != s_inputFile))
	{
		fclose(s_inputFile);
	}

	// Show input stream is closed
	s_inputFile = NULL;
	s_inputXml = NULL;
}

//=============================================================================
/**
 * \brief This function reports the input file that is open
 *
 * \return		Char * to string shows input file name
 */
const char *
inputXmlGetInput()
{
	return (s_inputXml);
}

//=============================================================================
/**
 * \brief This function is a callback for the start of an
 * 		  element during APIVSXML parsing.
 *
 * \param[in]	pData - Pointer to data structure to fill in
 * \param[in]	pElement - Pointer to element tag
 * \param[in]	pAttr - Pointer to attribute list
 * \return		STATUS_FAIL - Parsing of APIVSXML input file failed
 * 				STATUS_PASS - Parsing of APIVSXML input file was successful
 */
static void
inputXmlStartElement(void *pData, const char *pElement, const char **pAttr)
{
	// See if we have just started
	if (0 == s_depth)
	{
		// We are at the top, so the only valid line
		// is <APIVSXML>.  Ensure this is what we got
		if (STATUS_PASS == pCode_start(pData, pElement, pAttr))
		{
			s_depth++;
			OUTPUT_TRACE("inputXmlStartElement(): PASS");
		}
		else
		{
			OUTPUT_TRACE("inputXmlStartElement(): *** FAIL ***");
		}
	}
	else
	{
		// Parse the opening element
		if (STATUS_PASS == pCode_parseOpen(pData, pElement, pAttr))
		{
			s_depth++;
			OUTPUT_TRACE("inputXmlStartElement(): PASS");
		}
		else
		{
			OUTPUT_TRACE("inputXmlStartElement(): *** FAIL ***");
		}
	}
}

//=============================================================================
/**
 * \brief This function is a callback for the end of an element during APIVSXML
 * parsing.
 *
 * \param[in]	pData - Pointer to data structure to fill in
 * \param[in]	pElement - Pointer to element tag
 * \return		STATUS_FAIL - Parsing of APIVSXML input file failed
 * 				STATUS_PASS - Parsing of APIVSXML input file was successful
 */
static void
inputXmlEndElement(void *pData, const char *pElement)
{
	// Parse the closing of the element
	if (STATUS_PASS == pCode_parseClose(pData, pElement))
	{
		s_depth--;
		OUTPUT_TRACE("inputXmlEndElement(): PASS");
	}
	else
	{
		OUTPUT_TRACE("inputXmlEndElement(): *** FAIL ***");
	}

	// See if we are processing the root
	if (0 == s_depth)
	{
		// We are processing what should be the colsing tag for </APIVSXML>
		pCode_end(pData, pElement);
	}
}

//=============================================================================
/**
 * \brief This function handles data between tags
 *
 * \param[in]	pData - Pointer to data structure to fill in
 * \param[in]	pText - Pointer to text to process, NOT NULL terminated
 * \param[in]	len - Length of text
 * \return		STATUS_FAIL - Parsing of APIVSXML input file failed
 * 				STATUS_PASS - Parsing of APIVSXML input file was successful
 */
static void
inputXmlText(void *pData, const XML_Char *pText, int len)
{
	// See if anything to output
	if (!len)
	{
		return;
	}

	pCode_text(pData, pText, (uint16_t)len);
}

//=============================================================================
/**
 * \brief This function processes the input APIVSXML file
 *
 * \return		STATUS_FAIL - Parsing of APIVSXML input file failed
 * 				STATUS_PASS - Parsing of APIVSXML input file was successful
 */
int16_t
inputXmlParse()
{
	// See if stream is open
	if (NULL == s_inputFile)
	{
		OUTPUT_ERR(0, "Input stream is not open", NULL, NULL);
		return(STATUS_FAIL);
	}

	// Create the parser object
	XML_Parser	pParser = XML_ParserCreate(NULL);

	// Make sure we were able to allocate parser
	if (NULL == pParser)
	{
		OUTPUT_ERR(0,
				   "Failed to allocate APIVSXML parser",
				   strerror(errno),
				   NULL);
		return(STATUS_FAIL);
	}

	// Set up user data
	XML_SetUserData(pParser, pParser);

	// Set up the Element Handlers
	XML_SetElementHandler(pParser, inputXmlStartElement, inputXmlEndElement);

	// Set up the text handler
	XML_SetCharacterDataHandler(pParser, inputXmlText);

	// Process the APIVSXML input file
	for (;;)
	{
		int16_t	done;		// Flag for EOF
		int16_t	len;		// Number of bytes actually read

		// Read a line of input
		len = fread(s_inputBuffer, 1, INPUT_BUFFER_MAX, s_inputFile);

		// See if read was ok
		if (ferror(s_inputFile))
		{
			OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
					   "Failed to read from input APIVSXML",
					   strerror(errno),
					   NULL);
			XML_ParserFree(pParser);
			return(STATUS_FAIL);
		}
		done = feof(s_inputFile);	// Save EOF status

		// Now parse the input line
		if (!XML_Parse(pParser, s_inputBuffer, len, done))
		{
			OUTPUT_ERR(XML_GetCurrentLineNumber(pParser),
					   "Parse failed",
					   NULL,
					   XML_ErrorString(XML_GetErrorCode(pParser)));
			XML_ParserFree(pParser);
			return(STATUS_FAIL);
		}

		// See if an error has been reported
		if (STATUS_FAIL == outputXmlStatusGet())
		{
			// Yes, we are done!
			XML_ParserFree(pParser);
			return(STATUS_FAIL);
		}

		// See if EOF
		if (done)
		{
			break;
		}
	}

	// Show we are done
	XML_ParserFree(pParser);
	return (STATUS_PASS);
}
