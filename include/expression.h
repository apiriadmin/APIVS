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
 * \file expression.h
 *
 * This file contains the definition of the \c #expression processor
 *
 * \brief expression processor
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef	EXPRESSION_H
#define	EXPRESSION_H

//=============================================================================
/**
 * Includes
 */

#include	"vse_stnd.h"		// STD VSE Definitions

//=============================================================================
/**
 * Defines
 */

/** \brief Expression operator types */
typedef enum
{
	EXPR_OP_NONE,			// 0 Not used
	EXPR_OP_EQ,				// EQ ==
	EXPR_OP_NE,				// NE !=
	EXPR_OP_LT,				// LT <
	EXPR_OP_LE,				// LE <=
	EXPR_OP_GT,				// GT >
	EXPR_OP_GE				// GE >=
} EXPR_OP;

/** \brief Expression operator type strings */
#define	E_EQ		"EQ"
#define	E_NE		"NE"
#define	E_LT		"LT"
#define	E_LE		"LE"
#define	E_GT		"GT"
#define	E_GE		"GE"

//=============================================================================
/**
 * Macro Defines
 */

//=============================================================================
/**
 * Structure definitions
 */

/** \brief Expression processing structure */
typedef struct expression
{
	char			*pExpr;				// Original expression
	EXPR_OP			op;					// Operation for this expression
	ARG_P			*left;				// Left side argument
	char			*pLeftMember;		// Left side member, if any
	ARG_P			*right;				// Right side argument
	char			*pRightMember;		// Right side member, if any
} EXPR_P;

//=============================================================================
/**
 * Extern
 */

/** \brief Parse an expression */
extern	int16_t	exprParse(P_CODE *pCode);

/** \brief Evaluate an expression */
extern	int16_t	exprEvaluate(uint16_t ln, EXPR_P *pExpr, boolean *result);

#endif	/* EXPRESSION_H */

