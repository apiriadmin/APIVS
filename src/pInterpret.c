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
 * \file pInterpret.c
 *
 * This file contains the definition of functions for interpreting the APIVSXML
 * pCode
 *
 * \brief pInterpret XML processing
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
#include	<signal.h>		// Signal Definitions
#include	<sys/time.h>
#include	<sys/resource.h>
#include	<vt100.h>		// VT100 Definitions
#include	"vse_stnd.h"	// STD VSE Definitions
#include	"configFile.h"	// Configuration File Definitions
#include	"outputXML.h"	// Output XML Processor Definitions
#include	"argument.h"	// Argument Processor Definitions
#include	"pCode.h"		// pCode Processor Definitions
#include	"pInterpret.h"	// pInterpret  Processor Definitions
#include	<vt100.h>		// VT100 Definitions
#include	<emfio.h>		// EMFIO Definitions

//=============================================================================
/**
 * Extern
 */

/** \brief Forward reference for P_CODE interpreter */
extern	int16_t	interpretPcode(P_CODE *pCode);

//=============================================================================
/**
 * Local Defines
 */

/** \brief Static table mapping status to string */
static	char	*s_status_string[] = 
{
	VALUE_FAIL,
	VALUE_PASS
};

/** \brief Static table mappings for dump type */
static char		*s_dumpType_string[] =
{
	D_UNKNOWN,
	D_VD,
	D_CMDMSG
};

/** \brief Static table of operations for print */
static	char	*s_opType_string[] =
{
	OP_S_UNKNOWN,
	OP_S_EQ,
	OP_S_ADD,
	OP_S_SUB,
};

#define	VALIDATE_MODE	(TRUE)
#define	CAPTURE_MODE	(FALSE)
/** \brief Validation or capture mode TRUE = Validate, FALSE = Capture */
static	boolean	s_validateCapture = VALIDATE_MODE;

/** \brief Table of pCode to execute for a given signal */
static	P_CODE		*s_signalPcode[SIGNAL_MAX] = { 0 };

//=============================================================================
/**
 * \brief This function is used to set capture mode
 *
 * \return
 */
void
interpretSetCapture()
{
	s_validateCapture = CAPTURE_MODE;
}

//=============================================================================
/**
 * \brief This function interprets <SignalHandler> Code
 *
 * \param[in]	signal - The signal that invoked the handler
 *
 */
static
void
interpretSigHandler(int signal)
{
	// Received a signal, now execute pCode if present for signal
	if (NULL != s_signalPcode[signal])
	{
		(void)interpretPcode(s_signalPcode[signal]);
	}
	else
	{
		// Do nothing
	}
}

//=============================================================================
/**
 * \brief This function interprets <Print> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretPrint(P_CODE *pCode)
{
	// Complete Print format processing
	outputXmlAttrAddCommon(pCode->code.cPrint.level, &pCode->common, A_NAME);
	outputXmlTagOpen(pCode->code.cPrint.level, P_PRINT, outputXmlAttrGet());
	outputXmlText(pCode->code.cPrint.level, pCode->code.cPrint.pText);
	//outputXmlNewLine(pCode->code.cPrint.level);
	outputXmlTagClose(pCode->code.cPrint.level, P_PRINT);

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function interprets <Dump> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretDump(P_CODE *pCode)
{
	int16_t	status = STATUS_PASS;

	// Complete DUMP format processing
	if (   outputXmlShouldPrint(pCode->code.cDump.level)
		&& (   (pCode->code.cDump.force == TRUE)
			|| (s_validateCapture == CAPTURE_MODE)))
	{
		outputXmlAttrAddCommon(pCode->code.cDump.level, &pCode->common, A_FILE);
		outputXmlAttrAdd(A_DUMP, s_dumpType_string[pCode->code.cDump.type]);
		if (pCode->code.cDump.force)
		{
			outputXmlAttrAdd(A_FORCE, NC_TRUE);
		}
		outputXmlTagOpen(pCode->code.cDump.level, P_DUMP, outputXmlAttrGet());

		switch (pCode->code.cDump.type)
		{
			case DUMP_VD:
			{
				char	file[OUTPUT_STRING_MAX];
				int		sequence = 0;

				sprintf(file,
						"%s%s",
						configFileGetFPUIDFP(),
						pCode->code.cDump.pFile->arg.data.value.pCharValue);
				if (pCode->code.cDump.pSeq)
				{
					if (STATUS_FAIL ==
							(status = argCastInt(pCode->common.lineNumber,
												 pCode->code.cDump.pSeq,
												 &sequence)))
					{
						break;
					}
				}
				if (vt100_dumpVD(file,
								 (uint16_t)sequence,
								 pCode->common.pDesc))
				{
					OUTPUT_ERR(pCode->common.lineNumber,
							   "interpretDump(): Could not dump VD, error [%s]",
							   vt100_get_errorText(),
							   NULL);
					status = STATUS_FAIL;
				}
				else
				{
					outputXmlShowVD(pCode->common.pDesc);
				}
			}
			break;

			case DUMP_CMDMSG:
			{
				char	file[OUTPUT_STRING_MAX];
				int		frame;

				sprintf(file,
						"%s%s",
						configFileGetFIODFP(),
						pCode->code.cDump.pFile->arg.data.value.pCharValue);
				if (STATUS_FAIL ==
							(status = argCastInt(pCode->common.lineNumber,
												 pCode->code.cDump.pFrame,
												 &frame)))
				{
					break;
				}
				if (emfio_dumpCommand(frame, file))
				{
					// Dump failed
					char	string[OUTPUT_STRING_MAX];
					sprintf(string,
							"interpretDump(): Could not dump CMD [%d]",
							frame);
					OUTPUT_ERR(pCode->common.lineNumber,
							   string,
							   emfio_getErrorText(),
							   NULL);
					status = STATUS_FAIL;
				}
				else
				{
					outputXmlShowCMD(frame);
				}
			}
			break;

			default:
			{
				// Unknown dump type
				char	string[OUTPUT_STRING_MAX];
				sprintf(string,
						"interpretDump(): Unknown dump type [%d] requested",
						pCode->code.cDump.type);
				OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
				status = STATUS_FAIL;
			}
			break;
		}

		outputXmlTagClose(pCode->code.cDump.level, P_DUMP);
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <Load> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretLoad(P_CODE *pCode)
{
	int16_t	status = STATUS_PASS;
	char	frame[OUTPUT_STRING_MAX];

	if (pCode->code.cLoad.pFrame)
	{
		sprintf(frame, "%d", pCode->code.cLoad.pFrame->arg.data.value.intValue);
	}

	// Complete LOAD format processing
	outputXmlAttrAddCommon(LEVEL_SUMMARY, &pCode->common, A_FILE);
	outputXmlAttrAdd(A_LOAD, s_dumpType_string[pCode->code.cLoad.type]);
	if (pCode->code.cLoad.pFrame)
	{
		outputXmlAttrAdd(A_FRAME, frame);
	}
	outputXmlTagOpen(LEVEL_SUMMARY, P_LOAD, outputXmlAttrGet());

	switch (pCode->code.cLoad.type)
	{
		case DUMP_VD:
		{
			char	file[OUTPUT_STRING_MAX];
			sprintf(file,
					"%s%s",
					configFileGetFPUIDFP(),
					pCode->code.cLoad.pFile->arg.data.value.pCharValue);
			if (vt100_fromDumpToVD(file))
			{
				OUTPUT_ERR(pCode->common.lineNumber,
						   "interpretLoad(): Could not load VD",
						   vt100_get_errorText(),
						   NULL);
				status = STATUS_FAIL;
			}
		}
		break;

		case DUMP_CMDMSG:
		{
			unsigned char	*pBuf;
			unsigned int	size;

			if (STATUS_FAIL == argSet(pCode->common.lineNumber,
									  configFileGetFIODFP(),
									  pCode->code.cLoad.pFile->arg.data.value.pCharValue,
									  &pBuf,
									  &size))
			{
				status = STATUS_FAIL;
				break;
			}

			if (emfio_loadCommand(
						pCode->code.cLoad.pFrame->arg.data.value.intValue,
						pBuf,
						(uint32_t)size))
			{
				OUTPUT_ERR(pCode->common.lineNumber,
						   "interpretLoad(): Could not load CMD",
						   emfio_getErrorText(),
						   NULL);
				status = STATUS_FAIL;
				free(pBuf);
			}
		}
		break;

		default:
		{
			OUTPUT_ERR(pCode->common.lineNumber,
					   "interpretLoad(): Unknown load type requested",
					   NULL,
					   NULL);
			status = STATUS_FAIL;
		}
		break;
	}

	outputXmlTagClose(LEVEL_SUMMARY, P_LOAD);

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <FPUIInput> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretFPUII(P_CODE *pCode)
{
	int16_t	status = STATUS_PASS;
	char	file[OUTPUT_STRING_MAX];
	sprintf(file,
			"%s%s",
			configFileGetFPUIIFP(),
			pCode->code.cFPUII.pFile->arg.data.value.pCharValue);

	// Complete FPUII format processing
	outputXmlAttrAddCommon(LEVEL_TRACE,  &pCode->common, A_VAR);
	outputXmlAttrAdd(A_FILE, file);
	outputXmlTagCurrent(LEVEL_TRACE, P_FPUII, outputXmlAttrGet());

	if (vt100_fileToVD(file))
	{
		OUTPUT_ERR(pCode->common.lineNumber,
				   "interpretFPUII(): Could not input VD",
				   vt100_get_errorText(),
				   NULL);
		status = STATUS_FAIL;
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <FIOReponse> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretFIOR(P_CODE *pCode)
{
	char            file[OUTPUT_STRING_MAX];
	char            frame[OUTPUT_STRING_MAX];
	char            delaystr[OUTPUT_STRING_MAX];
	unsigned char   *pBuf;
	unsigned int    size;
	int		delay = 0;

	sprintf(file, "%s%s", configFileGetFIORFP(),
		pCode->code.cFIOR.pFile->arg.data.value.pCharValue);
	sprintf(frame, "%d", pCode->code.cFIOR.pFrame->arg.data.value.intValue);
	if (pCode->code.cFIOR.pDelay) {
		delay = pCode->code.cFIOR.pDelay->arg.data.value.intValue;
		sprintf(delaystr, "%d", delay);
	}

	// Complete FIOR format processing
	outputXmlAttrAddCommon(LEVEL_TRACE, &pCode->common, A_VAR);
	outputXmlAttrAdd(A_FILE, file);
	outputXmlAttrAdd(A_FRAME, frame);
	if (pCode->code.cFIOR.pDelay) {
		outputXmlAttrAdd(A_DELAY, delaystr);	
	}
	outputXmlTagCurrent(LEVEL_TRACE, P_FIOR, outputXmlAttrGet());

	if (STATUS_FAIL == argSet(pCode->common.lineNumber,
					configFileGetFIORFP(),
					pCode->code.cFIOR.pFile->arg.data.value.pCharValue,
					&pBuf,
					&size))
	{
		return(STATUS_FAIL);
	}

	if (emfio_setResponse(pCode->code.cFIOR.pFrame->arg.data.value.intValue,
				pBuf, (uint32_t)size, (uint32_t)delay) != 0)
	{
		// Could not load response
		char string[OUTPUT_STRING_MAX];
		sprintf(string, "interpretFIOR(): Could not load response frame [%s]",
			frame);
		OUTPUT_ERR(pCode->common.lineNumber, string, emfio_getErrorText(), NULL);
		free(pBuf);
		return(STATUS_FAIL);
	}

	free(pBuf);
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function interprets <Format> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretFormat(P_CODE *pCode)
{
	int16_t	status = STATUS_PASS;

	// Complete DUMP format processing
	if (outputXmlShouldPrint(pCode->code.cFormat.level))
	{
		outputXmlAttrAddCommon(pCode->code.cFormat.level, &pCode->common, A_VAR);
		outputXmlTagOpen(pCode->code.cFormat.level, P_FORMAT, outputXmlAttrGet());

		if (pCode->code.cFormat.pArg)
		{
			// Format a single variable
			status = argFormat(pCode->common.lineNumber,
							   pCode->code.cFormat.level,
							   pCode->code.cFormat.pArg);
		}
		else
		{
			// Format ALL variables
			ARG_NODE	*pNode = argVarTableGet();

			outputXmlText(pCode->code.cFormat.level, "ALL VARIABLES");
			outputXmlNewLine(pCode->code.cFormat.level);

			// Step through variable list
			while ((NULL != pNode) && (STATUS_PASS == status))
			{
				status = argFormat(pCode->common.lineNumber,
								   pCode->code.cFormat.level,
								   &pNode->argCode);
				pNode = pNode->pNode;
			}
		}

		//outputXmlNewLine(pCode->code.cFormat.level);
		outputXmlTagClose(pCode->code.cFormat.level, P_FORMAT);
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <SetUp> Code
 *
 * \param[in]	pSU - Start of SetUp to interpret, NULL if No SetUp
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretSetUp(P_SU *pSU)
{
	int16_t	status;

	if (NULL == pSU)
	{
		return(STATUS_PASS);
	}

	outputXmlAttrAddCommon(LEVEL_TRACE, &pSU->common, A_NAME);
	outputXmlTagOpen(LEVEL_TRACE, P_SETUP, outputXmlAttrGet());

	status = interpretPcode(pSU->pCode);

	outputXmlTagClose(LEVEL_TRACE, P_SETUP);

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <TearDown> Code
 *
 * \param[in]	pSU - Start of TearDown to interpret, NULL if No TearDown
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretTearDown(P_TD *pTD)
{
	int16_t	status;

	if (NULL == pTD)
	{
		return(STATUS_PASS);
	}

	outputXmlAttrAddCommon(LEVEL_TRACE, &pTD->common, A_NAME);
	outputXmlTagOpen(LEVEL_TRACE, P_TEARDOWN, outputXmlAttrGet());

	status = interpretPcode(pTD->pCode);

	outputXmlTagClose(LEVEL_TRACE, P_TEARDOWN);

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <Subroutine> Code
 *
 * \param[in]	pSU - Start of Subroutine to interpret, NULL if No Subroutine
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretSubroutine(P_SUB *pSUB)
{
	int16_t	status;

	if (NULL == pSUB)
	{
		return(STATUS_PASS);
	}

	outputXmlAttrAddCommon(LEVEL_TRACE, &pSUB->common, A_NAME);
	outputXmlTagOpen(LEVEL_TRACE, P_SUBR, outputXmlAttrGet());

	status = interpretPcode(pSUB->pCode);

	outputXmlTagClose(LEVEL_TRACE, P_SUBR);

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <Call> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretCall(P_CODE *pCode)
{
	int16_t	status;

	outputXmlAttrAddCommon(LEVEL_SUMMARY, &pCode->common, A_REF);
	outputXmlTagOpen(LEVEL_SUMMARY, P_CALL, outputXmlAttrGet());

	if (STATUS_PASS == (status = interpretSetUp(pCode->code.cCall.pSU)))
	{
		if (STATUS_PASS ==
			(status = interpretSubroutine(pCode->code.cCall.pSUB)))
		{
			status = interpretTearDown(pCode->code.cCall.pTD);
		}
	}

	outputXmlTagClose(LEVEL_SUMMARY, P_CALL);

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <Signal> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretSignal(P_CODE *pCode)
{
	int		signalNum;
	P_CODE	*pCodeSH;

	signalNum = pCode->code.cSig.pSignal->arg.data.value.intValue;

	outputXmlAttrAddCommon(LEVEL_SUMMARY, &pCode->common, A_REF);
	outputXmlAttrAdd(A_SIG, pCode->code.cSig.pSignal->pName);
	if (pCode->code.cSig.enable)
	{
		outputXmlAttrAdd(A_ACTION, VALUE_ENABLE);
	}
	else
	{
		outputXmlAttrAdd(A_ACTION, VALUE_DISABLE);
	}
	outputXmlTagCurrent(LEVEL_SUMMARY, P_SIG, outputXmlAttrGet());

	if ((signalNum < 0) || (signalNum > SIGNAL_MAX))
	{
		// Illegal signal, just to be safe
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"interpretSignal(): Illegal signal [%d]",
				signalNum);
		OUTPUT_ERR(pCode->common.lineNumber, string, strerror(errno), NULL);
		return(STATUS_FAIL);
	}

	if (pCode->code.cSig.enable)
	{
		pCodeSH = pCode->code.cSig.pSH->pCode;
		if (NULL == pCodeSH)
		{
			// Illegal pCode, just to be safe
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"interpretSignal(): Illegal pCode for signal [%d]",
					signalNum);
			OUTPUT_ERR(pCode->common.lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}

		s_signalPcode[signalNum] = pCodeSH;
		if (SIG_ERR == signal(signalNum, interpretSigHandler))
		{
			// Signal Handler error
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"interpretSignal(): Error setting up signal handler for signal [%d]",
					signalNum);
			OUTPUT_ERR(pCode->common.lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
	}
	else
	{
		s_signalPcode[signalNum] = NULL;
		if (SIG_ERR == signal(signalNum, SIG_IGN))
		{
			// Signal Handler error
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"interpretSignal(): Error ignoring signal [%d]",
					signalNum);
			OUTPUT_ERR(pCode->common.lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function interprets <Aux> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretAux(P_CODE *pCode)
{
	outputXmlAttrAddCommon(LEVEL_SUMMARY, &pCode->common, A_NAME);
	if (pCode->code.cAux.flip)
	{
		outputXmlAttrAdd(A_FLIP, NC_ON);
	}
	else
	{
		outputXmlAttrAdd(A_FLIP, NC_OFF);
	}
	outputXmlTagCurrent(LEVEL_SUMMARY, P_AUX, outputXmlAttrGet());

	if (vt100_set_auxSwitch(pCode->code.cAux.flip ? NC_ON_VALUE : NC_OFF_VALUE))
	{
		// Aux switch set failed
		OUTPUT_ERR(pCode->common.lineNumber,
				   "interpretAux(): Setting of Aux Switch Failed",
				   vt100_get_errorText(),
				   NULL);
		return(STATUS_FAIL);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function interprets <If> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretIf(P_CODE *pCode)
{
	int16_t	status;
	boolean	compareResult;


	if (STATUS_PASS ==
		(status = exprEvaluate(pCode->common.lineNumber,
							   &pCode->code.cIf.expr,
							   &compareResult)))
	{
		if (compareResult)
		{
			outputXmlAttrAddCommon(LEVEL_TRACE, &pCode->common, A_NAME);
			outputXmlAttrAdd(A_EXPR, pCode->code.cIf.expr.pExpr);
			outputXmlAttrAdd(A_EVAL, VALUE_TRUE);
			outputXmlTagOpen(LEVEL_TRACE, P_IF, outputXmlAttrGet());
			status = interpretPcode(pCode->code.cIf.pThen);
			outputXmlTagClose(LEVEL_TRACE, P_IF);
		}
		else
		{
			outputXmlAttrAddCommon(LEVEL_TRACE, &pCode->common, A_NAME);
			outputXmlAttrAdd(A_EXPR, pCode->code.cIf.expr.pExpr);
			outputXmlAttrAdd(A_EVAL, VALUE_FALSE);
			outputXmlTagOpen(LEVEL_TRACE, P_IF, outputXmlAttrGet());
			status = interpretPcode(pCode->code.cIf.pElse);
			outputXmlTagClose(LEVEL_TRACE, P_IF);
		}
	}
	
	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <While> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretWhile(P_CODE *pCode)
{
	int16_t	status;
	boolean	compareResult;


	status = exprEvaluate(pCode->common.lineNumber,
						  &pCode->code.cWhile.expr,
						  &compareResult);
	while (compareResult && (STATUS_PASS == status))
	{
		outputXmlAttrAddCommon(LEVEL_TRACE, &pCode->common, A_NAME);
		outputXmlAttrAdd(A_EXPR, pCode->code.cWhile.expr.pExpr);
		outputXmlAttrAdd(A_EVAL, VALUE_TRUE);
		outputXmlTagOpen(LEVEL_TRACE, P_WHILE, outputXmlAttrGet());
		status = interpretPcode(pCode->code.cWhile.pWhile);
		outputXmlTagClose(LEVEL_TRACE, P_WHILE);
		if (STATUS_PASS == status)
		{
			status = exprEvaluate(pCode->common.lineNumber,
								  &pCode->code.cWhile.expr,
								  &compareResult);
		}
	}
	
	if (STATUS_PASS == status)
	{
		outputXmlAttrAddCommon(LEVEL_TRACE, &pCode->common, A_NAME);
		outputXmlAttrAdd(A_EXPR, pCode->code.cWhile.expr.pExpr);
		outputXmlAttrAdd(A_EVAL, VALUE_FALSE);
		outputXmlTagCurrent(LEVEL_TRACE, P_WHILE, outputXmlAttrGet());
	}
	
	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <Abort> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretAbort(P_CODE *pCode)
{
	int16_t	status;

	outputXmlAttrAddCommon(LEVEL_SUMMARY, &pCode->common, A_NAME);
	outputXmlAttrAdd(A_STATUS, s_status_string[pCode->code.cAbort.status + 1]);
	outputXmlTagCurrent(LEVEL_SUMMARY, P_ABORT, outputXmlAttrGet());

	status = pCode->code.cAbort.status;

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets <Set> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretSet(P_CODE *pCode)
{
	// Set the Variable referenced

	outputXmlAttrAddCommon(LEVEL_TRACE, &pCode->common, A_VAR);
	outputXmlAttrAdd(A_OP, s_opType_string[pCode->code.cSet.operation]);
	outputXmlAttrAdd(A_MEMBER, pCode->code.cSet.pMember);
	if (pCode->code.cSet.pIndex)
		outputXmlAttrAdd(A_INDEX, pCode->code.cSet.pIndex->pName);
	if (pCode->code.cSet.pVal)
	{
		outputXmlAttrAdd(A_VALUE, pCode->code.cSet.pVal->pName);
	}
	else
	{
		outputXmlAttrAdd(A_FILE, pCode->code.cSet.pFile->pName);
	}
	outputXmlTagCurrent(LEVEL_TRACE, P_SET, outputXmlAttrGet());

	return(argSetVar(pCode->common.lineNumber,
					 pCode->code.cSet.operation,
					 pCode->code.cSet.pVal,
					 pCode->code.cSet.pFile,
					 pCode->code.cSet.pVar,
					 pCode->code.cSet.pIndex,
					 pCode->code.cSet.pMember));
}

//=============================================================================
/**
 * \brief This function interprets <Sleep> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretSleep(P_CODE *pCode)
{
	outputXmlAttrAddCommon(LEVEL_SUMMARY, &pCode->common, A_TIME);
	outputXmlTagCurrent(LEVEL_SUMMARY, P_SLEEP, outputXmlAttrGet());

	sleep(pCode->code.cSleep.pArg->arg.data.value.intValue);

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function interprets <Function> Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretFunction(P_CODE *pCode)
{
	int16_t	status;

	outputXmlAttrAddCommon(LEVEL_SUMMARY, &pCode->common, A_FNAME);
	if (NULL != pCode->code.cFunc.returnValue)
	{
		outputXmlAttrAdd(A_RETURN, pCode->code.cFunc.returnValue->pName);
	}
	if (NULL != pCode->code.cFunc.errnoValue)
	{
		outputXmlAttrAdd(A_ERRNO, pCode->code.cFunc.errnoValue->pName);
	}
	if (NULL != pCode->code.cFunc.arg[0])
	{
		outputXmlAttrAdd(A_P1, pCode->code.cFunc.arg[0]->pName);
	}
	if (NULL != pCode->code.cFunc.arg[1])
	{
		outputXmlAttrAdd(A_P2, pCode->code.cFunc.arg[1]->pName);
	}
	if (NULL != pCode->code.cFunc.arg[2])
	{
		outputXmlAttrAdd(A_P3, pCode->code.cFunc.arg[2]->pName);
	}
	if (NULL != pCode->code.cFunc.arg[3])
	{
		outputXmlAttrAdd(A_P4, pCode->code.cFunc.arg[3]->pName);
	}
	if (NULL != pCode->code.cFunc.arg[4])
	{
		outputXmlAttrAdd(A_P5, pCode->code.cFunc.arg[4]->pName);
	}
	if (NULL != pCode->code.cFunc.arg[5])
	{
		outputXmlAttrAdd(A_P6, pCode->code.cFunc.arg[5]->pName);
	}
	if (NULL != pCode->code.cFunc.arg[6])
	{
		outputXmlAttrAdd(A_P7, pCode->code.cFunc.arg[6]->pName);
	}
	outputXmlTagCurrent(LEVEL_SUMMARY, P_FUNCTION, outputXmlAttrGet());

	// See if there is a function defined
	if (NULL != pCode->code.cFunc.pFunc->pExe)
	{
		status = (*pCode->code.cFunc.pFunc->pExe)(pCode->common.lineNumber,
												  &pCode->code.cFunc);
	}
	else
	{
		// No function defined
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"interpretFunction(): No function defined for [%s]",
				pCode->code.cFunc.pFunc->pName);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		status = STATUS_FAIL;
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets APIVSXML Code
 *
 * \param[in]	pCode - Start of code to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int16_t
interpretPcode(P_CODE *pCode)
{
	int16_t	status = STATUS_PASS;
	boolean	done = FALSE;

	// Execute the pCode
	while (NULL != pCode)
	{
		// Execute current statment
		switch (pCode->type)
		{
			case PC_E_PRINT:
			{
				status = interpretPrint(pCode);
			}
			break;

			case PC_E_SET:
			{
				status = interpretSet(pCode);
			}
			break;

			case PC_E_FUNC:
			{
				status = interpretFunction(pCode);
			}
			break;

			case PC_E_IF:
			{
				status = interpretIf(pCode);
			}
			break;

			case PC_E_WHILE:
			{
				status = interpretWhile(pCode);
			}
			break;

			case PC_E_THEN:
			{
				outputXmlAttrAddCommon(LEVEL_TRACE, &pCode->common, A_NAME);
				outputXmlTagOpen(LEVEL_TRACE, P_THEN, outputXmlAttrGet());

				status = interpretPcode(pCode->pCode);

				outputXmlTagClose(LEVEL_TRACE, P_THEN);

				// Note done with this tree branch
				done = TRUE;
			}
			break;

			case PC_E_ELSE:
			{
				outputXmlAttrAddCommon(LEVEL_TRACE, &pCode->common, A_NAME);
				outputXmlTagOpen(LEVEL_TRACE, P_ELSE, outputXmlAttrGet());

				status = interpretPcode(pCode->pCode);

				outputXmlTagClose(LEVEL_TRACE, P_ELSE);

				// Note done with this tree branch
				done = TRUE;
			}
			break;

			case PC_E_CALL:
			{
				status = interpretCall(pCode);
			}
			break;

			case PC_E_SIG:
			{
				status = interpretSignal(pCode);
			}
			break;

			case PC_E_AUX:
			{
				status = interpretAux(pCode);
			}
			break;

			case PC_E_ABORT:
			{
				status = interpretAbort(pCode);
			}
			break;

			case PC_E_DUMP:
			{
				status = interpretDump(pCode);
			}
			break;

			case PC_E_LOAD:
			{
				status = interpretLoad(pCode);
			}
			break;

			case PC_E_FORMAT:
			{
				status = interpretFormat(pCode);
			}
			break;

			case PC_E_SLEEP:
			{
				status = interpretSleep(pCode);
			}
			break;

			case PC_E_FPUII:
			{
				status = interpretFPUII(pCode);
			}
			break;

			case PC_E_FIOR:
			{
				status = interpretFIOR(pCode);
			}
			break;

			default:
			{
				char	string[OUTPUT_STRING_MAX];

				sprintf(string,
						"Unknown P_CODE type[%d]",
						pCode->type);
				OUTPUT_ERR(pCode->common.lineNumber,
			   			   string,
						   NULL,
						   pCode->common.pDesc);
				status = STATUS_FAIL;
			}
			break;
		}

		if ( done || (/*(pCode->type == PC_E_ABORT) &&*/ (STATUS_FAIL == status)) )
		{
			break;		// We failed or we are done
		}

		// Move to next P_CODE
		pCode = pCode->pCode;
	}

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets APIVSXML for the passed Test Case
 *
 * \param[in]	pTC - A Specific Test Case to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
interpretTC(P_TC *pTC)
{
	int16_t	status;

	// Interpret Test Case
	
	outputXmlAttrAddCommon(LEVEL_SUMMARY, &pTC->common, A_NAME);
	outputXmlTagOpen(LEVEL_SUMMARY, P_TESTCASE, outputXmlAttrGet());

	if (STATUS_PASS == (status = interpretSetUp(pTC->pSU)))
	{
		if (STATUS_PASS == (status = interpretPcode(pTC->pCode)))
		{
			status = interpretTearDown(pTC->pTD);
		}
	}

	outputXmlTagClose(LEVEL_SUMMARY, P_TESTCASE);

	return(status);
}

//=============================================================================
/**
 * \brief This function interprets APIVSXML for the passed Test Suite
 *
 * \param[in]	pTS - A Specific Test Suite to interpret
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int16_t
interpretTS(P_TS *pTS)
{
	char msg[OUTPUT_STRING_MAX];
	int16_t	status;
	
	P_TC	*pTC;

	// Interpret Test Suite
	
	// Start the Output XML Generator
	outputXmlAttrAddCommon(LEVEL_SUMMARY, &pTS->common, A_NAME);
	outputXmlTagOpen(LEVEL_SUMMARY, P_TESTSUITE, outputXmlAttrGet());

	if (strcmp(configFileGetFPUILBDEV(), "NULL")
		&& (0 != vt100_start(configFileGetFPUILBDEV(),
					configFileGetSH(),
					configFileGetSW())))
	{
		// vt100_start failed
		sprintf(msg,
				"interpretTS(): VT100 Virtual Display error starting device [%s]",
				configFileGetFPUILBDEV());
		OUTPUT_ERR(pTS->common.lineNumber,
				   msg,
				   vt100_get_errorText(),
				   NULL);
		return(STATUS_FAIL);
	}

	if (strcmp(configFileGetFIOLBDEV(), "NULL")
		&& (EMFIO_OK != emfio_start(configFileGetFIOLBDEV())))
	{
		// emfio_start failed
		sprintf(msg,
				"interpretTS(): FIO Emulator start error starting device [%s]",
				configFileGetFIOLBDEV());
		OUTPUT_ERR(pTS->common.lineNumber,
				   msg,
				   emfio_getErrorText(),
				   NULL);
		return(STATUS_FAIL);
	}

	// Lower relative priority after thread startup
	errno = 0;
	int prio = getpriority(PRIO_PROCESS, 0);
	if (errno == 0) {
		setpriority(PRIO_PROCESS, 0, prio+1);
	}
	
	if (STATUS_PASS == (status = interpretSetUp(pTS->pSU)))
	{
		pTC = pTS->pTC;			// First TC
		while (NULL != pTC)
		{
			outputXmlNewLine(LEVEL_SUMMARY);

			status = interpretTC(pTC);

			outputXmlNewLine(LEVEL_SUMMARY);

			if (STATUS_FAIL == status)
			{
				break;
			}

			pTC = pTC->pTC;		// Next Test Case
		}

		if (STATUS_PASS == status)
		{
			status = interpretTearDown(pTS->pTD);
		}
	}

	if (strcmp(configFileGetFPUILBDEV(), "NULL") && configFileGetFPUILBDEV())
	{
		vt100_end();
	}

	if (strcmp(configFileGetFIOLBDEV(), "NULL") && configFileGetFIOLBDEV())
	{
		emfio_end();
	}

	// Output final closing element tag
	outputXmlTagClose(LEVEL_SUMMARY, P_TESTSUITE);

	return(status);
}
