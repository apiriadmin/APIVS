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
 * \file main.c
 * This file contains the definition of the \c #Validation Suite Engine (VSE)
 * main
 *
 * \brief Main entry point for VSE
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
#include	"vse_stnd.h"	// VSE Standard Definitions
#include	"vse.h"			// Validation Suite Engine Definitions
#include	"outputXML.h"	// Output XMl Processor Definitions
#include	"inputXML.h"	// Input XMl Processor Definitions
#include	"configFile.h"	// Configuration File Definitions
#include	"pInterpret.h"	// Interpreter Definitions

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
/** \brief Input XML file */
static	char	*s_inputXml = NULL;		// Input XML file 

/** \brief Output XML file */
static	char	*s_outputXml = NULL;	// Output XML file

/** \brief Specific test suite being asked for */
static	char	*s_testSuite = NULL;	// Specific test suite being asked for

/** \brief Output level requested */
static	RUN_LEVEL	s_level = LEVEL_ALWAYS;	// Output level requested

/** \brief Configuration File */
static	char	*s_configFile = NULL;	// Configuration file path

//=============================================================================
/**
 * \brief This function prints out a "usage" statment to stderr
 *
 * \param[in]	argc - Number of command line arguments
 * \param[in]	argv - Vectors to command line arguments
 * \return		void
 */
void
usage(int argc, char **argv)
{
	char	string[OUTPUT_STRING_MAX];

	sprintf(string,
 "Usage: %s -L [1-3] [-i APIVSXML-file] [-o output-file] [-n testsuite-name] [-c configuration-file] [-C]\n",
 			argv[0]);
	fprintf(stderr, string);
}

//=============================================================================
/**
 * \brief This is function processes the command line arguments
 *
 * Command Line:
 * 	vse [-i APIVSXML-file]  [-o output-file] [-n testsuite-name] -L [1-3]
 *
 * \param[in]	argc - Number of command line arguments
 * \param[in]	argv - Vectors to command line arguments
 * \return		STATUS_PASS - Command line ok
 * 				STATUS_FAIL - Command line had problem
 */
int16_t
processCommandLine(int argc, char **argv)
{
	int	option;		// Option that needs processing

	// Process the command line arguments
	while (-1 != (option = getopt(argc, argv, "i:o:n:L:c:C")))
	{
		switch (option)
		{
			case 'i':
			{
				s_inputXml = optarg;
			}
			break;

			case 'o':
			{
				s_outputXml = optarg;
			}
			break;

			case 'n':
			{
				s_testSuite = optarg;
			}
			break;

			case 'L':
			{
				s_level = atoi(optarg);
			}
			break;

			case 'c':
			{
				s_configFile = optarg;
			}
			break;

			case 'C':
			{
				interpretSetCapture();
			}
			break;

			default:			// A bad argument was passed
			{
				// Show caller that bad argument was passed
				return(STATUS_FAIL);
			}
			break;
		}
	}

	// See if required arguments given
	if ((LEVEL_CONFORMANCE > s_level) || (LEVEL_TRACE < s_level))
	{
		return(STATUS_FAIL);
	}

	return(STATUS_PASS);		// Show success to caller
}

//=============================================================================
/**
 * \brief This routine is used to set up processing for the VSE
 * This routine uses local static variables:
 * 		s_inputXml
 * 		s_outputXml
 * 		s_testSuite
 * 		s_level
 *
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int
setUpProcessing()
{
	static char	outPath[OUTPUT_STRING_MAX];
	static char	inPath[OUTPUT_STRING_MAX];
	char		*pPath;

	// Process the configuration file
	if (STATUS_FAIL == configFileProcess(s_configFile))
	{
		return(STATUS_FAIL);	// We failed processing the configuration file
	}

	// Initialize the argument processor
	if (STATUS_FAIL == argInit())
	{
		return(STATUS_FAIL);
	}

	pPath = s_outputXml;
	if (NULL != s_outputXml)
	{
		sprintf(outPath, "%s%s", configFileGetXOFP(), s_outputXml);
		pPath = outPath;
	}

	// Set up the output processor
	if (STATUS_FAIL == outputXmlStart(s_level, pPath))
	{
		return(STATUS_FAIL);	// We failed to start output processor
	}

	pPath = s_inputXml;
	if (NULL != s_inputXml)
	{
		sprintf(inPath, "%s%s", configFileGetXIFP(), s_inputXml);
		pPath = inPath;
	}

	// Prepare the input stream
	if (STATUS_FAIL == inputXmlStart(pPath))
	{
		return(STATUS_FAIL);	// We failed to start the input processor
	}

	return(STATUS_PASS);		// So far so good
}

//=============================================================================
/**
 * \brief This is the main entry point for the Validation Suite Engine (VSE).
 * The main processes the command line arguments and hands control off to
 * the APIVSXML language interpreter / processor.
 *
 * We purposefully do NOT clean up memory on exit of the process.
 * Since no memory will be reused, we allow the OS to clean up
 * used memory as the program exits.
 *
 * \param[in]	argc - Number of command line arguments
 * \param[in]	argv - Vectors to command line arguments
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int
main(int argc, char **argv)
{
	int16_t	statusCode;			// Status code for run

	// Process the command line
	if (processCommandLine(argc, argv))
	{
		// Command Line had some issues
		usage(argc, argv);		// Output a usage statement
		return(STATUS_FAIL);	// Show FAIL to the outside world
	}

	// Start the output processor
	if (STATUS_FAIL == setUpProcessing())
	{
		return(STATUS_FAIL);	// We failed to start output processor
	}

	// Output starting XML tag
	outputXmlTagCurrentString(LEVEL_ALWAYS,
							  XML_TAG,
							  XML_QUESTION_MARK,
							  XML_QUESTION_MARK);
	outputXmlNewLine(LEVEL_ALWAYS);

	// Start the Output XML Generator
	outputXmlAttrAddDate();
	outputXmlAttrAdd(ATTR_CONFIG, s_configFile);
	outputXmlAttrAdd(ATTR_INPUT, inputXmlGetInput());
	outputXmlAttrAdd(ATTR_OUTPUT, outputXmlGetOutput());
	outputXmlAttrAdd(ATTR_TESTSUITE,
					 (NULL != s_testSuite) ? s_testSuite : "ALL_TESTS");
	outputXmlAttrAdd(ATTR_LEVEL, outputXmlGetLevel());
	outputXmlTagOpen(LEVEL_ALWAYS, TAG_APIVSRUN, outputXmlAttrGet());
	outputXmlNewLine(LEVEL_ALWAYS);

	// Let the VSE do its stuff!!!
	statusCode = vse(s_testSuite);

	// Output the final status
	outputXmlNewLine(LEVEL_CONFORMANCE);
	outputXmlAttrAddDate();
	outputXmlAttrAdd(ATTR_STATUS,
					 (STATUS_PASS == statusCode) ? VALUE_PASS : VALUE_FAIL);
	outputXmlTagCurrent(LEVEL_CONFORMANCE, TAG_RUNRESULT, outputXmlAttrGet());
	outputXmlNewLine(LEVEL_CONFORMANCE);

	// Output final closing element tag
	outputXmlTagClose(LEVEL_ALWAYS, TAG_APIVSRUN);

	// close input
	inputXmlEnd();

	// close output
	outputXmlEnd();

	// Report status to caller
	return(statusCode);
}
