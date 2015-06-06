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
 * \file emfio_stats.c
 * 
 * (c) Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief This file contains the functions that manipulate the command statistics.
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
void
emfio_clear()
{
	uint32_t loopIndex;

	// Loop over all commands.
	for (loopIndex = 0; loopIndex < EMFIO_MAX_NUM_COMMANDS; loopIndex++)
	{
		SDLCCommandInfo * pCommand = &emfio_commands[loopIndex];
		if (pCommand->initialized != 0)
		{
			pCommand->commandReceived = 0;
			pCommand->lastCommandSize = 0;
			pCommand->commandCount = 0;
			pCommand->startCountEpoch = 0;
			pCommand->endCountEpoch = 0;
		}
	}
}

//=============================================================================
int16_t
emfio_begin(int command)
{
	int16_t localErrorCode = EMFIO_OK;

	emfio_setErrorCode(localErrorCode);

	int16_t index = (int16_t)command;
	if ((index < 0) || (index >= EMFIO_MAX_NUM_COMMANDS))
	{
		emfio_setErrorCode(EMFIO_BAD_COMMAND);
		return -1;
	}
	SDLCCommandInfo * pCommand = &emfio_commands[index];
	if (pCommand->initialized == 0)
	{
		emfio_setErrorCode(EMFIO_BAD_COMMAND);
		return -1;
	}
	pthread_mutex_lock(&pCommand->mutex);
	{
		pCommand->commandCount = 0;
		pCommand->startCountEpoch = time(0);
		pCommand->endCountEpoch = 0;
	}
	pthread_mutex_unlock(&pCommand->mutex);

	return 0;
}

//=============================================================================
int16_t
emfio_terminate(int command)
{
	int16_t localErrorCode = EMFIO_OK;

	emfio_setErrorCode(localErrorCode);

	int16_t index = (int16_t)command;
	if ((index < 0) || (index >= EMFIO_MAX_NUM_COMMANDS))
	{
		emfio_setErrorCode(EMFIO_BAD_COMMAND);
		return -1;
	}
	SDLCCommandInfo * pCommand = &emfio_commands[index];
	if (pCommand->initialized == 0)
	{
		emfio_setErrorCode(EMFIO_BAD_COMMAND);
		return -1;
	}
	pthread_mutex_lock(&pCommand->mutex);
	{
		pCommand->endCountEpoch = time(0);
	}
	pthread_mutex_unlock(&pCommand->mutex);

	return 0;
}

//=============================================================================
int16_t
emfio_stats(int command, float * hertz)
{
	int16_t localErrorCode = EMFIO_OK;

	emfio_setErrorCode(localErrorCode);

	int16_t index = (int16_t)command;
	if ((index < 0) || (index >= EMFIO_MAX_NUM_COMMANDS))
	{
		emfio_setErrorCode(EMFIO_BAD_COMMAND);
		return -1;
	}
	SDLCCommandInfo * pCommand = &emfio_commands[index];
	if (pCommand->initialized == 0)
	{
		emfio_setErrorCode(EMFIO_BAD_COMMAND);
		return -1;
	}
	pthread_mutex_lock(&pCommand->mutex);
	{
		if (pCommand->startCountEpoch == 0)
		{
			localErrorCode = EMFIO_COUNT_NOT_BEGUN;
		}
		else if (pCommand->endCountEpoch == 0)
		{
			localErrorCode = EMFIO_COUNT_NOT_TERMINATED;
		}
		else
		{
			uint32_t deltaTime = (uint32_t)(pCommand->endCountEpoch - pCommand->startCountEpoch);
			if (deltaTime == 0)
			{
				localErrorCode = EMFIO_NOT_ENOUGH_TIME;
			}
			else
			{
				*hertz = (float)pCommand->commandCount / (float)deltaTime;
			}
		}
	}
	pthread_mutex_unlock(&pCommand->mutex);

	// Was an error detected?
	if (localErrorCode != EMFIO_OK)
	{
		emfio_setErrorCode(localErrorCode);
		return -1;
	}

	return 0;
}
