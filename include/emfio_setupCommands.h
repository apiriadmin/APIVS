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
 * \file emfio_setupCommands.h
 *
 * (c) Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief This file declares the functions that setup and teardown the command structures.
 *
 * \date   12/22/2010
 *
 * \author Steve Kapp
 *
 * \version 1.0
 *****************************************************************************/

#ifndef emfio_setupCommands_h
#define emfio_setupCommands_h

//=============================================================================
/**
 * Includes
 */
#include	<stdint.h>		// STD IEEE Type Definitions - int16_t, uint8_t, etc.

//=============================================================================
/**
 * External Global Routines 
 */
extern int16_t emfio_setupCommands();
extern int16_t emfio_teardownCommands();
extern int16_t emfio_setupResponses();
extern int16_t emfio_teardownResponses();
extern uint32_t emfio_getNextLoadCounter();
extern uint32_t emfio_getNextReceiveCounter();

#endif
