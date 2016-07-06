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
 * \file emfio_setupCommands.cpp
 *
 * (c) Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief This file contains the setup and tear down functions for the
 *        command structures.
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
#include <stdlib.h>			// calloc
#include <string.h>			// memset
#include <pthread.h>		// mutexes
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
 * \brief Sets up the command structures.
 *
 * \return \c 0 upon success, \c -1 otherwise.
 */
int16_t emfio_setupCommands()
{
	SDLCCommandInfo * pCommand = 0;

	// LOAD_SWITCH_DRIVERS_COMMAND.
	pCommand = &emfio_commands[LOAD_SWITCH_DRIVERS_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = LOAD_SWITCH_DRIVERS_COMMAND;
	pCommand->maxCommandSize = LOAD_SWITCH_DRIVERS_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// MMU_INPUTS_STATUS_REQUEST_COMMAND.
	pCommand = &emfio_commands[MMU_INPUTS_STATUS_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = MMU_INPUTS_STATUS_REQUEST_COMMAND;
	pCommand->maxCommandSize = MMU_INPUTS_STATUS_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// MMU_PROGRAMMING_REQUEST_COMMAND.
	pCommand = &emfio_commands[MMU_PROGRAMMING_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = MMU_PROGRAMMING_REQUEST_COMMAND;
	pCommand->maxCommandSize = MMU_PROGRAMMING_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DATE_AND_TIME_BROADCAST_COMMAND.
	pCommand = &emfio_commands[DATE_AND_TIME_BROADCAST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->commandType = DATE_AND_TIME_BROADCAST_COMMAND;
	pCommand->maxCommandSize = DATE_AND_TIME_BROADCAST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// BIU_1_OUTPUTS_INPUTS_REQUEST_COMMAND.
	pCommand = &emfio_commands[BIU_1_OUTPUTS_INPUTS_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = BIU_1_OUTPUTS_INPUTS_REQUEST_COMMAND;
	pCommand->maxCommandSize = BIU_1_OUTPUTS_INPUTS_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// BIU_2_OUTPUTS_INPUTS_REQUEST_COMMAND.
	pCommand = &emfio_commands[BIU_2_OUTPUTS_INPUTS_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = BIU_2_OUTPUTS_INPUTS_REQUEST_COMMAND;
	pCommand->maxCommandSize = BIU_2_OUTPUTS_INPUTS_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// BIU_3_OUTPUTS_INPUTS_REQUEST_COMMAND.
	pCommand = &emfio_commands[BIU_3_OUTPUTS_INPUTS_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = BIU_3_OUTPUTS_INPUTS_REQUEST_COMMAND;
	pCommand->maxCommandSize = BIU_3_OUTPUTS_INPUTS_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// BIU_4_OUTPUTS_INPUTS_REQUEST_COMMAND.
	pCommand = &emfio_commands[BIU_4_OUTPUTS_INPUTS_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = BIU_4_OUTPUTS_INPUTS_REQUEST_COMMAND;
	pCommand->maxCommandSize = BIU_4_OUTPUTS_INPUTS_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// OUTPUT_TRANSFER_FRAME_COMMAND.
	pCommand = &emfio_commands[OUTPUT_TRANSFER_FRAME_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = OUTPUT_TRANSFER_FRAME_COMMAND;
	pCommand->maxCommandSize = OUTPUT_TRANSFER_FRAME_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_1_CALL_DATA_REQUEST_COMMAND.
	pCommand = &emfio_commands[DR_BIU_1_CALL_DATA_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = DR_BIU_1_CALL_DATA_REQUEST_COMMAND;
	pCommand->maxCommandSize = DR_BIU_1_CALL_DATA_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_2_CALL_DATA_REQUEST_COMMAND.
	pCommand = &emfio_commands[DR_BIU_2_CALL_DATA_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = DR_BIU_2_CALL_DATA_REQUEST_COMMAND;
	pCommand->maxCommandSize = DR_BIU_2_CALL_DATA_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_3_CALL_DATA_REQUEST_COMMAND.
	pCommand = &emfio_commands[DR_BIU_3_CALL_DATA_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = DR_BIU_3_CALL_DATA_REQUEST_COMMAND;
	pCommand->maxCommandSize = DR_BIU_3_CALL_DATA_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_4_CALL_DATA_REQUEST_COMMAND.
	pCommand = &emfio_commands[DR_BIU_4_CALL_DATA_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = DR_BIU_4_CALL_DATA_REQUEST_COMMAND;
	pCommand->maxCommandSize = DR_BIU_4_CALL_DATA_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_1_RESET_DIAG_REQUEST_COMMAND.
	pCommand = &emfio_commands[DR_BIU_1_RESET_DIAG_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = DR_BIU_1_RESET_DIAG_REQUEST_COMMAND;
	pCommand->maxCommandSize = DR_BIU_1_RESET_DIAG_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_2_RESET_DIAG_REQUEST_COMMAND.
	pCommand = &emfio_commands[DR_BIU_2_RESET_DIAG_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = DR_BIU_2_RESET_DIAG_REQUEST_COMMAND;
	pCommand->maxCommandSize = DR_BIU_2_RESET_DIAG_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_3_RESET_DIAG_REQUEST_COMMAND.
	pCommand = &emfio_commands[DR_BIU_3_RESET_DIAG_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = DR_BIU_3_RESET_DIAG_REQUEST_COMMAND;
	pCommand->maxCommandSize = DR_BIU_3_RESET_DIAG_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_4_RESET_DIAG_REQUEST_COMMAND.
	pCommand = &emfio_commands[DR_BIU_4_RESET_DIAG_REQUEST_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = DR_BIU_4_RESET_DIAG_REQUEST_COMMAND;
	pCommand->maxCommandSize = DR_BIU_4_RESET_DIAG_REQUEST_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DIAGNOSTICS_COMMAND.
	pCommand = &emfio_commands[DIAGNOSTICS_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = DIAGNOSTICS_COMMAND;
	pCommand->maxCommandSize = DIAGNOSTICS_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// POLL_FOR_SERVICE_COMMAND.
	pCommand = &emfio_commands[POLL_FOR_SERVICE_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 2;
	pCommand->commandType = POLL_FOR_SERVICE_COMMAND;
	pCommand->maxCommandSize = POLL_FOR_SERVICE_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// SECONDARY_DEST_MESSAGE_COMMAND.
	pCommand = &emfio_commands[SECONDARY_DEST_MESSAGE_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 2;
	pCommand->commandType = SECONDARY_DEST_MESSAGE_COMMAND;
	pCommand->commandVariableLength = 1;
	pCommand->maxCommandSize = SECONDARY_DEST_MESSAGE_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// SECONDARY_EXCHANGE_STATUS_COMMAND.
	pCommand = &emfio_commands[SECONDARY_EXCHANGE_STATUS_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = SECONDARY_EXCHANGE_STATUS_COMMAND;
	pCommand->maxCommandSize = SECONDARY_EXCHANGE_STATUS_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// REQUEST_MODULE_STATUS_COMMAND.
	pCommand = &emfio_commands[REQUEST_MODULE_STATUS_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = REQUEST_MODULE_STATUS_COMMAND;
	pCommand->maxCommandSize = REQUEST_MODULE_STATUS_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// MILLISECOND_COUNTER_MANAGEMENT_COMMAND.
	pCommand = &emfio_commands[MILLISECOND_COUNTER_MANAGEMENT_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = MILLISECOND_COUNTER_MANAGEMENT_COMMAND;
	pCommand->maxCommandSize = MILLISECOND_COUNTER_MANAGEMENT_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CONFIGURE_INPUTS_COMMAND.
	pCommand = &emfio_commands[CONFIGURE_INPUTS_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->commandVariableLength = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = CONFIGURE_INPUTS_COMMAND;
	pCommand->maxCommandSize = CONFIGURE_INPUTS_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// POLL_RAW_INPUT_DATA_COMMAND.
	pCommand = &emfio_commands[POLL_RAW_INPUT_DATA_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = POLL_RAW_INPUT_DATA_COMMAND;
	pCommand->maxCommandSize = POLL_RAW_INPUT_DATA_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// POLL_FILTERED_INPUT_DATA_COMMAND.
	pCommand = &emfio_commands[POLL_FILTERED_INPUT_DATA_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = POLL_FILTERED_INPUT_DATA_COMMAND;
	pCommand->maxCommandSize = POLL_FILTERED_INPUT_DATA_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// POLL_INPUT_TRANSITION_BUFFER_COMMAND.
	pCommand = &emfio_commands[POLL_INPUT_TRANSITION_BUFFER_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = POLL_INPUT_TRANSITION_BUFFER_COMMAND;
	pCommand->maxCommandSize = POLL_INPUT_TRANSITION_BUFFER_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// SET_OUTPUTS_COMMAND.
	pCommand = &emfio_commands[SET_OUTPUTS_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = SET_OUTPUTS_COMMAND;
	pCommand->maxCommandSize = SET_OUTPUTS_COMMAND_SIZE;
	pCommand->commandVariableLength = 1;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CONFIGURE_INPUT_TRACKING_FUNCS_COMMAND.
	pCommand = &emfio_commands[CONFIGURE_INPUT_TRACKING_FUNCS_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->commandVariableLength = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = CONFIGURE_INPUT_TRACKING_FUNCS_COMMAND;
	pCommand->maxCommandSize = CONFIGURE_INPUT_TRACKING_FUNCS_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CONFIGURE_COMPLEX_OUTPUT_FUNCS_COMMAND.
	pCommand = &emfio_commands[CONFIGURE_COMPLEX_OUTPUT_FUNCS_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->commandVariableLength = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = CONFIGURE_COMPLEX_OUTPUT_FUNCS_COMMAND;
	pCommand->maxCommandSize = CONFIGURE_COMPLEX_OUTPUT_FUNCS_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CONFIGURE_WATCHDOG_COMMAND.
	pCommand = &emfio_commands[CONFIGURE_WATCHDOG_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = CONFIGURE_WATCHDOG_COMMAND;
	pCommand->maxCommandSize = CONFIGURE_WATCHDOG_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// IO_MODULE_IDENTICATION_COMMAND.
	pCommand = &emfio_commands[IO_MODULE_IDENTICATION_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = IO_MODULE_IDENTICATION_COMMAND;
	pCommand->maxCommandSize = IO_MODULE_IDENTICATION_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CMU_SWITCH_PACK_DRIVERS_COMMAND.
	pCommand = &emfio_commands[CMU_SWITCH_PACK_DRIVERS_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = CMU_SWITCH_PACK_DRIVERS_COMMAND;
	pCommand->maxCommandSize = CMU_SWITCH_PACK_DRIVERS_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CMU_GET_CONFIGURATION_COMMAND.
	pCommand = &emfio_commands[CMU_GET_CONFIGURATION_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = CMU_GET_CONFIGURATION_COMMAND;
	pCommand->maxCommandSize = CMU_GET_CONFIGURATION_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CMU_SWITCH_PACK_DRIVERS_SHORT_STATUS_COMMAND.
	pCommand = &emfio_commands[CMU_SWITCH_PACK_DRIVERS_SHORT_STATUS_COMMAND];
	if (pthread_mutex_init(&pCommand->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pCommand->initialized = 1;
	pCommand->possibleResponses = 1;
	pCommand->commandType = CMU_SWITCH_PACK_DRIVERS_SHORT_STATUS_COMMAND;
	pCommand->maxCommandSize = CMU_SWITCH_PACK_DRIVERS_SHORT_STATUS_COMMAND_SIZE;
	if ((pCommand->lastCommandBuffer = (unsigned char *)calloc(1, pCommand->maxCommandSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	return 0;
}

//=============================================================================
/**
 * \brief Tears down the command structures.
 *
 * \return \c 0 upon success, \c -1 otherwise.
 */
int16_t emfio_teardownCommands()
{
	SDLCCommandInfo * pCommand = 0;
	uint32_t loopIndex;

	if (emfio_commands != 0)
	{
		// Free any allocated buffers.
		for (loopIndex = 0; loopIndex < EMFIO_MAX_NUM_COMMANDS; loopIndex++)
		{
			pCommand = &emfio_commands[loopIndex];
			if (pCommand->initialized != 0)
			{
				free(pCommand->lastCommandBuffer);
			}
			memset(pCommand, '\0', sizeof(SDLCCommandInfo));
		}
		free(emfio_commands);
		emfio_commands = 0;
	}

	return 0;
}

//=============================================================================
/**
 * \brief Sets up the response structures.
 *
 * \return \c 0 upon success, \c -1 otherwise.
 */
int16_t emfio_setupResponses()
{
	SDLCResponseInfo *pResponse = NULL;

	// LOAD_SWITCH_DRIVERS_RESPONSE.
	pResponse = &emfio_responses[LOAD_SWITCH_DRIVERS_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = LOAD_SWITCH_DRIVERS_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = LOAD_SWITCH_DRIVERS_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// MMU_INPUTS_STATUS_REQUEST_RESPONSE.
	pResponse = &emfio_responses[MMU_INPUTS_STATUS_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = MMU_INPUTS_STATUS_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = MMU_INPUTS_STATUS_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// MMU_PROGRAMMING_REQUEST_RESPONSE.
	pResponse = &emfio_responses[MMU_PROGRAMMING_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = MMU_PROGRAMMING_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = MMU_PROGRAMMING_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// BIU_1_OUTPUTS_INPUTS_REQUEST_RESPONSE.
	pResponse = &emfio_responses[BIU_1_OUTPUTS_INPUTS_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = BIU_1_OUTPUTS_INPUTS_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = BIU_1_OUTPUTS_INPUTS_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// BIU_2_OUTPUTS_INPUTS_REQUEST_RESPONSE.
	pResponse = &emfio_responses[BIU_2_OUTPUTS_INPUTS_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = BIU_2_OUTPUTS_INPUTS_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = BIU_2_OUTPUTS_INPUTS_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// BIU_3_OUTPUTS_INPUTS_REQUEST_RESPONSE.
	pResponse = &emfio_responses[BIU_3_OUTPUTS_INPUTS_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = BIU_3_OUTPUTS_INPUTS_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = BIU_3_OUTPUTS_INPUTS_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// BIU_4_OUTPUTS_INPUTS_REQUEST_RESPONSE.
	pResponse = &emfio_responses[BIU_4_OUTPUTS_INPUTS_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = BIU_4_OUTPUTS_INPUTS_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = BIU_4_OUTPUTS_INPUTS_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_1_CALL_DATA_REQUEST_RESPONSE.
	pResponse = &emfio_responses[DR_BIU_1_CALL_DATA_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = DR_BIU_1_CALL_DATA_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = DR_BIU_1_CALL_DATA_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_2_CALL_DATA_REQUEST_RESPONSE.
	pResponse = &emfio_responses[DR_BIU_2_CALL_DATA_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = DR_BIU_2_CALL_DATA_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = DR_BIU_2_CALL_DATA_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_3_CALL_DATA_REQUEST_RESPONSE.
	pResponse = &emfio_responses[DR_BIU_3_CALL_DATA_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = DR_BIU_3_CALL_DATA_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = DR_BIU_3_CALL_DATA_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_4_CALL_DATA_REQUEST_RESPONSE.
	pResponse = &emfio_responses[DR_BIU_4_CALL_DATA_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = DR_BIU_4_CALL_DATA_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = DR_BIU_4_CALL_DATA_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_1_RESET_DIAG_REQUEST_RESPONSE.
	pResponse = &emfio_responses[DR_BIU_1_RESET_DIAG_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = DR_BIU_1_RESET_DIAG_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = DR_BIU_1_RESET_DIAG_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_2_RESET_DIAG_REQUEST_RESPONSE.
	pResponse = &emfio_responses[DR_BIU_2_RESET_DIAG_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = DR_BIU_2_RESET_DIAG_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = DR_BIU_2_RESET_DIAG_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_3_RESET_DIAG_REQUEST_RESPONSE.
	pResponse = &emfio_responses[DR_BIU_3_RESET_DIAG_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = DR_BIU_3_RESET_DIAG_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = DR_BIU_3_RESET_DIAG_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DR_BIU_4_RESET_DIAG_REQUEST_RESPONSE.
	pResponse = &emfio_responses[DR_BIU_4_RESET_DIAG_REQUEST_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = DR_BIU_4_RESET_DIAG_REQUEST_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = DR_BIU_4_RESET_DIAG_REQUEST_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// DIAGNOSTICS_RESPONSE.
	pResponse = &emfio_responses[DIAGNOSTICS_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = DIAGNOSTICS_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = DIAGNOSTICS_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// POLL_FOR_SERVICE_NO_SERVICE_RESPONSE.
	pResponse = &emfio_responses[POLL_FOR_SERVICE_NO_SERVICE_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = POLL_FOR_SERVICE_NO_SERVICE_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = POLL_FOR_SERVICE_NO_SERVICE_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// SECONDARY_DEST_MESSAGE_NACK_RESPONSE.
	pResponse = &emfio_responses[SECONDARY_DEST_MESSAGE_NACK_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = SECONDARY_DEST_MESSAGE_NACK_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = SECONDARY_DEST_MESSAGE_NACK_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// SECONDARY_EXCHANGE_STATUS_RESPONSE.
	pResponse = &emfio_responses[SECONDARY_EXCHANGE_STATUS_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = SECONDARY_EXCHANGE_STATUS_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = SECONDARY_EXCHANGE_STATUS_RESPONSE_SIZE;
	pResponse->responseDelay = 500;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// REQUEST_MODULE_STATUS_RESPONSE.
	pResponse = &emfio_responses[REQUEST_MODULE_STATUS_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = REQUEST_MODULE_STATUS_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = REQUEST_MODULE_STATUS_RESPONSE_SIZE;
	pResponse->responseDelay = 0;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// MILLISECOND_COUNTER_MANAGEMENT_RESPONSE.
	pResponse = &emfio_responses[MILLISECOND_COUNTER_MANAGEMENT_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = MILLISECOND_COUNTER_MANAGEMENT_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = MILLISECOND_COUNTER_MANAGEMENT_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CONFIGURE_INPUTS_RESPONSE.
	pResponse = &emfio_responses[CONFIGURE_INPUTS_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = CONFIGURE_INPUTS_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = CONFIGURE_INPUTS_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// POLL_RAW_INPUT_DATA_RESPONSE.
	pResponse = &emfio_responses[POLL_RAW_INPUT_DATA_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseVariableLength = 1;
	pResponse->responseType = POLL_RAW_INPUT_DATA_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = POLL_RAW_INPUT_DATA_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// POLL_FILTERED_INPUT_DATA_RESPONSE.
	pResponse = &emfio_responses[POLL_FILTERED_INPUT_DATA_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseVariableLength = 1;
	pResponse->responseType = POLL_FILTERED_INPUT_DATA_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = POLL_FILTERED_INPUT_DATA_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// POLL_INPUT_TRANSITION_BUFFER_RESPONSE.
	pResponse = &emfio_responses[POLL_INPUT_TRANSITION_BUFFER_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseVariableLength = 1;
	pResponse->responseType = POLL_INPUT_TRANSITION_BUFFER_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = POLL_INPUT_TRANSITION_BUFFER_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// SET_OUTPUTS_RESPONSE.
	pResponse = &emfio_responses[SET_OUTPUTS_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = SET_OUTPUTS_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = SET_OUTPUTS_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CONFIGURE_INPUT_TRACKING_FUNCS_RESPONSE.
	pResponse = &emfio_responses[CONFIGURE_INPUT_TRACKING_FUNCS_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = CONFIGURE_INPUT_TRACKING_FUNCS_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = CONFIGURE_INPUT_TRACKING_FUNCS_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CONFIGURE_COMPLEX_OUTPUT_FUNCS_RESPONSE.
	pResponse = &emfio_responses[CONFIGURE_COMPLEX_OUTPUT_FUNCS_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = CONFIGURE_COMPLEX_OUTPUT_FUNCS_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = CONFIGURE_COMPLEX_OUTPUT_FUNCS_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CONFIGURE_WATCHDOG_RESPONSE.
	pResponse = &emfio_responses[CONFIGURE_WATCHDOG_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = CONFIGURE_WATCHDOG_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = CONFIGURE_WATCHDOG_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// IO_MODULE_IDENTICATION_RESPONSE.
	pResponse = &emfio_responses[IO_MODULE_IDENTICATION_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = IO_MODULE_IDENTICATION_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = IO_MODULE_IDENTICATION_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CMU_SWITCH_PACK_DRIVERS_RESPONSE.
	pResponse = &emfio_responses[CMU_SWITCH_PACK_DRIVERS_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = CMU_SWITCH_PACK_DRIVERS_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = CMU_SWITCH_PACK_DRIVERS_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CMU_GET_CONFIGURATION_RESPONSE.
	pResponse = &emfio_responses[CMU_GET_CONFIGURATION_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = CMU_GET_CONFIGURATION_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = CMU_GET_CONFIGURATION_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	// CMU_SWITCH_PACK_DRIVERS_SHORT_STATUS_RESPONSE.
	pResponse = &emfio_responses[CMU_SWITCH_PACK_DRIVERS_SHORT_STATUS_RESPONSE - EMFIO_SDLC_RESPONSE_OFFSET];
	if (pthread_mutex_init(&pResponse->mutex, 0) == -1)
	{
		emfio_setErrorCode(EMFIO_MUTEX_INIT_FAILED);
		return -1;
	}
	pResponse->initialized = 1;
	pResponse->responseType = CMU_SWITCH_PACK_DRIVERS_SHORT_STATUS_RESPONSE;
	pResponse->responseSize = pResponse->responseAllocSize = CMU_SWITCH_PACK_DRIVERS_SHORT_STATUS_RESPONSE_SIZE;
	pResponse->responseDelay = 1000;
	if ((pResponse->responseBuffer = (unsigned char *)calloc(1, pResponse->responseAllocSize)) == 0)
	{
		emfio_setErrorCode(EMFIO_ALLOC_FAILED);
		return -1;
	}

	return 0;
}

//=============================================================================
/**
 * \brief Tears down the response structures.
 *
 * \return \c 0 upon success, \c -1 otherwise.
 */
int16_t emfio_teardownResponses()
{
	SDLCResponseInfo * pResponse = 0;
	uint32_t loopIndex;

	if (emfio_responses != 0)
	{
		// Free any allocated buffers.
		for (loopIndex = 0; loopIndex < EMFIO_MAX_NUM_COMMANDS; loopIndex++)
		{
			pResponse = &emfio_responses[loopIndex];
			if (pResponse->initialized != 0)
			{
				free(pResponse->responseBuffer);
			}
			memset(pResponse, '\0', sizeof(SDLCResponseInfo));
		}
		free(emfio_responses);
		emfio_responses = 0;
	}

	return 0;
}
