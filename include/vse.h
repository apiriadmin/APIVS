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
 * \file vse.h
 * This file contains the definition of the \c #vse
 *
 * \brief vse
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef	VSE_H
#define	VSE_H

//=============================================================================
/**
 * Includes
 */

#include	"vse_stnd.h"	// VSE Standard type definitions

//=============================================================================
/**
 * Extern
 */
/** \brief Definition for the VSE engine function */
extern	int16_t	vse(char *);

//=============================================================================
/**
 * Local Defines
 */

/** \brief Run Levels */
typedef enum
{
	LEVEL_ALWAYS,			// Always output this message
	LEVEL_CONFORMANCE,		// Conformance / Non-conformance
	LEVEL_SUMMARY,			// Summary Output
	LEVEL_TRACE,			// Trace Output
	LEVEL_MAX				// For array definitions
} RUN_LEVEL;

/** \brief Level definitions */
#define	LVL_0		"always"
#define	LVL_1		"conformance"
#define	LVL_2		"summary"
#define	LVL_3		"trace"

#endif	/* VSE_H */

