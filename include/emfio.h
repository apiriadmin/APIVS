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
 * \file emfio.h
 *
 * (c) Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief This file defines the structures and constants for the FIO
 *        emulator library.
 *
 * \date   12/22/2010
 *
 * \author Steve Kapp
 *
 * \version 1.0
 *****************************************************************************/

#ifndef emfio_h
#define emfio_h

//=============================================================================
/**
 * Includes
 */
#include	<stdio.h>		// STD I/O Definitions
#include	<stdint.h>		// STD IEEE Type Definitions - int16_t, uint8_t, etc.
#include	<time.h> 		// UNIX time() function
#include	<pthread.h> 	// mutex functions

//=============================================================================
/**
 * Local Defines
 */

#define DEBUG_ON            1     //  = 1 for DEBUG mode, = 0 for production 

#undef  TRUE                      // prevent conflicts with other include files
#define TRUE                1      
#undef  FALSE                     // prevent conflicts with other include files 
#define FALSE               0

#define OFF                 0
#define ON                  1

#define SAFE_STRING(x)		((x == 0) ? "" : x)

//   Thread constants
#define EM_NUM_THREADS      1

#define EMFIO_MAX_NUM_COMMANDS	(128)

//    Error Codes 
#define EMFIO_OK           			((int16_t)0)
#define EMFIO_ALLOC_FAILED 		    ((int16_t)-1)
#define EMFIO_OPEN_FAILED   		((int16_t)-2)
#define EMFIO_READ_FAILED   		((int16_t)-3)
#define EMFIO_WRITE_FAILED  		((int16_t)-4)
#define EMFIO_BAD_SIZE  			((int16_t)-5)
#define EMFIO_BAD_COMMAND			((int16_t)-6)
#define EMFIO_NO_COMMAND_RECEIVED	((int16_t)-7)
#define EMFIO_NO_RESPONSE_EXPECTED	((int16_t)-8)
#define EMFIO_NO_RESPONSE_PRELOADED	((int16_t)-9)
#define EMFIO_NOT_IMPLEMENTED		((int16_t)-10)
#define EMFIO_NOT_ENOUGH_TIME		((int16_t)-11)
#define EMFIO_COUNT_NOT_BEGUN		((int16_t)-12)
#define EMFIO_COUNT_NOT_TERMINATED	((int16_t)-13)
#define EMFIO_MUTEX_INIT_FAILED		((int16_t)-14)
#define EMFIO_BAD_RESPONSE			((int16_t)-15)
#define EMFIO_LAST_ERRNUM  			EMFIO_BAD_RESPONSE
#define EMFIO_MAX_ERRNUM  			(EMFIO_LAST_ERRNUM * (-1) )   //  gives a positive value for array length

#define EMFIO_SDLC_RESPONSE_OFFSET	(128)

#define LOAD_SWITCH_DRIVERS_COMMAND 			(0)
#define MMU_INPUTS_STATUS_REQUEST_COMMAND		(1)
#define MMU_PROGRAMMING_REQUEST_COMMAND			(3)
#define DATE_AND_TIME_BROADCAST_COMMAND			(9)
#define BIU_1_OUTPUTS_INPUTS_REQUEST_COMMAND	(10)
#define BIU_2_OUTPUTS_INPUTS_REQUEST_COMMAND	(11)
#define BIU_3_OUTPUTS_INPUTS_REQUEST_COMMAND	(12)
#define BIU_4_OUTPUTS_INPUTS_REQUEST_COMMAND	(13)
#define OUTPUT_TRANSFER_FRAME_COMMAND			(18)
#define DR_BIU_1_CALL_DATA_REQUEST_COMMAND		(20)
#define DR_BIU_2_CALL_DATA_REQUEST_COMMAND		(21)
#define DR_BIU_3_CALL_DATA_REQUEST_COMMAND		(22)
#define DR_BIU_4_CALL_DATA_REQUEST_COMMAND		(23)
#define DR_BIU_1_RESET_DIAG_REQUEST_COMMAND		(24)
#define DR_BIU_2_RESET_DIAG_REQUEST_COMMAND		(25)
#define DR_BIU_3_RESET_DIAG_REQUEST_COMMAND		(26)
#define DR_BIU_4_RESET_DIAG_REQUEST_COMMAND		(27)
#define DIAGNOSTICS_COMMAND						(30)
#define POLL_FOR_SERVICE_COMMAND				(40)
#define SECONDARY_DEST_MESSAGE_COMMAND			(42)
#define SECONDARY_EXCHANGE_STATUS_COMMAND		(43)

#define REQUEST_MODULE_STATUS_COMMAND			(49)
#define MILLISECOND_COUNTER_MANAGEMENT_COMMAND	(50)
#define CONFIGURE_INPUTS_COMMAND				(51)
#define POLL_RAW_INPUT_DATA_COMMAND				(52)
#define POLL_FILTERED_INPUT_DATA_COMMAND		(53)
#define POLL_INPUT_TRANSITION_BUFFER_COMMAND	(54)
#define SET_OUTPUTS_COMMAND						(55)
#define CONFIGURE_INPUT_TRACKING_FUNCS_COMMAND	(56)
#define CONFIGURE_COMPLEX_OUTPUT_FUNCS_COMMAND	(57)
#define CONFIGURE_WATCHDOG_COMMAND				(58)
#define IO_MODULE_IDENTICATION_COMMAND			(60)

#define LOAD_SWITCH_DRIVERS_RESPONSE			(LOAD_SWITCH_DRIVERS_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define MMU_INPUTS_STATUS_REQUEST_RESPONSE		(MMU_INPUTS_STATUS_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define MMU_PROGRAMMING_REQUEST_RESPONSE		(MMU_PROGRAMMING_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define BIU_1_OUTPUTS_INPUTS_REQUEST_RESPONSE	(BIU_1_OUTPUTS_INPUTS_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define BIU_2_OUTPUTS_INPUTS_REQUEST_RESPONSE	(BIU_2_OUTPUTS_INPUTS_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define BIU_3_OUTPUTS_INPUTS_REQUEST_RESPONSE	(BIU_3_OUTPUTS_INPUTS_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define BIU_4_OUTPUTS_INPUTS_REQUEST_RESPONSE	(BIU_4_OUTPUTS_INPUTS_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define DR_BIU_1_CALL_DATA_REQUEST_RESPONSE		(DR_BIU_1_CALL_DATA_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define DR_BIU_2_CALL_DATA_REQUEST_RESPONSE		(DR_BIU_2_CALL_DATA_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define DR_BIU_3_CALL_DATA_REQUEST_RESPONSE		(DR_BIU_3_CALL_DATA_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define DR_BIU_4_CALL_DATA_REQUEST_RESPONSE		(DR_BIU_4_CALL_DATA_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define DR_BIU_1_RESET_DIAG_REQUEST_RESPONSE	(DR_BIU_1_RESET_DIAG_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define DR_BIU_2_RESET_DIAG_REQUEST_RESPONSE	(DR_BIU_2_RESET_DIAG_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define DR_BIU_3_RESET_DIAG_REQUEST_RESPONSE	(DR_BIU_3_RESET_DIAG_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define DR_BIU_4_RESET_DIAG_REQUEST_RESPONSE	(DR_BIU_4_RESET_DIAG_REQUEST_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define DIAGNOSTICS_RESPONSE					(DIAGNOSTICS_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define POLL_FOR_SERVICE_NO_SERVICE_RESPONSE	(POLL_FOR_SERVICE_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define POLL_FOR_SERVICE_SECONDARY_MESSAGE_RESPONSE	(POLL_FOR_SERVICE_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET + 1)
#define SECONDARY_DEST_MESSAGE_NACK_RESPONSE	(SECONDARY_DEST_MESSAGE_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define SECONDARY_DEST_MESSAGE_ACK_RESPONSE		(SECONDARY_DEST_MESSAGE_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET + 1)
#define SECONDARY_EXCHANGE_STATUS_RESPONSE		(SECONDARY_EXCHANGE_STATUS_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)

#define REQUEST_MODULE_STATUS_RESPONSE			(REQUEST_MODULE_STATUS_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define MILLISECOND_COUNTER_MANAGEMENT_RESPONSE	(MILLISECOND_COUNTER_MANAGEMENT_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define CONFIGURE_INPUTS_RESPONSE				(CONFIGURE_INPUTS_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define POLL_RAW_INPUT_DATA_RESPONSE			(POLL_RAW_INPUT_DATA_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define POLL_FILTERED_INPUT_DATA_RESPONSE		(POLL_FILTERED_INPUT_DATA_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define POLL_INPUT_TRANSITION_BUFFER_RESPONSE	(POLL_INPUT_TRANSITION_BUFFER_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define SET_OUTPUTS_RESPONSE					(SET_OUTPUTS_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define CONFIGURE_INPUT_TRACKING_FUNCS_RESPONSE	(CONFIGURE_INPUT_TRACKING_FUNCS_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define CONFIGURE_COMPLEX_OUTPUT_FUNCS_RESPONSE	(CONFIGURE_COMPLEX_OUTPUT_FUNCS_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define CONFIGURE_WATCHDOG_RESPONSE				(CONFIGURE_WATCHDOG_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)
#define IO_MODULE_IDENTICATION_RESPONSE			(IO_MODULE_IDENTICATION_COMMAND + EMFIO_SDLC_RESPONSE_OFFSET)

#define LOAD_SWITCH_DRIVERS_COMMAND_SIZE 			((int32_t)14)
#define LOAD_SWITCH_DRIVERS_RESPONSE_SIZE 			((int32_t)1)

#define MMU_INPUTS_STATUS_REQUEST_COMMAND_SIZE		((int32_t)1)
#define MMU_INPUTS_STATUS_REQUEST_RESPONSE_SIZE		((int32_t)11)

#define MMU_PROGRAMMING_REQUEST_COMMAND_SIZE		((int32_t)1)
#define MMU_PROGRAMMING_REQUEST_RESPONSE_SIZE		((int32_t)21)

#define DATE_AND_TIME_BROADCAST_COMMAND_SIZE		((int32_t)10)
#define DATE_AND_TIME_BROADCAST_RESPONSE_SIZE		((int32_t)-1)

#define BIU_1_OUTPUTS_INPUTS_REQUEST_COMMAND_SIZE	((uint32_t)9)
#define BIU_1_OUTPUTS_INPUTS_REQUEST_RESPONSE_SIZE	((uint32_t)6)

#define BIU_2_OUTPUTS_INPUTS_REQUEST_COMMAND_SIZE	((uint32_t)9)
#define BIU_2_OUTPUTS_INPUTS_REQUEST_RESPONSE_SIZE	((uint32_t)6)

#define BIU_3_OUTPUTS_INPUTS_REQUEST_COMMAND_SIZE	((uint32_t)6)
#define BIU_3_OUTPUTS_INPUTS_REQUEST_RESPONSE_SIZE	((uint32_t)6)

#define BIU_4_OUTPUTS_INPUTS_REQUEST_COMMAND_SIZE	((uint32_t)6)
#define BIU_4_OUTPUTS_INPUTS_REQUEST_RESPONSE_SIZE	((uint32_t)6)

#define OUTPUT_TRANSFER_FRAME_COMMAND_SIZE			((int32_t)1)
#define OUTPUT_TRANSFER_FRAME_RESPONSE_SIZE			((int32_t)-1)

#define DR_BIU_1_CALL_DATA_REQUEST_COMMAND_SIZE		((int32_t)1)
#define DR_BIU_1_CALL_DATA_REQUEST_RESPONSE_SIZE	((int32_t)37)

#define DR_BIU_2_CALL_DATA_REQUEST_COMMAND_SIZE		((int32_t)1)
#define DR_BIU_2_CALL_DATA_REQUEST_RESPONSE_SIZE	((int32_t)37)

#define DR_BIU_3_CALL_DATA_REQUEST_COMMAND_SIZE		((int32_t)1)
#define DR_BIU_3_CALL_DATA_REQUEST_RESPONSE_SIZE	((int32_t)37)

#define DR_BIU_4_CALL_DATA_REQUEST_COMMAND_SIZE		((int32_t)1)
#define DR_BIU_4_CALL_DATA_REQUEST_RESPONSE_SIZE	((int32_t)37)

#define DR_BIU_1_RESET_DIAG_REQUEST_COMMAND_SIZE	((int32_t)2)
#define DR_BIU_1_RESET_DIAG_REQUEST_RESPONSE_SIZE	((int32_t)17)

#define DR_BIU_2_RESET_DIAG_REQUEST_COMMAND_SIZE	((int32_t)2)
#define DR_BIU_2_RESET_DIAG_REQUEST_RESPONSE_SIZE	((int32_t)17)

#define DR_BIU_3_RESET_DIAG_REQUEST_COMMAND_SIZE	((int32_t)2)
#define DR_BIU_3_RESET_DIAG_REQUEST_RESPONSE_SIZE	((int32_t)17)

#define DR_BIU_4_RESET_DIAG_REQUEST_COMMAND_SIZE	((int32_t)2)
#define DR_BIU_4_RESET_DIAG_REQUEST_RESPONSE_SIZE	((int32_t)17)

#define DIAGNOSTICS_COMMAND_SIZE					((int32_t)1)
#define DIAGNOSTICS_RESPONSE_SIZE					((int32_t)25)

#define POLL_FOR_SERVICE_COMMAND_SIZE				((int32_t)1)
#define POLL_FOR_SERVICE_NO_SERVICE_RESPONSE_SIZE	((int32_t)1)
#define POLL_FOR_SERVICE_SECONDARY_MESSAGE_RESPONSE_SIZE	((int32_t)36)

#define SECONDARY_DEST_MESSAGE_COMMAND_SIZE			((int32_t)36)
#define SECONDARY_DEST_MESSAGE_NACK_RESPONSE_SIZE	((int32_t)1)
#define SECONDARY_DEST_MESSAGE_ACK_RESPONSE_SIZE	((int32_t)1)

#define SECONDARY_EXCHANGE_STATUS_COMMAND_SIZE		((int32_t)2)
#define SECONDARY_EXCHANGE_STATUS_RESPONSE_SIZE		((int32_t)1)

#define REQUEST_MODULE_STATUS_COMMAND_SIZE			((int32_t)2)
#define REQUEST_MODULE_STATUS_RESPONSE_SIZE			((int32_t)8)

#define MILLISECOND_COUNTER_MANAGEMENT_COMMAND_SIZE	((int32_t)5)
#define MILLISECOND_COUNTER_MANAGEMENT_RESPONSE_SIZE ((int32_t)2)

#define CONFIGURE_INPUTS_COMMAND_SIZE				((int32_t)767)
#define CONFIGURE_INPUTS_RESPONSE_SIZE 				((int32_t)2)

#define POLL_RAW_INPUT_DATA_COMMAND_SIZE			((int32_t)1)
#define POLL_RAW_INPUT_DATA_RESPONSE_SIZE 			((int32_t)20)

#define POLL_FILTERED_INPUT_DATA_COMMAND_SIZE		((int32_t)1)
#define POLL_FILTERED_INPUT_DATA_RESPONSE_SIZE 		((int32_t)20)

#define POLL_INPUT_TRANSITION_BUFFER_COMMAND_SIZE	((int32_t)2)
#define POLL_INPUT_TRANSITION_BUFFER_RESPONSE_SIZE 	((int32_t)776)

#define SET_OUTPUTS_COMMAND_SIZE					((int32_t)27)
#define SET_OUTPUTS_RESPONSE_SIZE 					((int32_t)2)

#define SET_OUTPUTS_COMMAND_SIZE					((int32_t)27)
#define SET_OUTPUTS_RESPONSE_SIZE 					((int32_t)2)

#define CONFIGURE_INPUT_TRACKING_FUNCS_COMMAND_SIZE	((int32_t)34)
#define CONFIGURE_INPUT_TRACKING_FUNCS_RESPONSE_SIZE ((int32_t)6)

#define CONFIGURE_COMPLEX_OUTPUT_FUNCS_COMMAND_SIZE	((int32_t)58)
#define CONFIGURE_COMPLEX_OUTPUT_FUNCS_RESPONSE_SIZE ((int32_t)6)

#define CONFIGURE_WATCHDOG_COMMAND_SIZE				((int32_t)2)
#define CONFIGURE_WATCHDOG_RESPONSE_SIZE 			((int32_t)2)

#define IO_MODULE_IDENTICATION_COMMAND_SIZE			((int32_t)1)
#define IO_MODULE_IDENTICATION_RESPONSE_SIZE 		((int32_t)2)

typedef struct SDLCCommandInfoTag
{
	pthread_mutex_t 	mutex;
	int8_t				initialized;
	int 				commandType;

	// For keeping track of received commands.
	uint8_t				commandReceived;
	uint8_t				commandVariableLength;
	int16_t				possibleResponses;
	uint32_t			maxCommandSize;
	uint32_t			lastCommandSize;
	unsigned char * 	lastCommandBuffer;
	uint32_t			commandReceivedCounter;

	// The next three are for statistics.
	uint32_t			commandCount;
	time_t				startCountEpoch;
	time_t				endCountEpoch;
} SDLCCommandInfo;

typedef struct SDLCResponseInfoTag
{
	pthread_mutex_t 	mutex;
	int8_t				initialized;
	int					responseType;

	// For pre-loading responses
	uint8_t				responsePreloaded;
	uint8_t				responseVariableLength;
	uint32_t			responseSize;
	uint32_t			responseAllocSize;
	unsigned char * 	responseBuffer;
} SDLCResponseInfo;

//=============================================================================

/**
 * External Global Routines 
 */
extern int16_t  	emfio_start(const char * portName);
extern void     	emfio_end();

extern void     	emfio_clear();

extern int16_t  	emfio_begin(int command);
extern int16_t  	emfio_terminate(int command);
extern int16_t  	emfio_stats(int command, float * hertz);

extern int16_t  	emfio_dumpCommand(int command, const char * fileName);
extern int16_t  	emfio_showCommand(int command, FILE * file);
extern int16_t  	emfio_compareCommand(int command, const unsigned char * buffer, uint32_t bufSize, uint8_t * compareResults);
extern int16_t  	emfio_getLastCommandSize(int command, uint32_t * commandSize);
extern int16_t  	emfio_loadCommand(int command, const unsigned char * buffer, uint32_t bufSize);

extern int16_t  	emfio_setResponse(int response, const unsigned char * buffer, uint32_t bufSize);
extern int16_t  	emfio_getResponse(int response, unsigned char * buffer, uint32_t bufSize, uint32_t * responseSize);
extern int16_t  	emfio_getResponseSize(int command, uint32_t * responseSize);

extern int16_t  	emfio_getErrorCode();
extern void     	emfio_setErrorCode(int16_t newErrorCode);
extern const char * emfio_getErrorText();

#endif
