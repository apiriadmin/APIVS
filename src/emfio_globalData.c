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
 * \file emfio_globalData.c
 * 
 * (c) Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief This file contains declarations of the global data used by the FIO
 *        emulator.
 *
 * \date   12/22/2010
 *
 * \author Steve Kapp
 *
 * \version 1.0
 *****************************************************************************/

//=============================================================================
/**
 * Includes
 */
#include "emfio.h"
#include "emfio_globalData.h"

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */
 
//=============================================================================
/**
 * Global Data Variables 
 */
char * fileNamePtr = "testing.txt";

//  global debug flags = use bits to turn on and off various behaviors 
int16_t debugFlags = 0;    //   set to 0 for production

//  pja - removed autoIncrement - new interface - fileSeqNum passed to dumpVD 
// int16_t autoIncrement = 0;     // for multiple repeated calls to dumpVD, showVD, etc - increment file # 

int16_t emfio_allDone = FALSE;  //  waiting for emfio_end() call

SDLCCommandInfo * emfio_commands = 0;
SDLCResponseInfo * emfio_responses = 0;
