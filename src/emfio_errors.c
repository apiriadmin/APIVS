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
 * \brief This file contains the error code and error strings used by the
 *        FIO emulator.
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

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */
 static int16_t emfio_errorCode = EMFIO_OK;
 static char * emfio_errorTexts[] =
 {
	"No error",
	"Failed to allocate memory",
	"Failed to open file",
	"Failed to read file",
	"Failed to write file",
	"Bad size",
	"Bad SDLC command type",
	"No command was received",
	"No response expected",
	"No response preloaded",
	"Not implemented",
	"Cannot calculate Hz unless collecting for at least one second",
	"Cannot calculate Hz unless collection was begun",
	"Cannot calculate Hz unless collection was terminated",
	"Failed to initialize a mutex",
	"Bad SDLC command type"
 };

//=============================================================================
 int16_t
 emfio_getErrorCode()
 {
	 return emfio_errorCode;
 }

 //=============================================================================
void
 emfio_setErrorCode(int16_t newErrorCode)
 {
	 emfio_errorCode = newErrorCode;
 }

//=============================================================================
const char *
 emfio_getErrorText()
{
	 int16_t index = emfio_errorCode * -1;

	 return emfio_errorTexts[index];
}
