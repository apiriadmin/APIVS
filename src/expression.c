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
 * \file expression.c
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief expression processing
 *
 * This file contains the definition of functions for processing expressions.
 * Expressions are of the form:
 * 		A op B
 * A and B MUST be valid arguments, as supported by argument processing.
 * "op" may be either "!=", "==", "<", "<=", ">" or ">=". 
 * Only "int" and "char []" comparisons are supported.  All argument types
 * are cast to one of these 2 types, based upon the arguments and comparison
 * is done accordingly.  If the arguments are incompatible, an error
 * occurs and the run is aborted.
 * Since XML requires special syntax for embedded = signs and < and > signs,
 * we use the strings:
 * 	EQ -> ==
 * 	NE -> !=
 * 	LT -> <
 * 	LE -> <=
 * 	GT -> >
 * 	GE -> >=
 *
 * White space is permitted and required to terminate a variable name and
 * denote the start.
 * No AND or OR operations are permitted at this time; though they may be
 * added in the future if there is a need.
 * Therefore, only a single  comparison is permitted.  If an AND or OR
 * operation is currently needed, this may be supported by the use of
 * nested <If> statements.
 *
 * For instance, the expression ($A EQ $B) AND ($C EQ D)
 * may be supported by encoding:
 * <If expression="$A EQ $B">
 * 	<Then>
 * 		<If expression="$C EQ $D">
 * 			<Then>
 * 				<Print>($A EQ $B) AND ($C EQ $D)</Print>
 * 			</Then>
 * 		</If>
 * 	</Then>
 * </If>
 *
 * For and OR instance ($A EQ $B) OR ($C EQ $D)
 * <Define var="$flag" varType="int"/>
 * <Initialize var="$flag" initializer="%0"/>
 * <If expression="$A EQ $B">
 * 	<Then>
 * 		<Intialize var="$flag" initializer="%1"/>
 * 	</Then
 * </If>
 * <If expression="$C EQ $D">
 * 	<Then>
 * 		<Intialize var="$flag" initializer="%1"/>
 * 	</Then>
 * </If>
 * <If expression="$flag EQ %1">
 * 	<Then>
 * 		<Print>($A EQ $B) OR ($C EQ $D)</Print>
 * 	</Then>
 * 	<Else>
 * 		<Print>($A NE $B) AND ($C NE $D)</Print>
 * 	</Else>
 * </If>
 *
 * Of course, anyone that knows logical and how logical equations can be
 * modified to produce the same results would notice that the OR case
 * would more efficiently be written as an AND case.
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
#include	"vse_stnd.h"	// STD VSE Definitions
#include	"outputXML.h"	// Output XML Processor Definitions
#include	"expression.h"	// Expression Processor Definitions

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */

typedef	struct	expressionMap
{
	char	*string;
	EXPR_OP	op;
} EXPR_MAP;

//=============================================================================
/**
 * Local Definitions
 */

/** \brief Static table mapping variable type to string */
static EXPR_MAP	s_expressionMap[] =
{
	{ E_EQ,		EXPR_OP_EQ },
	{ E_NE,		EXPR_OP_NE },
	{ E_LT,		EXPR_OP_LT },
	{ E_LE,		EXPR_OP_LE },
	{ E_GT,		EXPR_OP_GT },
	{ E_GE,		EXPR_OP_GE },
	{ NULL,		EXPR_OP_NONE }	// Must be last entry
};

//=============================================================================
/**
 * \brief This function resolves an OP
 *
 * \param[in]	pCode - Pointer to expression P_CODE
 * \param[in]	pOp - Point to OP string
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static
int16_t
exprResolveOp(P_CODE *pCode, char *pOp)
{
	EXPR_MAP	*pExprMap = s_expressionMap;

	while (pExprMap->string != NULL)
	{
		if (!strcmp(pOp, pExprMap->string))
		{
			pCode->code.cIf.expr.op = pExprMap->op;
			return(STATUS_PASS);
		}

		pExprMap++;
	}

	{
		// OP not found in table
		char	string[OUTPUT_STRING_MAX];
		sprintf(string, "exprResolveOp(): OP [%s] not recognized", pOp);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
	}

	return(STATUS_FAIL);
}

//=============================================================================
/**
 * \brief This function parses an expression
 *
 * \param[in]	pCode - Pointer to expression P_CODE
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int16_t
exprParse(P_CODE *pCode)
{
	// Parse expression
	// Expression is of the form:
	// 	LEFT OP RIGHT
	// We parse out the LEFT, then OP then RIGHT
	// And attempt to resolve the parts	
	char	left[OUTPUT_STRING_MAX];
	char	leftMember[OUTPUT_STRING_MAX];
	boolean	leftMemberFound = FALSE;
	char	op[OUTPUT_STRING_MAX];
	char	right[OUTPUT_STRING_MAX];
	char	rightMember[OUTPUT_STRING_MAX];
	boolean	rightMemberFound = FALSE;
	char	*pExpr = pCode->code.cIf.expr.pExpr;
	char	*ptr;

	// Parse out LEFT
	while ((*pExpr == ' ') || (*pExpr == '\t'))	// Advance past white space
	{
		pExpr++;
	}
	ptr = left;
	while ((*pExpr != ' ') && (*pExpr != '\t') && (*pExpr != '\0'))
	{
		*ptr++ = *pExpr++;
		if ((*pExpr == ARG_MEMBER) && (left[0] == ARG_VAR_PREFIX))
			break;
	}
	*ptr = '\0';
	if (*pExpr == ARG_MEMBER)
	{	// A member is present
		pExpr++;
		ptr = leftMember;
		leftMemberFound = TRUE;
		while ((*pExpr != ' ') && (*pExpr != '\t') && (*pExpr != '\0'))
		{
			*ptr++ = *pExpr++;
		}
		*ptr = '\0';
	}	

	// Parse out OP
	while ((*pExpr == ' ') || (*pExpr == '\t'))	// Advance past white space
	{
		pExpr++;
	}
	ptr = op;
	while ((*pExpr != ' ') && (*pExpr != '\t') && (*pExpr != '\0'))
	{
		*ptr++ = *pExpr++;
	}
	*ptr = '\0';

	// Parse out RIGHT
	while ((*pExpr == ' ') || (*pExpr == '\t'))	// Advance past white space
	{
		pExpr++;
	}
	ptr = right;
	while ((*pExpr != ' ') && (*pExpr != '\t') && (*pExpr != '\0'))
	{
		*ptr++ = *pExpr++;
		if ((*pExpr == ARG_MEMBER) && (right[0] == ARG_VAR_PREFIX))
			break;
	}
	*ptr = '\0';
	if (*pExpr == ARG_MEMBER)
	{	// A member is present
		pExpr++;
		ptr = rightMember;
		rightMemberFound = TRUE;
		while ((*pExpr != ' ') && (*pExpr != '\t') && (*pExpr != '\0'))
		{
			*ptr++ = *pExpr++;
		}
		*ptr = '\0';
	}	


	// Make sure no trailing garbage
	while ((*pExpr == ' ') || (*pExpr == '\t'))	// Advance past white space
	{
		pExpr++;
	}
	if ('\0' != *pExpr)
	{
		// Trailing garbage
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"exprParse(): Trailing garbage in expression [%s]",
				pCode->code.cIf.expr.pExpr);
		OUTPUT_ERR(pCode->common.lineNumber, string, NULL, NULL);
		return(STATUS_FAIL);
	}
	
	// Resolve LEFT
	if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber, left))
	{
		return(STATUS_FAIL);
	}

	if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber, left, &pCode->code.cIf.expr.left))
	{
		return(STATUS_FAIL);
	}
	if (leftMemberFound)
	{
		// Left member
		if (!(ptr = malloc(strlen(leftMember) + 1)))
		{
			// Malloc failed
			char string[OUTPUT_STRING_MAX];
			sprintf(string, "exprParse(): Failed to malloc space for left member [%s] size [%d]",
				leftMember,
				strlen(leftMember) + 1);
			OUTPUT_ERR(pCode->common.lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		strncpy(ptr, leftMember, strlen(leftMember) + 1);
		pCode->code.cIf.expr.pLeftMember = ptr;
	}

	// Resolve RIGHT
	if (STATUS_FAIL == argValidateArgSyntax(pCode->common.lineNumber, right))
	{
		return(STATUS_FAIL);
	}

	if (STATUS_FAIL == argDereferenceArg(pCode->common.lineNumber, right, &pCode->code.cIf.expr.right))
	{
		return(STATUS_FAIL);
	}
	if (rightMemberFound)
	{
		// Right member
		if (!(ptr = malloc(strlen(rightMember) + 1)))
		{
			// Malloc failed
			char string[OUTPUT_STRING_MAX];
			sprintf(string, "exprParse(): Failed to malloc space for right member [%s] size [%d]",
				rightMember,
				strlen(rightMember) + 1);
			OUTPUT_ERR(pCode->common.lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		strncpy(ptr, rightMember, strlen(rightMember) + 1);
		pCode->code.cIf.expr.pRightMember = ptr;
	}

	// Resolve OP
	if (STATUS_FAIL == exprResolveOp(pCode, op))
	{
		return(STATUS_FAIL);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function evalutes a boolean expression
 *
 * \param[in]	ln - Line number of XML
 * \param[in]	pExpr - Pointer to expression to evaluate
 * \param[out]	result - Boolean result
 * \return	STATUS_PASS - Test conformance (PASS)
 * 		STATUS_FAIL - Test nonconformance (FAIL)
 */
int16_t
exprEvaluate(uint16_t ln, EXPR_P *pExpr, boolean *result)
{
	// For now, only allow INT arguments
	// Expression Processing
	boolean	compareInt;
	long	left;
	long	right;
	ARG_P	pLeft;	// For char [] comparison
	ARG_P	pRight;	// For char [] comparison
	char	cLeft[OUTPUT_STRING_MAX];
	char	cRight[OUTPUT_STRING_MAX];

	*result = FALSE;

	// Determine either string comparison or int comparison
	if (STATUS_FAIL ==
		argCompareType(ln, pExpr->left, pExpr->pLeftMember, &compareInt))
	{
		return(STATUS_FAIL);
	}

	if (compareInt)
	{
		// Cast arguments to int
		if (STATUS_FAIL ==
			argCastNumCompare(ln, pExpr->left, pExpr->pLeftMember, &left))
		{
			return(STATUS_FAIL);
		}
		if (STATUS_FAIL ==
			argCastNumCompare(ln, pExpr->right, pExpr->pRightMember, &right))
		{
			return(STATUS_FAIL);
		}
	}
	else
	{
		if (pExpr->left->varType == VAR_PUCHAR) {
			// Cast arguments to unsigned char *
			pLeft.arg.data.size = sizeof(cLeft);
			pLeft.arg.data.value.pUcharValue = cLeft;
			if (STATUS_FAIL == argCastPuchar(ln, pExpr->left, &pLeft)){
				return(STATUS_FAIL);
			}
			pRight.arg.data.size = sizeof(cRight);
			pRight.arg.data.value.pUcharValue = cRight;
			if (STATUS_FAIL == argCastPuchar(ln, pExpr->right, &pRight)) {
				return(STATUS_FAIL);
			}
		} else {
			// Cast arguments to char *
			pLeft.arg.data.size = sizeof(cLeft);
			pLeft.arg.data.value.pCharValue = cLeft;
			if (STATUS_FAIL == argCastPchar(ln, pExpr->left, &pLeft)) {
				return(STATUS_FAIL);
			}
			pRight.arg.data.size = sizeof(cRight);
			pRight.arg.data.value.pCharValue = cRight;
			if (STATUS_FAIL == argCastPchar(ln, pExpr->right, &pRight)) {
				return(STATUS_FAIL);
			}
			// Test for NULL PCHAR value
			if ((pLeft.arg.data.value.pCharValue == NULL)
				|| (pRight.arg.data.value.pCharValue == NULL)) {
				left = pLeft.arg.data.value.pCharValue;
				right = pRight.arg.data.value.pCharValue;
				compareInt = true;
			}
		}
	}

	// Now process according to OP required
	switch(pExpr->op)
	{
		case EXPR_OP_EQ:
		{
			if (compareInt)
			{
				*result = (left == right);
			}
			else
			{
				if (pLeft.varType == VAR_PUCHAR)
					*result = (0 == memcmp(cLeft, cRight, pLeft.arg.data.size));
				else
					*result = (0 == strcmp(cLeft, cRight));
			}
		}
		break;

		case EXPR_OP_NE:
		{
			if (compareInt)
			{
				*result = (left != right);
			}
			else
			{
				if (pLeft.varType == VAR_PUCHAR)
					*result = (0 != memcmp(cLeft, cRight, pLeft.arg.data.size));
				else
					*result = (0 != strcmp(cLeft, cRight));
			}
		}
		break;

		case EXPR_OP_LT:
		{
			if (compareInt)
			{
				*result = (left < right);
			}
			else
			{
				if (pLeft.varType == VAR_PUCHAR)
					*result = (memcmp(cLeft, cRight, pLeft.arg.data.size) < 0);
				else
					*result = (strcmp(cLeft, cRight) < 0);
			}
		}
		break;

		case EXPR_OP_LE:
		{
			if (compareInt)
			{
				*result = (left <= right);
			}
			else
			{
				if (pLeft.varType == VAR_PUCHAR)
					*result = (memcmp(cLeft, cRight, pLeft.arg.data.size) <= 0);
				else
					*result = (strcmp(cLeft, cRight) <= 0);
			}
		}
		break;

		case EXPR_OP_GT:
		{
			if (compareInt)
			{
				*result = (left > right);
			}
			else
			{
				if (pLeft.varType == VAR_PUCHAR)
					*result = (memcmp(cLeft, cRight, pLeft.arg.data.size) > 0);
				else
					*result = (strcmp(cLeft, cRight) > 0);
			}
		}
		break;

		case EXPR_OP_GE:
		{
			if (compareInt)
			{
				*result = (left >= right);
			}
			else
			{
				if (pLeft.varType == VAR_PUCHAR)
					*result = (memcmp(cLeft, cRight, pLeft.arg.data.size) >= 0);
				else
					*result = (strcmp(cLeft, cRight) >= 0);
			}
		}
		break;

		default:
		{
			// Unknown OP
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"exprEvaluate(): Program error!! Unknown OP for [%s]",
					pExpr->pExpr);
			OUTPUT_ERR(ln, string, NULL, NULL);
			return(STATUS_FAIL);
		}
		break;
	}

	return(STATUS_PASS);
}
