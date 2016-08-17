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
 * \file function.h
 *
 * This file contains the definition of the \c #function processor
 *
 * \brief function processor
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef	FUNCTION_H
#define	FUNCTION_H

//=============================================================================
/**
 * Includes
 */
#include	"argument.h"		// Argument definitions

//=============================================================================
/**
 * Defines
 */

//=============================================================================
/**
 * Macro Defines
 */

//=============================================================================
/**
 * Structure definitions
 */

/** \brief Function code forward reference */
typedef	struct	codeFunc	C_FUNC;

/** \brief Function Argument Direction */
typedef enum
{
	FUNC_IN		= 0x01,			// Argument is input only
	FUNC_OUT	= 0x02,			// Argument is output only
} FUNC_ARG_DIR;
// This information is used to make sure that a CONST
// is NOT allowed to be destroyed by a function
// that treats an argument as an output
// Return values and errno are ALWAYS output values

/** \brief Reference for function */
typedef struct funcPCode
{
	char			*pName;				// name of function

	int16_t		(*pExe)(uint16_t	ln,
						C_FUNC		*pCF);	// Point to function to execute

	VAR_TYPE		returnVarType;		// Return Type

	int				argCount;			// Number of arguments for function
	VAR_TYPE		varType[ARG_MAX];	// Variable Type
	FUNC_ARG_DIR	argDir[ARG_MAX];	// Argument direction
} FUNC_P;

/** \brief Code for a <P_FUNCTION> */
struct codeFunc
{
	// This information deals with the API itself
	FUNC_P		*pFunc;				// API Function Pointer

	// This information tells about what the XML file would like
	// to do with arguments
	ARG_P		*arg[ARG_MAX];		// Referenced argument list
	ARG_P		*errnoValue;		// Errno Value returned
	ARG_P		*returnValue;		// The function return value
};

//=============================================================================
/**
 * Extern
 */

/** \brief Dereference a function */
extern	int16_t	funcDereferenceFunc(uint16_t	ln,
									const char	*pName,
									FUNC_P		**ppFunc);

/** \brief Verify the arguments passed to a function */
extern	int16_t	funcVerifyArgs(uint16_t		ln,
							   C_FUNC		*pCF);

#endif	/* FUNCTION_H */

