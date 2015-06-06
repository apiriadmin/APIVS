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
 * \file vse_stnd.h
 * This file contains the definition of the standard defines for the vse
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief vse_stnd
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef	VSE_STND_H
#define	VSE_STND_H

//=============================================================================
/**
 * Includes
 */

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */

typedef	int8_t	boolean;	// For defining booleans

#undef	TRUE
#define	TRUE	(1)

#undef	FALSE
#define	FALSE	(0)

#undef	MIN
#define	MIN(a, b)	(((a) < (b)) ? (a) : (b))

#undef	MAX
#define	MAX(a, b)	(((a) > (b)) ? (a) : (b))

#endif	/* VSE_STND_H */

