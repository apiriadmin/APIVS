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
 * \file emfio.c
 *
 * (c) Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief This file contains the main entry points and the thread
 *        function for the FIO emulator library.
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
#include <stdio.h>		// STD I/O definitions
#include <stdlib.h>		// STD memory allocation definitions
#include <string.h>		// STD string definitions
#include <unistd.h>		// sleep() function
#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>

#include   "emfio.h"
#include   "emfio_globalData.h"
#include   "emfio_setupCommands.h"

//=============================================================================
/**
 * Application Structures
 */
typedef struct threadArgs
{
	int16_t thr_threadId;
	void * thr_state;
	const char * thr_portName;
	int fd;
} EMFIO_THREAD_ARGS ;

//=============================================================================
/**
 * Extern Subroutines 
 */

//=============================================================================
/**
 * Extern Data
 */

//=============================================================================
/**
 * Local Define
 */
#define MAX_SDLC_BUFFER_SIZE		(2048)

//=============================================================================
/**
 * Global Data Variables 
 */

//=============================================================================
/**
 * Module Private Variables 
 */

static int threadErrorCode = 0;
static pthread_t thread_descriptor;		    	// thread descriptor
static EMFIO_THREAD_ARGS thread_data;           // data passed to the thread
static int16_t thread_result = 0;				// returned result from thread
static int16_t *pThreadResult = &thread_result; // pointer for the pthread_join() 
static int16_t threadNum = 0;

//=============================================================================
/**
 * \brief Thread function.
 *
 * \params[in] args A pointer to a EMFIO_THREAD_ARGS structure.
 */
int16_t
emfio_thread(EMFIO_THREAD_ARGS * args)
{
	int fd = args[0].fd;						// file descriptor for Serial Port open, read, write
	int16_t counter = 0;
	unsigned char sdlcBuffer[MAX_SDLC_BUFFER_SIZE];

#if 0/*DEBUG_ON*/
	printf("\n\nemfio_thread %d is STARTING", args[0].thr_threadId);
	printf("\n  - on port %s, fd=%d", args[0].thr_portName, fd );
#endif
	
	// Do we have a real port?
	if (fd == 0)
	{
		// Loop until we are told to stop.
		while (!emfio_allDone)
		{
			sleep(1);
		}
	}
	else     
	{        
		// Open the file.
		fd_set rfds;
		struct timeval tv;
		
		// Loop until we are told to stop.
		while (!emfio_allDone)
		{
			int retVal = 0, j = 0;
			struct pollfd fdinfo;
			fdinfo.fd = fd;
			fdinfo.events = POLLIN|POLLPRI;
			if( poll(&fdinfo,1,1000) <= 0 ) {
				retVal = -1;
			} else {
				retVal = read(fd, sdlcBuffer, MAX_SDLC_BUFFER_SIZE);
			}
#if 1
			if (retVal > 0) {
				printf("Received DATA:\n");
				for (j = 0; j < retVal; j++) {
					printf("%.2x ", sdlcBuffer[j]);
				}
				printf("\n");
			}
			printf("return value is %d\n", retVal);
#endif
			int bytesRead = retVal;
			if (bytesRead > 0)
			{
				// Get the proper command structure.
				int16_t index = (int16_t)sdlcBuffer[2];
				if ((index >= 0) || (index < EMFIO_MAX_NUM_COMMANDS))
				{
					SDLCCommandInfo * pCommand = &emfio_commands[index];
					if (pCommand->initialized != 0)
					{
						//printf("\nemfio_thread: command %d received, len=%d\n", index, bytesRead);
						// Protect the command structure.
						pthread_mutex_lock(&pCommand->mutex);
						{
							// Error checking to make sure that the length is correct.
							if (!((pCommand->commandVariableLength == 0) &&
								  (pCommand->maxCommandSize != (bytesRead-2))) &&
							    !((pCommand->commandVariableLength == 1) &&
									 (pCommand->maxCommandSize < (bytesRead-2))))
							{
								// Copy the buffer.
								memcpy(pCommand->lastCommandBuffer, &sdlcBuffer[2], (bytesRead-2));
								pCommand->commandReceived = 1;
								pCommand->lastCommandSize = bytesRead-2;
								printf("\nemfio_thread: command %d received\n", index);
							}
						}
						pthread_mutex_unlock(&pCommand->mutex);
						// Send corresponding response frame if available
						//
						//
					}
				}
			}
		}
		close(fd);
	}
	
	pthread_exit(&threadErrorCode);   // return error code to the original start function
}  // end emfio_thread()

//=============================================================================
/**
 * \brief Starts up the FIO emulator.
 *
 * \params[in] portName A pointer to the SDLC port name like "/dev/c60" or "/dev/fio",
 *                      or \c 0 if an SDLC port is not used.
 * \return \c 0 upon success, \c -1 otherwise.
 */
int16_t
emfio_start(const char * portName)
{
	int16_t res = 0;                        // holds results from sub-routine calls
	int fd = 0;

#if 0/*DEBUG_ON*/
	printf("\n emfio_start for port name %s\n ", SAFE_STRING(portName));
#endif
	
	// Initialize the error code.
	emfio_setErrorCode(EMFIO_OK);
	
	// Allocate an array of command structures.
	emfio_commands = (SDLCCommandInfo *)calloc(EMFIO_MAX_NUM_COMMANDS, sizeof(SDLCCommandInfo));
	if (emfio_commands == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// Allocate an array of response structures.
	emfio_responses = (SDLCResponseInfo *)calloc(EMFIO_MAX_NUM_COMMANDS, sizeof(SDLCResponseInfo));
	if (emfio_responses == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		emfio_teardownCommands();
		return -1;
	}

	// Initialize the commands.
	if (emfio_setupCommands() != 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		emfio_teardownCommands();
		emfio_teardownResponses();
		return -1;
	}

	// Initialize the commands.
	if (emfio_setupResponses() != 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		emfio_teardownCommands();
		emfio_teardownResponses();
		return -1;
	}

	// If we have a port, open it.
	if (portName != 0)
	{
		fd = open(portName, O_RDONLY|O_NONBLOCK, 0);
		if (fd < 0)
		{
			emfio_setErrorCode(EMFIO_OPEN_FAILED);
			emfio_teardownCommands();
			emfio_teardownResponses();
			return -1;
		}
	}

	thread_data.thr_threadId = ++threadNum;   //  notice if it is called more than once
	thread_data.thr_portName = (portName != 0) ? strdup(portName) : 0;
	thread_data.fd = fd;

	//  do all work in a thread so control can return to calling program
	res = pthread_create(&thread_descriptor, NULL, (void *) emfio_thread, &thread_data);
	
	sleep(1);
	
	return(res);
	
}  // end emfio_start()

//=============================================================================
/**
 * \brief Shuts down the FIO emulator.
 */
void
emfio_end()
{
	// Set the flag variable.
	emfio_allDone = TRUE;
	
	sleep(1);
	
#if 0/*DEBUG_ON */
	printf("\n emfio_end - looking for thread to exit");
#endif
	
	pthread_join( thread_descriptor, (void **) &pThreadResult );   // wait for it to finish 

	sleep(1); 
	
#if	0/*DEBUG_ON*/
	printf("\n Thread returned a value of %d", *pThreadResult);
#endif
	
	// Clean up.
	emfio_teardownCommands();
	emfio_teardownResponses();

	return;
}
