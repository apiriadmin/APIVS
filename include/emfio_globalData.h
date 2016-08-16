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
 * \file emfio_globalData.h
 *
 * \brief This file defines the global data for the FIO emulator library.
 *
 * \date   12/22/2010
 *
 * \author Steve Kapp
 *
 * \version 1.0
 *****************************************************************************/

#ifndef emfio_globalData_h
#define emfio_globalData_h

//=============================================================================
/**
 * Includes
 */
#include	<stdint.h>		// STD IEEE Type Definitions - int16_t, uint8_t, etc.
#include	"emfio.h"		// STD Unix Definitions

//=============================================================================

/** 
 * Global Data
 */

extern char *fileNamePtr;           // store the passed File Name Pointer for access by all 

extern int16_t debugFlags;    		// set to 0 for production - turns on some debug features

extern int16_t emfio_allDone;       // wait for emfio_end()

extern SDLCCommandInfo * emfio_commands;
extern SDLCResponseInfo * emfio_responses;

#endif
