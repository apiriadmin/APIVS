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
 * \file inputXML.h
 *
 * This file contains the definition of the \c #The input XML Processor
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief input XML Processor
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef	INPUTXML_H
#define	INPUTXML_H

//=============================================================================
/**
 * Includes
 */
#include	"expat.h"			// EXPAT Library definitions

//=============================================================================
/**
 * Defines
 */
/** \brief Maximum size of input buffer */
#define	INPUT_BUFFER_MAX	(256)

//=============================================================================
/**
 * Macro Defines
 */

//=============================================================================
/**
 * Structure Definitions
 */

//=============================================================================
/**
 * Extern
 */
/** \brief Definition for Input XML constructor */
extern	int16_t	inputXmlStart(char *inputXml);

/** \brief Definition for Input XML destructor */
extern	void	inputXmlEnd();

/** \brief Definition for getting input file name */
extern	const char	*inputXmlGetInput();

/** \brief Definition for parsing the APIVSXML input file */
extern	int16_t	inputXmlParse();

#endif	/* INPUTXML_H */

