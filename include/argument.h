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
 * \file argument.h
 *
 * This file contains the definition of the \c #argument processor
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief argument processor
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef	ARGUMENT_H
#define	ARGUMENT_H

//=============================================================================
/**
 * Includes
 */

#include	<tod.h>				// TOD Definitions
#include	<fpui.h>			// FPUI Definitions
#include	<fio.h>				// FIO Definitions

#include	"vse_stnd.h"		// VSE Standard Definitions
#include	"inputXML.h"		// Input processing definitions
#include	"outputXML.h"		// Output processing definitions

//=============================================================================
/**
 * Defines
 */

/** \brief Minimum number of bytes allowed for a special character array */
#define	ARG_SPECIAL_CHAR_BUF_SIZE_MIN (8)

/** \brief Maximum number of function arguments, of all functions */
#define	ARG_MAX (7)		// Maximum arguments

/** \brief Variable definition prefix */
#define	ARG_VAR_PREFIX '$'

/** \brief Named Constant definition prefix */
#define	ARG_NAMED_CONST_PREFIX '#'

/** \brief Numeric Constant definition prefix */
#define	ARG_NUMERIC_CONST_PREFIX '%'

/** \brief String Constant definition prefix */
#define	ARG_STRING_CONST_PREFIX '@'

/** \brief Macro definition prefix */
#define	ARG_MACRO_PREFIX '!'

/** \brief Member character */
#define	ARG_MEMBER '.'

/** \brief Variable Types */
typedef enum
{
	VAR_VOID,				// Void Type
	VAR_CHAR,				// Character variable
	VAR_PCHAR,				// Character string
	VAR_UCHAR,				// Unsigned character
	VAR_PUCHAR,				// Unsigned string
	VAR_BOOL,				// Boolean
	VAR_SSIZET,				// ssize_t
	VAR_INT,				// int
	VAR_UINT,				// unsigned int
	VAR_FPUIH,				// fpui_handle
	VAR_FPUIAUXH,				// fpui_aux_handle
	VAR_OFLAGS,				// O_FLAGS
	VAR_FAPPH,				// FIO_APP_HANDLE
	VAR_FDEVH,				// FIO_DEV_HANDLE
	VAR_FVER,				// FIO_VERSION
	VAR_FVIEW,				// FIO_VIEW
	VAR_FCMAP,				// FIO_CHANNEL_MAP
	VAR_FCMASK,				// FIO_CMU_DC_MASK
	VAR_FCFSA,				// FIO_CMU_FSA
	VAR_FNOTF,				// FIO_NOTIFY
	VAR_FFSCHD,				// FIO_FRAME_SCHD
	VAR_FINF,				// FIO_INPUT_FILTER
	VAR_FINT,				// FIO_INPUTS_TYPE
	VAR_FTST,				// FIO_TRANS_STATUS
	VAR_FTBUF,				// FIO_TRANS_BUFFER
	VAR_FMFB,				// FIO_MMU_FLASH_BIT
	VAR_FPORT,				// FIO_PORT
	VAR_FDEVT,				// FIO_DEVICE_TYPE
	VAR_FFDSTAT,			        // FIO_FIOD_STATUS
	VAR_FFINFO,				// FIO_FRAME_INFO
	VAR_FFMST,				// FIO_TS_FM_STATE
	VAR_FVMST,				// FIO_TS1_VM_STATE
	VAR_FNOTI,				// FIO_NOTIFY_INFO
	VAR_TVAL,				// struct timeval
	VAR_DSTIT,				// dst_info_t
} VAR_TYPE;

/** \brief Variable Type Definitions */
#define	V_VOID		"void"
#define	V_CHAR		"char"
#define	V_PCHAR		"char []"
#define	V_UCHAR		"unsigned char"
#define	V_PUCHAR	"unsigned char []"
#define	V_BOOL		"boolean"
#define	V_SSIZET	"ssize_t"
#define	V_INT		"int"
#define	V_UINT		"unsigned int"
#define	V_FPUIH		"fpui_handle"
#define V_FPUIAUXH	"fpui_aux_handle"
#define	V_OFLAGS	"O_FLAGS"
#define	V_FAPPH		"FIO_APP_HANDLE"
#define	V_FDEVH		"FIO_DEV_HANDLE"
#define	V_FVER		"FIO_VERSION"
#define	V_FVIEW		"FIO_VIEW"
#define	V_FCMAP		"FIO_CHANNEL_MAP"
#define	V_FCMASK	"FIO_CMU_DC_MASK"
#define	V_FCFSA		"FIO_CMU_FSA"
#define	V_FNOTF		"FIO_NOTIFY"
#define	V_FFSCHD	"FIO_FRAME_SCHD"
#define	V_FINF		"FIO_INPUT_FILTER"
#define	V_FINT		"FIO_INPUTS_TYPE"
#define	V_FTST		"FIO_TRANS_STATUS"
#define	V_FTBUF		"FIO_TRANS_BUFFER"
#define	V_FMFB		"FIO_MMU_FLASH_BIT"
#define	V_FPORT		"FIO_PORT"
#define	V_FDEVT		"FIO_DEVICE_TYPE"
#define	V_FFDSTAT	"FIO_FIOD_STATUS"
#define	V_FFINFO	"FIO_FRAME_INFO"
#define	V_FFMST		"FIO_TS_FM_STATE"
#define	V_FVMST		"FIO_TS1_VM_STATE"
#define	V_FNOTI		"FIO_NOTIFY_INFO"
#define	V_TVAL		"struct timeval"
#define	V_DSTIT		"dst_info_t"

#define	MEM_TV_SEC      "tv_sec"
#define	MEM_TV_USEC     "tv_usec"
#define	MEM_DST_ABS     "absolute"
#define	MEM_DST_BEGIN   "begin"
#define	MEM_DST_END     "end"
#define	MEM_DST_TRANS   "secs_from_epoch_to_transition"
#define	MEM_DST_ADJ     "seconds_to_adjust"
#define	MEM_DST_TYPE    "type"
#define MEM_FINF_IP     "input_point"
#define MEM_FINF_LEAD	"leading"
#define MEM_FINF_TRAIL	"trailing"
#define MEM_FFDSTAT_COM "comm_enabled"
#define MEM_FFDSTAT_SRX "success_rx"
#define MEM_FFDSTAT_ERX "error_rx"
#define MEM_FFDSTAT_INF "frame_info"
#define MEM_FFSCHD_REQ	"req_frame"
#define MEM_FFSCHD_HZ	"frequency"
#define MEM_FFINFO_HZ	"frequency"
#define MEM_FFINFO_SRX	"success_rx"
#define MEM_FFINFO_ERX	"error_rx"
#define MEM_FFINFO_E10	"error_last_10"
#define MEM_FFINFO_SEQ	"last_seq"
#define MEM_FNOTI_RXF	"rx_frame"
#define MEM_FNOTI_STAT	"status"
#define MEM_FNOTI_SEQ	"seq_number"
#define MEM_FNOTI_CNT	"count"
#define MEM_FNOTI_FIOD	"fiod"
#define MEM_FCMAP_OUT	"output_point"
#define MEM_FCMAP_FIOD	"fiod"
#define MEM_FCMAP_CHAN	"channel"
#define MEM_FCMAP_CLR	"color"
#define MEM_FTBUF_IP    "input_point"
#define MEM_FTBUF_STAT  "state"
#define MEM_FTBUF_TIM    "timestamp"


/** \brief Named Constant Definitions */
typedef	struct namedConstant
{
	char        *pName;  // String associated with constant
	int         value;   // The value of the constant
	VAR_TYPE    type;    // The variable type of constant
} ARG_NC;

#define	NC_NULL                 "#NULL"
#define	NC_NULL_VALUE           (NULL)
#define	NC_TRUE                 "#TRUE"
#define	NC_TRUE_VALUE           (0 == 0)
#define	NC_FALSE                "#FALSE"
#define	NC_FALSE_VALUE          (0 != 0)
#define	NC_ON                   "#ON"
#define	NC_ON_VALUE             (1)
#define	NC_OFF                  "#OFF"
#define	NC_OFF_VALUE            (0)
#define	NC_ROWS                 "#ROWS"
#define	NC_COLUMNS              "#COLUMNS"

#define	NC_ORDWR                "#O_RDWR"
#define	NC_ORDONLY              "#O_RDONLY"
#define	NC_OWRONLY              "#O_WRONLY"
#define	NC_ODIRECT              "#O_DIRECT"
#define	NC_ONONBLOCK            "#O_NONBLOCK"
#define	NC_OEXCL                "#O_EXCL"

#define	NC_FIO_VERLIB           "#FIO_VERSION_LIBRARY"
#define	NC_FIO_VERLKM           "#FIO_VERSION_LKM"
#define	NC_FIO_VIEWAPP          "#FIO_VIEW_APP"
#define	NC_FIO_VIEWSYS          "#FIO_VIEW_SYSTEM"
#define	NC_FIO_CDCM1            "#FIO_CMU_DC_MASK1"
#define	NC_FIO_CDCM2            "#FIO_CMU_DC_MASK2"
#define	NC_FIO_CDCM3            "#FIO_CMU_DC_MASK3"
#define	NC_FIO_CDCM4            "#FIO_CMU_DC_MASK4"
#define	NC_FIO_CFSANONE         "#FIO_CMU_FSA_NONE"
#define	NC_FIO_CFSANL           "#FIO_CMU_FSA_NON_LATCHING"
#define	NC_FIO_CFSAL            "#FIO_CMU_FSA_LATCHING"
#define	NC_FIO_NOT1             "#FIO_NOTIFY_ONCE"
#define	NC_FIO_NOTA             "#FIO_NOTIFY_ALWAYS"
#define	NC_FIO_ITRAW            "#FIO_INPUTS_RAW"
#define	NC_FIO_ITFLT            "#FIO_INPUTS_FILTERED"
#define	NC_FIO_TSS              "#FIO_TRANS_SUCCESS"
#define	NC_FIO_TSFO             "#FIO_TRANS_FIOD_OVERRUN"
#define	NC_FIO_TSAO             "#FIO_TRANS_APP_OVERRUN"
#define	NC_FIO_MFBOFF           "#FIO_MMU_FLASH_BIT_OFF"
#define	NC_FIO_MFBON            "#FIO_MMU_FLASH_BIT_ON"
#define	NC_FIO_TSFMOFF          "#FIO_TS_FM_OFF"
#define	NC_FIO_TSFMON           "#FIO_TS_FM_ON"
#define	NC_FIO_TS1VMOFF         "#FIO_TS1_VM_OFF"
#define	NC_FIO_TS1VMON          "#FIO_TS1_VM_ON"
#define	NC_FIO_IPB              "#FIO_INPUT_POINTS_BYTES"
#define	NC_FIO_OPB              "#FIO_OUTPUT_POINTS_BYTES"
#define	NC_FIO_CPB              "#FIO_CHANNEL_BYTES"
#define	NC_FIOSP3               "#FIO_SP3"
#define	NC_FIOSP5               "#FIO_SP5"
#define	NC_FIOSP8               "#FIO_SP8"
#define	NC_FIO332               "#FIO332"
#define	NC_FIOTS1               "#FIOTS1"
#define	NC_FIOTS2               "#FIOTS2"
#define	NC_FIOMMU               "#FIOMMU"
#define	NC_FIODR1               "#FIODR1"
#define	NC_FIODR2               "#FIODR2"
#define	NC_FIODR3               "#FIODR3"
#define	NC_FIODR4               "#FIODR4"
#define	NC_FIODR5               "#FIODR5"
#define	NC_FIODR6               "#FIODR6"
#define	NC_FIODR7               "#FIODR7"
#define	NC_FIODR8               "#FIODR8"
#define	NC_FIOTF1               "#FIOTF1"
#define	NC_FIOTF2               "#FIOTF2"
#define	NC_FIOTF3               "#FIOTF3"
#define	NC_FIOTF4               "#FIOTF4"
#define	NC_FIOTF5               "#FIOTF5"
#define	NC_FIOTF6               "#FIOTF6"
#define	NC_FIOTF7               "#FIOTF7"
#define	NC_FIOTF8               "#FIOTF8"
#define	NC_FIOCMU               "#FIOCMU"
#define	NC_FIOINSIU1            "#FIOINSIU1"
#define	NC_FIOINSIU2            "#FIOINSIU2"
#define	NC_FIOINSIU3            "#FIOINSIU3"
#define	NC_FIOINSIU4            "#FIOINSIU4"
#define	NC_FIOINSIU5            "#FIOINSIU5"
#define	NC_FIOOUT6SIU1          "#FIOOUT6SIU1"
#define	NC_FIOOUT6SIU2          "#FIOOUT6SIU2"
#define	NC_FIOOUT6SIU3          "#FIOOUT6SIU3"
#define	NC_FIOOUT6SIU4          "#FIOOUT6SIU4"
#define	NC_FIOOUT14SIU1         "#FIOOUT14SIU1"
#define	NC_FIOOUT14SIU2         "#FIOOUT14SIU2"
#define NC_FIO_HZ_0             "#FIO_HZ_0"
#define NC_FIO_HZ_ONCE          "#FIO_HZ_ONCE"
#define NC_FIO_HZ_1             "#FIO_HZ_1"
#define NC_FIO_HZ_2             "#FIO_HZ_2"
#define NC_FIO_HZ_5             "#FIO_HZ_5"
#define NC_FIO_HZ_10            "#FIO_HZ_10"
#define NC_FIO_HZ_20            "#FIO_HZ_20"
#define NC_FIO_HZ_30            "#FIO_HZ_30"
#define NC_FIO_HZ_40            "#FIO_HZ_40"
#define NC_FIO_HZ_50            "#FIO_HZ_50"
#define NC_FIO_HZ_60            "#FIO_HZ_60"
#define NC_FIO_HZ_70            "#FIO_HZ_70"
#define NC_FIO_HZ_80            "#FIO_HZ_80"
#define NC_FIO_HZ_90            "#FIO_HZ_90"
#define NC_FIO_HZ_100           "#FIO_HZ_100"
#define NC_FIO_HZ_MAX           "#FIO_HZ_MAX"
#define NC_FIO_GREEN            "#FIO_GREEN"
#define NC_FIO_YELLOW           "#FIO_YELLOW"
#define NC_FIO_RED              "#FIO_RED"

#define	NC_TOD_V_SRC_LINE	"#TOD_TIMESRC_LINESYNC"
#define	NC_TOD_V_SRC_RTC	"#TOD_TIMESRC_RTCSQWR"
#define	NC_TOD_V_SRC_CRYS	"#TOD_TIMESRC_CRYSTAL"
#define	NC_TOD_V_SRC_EXT1	"#TOD_TIMESRC_EXTERNAL1"
#define	NC_TOD_V_SRC_EXT2	"#TOD_TIMESRC_EXTERNAL2"

#define	NC_SIG_FIO		"#FIO_SIGIO"
#define NC_SIG_TOD		"#TOD_SIG"
#define	NC_SIG_WINCH		"#SIGWINCH"
#define NC_SIG_USR1		"#SIGUSR1"
#define NC_SIG_IO		"#SIGIO"

/** \brief Macro definitions */
#define	MAC_STRLEN		"!strlen"
#define	MAC_VDCOMP		"!VDCompare"
#define	MAC_SIZEOF		"!sizeof"
#define	MAC_BIT_TEST            "!FIO_BIT_TEST"
#define	MAC_CMDMSG		"!CMDMSGCompare"

#define	MAC_ARG_START		'('
#define	MAC_ARG_END			')'
#define	MAC_UNDERSCORE		'_'
#define	MAC_ARG_SEPARATOR	','
#define	MAC_ARG_MAX			(2)

typedef enum
{
	MAC_TYPE_UNKNOWN,
	MAC_TYPE_STRLEN,
	MAC_TYPE_VDCOMP,
	MAC_TYPE_SIZEOF,
	MAC_TYPE_BIT_TEST,
	MAC_TYPE_CMDMSG,
} MAC_TYPE;

/** \brief Macro Definitions */
typedef	struct macroArg
{
	char       *pName;               // Name of macro
	MAC_TYPE   macroType;            // The type index for macro
	VAR_TYPE   returnType;           // The value returned by macro
	int        argCount;             // Count of arguments
	VAR_TYPE   varType[MAC_ARG_MAX]; // The macros argument type
} ARG_MAC;

/** \brief Maximum size of a macro name */
#define	MAC_MAX			(20)

/** \brief ENUM of argument types */
typedef	enum
{
	ARG_UNKNOWN,		// Unknown argument type (0)
	ARG_VAR,			// A variable argument
	ARG_NAMED_CONST,	// A Named Constant argument
	ARG_NUMERIC_CONST,	// A Numeric Constant argument
	ARG_STRING_CONST,	// A String Constant argument
	ARG_MACRO,			// A Macro argument
	ARG_TMP				// For temporary, on the stack, arguments
} ARG_TYPE;

#define	VA_UNKNOWN			"Argument type unknown"
#define	VA_VAR				"Variable"
#define	VA_NAMED_CONST		"Named Constant"
#define	VA_NUMERIC_CONST	"Numeric Constant"
#define	VA_STRING_CONST		"String Constant"
#define	VA_MACRO			"Macro"
#define	VA_TMP				"Temporary stack argument"

/** \brief The Set operation */
typedef	enum
{
	OP_UNKNOWN,							// 0 = unknown
	OP_EQ,								// Set Var to Value
	OP_ADD,								// Add Value to Var
	OP_SUB,								// Subtract Value for Var
} OP_TYPE;

/** \brief Maximum size of converted buffer */
#define	ARG_CONVERTED_MAX	(2048)

//=============================================================================
/**
 * Macro Defines
 */

//=============================================================================
/**
 * Structure definitions
 */

/** \brief Forward reference */
typedef	struct	argPcode	ARG_P;

/** \brief Union for argument value */
typedef union argValue
{
	int                intValue;      // Integer value
	unsigned int       uintValue;     // Unsigned integer value
	char               charValue;     // Character values
	char               *pCharValue;   // Character string value
	unsigned char      uCharValue;    // Unsigned Character values
	unsigned char      *pUcharValue;  // Unsigned Character string value
	struct timeval     timevalValue;  // TV value
	dst_info_t         dstinfoValue;  // DST value
	fpui_handle        fpuiHandle;    // FPUI Handle
	fpui_aux_handle    fpuiAuxHandle; // FPUI AUX Handle
	ssize_t	           ssizetValue;   // Ssize_t value
	FIO_APP_HANDLE     fioAppHandle;  // FIO handle
	FIO_DEV_HANDLE     fioDevHandle;  // FIO Device Handle
	FIO_PORT           fioPort;       // FIO_PORT
	FIO_DEVICE_TYPE    fioDt;         // FIO_DEVICE_TYPE
	FIO_VERSION        fioVer;        // FIO_VERSION
	FIO_NOTIFY_INFO    fioNotifyInfo; // FIO_NOTIFY_INFO
	FIO_TS1_VM_STATE   fioVms;        // FIO_TS1_VM_STATE
	FIO_VIEW           fioView;       // FIO_VIEW
	FIO_TS_FM_STATE    fioFms;        // FIO_TS_FM_STATE
	FIO_FIOD_STATUS    fioFiodStatus; // FIO_FIOD_STATUS
	FIO_MMU_FLASH_BIT  fioFb;         // FIO_MMU_FLASH_BIT
	FIO_TRANS_STATUS   fioTstatus;    // FIO_TRANS_STATUS
	FIO_TRANS_BUFFER   *fioTbuffer;   // FIO_TRANS_BUFFER
	FIO_INPUTS_TYPE    fioFint;       // FIO_INPUTS_TYPE
	FIO_INPUT_FILTER   *fioFinf;      // FIO_INPUT_FILTER
	FIO_FRAME_SCHD     *fioFschd;     // FIO_FRAME_SCHD
	FIO_FRAME_INFO     *fioFrameInfo; // FIO_FRAME_INFO
	FIO_NOTIFY         fioFnotf;      // FIO_NOTIFY
	FIO_CMU_FSA        fioFcfsa;      // FIO_CMU_FSA
	FIO_CMU_DC_MASK    fioFcmask;     // FIO_CMU_DC_MASK
	FIO_CHANNEL_MAP    *fioFcmap;     // FIO_CHANNEL_MAP
	FIO_FRAME_STATUS   fioFstat;      // FIO_FRAME_STATUS
} ARG_VALUE_P;

/** \brief Macro storage */
typedef	struct	argMacro
{
	MAC_TYPE	macType;              // Macro type
	int             argCount;             // Count of macro arguments
	VAR_TYPE	varType[MAC_ARG_MAX]; // Argument type allowed
	ARG_P		*arg[MAC_ARG_MAX];    // Macro argument
} ARG_MAC_P;

/** \brief Value data */
typedef	struct	argData
{
	ARG_VALUE_P		value;				// Value of this argument
	uint16_t		size;				// Size of array
} ARG_DATA_P;

/** \brief Actual argument definition */
struct argPcode
{
	char			*pName;				// Name of this argument
	ARG_TYPE		argType;			// Type of arg data stored here
	VAR_TYPE		varType;			// Type of this variable
	union
	{
		ARG_DATA_P	data;				// Actual variable data
		ARG_MAC_P	macro;				// Macro value
	} arg;								// Actual argument data
};

/** \brief Forward refence for argumnet linked list */
typedef	struct	argNode	ARG_NODE;
/** \brief Linked list node for an argument */
struct	argNode
{
	ARG_NODE		*pNode;				// Point to next node
	ARG_P			argCode;			// The argument information
};

/** \brief The argument container */
typedef	struct	argContainer
{
	ARG_NODE		*pVar;				// Variable list
	ARG_NODE		*pNC;				// Named constant list
	ARG_NODE		*pNum;				// Numeric constant list
	ARG_NODE		*pStr;				// String constant list
	ARG_NODE		*pMacro;			// Macro list
} ARG_CONTAINER;

//=============================================================================
/**
 * Extern
 */

/** \brief Parse an argument type */
extern int16_t argParseType(const char *pValue, VAR_TYPE *pType);

/** \brief Parse an argument variable */
extern int16_t argParseVar(const char *pValue);

/** \brief Parse a variable parameter */
extern int16_t argValidateVarSyntax(uint16_t ln, const char *val);

/** \brief Parse an argument parameter */
extern int16_t argValidateArgSyntax(uint16_t ln, const char *val);

/** \brief Verify Compatibility of types */
extern int16_t argVerifyInitCompat(uint16_t ln, ARG_P *pTo,
					const char *pMember,
					ARG_P *pFrom);

/** \brief Return an variable type string */
extern const char *argVarStringGet(VAR_TYPE type);

/** \brief Return an argument type string */
extern const char *argArgStringGet(ARG_TYPE type);

/** \brief Define a variable */
extern int16_t argDefineVar(uint16_t ln, const char *pName, VAR_TYPE vt,
				uint16_t size);

/** \brief Define a Named Constant */
extern int16_t argDefineNC(const char *pName, int value, VAR_TYPE type);

/** \brief Define a Macro */
extern int16_t argDefineMacro(ARG_MAC *pMAC);

/** \brief Dereference a variable */
extern int16_t argDereferenceVar(uint16_t ln, const char *pName,
					ARG_P **ppVar);

/** \brief Dereference a numeric */
extern int16_t argDereferenceNumeric(uint16_t ln, const char *pName,
					ARG_P **ppVar);

/** \brief Dereference a string */
extern int16_t argDereferenceString(uint16_t ln, const char *pName,
					ARG_P **ppVar);

/** \brief Dereference a macro */
extern int16_t argDereferenceMacro(uint16_t ln, const char *pName,
					ARG_P **ppVar);

/** \brief Dereference a Named Constant */
extern int16_t argDereferenceNC(uint16_t ln, const char *pName,
					ARG_P **ppVar);

/** \brief Dereference an argument */
extern int16_t argDereferenceArg(uint16_t ln, const char *pName,
					ARG_P **ppVar);

/** \brief Get the first element in Var Table */
extern ARG_NODE *argVarTableGet();

/** \brief Format a argument */
extern int16_t argFormat(uint16_t ln, const RUN_LEVEL level, ARG_P *pArg);

/** \brief Set a variable */
extern int16_t argSetVar(uint16_t ln, OP_TYPE operation, ARG_P *pValue,
				ARG_P *pFile,
				ARG_P *pVar,
				ARG_P *pIndex,
				const char *pMember);

/** \brief Cast argument to int */
extern int16_t argCastInt(uint16_t ln, ARG_P *pArg, int *pInt);

/** \brief Cast argument to fpui_handle */
extern int16_t argCastFpuiHandle(uint16_t ln, ARG_P *pArg,
					fpui_handle *pFH);

/** \brief Cast argument to fpui_aux_handle */
extern int16_t argCastFpuiAuxHandle(uint16_t ln, ARG_P *pArg,
					fpui_aux_handle *pFAuxH);

/** \brief Cast argument to FIO_APP_HANDLE */
extern int16_t argCastFapph(uint16_t ln, ARG_P *pArg,
					FIO_APP_HANDLE *pFapph);

/** \brief Cast argument to FIO_DEV_HANDLE */
extern int16_t argCastFdevh(uint16_t ln, ARG_P *pArg,
					FIO_DEV_HANDLE *pFapph);

/** \brief Cast argument to FIO_PORT */
extern int16_t argCastFport(uint16_t ln, ARG_P *pArg,
					FIO_PORT *pFport);

/** \brief Cast argument to FIO_DEVICE_TYPE */
extern int16_t argCastFdevt(uint16_t ln, ARG_P *pArg,
					FIO_DEVICE_TYPE *pFdevt);

/** \brief Cast argument to FIO_VERSION */
extern int16_t argCastFver(uint16_t ln, ARG_P *pArg,
					FIO_VERSION *pFver);

/** \brief Cast argument to FIO_TS1_VM_STATE */
extern int16_t argCastFvmst(uint16_t ln, ARG_P *pArg,
					FIO_TS1_VM_STATE *pFvms);

/** \brief Cast argument to FIO_VIEW */
extern int16_t argCastFview(uint16_t ln, ARG_P *pArg,
					FIO_VIEW *pFview);

/** \brief Cast argument to FIO_TS_FM_STATE */
extern int16_t argCastFfmst(uint16_t ln, ARG_P *pArg,
					FIO_TS_FM_STATE *pFfms);

/** \brief Cast argument to FIO_MMU_FLASH_BIT */
extern int16_t argCastFmfb(uint16_t ln, ARG_P *pArg,
					FIO_MMU_FLASH_BIT *pFb);

/** \brief Cast argument to FIO_INPUTS_TYPE */
extern int16_t argCastFint(uint16_t ln, ARG_P *pArg,
					FIO_INPUTS_TYPE *pFint);

/** \brief Cast argument to FIO_NOTIFY */
extern int16_t argCastFnotf(uint16_t ln, ARG_P *pArg,
					FIO_NOTIFY *pFnotf);

/** \brief Cast argument to FIO_CMU_FSA */
extern int16_t argCastFcfsa(uint16_t ln, ARG_P *pArg,
					FIO_CMU_FSA *pFcfsa);

/** \brief Cast argument to FIO_CMU_DC_MASK */
extern int16_t argCastFcmask(uint16_t ln, ARG_P *pArg,
					FIO_CMU_DC_MASK *pFcmask);

/** \brief Cast argument to FIO_TRANS_STATUS */
extern int16_t argCastFtst(uint16_t ln, ARG_P *pArg,
					FIO_TRANS_STATUS *pFtst);

/** \brief Cast argument to long */
extern int16_t argCastLong(uint16_t ln, ARG_P *pArg, long *pLong);

/** \brief Cast argument to char */
extern int16_t argCastChar(uint16_t ln, ARG_P *pArg, char *pChar);

/** \brief Cast argument to pchar */
extern int16_t argCastPchar(uint16_t ln, ARG_P *pFrom, ARG_P *pTo);

/** \brief Cast argument to uchar */
extern int16_t argCastUchar(uint16_t ln, ARG_P *pArg,
					unsigned char *pUchar);

/** \brief Cast argument to puchar */
extern int16_t argCastPuchar(uint16_t ln, ARG_P *pFrom, ARG_P *pTo);

/** \brief Cast argument to ssizet */
extern int16_t argCastSsizet(uint16_t ln, ARG_P *pArg,
					ssize_t *pSsizet);

/** \brief Cast argument to boolean */
extern int16_t argCastBoolean(uint16_t ln, ARG_P *pArg,
					boolean *pBoolean);

/** \brief Cast argument to unsigned int */
extern int16_t argCastUint(uint16_t ln, ARG_P *pArg,
					unsigned int *pUint);

/** \brief Cast argument to struct timeval */
extern int16_t argCastTv(uint16_t ln, ARG_P *pArg,
					struct timeval *pTv);

/** \brief Cast argument to dst_info_t */
extern int16_t argCastDstInfo(uint16_t ln, ARG_P *pArg,
					dst_info_t *pDstInfo);

/** \brief Cast argument for numeric comparison */
extern int16_t argCastNumCompare(uint16_t ln, ARG_P *pArg,
					char *pMember, long *pLong);

/** \brief Initialize the argument processor */
extern int16_t argInit();

/** \brief Dump the variable table -- DEBUG */
extern int16_t argDumpVar(uint16_t lineNumber);

/** \brief See if argument is "comparable" */
extern int16_t	argCompareType(uint16_t ln, ARG_P *pArg, char *pMember,
					boolean *comp);

/** \brief Return an alternate type for the varType passed */
extern VAR_TYPE argAlternateVarType(VAR_TYPE type);

/** \brief Set up an initialization buffer from a file */
extern int16_t argSet(uint16_t ln, const char *pPath,
					const char *pFile,
					unsigned char **ppBuf,
					unsigned int *pSize);

#endif	/* ARGUMENT_H */

