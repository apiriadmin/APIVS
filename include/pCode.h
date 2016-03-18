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
 * \file pCode.h
 *
 * This file contains the definition of the \c #pCode generator
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief pCode generator
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef	PCODE_H
#define	PCODE_H

//=============================================================================
/**
 * Includes
 */

/** \brief Forward references */
typedef struct pCode P_CODE;

#include	"function.h"		// Function definitions
#include	"expression.h"		// Expression definitions

//=============================================================================
/**
 * Defines
 */
/** \brief Elements */
/** \brief Definition of top APIVSXML element tag */
#define	P_APIVSXML	"APIVSXML"

/** \brief Definition of the TestSuite element tag */
#define P_TESTSUITE	"TestSuite"

/** \brief Definition of the TestCase element tag */
#define	P_TESTCASE	"TestCase"

/** \brief Definition of the TestCaseRef element tag */
#define	P_TESTCASEREF	"TestCaseRef"

/** \brief Definition of the Print element tag */
#define	P_PRINT		"Print"

/** \brief Definition of the file Dump element tag */
#define	P_DUMP		"Dump"

/** \brief Definition of the VD Load element tag */
#define	P_LOAD		"Load"

/** \brief Definition of the file Format element tag */
#define	P_FORMAT	"Format"

/** \brief Definition of the SetUp element tag */
#define	P_SETUP		"SetUp"

/** \brief Definition of the TearDown element tag */
#define	P_TEARDOWN	"TearDown"

/** \brief Definition of the Define element tag */
#define	P_DEFINE	"Define"

/** \brief Definition of the Set element tag */
#define	P_SET		"Set"

/** \brief Definition of the Function element tag */
#define	P_FUNCTION	"Function"

/** \brief Definition of the If element tag */
#define	P_IF		"If"

/** \brief Definition of the While element tag */
#define	P_WHILE		"While"

/** \brief Definition of the Then element tag */
#define	P_THEN		"Then"

/** \brief Definition of the Else element tag */
#define	P_ELSE		"Else"

/** \brief Definition of the Subroutine element tag */
#define	P_SUBR		"Subroutine"

/** \brief Definition of the Call element tag */
#define	P_CALL		"Call"

/** \brief Definition of the Signal Handler  element tag */
#define	P_SH		"SignalHandler"

/** \brief Definition of the Signal element tag */
#define	P_SIG		"Signal"

/** \brief Definition of the Abort element tag */
#define	P_ABORT		"Abort"

/** \brief Definition of the Sleep element tag */
#define	P_SLEEP		"Sleep"

/** \brief Definition of the FPUIInput element tag */
#define	P_FPUII		"FPUIInput"

/** \brief Definition of the FIOResponse element tag */
#define	P_FIOR		"FIOResponse"

/** \brief Definition of the Aux element tag */
#define	P_AUX		"Aux"

/** \brief Attrbutes */
#define	A_NAME		"name"
#define	A_REF		"ref"
#define	A_SETUP		"setUp"
#define	A_TEARDOWN	"tearDown"
#define	A_DESC		"description"
#define	A_LEVEL		"level"
#define	A_VAR		"var"
#define	A_VALUE		"value"
#define	A_OP		"operation"
#define	OP_S_UNKNOWN	"unknown"
#define	OP_S_EQ		"equal"
#define	OP_S_ADD	"add"
#define	OP_S_SUB	"subtract"
#define	A_TYPE		"type"
#define	A_MEMBER	"member"
#define A_INDEX		"index"
#define	A_FNAME		"funcName"
#define	A_RETURN	"return"
#define	A_ERRNO		"errno"
#define	A_STATUS	"status"
#define	A_EXPR		"expression"
#define	A_EVAL		"evaluatesTo"
#define	DEFAULT_SIZE	(256)
#define	A_SIZE		"size"
#define	VALUE_TRUE	"TRUE/THEN"
#define	VALUE_FALSE	"FALSE/ELSE"
#define	A_FILE		"file"
#define	A_DUMP		"dump"
#define	A_SEQ		"sequence"
#define	A_LOAD		"load"
#define	A_P1		"p1"
#define	A_P2		"p2"
#define	A_P3		"p3"
#define	A_P4		"p4"
#define	A_P5		"p5"
#define	A_P6		"p6"
#define A_P7		"p7"
#define	A_TIME		"time"
#define	A_FORCE		"force"
#define	A_FRAME		"frame"
#define	A_SIG		"signal"
#define	A_ACTION	"action"
#define	VALUE_ENABLE	"enable"
#define	VALUE_DISABLE	"disable"
#define	A_FLIP		"flip"
#define A_DELAY		"delay"

/** \brief This enum defines the contents of a pCode */
typedef	enum
{
	PC_E_UNKNOWN,		// 0 is unknown
	PC_E_PRINT,			// <P_PRINT>
	PC_E_DUMP,			// <P_DUMP>
	PC_E_LOAD,			// <P_LOAD>
	PC_E_FORMAT,		// <P_FORMAT>
	PC_E_SET,			// <P_SET>
	PC_E_FUNC,			// <P_FUNC>
	PC_E_IF,			// <P_IF>
	PC_E_WHILE,			// <P_WHILE>
	PC_E_THEN,			// <P_THEN>
	PC_E_ELSE,			// <P_ELSE>
	PC_E_CALL,			// <P_CALL>
	PC_E_SIG,			// <P_SIG>
	PC_E_ABORT,			// <P_ABORT>
	PC_E_SLEEP,			// <P_SLEEP>
	PC_E_FPUII,			// <P_FPUII>
	PC_E_FIOR,			// <P_FIOR>
	PC_E_AUX,			// <P_AUX>
} PCODE_TYPE;

/** \brief Maximum nesting level */
#define	MAX_NESTING	(20)

//=============================================================================
/**
 * Macro Defines
 */

//=============================================================================
/**
 * Structure definitions
 */
/** \brief Structure of common P_CODE information */
typedef	struct	pCommon
{
	int16_t		lineNumber;				// Line number in APIVSXML input file
	const char	*pName;					// Name for this pCode
	const char	*pDesc;					// Description for this pCode
} P_COMMON;

/** \brief Print code forward reference */
typedef	struct codePrint	C_PRINT;
/** \brief Code for <P_PRINT> Element */
struct	codePrint
{
	char			*pText;				// Pointer to Text
	RUN_LEVEL		level;				// Level to output text at
};

/** \brief What to dump */
typedef	enum
{
	DUMP_UNKNOWN,						// 0 = unknown
	DUMP_VD,							// Dump to VD
	DUMP_CMDMSG							// Dump the CMDMSG
} DUMP_TYPE;

#define	D_UNKNOWN	"Unknown"			// Unknown type
#define	D_VD		"VD"				// String for dumping VD
#define	D_CMDMSG	"CMDMSG"			// String for dumping CMDMSG

/** \brief Dump code forward reference */
typedef	struct codeDump	C_DUMP;
/** \brief Code for <P_DUMP> Element */
struct	codeDump
{
	ARG_P			*pFile;				// File to dump to
	ARG_P			*pSeq;				// Sequence number for file
	ARG_P			*pFrame;			// Frame number to dump
	RUN_LEVEL		level;				// Level to output text at
	DUMP_TYPE		type;				// What to dump
	boolean			force;				// Always dump regardless of mode
};

/** \brief Load code forward reference */
typedef	struct codeLoad	C_LOAD;
/** \brief Code for <P_LOAD> Element */
struct	codeLoad
{
	ARG_P			*pFile;				// File to load
	ARG_P			*pFrame;			// Frame to load
	DUMP_TYPE		type;				// What to load
};

/** \brief Format code forward reference */
typedef	struct codeFormat	C_FORMAT;
/** \brief Code for <P_FORMAT> Element */
struct	codeFormat
{
	ARG_P			*pArg;				// Argument to format
	RUN_LEVEL		level;				// Level to output text at
};

/** \brief FPUII code forward reference */
typedef	struct codeFPUII	C_FPUII;
/** \brief Code for <P_FPUII> Element */
struct	codeFPUII
{
	ARG_P			*pFile;				// Argument to format
};

/** \brief FIOR code forward reference */
typedef	struct codeFIOR	C_FIOR;
/** \brief Code for <P_FIOR> Element */
struct	codeFIOR
{
	ARG_P			*pFile;				// File to dump
	ARG_P			*pFrame;			// Frame number
	ARG_P                   *pDelay;			// Delay
};

/** \brief Set code forward reference */
typedef	struct	codeSet	C_SET;
/** \brief Code for a <P_SET> */
struct	codeSet
{
	ARG_P		*pVar;				// Pointer to argument to Set
	ARG_P		*pVal;				// Pointer to argument to Set from
	ARG_P		*pFile;				// Pointer to file to Set from
	char		*pMember;			// member name, NULL if not present
	ARG_P           *pIndex;                        // array index if applicable
	OP_TYPE		operation;			// The operation to perform
};

/** \brief If code forward reference */
typedef	struct	codeIf		C_IF;
/** \brief Code for <P_IF> */
struct	codeIf
{
	EXPR_P		expr;				// Expression to be evaluated
	P_CODE		*pThen;				// Pointer to Then block
	P_CODE		*pElse;				// Pointer to Else block
};

/** \brief While code forward reference */
typedef	struct	codeWhile	C_WHILE;
/** \brief Code for <P_WHILE> */
struct	codeWhile
{
	EXPR_P		expr;				// Expression to be evaluated
	P_CODE		*pWhile;			// Pointer to While block
};

/** \brief Abort code forward reference */
typedef	struct	codeAbort		C_ABORT;
/** \brief Code for <P_ABORT> */
struct	codeAbort
{
	int16_t		status;				// Status associated with Abort
};

/** \brief Sleep code forward reference */
typedef	struct codeSleep	C_SLEEP;
/** \brief Code for <P_SLEEP> Element */
struct	codeSleep
{
	ARG_P			*pArg;				// File to load
};

/** \brief Aux code forward reference */
typedef	struct codeAux	C_AUX;
/** \brief Code for <P_AUX> Element */
struct	codeAux
{
	boolean			flip;				// #ON (1), #OFF (0)
};

/** \brief Forward reference */
typedef struct pCode_SU	P_SU;			// Set Up Definition
/** \brief Set Up Definition */
struct pCode_SU
{
	P_SU		*pSU;					// Pointer to next Set Up

	P_COMMON	common;					// Common information

	P_CODE		*pCode;					// Pointer to code for this Set Up
};

/** \brief Forward reference */
typedef struct pCode_TD	P_TD;			// Tear Down Definition
/** \brief Tear Down Definition */
struct pCode_TD
{
	P_TD		*pTD;					// Pointer to next Tear Down

	P_COMMON	common;					// Common information

	P_CODE		*pCode;					// Pointer to code for this Tear Down
};

/** \brief Forward reference */
typedef struct pCode_SUB	P_SUB;		// Subroutine Definition
/** \brief Subroutine Definition */
struct pCode_SUB
{
	P_SUB		*pSUB;					// Pointer to next Subroutine

	P_COMMON	common;					// Common information

	P_CODE		*pCode;					// Pointer to code for this Subroutine
};

/** \brief Forward reference */
typedef struct pCode_TC	P_TC;			// Test Case Definition
/** \brief Test Case Definition */
struct pCode_TC
{
	P_TC		*pTC;					// Pointer to next Test Case

	P_COMMON	common;					// Common information

	P_SU		*pSU;					// Test Suite Set Up
	P_TD		*pTD;					// Test Suite Tear Down
	P_CODE		*pCode;					// Pointer to code for this Test Case
};

/** \brief Test Suite forward reference */
typedef struct pCode_TS P_TS;			// Test suite forward reference
/** \brief Test Suite Definition */
struct pCode_TS
{
	P_TS		*pTS;					// Pointer to next Test Suite

	P_COMMON	common;					// Common information

	P_TC		*pTC;					// Pointer to first test case

	P_SU		*pSU;					// Test Suite Set Up
	P_TD		*pTD;					// Test Suite Tear Down
};

/** \brief Call code forward reference */
typedef	struct	codeCall		C_CALL;
/** \brief Code for <P_CALL> */
struct	codeCall
{
	P_SUB 		*pSUB;				// Referenced subroutine
	P_SU		*pSU;				// Referenced SetUp
	P_TD		*pTD;				// Referenced TearDown
};

/** \brief Forward reference */
typedef struct pSigHandler	P_SigHand;	// Signal Handler Definition
/** \brief Signal Handler Definition */
struct pSigHandler
{
	P_SigHand	*pSH;					// Pointer to next Signal Handler

	P_COMMON	common;					// Common information

	P_CODE		*pCode;					// Pointer to code for this SigHandler
};

/** \brief Maximum signal number */
// #define	SIGNAL_MAX	(SIGRTMAX)
// This line will not compile, complaining of variable size
// So hard code size below
#define	SIGNAL_MAX	(63)

/** \brief Signal code forward reference */
typedef	struct	codeSignal		C_SIG;
/** \brief Code for <P_SIG> */
struct	codeSignal
{
	P_SigHand 	*pSH;				// Referenced Signal Handler
	ARG_P		*pSignal;			// Signal variable
	boolean		enable;				// TRUE=enable, FALSE=disable
};

/** \brief pCode structure used by language processor */
struct pCode
{
	P_CODE		*pCode;					// Pointer to next pCode

	PCODE_TYPE	type;					// This pCode contains ...

	P_COMMON	common;					// Common information

	union								// Different pCodes, according to type
	{
		C_PRINT		cPrint;				// <P_PRINT> Element
		C_DUMP		cDump;				// <P_DUMP> Element
		C_LOAD		cLoad;				// <P_LOAD> Element
		C_FORMAT	cFormat;			// <P_FORMAT> Element
		C_SET		cSet;				// <P_SET> Element
		C_FUNC		cFunc;				// <P_FUNCTION> Element
		C_IF		cIf;				// <P_IF> Element
		C_WHILE		cWhile;				// <P_WHILE> Element
		C_CALL		cCall;				// <P_CALL> Element
		C_SIG		cSig;				// <P_SIG> Element
		C_ABORT		cAbort;				// <P_ABORT> Element
		C_SLEEP		cSleep;				// <P_SLEEP> Element
		C_FPUII		cFPUII;				// <P_FPUII> Element
		C_FIOR		cFIOR;				// <P_FIOR> Element
		C_AUX		cAux;				// <P_AUX> Element
	} code;								// Code to be interpreted
};

/** \brief APIVSXML Top */
typedef struct pCode_Top
{
	P_TS		*pTS;					// Pointer to first Test Suite

	P_TC		*pTC;					// Pointer to first Test Case

	P_SU		*pSU;					// Pointer to first Set Up

	P_TD		*pTD;					// Pointer to first Tear Down

	P_SUB		*pSUB;					// Pointer to first Subroutine

	P_SigHand	*pSH;					// Pointer to first SignalHandler

} P_TOP;

//=============================================================================
/**
 * Extern
 */

/** \brief Start PCODE processing */
extern	int16_t pCode_start(void *pData,
							const char *pElement,
							const char **pAttr);

/** \brief End PCODE processing */
extern	int16_t pCode_end(void *pData,
						  const char *pElement);

/** \brief Parse an opening tag */
extern	int16_t	pCode_parseOpen(void *pData,
								const char *pElement,
								const char **pAttr);
/** \brief Parse a closing tag */
extern	int16_t	pCode_parseClose(void *pData, const char *pElement);

/** \brief Parse text between tags */
extern	void	pCode_text(void *pData, const char *pText, uint16_t len);

/** \brief Parse a TestSuite open tag */
extern	int16_t pCode_parseTestSuiteOpen(XML_Parser pParser,
										 const char **pAttr);

/** \brief Parse a TestSuite closing tag */
extern	int16_t	pCode_parseTestSuiteClose(XML_Parser pParser);

/** \brief Parse a TestCase open tag */
extern	int16_t pCode_parseTestCaseOpen(XML_Parser pParser,
										const char **pAttr);

/** \brief Parse a TestCase closing tag */
extern	int16_t pCode_parseTestCaseClose(XML_Parser pParser);

/** \brief Parse a TestCaseRef open tag */
extern	int16_t pCode_parseTestCaseRefOpen(XML_Parser pParser,
										   const char **pAttr);

/** \brief Parse a TestCaseRef closing tag */
extern	int16_t pCode_parseTestCaseRefClose(XML_Parser pParser);

/** \brief Parse a Print open tag */
extern	int16_t pCode_parsePrintOpen(XML_Parser pParser,
									 const char **pAttr);

/** \brief Parse a Print closing tag */
extern	int16_t pCode_parsePrintClose(XML_Parser pParser);

/** \brief Parse a SetUp open tag */
extern	int16_t pCode_parseSetUpOpen(XML_Parser pParser,
									 const char **pAttr);

/** \brief Parse a SetUp closing tag */
extern	int16_t pCode_parseSetUpClose(XML_Parser pParser);

/** \brief Parse a TearDown open tag */
extern	int16_t pCode_parseTearDownOpen(XML_Parser pParser,
									 const char **pAttr);

/** \brief Parse a TearDown closing tag */
extern	int16_t pCode_parseTearDownClose(XML_Parser pParser);

/** \brief Parse a Subroutine open tag */
extern	int16_t pCode_parseSubroutineOpen(XML_Parser pParser,
										  const char **pAttr);

/** \brief Parse a Subroutine closing tag */
extern	int16_t pCode_parseSubroutineClose(XML_Parser pParser);

/** \brief Parse a Define open tag */
extern	int16_t pCode_parseDefineOpen(XML_Parser pParser,
									  const char **pAttr);

/** \brief Parse a Define closing tag */
extern	int16_t pCode_parseDefineClose(XML_Parser pParser);

/** \brief Parse a Set open tag */
extern	int16_t pCode_parseSetOpen(XML_Parser pParser,
										  const char **pAttr);

/** \brief Parse a Set closing tag */
extern	int16_t pCode_parseSetClose(XML_Parser pParser);

/** \brief Parse a Function open tag */
extern	int16_t pCode_parseFunctionOpen(XML_Parser pParser,
										const char **pAttr);

/** \brief Parse a Function closing tag */
extern	int16_t pCode_parseFunctionClose(XML_Parser pParser);

/** \brief Parse a If open tag */
extern	int16_t pCode_parseIfOpen(XML_Parser pParser,
								  const char **pAttr);

/** \brief Parse a If closing tag */
extern	int16_t pCode_parseIfClose(XML_Parser pParser);

/** \brief Parse a Then open tag */
extern	int16_t pCode_parseThenOpen(XML_Parser pParser,
								  const char **pAttr);

/** \brief Parse a Then closing tag */
extern	int16_t pCode_parseThenClose(XML_Parser pParser);

/** \brief Parse a Else open tag */
extern	int16_t pCode_parseElseOpen(XML_Parser pParser,
								  const char **pAttr);

/** \brief Parse a Else closing tag */
extern	int16_t pCode_parseElseClose(XML_Parser pParser);

/** \brief Parse a Call open tag */
extern	int16_t pCode_parseCallOpen(XML_Parser pParser,
								  const char **pAttr);

/** \brief Parse a Call closing tag */
extern	int16_t pCode_parseCallClose(XML_Parser pParser);

/** \brief Parse a Abort open tag */
extern	int16_t pCode_parseAbortOpen(XML_Parser pParser,
								  const char **pAttr);

/** \brief Parse a Abort closing tag */
extern	int16_t pCode_parseAbortClose(XML_Parser pParser);

/** \brief Find a requested TS */
extern	P_TS	*pCode_findTestSuite(const char *testSuite);


/** \brief Return the TS list */
extern	P_TS	*pCode_getTopTS();

#endif	/* PCODE_H */

