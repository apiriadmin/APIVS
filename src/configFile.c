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
 * \file configFile.c
 * This file contains the definition of the \c cofiguration file
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief configuration file processor
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
#include	"vse_stnd.h"	// Validation Suite Engine Definitions
#include	"outputXML.h"	// Output APIOUTXML processor definitions
#include	"configFile.h"	// Configuration File Definitions

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */

#define	LINE_MAX	(256)

//=============================================================================
/**
 * Local Variables
 */

/** \brief Default path for all paths */
static	char	s_defaultPath[] = "./";

/** \brief Input XML file path */
static	char	*s_XMLInputFilePath = s_defaultPath;

/** \brief Output XML file path */
static	char	*s_XMLOutputFilePath = s_defaultPath;

/** \brief Set file path */
static	char	*s_SetFilePath = s_defaultPath;

/** \brief Screen Height */
static	int		s_screenHeight = CF_SCREEN_HEIGHT;

/** \brief Screen Width */
static	int		s_screenWidth = CF_SCREEN_WIDTH;

/** \brief FPUI Loopback Device */
static	char	*s_FPUILoopbackDevice = NULL;

/** \brief FPUI Compare File Path */
static	char	*s_FPUICompareFilePath = s_defaultPath;

/** \brief FPUI Input File Path */
static	char	*s_FPUIInputFilePath = s_defaultPath;

/** \brief FPUI Dump File Path */
static	char	*s_FPUIDumpFilePath = s_defaultPath;

/** \brief FIO Loopback Device */
static	char	*s_FIOLoopbackDevice = NULL;

/** \brief FIO Compare File Path */
static	char	*s_FIOCompareFilePath = s_defaultPath;

/** \brief FIO Response File Path */
static	char	*s_FIOResponseFilePath = s_defaultPath;

/** \brief FIO Dump File Path */
static	char	*s_FIODumpFilePath = s_defaultPath;

//=============================================================================
/**
 * \brief This routine returns the configurated XML Input File Path
 *
 * \return		The configured value
 */
const char *
configFileGetXIFP()
{
	return(s_XMLInputFilePath);
}

//=============================================================================
/**
 * \brief This routine returns the configurated XML Output File Path
 *
 * \return		The configured value
 */
const char *
configFileGetXOFP()
{
	return(s_XMLOutputFilePath);
}

//=============================================================================
/**
 * \brief This routine returns the configurated Set File Path
 *
 * \return		The configured value
 */
const char *
configFileGetSFP()
{
	return(s_SetFilePath);
}

//=============================================================================
/**
 * \brief This routine returns the configurated Screen Height
 *
 * \return		The configured value
 */
int
configFileGetSH()
{
	return(s_screenHeight);
}

//=============================================================================
/**
 * \brief This routine returns the configurated Screen Width
 *
 * \return		The configured value
 */
int
configFileGetSW()
{
	return(s_screenWidth);
}

//=============================================================================
/**
 * \brief This routine returns the configurated FPUI Compare File Path
 *
 * \return		The configured value
 */
const char *
configFileGetFPUICFP()
{
	return(s_FPUICompareFilePath);
}

//=============================================================================
/**
 * \brief This routine returns the configurated FPUI Input File Path
 *
 * \return		The configured value
 */
const char *
configFileGetFPUIIFP()
{
	return(s_FPUIInputFilePath);
}

//=============================================================================
/**
 * \brief This routine returns the configurated FPUI Dump File Path
 *
 * \return		The configured value
 */
const char *
configFileGetFPUIDFP()
{
	return(s_FPUIDumpFilePath);
}

//=============================================================================
/**
 * \brief This routine returns the configurated FPUI Loopback Device
 *
 * \return		The configured value
 */
const char *
configFileGetFPUILBDEV()
{
	return(s_FPUILoopbackDevice);
}

//=============================================================================
/**
 * \brief This routine returns the configurated FIO Compare File Path
 *
 * \return		The configured value
 */
const char *
configFileGetFIOCFP()
{
	return(s_FIOCompareFilePath);
}

//=============================================================================
/**
 * \brief This routine returns the configurated FIO Responce File Path
 *
 * \return		The configured value
 */
const char *
configFileGetFIORFP()
{
	return(s_FIOResponseFilePath);
}

//=============================================================================
/**
 * \brief This routine returns the configurated FIO Dump File Path
 *
 * \return		The configured value
 */
const char *
configFileGetFIODFP()
{
	return(s_FIODumpFilePath);
}

//=============================================================================
/**
 * \brief This routine returns the configurated FIO Loopback Device
 *
 * \return		The configured value
 */
const char *
configFileGetFIOLBDEV()
{
	return(s_FIOLoopbackDevice);
}

//=============================================================================
/**
 * \brief This routine is used to parse a line of the configuration file
 *
 * \param[in]	fileName - File being processed
 * \param[in]	ln - Line number of configuration file
 * \parma[in]	configLine - Input line from configuration file
 * \param[out]	process - TRUE if line should be processed, FALSE if ignore
 * \param[out]	name - The name portion of the setting
 * \param[out]	value - The value portion of the setting
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
configFileParse(char	*fileName,
				int		ln,
				char 	*configLine,
				boolean	*process,
				char	*name,
				char	*value)
{
	char	*pFrom;
	char	*pTo;

	*process = FALSE;

	// Process possible blanks at the beginning
	pFrom = configLine;
	while (   (*pFrom == CF_SPACE_CHAR)
		   || (*pFrom == CF_TAB_CHAR))

	{
		pFrom++;
	}

	// Eliminate lines we don't care about
	if (   (*pFrom == CF_COMMENT_LINE_CHAR)
		|| (*pFrom == CF_EOL_CHAR))
	{
		return(STATUS_PASS);
	}

	if (*pFrom == CF_SEPARATOR_CHAR)
	{
		// Bad Syntax
		fprintf(stderr,
				"Failed to parse configuration file [%s] line number [%d], not of form [name %c value]\n",
				fileName,
				ln,
				CF_SEPARATOR_CHAR);
		fprintf(stderr, "Line = [%s]\n", configLine);
		return(STATUS_FAIL);
	}

	// Now parse out the name and the value
	pTo = name;
	while (   (*pFrom != CF_SPACE_CHAR)
		   && (*pFrom != CF_TAB_CHAR)
		   && (*pFrom != CF_EOL_CHAR)
		   && (*pFrom != CF_NULL_CHAR)
		   && (*pFrom != CF_SEPARATOR_CHAR))

	{
		*pTo++ = *pFrom++;
	}
	*pTo = '\0';

	while (   (*pFrom == CF_SPACE_CHAR)
		   || (*pFrom == CF_TAB_CHAR))
	{
		pFrom++;
	}

	if (*pFrom != CF_SEPARATOR_CHAR)
	{
		// Bad Syntax
		fprintf(stderr,
				"Failed to parse configuration file [%s] line number [%d], not of form [name %c value]\n",
				fileName,
				ln,
				CF_SEPARATOR_CHAR);
		fprintf(stderr, "Line = [%s]\n", configLine);
		return(STATUS_FAIL);
	}
	pFrom++;

	while (   (*pFrom == CF_SPACE_CHAR)
		   || (*pFrom == CF_TAB_CHAR))
	{
		pFrom++;
	}

	pTo = value;
	while (   (*pFrom != CF_SPACE_CHAR)
		   && (*pFrom != CF_TAB_CHAR)
		   && (*pFrom != CF_EOL_CHAR)
		   && (*pFrom != CF_NULL_CHAR))

	{
		*pTo++ = *pFrom++;
	}
	*pTo = '\0';

	while (   (*pFrom == CF_SPACE_CHAR)
		   || (*pFrom == CF_TAB_CHAR))
	{
		pFrom++;
	}

	if (   (*pFrom != CF_EOL_CHAR)
	    && (*pFrom != CF_NULL_CHAR))
	{
		// Bad Syntax
		fprintf(stderr,
				"Failed to parse configuration file [%s] line number [%d], extra characters\n",
				fileName,
				ln);
		fprintf(stderr, "Line = [%s]\n", configLine);
		return(STATUS_FAIL);
	}

	*process = TRUE;
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This routine interprets each configuration line
 *
 * \param[in]	fileName - File being processed
 * \param[in]	ln - Line number of configuration file
 * \param[in]	name - The name portion of the setting
 * \param[in]	value - The value portion of the setting
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
configFileInterpret(char	*fileName,
					int		ln,
					char	*name,
					char	*value)
{
	if (!strcmp(name, CF_XIFP))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_XMLInputFilePath = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_XMLInputFilePath, value, len);
	}
	else if (!strcmp(name, CF_XOFP))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_XMLOutputFilePath = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_XMLOutputFilePath, value, len);
	}
	else if (!strcmp(name, CF_SFP))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_SetFilePath = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_SetFilePath, value, len);
	}
	else if (!strcmp(name, CF_SH))
	{
		s_screenHeight = atoi(value);
	}
	else if (!strcmp(name, CF_SW))
	{
		s_screenWidth = atoi(value);
	}
	else if (!strcmp(name, CF_FPUICFP))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_FPUICompareFilePath = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_FPUICompareFilePath, value, len);
	}
	else if (!strcmp(name, CF_FPUIIFP))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_FPUIInputFilePath = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_FPUIInputFilePath, value, len);
	}
	else if (!strcmp(name, CF_FPUIDFP))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_FPUIDumpFilePath = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_FPUIDumpFilePath, value, len);
	}
	else if (!strcmp(name, CF_FPUILBDEV))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_FPUILoopbackDevice = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_FPUILoopbackDevice, value, len);
	}
	else if (!strcmp(name, CF_FIOCFP))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_FIOCompareFilePath = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_FIOCompareFilePath, value, len);
	}
	else if (!strcmp(name, CF_FIORFP))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_FIOResponseFilePath = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_FIOResponseFilePath, value, len);
	}
	else if (!strcmp(name, CF_FIODFP))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_FIODumpFilePath = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_FIODumpFilePath, value, len);
	}
	else if (!strcmp(name, CF_FIOLBDEV))
	{
		int		len = strlen(value) + 1;
		if (NULL == (s_FIOLoopbackDevice = malloc(sizeof(char) * len)))
		{
			// malloc failed
			fprintf(stderr,
					"Failed to malloc space [%d] for [%s %c %s], configuration file [%s], line [%d]\n",
					len,
					name,
					CF_SEPARATOR_CHAR,
					value,
					fileName,
					ln);
			return(STATUS_FAIL);
		}
		strncpy(s_FIOLoopbackDevice, value, len);
	}
	else
	{
		// Unrecognized name
		fprintf(stderr,
				"Unrecognized name [%s] in configuration file [%s], line [%d]\n",
				name,
				fileName,
				ln);
		return(STATUS_FAIL);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This is the main connfiguration file processing function
 *        This function opens and processes all configuration items
 *        define in configFile.h.
 *        Configuration File items are of the syntax:
 *        		name = value
 *        Blank space (spaces and tabs) is allowed after the name
 *        and before the value
 *        Blank lines are also allowed
 *
 * \param[in]	fileName - Path to configuration file to process
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int16_t
configFileProcess(char *fileName)
{
	FILE	*file;
	int		ln = 0;	// Configuration File line number
	char	configLine[LINE_MAX];
	char	name[LINE_MAX];
	char	value[LINE_MAX];
	boolean	process;

	if (fileName == NULL)
	{
		return(STATUS_PASS);
	}

	// Open the configuration file
	if (NULL == (file = fopen(fileName, "r")))
	{
		// Output an error message
		fprintf(stderr,
				"Failed to open configuration file [%s], errno[%s]\n",
				fileName,
				strerror(errno));
		return(STATUS_FAIL);
	}

	// Process each line
	while (!feof(file))
	{
		fgets(configLine, LINE_MAX, file);
		ln++;

		// Parse the configuration line
		if (STATUS_FAIL == configFileParse(fileName,
										   ln,
										   configLine,
										   &process,
										   name,
										   value))
		{
			fclose(file);
			return(STATUS_FAIL);
		}
		
		// See if the line should be processed
		if (FALSE == process)
		{
			continue;
		}

		// Now handle the configuration parsed line
		if (STATUS_FAIL == configFileInterpret(fileName, ln, name, value))
		{
			fclose(file);
			return(STATUS_FAIL);
		}
	}

	fclose(file);
	return(STATUS_PASS);
}
