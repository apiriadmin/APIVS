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
 * \file emfio_responses.c
 * 
 * (c) Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief This file contains the functions that manipulate the command responses.
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
#include <unistd.h>
#include <string.h>		// memcmp, memcpy
#include <stdlib.h>		// malloc, free
#include "emfio.h"
#include "emfio_globalData.h"
#include "emfio_setupCommands.h"

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Defines
 */

//=============================================================================
static void
outputHex(FILE * file, void *pBuf, unsigned int size)
{
	unsigned int	ii;
	void			*end;
	void			*tmpptr;
	void			*tmpend;
	void			*addr = NULL;

	fprintf(file, "0x???????? ");
	for (ii = 0; ii < 16; ii++)
	{
		fprintf(file, "%02x ", ii);
	}
	fprintf(file, "\n");
	fprintf(file, "---------- ");
	for (ii = 0; ii < 16; ii++)
	{
		fprintf(file, "-- ");
	}
	fprintf(file, "\n");

	end = pBuf + size;
	while (pBuf < end)
	{
		tmpptr = pBuf;
		tmpend = pBuf + 16;
		if (tmpend > end)
		{
			tmpend = end;
		}

		// Output the hex codes
		fprintf(file, "%10p ", addr);
		addr += 16;
		ii = 0;
		while (tmpptr < tmpend)
		{
			fprintf(file, "%02x ", *(uint8_t *)tmpptr);
			tmpptr++;
			ii++;
		}
		for (; ii < 16; ii++)
		{
			fprintf(file, "   ");
		}

		// Output the ASCII codes
		tmpptr = pBuf;
		while (tmpptr < tmpend)
		{
			if ((' ' > *(uint8_t *)tmpptr) || (0x7f < *(uint8_t *)tmpptr))
			{
				fprintf(file, ".");
			}
			else
			{
				fprintf(file, "%c", *(uint8_t *)tmpptr);
			}
			tmpptr++;
		}
		fprintf(file, "\n");

		pBuf = tmpend;
	}
}

//=============================================================================
int16_t
emfio_dumpCommand(int command, const char * fileName)
{
	int16_t localErrorCode = EMFIO_OK;
	unsigned char * lastCommandBuffer = 0;
	uint32_t lastCommandSize = 0;

	// Clear the error code.
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
		if (pCommand->commandReceived != 1)
		{
			localErrorCode = EMFIO_NO_COMMAND_RECEIVED;
		}
		else
		{
			// Allocate some memory to hold the last received command.
			lastCommandSize = pCommand->lastCommandSize;
			lastCommandBuffer = (unsigned char *)malloc(lastCommandSize);

			// Copy the buffer.
			memcpy(lastCommandBuffer, pCommand->lastCommandBuffer, lastCommandSize);
		}
	}
	pthread_mutex_unlock(&pCommand->mutex);

	// Only dump if we were successful.
	if (localErrorCode != EMFIO_OK)
	{
		emfio_setErrorCode(localErrorCode);
		return -1;
	}

	// Open the file, truncating to zero size as necessary.
	FILE * file = fopen(fileName, "w");
	if (file == 0)
	{
		emfio_setErrorCode(EMFIO_OPEN_FAILED);
		free(lastCommandBuffer);
		return -1;
	}

	// Output the buffer.
	outputHex(file, lastCommandBuffer, lastCommandSize);

	// Close the file and then release the buffer.
	fclose(file);
	free(lastCommandBuffer);

	return 0;
}

//=============================================================================
int16_t
emfio_showCommand(int command, FILE * file)
{
	int16_t localErrorCode = EMFIO_OK;
	unsigned char * lastCommandBuffer = 0;
	uint32_t lastCommandSize = 0;

	// Clear the error code.
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
		if (pCommand->commandReceived != 1)
		{
			localErrorCode = EMFIO_NO_COMMAND_RECEIVED;
		}
		else
		{
			// Allocate some memory to hold the last received command.
			lastCommandSize = pCommand->lastCommandSize;
			lastCommandBuffer = (unsigned char *)malloc(lastCommandSize);

			// Copy the buffer.
			memcpy(lastCommandBuffer, pCommand->lastCommandBuffer, lastCommandSize);
		}
	}
	pthread_mutex_unlock(&pCommand->mutex);

	// Only show if we were successful.
	if (localErrorCode != EMFIO_OK)
	{
		emfio_setErrorCode(localErrorCode);
		return -1;
	}

	// Output the buffer.
	outputHex(file, lastCommandBuffer, lastCommandSize);

	// Release the buffer.
	free(lastCommandBuffer);

	return 0;
}

//=============================================================================
int16_t
emfio_compareCommand(int command, const unsigned char * buffer, uint32_t bufSize, uint8_t * compareResults)
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
printf("\nemfio_compareCommand %d, bufsize %d, cmdsize %d\n", command, bufSize, pCommand->lastCommandSize);
		// Make sure that we received a command first.
		if (pCommand->commandReceived == 0)
		{
			localErrorCode = EMFIO_NO_COMMAND_RECEIVED;
		}
		else
		{
			// Check the size.
			if (pCommand->lastCommandSize != bufSize)
			{
				localErrorCode = EMFIO_BAD_SIZE;
			}
			else
			{
				if (memcmp(buffer, pCommand->lastCommandBuffer, bufSize) == 0)
				{
					*compareResults = 1;
				}
				else
				{
					*compareResults = 0;
				}
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

//=============================================================================
int16_t
emfio_loadCommand(int command, const unsigned char * buffer, uint32_t bufSize)
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
		// Check the size for statically-size commands.
		if ((pCommand->commandVariableLength == 0) &&
			(pCommand->maxCommandSize != bufSize))
		{
			localErrorCode = EMFIO_BAD_SIZE;
		}
		else if ((pCommand->commandVariableLength == 1) &&
				 (pCommand->maxCommandSize < bufSize))
		{
			localErrorCode = EMFIO_BAD_SIZE;
		}
		else
		{
			// Copy the buffer.
			memcpy(pCommand->lastCommandBuffer, buffer, bufSize);
			pCommand->commandReceived = 1;
			pCommand->lastCommandSize = bufSize;
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

//=============================================================================
int16_t
emfio_getLastCommandSize(int command, uint32_t * commandSize)
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
		// Make sure that we received a command first.
		if (pCommand->commandReceived == 0)
		{
			localErrorCode = EMFIO_NO_COMMAND_RECEIVED;
		}
		else
		{
			*commandSize = pCommand->lastCommandSize;
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

//=============================================================================
int16_t
emfio_setResponse(int response, const unsigned char *buffer, uint32_t bufSize, uint32_t delay)
{
	int16_t localErrorCode = EMFIO_OK;

	// Clear the error code.
	emfio_setErrorCode(localErrorCode);

	int16_t index = (int16_t)response;
	if ((index < EMFIO_SDLC_RESPONSE_OFFSET) ||
		(index > (EMFIO_SDLC_RESPONSE_OFFSET + EMFIO_MAX_NUM_COMMANDS)))
	{
		emfio_setErrorCode(EMFIO_BAD_RESPONSE);
		return -1;
	}
	SDLCResponseInfo *pResponse = &emfio_responses[index - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pResponse->initialized == 0)
	{
		emfio_setErrorCode(EMFIO_BAD_RESPONSE);
		return -1;
	}
	if (pResponse->responseBuffer == 0)
	{
		emfio_setErrorCode(EMFIO_NO_RESPONSE_EXPECTED);
		return -1;
	}
	pthread_mutex_lock(&pResponse->mutex);
	{
		// Check the size.
		if ((pResponse->responseVariableLength == 0) &&
			(pResponse->responseAllocSize != bufSize))
		{
			localErrorCode = EMFIO_BAD_SIZE;
		}
		else if (pResponse->responseAllocSize < bufSize)
		{
			localErrorCode = EMFIO_BAD_SIZE;
		}
		else
		{
			// Copy the buffer.
			memcpy(pResponse->responseBuffer, buffer, bufSize);
			pResponse->responseSize = bufSize;
			pResponse->responsePreloaded = 1;
			pResponse->responseDelay = delay;
		}
	}
	pthread_mutex_unlock(&pResponse->mutex);

	// Was an error detected?
	if (localErrorCode != EMFIO_OK)
	{
		emfio_setErrorCode(localErrorCode);
		return -1;
	}

	return 0;
}

//=============================================================================
int16_t
emfio_getResponse(int response, unsigned char *buffer, uint32_t bufSize, uint32_t *responseSize)
{
	int16_t localErrorCode = EMFIO_OK;

	emfio_setErrorCode(localErrorCode);

	int16_t index = (int16_t)response;
	if ((index < EMFIO_SDLC_RESPONSE_OFFSET) ||
		(index > (EMFIO_SDLC_RESPONSE_OFFSET + EMFIO_MAX_NUM_COMMANDS)))
	{
		emfio_setErrorCode(EMFIO_BAD_RESPONSE);
		return -1;
	}
	SDLCResponseInfo * pResponse = &emfio_responses[index - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pResponse->initialized == 0)
	{
		emfio_setErrorCode(EMFIO_BAD_RESPONSE);
		return -1;
	}
	if (pResponse->responseBuffer == 0)
	{
		emfio_setErrorCode(EMFIO_NO_RESPONSE_EXPECTED);
		return -1;
	}
	if (pResponse->responseDelay) {
		usleep(pResponse->responseDelay);
	}
	pthread_mutex_lock(&pResponse->mutex);
	{
		if (pResponse->responsePreloaded == 0)
		{
			localErrorCode = EMFIO_NO_RESPONSE_PRELOADED;
		}
		else
		{
			// Check the size.
			if (pResponse->responseSize > bufSize)
			{
				localErrorCode = EMFIO_BAD_SIZE;
			}
			else
			{
				// Copy the buffer.
				memcpy((void *)buffer, pResponse->responseBuffer, pResponse->responseSize);
				*responseSize = pResponse->responseSize;
			}
		}
	}
	pthread_mutex_unlock(&pResponse->mutex);

	// Was an error detected?
	if (localErrorCode != EMFIO_OK)
	{
		emfio_setErrorCode(localErrorCode);
		return -1;
	}

	return 0;
}

//=============================================================================
int16_t
emfio_getResponseSize(int response, uint32_t *responseSize)
{
	int16_t localErrorCode = EMFIO_OK;

	int16_t index = (int16_t)response;
	if ((index < EMFIO_SDLC_RESPONSE_OFFSET) ||
        (index > (EMFIO_SDLC_RESPONSE_OFFSET + EMFIO_MAX_NUM_COMMANDS)))
	{
		emfio_setErrorCode(EMFIO_BAD_RESPONSE);
		return -1;
	}
	SDLCResponseInfo * pResponse = &emfio_responses[index - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pResponse->initialized == 0)
	{
		emfio_setErrorCode(EMFIO_BAD_RESPONSE);
		return -1;
	}
	if (pResponse->responseBuffer == 0)
	{
		emfio_setErrorCode(EMFIO_NO_RESPONSE_EXPECTED);
		return -1;
	}
	pthread_mutex_lock(&pResponse->mutex);
	{
		if (pResponse->responsePreloaded == 0)
		{
			localErrorCode = EMFIO_NO_RESPONSE_PRELOADED;
		}
		else
		{
			*responseSize = pResponse->responseSize;
		}
	}
	pthread_mutex_unlock(&pResponse->mutex);

	// Was an error detected?
	if (localErrorCode != EMFIO_OK)
	{
		emfio_setErrorCode(localErrorCode);
		return -1;
	}

	return 0;
}
