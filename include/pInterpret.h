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
 * \file pInterpret.h
 *
 * This file contains the definition of the \c #pInterpret generator
 *
 * \brief pInterpret generator
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef	PINTERPRET_H
#define	PINTERPRET_H

//=============================================================================
/**
 * Includes
 */
#include	"inputXML.h"		// Input processing definitions
#include	"pCode.h"			// pInterpret processing definitions

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

//=============================================================================
/**
 * Extern
 */

/** \brief Set Capture Mode */
extern	void	interpretSetCapture();

/** \brief Interpret a TS */
extern	int16_t	interpretTS(P_TS *pTS);

/** \brief Interpret the P_CODE */
extern	int16_t	interpretPcode(P_CODE	*pCode);

#endif	/* PINTERPRET_H */

