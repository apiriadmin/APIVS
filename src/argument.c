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
 * \file argument.c
 *
 * This file contains the definition of functions for processing all
 * arguments
 * 		$variable
 * 		#named-constant
 * 		%numeric-constant
 * 		@string-constant
 * 		!function/macro
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief argument processing
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
#include	<ctype.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<asm/fcntl.h>
#include	<errno.h>		// STD errno Definitions
#include	"vse_stnd.h"	// STD VSE Definitions
#include	"configFile.h"	// Configuration File Definitions
#include	"outputXML.h"	// Output XML Processor Definitions
#include	"argument.h"	// Argument Processor Definitions
#include	"vt100.h"		// VT100 Definitions
#include	"emfio.h"		// FIO Emulator Definitions

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Definitions
 */

/** \brief Static table of pointers to argument tables */
static	ARG_CONTAINER	s_argContainer = { NULL, NULL, NULL, NULL, NULL };

/** \brief Static table mapping variable type to string */
static char		*s_varType_string[] =
{
	V_VOID,
	V_CHAR,
	V_PCHAR,
	V_UCHAR,
	V_PUCHAR,
	V_BOOL,
	V_SSIZET,
	V_INT,
	V_UINT,
	V_FPUIH,
	V_FPUIAUXH,
	V_OFLAGS,
	V_FAPPH,
	V_FDEVH,
	V_FVER,
	V_FVIEW,
	V_FCMAP,
	V_FCMASK,
	V_FCFSA,
	V_FNOTF,
	V_FFSCHD,
	V_FINF,
	V_FINT,
	V_FTST,
	V_FTBUF,
	V_FMFB,
	V_FPORT,
	V_FDEVT,
	V_FFDSTAT,
	V_FFINFO,
	V_FFMST,
	V_FVMST,
	V_FNOTI,
	V_TVAL,
	V_DSTIT,
};

/** \brief Variable type equivalence table */
// Used to solve issues with TOD interface passing INT for several
// parameters and we want to be able to specify parameters
// like #TRUE, for instance
// This table is bi-directional: IE.
// A VAR_BOOL can be used for a VAR_INT and a VAR_INT can be used in
// place on a VAR_BOOL
static	VAR_TYPE	s_varType[] =
{
	VAR_VOID,
	VAR_CHAR,
	VAR_PCHAR,
	VAR_UCHAR,
	VAR_PUCHAR,
	VAR_INT,		// VAR_BOOL
	VAR_INT,		// VAR_SSIZET
	VAR_INT,		// VAR_INT
	VAR_INT,		// VAR_UINT
	VAR_INT,		// VAR_FPUIH
	VAR_FPUIAUXH,		// VAR_FPUIAUXH
	VAR_INT,		// VAR_OFLAGS
	VAR_INT,		// VAR_FAPPH
	VAR_INT,		// VAR_FDEVH
	VAR_INT,		// VAR_FVER
	VAR_INT,		// VAR_FVIEW
	VAR_FCMAP,
	VAR_INT,		// VAR_FCMASK
	VAR_INT,		// VAR_FCFSA
	VAR_INT,		// VAR_FNOTF
	VAR_FFSCHD,
	VAR_FINF,
	VAR_INT,		// VAR_FINT
	VAR_INT,		// VAR_FTST
	VAR_FTBUF,
	VAR_INT,		// VAR_FMSB
	VAR_INT,		// VAR_FPORT
	VAR_INT,		// VAR_FDEVT
	VAR_FFDSTAT,
	VAR_FFINFO,		// VAR_FFINFO
	VAR_INT,		// VAR_FFMST
	VAR_INT,		// VAR_FVMST
	VAR_FNOTI,
	VAR_TVAL,
	VAR_DSTIT,
};

/** \brief Static table mappings for argument type to string */
static char		*s_argType_string[] =
{
	VA_UNKNOWN,
	VA_VAR,
	VA_NAMED_CONST,
	VA_NUMERIC_CONST,
	VA_STRING_CONST,
	VA_MACRO,
	VA_TMP
};

//#ifndef	O_DIRECT
//#define	O_DIRECT	0x00	// Define if not defined in include files
//#endif

static	ARG_NC	s_NCtable[] =
{
	{ NC_NULL,				(int)NC_NULL_VALUE,			VAR_PCHAR },
	{ NC_TRUE,				NC_TRUE_VALUE,				VAR_BOOL },
	{ NC_FALSE,				NC_FALSE_VALUE,				VAR_BOOL },
	{ NC_ON,				NC_ON_VALUE,				VAR_BOOL },
	{ NC_OFF,				NC_OFF_VALUE,				VAR_BOOL },

	{ NC_ORDWR,				O_RDWR,						VAR_INT },
	{ NC_ORDONLY,			O_RDONLY,					VAR_INT },
	{ NC_OWRONLY,			O_WRONLY,					VAR_INT },
	{ NC_ODIRECT,			O_DIRECT,					VAR_INT },
	{ NC_ONONBLOCK,			O_NONBLOCK,					VAR_INT },

	{ NC_FIO_VERLIB,		FIO_VERSION_LIBRARY,		VAR_INT },
	{ NC_FIO_VERLKM,		FIO_VERSION_LKM,			VAR_INT },
	{ NC_FIO_VIEWAPP,		FIO_VIEW_APP,				VAR_INT },
	{ NC_FIO_VIEWSYS,		FIO_VIEW_SYSTEM,			VAR_INT },
	{ NC_FIO_CDCM1,			FIO_CMU_DC_MASK1,			VAR_INT },
	{ NC_FIO_CDCM2,			FIO_CMU_DC_MASK2,			VAR_INT },
	{ NC_FIO_CDCM3,			FIO_CMU_DC_MASK3,			VAR_INT },
	{ NC_FIO_CDCM4,			FIO_CMU_DC_MASK4,			VAR_INT },
	{ NC_FIO_CFSANONE,		FIO_CMU_FSA_NONE,			VAR_INT },
	{ NC_FIO_CFSANL,		FIO_CMU_FSA_NON_LATCHING,	VAR_INT },
	{ NC_FIO_CFSAL,			FIO_CMU_FSA_LATCHING,		VAR_INT },
	{ NC_FIO_NOT1,			FIO_NOTIFY_ONCE,			VAR_INT },
	{ NC_FIO_NOTA,			FIO_NOTIFY_ALWAYS,			VAR_INT },
	{ NC_FIO_ITRAW,			FIO_INPUTS_RAW,				VAR_INT },
	{ NC_FIO_ITFLT,			FIO_INPUTS_FILTERED,		VAR_INT },
	{ NC_FIO_TSS,			FIO_TRANS_SUCCESS,			VAR_INT },
	{ NC_FIO_TSFO,			FIO_TRANS_FIOD_OVERRUN,		VAR_INT },
	{ NC_FIO_TSAO,			FIO_TRANS_APP_OVERRUN,		VAR_INT },
	{ NC_FIO_MFBOFF,		FIO_MMU_FLASH_BIT_OFF,		VAR_INT },
	{ NC_FIO_MFBON,			FIO_MMU_FLASH_BIT_ON,		VAR_INT },
	{ NC_FIO_TSFMOFF,		FIO_TS_FM_OFF,				VAR_INT },
	{ NC_FIO_TSFMON,		FIO_TS_FM_ON,				VAR_INT },
	{ NC_FIO_TS1VMOFF,		FIO_TS1_VM_OFF,				VAR_INT },
	{ NC_FIO_TS1VMON,		FIO_TS1_VM_ON,				VAR_INT },
	{ NC_FIO_IPB,			FIO_INPUT_POINTS_BYTES,		VAR_INT },
	{ NC_FIO_OPB,			FIO_OUTPUT_POINTS_BYTES,	VAR_INT },
	{ NC_FIO_CPB,			FIO_CHANNEL_BYTES,			VAR_INT },
	{ NC_FIOSP3,			FIO_PORT_SP3,				VAR_INT },
	{ NC_FIOSP5,			FIO_PORT_SP5,				VAR_INT },
	{ NC_FIOSP8,			FIO_PORT_SP8,				VAR_INT },
	{ NC_FIO332,			FIO332,						VAR_INT },
	{ NC_FIOTS1,			FIOTS1,						VAR_INT },
	{ NC_FIOTS2,			FIOTS2,						VAR_INT },
	{ NC_FIOMMU,			FIOMMU,						VAR_INT },
	{ NC_FIODR1,			FIODR1,						VAR_INT },
	{ NC_FIODR2,			FIODR2,						VAR_INT },
	{ NC_FIODR3,			FIODR3,						VAR_INT },
	{ NC_FIODR4,			FIODR4,						VAR_INT },
	{ NC_FIODR5,			FIODR5,						VAR_INT },
	{ NC_FIODR6,			FIODR6,						VAR_INT },
	{ NC_FIODR7,			FIODR7,						VAR_INT },
	{ NC_FIODR8,			FIODR8,						VAR_INT },
	{ NC_FIOTF1,			FIOTF1,						VAR_INT },
	{ NC_FIOTF2,			FIOTF2,						VAR_INT },
	{ NC_FIOTF3,			FIOTF3,						VAR_INT },
	{ NC_FIOTF4,			FIOTF4,						VAR_INT },
	{ NC_FIOTF5,			FIOTF5,						VAR_INT },
	{ NC_FIOTF6,			FIOTF6,						VAR_INT },
	{ NC_FIOTF7,			FIOTF7,						VAR_INT },
	{ NC_FIOTF8,			FIOTF8,						VAR_INT },
	{ NC_FIOCMU,			FIOCMU,						VAR_INT },
	{ NC_FIOINSIU1,			FIOINSIU1,					VAR_INT },
	{ NC_FIOINSIU2,			FIOINSIU2,					VAR_INT },
	{ NC_FIOINSIU3,			FIOINSIU3,					VAR_INT },
	{ NC_FIOINSIU4,			FIOINSIU4,					VAR_INT },
	{ NC_FIOINSIU5,			FIOINSIU5,					VAR_INT },
	{ NC_FIOOUT6SIU1,		FIOOUT6SIU1,				VAR_INT },
	{ NC_FIOOUT6SIU2,		FIOOUT6SIU2,				VAR_INT },
	{ NC_FIOOUT6SIU3,		FIOOUT6SIU3,				VAR_INT },
	{ NC_FIOOUT6SIU4,		FIOOUT6SIU4,				VAR_INT },
	{ NC_FIOOUT14SIU1,		FIOOUT14SIU1,				VAR_INT },
	{ NC_FIOOUT14SIU2,		FIOOUT14SIU2,				VAR_INT },
	{ NC_FIO_HZ_0,			FIO_HZ_0,				VAR_INT },
	{ NC_FIO_HZ_ONCE,		FIO_HZ_ONCE,				VAR_INT },
	{ NC_FIO_HZ_1,			FIO_HZ_1,				VAR_INT },
	{ NC_FIO_HZ_5,			FIO_HZ_5,				VAR_INT },
	{ NC_FIO_HZ_10,			FIO_HZ_10,				VAR_INT },
	{ NC_FIO_HZ_20,			FIO_HZ_20,				VAR_INT },
	{ NC_FIO_HZ_30,			FIO_HZ_30,				VAR_INT },
	{ NC_FIO_HZ_40,			FIO_HZ_40,				VAR_INT },
	{ NC_FIO_HZ_50,			FIO_HZ_50,				VAR_INT },
	{ NC_FIO_HZ_60,			FIO_HZ_60,				VAR_INT },
	{ NC_FIO_HZ_70,			FIO_HZ_70,				VAR_INT },
	{ NC_FIO_HZ_80,			FIO_HZ_80,				VAR_INT },
	{ NC_FIO_HZ_90,			FIO_HZ_90,				VAR_INT },
	{ NC_FIO_HZ_100,		FIO_HZ_100,				VAR_INT },
	{ NC_FIO_HZ_MAX,		FIO_HZ_MAX,				VAR_INT },
	{ NC_FIO_GREEN,			FIO_GREEN,				VAR_INT },
	{ NC_FIO_YELLOW,		FIO_YELLOW,				VAR_INT },
	{ NC_FIO_RED,                   FIO_RED,				VAR_INT },

	{ NC_TOD_V_SRC_LINE,	TOD_TIMESRC_LINESYNC,		VAR_INT },
	{ NC_TOD_V_SRC_RTC,     TOD_TIMESRC_RTCSQWR,		VAR_INT },
	{ NC_TOD_V_SRC_CRYS,	TOD_TIMESRC_CRYSTAL,		VAR_INT },
	{ NC_TOD_V_SRC_EXT1,	TOD_TIMESRC_EXTERNAL1,		VAR_INT },
	{ NC_TOD_V_SRC_EXT2,	TOD_TIMESRC_EXTERNAL2,		VAR_INT },

	{ NC_SIG_FIO,           (32+4)/*FIO_SIGIO*/,            VAR_INT },
	{ NC_SIG_TOD,           SIGIO,                          VAR_INT },
	{ NC_SIG_WINCH,         SIGWINCH,                       VAR_INT },
	{ NC_SIG_USR1,          SIGUSR1,                        VAR_INT },
	{ NC_SIG_IO,            SIGIO,                          VAR_INT },

	// End of table, must be last entry
	{ NULL,                 0,                              0  }
};

static	ARG_MAC	s_MACtable[] =
{
	{ MAC_STRLEN,	MAC_TYPE_STRLEN, 	VAR_INT, 1, { VAR_PCHAR, VAR_VOID } },
	{ MAC_VDCOMP,	MAC_TYPE_VDCOMP, 	VAR_INT, 1, { VAR_PCHAR, VAR_VOID } },
	{ MAC_SIZEOF,	MAC_TYPE_SIZEOF, 	VAR_INT, 1, { VAR_VOID, VAR_VOID } },
	{ MAC_BIT_TEST,	MAC_TYPE_BIT_TEST, 	VAR_INT, 2, { VAR_PUCHAR, VAR_INT } },
	{ MAC_CMDMSG,	MAC_TYPE_CMDMSG, 	VAR_INT, 2, { VAR_PCHAR, VAR_INT } },

	// Must be last entry
	{ NULL,			0,					0,		 0, { VAR_VOID, VAR_VOID } },
};

//=============================================================================
/**
 * Local Defines
 */

//=============================================================================
/**
 * \brief This function initializes the argument processor
 *
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int16_t
argInit()
{
	ARG_NC	*pNC = &s_NCtable[0];
	ARG_MAC	*pMAC = &s_MACtable[0];

	// Initialize the named constants list
	// C does not let you initialize statically defined unions at compile time
	// So I have to do it in an initialization routine
	if (STATUS_FAIL == argDefineNC(NC_COLUMNS, configFileGetSW(), VAR_INT))
	{
		return(STATUS_FAIL);
	}

	if (STATUS_FAIL == argDefineNC(NC_ROWS, configFileGetSH(), VAR_INT))
	{
		return(STATUS_FAIL);
	}

	while (NULL != pNC->pName)
	{
		if (STATUS_FAIL == argDefineNC(pNC->pName,
									   pNC->value,
									   pNC->type))
		{
			return(STATUS_FAIL);
		}

		pNC++;
	}

	// Initialize the macro list
	// C does not let you initialize statically defined unions at compile time
	// So I have to do it in an initialization routine
	while (NULL != pMAC->pName)
	{
		if (STATUS_FAIL == argDefineMacro(pMAC))
		{
			return(STATUS_FAIL);
		}

		pMAC++;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function parses an argument type
 *
 * \param[in]	pValue - Start of code to interpret
 * \param[out]	pType - Where to store the variable type
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int16_t
argParseType(const char *pValue, VAR_TYPE *pType)
{
	int16_t	status = STATUS_PASS;

	if (!strcmp(pValue, V_CHAR))
	{
		*pType = VAR_CHAR;
	}
	else if (!strcmp(pValue, V_PCHAR))
	{
		*pType = VAR_PCHAR;
	}
	else if (!strcmp(pValue, V_UCHAR))
	{
		*pType = VAR_UCHAR;
	}
	else if (!strcmp(pValue, V_PUCHAR))
	{
		*pType = VAR_PUCHAR;
	}
	else if (!strcmp(pValue, V_BOOL))
	{
		*pType = VAR_BOOL;
	}
	else if (!strcmp(pValue, V_SSIZET))
	{
		*pType = VAR_SSIZET;
	}
	else if (!strcmp(pValue, V_INT))
	{
		*pType = VAR_INT;
	}
	else if (!strcmp(pValue, V_UINT))
	{
		*pType = VAR_UINT;
	}
	else if (!strcmp(pValue, V_FPUIH))
	{
		*pType = VAR_FPUIH;
	}
	else if (!strcmp(pValue, V_FPUIAUXH))
	{
		*pType = VAR_FPUIAUXH;
	}
	else if (!strcmp(pValue, V_OFLAGS))
	{
		*pType = VAR_OFLAGS;
	}
	else if (!strcmp(pValue, V_FAPPH))
	{
		*pType = VAR_FAPPH;
	}
	else if (!strcmp(pValue, V_FDEVH))
	{
		*pType = VAR_FDEVH;
	}
	else if (!strcmp(pValue, V_FVER))
	{
		*pType = VAR_FVER;
	}
	else if (!strcmp(pValue, V_FVIEW))
	{
		*pType = VAR_FVIEW;
	}
	else if (!strcmp(pValue, V_FCMAP))
	{
		*pType = VAR_FCMAP;
	}
	else if (!strcmp(pValue, V_FCMASK))
	{
		*pType = VAR_FCMASK;
	}
	else if (!strcmp(pValue, V_FCFSA))
	{
		*pType = VAR_FCFSA;
	}
	else if (!strcmp(pValue, V_FNOTF))
	{
		*pType = VAR_FNOTF;
	}
	else if (!strcmp(pValue, V_FFSCHD))
	{
		*pType = VAR_FFSCHD;
	}
	else if (!strcmp(pValue, V_FINF))
	{
		*pType = VAR_FINF;
	}
	else if (!strcmp(pValue, V_FINT))
	{
		*pType = VAR_FINT;
	}
	else if (!strcmp(pValue, V_FTST))
	{
		*pType = VAR_FTST;
	}
	else if (!strcmp(pValue, V_FTBUF))
	{
		*pType = VAR_FTBUF;
	}
	else if (!strcmp(pValue, V_FMFB))
	{
		*pType = VAR_FMFB;
	}
	else if (!strcmp(pValue, V_FPORT))
	{
		*pType = VAR_FPORT;
	}
	else if (!strcmp(pValue, V_FDEVT))
	{
		*pType = VAR_FDEVT;
	}
	else if (!strcmp(pValue, V_FFDSTAT))
	{
		*pType = VAR_FFDSTAT;
	}
	else if (!strcmp(pValue, V_FFINFO))
	{
		*pType = VAR_FFINFO;
	}
	else if (!strcmp(pValue, V_FFMST))
	{
		*pType = VAR_FFMST;
	}
	else if (!strcmp(pValue, V_FVMST))
	{
		*pType = VAR_FVMST;
	}
	else if (!strcmp(pValue, V_FNOTI))
	{
		*pType = VAR_FNOTI;
	}
	else if (!strcmp(pValue, V_TVAL))
	{
		*pType = VAR_TVAL;
	}
	else if (!strcmp(pValue, V_DSTIT))
	{
		*pType = VAR_DSTIT;
	}
	else
	{
		status = STATUS_FAIL;
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function returns the first NODE in the variable table
 *
 * \return		ARG_NODE pointer to first variable
 */
ARG_NODE	*
argVarTableGet()
{
	return(s_argContainer.pVar);
}

//=============================================================================
/**
 * \brief This function returns an variable type string
 *
 * \param[in]	pType - The type string to be returned
 * \return		Character string
 */
const char *
argVarStringGet(VAR_TYPE type)
{
	return(s_varType_string[type]);
}

//=============================================================================
/**
 * \brief This function returns an alternate VAR_TYPE for function arguments
 *
 * \param[in]	pType - The variable type to convert
 * \return		Character string
 */
VAR_TYPE
argAlternateVarType(VAR_TYPE type)
{
	return(s_varType[type]);
}

//=============================================================================
/**
 * \brief This function returns an argument type string
 *
 * \param[in]	pType - The type string to be returned
 * \return		Character string
 */
const char *
argArgStringGet(ARG_TYPE type)
{
	return(s_argType_string[type]);
}

//=============================================================================
/**
 * \brief This function is used to validate parameter syntax
 *
 * \param[in]	lineNumber - Line number in APIVSXML being processed
 * \param[in]	value - The value of the argument being parsed
 * \param[in] 	prefix - List of valid prefixes
 * \param[in,out]	arg - Pointer to argument reference to be resolved
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
static
int16_t
argValidateParameter(uint16_t 	lineNumber,
				     const char	*value,
				     const char	*prefix)
{
	boolean	prefixFound = FALSE;	// Was a prefix in the list matched?

	// First try to locate a prefix in the list
	while ('\0' != *prefix)
	{
		if (*prefix == *value)
		{
			prefixFound = TRUE;
			break;
		}

		prefix++;
	}

	// Make sure correct variable definition
	if (!prefixFound)
	{
		// Prefix is not present
		char	string[OUTPUT_STRING_MAX];

		sprintf(string,
				"argParseParameter(): Invalid argument, syntax not recognized [%s]",
				value);
		OUTPUT_ERR(lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used to validate a variable parameter
 *
 * \param[in]	lineNumber - Line number in APIVSXML being processed
 * \param[in]	value - The value of the argument being parsed
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argValidateVarSyntax(uint16_t 		lineNumber,
				  	 const char 	*value)
{
	const char varPrefix[] = { ARG_VAR_PREFIX,
							   '\0'
			  				 };

	return(argValidateParameter(lineNumber, value, varPrefix));
}

//=============================================================================
/**
 * \brief This function is used to validate an argument parameter
 *
 * \param[in]	lineNumber - Line number in APIVSXML being processed
 * \param[in]	value - The value of the argument being parsed
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argValidateArgSyntax(uint16_t 		lineNumber,
				  	 const char 	*value)
{
	const char argPrefix[] = { ARG_VAR_PREFIX,
					ARG_NAMED_CONST_PREFIX,
					ARG_NUMERIC_CONST_PREFIX,
					ARG_STRING_CONST_PREFIX,
					ARG_MACRO_PREFIX,
				  	'\0'
	};

	return(argValidateParameter(lineNumber, value, argPrefix));
}

//=============================================================================
/**
 * \brief This function actually defines a variable
 *
 * \param[in]	lineNumber - Line number in APIVSXML being processed
 * \param[in]	pName - The variable name
 * \param[in]	varType - Variable type
 * \param[in]	size - Array size, if needed
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argDefineVar(uint16_t lineNumber, const char *pName, VAR_TYPE varType,
		uint16_t size)
{
	char		*ptr;
	ARG_NODE	*pNode = s_argContainer.pVar;
	ARG_NODE	**ppNode = &s_argContainer.pVar;

	// See if this variable is already defined
	while (NULL != pNode)
	{
		if (!strcmp(pName, pNode->argCode.pName))
		{
			// We found an instance of the variable, this is an error
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDefineVar(): Variable [%s] already defined",
					pName);
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		ppNode = &pNode->pNode;
		pNode = pNode->pNode;
	}

	// Variable not already defined
	// Malloc space for variable
	if (NULL == (pNode = malloc(sizeof(ARG_NODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDefineVar(): Failed to malloc space for variable [%s] size [%d]",
				pName,
				sizeof(ARG_NODE));
		OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	// Malloc space for variable name
	if (NULL == (ptr = malloc(strlen(pName) + 1)))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDefineVar(): Failed to malloc space for variable name [%s] size [%d]",
				pName,
				strlen(pName) + 1);
		OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	// Initialize variable
	memset(pNode, 0, sizeof(ARG_NODE));
	pNode->argCode.argType = ARG_VAR;
	strncpy(ptr, pName, strlen(pName) + 1);
	pNode->argCode.pName = ptr;
	pNode->argCode.varType = varType;
	pNode->argCode.arg.data.size = size;

	// Set up initial buffer if var is an array
	if (VAR_PCHAR == varType)
	{
		if (NULL == (ptr = malloc(size * sizeof(char))))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDefineVar(): Failed to malloc space for variable array [%s] size [%d]",
					pName,
					size * sizeof(char));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(ptr, 0, size * sizeof(char));
		pNode->argCode.arg.data.value.pCharValue = ptr;
	}
	else if (VAR_PUCHAR == varType)
	{
		unsigned char	*pUC;
		if (NULL == (pUC = malloc(size * sizeof(unsigned char))))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDefineVar(): Failed to malloc space for variable array [%s] size [%d]",
					pName,
					size * sizeof(unsigned char));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(pUC, 0, size * sizeof(unsigned char));
		pNode->argCode.arg.data.value.pUcharValue = pUC;
	}
	else if (VAR_FTBUF == varType)
	{
		FIO_TRANS_BUFFER	*pUC;
		if (NULL == (pUC = malloc(size * sizeof(FIO_TRANS_BUFFER))))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDefineVar(): Failed to malloc space for variable array [%s] size [%d]",
					pName,
					size * sizeof(FIO_TRANS_BUFFER));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(pUC, 0, size * sizeof(FIO_TRANS_BUFFER));
		pNode->argCode.arg.data.value.fioTbuffer = pUC;
	}
	else if (VAR_FINF == varType)
	{
		FIO_INPUT_FILTER	*pUC;
		if (NULL == (pUC = malloc(size * sizeof(FIO_INPUT_FILTER))))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDefineVar(): Failed to malloc space for variable array [%s] size [%d]",
					pName,
					size * sizeof(FIO_INPUT_FILTER));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(pUC, 0, size * sizeof(FIO_INPUT_FILTER));
		pNode->argCode.arg.data.value.fioFinf = pUC;
	}
	else if (VAR_FFSCHD == varType)
	{
		FIO_FRAME_SCHD	*pUC;
		if (NULL == (pUC = malloc(size * sizeof(FIO_FRAME_SCHD))))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDefineVar(): Failed to malloc space for variable array [%s] size [%d]",
					pName,
					size * sizeof(FIO_FRAME_SCHD));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(pUC, 0, size * sizeof(FIO_FRAME_SCHD));
		pNode->argCode.arg.data.value.fioFschd = pUC;
	}
	else if (VAR_FFINFO == varType)
	{
		FIO_FRAME_INFO	*pUC;
		if (NULL == (pUC = malloc(size * sizeof(FIO_FRAME_INFO))))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDefineVar(): Failed to malloc space for variable array [%s] size [%d]",
					pName,
					size * sizeof(FIO_FRAME_INFO));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(pUC, 0, size * sizeof(FIO_FRAME_INFO));
		pNode->argCode.arg.data.value.fioFrameInfo = pUC;
	}
	else if (VAR_FCMAP == varType)
	{
		FIO_CHANNEL_MAP	*pUC;
		if (NULL == (pUC = malloc(size * sizeof(FIO_CHANNEL_MAP))))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDefineVar(): Failed to malloc space for variable array [%s] size [%d]",
					pName,
					size * sizeof(FIO_CHANNEL_MAP));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(pUC, 0, size * sizeof(FIO_CHANNEL_MAP));
		pNode->argCode.arg.data.value.fioFcmap = pUC;
	}

	// Link variable in
	*ppNode = pNode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function actually defines a named constant
 *
 * \param[in]	pName - The named constant name
 * \param[in]	value - Named constant value
 * \param[in]	type - The value type of named constant
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argDefineNC(const char 	*pName,
			int			value,
			VAR_TYPE	type)
{
	char		*ptr;
	ARG_NODE	*pNode = s_argContainer.pNC;
	ARG_NODE	**ppNode = &s_argContainer.pNC;

	// See if this named constant is already defined
	while (NULL != pNode)
	{
		if (!strcmp(pName, pNode->argCode.pName))
		{
			// We found an instance of the named constant, this is an error
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDefineNC(): Program Error!! Named Constant [%s] already defined",
					pName);
			OUTPUT_ERR(0, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		ppNode = &pNode->pNode;
		pNode = pNode->pNode;
	}

	// Named Constant not already defined
	// Malloc space for Named Constant
	if (NULL == (pNode = malloc(sizeof(ARG_NODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDefineNC(): Failed to malloc space for named constant [%s] size [%d]",
				pName,
				sizeof(ARG_NODE));
		OUTPUT_ERR(0, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	// Malloc space for Named Constant
	if (NULL == (ptr = malloc(strlen(pName) + 1)))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDefineNC(): Failed to malloc space for Named Constant [%s] size [%d]",
				pName,
				strlen(pName) + 1);
		OUTPUT_ERR(0, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	// Initialize Named Constant
	memset(pNode, 0, sizeof(ARG_NODE));
	pNode->argCode.argType = ARG_NAMED_CONST;
	strncpy(ptr, pName, strlen(pName) + 1);
	pNode->argCode.pName = ptr;
	pNode->argCode.varType = type;
	pNode->argCode.arg.data.value.intValue = value;
	pNode->argCode.arg.data.size = sizeof(int);

	// Link variable in
	*ppNode = pNode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function actually defines a macro
 *
 * \param[in]	pMAC - Point to Macro definitions element
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argDefineMacro(ARG_MAC		*pMAC)
{
	char		*ptr;
	ARG_NODE	*pNode = s_argContainer.pMacro;
	ARG_NODE	**ppNode = &s_argContainer.pMacro;

	// Macro not already defined
	// Get to end of list
	while (NULL != pNode)
	{
		ppNode = &pNode->pNode;
		pNode = pNode->pNode;
	}

	// Malloc space for Macro
	if (NULL == (pNode = malloc(sizeof(ARG_NODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDefineMacro(): Failed to malloc space for macro [%s] size [%d]",
				pMAC->pName,
				sizeof(ARG_NODE));
		OUTPUT_ERR(0, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	// Malloc space for macro
	if (NULL == (ptr = malloc(strlen(pMAC->pName) + 1)))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDefineMacro(): Failed to malloc space for Macro [%s] size [%d]",
				pMAC->pName,
				strlen(pMAC->pName) + 1);
		OUTPUT_ERR(0, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	// Initialize Macro
	memset(pNode, 0, sizeof(ARG_NODE));
	pNode->argCode.argType = ARG_MACRO;
	strncpy(ptr, pMAC->pName, strlen(pMAC->pName) + 1);
	pNode->argCode.pName = ptr;
	pNode->argCode.arg.macro.macType = pMAC->macroType;
	pNode->argCode.varType = pMAC->returnType;
	pNode->argCode.arg.macro.argCount = pMAC->argCount;
	pNode->argCode.arg.macro.varType[0] = pMAC->varType[0];
	pNode->argCode.arg.macro.varType[1] = pMAC->varType[1];

	// Link macro in
	*ppNode = pNode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function dumps the variable table
 *
 * \param[in]	lineNumber - XML line number of statement
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argDumpVar(uint16_t		lineNumber)
{
	ARG_NODE	*pNode = s_argContainer.pVar;	// Start at top

	// See if this variable is defined
	while (NULL != pNode)
	{
		OUTPUT_INFO(lineNumber, "VAR", pNode->argCode.pName);

		pNode = pNode->pNode;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function dereferences a variable
 *
 * \param[in]	lineNumber - XML line number of statement
 * \param[in]	pName - The variable name
 * \param[out]	pVar - Pointer where to place variable reference
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argDereferenceVar(uint16_t		lineNumber,
				  const char 	*pName,
			 	  ARG_P			**ppVar)
{
	ARG_NODE	*pNode = s_argContainer.pVar;	// Start at top

	// See if this variable is defined
	while (NULL != pNode)
	{
		if (!strcmp(pName, pNode->argCode.pName))
		{
			// We found an instance of the variable
			*ppVar = &pNode->argCode;

			// Make sure it is a VAR
			if (ARG_VAR != pNode->argCode.argType)
			{
				// Something is messed up
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argDereferenceVar(): Program error! Var [%s] is not ARG_VAR",
						pName);
				OUTPUT_ERR(lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
			return(STATUS_PASS);
		}

		pNode = pNode->pNode;
	}

	{
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDereferenceVar(): Variable [%s] not defined",
				pName);
		OUTPUT_ERR(lineNumber, string, NULL, NULL);
	}
	return(STATUS_FAIL);
}

//=============================================================================
/**
 * \brief This function dereferences a numeric constant
 *
 * \param[in]	lineNumber - XML line number of statement
 * \param[in]	pName - The variable name
 * \param[out]	ppNum - Pointer where to place reference
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argDereferenceNumeric(uint16_t		lineNumber,
					  const char 	*pName,
			 	  	  ARG_P			**ppNum)
{
	char		*ptr;
	ARG_NODE	*pNode = s_argContainer.pNum;	// Start at top
	ARG_NODE	**ppNode = &s_argContainer.pNum;

	// See if this numeric is defined
	while (NULL != pNode)
	{
		if (!strcmp(pName, pNode->argCode.pName))
		{
			// We found an instance of the variable
			*ppNum = &pNode->argCode;

			// Make sure it is a NUMERIC
			if (   (ARG_NUMERIC_CONST != pNode->argCode.argType)
				|| (VAR_INT != pNode->argCode.varType))
			{
				// Something is messed up
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argDereferenceNumeric(): Program error! Var [%s,%d,%d] is not NUMERIC",
						pName,
						pNode->argCode.argType,
						pNode->argCode.varType);
				OUTPUT_ERR(lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
			return(STATUS_PASS);
		}

		ppNode = &pNode->pNode;
		pNode = pNode->pNode;
	}

	// Numeric is not already defined
	// Define it!!!!
	// Malloc space for variable
	if (NULL == (pNode = malloc(sizeof(ARG_NODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDereferenceNumeric(): Failed to malloc space for numeric [%s] size [%d]",
				pName,
				sizeof(ARG_NODE));
		OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	// Malloc space for numeric name
	if (NULL == (ptr = malloc(strlen(pName) + 1)))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDereferenceNumeric(): Failed to malloc space for numeric name [%s] size [%d]",
				pName,
				strlen(pName) + 1);
		OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	// Initialize variable
	memset(pNode, 0, sizeof(ARG_NODE));
	pNode->argCode.argType = ARG_NUMERIC_CONST;
	strncpy(ptr, pName, strlen(pName) + 1);
	pNode->argCode.pName = ptr;
	pNode->argCode.varType = VAR_INT;
	pNode->argCode.arg.data.value.intValue = atoi(pName + 1);
	pNode->argCode.arg.data.size = sizeof(int);

	// Link numeric in to numeric list
	*ppNode = pNode;

	// Return the argument created
	*ppNum = &pNode->argCode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function dereferences a string constant
 *
 * \param[in]	lineNumber - XML line number of statement
 * \param[in]	pName - The variable name
 * \param[out]	ppStr - Pointer where to place reference
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argDereferenceString(uint16_t		lineNumber,
					 const char 	*pName,
			 	  	 ARG_P			**ppStr)
{
	char		*ptr;
	ARG_NODE	*pNode = s_argContainer.pStr;	// Start at top
	ARG_NODE	**ppNode = &s_argContainer.pStr;

	// See if this string is defined
	while (NULL != pNode)
	{
		if (!strcmp(pName, pNode->argCode.pName))
		{
			// We found an instance of the variable
			*ppStr = &pNode->argCode;

			// Make sure it is a STRING
			if (   (ARG_STRING_CONST != pNode->argCode.argType)
				|| (VAR_PCHAR != pNode->argCode.varType))
			{
				// Something is messed up
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argDereferenceString(): Program error! Var [%s,%d,%d] is not a STRING",
						pName,
						pNode->argCode.argType,
						pNode->argCode.varType);
				OUTPUT_ERR(lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
			return(STATUS_PASS);
		}

		ppNode = &pNode->pNode;
		pNode = pNode->pNode;
	}

	// String is not already defined
	// Define it!!!!
	// Malloc space for variable
	if (NULL == (pNode = malloc(sizeof(ARG_NODE))))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDereferenceString(): Failed to malloc space for string [%s] size [%d]",
				pName,
				sizeof(ARG_NODE));
		OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	// Malloc space for string name
	if (NULL == (ptr = malloc(strlen(pName) + 1)))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDereferenceString(): Failed to malloc space for string name [%s] size [%d]",
				pName,
				strlen(pName) + 1);
		OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	// Initialize variable
	memset(pNode, 0, sizeof(ARG_NODE));
	pNode->argCode.argType = ARG_STRING_CONST;
	strncpy(ptr, pName, strlen(pName) + 1);
	pNode->argCode.pName = ptr;
	pNode->argCode.varType = VAR_PCHAR;
	pNode->argCode.arg.data.size = strlen(pName) + 1;
	// It is ok to save the constant string in 2 locations
	// But get rid of the @
	pNode->argCode.arg.data.value.pCharValue = ptr + 1;

	// Link string in to string list
	*ppNode = pNode;

	// Return the argument created
	*ppStr = &pNode->argCode;

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function is used to process a macro
 *
 * \param[in]	ln - Line number in APIVSXML being processed
 * \param[in]	pArg - Pointer to macro info
 * \param[out] 	pInt - Where to return result
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
static
int16_t
argProcessMacro(uint16_t 	ln, ARG_P *pArg, int **ppInt)
{
	switch (pArg->arg.macro.macType)
	{
		case MAC_TYPE_VDCOMP:
		{
			int16_t	result;
			char	file[OUTPUT_STRING_MAX];

			sprintf(file,
					"%s%s",
					configFileGetFPUICFP(),
					pArg->arg.macro.arg[0]->arg.data.value.pCharValue);

			if ((result = vt100_compareVD(file)) < 0)
			{
				OUTPUT_ERR(ln,
						   "argProcessMacro(): Error processing macro",
						   vt100_get_errorText(),
						   pArg->pName);
				return(STATUS_FAIL);
			}

			**ppInt = (result ? FALSE : TRUE);
		}
		break;

		case MAC_TYPE_STRLEN:
		{
			**ppInt = strlen(pArg->arg.macro.arg[0]->arg.data.value.pCharValue);
		}
		break;

		case MAC_TYPE_SIZEOF:
		{
			**ppInt = pArg->arg.macro.arg[0]->arg.data.size;
		}
		break;

		case MAC_TYPE_BIT_TEST:
		{
			ARG_P			pAddr;
			unsigned char	*pUC;
			int				bit;

			if (NULL ==
				(pUC = malloc(sizeof(unsigned char) * pArg->arg.macro.arg[0]->arg.data.size)))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argProcessMacro(): Failed to malloc space for BIT_ARRAY [%d]",
						pArg->arg.macro.arg[0]->arg.data.size
							* sizeof(unsigned char));
				OUTPUT_ERR(ln, string, strerror(errno), NULL);
				return(STATUS_FAIL);
			}
			memset(pUC, 0, sizeof(unsigned char) * pArg->arg.macro.arg[0]->arg.data.size);

			pAddr.arg.data.value.pUcharValue = pUC;
			pAddr.arg.data.size = sizeof(unsigned char) *
									pArg->arg.macro.arg[0]->arg.data.size;
			if (STATUS_FAIL == argCastPuchar(ln, pArg->arg.macro.arg[0], &pAddr))
			{
				free(pUC);
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argCastInt(ln, pArg->arg.macro.arg[1], &bit))
			{
				free(pUC);
				return(STATUS_FAIL);
			}
			**ppInt = (FIO_BIT_TEST(pUC, bit) ? TRUE : FALSE);
			free(pUC);
		}
		break;

		case MAC_TYPE_CMDMSG:
		{
			// macro.arg[0] = Filename of file to convert and compare
			// macro.arg[1] = Command # that the EMFIO should compare against
			unsigned char	*pBuf;		// Point to command buffer
			unsigned int	size;		// Size of command buffer
			int				cmd_frame;	// Command frame number
			uint8_t			result;

			if (STATUS_FAIL ==
					argSet(ln,
						   configFileGetFIOCFP(),
						   pArg->arg.macro.arg[0]->arg.data.value.pCharValue,
						   &pBuf,
						   &size))
			{
				return(STATUS_FAIL);
			}

			if (STATUS_FAIL == argCastInt(ln,
										  pArg->arg.macro.arg[1],
										  &cmd_frame))
			{
				free(pBuf);
				return(STATUS_FAIL);
			}

			if (EMFIO_OK != emfio_compareCommand(cmd_frame,
												 pBuf,
												 (uint32_t)size,
												 &result))
			{
				// compare failed
				OUTPUT_ERR(ln,
						   "argProcessMacro(): Error processing macro",
						   emfio_getErrorText(),
						   pArg->pName);
				free(pBuf);
				return(STATUS_FAIL);
			}

			**ppInt = (int)result;
			free(pBuf);
		}
		break;

		default:
		{
			// Unknown macro
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argProcessMacro(): Unknown macro type [%s]",
					pArg->pName);
			OUTPUT_ERR(ln, string, NULL, NULL);
			return(STATUS_FAIL);
		}
		break;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function parses the macro name out of the input string
 * 		  This function assumes that pStr is pointing at the macro prefix
 * 		  Macros are of the form xyz(abc,def), where xyz is the macro name
 * 		  and abc is any argument 1, and def is argument 2 (if present)
 * 		  Spaces are NOT allowed between the ( and )
 *
 * \param[in]	ln - Line number in XML
 * \param[in]	pStr - Input string
 * \param[out]	pMacro - Point to where to place macro name
 * \param[out]	pArg1 - Pointer to where to place argument 1 string
 * \param[out]	pArg2 - Pointer to where to place argument 2 string
 * \return		Pointer to parsed out macro name
 */
static
int16_t
argParseMacro(uint16_t ln, const char *pStr, char **pMacro, char **pArg1, char **pArg2)
{
	static	char	macro[MAC_MAX+1];
	static	char	arg1[OUTPUT_STRING_MAX+1];
	static	char	arg2[OUTPUT_STRING_MAX+1];

	const char		*pStart = pStr;
	char			*pMac = macro;
	char			*pArg1s = arg1;
	char			*pArg2s = arg2;

	*pArg1 = NULL;
	*pArg2 = NULL;
	
	// Get past Macro prefix
	*pMac++ = *pStr++;

	while (   (*pStr >= 'A' && *pStr <= 'Z')
		   || (*pStr >= 'a' && *pStr <= 'z')
		   || (*pStr == MAC_UNDERSCORE))
	{
		*pMac++ = *pStr++;
		if (pMac >= (macro + MAC_MAX))
		{
			break;
		}
	}
	*pMac = '\0';
	*pMacro = macro;

	if (*pStr != MAC_ARG_START)
	{
		// Bad macro syntax
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argParseMacro(): Macro [%s] syntax error, no opening '%c'",
				macro,
				MAC_ARG_START);
		OUTPUT_ERR(ln, string, NULL, NULL);
		return(STATUS_FAIL);
	}
	pStr++;

	// Parse out the argument #1
	while (   (*pStr != MAC_ARG_END)
		   && (*pStr != MAC_ARG_SEPARATOR)
		   && (*pStr != '\0'))
	{
		*pArg1s++ = *pStr++;
		if (pArg1s >= (arg1 + OUTPUT_STRING_MAX))
		{
			break;
		}
	}
	*pArg1s = '\0';
	*pArg1 = arg1;

	if (*pStr == MAC_ARG_SEPARATOR)
	{
		pStr++;

		// Parse out the argument #1
		while (   (*pStr != MAC_ARG_END) && (*pStr != '\0'))
		{
			*pArg2s++ = *pStr++;
			if (pArg2s >= (arg2 + OUTPUT_STRING_MAX))
			{
				break;
			}
		}
		*pArg2s = '\0';
		*pArg2 = arg2;
	}

	if (*pStr++ != MAC_ARG_END)
	{
		// Bad macro syntax
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argParseMacro(): Macro [%s] syntax error, no closing '%c'",
				pStart,
				MAC_ARG_END);
		OUTPUT_ERR(ln, string, NULL, NULL);
		return(STATUS_FAIL);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function dereferences a macro
 *
 * \param[in]	lineNumber - XML line number of statement
 * \param[in]	pName - The variable name
 * \param[out]	ppMac - Pointer where to place reference
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argDereferenceMacro(uint16_t		lineNumber,
					const char 		*pName,
			 	  	ARG_P			**ppMac)
{
	// Macro's need to be defined and the the xyz of xyz(abc)
	// and the whole xyz(abc) or the arguments need to be kept and dereferenced
	ARG_NODE	*pNode = s_argContainer.pMacro;	// Start at top
	char		*pMacro;	// For parsing out macro name
	char		*pArg1;		// For parsing out the arguments
	char		*pArg2;		// For parsing out the arguments
	ARG_P		*pMac;		// For new macro instance
	VAR_TYPE	argAltVarType;
	VAR_TYPE	macAltVarType;

	// Parse out macro name
	if (STATUS_FAIL == argParseMacro(lineNumber, pName, &pMacro, &pArg1, &pArg2))
	{
		return(STATUS_FAIL);
	}

	// See if this string is defined
	while (NULL != pNode)
	{
		if (!strcmp(pMacro, pNode->argCode.pName))
		{
			// Make sure it is a MACRO
			if (ARG_MACRO != pNode->argCode.argType)
			{
				// Something is messed up
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argDereferenceMacro(): Program error! Var [%s,%d] is not a MACRO",
						pName,
						pNode->argCode.argType);
				OUTPUT_ERR(lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}

			// We found an instance of the macro
			// Now we have to make a copy so that we can modify it
			// Malloc space for macro
			if (NULL == (pMac = malloc(sizeof(ARG_P))))
			{
				// Malloc failed
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argDereferenceMacro(): Failed to malloc space for macro [%s] size [%d]",
						pName,
						sizeof(ARG_P));
				OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
				return(STATUS_FAIL);
			}

			// Make a copy
			*pMac = pNode->argCode;

			// Save the address
			*ppMac = pMac;

			break;
		}

		pNode = pNode->pNode;
	}

	// See if we found a node
	if (NULL != pNode)
	{	// Yes, valid macro found
		// Resolve argument #1
		if (STATUS_FAIL == argValidateArgSyntax(lineNumber, pArg1))
		{
			return(STATUS_FAIL);
		}

		if (STATUS_FAIL == argDereferenceArg(lineNumber,
										 	 pArg1,
										 	 &pMac->arg.macro.arg[0]))
		{
			return(STATUS_FAIL);
		}

		argAltVarType = argAlternateVarType(pMac->arg.macro.arg[0]->varType);
		macAltVarType = argAlternateVarType(pMac->arg.macro.varType[0]);

		if (   (pMac->arg.macro.varType[0] != VAR_VOID)
		    && (argAltVarType != macAltVarType))
		{
			// Incompatible argument to macro
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDereferenceMacro(): Macro argument [%s] type [%s] not compatible with macro [%s]",
					pArg1,
					argVarStringGet(pMac->arg.macro.arg[0]->varType),
					argVarStringGet(pMac->arg.macro.varType[0]));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		if (pMac->arg.macro.argCount == 1)
		{
			return(STATUS_PASS);
		}

		if ((pMac->arg.macro.argCount == 2) && (pArg2 == NULL))
		{
			// Argument missing
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDereferenceMacro(): Macro argument #2 type [%s] is missing",
					argVarStringGet(pMac->arg.macro.varType[1]));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Resolve argument #2, if present
		if (STATUS_FAIL == argValidateArgSyntax(lineNumber, pArg2))
		{
			return(STATUS_FAIL);
		}

		if (STATUS_FAIL == argDereferenceArg(lineNumber,
										 	 pArg2,
										 	 &pMac->arg.macro.arg[1]))
		{
			return(STATUS_FAIL);
		}

		argAltVarType = argAlternateVarType(pMac->arg.macro.arg[1]->varType);
		macAltVarType = argAlternateVarType(pMac->arg.macro.varType[1]);

		if (   (pMac->arg.macro.varType[1] != VAR_VOID)
		    && (argAltVarType != macAltVarType))
		{
			// Incompatible argument to macro
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argDereferenceMacro(): Macro argument [%s] type [%s] not compatible with macro [%s]",
					pArg2,
					argVarStringGet(pMac->arg.macro.arg[1]->varType),
					argVarStringGet(pMac->arg.macro.varType[1]));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		return(STATUS_PASS);
	}

	{
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDereferenceMacro(): Macro [%s] not defined",
				pMacro);
		OUTPUT_ERR(lineNumber, string, NULL, NULL);
	}
	return(STATUS_FAIL);
}

//=============================================================================
/**
 * \brief This function dereferences a named constant
 *
 * \param[in]	lineNumber - XML line number of statement
 * \param[in]	pName - The named constant name
 * \param[out]	ppNC - Pointer where to place named constant reference
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argDereferenceNC(uint16_t lineNumber, const char *pName,
			ARG_P **ppNC)
{
	ARG_NODE *pNode = s_argContainer.pNC;	// Start at top

	// See if this named constant is defined
	while (NULL != pNode)
	{
		if (!strcmp(pName, pNode->argCode.pName))
		{
			// We found an instance of the named constant
			*ppNC = &pNode->argCode;

			// Make sure it is a Named Constant
			if (ARG_NAMED_CONST != pNode->argCode.argType)
			{
				// Something is messed up
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argDereferenceNC(): Program error! Named Constant [%s] is not ARG_NAMED_CONST",
						pName);
				OUTPUT_ERR(lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
			return(STATUS_PASS);
		}

		pNode = pNode->pNode;
	}

	{
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDereferenceNC(): Named Constant [%s] not defined",
				pName);
		OUTPUT_ERR(lineNumber, string, NULL, NULL);
	}
	return(STATUS_FAIL);
}

//=============================================================================
/**
 * \brief This function actually defines a variable
 *
 * \param[in]	lineNumber - Line number in APIVSXML being processed
 * \param[in]	pName - The variable name
 * \param[out]	ppArg - Pointer to where to place argument reference
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argDereferenceArg(uint16_t 		lineNumber,
			 	  const char 	*pName,
				  ARG_P			**ppArg)
{
	switch(*pName)
	{
		case ARG_VAR_PREFIX:
			{
				if (STATUS_FAIL == argDereferenceVar(lineNumber, pName, ppArg))
				{
					return(STATUS_FAIL);
				}
				return(STATUS_PASS);
			}
			break;

		case ARG_NAMED_CONST_PREFIX:
			{
				if (STATUS_FAIL == argDereferenceNC(lineNumber, pName, ppArg))
				{
					return(STATUS_FAIL);
				}
				return(STATUS_PASS);
			}
			break;

		case ARG_NUMERIC_CONST_PREFIX:
			{
				if (STATUS_FAIL == argDereferenceNumeric(lineNumber,
														 pName,
														 ppArg))
				{
					return(STATUS_FAIL);
				}
				return(STATUS_PASS);
			}
			break;

		case ARG_STRING_CONST_PREFIX:
			{
				if (STATUS_FAIL == argDereferenceString(lineNumber,
														pName,
														ppArg))
				{
					return(STATUS_FAIL);
				}
				return(STATUS_PASS);
			}
			break;

		case ARG_MACRO_PREFIX:
			{
				if (STATUS_FAIL == argDereferenceMacro(lineNumber,
													   pName,
													   ppArg))
				{
					return(STATUS_FAIL);
				}
				return(STATUS_PASS);
			}
			break;

		default:
			break;
	}

	{
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argDereferenceArg(): Not supported [%s], unknown prefix",
				pName);
		OUTPUT_ERR(lineNumber, string, NULL, NULL);
	}
	return(STATUS_FAIL);
}

//=============================================================================
/**
 * \brief This function formats a variable for output
 *
 * \param[in]	ln - Line number in APIVSXML being processed
 * \param[in]	level - Output level
 * \param[in]	pArg - Pointer to Argument to output
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argFormat(uint16_t ln, const RUN_LEVEL level, ARG_P *pArg)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch(pArg->varType)
	{
		case VAR_CHAR:
		{
			char ch = pArg->arg.data.value.charValue;
			if (isprint(ch))
				sprintf(string, "%c", pArg->arg.data.value.charValue);
			else
				sprintf(string, "[0x%02x]", pArg->arg.data.value.charValue);
			outputXmlText(level, string);
		}
		break;

		case VAR_PCHAR:
		{
			char buf[OUTPUT_STRING_MAX];
			int i=0;
			strncpy(buf, pArg->arg.data.value.pCharValue, pArg->arg.data.size);
			buf[pArg->arg.data.size-1] = '\0';
			for(i=0;i<pArg->arg.data.size;i++) {
				if (!isprint(buf[i]))
					break;
			}
			if (buf[i] != '\0') {
				outputXmlHex(level, pArg->pName,
					pArg->arg.data.value.pCharValue,
					pArg->arg.data.size);
			} else {
				sprintf(string, "%s", buf);
				outputXmlText(level, string);
			}
		}
		break;

		case VAR_UCHAR:
		{
			sprintf(string, "[0x%02x]", pArg->arg.data.value.uCharValue);
			outputXmlText(level, string);
		}
		break;

		case VAR_PUCHAR:
		{
			outputXmlHex(level, pArg->pName,
					pArg->arg.data.value.pUcharValue,
					pArg->arg.data.size);
		}
		break;

		case VAR_UINT:
		{
			unsigned int		value;
			(void)argCastUint(ln, pArg, &value);
			sprintf(string, "[%u]", value);
			outputXmlText(level, string);
		}
		break;

		case VAR_BOOL:
		case VAR_INT:
		case VAR_FPUIH:
		case VAR_SSIZET:
		case VAR_OFLAGS:
		case VAR_FAPPH:
		case VAR_FDEVH:
		case VAR_FVER:
		case VAR_FVIEW:
		case VAR_FCMASK:
		case VAR_FCFSA:
		case VAR_FNOTF:
		case VAR_FINT:
		case VAR_FTST:
		case VAR_FMFB:
		case VAR_FPORT:
		case VAR_FDEVT:
		case VAR_FFMST:
		case VAR_FVMST:
		{
			int		value;
			(void)argCastInt(ln, pArg, &value);
			sprintf(string, "[%d]", value);
			outputXmlText(level, string);
		}
		break;

		case VAR_FFDSTAT:
		{
			outputXmlHex(level, pArg->pName,
					&pArg->arg.data.value.fioFiodStatus,
					sizeof(FIO_FIOD_STATUS));
		}
		break;

		case VAR_FCMAP:
		{
			if (pArg->arg.data.size > 1) {
				outputXmlHex(level, pArg->pName,
					pArg->arg.data.value.fioFcmap,
					pArg->arg.data.size * sizeof(FIO_CHANNEL_MAP));
			} else {
				sprintf(string, "%s%c%s = [%d], %s%c%s = [%d], %s%c%s = [%d], %s%c%s = [%d]",
					pArg->pName, ARG_MEMBER, MEM_FCMAP_OUT,
					pArg->arg.data.value.fioFcmap->output_point,
					pArg->pName, ARG_MEMBER, MEM_FCMAP_FIOD,
					pArg->arg.data.value.fioFcmap->fiod,
					pArg->pName, ARG_MEMBER, MEM_FCMAP_CHAN,
					pArg->arg.data.value.fioFcmap->channel,
					pArg->pName, ARG_MEMBER, MEM_FCMAP_CLR,
					pArg->arg.data.value.fioFcmap->color);
				outputXmlText(level, string);
			}
		}
		break;

		case VAR_FFSCHD:
		{
			if (pArg->arg.data.size > 1) {
				sprintf(string, "%s%c%s = [%d], %s%c%s = [%d]",
					pArg->pName, ARG_MEMBER, MEM_FFSCHD_REQ,
					pArg->arg.data.value.fioFschd->req_frame,
					pArg->pName, ARG_MEMBER, MEM_FFSCHD_HZ,
					pArg->arg.data.value.fioFschd->frequency);
				outputXmlText(level, string);
			} else {
				outputXmlHex(level, pArg->pName,
					pArg->arg.data.value.fioFschd,
					pArg->arg.data.size * sizeof(FIO_FRAME_SCHD));
			}
		}
		break;

		case VAR_FINF:
		{
			if (pArg->arg.data.size > 1) {
				outputXmlHex(level, pArg->pName,
					pArg->arg.data.value.fioFinf,
					pArg->arg.data.size * sizeof(FIO_INPUT_FILTER));
			} else {
				sprintf(string, "%s%c%s = [%d], %s%c%s = [%d], %s%c%s = [%d]",
					pArg->pName, ARG_MEMBER, MEM_FINF_IP,
					pArg->arg.data.value.fioFinf->input_point,
					pArg->pName, ARG_MEMBER, MEM_FINF_LEAD,
					pArg->arg.data.value.fioFinf->leading,
					pArg->pName, ARG_MEMBER, MEM_FINF_TRAIL,
					pArg->arg.data.value.fioFinf->trailing);
				outputXmlText(level, string);
			}
		}
		break;

		case VAR_FFINFO:
		{
			if (pArg->arg.data.size > 1) {
				outputXmlHex(level, pArg->pName,
					pArg->arg.data.value.fioFinf,
					pArg->arg.data.size * sizeof(FIO_FRAME_INFO));
			} else {
				sprintf(string, "%s%c%s = [%d], %s%c%s = [%d], %s%c%s = [%d], %s%c%s = [%d], %s%c%s = [%d]",
					pArg->pName, ARG_MEMBER, MEM_FFINFO_HZ,
					pArg->arg.data.value.fioFrameInfo->frequency,
					pArg->pName, ARG_MEMBER, MEM_FFINFO_SRX,
					pArg->arg.data.value.fioFrameInfo->success_rx,
					pArg->pName, ARG_MEMBER, MEM_FFINFO_ERX,
					pArg->arg.data.value.fioFrameInfo->error_rx,
					pArg->pName, ARG_MEMBER, MEM_FFINFO_E10,
					pArg->arg.data.value.fioFrameInfo->error_last_10,
					pArg->pName, ARG_MEMBER, MEM_FFINFO_SEQ,
					pArg->arg.data.value.fioFrameInfo->last_seq);
				outputXmlText(level, string);
			}
		}
		break;

		case VAR_FTBUF:
		{
			outputXmlHex(level, pArg->pName,
					pArg->arg.data.value.fioTbuffer,
					pArg->arg.data.size * sizeof(FIO_TRANS_BUFFER));
		}
		break;

		case VAR_FNOTI:
		{
			sprintf(string, "%s%c%s = [%d], %s%c%s = [%d], %s%c%s = [%d], %s%c%s = [%d], %s%c%s = [%d]",
				pArg->pName, ARG_MEMBER, MEM_FNOTI_RXF,
				pArg->arg.data.value.fioNotifyInfo.rx_frame,
				pArg->pName, ARG_MEMBER, MEM_FNOTI_STAT,
				pArg->arg.data.value.fioNotifyInfo.status,
				pArg->pName, ARG_MEMBER, MEM_FNOTI_SEQ,
				pArg->arg.data.value.fioNotifyInfo.seq_number,
				pArg->pName, ARG_MEMBER, MEM_FNOTI_CNT,
				pArg->arg.data.value.fioNotifyInfo.count,
				pArg->pName, ARG_MEMBER, MEM_FNOTI_FIOD,
				pArg->arg.data.value.fioNotifyInfo.fiod);
			outputXmlText(level, string);
		}
		break;

		case VAR_TVAL:
		{
			sprintf(string,
					"%s%c%s = [%ld], %s%c%s = [%ld]",
					pArg->pName, ARG_MEMBER, MEM_TV_SEC,
					pArg->arg.data.value.timevalValue.tv_sec,
					pArg->pName, ARG_MEMBER, MEM_TV_USEC,
					pArg->arg.data.value.timevalValue.tv_usec);
			outputXmlText(level, string);
		}
		break;

		case VAR_DSTIT:
		{
			outputXmlHex(level, pArg->pName,
					&pArg->arg.data.value.dstinfoValue,
					sizeof(dst_info_t));
		}
		break;

		case VAR_VOID:
		default:
		{
			status = STATUS_FAIL;
			sprintf(string,
					"argFormat(): Invalid variable type [%s] for [%s]",
					argVarStringGet(pArg->varType),
					pArg->pName);
			OUTPUT_ERR(ln, string, NULL, NULL);
		}
		break;
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function Sets a variable
 * 		  By the time we get here, the value type is correct of the
 * 		  variable that is being set
 *
 * \param[in]	ln - Line number in APIVSXML file
 * \param[in]	operation - equal, add, subtract
 * \param[in]	pValue - Pointer to initialize from a value
 * \param[in]	pFile - Pointer to initialize from a file
 * \param[in]	pVar - Pointer of Variable to be initialized
 * \param[in]	pIndex - array index if applicable
 * \param[in]	pMember - Member to initialize if structure
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argSetVar(uint16_t ln,
		  OP_TYPE operation,
		  ARG_P *pValue,
		  ARG_P *pFile,
		  ARG_P *pVar,
		  ARG_P *pIndex,
		  const char *pMember)
{
	boolean	memberAllowed = FALSE;		// If a Member name is allowed
	int16_t	status = STATUS_FAIL;

	switch(pVar->varType)
	{
		case VAR_VOID:
			{
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argSetVar(): Invalid variable type for <%s/>",
						P_SET);
				OUTPUT_ERR(ln, string, NULL, NULL);
			}
			break;

		case VAR_CHAR:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastChar(ln, pValue, &pVar->arg.data.value.charValue);
				}
			}
			break;

		case VAR_PCHAR:
			{
				if (pFile != NULL)
				{
					unsigned char *pBuf;
					unsigned int size;
					if (STATUS_FAIL == (status = argSet(ln,
								configFileGetSFP(),
								pFile->arg.data.value.pCharValue,
								&pBuf,
								&size)))
					{
						break;
					}
					memset(pVar->arg.data.value.pCharValue, 0, pVar->arg.data.size);
					memcpy(pVar->arg.data.value.pCharValue, pBuf,
						MIN(pVar->arg.data.size, size));
					free(pBuf);
					break;
				}

				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a file or value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastPchar(ln, pValue, pVar);
				}
			}
			break;

		case VAR_UCHAR:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastUchar(ln,
										  pValue,
										  &pVar->arg.data.value.uCharValue);
				}
			}
			break;

		case VAR_PUCHAR:
			{
				unsigned	char	*pBuf;
				unsigned	int		size;

				if (pFile == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a file",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					if (STATUS_FAIL ==
							(status = argSet(ln,
											 configFileGetSFP(),
											 pFile->arg.data.value.pCharValue,
										 	 &pBuf,
											 &size)))
					{
						break;
					}
					memset(pVar->arg.data.value.pUcharValue,
						   0,
						   pVar->arg.data.size);
					memcpy(pVar->arg.data.value.pUcharValue,
						   pBuf,
						   MIN(pVar->arg.data.size, size));
					free(pBuf);
				}
			}
			break;

		case VAR_BOOL:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastInt(ln, pValue, &pVar->arg.data.value.intValue);
				}
			}
			break;

		case VAR_SSIZET:
			{
				ssize_t		value;

				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (STATUS_PASS == (status = argCastSsizet(ln, pValue, &value)))
				{
					switch (operation)
					{
						case OP_ADD:
						{
							pVar->arg.data.value.ssizetValue += value;
						}
						break;

						case OP_SUB:
						{
							pVar->arg.data.value.ssizetValue -= value;
						}
						break;

						default:
						{
							pVar->arg.data.value.ssizetValue = value;
						}
						break;
					}
				}
			}
			break;

		case VAR_INT:
			{
				int		value;

				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (STATUS_PASS == (status = argCastInt(ln, pValue, &value)))
				{
					switch (operation)
					{
						case OP_ADD:
						{
							pVar->arg.data.value.intValue += value;
						}
						break;

						case OP_SUB:
						{
							pVar->arg.data.value.intValue -= value;
						}
						break;

						default:
						{
							pVar->arg.data.value.intValue = value;
						}
						break;
					}
				}
			}
			break;

		case VAR_UINT:
			{
				unsigned int		value;

				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (STATUS_PASS == (status = argCastUint(ln, pValue, &value)))
				{
					switch (operation)
					{
						case OP_ADD:
						{
							pVar->arg.data.value.uintValue += value;
						}
						break;

						case OP_SUB:
						{
							pVar->arg.data.value.uintValue -= value;
						}
						break;

						default:
						{
							pVar->arg.data.value.uintValue = value;
						}
						break;
					}
				}
			}
			break;

		case VAR_FPUIH:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFpuiHandle(ln,
											   pValue,
											   &pVar->arg.data.value.fpuiHandle);
				}
			}
			break;

		case VAR_FPUIAUXH:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFpuiAuxHandle(ln,
											   pValue,
											   &pVar->arg.data.value.fpuiAuxHandle);
				}
			}
			break;

		case VAR_OFLAGS:
			{
				int		value;

				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (STATUS_PASS == (status = argCastInt(ln, pValue, &value)))
				{
					switch (operation)
					{
						case OP_ADD:
						{
							pVar->arg.data.value.intValue += value;
						}
						break;

						case OP_SUB:
						{
							pVar->arg.data.value.intValue -= value;
						}
						break;

						default:
						{
							pVar->arg.data.value.intValue = value;
						}
						break;
					}
				}
			}
			break;

		case VAR_FAPPH:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFapph(ln, pValue, &pVar->arg.data.value.fioAppHandle);
				}
			}
			break;

		case VAR_FDEVH:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFdevh(ln, pValue, &pVar->arg.data.value.fioAppHandle);
				}
			}
			break;

		case VAR_FVER:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFver(ln, pValue, &pVar->arg.data.value.fioVer);
				}
			}
			break;

		case VAR_FVIEW:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFview(ln,
										  pValue,
										  &pVar->arg.data.value.fioView);
				}
			}
			break;

		case VAR_FCMAP:
			{
				unsigned char *pBuf;
				unsigned int size;
				unsigned int index = 0;

				if (pFile != NULL) {
					if ((operation == OP_ADD) || (operation == OP_SUB))
					{
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL, NULL);
					}
					else
					{
						if (STATUS_FAIL == (status = argSet(ln,
								configFileGetSFP(),
								pFile->arg.data.value.pCharValue,
								&pBuf,
								&size)))
						{
							break;
						}
						memset(pVar->arg.data.value.fioFcmap,
							0, pVar->arg.data.size * sizeof(FIO_CHANNEL_MAP));
						memcpy(pVar->arg.data.value.fioFcmap,
							pBuf, MIN(pVar->arg.data.size * sizeof(FIO_CHANNEL_MAP), size));
						free(pBuf);
					}
					break;
				}
				
				if (pValue == NULL)
				{
					char string[OUTPUT_STRING_MAX];
					sprintf(string, "argSetVar(): Must set data type [%s] from file or value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				// get array index, if any
				if (pIndex != NULL) {
					if (argCastUint(ln, pIndex, &index) == STATUS_PASS) {
						if (index >= pVar->arg.data.size)
							index = pVar->arg.data.size;
					} else {
						break;
					}
				}
								
				if (pMember != NULL) {
					unsigned int value;
					unsigned int *var;

					// get unsigned int value
					if ((status = argCastUint(ln, pValue, &value)) != STATUS_PASS)
						break;
					
					memberAllowed = TRUE;	
					if (!strcmp(MEM_FCMAP_OUT, pMember)) {
						var = &pVar->arg.data.value.fioFcmap[index].output_point;
					}
					else if (!strcmp(MEM_FCMAP_FIOD, pMember)) {
						var = &pVar->arg.data.value.fioFcmap[index].fiod;
					}
					else if (!strcmp(MEM_FCMAP_CHAN, pMember)) {
						var = &pVar->arg.data.value.fioFcmap[index].channel;
					}
					else if (!strcmp(MEM_FCMAP_CLR, pMember)) {
						var = &pVar->arg.data.value.fioFcmap[index].color;
					}
					else {
						// Invalid member specified
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argSetVar(): Illegal member [%s] specified for variable [%s]",
							pMember,
							pVar->pName);
						OUTPUT_ERR(ln, string, NULL, NULL);
						break;
					}
					switch (operation)
					{
						case OP_ADD:
							*var += value;
							break;
						case OP_SUB:
							*var -= value;
							break;
						default:
							*var = value;
							break;
					}
				} else {
					if ((operation == OP_ADD) || (operation == OP_SUB)) {
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL, NULL);
					} else {
						if ((pValue->argType == ARG_VAR)
							&& (pValue->varType == VAR_FCMAP)) {
							if (pIndex != NULL) {
								// Set array element
								memcpy(&pVar->arg.data.value.fioFcmap[index],
									pValue->arg.data.value.fioFcmap,
									sizeof(FIO_CHANNEL_MAP));
							} else {
								// Set whole array
								memset(pVar->arg.data.value.fioFcmap,
									0, pVar->arg.data.size * sizeof(FIO_CHANNEL_MAP));
								memcpy(pVar->arg.data.value.fioFcmap,
									pValue->arg.data.value.fioFcmap,
									MIN(pVar->arg.data.size * sizeof(FIO_CHANNEL_MAP),
									pValue->arg.data.size));
							}
						}
					}
				}
			}
			break;

		case VAR_FCMASK:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFcmask(ln,
										   pValue,
										   &pVar->arg.data.value.fioFcmask);
				}
			}
			break;

		case VAR_FCFSA:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFcfsa(ln,
										  pValue,
										  &pVar->arg.data.value.fioFcfsa);
				}
			}
			break;

		case VAR_FNOTF:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFnotf(ln,
										  pValue,
										  &pVar->arg.data.value.fioFnotf);
				}
			}
			break;

		case VAR_FINF:
			{
				unsigned char *pBuf;
				unsigned int size;
				unsigned int index = 0;

				if (pFile != NULL) {
					if ((operation == OP_ADD) || (operation == OP_SUB))
					{
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL, NULL);
					}
					else
					{
						if (STATUS_FAIL == (status = argSet(ln,
								configFileGetSFP(),
								pFile->arg.data.value.pCharValue,
								&pBuf,
								&size)))
						{
							break;
						}
						memset(pVar->arg.data.value.fioFinf,
							0, pVar->arg.data.size * sizeof(FIO_INPUT_FILTER));
						memcpy(pVar->arg.data.value.fioFinf,
							pBuf, MIN(pVar->arg.data.size * sizeof(FIO_INPUT_FILTER), size));
						free(pBuf);
					}
					break;
				}
				
				if (pValue == NULL)
				{
					char string[OUTPUT_STRING_MAX];
					sprintf(string, "argSetVar(): Must set data type [%s] from file or value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				// get array index, if any
				if (pIndex != NULL) {
					if (argCastUint(ln, pIndex, &index) == STATUS_PASS) {
						if (index >= pVar->arg.data.size)
							index = pVar->arg.data.size;
					} else {
						break;
					}
				}
								
				if (pMember != NULL) {
					unsigned int value;
					unsigned int *var;

					// get unsigned int value
					if ((status = argCastUint(ln, pValue, &value)) != STATUS_PASS)
						break;
					
					memberAllowed = TRUE;	
					if (!strcmp(MEM_FINF_IP, pMember)) {
						var = &pVar->arg.data.value.fioFinf[index].input_point;
					}
					else if (!strcmp(MEM_FINF_LEAD, pMember)) {
						var = &pVar->arg.data.value.fioFinf[index].leading;
					}
					else if (!strcmp(MEM_FINF_TRAIL, pMember)) {
						var = &pVar->arg.data.value.fioFinf[index].trailing;
					}
					else {
						// Invalid member specified
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argSetVar(): Illegal member [%s] specified for variable [%s]",
							pMember,
							pVar->pName);
						OUTPUT_ERR(ln, string, NULL, NULL);
						break;
					}
					switch (operation)
					{
						case OP_ADD:
							*var += value;
							break;
						case OP_SUB:
							*var -= value;
							break;
						default:
							*var = value;
							break;
					}
				} else {
					if ((operation == OP_ADD) || (operation == OP_SUB)) {
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL, NULL);
					} else {
						if ((pValue->argType == ARG_VAR)
							&& (pValue->varType == VAR_FINF)) {
							if (pIndex != NULL) {
								// Set array element
								memcpy(&pVar->arg.data.value.fioFinf[index],
									pValue->arg.data.value.fioFinf,
									sizeof(FIO_INPUT_FILTER));
							} else {
								// Set whole array
								memset(pVar->arg.data.value.fioFinf,
									0, pVar->arg.data.size * sizeof(FIO_INPUT_FILTER));
								memcpy(pVar->arg.data.value.fioFinf,
									pValue->arg.data.value.fioFinf,
									MIN(pVar->arg.data.size * sizeof(FIO_INPUT_FILTER),
									pValue->arg.data.size));
							}
						}
					}
				}
			}
			break;

		case VAR_FFSCHD:
			{
				unsigned char *pBuf;
				unsigned int size;
				if (pFile != NULL) {
					if ((operation == OP_ADD) || (operation == OP_SUB))
					{
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL, NULL);
					}
					else
					{
						if (STATUS_FAIL == (status = argSet(ln,
								configFileGetSFP(),
								pFile->arg.data.value.pCharValue,
								&pBuf,
								&size)))
						{
							break;
						}
						memset(pVar->arg.data.value.fioFschd,
							0, pVar->arg.data.size * sizeof(FIO_INPUT_FILTER));
						memcpy(pVar->arg.data.value.fioFschd,
							pBuf, MIN(pVar->arg.data.size * sizeof(FIO_INPUT_FILTER), size));
						free(pBuf);
					}
					break;
				}
				
				if (pValue == NULL)
				{
					char string[OUTPUT_STRING_MAX];
					sprintf(string, "argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (NULL != pMember)
				{
					unsigned int value;
					
					memberAllowed = TRUE;
					if (!strcmp(MEM_FFSCHD_REQ, pMember))
					{
						// Initialize member
						if (STATUS_PASS == (status = argCastUint(ln, pValue, &value)))
						{
							switch (operation)
							{
								case OP_ADD:
								{
									pVar->arg.data.value.fioFschd->req_frame += value;
								}
								break;
								case OP_SUB:
								{
									pVar->arg.data.value.fioFschd->req_frame -= value;
								}
								break;
								default:
								{
									pVar->arg.data.value.fioFschd->req_frame = value;
								}
								break;
							}
						}
					}
					else if (!strcmp(MEM_FFSCHD_HZ, pMember))
					{
						if (STATUS_PASS == (status = argCastUint(ln, pValue, &value)))
						{
							switch (operation)
							{
								case OP_ADD:
								{
									pVar->arg.data.value.fioFschd->frequency += value;
								}
								break;
								case OP_SUB:
								{
									pVar->arg.data.value.fioFschd->frequency -= value;
								}
								break;
								default:
								{
									pVar->arg.data.value.fioFschd->frequency = value;
								}
								break;
							}
						}
					}
					else
					{
						// Invalid member specified
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argSetVar(): Illegal member [%s] specified for variable [%s]",
							pMember,
							pVar->pName);
						OUTPUT_ERR(ln, string, NULL, NULL);
					}
				}
				else
				{
					OUTPUT_ERR(ln, "argSetVar(): Not supported", NULL, NULL);
				}
			}
			break;

		case VAR_FINT:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFint(ln,
										 pValue,
										 &pVar->arg.data.value.fioFint);
				}
			}
			break;

		case VAR_FTST:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFtst(ln,
										 pValue,
										 &pVar->arg.data.value.fioTstatus);
				}
			}
			break;

		case VAR_FTBUF:
			{
				unsigned	char	*pBuf;
				unsigned	int		size;

				if (pFile == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a file",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					if (STATUS_FAIL ==
							(status = argSet(ln,
											 configFileGetSFP(),
											 pFile->arg.data.value.pCharValue,
										 	 &pBuf,
											 &size)))
					{
						break;
					}
					memset(pVar->arg.data.value.fioTbuffer,
						   0,
						   pVar->arg.data.size * sizeof(FIO_TRANS_BUFFER));
					memcpy(pVar->arg.data.value.fioTbuffer,
						   pBuf,
						   MIN(pVar->arg.data.size * sizeof(FIO_TRANS_BUFFER),
						   	   size));
					free(pBuf);
				}
			}
			break;

		case VAR_FMFB:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFmfb(ln,
										 pValue,
										 &pVar->arg.data.value.fioFb);
				}
			}
			break;

		case VAR_FPORT:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFport(ln,
										  pValue,
										  &pVar->arg.data.value.fioPort);
				}
			}
			break;

		case VAR_FDEVT:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFdevt(ln,
										  pValue,
										  &pVar->arg.data.value.fioDt);
				}
			}
			break;

		case VAR_FFDSTAT:
			{
				unsigned char *pBuf;
				unsigned int size;

				if (pFile != NULL) {
					if ((operation == OP_ADD) || (operation == OP_SUB)) {
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL, NULL);
					} else {
						if (STATUS_FAIL == (status = argSet(ln,
								configFileGetSFP(),
								pFile->arg.data.value.pCharValue,
								&pBuf,
								&size))) {
							break;
						}
						memset(&pVar->arg.data.value.fioFiodStatus,
							0, sizeof(FIO_FIOD_STATUS));
						memcpy(&pVar->arg.data.value.fioFiodStatus,
							pBuf, MIN(pVar->arg.data.size * sizeof(FIO_FIOD_STATUS), size));
						free(pBuf);
					}
					break;
				}
				
				if (pValue == NULL) {
					char string[OUTPUT_STRING_MAX];
					sprintf(string, "argSetVar(): Must set data type [%s] from file or value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (pMember != NULL) {
					memberAllowed = TRUE;	

					if (!strcmp(MEM_FFDSTAT_INF, pMember)) {
						if ((operation == OP_ADD) || (operation == OP_SUB)) {
							OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
								NULL, NULL);
						} else if ((pValue->argType == ARG_VAR)
							&& (pValue->varType == VAR_FFINFO)) {
								memset(pVar->arg.data.value.fioFiodStatus.frame_info,
									0, pVar->arg.data.size * sizeof(FIO_FRAME_INFO));
								memcpy(pVar->arg.data.value.fioFiodStatus.frame_info,
									pValue->arg.data.value.fioFrameInfo,
									MIN(pVar->arg.data.size * sizeof(FIO_FRAME_INFO),
									pValue->arg.data.size));
						}
					} else {
						unsigned int value;
						unsigned int *var;
						
						// get unsigned int value
						if ((status = argCastUint(ln, pValue, &value)) != STATUS_PASS)
							break;

						if (!strcmp(MEM_FFDSTAT_COM, pMember))
							var = &pVar->arg.data.value.fioFiodStatus.comm_enabled;
						else if (!strcmp(MEM_FFDSTAT_SRX, pMember))
							var = &pVar->arg.data.value.fioFiodStatus.success_rx;
						else if (!strcmp(MEM_FFDSTAT_ERX, pMember))
							var = &pVar->arg.data.value.fioFiodStatus.error_rx;
						else {
							// Invalid member specified
							char string[OUTPUT_STRING_MAX];
							sprintf(string, "argSetVar(): Illegal member [%s] specified for variable [%s]",
								pMember,
								pVar->pName);
							OUTPUT_ERR(ln, string, NULL, NULL);
							break;
						}
						switch (operation)
						{
							case OP_ADD:
								*var += value;
								break;
							case OP_SUB:
								*var -= value;
								break;
							default:
								*var = value;
								break;
						}
					}
				} else {
					if ((operation == OP_ADD) || (operation == OP_SUB)) {
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL, NULL);
					} else {
						if ((pValue->argType == ARG_VAR)
							&& (pValue->varType == VAR_FFDSTAT)) {
								memset(&pVar->arg.data.value.fioFiodStatus,
									0, sizeof(FIO_FIOD_STATUS));
								memcpy(&pVar->arg.data.value.fioFiodStatus,
									&pValue->arg.data.value.fioFiodStatus,
									sizeof(FIO_FIOD_STATUS));
						}
					}
				}
			}
			break;

		case VAR_FFINFO:
			{
				unsigned char *pBuf;
				unsigned int size;
				unsigned int index = 0;

				if (pFile != NULL) {
					if ((operation == OP_ADD) || (operation == OP_SUB))
					{
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL, NULL);
					}
					else
					{
						if (STATUS_FAIL == (status = argSet(ln,
								configFileGetSFP(),
								pFile->arg.data.value.pCharValue,
								&pBuf,
								&size)))
						{
							break;
						}
						memset(pVar->arg.data.value.fioFrameInfo,
							0, pVar->arg.data.size * sizeof(FIO_FRAME_INFO));
						memcpy(pVar->arg.data.value.fioFrameInfo,
							pBuf, MIN(pVar->arg.data.size * sizeof(FIO_FRAME_INFO), size));
						free(pBuf);
					}
					break;
				}
				
				if (pValue == NULL)
				{
					char string[OUTPUT_STRING_MAX];
					sprintf(string, "argSetVar(): Must set data type [%s] from file or value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				// get array index, if any
				if (pIndex != NULL) {
					if (argCastUint(ln, pIndex, &index) == STATUS_PASS) {
						if (index >= pVar->arg.data.size)
							index = pVar->arg.data.size;
					} else {
						break;
					}
				}
								
				if (pMember != NULL) {
					unsigned int value;
					unsigned int *var;

					// get unsigned int value
					if ((status = argCastUint(ln, pValue, &value)) != STATUS_PASS)
						break;
					
					memberAllowed = TRUE;	
					if (!strcmp(MEM_FFINFO_HZ, pMember)) {
						var = &pVar->arg.data.value.fioFrameInfo[index].frequency;
					}
					else if (!strcmp(MEM_FFINFO_SRX, pMember)) {
						var = &pVar->arg.data.value.fioFrameInfo[index].success_rx;
					}
					else if (!strcmp(MEM_FFINFO_ERX, pMember)) {
						var = &pVar->arg.data.value.fioFrameInfo[index].error_rx;
					}
					else if (!strcmp(MEM_FFINFO_E10, pMember)) {
						var = &pVar->arg.data.value.fioFrameInfo[index].error_last_10;
					}
					else if (!strcmp(MEM_FFINFO_SEQ, pMember)) {
						var = &pVar->arg.data.value.fioFrameInfo[index].last_seq;
					}
					else {
						// Invalid member specified
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argSetVar(): Illegal member [%s] specified for variable [%s]",
							pMember,
							pVar->pName);
						OUTPUT_ERR(ln, string, NULL, NULL);
						break;
					}
					switch (operation)
					{
						case OP_ADD:
							*var += value;
							break;
						case OP_SUB:
							*var -= value;
							break;
						default:
							*var = value;
							break;
					}
				} else {
					if ((operation == OP_ADD) || (operation == OP_SUB)) {
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL, NULL);
					} else {
						if ((pValue->argType == ARG_VAR)
							&& (pValue->varType == VAR_FFINFO)) {
							if (pIndex != NULL) {
								// Set array element
								memcpy(&pVar->arg.data.value.fioFrameInfo[index],
									pValue->arg.data.value.fioFrameInfo,
									sizeof(FIO_FRAME_INFO));
							} else {
								// Set whole array
								memset(pVar->arg.data.value.fioFrameInfo,
									0, pVar->arg.data.size * sizeof(FIO_FRAME_INFO));
								memcpy(pVar->arg.data.value.fioFrameInfo,
									pValue->arg.data.value.fioFrameInfo,
									MIN(pVar->arg.data.size * sizeof(FIO_FRAME_INFO),
									pValue->arg.data.size));
							}
						}
					}
				}
			}
			break;

		case VAR_FFMST:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFfmst(ln,
										  pValue,
										  &pVar->arg.data.value.fioFms);
				}
			}
			break;

		case VAR_FVMST:
			{
				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					status = argCastFvmst(ln,
										  pValue,
										  &pVar->arg.data.value.fioVms);
				}
			}
			break;

		case VAR_FNOTI:
			{
				unsigned char *pBuf;
				unsigned int size;

				if (pFile != NULL) {
					if ((operation == OP_ADD)
						|| (operation == OP_SUB)) {
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL,
							NULL);
					} else {
						if (STATUS_FAIL == (status = argSet(ln,
								configFileGetSFP(),
								pFile->arg.data.value.pCharValue,
								&pBuf,
								&size))) {
							break;
						}
						memset(&pVar->arg.data.value.fioNotifyInfo, 0,
						   sizeof(FIO_NOTIFY_INFO));
						memcpy(&pVar->arg.data.value.fioNotifyInfo, pBuf,
						   MIN(sizeof(FIO_NOTIFY_INFO), size));
						free(pBuf);
					}
					break;
				}
				
				if (pValue == NULL) {
					char string[OUTPUT_STRING_MAX];
					sprintf(string,
						"argSetVar(): Must set data type [%s] from a file or value",
						argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (pMember != NULL) {
					unsigned int value;
					unsigned int *var;

					// get unsigned int value
					if ((status = argCastUint(ln, pValue, &value)) != STATUS_PASS)
						break;
					
					memberAllowed = TRUE;	
					if (!strcmp(MEM_FNOTI_RXF, pMember)) {
						var = &pVar->arg.data.value.fioNotifyInfo.rx_frame;
					}
					else if (!strcmp(MEM_FNOTI_STAT, pMember)) {
						var = &pVar->arg.data.value.fioNotifyInfo.status;
					}
					else if (!strcmp(MEM_FNOTI_SEQ, pMember)) {
						var = &pVar->arg.data.value.fioNotifyInfo.seq_number;
					}
					else if (!strcmp(MEM_FNOTI_CNT, pMember)) {
						var = &pVar->arg.data.value.fioNotifyInfo.count;
					}
					else if (!strcmp(MEM_FNOTI_FIOD, pMember)) {
						var = &pVar->arg.data.value.fioNotifyInfo.fiod;
					}
					else {
						// Invalid member specified
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argSetVar(): Illegal member [%s] specified for variable [%s]",
							pMember,
							pVar->pName);
						OUTPUT_ERR(ln, string, NULL, NULL);
						break;
					}
					switch (operation)
					{
						case OP_ADD:
							*var += value;
							break;
						case OP_SUB:
							*var -= value;
							break;
						default:
							*var = value;
							break;
					}
				} else {
					if ((operation == OP_ADD) || (operation == OP_SUB)) {
						OUTPUT_ERR(ln, "argSetVar(): Operation add/subtract not allowed on this variable type",
							NULL, NULL);
					} else {
						if ((pValue->argType == ARG_VAR)
							&& (pValue->varType == VAR_FNOTI)) {
								memcpy(&pVar->arg.data.value.fioNotifyInfo,
									&pValue->arg.data.value.fioNotifyInfo,
									sizeof(FIO_NOTIFY_INFO));
						}
					}
				}
			}
			break;

		case VAR_TVAL:
			{
				if (pFile != NULL)
				{
					unsigned	char	*pBuf;
					unsigned	int		size;

					if (   (operation == OP_ADD)
						|| (operation == OP_SUB))
					{
						OUTPUT_ERR(ln, 
								   "argSetVar(): Operation add/subtract not allowed on this variable type",
								   NULL,
								   NULL);
					}
					else
					{
						if (STATUS_FAIL ==
								(status = argSet(ln,
												 configFileGetSFP(),
												 pFile->arg.data.value.pCharValue,
											 	 &pBuf,
												 &size)))
						{
							break;
						}
						memset(&pVar->arg.data.value.timevalValue,
							   0,
							   sizeof(struct timeval));
						memcpy(&pVar->arg.data.value.timevalValue,
							   pBuf,
							   MIN(sizeof(struct timeval), size));
						free(pBuf);
					}
					break;
				}

				if (pValue == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a value",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (NULL != pMember)
				{
					memberAllowed = TRUE;
					if (!strcmp(MEM_TV_SEC, pMember))
					{
						// Initialize member
						long		value;

						if (STATUS_PASS == (status = argCastLong(ln,
																 pValue,
																 &value)))
						{
							switch (operation)
							{
								case OP_ADD:
								{
									pVar->arg.data.value.timevalValue.tv_sec += value;
								}
								break;

								case OP_SUB:
								{
									pVar->arg.data.value.timevalValue.tv_sec -= value;
								}
								break;

								default:
								{
									pVar->arg.data.value.timevalValue.tv_sec = value;
								}
								break;
							}
						}
					}
					else if (!strcmp(MEM_TV_USEC, pMember))
					{
						// Initialize member
						long		value;

						if (STATUS_PASS == (status = argCastLong(ln,
																 pValue,
																 &value)))
						{
							switch (operation)
							{
								case OP_ADD:
								{
									pVar->arg.data.value.timevalValue.tv_usec += value;
								}
								break;

								case OP_SUB:
								{
									pVar->arg.data.value.timevalValue.tv_usec -= value;
								}
								break;

								default:
								{
									pVar->arg.data.value.timevalValue.tv_usec = value;
								}
								break;
							}
						}
					}
					else
					{
						// Invalid member specified
						char	string[OUTPUT_STRING_MAX];
						sprintf(string,
								"argSetVar(): Illegal member [%s] specified for variable [%s]",
								pMember,
								pVar->pName);
						OUTPUT_ERR(ln, string, NULL, NULL);
					}
				}
				else
				{
					OUTPUT_ERR(ln, "argSetVar(): Not supported", NULL, NULL);
				}
			}
			break;

		case VAR_DSTIT:
			{
				unsigned	char	*pBuf;
				unsigned	int		size;

				if (pFile == NULL)
				{
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argSetVar(): Must set data type [%s] from a file",
							argVarStringGet(pVar->varType));
					OUTPUT_ERR(ln, string, NULL, NULL);
					break;
				}

				if (   (operation == OP_ADD)
					|| (operation == OP_SUB))
				{
					OUTPUT_ERR(ln, 
							   "argSetVar(): Operation add/subtract not allowed on this variable type",
							   NULL,
							   NULL);
				}
				else
				{
					if (STATUS_FAIL ==
							(status = argSet(ln,
											 configFileGetSFP(),
											 pFile->arg.data.value.pCharValue,
										 	 &pBuf,
											 &size)))
					{
						break;
					}
					memset(&pVar->arg.data.value.dstinfoValue,
						   0,
						   sizeof(dst_info_t));
					memcpy(&pVar->arg.data.value.dstinfoValue,
						   pBuf,
						   MIN(sizeof(dst_info_t), size));
					free(pBuf);
				}
			}
			break;

		default:
			{
				// Bad variable type
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argSetVar(): Unsupported type [%d] for variable [%s]",
						pVar->varType,
						pVar->pName);
				OUTPUT_ERR(ln, string, NULL, NULL);
			}
			break;
	}

	// See if member passed but not allowed
	if ((NULL != pMember) && !memberAllowed)
	{
		// pMember was passed but was not allowed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argSetVar(): Member name [%s] not allowed, for variable [%s]",
				pMember,
				pVar->pName);
		OUTPUT_ERR(ln, string, NULL, NULL);
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to an int
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from from argument
 * \param[in]	pMember - Pointer to member name, if present
 * \param[out]	pLong - Point to long to compare to return
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCastNumCompare(uint16_t lineNumber, ARG_P *pArg, char *pMember, long *pLong)
{
	char string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;
	boolean	memberAllowed = FALSE;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				case VAR_BOOL:
				case VAR_OFLAGS:
					*pLong = (long)pArg->arg.data.value.intValue;
					break;
				case VAR_UINT:
					*pLong = (long)pArg->arg.data.value.uintValue;
					break;
				case VAR_SSIZET:
					*pLong = (long)pArg->arg.data.value.ssizetValue;
					break;
				case VAR_FPUIH:
					*pLong = (long)pArg->arg.data.value.fpuiHandle;
					break;
				case VAR_FPUIAUXH:
					*pLong = (long)pArg->arg.data.value.fpuiAuxHandle;
					break;
				case VAR_FAPPH:
					*pLong = (long)pArg->arg.data.value.fioAppHandle;
					break;
				case VAR_FDEVH:
					*pLong = (long)pArg->arg.data.value.fioDevHandle;
					break;
				case VAR_FVER:
					*pLong = (long)pArg->arg.data.value.fioVer;
					break;
				case VAR_FVIEW:
					*pLong = (long)pArg->arg.data.value.fioView;
					break;
				case VAR_FCMASK:
					*pLong = (long)pArg->arg.data.value.fioFcmask;
					break;
				case VAR_FCFSA:
					*pLong = (long)pArg->arg.data.value.fioFcfsa;
					break;
				case VAR_FNOTF:
					*pLong = (long)pArg->arg.data.value.fioFnotf;
					break;
				case VAR_FINT:
					*pLong = (long)pArg->arg.data.value.fioFint;
					break;
				case VAR_FTST:
					*pLong = (long)pArg->arg.data.value.fioTstatus;
					break;
				case VAR_FMFB:
					*pLong = (long)pArg->arg.data.value.fioFb;
					break;
				case VAR_FPORT:
					*pLong = (long)pArg->arg.data.value.fioPort;
					break;
				case VAR_FDEVT:
					*pLong = (long)pArg->arg.data.value.fioDt;
					break;
				case VAR_FFMST:
					*pLong = (long)pArg->arg.data.value.fioFms;
					break;
				case VAR_FVMST:
					*pLong = (long)pArg->arg.data.value.fioVms;
					break;
				case VAR_TVAL:
					if (NULL != pMember)
					{
						memberAllowed = TRUE;
						// Initialize member
						if (!strcmp(MEM_TV_SEC, pMember))
						{
							*pLong = pArg->arg.data.value.timevalValue.tv_sec;
						}
						else if (!strcmp(MEM_TV_USEC, pMember))
						{
							*pLong = pArg->arg.data.value.timevalValue.tv_usec;
						}
						else
						{
							// Invalid member specified
							char	string[OUTPUT_STRING_MAX];
							sprintf(string,
									"argCastNumCompare(): Illegal member [%s] specified for variable [%s]",
									pMember,
									pArg->pName);
							OUTPUT_ERR(lineNumber, string, NULL, NULL);
							status = STATUS_FAIL;
						}
					}
					else
					{
						// Illegal member
						char	string[OUTPUT_STRING_MAX];
						sprintf(string,
								"argCastNumCompare(): No member specified for variable [%s]",
								pArg->pName);
						OUTPUT_ERR(lineNumber, string, NULL, NULL);
						status = STATUS_FAIL;
					}
					break;

				case VAR_FINF:
					if (NULL != pMember)
					{
						memberAllowed = TRUE;
						// Initialize member
						if (!strcmp(MEM_FINF_IP, pMember))
						{
							*pLong = pArg->arg.data.value.fioFinf->input_point;
						}
						else if (!strcmp(MEM_FINF_LEAD, pMember))
						{
							*pLong = pArg->arg.data.value.fioFinf->leading;
						}
						else if (!strcmp(MEM_FINF_TRAIL, pMember))
						{
							*pLong = pArg->arg.data.value.fioFinf->trailing;
						}
						else
						{
							// Invalid member specified
							char string[OUTPUT_STRING_MAX];
							sprintf(string, "argCastNumCompare(): Illegal member [%s] specified for variable [%s]",
								pMember,
								pArg->pName);
							OUTPUT_ERR(lineNumber, string, NULL, NULL);
							status = STATUS_FAIL;
						}
					}
					else
					{
						// Illegal member
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argCastNumCompare(): No member specified for variable [%s]",
							pArg->pName);
						OUTPUT_ERR(lineNumber, string, NULL, NULL);
						status = STATUS_FAIL;
					}
					break;

				case VAR_FFSCHD:
					if (NULL != pMember)
					{
						memberAllowed = TRUE;
						// Initialize member
						if (!strcmp(MEM_FFSCHD_REQ, pMember))
						{
							*pLong = pArg->arg.data.value.fioFschd->req_frame;
						}
						else if (!strcmp(MEM_FFSCHD_HZ, pMember))
						{
							*pLong = pArg->arg.data.value.fioFschd->req_frame;
						}
						else
						{
							// Invalid member specified
							char string[OUTPUT_STRING_MAX];
							sprintf(string, "argCastNumCompare(): Illegal member [%s] specified for variable [%s]",
								pMember,
								pArg->pName);
							OUTPUT_ERR(lineNumber, string, NULL, NULL);
							status = STATUS_FAIL;
						}
					}
					else
					{
						// Illegal member
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argCastNumCompare(): No member specified for variable [%s]",
							pArg->pName);
						OUTPUT_ERR(lineNumber, string, NULL, NULL);
						status = STATUS_FAIL;
					}
					break;

				case VAR_FFDSTAT:
					if (NULL != pMember)
					{
						memberAllowed = TRUE;
						// Initialize member
						if (!strcmp(MEM_FFDSTAT_COM, pMember))
						{
							*pLong = pArg->arg.data.value.fioFiodStatus.comm_enabled;
						}
						else if (!strcmp(MEM_FFDSTAT_SRX, pMember))
						{
							*pLong = pArg->arg.data.value.fioFiodStatus.success_rx;
						}
						else if (!strcmp(MEM_FFDSTAT_ERX, pMember))
						{
							*pLong = pArg->arg.data.value.fioFiodStatus.error_rx;
						}
						else
						{
							// Invalid member specified
							char string[OUTPUT_STRING_MAX];
							sprintf(string, "argCastNumCompare(): Illegal member [%s] specified for variable [%s]",
								pMember,
								pArg->pName);
							OUTPUT_ERR(lineNumber, string, NULL, NULL);
							status = STATUS_FAIL;
						}
					}
					else
					{
						// Illegal member
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argCastNumCompare(): No member specified for variable [%s]",
							pArg->pName);
						OUTPUT_ERR(lineNumber, string, NULL, NULL);
						status = STATUS_FAIL;
					}
					break;

				case VAR_FFINFO:
					if (pMember != NULL)
					{
						memberAllowed = TRUE;
						// Initialize member
						if (!strcmp(MEM_FFINFO_HZ, pMember))
						{
							*pLong = pArg->arg.data.value.fioFrameInfo->frequency;
						}
						else if (!strcmp(MEM_FFINFO_SRX, pMember))
						{
							*pLong = pArg->arg.data.value.fioFrameInfo->success_rx;
						}
						else if (!strcmp(MEM_FFINFO_ERX, pMember))
						{
							*pLong = pArg->arg.data.value.fioFrameInfo->error_rx;
						}
						else if (!strcmp(MEM_FFINFO_E10, pMember))
						{
							*pLong = pArg->arg.data.value.fioFrameInfo->error_last_10;
						}
						else if (!strcmp(MEM_FFINFO_SEQ, pMember))
						{
							*pLong = pArg->arg.data.value.fioFrameInfo->last_seq;
						}
						else
						{
							// Invalid member specified
							char string[OUTPUT_STRING_MAX];
							sprintf(string, "argCastNumCompare(): Illegal member [%s] specified for variable [%s]",
								pMember,
								pArg->pName);
							OUTPUT_ERR(lineNumber, string, NULL, NULL);
							status = STATUS_FAIL;
						}
					}
					else
					{
						// Illegal member
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argCastNumCompare(): No member specified for variable [%s]",
							pArg->pName);
						OUTPUT_ERR(lineNumber, string, NULL, NULL);
						status = STATUS_FAIL;
					}
					break;

				case VAR_FCMAP:
					if (pMember != NULL)
					{
						memberAllowed = TRUE;
						// Initialize member
						if (!strcmp(MEM_FCMAP_OUT, pMember))
						{
							*pLong = pArg->arg.data.value.fioFcmap->output_point;
						}
						else if (!strcmp(MEM_FCMAP_FIOD, pMember))
						{
							*pLong = pArg->arg.data.value.fioFcmap->fiod;
						}
						else if (!strcmp(MEM_FCMAP_CHAN, pMember))
						{
							*pLong = pArg->arg.data.value.fioFcmap->channel;
						}
						else if (!strcmp(MEM_FCMAP_CLR, pMember))
						{
							*pLong = pArg->arg.data.value.fioFcmap->color;
						}
						else
						{
							// Invalid member specified
							char string[OUTPUT_STRING_MAX];
							sprintf(string, "argCastNumCompare(): Illegal member [%s] specified for variable [%s]",
								pMember,
								pArg->pName);
							OUTPUT_ERR(lineNumber, string, NULL, NULL);
							status = STATUS_FAIL;
						}
					}
					else
					{
						// Illegal member
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argCastNumCompare(): No member specified for variable [%s]",
							pArg->pName);
						OUTPUT_ERR(lineNumber, string, NULL, NULL);
						status = STATUS_FAIL;
					}
					break;

				case VAR_FNOTI:
					if (pMember != NULL)
					{
						memberAllowed = TRUE;
						// Initialize member
						if (!strcmp(MEM_FNOTI_RXF, pMember)) {
							*pLong = pArg->arg.data.value.fioNotifyInfo.rx_frame;
						} else if (!strcmp(MEM_FNOTI_STAT, pMember)) {
							*pLong = pArg->arg.data.value.fioNotifyInfo.status;
						} else if (!strcmp(MEM_FNOTI_SEQ, pMember)) {
							*pLong = pArg->arg.data.value.fioNotifyInfo.seq_number;
						} else if (!strcmp(MEM_FNOTI_CNT, pMember)) {
							*pLong = pArg->arg.data.value.fioNotifyInfo.count;
						} else if (!strcmp(MEM_FNOTI_FIOD, pMember)) {
							*pLong = pArg->arg.data.value.fioNotifyInfo.fiod;
						} else {
							// Invalid member specified
							char string[OUTPUT_STRING_MAX];
							sprintf(string, "argCastNumCompare(): Illegal member [%s] specified for variable [%s]",
								pMember,
								pArg->pName);
							OUTPUT_ERR(lineNumber, string, NULL, NULL);
							status = STATUS_FAIL;
						}
					}
					else
					{
						// Illegal member
						char string[OUTPUT_STRING_MAX];
						sprintf(string, "argCastNumCompare(): No member specified for variable [%s]",
							pArg->pName);
						OUTPUT_ERR(lineNumber, string, NULL, NULL);
						status = STATUS_FAIL;
					}
					break;

				case VAR_VOID:
				case VAR_CHAR:
				case VAR_PCHAR:
				case VAR_UCHAR:
				case VAR_PUCHAR:
				case VAR_FTBUF:
				case VAR_DSTIT:
				default:
					// Finish implementing all cast types
					sprintf(string,
							"argCastNumCompare(): Cannot cast [%s, %s] to [%s]",
							pArg->pName,
							argVarStringGet(pArg->varType),
							argVarStringGet(VAR_INT));
					OUTPUT_ERR(lineNumber, string, NULL, NULL);
					status = STATUS_FAIL;
					break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pLong = (long)pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			int		value;
			int		*pInt = &value;
			status = argProcessMacro(lineNumber, pArg, &pInt);
			*pLong = (long)value;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastNumCompare(): Incompatible argument [%s] type for [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_INT));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	if ((status == STATUS_PASS) && (pMember != NULL)
		&& (memberAllowed == FALSE)) {
		sprintf(string,
			"argCastNumCompare(): Member [%s] not allowed",
			pMember);
		OUTPUT_ERR(lineNumber, string, NULL, NULL);
		status = STATUS_FAIL;
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to an int
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from from argument
 * \param[in]	pInt - Point to integer to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastInt(uint16_t		lineNumber,
		   ARG_P		*pArg,
		   int			*pInt)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				case VAR_BOOL:
				case VAR_OFLAGS:
				{
					*pInt = pArg->arg.data.value.intValue;
				}
				break;

				case VAR_UINT:
				{
					*pInt = (int)pArg->arg.data.value.uintValue;
				}
				break;

				case VAR_FPUIH:
				{
					*pInt = (int)pArg->arg.data.value.fpuiHandle;
				}
				break;

				case VAR_FPUIAUXH:
				{
					*pInt = (int)pArg->arg.data.value.fpuiAuxHandle;
				}
				break;

				case VAR_SSIZET:
				{
					*pInt = (int)pArg->arg.data.value.ssizetValue;
				}
				break;

				case VAR_FAPPH:
				{
					*pInt = (int)pArg->arg.data.value.fioAppHandle;
				}
				break;

				case VAR_FDEVH:
				{
					*pInt = (int)pArg->arg.data.value.fioDevHandle;
				}
				break;

				case VAR_FVER:
				{
					*pInt = (int)pArg->arg.data.value.fioVer;
				}
				break;

				case VAR_FVIEW:
				{
					*pInt = (int)pArg->arg.data.value.fioView;
				}
				break;

				case VAR_FCMASK:
				{
					*pInt = (int)pArg->arg.data.value.fioFcmask;
				}
				break;

				case VAR_FCFSA:
				{
					*pInt = (int)pArg->arg.data.value.fioFcfsa;
				}
				break;

				case VAR_FNOTF:
				{
					*pInt = (int)pArg->arg.data.value.fioFnotf;
				}
				break;

				case VAR_FINT:
				{
					*pInt = (int)pArg->arg.data.value.fioFint;
				}
				break;

				case VAR_FTST:
				{
					*pInt = (int)pArg->arg.data.value.fioTstatus;
				}
				break;

				case VAR_FMFB:
				{
					*pInt = (int)pArg->arg.data.value.fioFb;
				}
				break;

				case VAR_FPORT:
				{
					*pInt = (int)pArg->arg.data.value.fioPort;
				}
				break;

				case VAR_FDEVT:
				{
					*pInt = (int)pArg->arg.data.value.fioDt;
				}
				break;

				case VAR_FFMST:
				{
					*pInt = (int)pArg->arg.data.value.fioFms;
				}
				break;

				case VAR_FVMST:
				{
					*pInt = (int)pArg->arg.data.value.fioVms;
				}
				break;

				case VAR_VOID:
				case VAR_CHAR:
				case VAR_PCHAR:
				case VAR_UCHAR:
				case VAR_PUCHAR:
				case VAR_FCMAP:
				case VAR_FINF:
				case VAR_FTBUF:
				case VAR_FFDSTAT:
				case VAR_FFINFO:
				case VAR_FNOTI:
				case VAR_TVAL:
				case VAR_DSTIT:
				default:
				{
					// Finish implementing all cast types
					sprintf(string,
							"argCastInt(): Cannot cast [%s, %s] to [%s]",
							pArg->pName,
							argVarStringGet(pArg->varType),
							argVarStringGet(VAR_INT));
					OUTPUT_ERR(lineNumber, string, NULL, NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pInt = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			status = argProcessMacro(lineNumber, pArg, &pInt);
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastInt(): Incompatible argument [%s] type for [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_INT));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a fpui_handle
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFH - Point to fpui_handle to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFpuiHandle(uint16_t		lineNumber,
		   		  ARG_P			*pArg,
		   		  fpui_handle	*pFH)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFH = (fpui_handle)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FPUIH:
				{
					*pFH = pArg->arg.data.value.fpuiHandle;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFpuiHandle(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFH = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFpuiHandle(): ARG_MACRO NOT Implemented",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFpuiHandle(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FPUIH));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a fpui_aux_handle
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFH - Point to fpui_handle to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFpuiAuxHandle(uint16_t		lineNumber,
		   		  ARG_P			*pArg,
		   		  fpui_aux_handle	*pFAuxH)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFAuxH = (fpui_aux_handle)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FPUIAUXH:
				{
					*pFAuxH = pArg->arg.data.value.fpuiAuxHandle;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFpuiAuxHandle(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFAuxH = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFpuiAuxHandle(): ARG_MACRO NOT Implemented",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFpuiAuxHandle(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FPUIAUXH));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_APP_HANDLE
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFH - Point to fpui_handle to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFapph(uint16_t		lineNumber,
		   	 ARG_P			*pArg,
		   	 FIO_APP_HANDLE	*pFapph)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFapph = (FIO_APP_HANDLE)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FAPPH:
				{
					*pFapph = pArg->arg.data.value.fioAppHandle;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFapph(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFapph = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFapph(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFapph(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FAPPH));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_VERSION
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFver - Pointer to FIO_VERSION to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFver(uint16_t		lineNumber,
		   	ARG_P			*pArg,
		   	FIO_VERSION		*pFver)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFver = (FIO_VERSION)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FVER:
				{
					*pFver = pArg->arg.data.value.fioVer;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFver(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFver = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFver(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFver(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FVER));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_TS1_VM_STATE
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFvms - Pointer to VMS to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFvmst(uint16_t			lineNumber,
		   	 ARG_P				*pArg,
		   	 FIO_TS1_VM_STATE	*pFvms)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFvms = (FIO_TS1_VM_STATE)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FVMST:
				{
					*pFvms = pArg->arg.data.value.fioVms;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFvmst(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFvms = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFvmst(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFvmst(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FVMST));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_VIEW
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFview - Pointer to FIO_VIEW to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFview(uint16_t	lineNumber,
		   	 ARG_P		*pArg,
		   	 FIO_VIEW	*pFview)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFview = (FIO_VIEW)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FVIEW:
				{
					*pFview = pArg->arg.data.value.fioView;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFview(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFview = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFview(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFview(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FVIEW));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_TS_FM_STATE
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFfms - Pointer to FIO_TS_FM_STATE to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFfmst(uint16_t			lineNumber,
		   	 ARG_P				*pArg,
		   	 FIO_TS_FM_STATE	*pFfms)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFfms = (FIO_TS_FM_STATE)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FFMST:
				{
					*pFfms = pArg->arg.data.value.fioFms;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFfmst(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFfms = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFfmst(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFfmst(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FFMST));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_MMU_FLASH_BIT
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFmfb - Pointer to FIO_MMU_FLASH_BIT to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFmfb(uint16_t			lineNumber,
		   	ARG_P				*pArg,
		   	FIO_MMU_FLASH_BIT	*pFmfb)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFmfb = (FIO_MMU_FLASH_BIT)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FMFB:
				{
					*pFmfb = pArg->arg.data.value.fioFb;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFmfb(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFmfb = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFmfb(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFmfb(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FMFB));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_INPUTS_TYPE
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFint - Pointer to FIO_INPUTS_TYPE to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFint(uint16_t			lineNumber,
		   	ARG_P				*pArg,
		   	FIO_INPUTS_TYPE		*pFint)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFint = (FIO_INPUTS_TYPE)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FINT:
				{
					*pFint = pArg->arg.data.value.fioFint;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFint(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFint = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFint(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFint(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FINT));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_NOTIFY
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFnotf - Pointer to FIO_NOTIFY to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFnotf(uint16_t lineNumber, ARG_P *pArg, FIO_NOTIFY *pFnotf)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFnotf = (FIO_NOTIFY)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FNOTF:
				{
					*pFnotf = pArg->arg.data.value.fioFnotf;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFnotf(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFnotf = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFnotf(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFnotf(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FNOTF));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_CMU_FSA
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFcfsa - Pointer to FIO_CMU_FSA to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFcfsa(uint16_t		lineNumber,
		   	 ARG_P			*pArg,
		   	 FIO_CMU_FSA	*pFcfsa)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFcfsa = (FIO_CMU_FSA)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FCFSA:
				{
					*pFcfsa = pArg->arg.data.value.fioFcfsa;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFcfsa(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFcfsa = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFcfsa(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFcfsa(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FCFSA));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_CMU_DC_MASK
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFcmask - Pointer to FIO_CMU_DC_MASK to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFcmask(uint16_t			lineNumber,
		   	 ARG_P				*pArg,
		   	 FIO_CMU_DC_MASK	*pFcmask)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFcmask = (FIO_CMU_DC_MASK)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FCMASK:
				{
					*pFcmask = pArg->arg.data.value.fioFcmask;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFcmask(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFcmask = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFcmask(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFcmask(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FCMASK));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_TRANS_STATUS
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFcmask - Pointer to FIO_TRANS_STATUS to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFtst(uint16_t			lineNumber,
		   	 ARG_P				*pArg,
		   	 FIO_TRANS_STATUS	*pFtst)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFtst = (FIO_TRANS_STATUS)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FTST:
				{
					*pFtst = pArg->arg.data.value.fioTstatus;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFtst(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFtst = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFtst(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFtst(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FCMASK));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_DEV_HANDLE
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFH - Point to fpui_handle to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFdevh(uint16_t		lineNumber,
		   	 ARG_P			*pArg,
		   	 FIO_DEV_HANDLE	*pFdevh)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFdevh = (FIO_DEV_HANDLE)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FDEVH:
				{
					*pFdevh = pArg->arg.data.value.fioDevHandle;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFdevh(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFdevh = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFdevh(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFdevh(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FDEVH));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_PORT
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFH - Point to fpui_handle to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFport(uint16_t		lineNumber,
		   	 ARG_P			*pArg,
		   	 FIO_PORT		*pFport)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFport = (FIO_PORT)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FPORT:
				{
					*pFport = pArg->arg.data.value.fioPort;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFport(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFport = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFport(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFport(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FPORT));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a FIO_DEVICE_TYPE
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pFH - Point to fpui_handle to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastFdevt(uint16_t			lineNumber,
		   	 ARG_P				*pArg,
		   	 FIO_DEVICE_TYPE	*pFdevt)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				{
					*pFdevt = (FIO_DEVICE_TYPE)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FDEVT:
				{
					*pFdevt = pArg->arg.data.value.fioDt;
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastFdevt(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pFdevt = pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastFdevt(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastFdevt(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_FDEVT));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a long
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from argument
 * \param[in]	pLong - Point to long to return
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCastLong(uint16_t lineNumber, ARG_P *pArg, long *pLong)
{
	char string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			OUTPUT_ERR(lineNumber,
						"argCastLong(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pLong = (long)pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastLong(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastLong(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					"long");
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to an int
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from from argument
 * \param[in]	pChar - Point to char to return
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCastChar(uint16_t lineNumber, ARG_P *pArg, char *pChar)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_CHAR:
				{
					*pChar = pArg->arg.data.value.charValue;
				}
				break;

				case VAR_PCHAR:
				{
					if (pArg->arg.data.value.pCharValue)
					{
						*pChar = *pArg->arg.data.value.pCharValue;
					}
					else
					{
						*pChar = '\0';
						sprintf(string,
								"argCastChar(): Cannot assign [%s] from NULL pointer",
								argVarStringGet(VAR_CHAR));
						OUTPUT_ERR(lineNumber, string, NULL, NULL);
						status = STATUS_FAIL;
					}
				}
				break;

				default:
				{
					OUTPUT_ERR(lineNumber,
								"argCastChar(): Cast not allowed from this data type",
								NULL,
								NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pChar = (char)pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastChar(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		{
			if (pArg->arg.data.value.pCharValue)
			{
				*pChar = *pArg->arg.data.value.pCharValue;
			}
			else
			{
				sprintf(string,
						"argCastChar(): Cannot assign [%s] from NULL pointer",
						argVarStringGet(VAR_CHAR));
				OUTPUT_ERR(lineNumber, string, NULL, NULL);
				status = STATUS_FAIL;
			}
		}
		break;
		
		default:
		{
			sprintf(string,
					"argCastChar(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_CHAR));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to an pchar
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pFrom - Pointer from from argument
 * \param[in]	pTo - Point to to return
 * \return		STATUS_PASS - Parse occurred correctly
 * 				STATUS_FAIL - Parse failed
 */
int16_t
argCastPchar(uint16_t		lineNumber,
		     ARG_P			*pFrom,
		     ARG_P			*pTo)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pFrom->argType)
	{
		case ARG_VAR:
		{
			switch(pFrom->varType)
			{
				case VAR_CHAR:
				{
					char	*ptr = pTo->arg.data.value.pCharValue;
					*ptr++ = pFrom->arg.data.value.charValue;
					*ptr   = '\0';
				}
				break;

				case VAR_PCHAR:
				{
					strncpy(pTo->arg.data.value.pCharValue,
							pFrom->arg.data.value.pCharValue,
							pTo->arg.data.size);
				}
				break;

				// Cast all appropriate data types
				default:
				{
					sprintf(string,
							"argCastPchar(): Cannot cast argument [%s, %s] to a [%s]",
							pFrom->pName,
							argVarStringGet(pFrom->varType),
							argVarStringGet(VAR_PCHAR));
					OUTPUT_ERR(lineNumber, string, NULL, NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastPchar(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		{
			strncpy(pTo->arg.data.value.pCharValue,
					pFrom->arg.data.value.pCharValue,
					pTo->arg.data.size);
			pTo->arg.data.value.pCharValue[pTo->arg.data.size - 1] = '\0';
		}
		break;
		
		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
			if (!strcmp(pFrom->pName, NC_NULL)) {
				pTo->arg.data.value.pCharValue = NULL;
			}
			break;	
		default:
		{
			sprintf(string,
					"argCastPchar(): Cannot cast argument [%s, %s] to a [%s]",
					pFrom->pName,
					argVarStringGet(pFrom->varType),
					argVarStringGet(VAR_PCHAR));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a uchar
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from from argument
 * \param[in]	pUchar - Point to uchar to return
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCastUchar(uint16_t lineNumber, ARG_P *pArg, unsigned char *pUchar)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			OUTPUT_ERR(lineNumber,
						"argCastUchar(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pUchar = (unsigned char)pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastUchar(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		{
			*pUchar = (unsigned char)*pArg->arg.data.value.pCharValue;
		}
		break;
		
		default:
		{
			sprintf(string,
					"argCastUchar(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_UCHAR));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a puchar
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pFrom - Pointer from from argument
 * \param[in]	pTo - Point to to return
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCastPuchar(uint16_t lineNumber, ARG_P *pFrom, ARG_P *pTo)
{
	char string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pFrom->argType)
	{
		case ARG_VAR:
		{
			switch(pFrom->varType)
			{
				case VAR_PCHAR:
				{
					strncpy((char *)pTo->arg.data.value.pUcharValue,
							pFrom->arg.data.value.pCharValue,
							pTo->arg.data.size);
				}
				break;

				case VAR_PUCHAR:
				{
					memset(pTo->arg.data.value.pUcharValue,
						   0,
						   pTo->arg.data.size);
					memcpy(pTo->arg.data.value.pUcharValue,
						   pFrom->arg.data.value.pUcharValue,
						   MIN(pTo->arg.data.size, pFrom->arg.data.size));
				}
				break;

				// Cast all appropriate data types
				default:
				{
					sprintf(string,
							"argCastPuchar(): Cannot cast argument [%s, %s] to a [%s]",
							pFrom->pName,
							argVarStringGet(pFrom->varType),
							argVarStringGet(VAR_PUCHAR));
					OUTPUT_ERR(lineNumber, string, NULL, NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastPuchar(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		{
			strncpy((char *)pTo->arg.data.value.pUcharValue,
					pFrom->arg.data.value.pCharValue,
					pTo->arg.data.size);
			pTo->arg.data.value.pUcharValue[pTo->arg.data.size - 1] = '\0';
		}
		break;
		
		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		default:
		{
			sprintf(string,
					"argCastPuchar(): Cannot cast argument [%s, %s] to a [%s]",
					pFrom->pName,
					argVarStringGet(pFrom->varType),
					argVarStringGet(VAR_PUCHAR));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a ssize_t
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from from argument
 * \param[in]	pSsizet - Point to ssize_t to return
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCastSsizet(uint16_t lineNumber, ARG_P *pArg, ssize_t *pSsizet)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			OUTPUT_ERR(lineNumber,
						"argCastSsizet(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pSsizet = (ssize_t)pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			OUTPUT_ERR(lineNumber,
						"argCastSsizet(): Cast not allowed from this data type",
						NULL,
						NULL);
			status = STATUS_FAIL;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastSsizet(): Cannot assign argument [%s] to a [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_SSIZET));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a boolean
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from from argument
 * \param[in]	pBoolean - Point to boolean to return
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCastBoolean(uint16_t lineNumber, ARG_P *pArg, boolean *pBoolean)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				case VAR_BOOL:
				{
					*pBoolean = (boolean)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FPUIH:
				{
					*pBoolean = (boolean)pArg->arg.data.value.fpuiHandle;
				}
				break;

				case VAR_FPUIAUXH:
				{
					*pBoolean = (boolean)pArg->arg.data.value.fpuiAuxHandle;
				}
				break;

				case VAR_SSIZET:
				{
					*pBoolean = (boolean)pArg->arg.data.value.ssizetValue;
				}
				break;

				default:
				{
					// Finish implementing all cast types
					sprintf(string,
							"argCastInt(): Cannot cast [%s, %s] to [%s]",
							pArg->pName,
							argVarStringGet(pArg->varType),
							argVarStringGet(VAR_INT));
					OUTPUT_ERR(lineNumber, string, NULL, NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pBoolean = (boolean)pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			int		intValue;
			int		*pInt = &intValue;
			status = argProcessMacro(lineNumber, pArg, &pInt);
			*pBoolean = (boolean)intValue;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastBoolean(): Incompatible argument [%s] type for [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_BOOL));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to an unsigned int
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from from argument
 * \param[in]	pUint - Point to unsigned integer to return
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCastUint(uint16_t lineNumber, ARG_P *pArg, unsigned int *pUint)
{
	char	string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_INT:
				case VAR_BOOL:
				{
					*pUint = (unsigned int)pArg->arg.data.value.intValue;
				}
				break;

				case VAR_FPUIH:
				{
					*pUint = (unsigned int)pArg->arg.data.value.fpuiHandle;
				}
				break;

				case VAR_FPUIAUXH:
				{
					*pUint = (unsigned int)pArg->arg.data.value.fpuiAuxHandle;
				}
				break;

				case VAR_SSIZET:
				{
					*pUint = (unsigned int)pArg->arg.data.value.ssizetValue;
				}
				break;

				case VAR_UINT:
				{
					*pUint = pArg->arg.data.value.uintValue;
				}
				break;

				default:
				{
					// Finish implementing all cast types
					sprintf(string,
							"argCastInt(): Cannot cast [%s, %s] to [%s]",
							pArg->pName,
							argVarStringGet(pArg->varType),
							argVarStringGet(VAR_INT));
					OUTPUT_ERR(lineNumber, string, NULL, NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		{
			// Named constant is always a VAR_INT
			*pUint = (unsigned int)pArg->arg.data.value.intValue;
		}
		break;

		case ARG_MACRO:
		{
			int		intValue;
			int		*pInt = &intValue;
			status = argProcessMacro(lineNumber, pArg, &pInt);
			*pUint = (unsigned int)intValue;
		}
		break;

		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastInt(): Incompatible argument [%s] type for [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_INT));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a struct timeval
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from from argument
 * \param[out]	pTv - Point to timeval struct to return
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCastTv(uint16_t lineNumber, ARG_P *pArg, struct timeval *pTv)
{
	char string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_TVAL:
				{
					*pTv = pArg->arg.data.value.timevalValue;
				}
				break;

				default:
				{
					sprintf(string,
							"argCastTv(): Cannot cast [%s, %s] to [%s]",
							pArg->pName,
							argVarStringGet(pArg->varType),
							argVarStringGet(VAR_TVAL));
					OUTPUT_ERR(lineNumber, string, NULL, NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		case ARG_MACRO:
		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastTv(): Incompatible argument [%s] type for [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_TVAL));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function casts an argument to a dst_info_t
 *
 * \param[in]	lineNumber - Line number in APIVSXML file
 * \param[in]	pArg - Pointer from from argument
 * \param[in]	pDstInfo - Point to dst_info_t to return
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCastDstInfo(uint16_t lineNumber, ARG_P *pArg, dst_info_t *pDstInfo)
{
	char string[OUTPUT_STRING_MAX];
	int16_t	status = STATUS_PASS;

	switch (pArg->argType)
	{
		case ARG_VAR:
		{
			switch (pArg->varType)
			{
				case VAR_DSTIT:
				{
					*pDstInfo = pArg->arg.data.value.dstinfoValue;
				}
				break;

				default:
				{
					sprintf(string,
							"argCastDstInfo(): Cannot cast [%s, %s] to [%s]",
							pArg->pName,
							argVarStringGet(pArg->varType),
							argVarStringGet(VAR_TVAL));
					OUTPUT_ERR(lineNumber, string, NULL, NULL);
					status = STATUS_FAIL;
				}
				break;
			}
		}
		break;

		case ARG_NAMED_CONST:
		case ARG_NUMERIC_CONST:
		case ARG_MACRO:
		case ARG_STRING_CONST:
		default:
		{
			sprintf(string,
					"argCastDstInfo(): Incompatible argument [%s] type for [%s]",
					argArgStringGet(pArg->argType),
					argVarStringGet(VAR_TVAL));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

	}

	return(status);
}

//=============================================================================
/**
 * \brief This function is used to see what type of expression comparison to do
 * 		  Only int and string comparisons are allowed
 *
 * \param[in]	ln - Line number in APIVSXML file
 * \param[in]	pArg - The argument that will decide the comparison
 * \param[in]	pMember - member name if any
 * \param[out]	comp - Flag for INT = TRUE, PCHAR = FALSE
 * \return	STATUS_PASS - Parse occurred correctly
 * 		STATUS_FAIL - Parse failed
 */
int16_t
argCompareType(uint16_t ln, ARG_P *pArg, char *pMember, boolean *comp)
{
	int16_t status = STATUS_PASS;
	boolean memberOK = FALSE;
	*comp = TRUE;

	switch(pArg->varType)
	{
		case VAR_VOID:
		{
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argCompareType(): Invalid variable type for [%s, %s]",
					pArg->pName,
					argVarStringGet(pArg->varType));
			OUTPUT_ERR(ln, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;

		case VAR_TVAL:
		{
			if (NULL != pMember)
			{
				memberOK = TRUE;
				if ((!strcmp(MEM_TV_SEC, pMember))
					|| (!strcmp(MEM_TV_USEC, pMember)))
				{
					*comp = TRUE;	// Show numeric comparison
				}
				else
				{
					// Invalid member specified
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"argCompareType(): Illegal member [%s] specified for variable [%s]",
							pMember,
							pArg->pName);
					OUTPUT_ERR(ln, string, NULL, NULL);
				}
			}
			else
			{
				// member not specified, must have member
				char string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argCompareType(): Member must be specified for variable [%s, %s]",
						pArg->pName,
						argVarStringGet(pArg->varType));
				OUTPUT_ERR(ln, string, NULL, NULL);
			}
		}
		break;

		case VAR_FINF:
		{
			if (NULL != pMember)
			{
				memberOK = TRUE;
				if ((!strcmp(MEM_FINF_IP, pMember))
					|| (!strcmp(MEM_FINF_LEAD, pMember))
					|| (!strcmp(MEM_FINF_TRAIL, pMember)))
				{
					*comp = TRUE;	// Show numeric comparison
				}
				else
				{
					// Invalid member specified
					char string[OUTPUT_STRING_MAX];
					sprintf(string,
						"argCompareType(): Illegal member [%s] specified for variable [%s]",
						pMember,
						pArg->pName);
					OUTPUT_ERR(ln, string, NULL, NULL);
				}
			}
			else
			{
				// member not specified, must have member
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argCompareType(): Member must be specified for variable [%s, %s]",
						pArg->pName,
						argVarStringGet(pArg->varType));
				OUTPUT_ERR(ln, string, NULL, NULL);
			}
		}
		break;

		case VAR_FFDSTAT:
		{
			if (NULL != pMember)
			{
				memberOK = TRUE;
				if ((!strcmp(MEM_FFDSTAT_COM, pMember))
					|| (!strcmp(MEM_FFDSTAT_SRX, pMember))
					|| (!strcmp(MEM_FFDSTAT_ERX, pMember)))
				{
					*comp = TRUE;	// Show numeric comparison
				}
				else
				{
					// Invalid member specified
					char string[OUTPUT_STRING_MAX];
					sprintf(string,
						"argCompareType(): Illegal member [%s] specified for variable [%s]",
						pMember,
						pArg->pName);
					OUTPUT_ERR(ln, string, NULL, NULL);
				}
			}
			else
			{
				// member not specified, must have member
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argCompareType(): Member must be specified for variable [%s, %s]",
						pArg->pName,
						argVarStringGet(pArg->varType));
				OUTPUT_ERR(ln, string, NULL, NULL);
			}
		}
		break;

		case VAR_FFINFO:
		{
			if (NULL != pMember)
			{
				memberOK = TRUE;
				if ((!strcmp(MEM_FFINFO_HZ, pMember))
					|| (!strcmp(MEM_FFINFO_SRX, pMember))
					|| (!strcmp(MEM_FFINFO_ERX, pMember))
					|| (!strcmp(MEM_FFINFO_E10, pMember))
					|| (!strcmp(MEM_FFINFO_SEQ, pMember)))
				{
					*comp = TRUE;	// Show numeric comparison
				}
				else
				{
					// Invalid member specified
					char string[OUTPUT_STRING_MAX];
					sprintf(string,
						"argCompareType(): Illegal member [%s] specified for variable [%s]",
						pMember,
						pArg->pName);
					OUTPUT_ERR(ln, string, NULL, NULL);
				}
			}
			else
			{
				// member not specified, must have member
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argCompareType(): Member must be specified for variable [%s, %s]",
						pArg->pName,
						argVarStringGet(pArg->varType));
				OUTPUT_ERR(ln, string, NULL, NULL);
			}
		}
		break;

		case VAR_FNOTI:
		{
			if (pMember != NULL) {
				memberOK = TRUE;
				if ((!strcmp(MEM_FNOTI_RXF, pMember))
					|| (!strcmp(MEM_FNOTI_STAT, pMember))
					|| (!strcmp(MEM_FNOTI_SEQ, pMember))
					|| (!strcmp(MEM_FNOTI_CNT, pMember))
					|| (!strcmp(MEM_FNOTI_FIOD, pMember))) {
					*comp = TRUE;	// Show numeric comparison
				} else {
					// Invalid member specified
					char string[OUTPUT_STRING_MAX];
					sprintf(string, "argCompareType(): Illegal member [%s] specified for variable [%s]",
						pMember,
						pArg->pName);
					OUTPUT_ERR(ln, string, NULL, NULL);
				}
			} else {
				// member not specified, must have member
				char	string[OUTPUT_STRING_MAX];
				sprintf(string, "argCompareType(): Member must be specified for variable [%s, %s]",
						pArg->pName,
						argVarStringGet(pArg->varType));
				OUTPUT_ERR(ln, string, NULL, NULL);
			}
		}
		break;

		case VAR_FCMAP:
		{
			if (NULL != pMember)
			{
				memberOK = TRUE;
				if ((!strcmp(MEM_FCMAP_OUT, pMember))
					|| (!strcmp(MEM_FCMAP_FIOD, pMember))
					|| (!strcmp(MEM_FCMAP_CHAN, pMember))
					|| (!strcmp(MEM_FCMAP_CLR, pMember)))
				{
					*comp = TRUE;	// Show numeric comparison
				}
				else
				{
					// Invalid member specified
					char string[OUTPUT_STRING_MAX];
					sprintf(string,
						"argCompareType(): Illegal member [%s] specified for variable [%s]",
						pMember,
						pArg->pName);
					OUTPUT_ERR(ln, string, NULL, NULL);
				}
			}
			else
			{
				// member not specified, must have member
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"argCompareType(): Member must be specified for variable [%s, %s]",
						pArg->pName,
						argVarStringGet(pArg->varType));
				OUTPUT_ERR(ln, string, NULL, NULL);
			}
		}
		break;

		case VAR_CHAR:
		case VAR_PCHAR:
		case VAR_UCHAR:
		case VAR_PUCHAR:
		{
			*comp = FALSE;	// Show string comparison
		}
		break;

		// Ensure each of these types can actually be cast to an int
		case VAR_BOOL:
		case VAR_SSIZET:
		case VAR_INT:
		case VAR_UINT:
		case VAR_FPUIH:
		case VAR_FPUIAUXH:
		case VAR_OFLAGS:
		case VAR_FAPPH:
		case VAR_FDEVH:
		case VAR_FVER:
		case VAR_FVIEW:
		case VAR_FCMASK:
		case VAR_FCFSA:
		case VAR_FNOTF:
		case VAR_FINT:
		case VAR_FTST:
		case VAR_FMFB:
		case VAR_FPORT:
		case VAR_FDEVT:
		case VAR_FFMST:
		case VAR_FVMST:
		{
			*comp = TRUE;	// Show int comparison
		}
		break;

		case VAR_FTBUF:
		case VAR_DSTIT:
		default:
		{
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"argCompareType(): Invalid variable type for [%s, %s]",
					pArg->pName,
					argVarStringGet(pArg->varType));
			OUTPUT_ERR(ln, string, NULL, NULL);
			status = STATUS_FAIL;
		}
		break;
	}

	if ((STATUS_PASS == status) && (pMember != NULL) && (memberOK == FALSE))
	{
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argCompareType(): Member [%s] not valid for variable [%s, %s]",
				pMember,
				pArg->pName,
				argVarStringGet(pArg->varType));
		OUTPUT_ERR(ln, string, NULL, NULL);
		status = STATUS_FAIL;
	}

	return(status);
}

//=============================================================================
/**
 * \brief       Convert a line of input to a buffer that we can initialize with
 *
 * \param[in]	pIn - Input buffer pointer
 * \param[out]	pOut - Output buffer pointer
 * 
 * \return		none
 */
static
void
argConvert (char *pIn, unsigned char **ppOut)
{
	char			nextChar; 
	int				conversionValue;
	unsigned char	*pOut = *ppOut;

	while ((nextChar = *pIn++))
	{	
		if ((nextChar == '\n') || (nextChar == '\r'))
		{
			continue;
		}

		// Hex Char 0xNN
		if(   (nextChar == '0') 
		   && (   (*pIn == 'x')
		   	   || (*pIn == 'X')))
		{				
			sscanf(pIn+1, "%2x", &conversionValue);
			*pOut++ = (unsigned char) (conversionValue & 0xFF);   
			pIn += 4;
		}
		else 
		{       // ordinary character 
			*pOut++ = nextChar;
		}
	}

	*ppOut = pOut;
}

//=============================================================================
/**
 * \brief Read a human-readble file and convert to data to initialize structure
 * 		  The caller must free() *ppBuf when done.
 *
 * \param[in]	ln - Line number in XML file
 * \param[in]	pPath - Path to file
 * \param[in]	pFileName - A file name
 * \param[out]	ppBuf - Buffer that has data to initialize from
 * \param[out]	pSize - Bytes in *ppBuf
 * 
 * \return	STATUS_FAIL = Operation failed
 *              STATUS_SUCCESS = Operation succeeded
 */
int16_t
argSet(uint16_t ln, const char *pPath, const char *pFileName,
		unsigned char **ppBuf,
		unsigned int *pSize)
{
	FILE *pFile;
	char line[OUTPUT_STRING_MAX];
	unsigned char *pOut;

	if (NULL == (pOut = malloc(ARG_CONVERTED_MAX)))
	{
		// Malloc failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argSet(): Could not malloc [%d] for conversion buffer",
				ARG_CONVERTED_MAX);
		OUTPUT_ERR(ln, string, strerror(errno), NULL);
	}
	*ppBuf = pOut;

	sprintf(line, "%s%s", pPath, pFileName);

	if( (pFile = fopen(line, "r" )) == NULL)
	{
		// Open failed
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"argSet(): Failed to open [%s]",
				line);
		OUTPUT_ERR(ln, string, strerror(errno), NULL);
		free(*ppBuf);
		return(STATUS_FAIL);
	}

	while (fgets(line, OUTPUT_STRING_MAX, pFile) != NULL)
	{	
		if( line[0] != '#')   // only process data lines, not comments
		{
			argConvert(line, &pOut);
		}
	}
	
	fclose(pFile);

	*pSize = pOut - *ppBuf;

	return(STATUS_PASS);
}
