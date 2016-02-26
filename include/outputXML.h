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
 * \file outputXML.h
 *
 * This file contains the definition of the \c #The Output XML Processor
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief Output XML Processor
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef	OUTPUTXML_H
#define	OUTPUTXML_H

//=============================================================================
/**
 * Includes
 */

#include "vse.h"        // VSE Definitions
#include "pCode.h"      // P_CODE Definitions

//=============================================================================
/**
 * Defines
 */

/** \brief Test Run PASS */
#define	STATUS_PASS (0)        // Status of a PASS run
/** \brief Test Run FAIL */
#define	STATUS_FAIL (-1)       // Status of a FAIL run

/** \brief Output string buffer */
#define	OUTPUT_STRING_MAX (256)

/** \brief Output Element Tags */
#define	XML_TAG                "xml version=\"1.0\" encoding=\"utf-8\""
#define	XML_QUESTION_MARK      '?'
#define	XML_CLOSE              '/'
#define	TAG_APIVSRUN           "ApiVsRun"
#define	ATTR_CONFIG            "configuration"
#define	ATTR_INPUT             "input"
#define	ATTR_OUTPUT            "output"
#define	ATTR_TESTSUITE         "testSuite"
#define	ATTR_LEVEL             "level"

#define	TAG_RUNRESULT          "RunResult"
#define	ATTR_STATUS            "status"
#define	VALUE_PASS             "PASS"
#define	VALUE_FAIL             "FAIL"

#define	TAG_ERROR              "Error"
#define	ATTR_ERROR             "error"
#define	ATTR_ERRNO             "errno"
#define	ATTR_DESC              "description"
#define	ATTR_LINE              "line"

#define	TAG_INFO               "Info"
#define	ATTR_INFO              "information"

#define	DATE_FORMAT            "%F %r %Z"
#define	ATTR_DATE              "date"

#define ATTR_TIME              "timestamp"
//=============================================================================
/**
 * Macro Defines
 */
/** \brief Macro to write error to output file */
#define	OUTPUT_ERR(lineNo, errStr, errnoStr, desc) \
		outputXmlErr(lineNo, errStr, errnoStr, desc)

/** \brief Macro to write infomation to output file */
#define	OUTPUT_INFO(lineNo, pInfo, pDesc) \
		outputXmlInfo(lineNo, pInfo, pDesc)

/** \brief Macro for setting a debug trace */
// #define	OUTPUT_TRACE(string)			outputXmlTrace(string, __FILE__, __LINE__)
#define	OUTPUT_TRACE(string)

/** \brief Macro for defining attribute array */
#define	ATTR_ARRAY_MAX	(20)

//=============================================================================
/**
 * Extern
 */

/** \brief Get the current run atatus */
extern int outputXmlStatusGet();

/** \brief Should a print / dump occur? */
extern int outputXmlShouldPrint(const RUN_LEVEL level);

/** \brief Definition for Output XML constructor */
extern int16_t	outputXmlStart(const RUN_LEVEL level, const char *outputXml);

/** \brief Definition for Output XML destructor */
extern void outputXmlEnd();

/** \brief Defintion for Output XML trace to STDERR */
extern void outputXmlTrace(const char *string, const char *file,
						int16_t line);

/** \brief Definition for Output XML Error to APIOUTXML File */
extern void outputXmlErr(const int16_t lineNo, const char *errStr,
						const char *errnoStr,
						const char *desc);

/** \brief Definition for Output XML Info to APIOUTXML File */
extern void outputXmlInfo(const int16_t lineNo, const char *pInfo,
						const char *pDesc);

/** \brief Function for outputting an opening tag <tag attr-list> */
extern void outputXmlTagOpen(const RUN_LEVEL level, const char *tag,
						const char **attr);

/** \brief Function for outputting <tag attr-list /> */
extern void outputXmlTagCurrent(const RUN_LEVEL level, const char *tag,
						const char **attr);

/** \brief Function for outputting <? string ?>*/
extern void outputXmlTagCurrentString(const RUN_LEVEL level,
						const char *string,
						const char open,
						const char close);

/** \brief Function for outputting a closing tag </tag> */
extern void outputXmlTagClose(const RUN_LEVEL level, const char *tag);

/** \brief Function for outputting text <tag>text</tag> */
extern void outputXmlText(const RUN_LEVEL level, const char *pText);

/** \brief Function for outputting a newline <tag>[newline]</tag> */
extern void outputXmlNewLine(const RUN_LEVEL level);

/** \brief Definition for getting output file name */
extern const char *outputXmlGetOutput();

/** \brief Definition for getting output level */
extern const char *outputXmlGetLevel();

/** \brief Definition for adding an attribute to array */
extern void outputXmlAttrAdd(const char *name, const char *value);

/** \brief Definition for adding a date attribute to array */
extern void outputXmlAttrAddDate();

/** \brief Definition for adding a timestamp attribute to array */
extern void outputXmlAttrAddTimeStamp();

/** \brief Function for adding common attributes to tag */
extern void outputXmlAttrAddCommon(const RUN_LEVEL level,
					P_COMMON *pCommon, char *pName);

/** \brief Show the VD in the output file */
extern void outputXmlShowVD(const char	*desc);

/** \brief Show the CMD in the output file */
extern void outputXmlShowCMD(int frame);

/** \brief Function for getting attribute list */
extern const char **outputXmlAttrGet();

/** \brief Function to perform a hexdump of a buffer */
extern void outputXmlHex(const RUN_LEVEL level, const char *pName,
				void *pBuf, unsigned int size);

//=============================================================================
/**
 * Structure definitions
 */

#endif	/*OUTPUTXML_H */
