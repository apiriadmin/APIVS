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
 * \file outputXML.c
 *
 * This file contains the definition of functions for processing the output
 * XML
 *
 * \brief Output XML processing
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
#include	<time.h>		// STD time Definitions
#include	<vt100.h>		// VT100 Definitions
#include	<emfio.h>		// EM FIO Definitions
#include	"outputXML.h"	// Output XMl Processor Definitions

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */
/** \brief Output the current indent level */
static	void	outputIndent();		// Output the current indent level

//=============================================================================
/**
 * Local Variables
 */

/** \brief  Where to place the output */
static	FILE	*s_outputFile = NULL;		// FILE* Where to place the output

/** \brief Name of output file */
static	const char	*s_outputXml = "stdout";	// Name of output file

/** \brief  Output level desired */
static	RUN_LEVEL	s_level = LEVEL_ALWAYS;	// Output level desired

/** \brief Current status of run */
static	int	s_status = STATUS_PASS;		// We are PASS until a FAIL condition

/** \brief Current indent level */
static	int16_t	s_indent = 0;				// Current indent level

/** \brief Static table mapping level #'s to strings for display */
static	char	*s_level_string[LEVEL_MAX] = 
{
	LVL_0,
	LVL_1,
	LVL_2,
	LVL_3
};


/** \brief Static table for housing an attribute list for outputting an element
 *         Allow ATTR_ARRAY_MAX attributes (name / value pairs) + 1
 *         for a NULL always to end list
 */
static	const char		*s_attrArray[(ATTR_ARRAY_MAX * 2) + 1] = { NULL };
/** \brief Where to add next name / value pair */
static	const char		**s_attrNext = &s_attrArray[0];	// next element
static struct timeval	start_tv;
//=============================================================================
/**
 * \brief This function reports the current status
 *
 * \return		int	STATUS_PASS - We are ok
 * 				    STATUS_FAIL - Something bad happened
 */
int
outputXmlStatusGet()
{
	return(s_status);
}

//=============================================================================
/**
 * \brief This function tells an extern library if something should be printed
 *
 * \param[in]	level - Run level in question
 * \return		int	TRUE - Print should happen
 * 				    FALSE - Print should not
 */
int
outputXmlShouldPrint(const RUN_LEVEL	level)
{
	return(level <= s_level);
}

//=============================================================================
/**
 * \brief This function is the constructor to start the output XML generator.
 * 		  The default output place is stdout
 *
 * \param[in]	outputXml - Path to output file to use
 * \return		STATUS_FAIL - Failed to open output XML file
 * 				STATUS_PASS - Ready to process output XML
 */
int16_t
outputXmlStart(RUN_LEVEL level, const char *outputXml)
{
	FILE	*outputFile;	// Temporary Output File *

	s_outputFile = stdout;	// Default to stdout
	s_level = level;		// Save the output level
	s_indent = 0;			// Start with no indent

	// Prepare the output stream
	if (NULL != outputXml)
	{
		// A specific output stream was requested, open it (append)
		if (NULL == (outputFile = fopen(outputXml, "a")))
		{
			// Output an error message
			char	string[OUTPUT_STRING_MAX];

			sprintf(string,
					"Failed to open output file [%s]",
					outputXml);
			OUTPUT_ERR(0, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		s_outputFile = outputFile;	
		s_outputXml = outputXml;
	}

	gettimeofday(&start_tv, NULL);
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function cleans up the output XML generator and reports
 * the final run status
 *
 * \param[in]	cfile - C File that made call
 * \param[in]	cline - Line in C file where call was made from
 * \return		void
 */
void
outputXmlEnd(const char *file, const int line)
{
	// See if output stream is open
	// Close the output stream
	if ((stderr != s_outputFile) && (stdout != s_outputFile))
	{
		fclose(s_outputFile);
	}

	// Show output stream is closed
	// Default to stdout, JUST IN CASE WE TRY TO OUPTU SOMETHING AFTER CLOSE
	s_outputFile = stdout;
	s_outputXml = "stdout";
}

//=============================================================================
/**
 * \brief This function outputs spaces space based upon the current indent level
 *
 * \return		void
 */
static void
outputIndent()
{
	int16_t	ii;							// Loop variable
	char	string[OUTPUT_STRING_MAX];	// String buffer
	char	*ptr;						// Pointer to next output pos

	ptr = string;						// Start at beginning of buffer
	for (ii = 0; ii < s_indent; ii++)
	{
		*ptr++ = ' ';					// Place
		*ptr++ = ' ';					// Spaces
		*ptr++ = ' ';					// Spaces
		*ptr++ = ' ';					// Spaces
	}
	*ptr = '\0';						// Null terminate

	fprintf(s_outputFile, string);
}

//=============================================================================
/**
 * \brief This function outputs an trace message to outfile
 *
 * \param[in]	string - String to output to outfile
 * \param[in]	file - File trace done from
 * \param[in]	line - Line trace done from
 * \return		void
 */
void
outputXmlTrace(const char *string, const char *file, int16_t line)
{
	// Output the current indent level
	outputIndent();

	// Report to outfile
	fprintf(s_outputFile,
			"**TRACE --> File[%s], line[%d]: %s\n",
			file, line, string);
}

//=============================================================================
/**
 * \brief This function outputs an error message to outfile
 *
 * \param[in]	lineNo - Line number in APIVSXML file, 0 = no line
 * \param[in]	errStr - A formatted error string to output to outfile
 * \param[in]	errnoStr - A formatted errno string to output to outfile
 * \param[in]	desc - A description string to output
 * \return		void
 */
void
outputXmlErr(const int16_t lineNo,
			 const char *errStr,
			 const char *errnoStr,
			 const char *desc)
{
	// Construct the error element to output
	outputXmlAttrAddTimeStamp();		// Date ATTR is first

	if (lineNo)
	{
		static char lineNoStr[OUTPUT_STRING_MAX];
		sprintf(lineNoStr, "%d", lineNo);
		outputXmlAttrAdd(ATTR_LINE, lineNoStr);
	}

	if (NULL != errStr)
	{
		outputXmlAttrAdd(ATTR_ERROR, errStr);
	}

	if (NULL != errnoStr)
	{
		outputXmlAttrAdd(ATTR_ERRNO, errnoStr);
	}

	if (NULL != desc)
	{
		outputXmlAttrAdd(ATTR_DESC, desc);
	}

	// Output the element and its attributes
	outputXmlTagCurrent(LEVEL_ALWAYS, TAG_ERROR, outputXmlAttrGet());

	// Show we failed
	s_status = STATUS_FAIL;
}

//=============================================================================
/**
 * \brief This function outputs an information message to outfile
 *
 * \param[in]	lineNo - Line number in APIVSXML file, 0 = no line
 * \param[in]	pInfo - A formatted error string to output to outfile
 * \param[in]	pDesc - A description string to output
 * \return		void
 */
void
outputXmlInfo(const int16_t lineNo,
			  const char 	*pInfo,
			  const char 	*pDesc)
{
	// Construct the information element to output
	outputXmlAttrAddTimeStamp();		// Time ATTR is first

	if (lineNo)
	{
		static char lineNoStr[OUTPUT_STRING_MAX];
		sprintf(lineNoStr, "%d", lineNo);
		outputXmlAttrAdd(ATTR_LINE, lineNoStr);
	}

	if (NULL != pInfo)
	{
		outputXmlAttrAdd(ATTR_INFO, pInfo);
	}

	if (NULL != pDesc)
	{
		outputXmlAttrAdd(ATTR_DESC, pDesc);
	}

	// Output the element and its attributes
	outputXmlTagCurrent(LEVEL_ALWAYS, TAG_INFO, outputXmlAttrGet());
}

//=============================================================================
/**
 * \brief This function outputs info to the Output XML
 * file for the current indent
 * Uses attrArray
 *
 * \param[in]	level - Run level this output applies to
 * \param[in]	tag - Tag for this element
 * \param[in]	attr - Attribute list, NULL terminated
 * \return		void
 */
void
outputXmlTagCurrent(const RUN_LEVEL level,
					const char *tag,
					const char **attr)
{
	// See if this output should be displayed
	if (level <= s_level)
	{
		int16_t	attributes = 0;

		// Output the current indent level
		outputIndent();

		// Open element
		fprintf(s_outputFile, "<%s", tag);

		// Process all attributes
		while (NULL != *attr)
		{
			attributes++;
			fprintf(s_outputFile, " %s=\"%s\"", *attr, *(attr+1));
			attr += 2;	// Bump to next name / value pair
		}

		// End in a close tag, CR
		fprintf(s_outputFile, "%s/>\n", (0 == attributes) ? "" : " ");
	}

	// Zero out attr array for next element
	s_attrNext = &s_attrArray[0];
	s_attrArray[0] = NULL;
}

//=============================================================================
/**
 * \brief This function outputs info to the Output XML file for
 * 		  the current indent
 *
 * \param[in]	level - Run level this output applies to
 * \param[in]	string - Text to output
 * \param[in]	open - Open tag character (if any, '\0' if not)
 * \param[in]	close - Close tag character (if any, '\0' if not)
 * \return		void
 */
void
outputXmlTagCurrentString(const RUN_LEVEL level,
						  const char *string,
						  const char *open,
						  const char *close)
{
	// See if this output should be displayed
	if (level <= s_level)
	{
		// Output the current indent level
		outputIndent();

		// Open element
		fprintf(s_outputFile, "<");

		// Open tag character
		if (open != NULL)
		{
			fprintf(s_outputFile, "%s", open);
		}

		// Output requested tag
		fprintf(s_outputFile, string);

		// End in a close tag, CR
		if (close != NULL)
		{
			fprintf(s_outputFile, "%s", close);
		}
		fprintf(s_outputFile, ">\n");
	}

	// Zero out attr array for next element
	s_attrNext = &s_attrArray[0];
	s_attrArray[0] = NULL;
}

//=============================================================================
/**
 * \brief This function outputs text to the output XML file
 *
 * \param[in]	level - Run level this output applies to
 * \param[in]	text - Text to output
 * \return		void
 */
void
outputXmlText(const RUN_LEVEL level, const char *pText)
{
	outputXmlTagOpen(level,"Text",outputXmlAttrGet());
	// See if this output should be displayed
	if (level <= s_level)
	{
		// Output the current indent level
		outputIndent();
		// Output requested tag
		fprintf(s_outputFile, "\"%s\"\n", pText);
	}
	outputXmlTagClose(level, "Text");
}

//=============================================================================
/**
 * \brief This function outputs a newline to the output XML file
 *
 * \param[in]	level - Run level this output applies to
 * \return		void
 */
void
outputXmlNewLine(const RUN_LEVEL level)
{
	// See if this output should be displayed
	if (level <= s_level)
	{
		// Output requested newline
		fprintf(s_outputFile, "\n");
	}
}

//=============================================================================
/**
 * \brief This function outputs info to the Output XML file for an opening tag
 *
 * \param[in]	level - Run level this output applies to
 * \param[in]	tag - Tag for this element
 * \param[in]	attr - Array of attribute name / value pairs, NULL terminated
 * \return		void
 */
void
outputXmlTagOpen(const RUN_LEVEL level,
				 const char *tag,
				 const char **attr)
{
	// See if this output should be displayed
	if (level <= s_level)
	{
		int16_t	attributes = 0;

		// Output the current indent level
		outputIndent();

		// Open element
		fprintf(s_outputFile, "<%s", tag);

		// Process all attributes
		while (NULL != *attr)
		{
			attributes++;
			fprintf(s_outputFile, " %s=\"%s\"", *attr, *(attr+1));
			attr += 2;	// Bump to next name / value pair
		}

		// End in a close tag, CR
		fprintf(s_outputFile, "%s>\n", (0 == attributes) ? "" : " ");

		// Show we are indented
		s_indent++;	
	}

	// Zero out attr array for next element
	s_attrNext = &s_attrArray[0];
	s_attrArray[0] = NULL;
}

//=============================================================================
/**
 * \brief This function outputs info to the Output XML file for a closing tag
 *
 * \param[in]	tag - Tag for this element
 * \return		void
 */
void
outputXmlTagClose(const RUN_LEVEL level, const char *tag)
{
	// See if this output should be displayed
	if (level <= s_level)
	{
		// Show we are not indented
		s_indent--;	

		// Output current indent level
		outputIndent();

		// Output close element
		fprintf(s_outputFile, "</");

		// Output the string
		fprintf(s_outputFile, tag);

		// End in a close tag, CR
		fprintf(s_outputFile, ">\n");
	}
}

//=============================================================================
/**
 * \brief This function reports the output file that is open
 *
 * \return		Char * to string shows output file name
 */
const char *
outputXmlGetOutput()
{
	return (s_outputXml);
}

//=============================================================================
/**
 * \brief This function reports the level string that was requested
 *
 * \return		Char * to string shows the level
 */
const char *
outputXmlGetLevel()
{
	return (s_level_string[s_level]);
}

//=============================================================================
/**
 * \brief This function adds an attribute to the attribute array
 *
 * \param[in]	name - Pointer to name of attribute
 * \param[in]	value - Pointer to value of attribute, can be NULL
 * \return		void
 */
void
outputXmlAttrAdd(const char *name, const char *value)
{
	// Make sure something to add
	if (NULL == value)
	{
		return;
	}

	// Make sure array is not full
	if (s_attrNext >= &s_attrArray[ATTR_ARRAY_MAX * 2])
	{
		fprintf(stderr,
				"Program error: Attribute array is full, cannot add [%s=\"%s\"]",
				name,
				value);

		// Show we failed
		s_status = STATUS_FAIL;

		return;
	}

	// Place name / value pair in array
	*s_attrNext++ = name;
	*s_attrNext++ = value;
	*s_attrNext = NULL;		// Make sure NULL terminated
}

//=============================================================================
/**
 * \brief This function adds the date attribute to the attribute array
 *
 * \return		void
 */
void
outputXmlAttrAddDate()
{
	time_t			tm;
	struct tm		*tms;
	static char		timeString[OUTPUT_STRING_MAX];

	// Add the time to the attribute list
	tm = time(NULL);
	tms = gmtime(&tm);
	strftime(timeString, sizeof(timeString), DATE_FORMAT, tms);
	
	outputXmlAttrAdd(ATTR_DATE, timeString);
}

//=============================================================================
/**
 * \brief This function adds the time attribute to the attribute array
 *
 * \return		void
 */
void
outputXmlAttrAddTimeStamp()
{
	struct timeval tv, current_tv;
	static char timeString[OUTPUT_STRING_MAX];

	// Add the timestamp to the attribute list
	gettimeofday(&current_tv, NULL);
	timersub(&current_tv, &start_tv, &tv);
	sprintf(timeString, "%+ld.%06ld", tv.tv_sec, tv.tv_usec);
	outputXmlAttrAdd(ATTR_TIME, timeString);
}

//=============================================================================
/**
 * \brief This function returns the s_attribute array so it can be used
 *
 * \return		char ** - Address of s_attrArray
 */
const char **
outputXmlAttrGet()
{
	return(&s_attrArray[0]);
}

//=============================================================================
/**
 * \brief This function adds attributes based upon the print flags
 *
 * \param[in]	level - Run level this output applies to
 * \param[in]	pCommon - Pointer to common print information
 * \param[in]	pName - Pointer to attribute for name
 * \return		void
 */
void
outputXmlAttrAddCommon(const RUN_LEVEL level, P_COMMON *pCommon, char *pName)
{
	// See if this output should be displayed
	if (level <= s_level)
	{
		// We want to add the attributes
		static char	string[OUTPUT_STRING_MAX];
		sprintf(string, "%d", pCommon->lineNumber);
		outputXmlAttrAdd(ATTR_LINE, string);

		// Add the time stamp
		outputXmlAttrAddTimeStamp();

		// Add the name if it is there
		outputXmlAttrAdd(pName, pCommon->pName);

		// Then add the desciption
		outputXmlAttrAdd(A_DESC, pCommon->pDesc);
	}
}

//=============================================================================
/**
 * \brief This function dumps the VD to the output file
 *
 * \param[in]	pDesc - Pointer to description
 * \return		void
 */
void
outputXmlShowVD(const char *desc)
{
	vt100_showVD(s_outputFile, desc);
}

//=============================================================================
/**
 * \brief This function dumps the CMD to the output file
 *
 * \param[in]	frame - CMD frame to show
 * \return		void
 */
void
outputXmlShowCMD(int frame)
{
	emfio_showCommand(frame, s_outputFile);
}

//=============================================================================
/**
 * \brief Dump a buffer in Hex format
 *
 * \param[in]	pFile - FILE * to dump to
 * \param[in]	pName - Name attribute
 * \param[in]	pBuf - Buffer to hexdump
 * \param[in]	size - Number of bytes to dump
 * 
 * \return		STATUS_FAIL = Operation failed
 *              STATUS_SUCCESS = Operation succeeded
 */
void
outputXmlHex(const RUN_LEVEL level, const char *pName, void *pBuf, unsigned int size)
{
	unsigned int ii;
	void *end;
	void *tmpptr;
	void *tmpend;
	char string[OUTPUT_STRING_MAX];
	
	if (level > s_level)
	{
		return;
	}

	sprintf(string, "%d", size);
	outputXmlAttrAdd("length", string);
	outputXmlTagOpen(level,"HexDump",outputXmlAttrGet());
	// The bytes
	end = pBuf + size;
	while (pBuf < end)
	{
		outputIndent();
		tmpptr = pBuf;
		tmpend = pBuf + 16;
		if (tmpend > end)
		{
			tmpend = end;
		}

		// Output the hex codes
		ii = 0;
		while (tmpptr < tmpend)
		{
			fprintf(s_outputFile, "%02x ", *(uint8_t *)tmpptr);
			tmpptr++;
			ii++;
		}
		for (; ii < 16; ii++)
		{
			fprintf(s_outputFile, "   ");
		}

		fprintf(s_outputFile, "\n");

		pBuf = tmpend;
	}
	outputXmlTagClose(level, "HexDump");
}
