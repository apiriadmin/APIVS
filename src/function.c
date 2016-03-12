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
 * \file function.c
 *
 * This file contains the definition of processing <Function>s
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief function processing
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/

//=============================================================================
/**
 * Includes
 */

#include	<stdio.h>		// STD I/O Definitions
#include	<unistd.h>		// STD Unix Definitions
#include	<stdint.h>		// STD IEEE Type Definitions
#include	<stdlib.h>		// STD Library Definitions
#include	<string.h>		// STD String Definitions
#include	<errno.h>		// STD errno Definitions
#include	"vse_stnd.h"	// STD VSE Definitions
#include	"outputXML.h"	// Output XML Processor Definitions
#include	"function.h"	// Function Processor Definitions

#include	<tod.h>			// TOD Function Definitions
#include	<fpui.h>		// FPUI Function Definitions
#include	<fio.h>			// FIO Function Definitions

//=============================================================================
/**
 * Extern
 */

//=============================================================================
/**
 * Local Definitions
 */

/** \brief fpui_apiver handler */
static	int16_t	funcFpuiApiver(uint16_t		ln,
						       C_FUNC		*pCF);

/** \brief fpui_close handler */
static	int16_t	funcFpuiClose(uint16_t		ln,
						      C_FUNC		*pCF);

/** \brief fpui_close_aux_switch handler */
static	int16_t	funcFpuiCloseAuxSwitch(uint16_t		ln,
						      		   C_FUNC		*pCF);

/** \brief fpui_clear handler */
static	int16_t	funcFpuiClear(uint16_t		ln,
						      C_FUNC		*pCF);

/** \brief fpui_clear_tab handler */
static	int16_t	funcFpuiClearTab(uint16_t		ln,
								 C_FUNC			*pCF);

/** \brief fpui_compose_special_char handler */
static	int16_t	funcFpuiComposeSpecialChar(uint16_t		ln,
						      			   C_FUNC		*pCF);

/** \brief fpui_del_keymap handler */
static	int16_t	funcFpuiDelKeymap(uint16_t		ln,
						      	  C_FUNC		*pCF);

/** \brief fpui_display_special_char handler */
static	int16_t	funcFpuiDisplaySpecialChar(uint16_t		ln,
						      			   C_FUNC		*pCF);

/** \brief fpui_get_auto_repeat handler */
static	int16_t	funcFpuiGetAutoRepeat(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief fpui_get_auto_scroll handler */
static	int16_t	funcFpuiGetAutoScroll(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief fpui_get_auto_wrap handler */
static	int16_t	funcFpuiGetAutoWrap(uint16_t	ln,
						      		C_FUNC		*pCF);

/** \brief fpui_get_backlight handler */
static	int16_t	funcFpuiGetBacklight(uint16_t	ln,
						      		 C_FUNC		*pCF);

/** \brief fpui_get_character_blink handler */
static	int16_t	funcFpuiGetCharacterBlink(uint16_t		ln,
						      			  C_FUNC		*pCF);

/** \brief fpui_get_cursor handler */
static	int16_t	funcFpuiGetCursor(uint16_t		ln,
						      	  C_FUNC		*pCF);

/** \brief fpui_get_cursor_blink handler */
static	int16_t	funcFpuiGetCursorBlink(uint16_t		ln,
						      		   C_FUNC		*pCF);

/** \brief fpui_get_cursor_pos handler */
static	int16_t	funcFpuiGetCursorPos(uint16_t	ln,
						      		 C_FUNC		*pCF);

/** \brief fpui_get_focus handler */
static	int16_t	funcFpuiGetFocus(uint16_t	ln,
						      	 C_FUNC		*pCF);

/** \brief fpui_get_keymap handler */
static	int16_t	funcFpuiGetKeymap(uint16_t		ln,
						      	  C_FUNC		*pCF);

/** \brief fpui_get_led handler */
/*static	int16_t	funcFpuiGetLed(uint16_t		ln,
						       C_FUNC		*pCF);*/

/** \brief fpui_get_reverse_video handler */
static	int16_t	funcFpuiGetReverseVideo(uint16_t	ln,
						      			C_FUNC		*pCF);

/** \brief fpui_get_underline handler */
static	int16_t	funcFpuiGetUnderline(uint16_t	ln,
						      		 C_FUNC		*pCF);

/** \brief fpui_get_window_attr handler */
static	int16_t	funcFpuiGetWindowAttr(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief fpui_get_window_size handler */
static	int16_t	funcFpuiGetWindowSize(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief fpui_home handler */
static	int16_t	funcFpuiHome(uint16_t		ln,
						     C_FUNC			*pCF);

/** \brief fpui_open handler */
static	int16_t	funcFpuiOpen(uint16_t		ln,
						     C_FUNC			*pCF);

/** \brief fpui_open_aux_switch handler */
static	int16_t	funcFpuiOpenAuxSwitch(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief fpui_panel_present handler */
static	int16_t	funcFpuiPanelPresent(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief fpui_poll handler */
static	int16_t	funcFpuiPoll(uint16_t		ln,
						     C_FUNC			*pCF);

/** \brief fpui_read handler */
static	int16_t	funcFpuiRead(uint16_t		ln,
						     C_FUNC			*pCF);

/** \brief fpui_read_aux_switch handler */
static	int16_t	funcFpuiReadAuxSwitch(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief fpui_read_char handler */
static	int16_t	funcFpuiReadChar(uint16_t		ln,
						      	 C_FUNC			*pCF);

/** \brief fpui_read_string handler */
static	int16_t	funcFpuiReadString(uint16_t		ln,
						      	   C_FUNC		*pCF);

/** \brief fpui_refresh handler */
static	int16_t	funcFpuiRefresh(uint16_t		ln,
						      	C_FUNC			*pCF);

/** \brief fpui_reset_all_attributes handler */
static	int16_t	funcFpuiResetAllAttributes(uint16_t		ln,
						      		  	   C_FUNC		*pCF);

/** \brief fpui_reset_keymap handler */
static	int16_t	funcFpuiResetKeymap(uint16_t		ln,
						      		C_FUNC			*pCF);

/** \brief fpui_set_auto_repeat handler */
static	int16_t	funcFpuiSetAutoRepeat(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief fpui_set_auto_scroll handler */
static	int16_t	funcFpuiSetAutoScroll(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief fpui_set_auto_wrap handler */
static	int16_t	funcFpuiSetAutoWrap(uint16_t		ln,
						      		C_FUNC			*pCF);

/** \brief fpui_set_backlight handler */
static	int16_t	funcFpuiSetBacklight(uint16_t		ln,
						      		 C_FUNC			*pCF);

/** \brief fpui_set_backlight_timeout handler */
static	int16_t	funcFpuiSetBacklightTimeout(uint16_t		ln,
						      		  		C_FUNC			*pCF);

/** \brief fpui_set_character_blink handler */
static	int16_t	funcFpuiSetCharacterBlink(uint16_t		ln,
						      		      C_FUNC		*pCF);

/** \brief fpui_set_cursor handler */
static	int16_t	funcFpuiSetCursor(uint16_t		ln,
						      	  C_FUNC		*pCF);

/** \brief fpui_set_cursor_blink handler */
static	int16_t	funcFpuiSetCursorBlink(uint16_t		ln,
						      		   C_FUNC		*pCF);

/** \brief fpui_set_cursor_pos handler */
static	int16_t	funcFpuiSetCursorPos(uint16_t		ln,
						      		 C_FUNC			*pCF);

/** \brief fpui_set_emergency handler */
static	int16_t	funcFpuiSetEmergency(uint16_t		ln,
						      		 C_FUNC			*pCF);

/** \brief fpui_set_keymap handler */
static	int16_t	funcFpuiSetKeymap(uint16_t		ln,
						      	  C_FUNC		*pCF);

/** \brief fpui_set_led handler */
/*static	int16_t	funcFpuiSetLed(uint16_t		ln,
						       C_FUNC		*pCF);*/

/** \brief fpui_set_reverse_video handler */
static	int16_t	funcFpuiSetReverseVideo(uint16_t		ln,
						      		    C_FUNC			*pCF);

/** \brief fpui_set_tab handler */
static	int16_t	funcFpuiSetTab(uint16_t		ln,
						       C_FUNC		*pCF);

/** \brief fpui_set_underline handler */
static	int16_t	funcFpuiSetUnderline(uint16_t		ln,
						      		 C_FUNC			*pCF);

/** \brief fpui_set_window_attr handler */
static	int16_t	funcFpuiSetWindowAttr(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief fpui_write handler */
static	int16_t	funcFpuiWrite(uint16_t		ln,
						      C_FUNC		*pCF);

/** \brief fpui_write_at handler */
static	int16_t	funcFpuiWriteAt(uint16_t		ln,
						     	C_FUNC			*pCF);

/** \brief fpui_write_char handler */
static	int16_t	funcFpuiWriteChar(uint16_t		ln,
						      	  C_FUNC		*pCF);

/** \brief fpui_write_char_at handler */
static	int16_t	funcFpuiWriteCharAt(uint16_t		ln,
						      		C_FUNC			*pCF);

/** \brief fpui_write_string handler */
static	int16_t	funcFpuiWriteString(uint16_t		ln,
						      		C_FUNC			*pCF);

/** \brief fpui_write_string_at handler */
static	int16_t	funcFpuiWriteStringAt(uint16_t		ln,
						      		  C_FUNC		*pCF);

/** \brief tod_get handler */
static	int16_t	funcTodGet(uint16_t		ln,
						   C_FUNC		*pCF);

/** \brief tod_set handler */
static	int16_t	funcTodSet(uint16_t		ln,
						   C_FUNC		*pCF);

/** \brief tod_cancel_onchange_signal handler */
static	int16_t	funcTodCancelOnchangeSignal(uint16_t	ln,
						   					C_FUNC		*pCF);

/** \brief tod_cancel_tick_signal handler */
static	int16_t	funcTodCancelTickSignal(uint16_t	ln,
						   				C_FUNC		*pCF);

/** \brief tod_get_dst_info handler */
static	int16_t	funcTodGetDstInfo(uint16_t		ln,
						   		  C_FUNC		*pCF);

/** \brief tod_get_dst_state handler */
static	int16_t	funcTodGetDstState(uint16_t		ln,
						   		   C_FUNC		*pCF);

/** \brief tod_get_timesrc handler */
static	int16_t	funcTodGetTimesrc(uint16_t		ln,
						   		  C_FUNC		*pCF);

/** \brief tod_get_timesrc_freq handler */
static	int16_t	funcTodGetTimesrcFreq(uint16_t		ln,
						   			  C_FUNC		*pCF);

/** \brief tod_request_onchange_signal handler */
static	int16_t	funcTodRequestOnchangeSignal(uint16_t	ln,
						   					 C_FUNC		*pCF);

/** \brief tod_request_tick_signal handler */
static	int16_t	funcTodRequestTickSignal(uint16_t	ln,
						   				 C_FUNC		*pCF);

/** \brief tod_set_dst_info handler */
static	int16_t	funcTodSetDstInfo(uint16_t		ln,
						   		  C_FUNC		*pCF);

/** \brief tod_set_dst_state handler */
static	int16_t	funcTodSetDstState(uint16_t		ln,
						   		   C_FUNC		*pCF);

/** \brief tod_set_timesrc handler */
static	int16_t	funcTodSetTimesrc(uint16_t		ln,
						   		  C_FUNC		*pCF);

/** \brief tod_apiver handler */
static	int16_t	funcTodApiver(uint16_t	ln,
							  C_FUNC	*pCF);

/** \brief fio_apiver handler */
static	int16_t	funcFioApiver(uint16_t	ln,
							  C_FUNC	*pCF);

/** \brief fio_deregister handler */
static	int16_t	funcFioDeregister(uint16_t	ln,
								  C_FUNC	*pCF);

/** \brief fio_fiod_channel_map_count handler */
static	int16_t	funcFioFiodChannelMapCount(uint16_t	ln,
								  		   C_FUNC	*pCF);

/** \brief fio_fiod_channel_map_get handler */
static	int16_t	funcFioFiodChannelMapGet(uint16_t	ln,
								  		 C_FUNC		*pCF);

/** \brief fio_fiod_channel_map_set handler */
static	int16_t	funcFioFiodChannelMapSet(uint16_t	ln,
								  		 C_FUNC		*pCF);

/** \brief fio_fiod_channel_reservation_get handler */
static	int16_t	funcFioFiodChannelReservationGet(uint16_t	ln,
								  				 C_FUNC		*pCF);

/** \brief fio_fiod_channel_reservation_set handler */
static	int16_t	funcFioFiodChannelReservationSet(uint16_t	ln,
								  				 C_FUNC		*pCF);

/** \brief fio_fiod_cmu_dark_channel_get handler */
static	int16_t	funcFioFiodCmuDarkChannelGet(uint16_t	ln,
								  			 C_FUNC		*pCF);

/** \brief fio_fiod_cmu_dark_channel_set handler */
static	int16_t	funcFioFiodCmuDarkChannelSet(uint16_t	ln,
								  			 C_FUNC		*pCF);

/** \brief fio_fiod_cmu_fault_get handler */
static	int16_t	funcFioFiodCmuFaultGet(uint16_t	ln,
								  	   C_FUNC	*pCF);

/** \brief fio_fiod_cmu_fault_set handler */
static	int16_t	funcFioFiodCmuFaultSet(uint16_t	ln,
								  	   C_FUNC	*pCF);

/** \brief fio_fiod_cmu_config_change_count handler */
static	int16_t	funcFioFiodCmuConfigChangeCount(uint16_t	ln,
								  	   C_FUNC	*pCF);

/** \brief fio_fiod_deregister handler */
static	int16_t	funcFioFiodDeregister(uint16_t	ln,
								  	  C_FUNC	*pCF);

/** \brief fio_fiod_disable handler */
static	int16_t	funcFioFiodDisable(uint16_t	ln,
								   C_FUNC	*pCF);

/** \brief fio_fiod_enable handler */
static	int16_t	funcFioFiodEnable(uint16_t	ln,
								  C_FUNC	*pCF);

/** \brief fio_fiod_frame_notify_deregister handler */
static	int16_t	funcFioFiodFrameNotifyDeregister(uint16_t	ln,
								  				 C_FUNC		*pCF);

/** \brief fio_fiod_frame_notify_register handler */
static	int16_t	funcFioFiodFrameNotifyRegister(uint16_t	ln,
								  			   C_FUNC	*pCF);

/** \brief fio_fiod_frame_read handler */
static	int16_t	funcFioFiodFrameRead(uint16_t	ln,
								  	 C_FUNC		*pCF);

/** \brief fio_fiod_frame_write handler */
static	int16_t	funcFioFiodFrameWrite(uint16_t	ln,
								  	 C_FUNC		*pCF);

/** \brief fio_fiod_frame_schedule_get handler */
static	int16_t	funcFioFiodFrameScheduleGet(uint16_t	ln,
								  			C_FUNC		*pCF);

/** \brief fio_fiod_frame_schedule_set handler */
static	int16_t	funcFioFiodFrameScheduleSet(uint16_t	ln,
								  			C_FUNC		*pCF);

/** \brief fio_fiod_frame_size handler */
static	int16_t	funcFioFiodFrameSize(uint16_t	ln,
								  	 C_FUNC		*pCF);

/** \brief fio_fiod_inputs_filter_get handler */
static	int16_t	funcFioFiodInputsFilterGet(uint16_t	ln,
								  		   C_FUNC	*pCF);

/** \brief fio_fiod_inputs_filter_set handler */
static	int16_t	funcFioFiodInputsFilterSet(uint16_t	ln,
								  		   C_FUNC	*pCF);

/** \brief fio_fiod_inputs_get handler */
static	int16_t	funcFioFiodInputsGet(uint16_t	ln,
								  	 C_FUNC		*pCF);

/** \brief fio_fiod_inputs_trans_get handler */
static	int16_t	funcFioFiodInputsTransGet(uint16_t	ln,
								  		  C_FUNC	*pCF);

/** \brief fio_fiod_inputs_trans_read handler */
static	int16_t	funcFioFiodInputsTransRead(uint16_t	ln,
								  		   C_FUNC	*pCF);

/** \brief fio_fiod_inputs_trans_set handler */
static	int16_t	funcFioFiodInputsTransSet(uint16_t	ln,
								  		  C_FUNC	*pCF);

/** \brief fio_fiod_mmu_flash_bit_get handler */
static	int16_t	funcFioFiodMmuFlashBitGet(uint16_t	ln,
								  		  C_FUNC	*pCF);

/** \brief fio_fiod_mmu_flash_bit_set handler */
static	int16_t	funcFioFiodMmuFlashBitSet(uint16_t	ln,
								  		  C_FUNC	*pCF);

/** \brief fio_fiod_outputs_get handler */
static	int16_t	funcFioFiodOutputsGet(uint16_t	ln,
								  	  C_FUNC	*pCF);

/** \brief fio_fiod_outputs_reservation_get handler */
static	int16_t	funcFioFiodOutputsReservationGet(uint16_t	ln,
								  				 C_FUNC		*pCF);

/** \brief fio_fiod_outputs_reservation_set handler */
static	int16_t	funcFioFiodOutputsReservationSet(uint16_t	ln,
								  				 C_FUNC		*pCF);

/** \brief fio_fiod_begin_outputs_set handler */
static	int16_t	funcFioFiodBeginOutputsSet(uint16_t	ln,
								  	  C_FUNC	*pCF);

/** \brief fio_fiod_commit_outputs_set handler */
static	int16_t	funcFioFiodCommitOutputsSet(uint16_t	ln,
								  	  C_FUNC	*pCF);

/** \brief fio_fiod_outputs_set handler */
static	int16_t	funcFioFiodOutputsSet(uint16_t	ln,
								  	  C_FUNC	*pCF);

/** \brief fio_fiod_register handler */
static	int16_t	funcFioFiodRegister(uint16_t	ln,
								    C_FUNC		*pCF);

/** \brief fio_fiod_status_get handler */
static	int16_t	funcFioFiodStatusGet(uint16_t	ln,
								  	 C_FUNC		*pCF);

/** \brief fio_fiod_status_reset handler */
static	int16_t	funcFioFiodStatusReset(uint16_t	ln,
								  	   C_FUNC	*pCF);

/** \brief fio_fiod_ts_fault_monitor_get handler */
static	int16_t	funcFioFiodTsFaultMonitorGet(uint16_t	ln,
								  			 C_FUNC		*pCF);

/** \brief fio_fiod_ts_fault_monitor_set handler */
static	int16_t	funcFioFiodTsFaultMonitorSet(uint16_t	ln,
								  			 C_FUNC		*pCF);

/** \brief fio_fiod_ts1_volt_monitor_get handler */
static	int16_t	funcFioFiodTs1VoltMonitorGet(uint16_t	ln,
								  			 C_FUNC		*pCF);

/** \brief fio_fiod_ts1_volt_monitor_set handler */
static	int16_t	funcFioFiodTs1VoltMonitorSet(uint16_t	ln,
								  			 C_FUNC		*pCF);

/** \brief fio_fiod_wd_deregister handler */
static	int16_t	funcFioFiodWdDeregister(uint16_t	ln,
								  		C_FUNC		*pCF);

/** \brief fio_fiod_wd_heartbeat handler */
static	int16_t	funcFioFiodWdHeartbeat(uint16_t	ln,
								  	   C_FUNC	*pCF);

/** \brief fio_fiod_wd_register handler */
static	int16_t	funcFioFiodWdRegister(uint16_t	ln,
								  	  C_FUNC	*pCF);

/** \brief fio_fiod_wd_reservation_get handler */
static	int16_t	funcFioFiodWdReservationGet(uint16_t	ln,
								  			C_FUNC		*pCF);

/** \brief fio_fiod_wd_reservation_set handler */
static	int16_t	funcFioFiodWdReservationSet(uint16_t	ln,
								  			C_FUNC		*pCF);

/** \brief fio_hm_deregister handler */
static	int16_t	funcFioHmDeregister(uint16_t	ln,
								    C_FUNC		*pCF);

/** \brief fio_hm_fault_reset handler */
static	int16_t	funcFioHmFaultReset(uint16_t	ln,
								    C_FUNC		*pCF);

/** \brief fio_hm_heartbeat handler */
static	int16_t	funcFioHmHeartbeat(uint16_t	ln,
								   C_FUNC	*pCF);

/** \brief fio_hm_register handler */
static	int16_t	funcFioHmRegister(uint16_t	ln,
								  C_FUNC	*pCF);

/** \brief fio_query_fiod handler */
static	int16_t	funcFioQueryFiod(uint16_t	ln,
								 C_FUNC		*pCF);

/** \brief fio_query_frame_notify_status handler */
static	int16_t	funcFioQueryFrameNotifyStatus(uint16_t	ln,
								  			  C_FUNC	*pCF);

/** \brief fio_register handler */
static	int16_t	funcFioRegister(uint16_t	ln,
								C_FUNC		*pCF);

/** \brief fio_set_local_time_offset handler */
static	int16_t	funcFioSetLocalTimeOffset(uint16_t	ln,
								C_FUNC		*pCF);


/** \brief Table of function pointers and arguments */
static	FUNC_P	s_funcTable[] =
{
	// FPUI Library
	{ "fpui_apiver", funcFpuiApiver, VAR_PCHAR,
		2, { VAR_FPUIH, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN, 0,        0,        0,        0 } },
	{ "fpui_clear", funcFpuiClear, VAR_INT,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_clear_tab", funcFpuiClearTab, VAR_INT,
		2, { VAR_FPUIH, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN, 0,        0,        0,        0 } },
	{ "fpui_close", funcFpuiClose, VAR_INT,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_close_aux_switch", funcFpuiCloseAuxSwitch, VAR_INT,
		1, { VAR_FPUIAUXH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,        0,        0,        0,        0,        0 } },
	{ "fpui_compose_special_char", funcFpuiComposeSpecialChar, VAR_INT,
		3, { VAR_FPUIH, VAR_INT, VAR_PUCHAR, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN, FUNC_IN,    0,        0,        0 } },
	{ "fpui_del_keymap", funcFpuiDelKeymap, VAR_INT,
		2, { VAR_FPUIH, VAR_CHAR, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_display_special_char", funcFpuiDisplaySpecialChar, VAR_INT,
		2, { VAR_FPUIH, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN, 0,        0,        0,        0 } },
	{ "fpui_get_auto_repeat", funcFpuiGetAutoRepeat, VAR_BOOL,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_auto_scroll", funcFpuiGetAutoScroll, VAR_BOOL,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_auto_wrap", funcFpuiGetAutoWrap, VAR_BOOL,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_backlight", funcFpuiGetBacklight, VAR_BOOL,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_character_blink", funcFpuiGetCharacterBlink, VAR_BOOL,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_cursor", funcFpuiGetCursor, VAR_BOOL,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_cursor_blink", funcFpuiGetCursorBlink, VAR_BOOL,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_cursor_pos", funcFpuiGetCursorPos, VAR_INT,
		3, { VAR_FPUIH, VAR_INT,  VAR_INT,  VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_OUT, FUNC_OUT, 0,        0,        0 } },
	{ "fpui_get_focus", funcFpuiGetFocus, VAR_INT,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_keymap", funcFpuiGetKeymap, VAR_INT,
		4, { VAR_FPUIH, VAR_INT, VAR_PCHAR, VAR_INT, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN, FUNC_OUT,  FUNC_IN, 0,        0 } },
/*	{ "fpui_get_led", funcFpuiGetLed, VAR_BOOL,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },*/
	{ "fpui_get_reverse_video", funcFpuiGetReverseVideo, VAR_BOOL,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_underline", funcFpuiGetUnderline, VAR_BOOL,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_window_attr", funcFpuiGetWindowAttr, VAR_INT,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_get_window_size", funcFpuiGetWindowSize, VAR_INT,
		3, { VAR_FPUIH, VAR_INT,  VAR_INT,  VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_OUT, FUNC_OUT, 0,        0,        0 } },
	{ "fpui_home", funcFpuiHome, VAR_INT,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_open", funcFpuiOpen, VAR_FPUIH,
		2, { VAR_INT, VAR_PCHAR, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN, FUNC_IN, 0,        0,        0,        0 } },
	{ "fpui_open_aux_switch", funcFpuiOpenAuxSwitch, VAR_FPUIAUXH,
		0, { VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { 0,        0,        0,        0,        0,        0 } },
	{ "fpui_panel_present", funcFpuiPanelPresent, VAR_INT,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_poll", funcFpuiPoll, VAR_INT,
		2, { VAR_FPUIH, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN, 0,        0,        0,        0 } },
	{ "fpui_read", funcFpuiRead, VAR_SSIZET,
		3, { VAR_FPUIH, VAR_PCHAR, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_OUT,  FUNC_IN, 0,        0,        0 } },
	{ "fpui_read_aux_switch", funcFpuiReadAuxSwitch, VAR_INT,
		1, { VAR_FPUIAUXH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,      0,        0,        0,        0,        0 } },
	{ "fpui_read_char", funcFpuiReadChar, VAR_CHAR,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_read_string", funcFpuiReadString, VAR_SSIZET,
		3, { VAR_FPUIH, VAR_PCHAR, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_OUT,  FUNC_IN, 0,        0,        0 } },
	{ "fpui_refresh", funcFpuiRefresh, VAR_INT,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_reset_all_attributes", funcFpuiResetAllAttributes, VAR_INT,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_reset_keymap", funcFpuiResetKeymap, VAR_INT,
		2, { VAR_FPUIH, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN, 0,        0,        0,        0 } },
	{ "fpui_set_auto_repeat", funcFpuiSetAutoRepeat, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_set_auto_scroll", funcFpuiSetAutoScroll, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_set_auto_wrap", funcFpuiSetAutoWrap, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_set_backlight", funcFpuiSetBacklight, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_set_backlight_timeout", funcFpuiSetBacklightTimeout, VAR_INT,
		2, { VAR_FPUIH, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN, 0,        0,        0,        0 } },
	{ "fpui_set_character_blink", funcFpuiSetCharacterBlink, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_set_cursor", funcFpuiSetCursor, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_set_cursor_blink", funcFpuiSetCursorBlink, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_set_cursor_pos", funcFpuiSetCursorPos, VAR_INT,
		3, { VAR_FPUIH, VAR_INT, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN, FUNC_IN, 0,        0,        0 } },
	{ "fpui_set_emergency", funcFpuiSetEmergency, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_set_keymap", funcFpuiSetKeymap, VAR_INT,
		3, { VAR_FPUIH, VAR_CHAR, VAR_PCHAR, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  FUNC_IN,   0,        0,        0 } },
/*	{ "fpui_set_led", funcFpuiSetLed, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },*/
	{ "fpui_set_reverse_video", funcFpuiSetReverseVideo, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_set_tab", funcFpuiSetTab, VAR_INT,
		1, { VAR_FPUIH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fpui_set_underline", funcFpuiSetUnderline, VAR_INT,
		2, { VAR_FPUIH, VAR_BOOL, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_set_window_attr", funcFpuiSetWindowAttr, VAR_INT,
		2, { VAR_FPUIH, VAR_UINT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_write", funcFpuiWrite, VAR_SSIZET,
		3, { VAR_FPUIH, VAR_PCHAR, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN, 0,        0,        0 } },
	{ "fpui_write_at", funcFpuiWriteAt, VAR_SSIZET,
		5, { VAR_FPUIH, VAR_PCHAR, VAR_INT, VAR_INT, VAR_INT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN, FUNC_IN, FUNC_IN, 0 } },
	{ "fpui_write_char", funcFpuiWriteChar, VAR_SSIZET,
		2, { VAR_FPUIH, VAR_CHAR, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fpui_write_char_at", funcFpuiWriteCharAt, VAR_SSIZET,
		4, { VAR_FPUIH, VAR_CHAR, VAR_INT, VAR_INT, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  FUNC_IN, FUNC_IN, 0,        0 } },
	{ "fpui_write_string", funcFpuiWriteString, VAR_SSIZET,
		2, { VAR_FPUIH, VAR_PCHAR, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   0,        0,        0,        0 } },
	{ "fpui_write_string_at", funcFpuiWriteStringAt, VAR_SSIZET,
		4, { VAR_FPUIH, VAR_PCHAR, VAR_INT, VAR_INT, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN, FUNC_IN, 0,        0 } },

	// FIO Library
	{ "fio_apiver", funcFioApiver, VAR_PCHAR,
		2, { VAR_FAPPH, VAR_FVER, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fio_deregister", funcFioDeregister, VAR_INT,
		1, { VAR_FAPPH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fio_fiod_channel_map_count", funcFioFiodChannelMapCount, VAR_INT,
		3, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   0,        0,        0 } },
	{ "fio_fiod_channel_map_get", funcFioFiodChannelMapGet, VAR_INT,
		5, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_FCMAP, VAR_UINT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT,  FUNC_IN,  0 } },
	{ "fio_fiod_channel_map_set", funcFioFiodChannelMapSet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_FCMAP, VAR_UINT, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_IN,  0,        0 } },
	{ "fio_fiod_channel_reservation_get", funcFioFiodChannelReservationGet, VAR_INT,
		5, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_PUCHAR, VAR_INT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT,   FUNC_IN,        0 } },
	{ "fio_fiod_channel_reservation_set", funcFioFiodChannelReservationSet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_PUCHAR, VAR_INT, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,    FUNC_IN,        0,        0 } },
	{ "fio_fiod_cmu_dark_channel_get", funcFioFiodCmuDarkChannelGet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_FCMASK, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT,   0,        0 } },
	{ "fio_fiod_cmu_dark_channel_set", funcFioFiodCmuDarkChannelSet, VAR_INT,
		3, { VAR_FAPPH, VAR_FDEVH, VAR_FCMASK, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,    0,        0,        0 } },
	{ "fio_fiod_cmu_fault_get", funcFioFiodCmuFaultGet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_FCFSA, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT,  0,        0 } },
	{ "fio_fiod_cmu_fault_set", funcFioFiodCmuFaultSet, VAR_INT,
		3, { VAR_FAPPH, VAR_FDEVH, VAR_FCFSA, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   0,        0,        0 } },
	{ "fio_fiod_cmu_config_change_count", funcFioFiodCmuConfigChangeCount, VAR_INT,
		2, { VAR_FAPPH, VAR_FDEVH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   0,        0,        0,        0 } },
	{ "fio_fiod_deregister", funcFioFiodDeregister, VAR_INT,
		2, { VAR_FAPPH, VAR_FDEVH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   0,        0,        0,        0 } },
	{ "fio_fiod_disable", funcFioFiodDisable, VAR_INT,
		2, { VAR_FAPPH, VAR_FDEVH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   0,        0,        0,        0 } },
	{ "fio_fiod_enable", funcFioFiodEnable, VAR_INT,
		2, { VAR_FAPPH, VAR_FDEVH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   0,        0,        0,        0 } },
	{ "fio_fiod_frame_notify_deregister", funcFioFiodFrameNotifyDeregister, VAR_INT,
		3, { VAR_FAPPH, VAR_FDEVH, VAR_UINT, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,  0,        0,        0 } },
	{ "fio_fiod_frame_notify_register", funcFioFiodFrameNotifyRegister, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_UINT, VAR_FNOTF, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,  FUNC_IN,   0,        0 } },
	{ "fio_fiod_frame_read", funcFioFiodFrameRead, VAR_INT,
		7, { VAR_FAPPH, VAR_FDEVH, VAR_UINT, VAR_UINT, VAR_PUCHAR, VAR_UINT, VAR_UINT },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,  FUNC_OUT,  FUNC_OUT,   FUNC_IN,  FUNC_IN } },
	{ "fio_fiod_frame_schedule_get", funcFioFiodFrameScheduleGet, VAR_INT,
		5, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_FFSCHD, VAR_UINT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT,   FUNC_IN,  0 } },
	{ "fio_fiod_frame_schedule_set", funcFioFiodFrameScheduleSet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_FFSCHD, VAR_UINT, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,    FUNC_IN,  0,        0 } },
	{ "fio_fiod_frame_size", funcFioFiodFrameSize, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_UINT, VAR_UINT, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,  FUNC_OUT, 0,        0 } },
	{ "fio_fiod_frame_write", funcFioFiodFrameWrite, VAR_INT,
		5, { VAR_FAPPH, VAR_FDEVH, VAR_UINT, VAR_PUCHAR, VAR_UINT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,  FUNC_IN,    FUNC_IN,  0 } },
	{ "fio_fiod_inputs_filter_get", funcFioFiodInputsFilterGet, VAR_INT,
		5, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_FINF, VAR_UINT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT, FUNC_IN,  0 } },
	{ "fio_fiod_inputs_filter_set", funcFioFiodInputsFilterSet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_FINF, VAR_UINT, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,  FUNC_IN,  0,        0 } },
	{ "fio_fiod_inputs_get", funcFioFiodInputsGet, VAR_INT,
		5, { VAR_FAPPH, VAR_FDEVH, VAR_FINT, VAR_PUCHAR, VAR_INT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,  FUNC_OUT,   FUNC_IN,        0 } },
	{ "fio_fiod_inputs_trans_get", funcFioFiodInputsTransGet, VAR_INT,
		5, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_PUCHAR, VAR_INT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT,   FUNC_IN,        0 } },
	{ "fio_fiod_inputs_trans_read", funcFioFiodInputsTransRead, VAR_INT,
		5, { VAR_FAPPH, VAR_FDEVH, VAR_FTST, VAR_FTBUF, VAR_UINT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_OUT, FUNC_OUT,  FUNC_IN,  0 } },
	{ "fio_fiod_inputs_trans_set", funcFioFiodInputsTransSet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_PUCHAR, VAR_INT, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,  FUNC_IN,        0,        0 } },
	{ "fio_fiod_mmu_flash_bit_get", funcFioFiodMmuFlashBitGet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_FMFB, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT, 0,        0 } },
	{ "fio_fiod_mmu_flash_bit_set", funcFioFiodMmuFlashBitSet, VAR_INT,
		3, { VAR_FAPPH, VAR_FDEVH, VAR_FMFB, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,  0,        0,        0 } },
	{ "fio_fiod_outputs_get", funcFioFiodOutputsGet, VAR_INT,
		6, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_PUCHAR, VAR_PUCHAR, VAR_UINT },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT,   FUNC_OUT,   FUNC_IN } },
	{ "fio_fiod_outputs_reservation_get", funcFioFiodOutputsReservationGet, VAR_INT,
		5, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_PUCHAR, VAR_INT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT,   FUNC_IN,        0 } },
	{ "fio_fiod_outputs_reservation_set", funcFioFiodOutputsReservationSet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_PUCHAR, VAR_INT, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,    FUNC_IN,        0,        0 } },
	{ "fio_fiod_begin_outputs_set", funcFioFiodBeginOutputsSet, VAR_INT,
		1, { VAR_FAPPH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fio_fiod_commit_outputs_set", funcFioFiodCommitOutputsSet, VAR_INT,
		1, { VAR_FAPPH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fio_fiod_outputs_set", funcFioFiodOutputsSet, VAR_INT,
		5, { VAR_FAPPH, VAR_FDEVH, VAR_PUCHAR, VAR_PUCHAR, VAR_INT, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,    FUNC_IN,    FUNC_IN,        0 } },
	{ "fio_fiod_register", funcFioFiodRegister, VAR_FDEVH,
		3, { VAR_FAPPH, VAR_FPORT, VAR_FDEVT, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   0,        0,        0 } },
	{ "fio_fiod_status_get", funcFioFiodStatusGet, VAR_INT,
		3, { VAR_FAPPH, VAR_FDEVH, VAR_FFDSTAT, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_OUT,    0,        0,        0 } },
	{ "fio_fiod_status_reset", funcFioFiodStatusReset, VAR_INT,
		2, { VAR_FAPPH, VAR_FDEVH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   0,        0,        0,        0 } },
	{ "fio_fiod_ts_fault_monitor_get", funcFioFiodTsFaultMonitorGet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_FFMST, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT,  0,        0 } },
	{ "fio_fiod_ts_fault_monitor_set", funcFioFiodTsFaultMonitorSet, VAR_INT,
		3, { VAR_FAPPH, VAR_FDEVH, VAR_FFMST, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   0,        0,        0 } },
	{ "fio_fiod_ts1_volt_monitor_get", funcFioFiodTs1VoltMonitorGet, VAR_INT,
		4, { VAR_FAPPH, VAR_FDEVH, VAR_FVIEW, VAR_FVMST, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   FUNC_OUT,  0,        0 } },
	{ "fio_fiod_ts1_volt_monitor_set", funcFioFiodTs1VoltMonitorSet, VAR_INT,
		3, { VAR_FAPPH, VAR_FDEVH, VAR_FVMST, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   0,        0,        0 } },
	{ "fio_fiod_wd_deregister", funcFioFiodWdDeregister, VAR_INT,
		2, { VAR_FAPPH, VAR_FDEVH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   0,        0,        0,        0 } },
	{ "fio_fiod_wd_heartbeat", funcFioFiodWdHeartbeat, VAR_INT,
		2, { VAR_FAPPH, VAR_FDEVH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   0,        0,        0,        0 } },
	{ "fio_fiod_wd_register", funcFioFiodWdRegister, VAR_INT,
		2, { VAR_FAPPH, VAR_FDEVH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   0,        0,        0,        0 } },
	{ "fio_fiod_wd_reservation_get", funcFioFiodWdReservationGet, VAR_INT,
		3, { VAR_FAPPH, VAR_FDEVH, VAR_UINT, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_OUT, 0,        0,        0 } },
	{ "fio_fiod_wd_reservation_set", funcFioFiodWdReservationSet, VAR_INT,
		3, { VAR_FAPPH, VAR_FDEVH, VAR_UINT, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,  0,        0,        0 } },
	{ "fio_hm_deregister", funcFioHmDeregister, VAR_INT,
		1, { VAR_FAPPH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fio_hm_fault_reset", funcFioHmFaultReset, VAR_INT,
		1, { VAR_FAPPH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fio_hm_heartbeat", funcFioHmHeartbeat, VAR_INT,
		1, { VAR_FAPPH, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "fio_hm_register", funcFioHmRegister, VAR_INT,
		2, { VAR_FAPPH, VAR_UINT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,  0,        0,        0,        0 } },
	{ "fio_query_fiod", funcFioQueryFiod, VAR_INT,
		3, { VAR_FAPPH, VAR_FPORT, VAR_FDEVT, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN,   FUNC_IN,   0,        0,        0 } },
	{ "fio_query_frame_notify_status", funcFioQueryFrameNotifyStatus, VAR_INT,
		2, { VAR_FAPPH, VAR_FNOTI, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_OUT,  0,        0,        0,        0 } },
	{ "fio_register", funcFioRegister, VAR_FAPPH,
		0, { VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { 0,        0,        0,        0,        0,        0 } },
	{ "fio_set_local_time_offset", funcFioSetLocalTimeOffset, VAR_INT,
		2, { VAR_FAPPH, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   FUNC_IN, 0,        0,        0,        0 } },

	// TOD Library
	{ "tod_cancel_onchange_signal", funcTodCancelOnchangeSignal, VAR_INT,
		1, { VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { 0,        0,        0,        0,        0,        0 } },
	{ "tod_cancel_tick_signal", funcTodCancelTickSignal, VAR_INT,
		1, { VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { 0,        0,        0,        0,        0,        0 } },
	{ "tod_get", funcTodGet, VAR_INT, 
		3, { VAR_TVAL, VAR_INT,  VAR_INT,  VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_OUT, FUNC_OUT, FUNC_OUT, 0,        0,        0 } },
	{ "tod_get_dst_info", funcTodGetDstInfo, VAR_INT,
		1, { VAR_DSTIT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_OUT,  0,        0,        0,        0,        0 } },
	{ "tod_get_dst_state", funcTodGetDstState, VAR_INT,
		0, { VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { 0,        0,        0,        0,        0,        0 } },
	{ "tod_get_timesrc", funcTodGetTimesrc, VAR_INT,
		0, { VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { 0,        0,        0,        0,        0,        0 } },
	{ "tod_get_timesrc_freq", funcTodGetTimesrcFreq, VAR_INT,
		0, { VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { 0,        0,        0,        0,        0,        0 } },
	{ "tod_request_onchange_signal", funcTodRequestOnchangeSignal, VAR_INT,
		1, { VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN, 0,        0,        0,        0,        0 } },
	{ "tod_request_tick_signal", funcTodRequestTickSignal, VAR_INT,
		1, { VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN, 0,        0,        0,        0,        0 } },
	{ "tod_set", funcTodSet, VAR_INT, 
		2, { VAR_TVAL, VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,  FUNC_IN, 0,        0,        0,        0 } },
	{ "tod_set_dst_info", funcTodSetDstInfo, VAR_INT,
		1, { VAR_DSTIT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "tod_set_dst_state", funcTodSetDstState, VAR_INT,
		1, { VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN,   0,        0,        0,        0,        0 } },
	{ "tod_set_timesrc", funcTodSetTimesrc, VAR_INT,
		1, { VAR_INT, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { FUNC_IN, 0,        0,        0,        0,        0 } },
	{ "tod_apiver", funcTodApiver, VAR_PCHAR,
		0, { VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { 0,		0, 	 0,        0,        0,        0 } },

	// LAST ENTRY
	{ NULL, NULL, VAR_VOID,
		0, { VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID, VAR_VOID },
		   { 0,        0,        0,        0,        0,        0 } }
};

//=============================================================================
/**
 * Local Defines
 */

//=============================================================================
/**
 * \brief This function dereferences a function from the function table
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pName - Name of function being referenced
 * \param[out]	ppFunc - Dereferenced function table entry
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int16_t
funcDereferenceFunc(uint16_t	lineNumber,
					const char	*pName,
					FUNC_P		**ppFunc)
{
	FUNC_P	*pFunc = s_funcTable;
	while (NULL != pFunc->pName)
	{
		if (!strcmp(pName, pFunc->pName))
		{
			// Found the referenced function, we are done
			*ppFunc = pFunc;
			return(STATUS_PASS);
		}

		pFunc++;
	}

	{
		// Function not found
		char	string[OUTPUT_STRING_MAX];
		sprintf(string,
				"funcDereferenceFunc(): Invalid function name [%s]",
				pName);
		OUTPUT_ERR(lineNumber, string, NULL, NULL);
	}

	return(STATUS_FAIL);
}

//=============================================================================
/**
 * \brief This function verifies the arguments to be passed to a function
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - pCode generated for this function
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
int16_t
funcVerifyArgs(uint16_t		lineNumber,
			   C_FUNC		*pCF)
{
	char	string[OUTPUT_STRING_MAX];
	int		ii;

	// Verify return code
	if (pCF->returnValue)
	{
		if (pCF->pFunc->returnVarType != pCF->returnValue->varType)
		{
			sprintf(string,
					"funcVerifyArgs(): function[%s] passed return type [%s] doesn't match required [%s]",
					pCF->pFunc->pName,
					argVarStringGet(pCF->returnValue->varType),
					argVarStringGet(pCF->pFunc->returnVarType));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Verify that this argument IS a variable
		if (ARG_VAR != pCF->returnValue->argType)
		{
			sprintf(string,
					"funcVerifyArgs(): function[%s] passed return argument type [%s] is not a variable",
					pCF->pFunc->pName,
					argArgStringGet(pCF->returnValue->argType));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}
	}

	// Verify errno code
	if (pCF->errnoValue)
	{
		if (VAR_INT != pCF->errnoValue->varType)
		{
			sprintf(string,
					"funcVerifyArgs(): function[%s] passed errno type [%s] is not of type '%s'",
					pCF->pFunc->pName,
					argVarStringGet(pCF->errnoValue->varType),
					argVarStringGet(VAR_INT));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		// Verify that this argument IS a variable
		if (ARG_VAR != pCF->errnoValue->argType)
		{
			sprintf(string,
					"funcVerifyArgs(): function[%s] passed errno argument type [%s] is not a variable",
					pCF->pFunc->pName,
					argArgStringGet(pCF->errnoValue->argType));
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}
	}

	// Verify each argument
	for (ii = 0; ii < pCF->pFunc->argCount; ii++)
	{
		if (pCF->arg[ii])
		{
			VAR_TYPE	argAltVarType;
			VAR_TYPE	funcAltVarType;
			argAltVarType = argAlternateVarType(pCF->arg[ii]->varType);
			funcAltVarType = argAlternateVarType(pCF->pFunc->varType[ii]);

			if (argAltVarType != funcAltVarType)
			{
				sprintf(string,
						"funcVerifyArgs(): function[%s] passed argument p%d type [%s] doesn't match required [%s]",
						pCF->pFunc->pName,
						ii+1,
						argVarStringGet(argAltVarType),
						argVarStringGet(pCF->pFunc->varType[ii]));
				OUTPUT_ERR(lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}

			// Verify that this argument IS a variable, if it needs to be
			if (   (pCF->pFunc->argDir[ii] & FUNC_OUT)
				&& (ARG_VAR != pCF->arg[ii]->argType))
			{
				sprintf(string,
						"funcVerifyArgs(): function[%s] passed argument p%d type [%d, %s] is not a [%d, %s]",
						pCF->pFunc->pName,
						ii+1,
						pCF->arg[ii]->argType,
						argArgStringGet(pCF->arg[ii]->argType),
						ARG_VAR,
						argArgStringGet(ARG_VAR));
				OUTPUT_ERR(lineNumber, string, NULL, NULL);
				return(STATUS_FAIL);
			}
		}
		else
		{
			sprintf(string,
					"funcVerifyArgs(): function[%s] required argument p%d, not passed",
					pCF->pFunc->pName,
					ii+1);
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}
	}
	
	// Verify argument is not passed if not expected
	for (ii = pCF->pFunc->argCount; ii < ARG_MAX; ii++)
	{
		if (pCF->arg[ii])
		{
			sprintf(string,
					"funcVerifyArgs(): function[%s] invalid argument p%d",
					pCF->pFunc->pName,
					ii+1);
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_apiver()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiApiver(uint16_t	lineNumber,
		   	   C_FUNC	*pCF)
{
	// Call fpui_apiver()
	char			*argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	int				argType = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[1], &argType))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_apiver(argHandle, argType);
	argErrno = errno;

	if (pCF->returnValue)
	{
		if (argReturn != NULL)
			strncpy(pCF->returnValue->arg.data.value.pCharValue,
				argReturn,
				pCF->returnValue->arg.data.size);
		else //may need to free value array?
			pCF->returnValue->arg.data.value.pCharValue = NULL;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_clear()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiClear(uint16_t	lineNumber,
		   	  C_FUNC	*pCF)
{
	// Call fpui_clear()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_clear(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_clear_tab()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiClearTab(uint16_t	lineNumber,
		   	  	 C_FUNC		*pCF)
{
	// Call fpui_clear_tab()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	int				argType = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[1], &argType))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_clear_tab(argHandle, argType);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_close()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiClose(uint16_t	lineNumber,
		   	  C_FUNC	*pCF)
{
	// Call fpui_close()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_close(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_close_aux_switch()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiCloseAuxSwitch(uint16_t	lineNumber, C_FUNC *pCF)
{
	// Call fpui_close_aux_switch()
	int                argReturn;
	int                argErrno;
	fpui_aux_handle    argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiAuxHandle(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_close_aux_switch(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_compose_special_char()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiComposeSpecialChar(uint16_t	lineNumber,
		   	  			   C_FUNC	*pCF)
{
	// Call fpui_compose_special_char()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	int				argIndex = -1;;
	ARG_P			argBuf;
	unsigned char	buf[] = "12345678";

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastInt(lineNumber,
									  pCF->arg[1],
									  &argIndex))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (pCF->arg[2]->arg.data.size < ARG_SPECIAL_CHAR_BUF_SIZE_MIN)
		{
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFpuiComposeSpecialChar(): Argument 3 minimum size is %d, passed %d",
					ARG_SPECIAL_CHAR_BUF_SIZE_MIN,
					pCF->arg[2]->arg.data.size);
			OUTPUT_ERR(lineNumber, string, NULL, NULL);
			return(STATUS_FAIL);
		}

		argBuf.argType = ARG_TMP;
		argBuf.varType = VAR_PUCHAR;
		argBuf.arg.data.value.pUcharValue = buf;
		argBuf.arg.data.size = sizeof(buf);

		if (STATUS_FAIL == argCastPuchar(lineNumber,
										 pCF->arg[2],
										 &argBuf))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_compose_special_char(argHandle, argIndex, buf);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_del_keymap()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiDelKeymap(uint16_t	lineNumber,
		   	  	  C_FUNC	*pCF)
{
	// Call fpui_del_keymap()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	char			key = '\0';

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastChar(lineNumber, pCF->arg[1], &key))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_del_keymap(argHandle, key);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_display_special_char()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiDisplaySpecialChar(uint16_t	lineNumber,
		   	  			   C_FUNC	*pCF)
{
	// Call fpui_display_special_char()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	int				argIndex;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastInt(lineNumber,
									  pCF->arg[1],
									  &argIndex))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_display_special_char(argHandle, argIndex);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_auto_repeat()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetAutoRepeat(uint16_t lineNumber, C_FUNC *pCF)
{
	// Call fpui_get_auto_repeat()
	boolean argReturn;
	int argErrno;
	fpui_handle argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
							pCF->arg[0],
							&argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_auto_repeat(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_auto_scroll()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetAutoScroll(uint16_t	lineNumber,
		   	  		  C_FUNC	*pCF)
{
	// Call fpui_get_auto_scroll()
	boolean			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_auto_scroll(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_auto_wrap()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetAutoWrap(uint16_t	lineNumber,
		   	  		C_FUNC		*pCF)
{
	// Call fpui_get_auto_wrap()
	boolean			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_auto_wrap(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_backlight()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetBacklight(uint16_t	lineNumber,
		   	  		 C_FUNC		*pCF)
{
	// Call fpui_get_backlight()
	boolean			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_backlight(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_character_blink()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetCharacterBlink(uint16_t	lineNumber,
		   	  			  C_FUNC	*pCF)
{
	// Call fpui_get_character_blink()
	boolean			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_character_blink(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_cursor()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetCursor(uint16_t	lineNumber,
		   	  	  C_FUNC	*pCF)
{
	// Call fpui_get_cursor()
	boolean			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_cursor(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_cursor_blink()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetCursorBlink(uint16_t	lineNumber,
		   	  		   C_FUNC	*pCF)
{
	// Call fpui_get_cursor_blink()
	boolean			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_cursor_blink(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_cursor_pos()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetCursorPos(uint16_t	lineNumber,
		   	  		 C_FUNC		*pCF)
{
	// Call fpui_get_cursor_pos()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	int				argRow = 0;
	int				argColumn = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_cursor_pos(argHandle, &argRow, &argColumn);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (pCF->arg[1])
	{
		pCF->arg[1]->arg.data.value.intValue = argRow;
	}

	if (pCF->arg[2])
	{
		pCF->arg[2]->arg.data.value.intValue = argColumn;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_focus()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetFocus(uint16_t	lineNumber,
		   	  	 C_FUNC		*pCF)
{
	// Call fpui_get_focus()
	int			argReturn;
	int			argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_focus(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_keymap()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetKeymap(uint16_t	lineNumber,
		   	  	  C_FUNC	*pCF)
{
	// Call fpui_get_keymap()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	int				argKey = 0;
	char			*argStr = NULL;
	int				argSize = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastInt(lineNumber,
									  pCF->arg[1],
									  &argKey))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastInt(lineNumber,
									  pCF->arg[3],
									  &argSize))
		{
			return(STATUS_FAIL);
		}
	}

	if (argSize > 0)
	{
		// Malloc space for return string
		if (NULL == (argStr = malloc(sizeof(char) * argSize)))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFpuiGetKeymap(): Failed to malloc space for return string size [%d]",
					argSize * sizeof(char));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(argStr, 0, sizeof(char) * argSize);
	}

	argReturn = fpui_get_keymap(argHandle, argKey, argStr, argSize);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (pCF->arg[2])
	{
		if (pCF->arg[2]->arg.data.value.pCharValue)
		{
			if (argStr)
			{
				memcpy(pCF->arg[2]->arg.data.value.pCharValue,
					   argStr,
					   MIN(pCF->arg[2]->arg.data.size, sizeof(char) * argSize));
			}
		}
	}

	if (argStr)
	{
		free(argStr);
	}

	return(STATUS_PASS);
}

#if 0
//=============================================================================
/**
 * \brief This function handles call fpui_get_led()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetLed(uint16_t	lineNumber,
		   	   C_FUNC	*pCF)
{
	// Call fpui_get_led()
	boolean			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_led(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
#endif

//=============================================================================
/**
 * \brief This function handles call fpui_get_reverse_video()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetReverseVideo(uint16_t	lineNumber,
		   	  			C_FUNC		*pCF)
{
	// Call fpui_get_reverse_video()
	boolean			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_reverse_video(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_ubderline()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetUnderline(uint16_t	lineNumber,
		   	  		 C_FUNC		*pCF)
{
	// Call fpui_get_underline()
	boolean			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_underline(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_window_attr()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetWindowAttr(uint16_t	lineNumber,
		   	  		  C_FUNC	*pCF)
{
	// Call fpui_get_window_attr()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_window_attr(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_get_window_size()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiGetWindowSize(uint16_t	lineNumber,
		   	  		  C_FUNC	*pCF)
{
	// Call fpui_get_window_size()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	int				argRow = 0;
	int				argColumn = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_get_window_size(argHandle, &argRow, &argColumn);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (pCF->arg[1])
	{
		pCF->arg[1]->arg.data.value.intValue = argRow;
	}

	if (pCF->arg[2])
	{
		pCF->arg[2]->arg.data.value.intValue = argColumn;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_home()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiHome(uint16_t	lineNumber,
		   	 C_FUNC		*pCF)
{
	// Call fpui_home()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_home(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_open()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiOpen(uint16_t	lineNumber,
		   	 C_FUNC		*pCF)
{
	// Call fpui_open()
	int				argErrno;
	fpui_handle		argHandle;
	int				argFlags = 0;
	const char		*argRegname = NULL;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[0], &argFlags))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		argRegname = pCF->arg[1]->arg.data.value.pCharValue;
	}

	argHandle = fpui_open(argFlags, argRegname);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.fpuiHandle = argHandle;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_open_aux_switch()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiOpenAuxSwitch(uint16_t lineNumber, C_FUNC *pCF)
{
	// Call fpui_open_aux_switch()
	fpui_aux_handle    argHandle;
	int                argErrno;

	argHandle = fpui_open_aux_switch();
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.fpuiAuxHandle = argHandle;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_panel_present()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiPanelPresent(uint16_t	lineNumber,
		   	 C_FUNC		*pCF)
{
	// Call fpui_panel_present()
	int			argReturn;
	int			argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
							pCF->arg[0], 
							&argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_panel_present(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_poll()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiPoll(uint16_t	lineNumber,
		   	 C_FUNC		*pCF)
{
	// Call fpui_poll()
	int			argReturn;
	int			argErrno;
	fpui_handle		argHandle = 0;
	int			argFlags = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
							pCF->arg[0],
							&argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[1], &argFlags))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_poll(argHandle, argFlags);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_read()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiRead(uint16_t	lineNumber,
		   	 C_FUNC		*pCF)
{
	// Call fpui_read()
	ssize_t			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	char			*argBuf = NULL;
	int				argSize = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastInt(lineNumber,
									  pCF->arg[2],
									  &argSize))
		{
			return(STATUS_FAIL);
		}
	}

	if (argSize > 0)
	{
		// Malloc space for return string
		if (NULL == (argBuf = malloc(sizeof(char) * argSize)))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFpuiGetKeymap(): Failed to malloc space for return string size [%d]",
					argSize * sizeof(char));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(argBuf, 0, sizeof(char) * argSize);
	}

	argReturn = fpui_read(argHandle, argBuf, argSize);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.ssizetValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (pCF->arg[1])
	{
		if (pCF->arg[1]->arg.data.value.pCharValue)
		{
			if (argBuf)
			{
				memcpy(pCF->arg[1]->arg.data.value.pCharValue,
					   argBuf,
					   MIN(pCF->arg[1]->arg.data.size, sizeof(char) * argSize));
			}
		}
	}

	if (argBuf)
	{
		free(argBuf);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_read_aux_switch()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiReadAuxSwitch(uint16_t lineNumber, C_FUNC *pCF)
{
	// Call fpui_read_aux_switch()
	int                argReturn;
	int                argErrno;
	fpui_aux_handle    argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiAuxHandle(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_read_aux_switch(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_read_char()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiReadChar(uint16_t	lineNumber,
		   	  	 C_FUNC		*pCF)
{
	// Call fpui_read_char()
	char			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_read_char(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.charValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_read_string()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiReadString(uint16_t	lineNumber,
		   	  	   C_FUNC	*pCF)
{
	// Call fpui_read_string()
	ssize_t			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	char			*argBuf = NULL;
	int				argSize = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastInt(lineNumber,
									  pCF->arg[2],
									  &argSize))
		{
			return(STATUS_FAIL);
		}
	}

	if (argSize > 0)
	{
		// Malloc space for return string
		if (NULL == (argBuf = malloc(sizeof(char) * argSize)))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFpuiGetKeymap(): Failed to malloc space for return string size [%d]",
					argSize * sizeof(char));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(argBuf, 0, sizeof(char) * argSize);
	}

	argReturn = fpui_read_string(argHandle, argBuf, argSize);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.ssizetValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (pCF->arg[1])
	{
		if (pCF->arg[1]->arg.data.value.pCharValue)
		{
			if (argBuf)
			{
				memcpy(pCF->arg[1]->arg.data.value.pCharValue,
					   argBuf,
					   MIN(pCF->arg[1]->arg.data.size, sizeof(char) * argSize));
			}
		}
	}

	if (argBuf)
	{
		free(argBuf);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_refresh()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiRefresh(uint16_t	lineNumber,
		   	  	C_FUNC		*pCF)
{
	// Call fpui_refresh()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_refresh(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_reset_all_attributes()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiResetAllAttributes(uint16_t	lineNumber,
		   	  		  	   C_FUNC	*pCF)
{
	// Call fpui_reset_all_attributes()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_reset_all_attributes(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_reset_keymap()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiResetKeymap(uint16_t	lineNumber,
		   	  		C_FUNC		*pCF)
{
	// Call fpui_reset_keymap()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	int				argType = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[1], &argType))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_reset_keymap(argHandle, argType);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_auto_repeat()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetAutoRepeat(uint16_t	lineNumber,
		   	  		  C_FUNC	*pCF)
{
	// Call fpui_set_auto_repeat()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber, pCF->arg[1], &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_auto_repeat(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_auto_scroll()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetAutoScroll(uint16_t	lineNumber,
		   	  		  C_FUNC	*pCF)
{
	// Call fpui_set_auto_scroll()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber, pCF->arg[1], &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_auto_scroll(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_auto_wrap()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetAutoWrap(uint16_t	lineNumber,
		   	  		C_FUNC		*pCF)
{
	// Call fpui_set_auto_wrap()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber, pCF->arg[1], &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_auto_wrap(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_backlight()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetBacklight(uint16_t	lineNumber,
		   	  		 C_FUNC		*pCF)
{
	// Call fpui_set_backlight()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber, pCF->arg[1], &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_backlight(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_backlight_timeout()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetBacklightTimeout(uint16_t	lineNumber,
		   	  		  		C_FUNC		*pCF)
{
	// Call fpui_set_backlight_timeout()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	int				argTimeout = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[1], &argTimeout))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_backlight_timeout(argHandle, argTimeout);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_character_blink()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetCharacterBlink(uint16_t	lineNumber,
		   	  		  	  C_FUNC	*pCF)
{
	// Call fpui_set_character_blink()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber, pCF->arg[1], &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_character_blink(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_cursor()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetCursor(uint16_t	lineNumber,
		   	  	  C_FUNC	*pCF)
{
	// Call fpui_set_cursor()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber, pCF->arg[1], &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_cursor(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_cursor_blink()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetCursorBlink(uint16_t	lineNumber,
		   	  		   C_FUNC	*pCF)
{
	// Call fpui_set_cursor_blink()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber, pCF->arg[1], &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_cursor_blink(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_cursor_pos()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetCursorPos(uint16_t	lineNumber,
		   	  		 C_FUNC		*pCF)
{
	// Call fpui_set_cursor_pos()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	int				argRow = -1;
	int				argColumn = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[1], &argRow))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[2], &argColumn))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_cursor_pos(argHandle, argRow, argColumn);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_emergency()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetEmergency(uint16_t	lineNumber,
		   	  		 C_FUNC		*pCF)
{
	// Call fpui_set_emergency()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber, pCF->arg[1], &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_emergency(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_keymap()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetKeymap(uint16_t	lineNumber,
		   	  		  C_FUNC	*pCF)
{
	// Call fpui_set_keymap()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	char			argKey = '\0';
	char			*argEseq = NULL;
	ARG_P			pTo;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastChar(lineNumber,
									   pCF->arg[1],
									   &argKey))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		// Malloc space for return string
		if (NULL == (argEseq = malloc(sizeof(char) * pCF->arg[2]->arg.data.size)))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFpuiSetKeymap(): Failed to malloc space for return string size [%d]",
					pCF->arg[2]->arg.data.size * sizeof(char));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(argEseq, 0, sizeof(char) * pCF->arg[2]->arg.data.size);

		pTo.arg.data.value.pCharValue = argEseq;
		pTo.arg.data.size = pCF->arg[2]->arg.data.size;
		if (STATUS_FAIL == argCastPchar(lineNumber, pCF->arg[2], &pTo))
		{
			free(argEseq);
			return(STATUS_FAIL);
		}
	}
	argReturn = fpui_set_keymap(argHandle, argKey, argEseq);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.ssizetValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argEseq)
	{
		free(argEseq);
	}

	return(STATUS_PASS);
}

#if 0
//=============================================================================
/**
 * \brief This function handles call fpui_set_led()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetLed(uint16_t	lineNumber,
		   	   C_FUNC	*pCF)
{
	// Call fpui_set_led()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber,
										  pCF->arg[1],
										  &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_led(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
#endif

//=============================================================================
/**
 * \brief This function handles call fpui_set_reverse_video()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetReverseVideo(uint16_t	lineNumber,
		   	  		    C_FUNC		*pCF)
{
	// Call fpui_set_reverse_video()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber,
										  pCF->arg[1],
										  &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_reverse_video(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_tab()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetTab(uint16_t	lineNumber,
		   	   C_FUNC	*pCF)
{
	// Call fpui_set_tab()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_tab(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_underline()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetUnderline(uint16_t	lineNumber,
		   	  		 C_FUNC		*pCF)
{
	// Call fpui_set_underline()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	boolean			argState = FALSE;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastBoolean(lineNumber, pCF->arg[1], &argState))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_underline(argHandle, argState);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_set_window_attr()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiSetWindowAttr(uint16_t	lineNumber,
		   	  		  C_FUNC	*pCF)
{
	// Call fpui_set_window_attr()
	int				argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	unsigned int	argArg = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[1], &argArg))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_set_window_attr(argHandle, argArg);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_write()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiWrite(uint16_t	lineNumber, C_FUNC *pCF)
{
	// Call fpui_write()
	ssize_t        argReturn;
	int            argErrno;
	fpui_handle    argHandle = 0;
	char           *argBuf = NULL;
	int            argSize = 0;
	ARG_P          pTo;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[2], &argSize))
		{
			return(STATUS_FAIL);
		}
	}

	if (argSize > 0)
	{
		// Malloc space for return string
		if (NULL == (argBuf = malloc(sizeof(char) * argSize)))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFpuiGetKeymap(): Failed to malloc space for return string size [%d]",
					argSize * sizeof(char));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(argBuf, 0, sizeof(char) * argSize);

		if (pCF->arg[1])
		{
			pTo.arg.data.value.pCharValue = argBuf;
			pTo.arg.data.size = argSize * sizeof(char);
			if (STATUS_FAIL == argCastPchar(lineNumber, pCF->arg[1], &pTo))
			{
				free(argBuf);
				return(STATUS_FAIL);
			}
		}
	}

	argReturn = fpui_write(argHandle, argBuf, argSize);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.ssizetValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argBuf)
	{
		free(argBuf);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_write_at()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiWriteAt(uint16_t lineNumber, C_FUNC *pCF)
{
	// Call fpui_write_at()
	ssize_t        argReturn;
	int            argErrno;
	fpui_handle    argHandle = 0;
	char           *argBuf = NULL;
	int            argSize = 0;
	ARG_P          pTo;
	int            argRow = -1;
	int            argColumn = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[2], &argSize))
		{
			return(STATUS_FAIL);
		}
	}

	if (argSize > 0)
	{
		// Malloc space for return string
		if (NULL == (argBuf = malloc(sizeof(char) * argSize)))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFpuiWriteAt(): Failed to malloc space for return string size [%d]",
					argSize * sizeof(char));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(argBuf, 0, sizeof(char) * argSize);

		if (pCF->arg[1])
		{
			pTo.arg.data.value.pCharValue = argBuf;
			pTo.arg.data.size = argSize * sizeof(char);
			if (STATUS_FAIL == argCastPchar(lineNumber, pCF->arg[1], &pTo))
			{
				free(argBuf);
				return(STATUS_FAIL);
			}
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[3], &argRow))
		{
			if (argBuf)
			{
				free(argBuf);
			}
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[4], &argColumn))
		{
			if (argBuf)
			{
				free(argBuf);
			}
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_write_at(argHandle, argBuf, argSize, argRow, argColumn);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.ssizetValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argBuf)
	{
		free(argBuf);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_write_char()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiWriteChar(uint16_t lineNumber, C_FUNC *pCF)
{
	// Call fpui_write_char()
	ssize_t        argReturn;
	int            argErrno;
	fpui_handle    argHandle = 0;
	char           argCh = '\0';

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastChar(lineNumber, pCF->arg[1], &argCh))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_write_char(argHandle, argCh);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.ssizetValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_write_char_at()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiWriteCharAt(uint16_t lineNumber, C_FUNC *pCF)
{
	// Call fpui_write_char_at()
	ssize_t        argReturn;
	int            argErrno;
	fpui_handle    argHandle = 0;
	char           argCh = '\0';
	int            argRow = -1;
	int            argColumn = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastChar(lineNumber, pCF->arg[1], &argCh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[2], &argRow))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[3], &argColumn))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_write_char_at(argHandle, argCh, argRow, argColumn);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.ssizetValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_write_string()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiWriteString(uint16_t lineNumber, C_FUNC *pCF)
{
	// Call fpui_write_string()
	ssize_t        argReturn;
	int            argErrno;
	fpui_handle    argHandle = 0;
	char           *argBuf = NULL;
	ARG_P          pTo;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		// Malloc space for return string
		if (NULL == (argBuf = malloc(sizeof(char) * pCF->arg[1]->arg.data.size)))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFpuiWriteString(): Failed to malloc space for return string size [%d]",
					pCF->arg[1]->arg.data.size * sizeof(char));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(argBuf, 0, sizeof(char) * pCF->arg[1]->arg.data.size);

		pTo.arg.data.value.pCharValue = argBuf;
		pTo.arg.data.size = pCF->arg[1]->arg.data.size * sizeof(char);
		if (STATUS_FAIL == argCastPchar(lineNumber, pCF->arg[1], &pTo))
		{
			free(argBuf);
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_write_string(argHandle, argBuf);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.ssizetValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argBuf)
	{
		free(argBuf);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fpui_write_string_at()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFpuiWriteStringAt(uint16_t	lineNumber,
		   	  		  C_FUNC	*pCF)
{
	// Call fpui_write_string_at()
	ssize_t			argReturn;
	int				argErrno;
	fpui_handle		argHandle = 0;
	char			*argBuf = NULL;
	ARG_P			pTo;
	int				argRow = -1;
	int				argColumn = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFpuiHandle(lineNumber,
											 pCF->arg[0],
											 &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		// Malloc space for return string
		if (NULL == (argBuf = malloc(sizeof(char) * pCF->arg[1]->arg.data.size)))
		{
			// Malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFpuiWriteStringAt(): Failed to malloc space for return string size [%d]",
					pCF->arg[1]->arg.data.size * sizeof(char));
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}
		memset(argBuf, 0, sizeof(char) * pCF->arg[1]->arg.data.size);

		pTo.arg.data.value.pCharValue = argBuf;
		pTo.arg.data.size = pCF->arg[1]->arg.data.size * sizeof(char);
		if (STATUS_FAIL == argCastPchar(lineNumber,
										pCF->arg[1],
										&pTo))
		{
			free(argBuf);
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[2], &argRow))
		{
			if (argBuf)
			{
				free(argBuf);
			}
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[3], &argColumn))
		{
			if (argBuf)
			{
				free(argBuf);
			}
			return(STATUS_FAIL);
		}
	}

	argReturn = fpui_write_string_at(argHandle, argBuf, argRow, argColumn);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.ssizetValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argBuf)
	{
		free(argBuf);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call tod_cancel_onchange_signal()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodCancelOnchangeSignal(uint16_t	lineNumber,
		   					C_FUNC		*pCF)
{
	// Call tod_cancel_onchange_signal()
	int				argHandle = 0;
	int				argReturn;
	int				argErrno;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}
	argReturn = tod_cancel_onchange_signal(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call tod_cancel_tick_signal()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodCancelTickSignal(uint16_t	lineNumber,
		   				C_FUNC		*pCF)
{
	// Call tod_cancel_tick_signal()
	int				argHandle = 0;
	int				argReturn;
	int				argErrno;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = tod_cancel_tick_signal(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call tod_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodGet(uint16_t	lineNumber,
		   C_FUNC	*pCF)
{
	// Call tod_get()
	int				argErrno;
	int				argReturn;
	struct timeval	argTv;
	int				argTzsec;
	int				argDst;

	argReturn = tod_get(&argTv, &argTzsec, &argDst);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (pCF->arg[0])
	{
		pCF->arg[0]->arg.data.value.timevalValue = argTv;
	}

	if (pCF->arg[1])
	{
		pCF->arg[1]->arg.data.value.intValue = argTzsec;
	}

	if (pCF->arg[2])
	{
		pCF->arg[2]->arg.data.value.intValue = argDst;
	}
	
	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call tod_get_dst_info()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodGetDstInfo(uint16_t	lineNumber,
		   		  C_FUNC	*pCF)
{
	// Call tod_get_dst_info()
	int				argErrno;
	int				argReturn;
	dst_info_t		argDst;

	argReturn = tod_get_dst_info(&argDst);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (pCF->arg[0])
	{
		pCF->arg[0]->arg.data.value.dstinfoValue = argDst;
	}
	
	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call tod_get_dst_state()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodGetDstState(uint16_t	lineNumber,
		   		   C_FUNC	*pCF)
{
	// Call tod_get_dst_state()
	int				argErrno;
	int				argReturn;

	argReturn = tod_get_dst_state();
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call tod_get_timesrc()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodGetTimesrc(uint16_t	lineNumber,
		   		  C_FUNC	*pCF)
{
	// Call tod_get_timesrc()
	int				argErrno;
	int				argReturn;

	argReturn = tod_get_timesrc();
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call tod_get_timesrc_freq()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodGetTimesrcFreq(uint16_t	lineNumber,
		   			  C_FUNC	*pCF)
{
	// Call tod_get_timesrc_freq()
	int				argErrno;
	int				argReturn;

	argReturn = tod_get_timesrc_freq();
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call tod_request_onchange_signal()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodRequestOnchangeSignal(uint16_t	lineNumber,
		   					 C_FUNC		*pCF)
{
	// Call tod_request_onchange_signal()
	int				argErrno;
	int				argReturn;
	int				argSignalnum = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[0], &argSignalnum))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = tod_request_onchange_signal(argSignalnum);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call tod_request_tick_signal()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodRequestTickSignal(uint16_t	lineNumber,
		   				 C_FUNC		*pCF)
{
	// Call tod_request_tick_signal()
	int				argErrno;
	int				argReturn;
	int				argSignalnum = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[0], &argSignalnum))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = tod_request_tick_signal(argSignalnum);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call tod_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodSet(uint16_t	lineNumber,
		   C_FUNC	*pCF)
{
	// Call tod_set()
	int				argErrno;
	int				argReturn;
	struct timeval	argTv = { 0, 0 };
	int				argTzsec_offset = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastTv(lineNumber, pCF->arg[0], &argTv))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL ==
				argCastInt(lineNumber, pCF->arg[1], &argTzsec_offset))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = tod_set(&argTv, &argTzsec_offset);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}
	
	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call tod_set_dst_info()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodSetDstInfo(uint16_t	lineNumber,
		   		  C_FUNC	*pCF)
{
	// Call tod_set_dst_info()
	int				argErrno;
	int				argReturn;
	dst_info_t		argDstinfo;

	memset(&argDstinfo, 0, sizeof(argDstinfo));

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastDstInfo(lineNumber, pCF->arg[0], &argDstinfo))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = tod_set_dst_info(&argDstinfo);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}
	
	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call tod_set_dst_state()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodSetDstState(uint16_t	lineNumber,
		   		   C_FUNC	*pCF)
{
	// Call tod_set_dst_state()
	int				argErrno;
	int				argReturn;
	int				argEnable;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[0], &argEnable))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = tod_set_dst_state(argEnable);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}
	
	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call tod_set_timesrc()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodSetTimesrc(uint16_t	lineNumber,
		   		  C_FUNC	*pCF)
{
	// Call tod_set_timesrc()
	int				argErrno;
	int				argReturn;
	int				argTimesrc = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastInt(lineNumber, pCF->arg[0], &argTimesrc))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = tod_set_timesrc(argTimesrc);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}
	
	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call tod_apiver()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcTodApiver(uint16_t	lineNumber,
		   	   C_FUNC	*pCF)
{
	// Call tod_apiver()
	char			*argReturn;
	int			argErrno;

	argReturn = tod_apiver();
	argErrno = errno;

	if (pCF->returnValue)
	{
		if (argReturn != NULL)
			strncpy(pCF->returnValue->arg.data.value.pCharValue,
				argReturn,
				pCF->returnValue->arg.data.size);
		else // may need to free value array?
			pCF->returnValue->arg.data.value.pCharValue = NULL;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_apiver()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioApiver(uint16_t	lineNumber,
			  C_FUNC		*pCF)
{
	// Call fio_apiver()
	int				argErrno;
	char			*argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_VERSION		argVersion = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFver(lineNumber, pCF->arg[1], &argVersion))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_apiver(argHandle, argVersion);
	argErrno = errno;

	if (pCF->returnValue)
	{
		if (argReturn != NULL)
			strncpy(pCF->returnValue->arg.data.value.pCharValue,
				argReturn,
				pCF->returnValue->arg.data.size);
		else // may need to free value array?
			pCF->returnValue->arg.data.value.pCharValue = NULL;
			
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_deregister()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioDeregister(uint16_t	lineNumber,
				C_FUNC		*pCF)
{
	// Call fio_deregister()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_deregister(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_channel_map_count()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodChannelMapCount(uint16_t	lineNumber,
						   C_FUNC		*pCF)
{
	// Call fio_fiod_channel_map_count()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		argView = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_channel_map_count(argHandle, argDh, argView);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_channel_map_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodChannelMapGet(uint16_t	lineNumber,
						 C_FUNC		*pCF)
{
	// Call fio_fiod_channel_map_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		argView = -1;
	FIO_CHANNEL_MAP	*argMap = NULL;
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[4], &count))
		{
			return(STATUS_FAIL);
		}
	}

	if (count > 0)
	{
		if (NULL == (argMap = malloc(count * sizeof(FIO_CHANNEL_MAP))))
		{
			// malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFioFiodChannelMapGet(): Failed to malloc [%d] bytes for FIO_CHANNEL_MAP, count [%d]",
					count * sizeof(FIO_CHANNEL_MAP),
					count);
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}

		memset(argMap, 0, count * sizeof(FIO_CHANNEL_MAP));
	}

	argReturn = fio_fiod_channel_map_get(argHandle,
										 argDh,
										 argView,
										 argMap,
										 count);
	argErrno = errno;

	if (pCF->arg[3])
	{
		memset(pCF->arg[3]->arg.data.value.fioFcmap,
			   0,
			   sizeof(FIO_CHANNEL_MAP) * pCF->arg[3]->arg.data.size);
		memcpy(pCF->arg[3]->arg.data.value.fioFcmap,
			   argMap,
			   MIN(sizeof(FIO_CHANNEL_MAP) * pCF->arg[3]->arg.data.size,
			   	   sizeof(FIO_CHANNEL_MAP) * count));
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argMap)
	{
		free(argMap);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_channel_map_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodChannelMapSet(uint16_t	lineNumber,
						 C_FUNC		*pCF)
{
	// Call fio_fiod_channel_map_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_CHANNEL_MAP	*argMap = NULL;
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[3], &count))
		{
			return(STATUS_FAIL);
		}
	}

	if (count > 0)
	{
		if (NULL == (argMap = malloc(count * sizeof(FIO_CHANNEL_MAP))))
		{
			// malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFioFiodChannelMapSet(): Failed to malloc [%d] bytes for FIO_CHANNEL_MAP, count [%d]",
					count * sizeof(FIO_CHANNEL_MAP),
					count);
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}

		memset(argMap, 0, count * sizeof(FIO_CHANNEL_MAP));
		if (pCF->arg[2])
		{
			memcpy(argMap,
				   pCF->arg[2]->arg.data.value.fioFcmap,
				   MIN(sizeof(FIO_CHANNEL_MAP) * pCF->arg[2]->arg.data.size,
				   	   sizeof(FIO_CHANNEL_MAP) * count));
		}
	}

	argReturn = fio_fiod_channel_map_set(argHandle, argDh, argMap, count);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_channel_reservation_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodChannelReservationGet(uint16_t	lineNumber,
								 C_FUNC		*pCF)
{
	// Call fio_fiod_channel_reservation_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		argView = -1;
	unsigned char	channel[FIO_CHANNEL_BYTES];
	unsigned int count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[4], &count))
		{
			return(STATUS_FAIL);
		}
	}

	memset(channel, 0, sizeof(channel));

	argReturn = fio_fiod_channel_reservation_get(argHandle,
							argDh,
							argView,
							channel,
							count);
	argErrno = errno;

	if (pCF->arg[3])
	{
		memset(pCF->arg[3]->arg.data.value.pUcharValue, 0, pCF->arg[3]->arg.data.size);
		memcpy(pCF->arg[3]->arg.data.value.pUcharValue,
			   channel,
			   MIN(pCF->arg[3]->arg.data.size, sizeof(channel)));
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_channel_reservation_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodChannelReservationSet(uint16_t	lineNumber,
								 C_FUNC		*pCF)
{
	// Call fio_fiod_channel_reservation_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	ARG_P			argTo;
	unsigned char	channel[FIO_CHANNEL_BYTES];
	unsigned int count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argTo.argType = ARG_TMP;
	argTo.varType = VAR_PUCHAR;
	argTo.arg.data.value.pUcharValue = channel;
	argTo.arg.data.size = sizeof(channel);
	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastPuchar(lineNumber, pCF->arg[2], &argTo))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[3], &count))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_channel_reservation_set(argHandle, argDh, channel, count);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_cmu_dark_channel_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodCmuDarkChannelGet(uint16_t	lineNumber,
				C_FUNC		*pCF)
{
	// Call fio_fiod_cmu_dark_channel_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		view = -1;
	FIO_CMU_DC_MASK	mask = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &view))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_cmu_dark_channel_get(argHandle, argDh, view, &mask);
	argErrno = errno;

	if (pCF->arg[3])
	{
		pCF->arg[3]->arg.data.value.fioFcmask = mask;
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_cmu_dark_channel_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodCmuDarkChannelSet(uint16_t	lineNumber,
							 C_FUNC		*pCF)
{
	// Call fio_fiod_cmu_dark_channel_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_CMU_DC_MASK	mask = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFcmask(lineNumber, pCF->arg[2], &mask))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_cmu_dark_channel_set(argHandle, argDh, mask);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_cmu_fault_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodCmuFaultGet(uint16_t	lineNumber,
					   C_FUNC		*pCF)
{
	// Call fio_fiod_cmu_fault_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		view = -1;
	FIO_CMU_FSA		fsa = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &view))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_cmu_fault_get(argHandle, argDh, view, &fsa);
	argErrno = errno;

	if (pCF->arg[3])
	{
		pCF->arg[3]->arg.data.value.fioFcfsa = fsa;
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_cmu_fault_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodCmuFaultSet(uint16_t	lineNumber,
					   C_FUNC		*pCF)
{
	// Call fio_fiod_cmu_fault_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_CMU_FSA		fsa = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFcfsa(lineNumber, pCF->arg[2], &fsa))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_cmu_fault_set(argHandle, argDh, fsa);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_cmu_config_change_count()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodCmuConfigChangeCount(uint16_t	lineNumber,
					   C_FUNC		*pCF)
{
	// Call fio_fiod_cmu_config_change_count()
	int				argErrno;
	int				argChangeCount;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argChangeCount = fio_fiod_cmu_config_change_count(argHandle, argDh);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argChangeCount;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_deregister()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodDeregister(uint16_t	lineNumber,
					  C_FUNC		*pCF)
{
	// Call fio_fiod_deregister()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_deregister(argHandle, argDh);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_disable()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodDisable(uint16_t	lineNumber,
				   C_FUNC		*pCF)
{
	// Call fio_fiod_disable()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_disable(argHandle, argDh);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_enable()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodEnable(uint16_t	lineNumber,
				  C_FUNC		*pCF)
{
	// Call fio_fiod_enable()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_enable(argHandle, argDh);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_frame_notify_deregister()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodFrameNotifyDeregister(uint16_t	lineNumber,
								 C_FUNC		*pCF)
{
	// Call fio_fiod_frame_notify_deregister()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	unsigned int	rx_frame = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[2], &rx_frame))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_frame_notify_deregister(argHandle, argDh, rx_frame);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_frame_notfiy_register()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodFrameNotifyRegister(uint16_t	lineNumber,
							   C_FUNC		*pCF)
{
	// Call fio_fiod_frame_notfiy_register()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	unsigned int	rx_frame = -1;
	FIO_NOTIFY		notify;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[2], &rx_frame))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastFnotf(lineNumber, pCF->arg[3], &notify))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_frame_notify_register(argHandle,
											   argDh,
											   rx_frame,
											   notify);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_frame_read()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodFrameRead(uint16_t	lineNumber,
					 C_FUNC		*pCF)
{
	// Call fio_fiod_frame_read()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	unsigned int	rx_frame = -1;
	unsigned int	seq = -1;
	unsigned char	*argBuf = NULL;
	unsigned int	count = 0;
	unsigned int	timeout = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[2], &rx_frame))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[5])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[5], &count))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[6])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[6], &timeout))
		{
			return(STATUS_FAIL);
		}
	}

	if (count > 0)
	{
		if (NULL == (argBuf = malloc(count * sizeof(unsigned char))))
		{
			// malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFioFiodFrameRead(): Failed to malloc [%d] bytes for unsigned char, count [%d]",
					count * sizeof(unsigned char),
					count);
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}

		memset(argBuf, 0, count * sizeof(unsigned char));
	}

	argReturn = fio_fiod_frame_read(argHandle, argDh, rx_frame, &seq,
					argBuf,	count, timeout);
	argErrno = errno;

	if (pCF->arg[3])
	{
		pCF->arg[3]->arg.data.value.uintValue = seq;
	}

	if (pCF->arg[4])
	{
		memset(pCF->arg[4]->arg.data.value.pUcharValue,
			   0,
			   sizeof(unsigned char) * pCF->arg[4]->arg.data.size);
		memcpy(pCF->arg[4]->arg.data.value.pUcharValue,
			   argBuf,
			   MIN(sizeof(unsigned char) * pCF->arg[4]->arg.data.size,
			   	   sizeof(unsigned char) * count));
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argBuf)
	{
		free(argBuf);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_frame_schedule_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodFrameScheduleGet(uint16_t	lineNumber,
							C_FUNC		*pCF)
{
	// Call fio_fiod_frame_schedule_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		argView = -1;
	FIO_FRAME_SCHD	*argSchd = NULL;
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[4], &count))
		{
			return(STATUS_FAIL);
		}
	}

	if (count > 0)
	{
		if (NULL == (argSchd = malloc(count * sizeof(FIO_FRAME_SCHD))))
		{
			// malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFioFiodFrameScheduleGet(): Failed to malloc [%d] bytes for FIO_FRAME_SCHD, count [%d]",
					count * sizeof(FIO_FRAME_SCHD),
					count);
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}

		memset(argSchd, 0, count * sizeof(FIO_FRAME_SCHD));
	}

	argReturn = fio_fiod_frame_schedule_get(argHandle,
											argDh,
											argView,
											argSchd,
											count);
	argErrno = errno;

	if (pCF->arg[3])
	{
		memset(pCF->arg[3]->arg.data.value.fioFschd,
			   0,
			   sizeof(FIO_FRAME_SCHD) * pCF->arg[3]->arg.data.size);
		memcpy(pCF->arg[3]->arg.data.value.fioFschd,
			   argSchd,
			   MIN(sizeof(FIO_FRAME_SCHD) * pCF->arg[3]->arg.data.size,
			   	   sizeof(FIO_FRAME_SCHD) * count));
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argSchd)
	{
		free(argSchd);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_frame_schedule_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodFrameScheduleSet(uint16_t	lineNumber,
							C_FUNC		*pCF)
{
	// Call fio_fiod_frame_schedule_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_FRAME_SCHD	*argSchd = NULL;
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[3], &count))
		{
			return(STATUS_FAIL);
		}
	}

	if (count > 0)
	{
		if (NULL == (argSchd = malloc(count * sizeof(FIO_FRAME_SCHD))))
		{
			// malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFioFiodFrameScheduleSet(): Failed to malloc [%d] bytes for FIO_FRAME_SCHD, count [%d]",
					count * sizeof(FIO_FRAME_SCHD),
					count);
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}

		memset(argSchd, 0, count * sizeof(FIO_FRAME_SCHD));
		if (pCF->arg[2])
		{
			memcpy(argSchd,
				   pCF->arg[2]->arg.data.value.fioFschd,
				   MIN(sizeof(FIO_FRAME_SCHD) * pCF->arg[2]->arg.data.size,
				   	   sizeof(FIO_FRAME_SCHD) * count));
		}
	}

	argReturn = fio_fiod_frame_schedule_set(argHandle, argDh, argSchd, count);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argSchd)
	{
		free(argSchd);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_frame_size()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodFrameSize(uint16_t	lineNumber,
					 C_FUNC		*pCF)
{
	// Call fio_fiod_frame_size()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	unsigned int	rx_frame = -1;
	unsigned int	seq = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[2], &rx_frame))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_frame_size(argHandle, argDh, rx_frame, &seq);
	argErrno = errno;

	if (pCF->arg[3])
	{
		pCF->arg[3]->arg.data.value.uintValue = seq;
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_frame_write()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodFrameWrite(uint16_t	lineNumber,
							C_FUNC		*pCF)
{
	// Call fio_fiod_frame_write()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	unsigned int	argFrameNumber;
	unsigned char	*argPayload = NULL;
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[2], &argFrameNumber))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[4], &count))
		{
			return(STATUS_FAIL);
		}
	}

	if (count > 0)
	{
		if (NULL == (argPayload = malloc(count)))
		{
			// malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFioFiodFrameWrite(): Failed to malloc [%d] bytes for payload, count [%d]",
					count,
					count);
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}

		memset(argPayload, 0, count);
		if (pCF->arg[3])
		{
			memcpy(argPayload,
				   pCF->arg[3]->arg.data.value.pUcharValue,
				   MIN(pCF->arg[3]->arg.data.size, count));
		}
	}

	argReturn = fio_fiod_frame_write(argHandle, argDh, argFrameNumber, argPayload, count);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argPayload)
	{
		free(argPayload);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_inputs_filter_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodInputsFilterGet(uint16_t	lineNumber,
						   C_FUNC		*pCF)
{
	// Call fio_fiod_inputs_filter_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		argView = -1;
	FIO_INPUT_FILTER	*argFilter = NULL;
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[4], &count))
		{
			return(STATUS_FAIL);
		}
	}

	if (count > 0)
	{
		if (NULL == (argFilter = malloc(count * sizeof(FIO_INPUT_FILTER))))
		{
			// malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFioFiodInputsFilterGet(): Failed to malloc [%d] bytes for FIO_INPUT_FILTER, count [%d]",
					count * sizeof(FIO_INPUT_FILTER),
					count);
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}

		memset(argFilter, 0, count * sizeof(FIO_INPUT_FILTER));
	}

	argReturn = fio_fiod_inputs_filter_get(argHandle,
										   argDh,
										   argView,
										   argFilter,
										   count);
	argErrno = errno;

	if (pCF->arg[3])
	{
		memset(pCF->arg[3]->arg.data.value.fioFinf,
			   0,
			   sizeof(FIO_INPUT_FILTER) * pCF->arg[3]->arg.data.size);
		memcpy(pCF->arg[3]->arg.data.value.fioFinf,
			   argFilter,
			   MIN(sizeof(FIO_INPUT_FILTER) * pCF->arg[3]->arg.data.size,
			   	   sizeof(FIO_INPUT_FILTER) * count));
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argFilter)
	{
		free(argFilter);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_inputs_filter_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodInputsFilterSet(uint16_t	lineNumber,
						   C_FUNC		*pCF)
{
	// Call fio_fiod_inputs_filter_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_INPUT_FILTER	*argFilter = NULL;
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[3], &count))
		{
			return(STATUS_FAIL);
		}
	}

	if (count > 0)
	{
		if (NULL == (argFilter = malloc(count * sizeof(FIO_INPUT_FILTER))))
		{
			// malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFioFiodInputsFilterSet(): Failed to malloc [%d] bytes for FIO_INPUT_FILTER, count [%d]",
					count * sizeof(FIO_INPUT_FILTER),
					count);
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}

		memset(argFilter, 0, count * sizeof(FIO_INPUT_FILTER));
		if (pCF->arg[2])
		{
			memcpy(argFilter,
				   pCF->arg[2]->arg.data.value.fioFinf,
				   MIN(sizeof(FIO_INPUT_FILTER) * pCF->arg[2]->arg.data.size,
				   	   sizeof(FIO_INPUT_FILTER) * count));
		}
	}

	argReturn = fio_fiod_inputs_filter_set(argHandle, argDh, argFilter, count);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argFilter)
	{
		free(argFilter);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_inputs_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodInputsGet(uint16_t	lineNumber,
					 C_FUNC		*pCF)
{
	// Call fio_fiod_inputs_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_INPUTS_TYPE	argFint = -1;
	unsigned char	data[FIO_INPUT_POINTS_BYTES];
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFint(lineNumber, pCF->arg[2], &argFint))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[4], &count))
		{
			return(STATUS_FAIL);
		}
	}

	memset(data, 0, sizeof(data));

	argReturn = fio_fiod_inputs_get(argHandle, argDh, argFint, data, count);
	argErrno = errno;

	if (pCF->arg[3])
	{
		memset(pCF->arg[3]->arg.data.value.pUcharValue, 0, pCF->arg[3]->arg.data.size);
		memcpy(pCF->arg[3]->arg.data.value.pUcharValue,
			   data,
			   MIN(pCF->arg[3]->arg.data.size, count));
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_inputs_trans_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodInputsTransGet(uint16_t	lineNumber,
						  C_FUNC		*pCF)
{
	// Call fio_fiod_inputs_trans_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW	argView = -1;
	unsigned char	data[FIO_INPUT_POINTS_BYTES];
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[4], &count))
		{
			return(STATUS_FAIL);
		}
	}

	memset(data, 0, sizeof(data));

	argReturn = fio_fiod_inputs_trans_get(argHandle, argDh, argView, data, count);
	argErrno = errno;

	if ((count > 0) && pCF->arg[3])
	{
		memset(pCF->arg[3]->arg.data.value.pUcharValue, 0, pCF->arg[3]->arg.data.size);
		memcpy(pCF->arg[3]->arg.data.value.pUcharValue,
			   data,
			   MIN(pCF->arg[3]->arg.data.size, sizeof(data)));
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_inputs_trans_read()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodInputsTransRead(uint16_t	lineNumber,
						   C_FUNC		*pCF)
{
	// Call fio_fiod_inputs_trans_read()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_TRANS_STATUS	argStatus = -1;
	FIO_TRANS_BUFFER	*argBuf = NULL;
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[4], &count))
		{
			return(STATUS_FAIL);
		}
	}

	if (count > 0)
	{
		if (NULL == (argBuf = malloc(count * sizeof(FIO_TRANS_BUFFER))))
		{
			// malloc failed
			char	string[OUTPUT_STRING_MAX];
			sprintf(string,
					"funcFioFiodInputsTransRead(): Failed to malloc [%d] bytes for FIO_TRANS_BUFFER, count [%d]",
					count * sizeof(FIO_TRANS_BUFFER),
					count);
			OUTPUT_ERR(lineNumber, string, strerror(errno), NULL);
			return(STATUS_FAIL);
		}

		memset(argBuf, 0, count * sizeof(FIO_TRANS_BUFFER));
	}

	argReturn = fio_fiod_inputs_trans_read(argHandle,
										   argDh,
										   &argStatus,
										   argBuf,
										   count);
	argErrno = errno;

	if (pCF->arg[2])
	{
		pCF->arg[2]->arg.data.value.fioTstatus = argStatus;
	}

	if (pCF->arg[3])
	{
		memset(pCF->arg[3]->arg.data.value.fioTbuffer,
			   0,
			   sizeof(FIO_TRANS_BUFFER) * pCF->arg[3]->arg.data.size);
		memcpy(pCF->arg[3]->arg.data.value.fioTbuffer,
			   argBuf,
			   MIN(sizeof(FIO_TRANS_BUFFER) * pCF->arg[3]->arg.data.size,
			   	   sizeof(FIO_TRANS_BUFFER) * count));
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	if (argBuf)
	{
		free(argBuf);
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_inputs_trans_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodInputsTransSet(uint16_t	lineNumber,
						  C_FUNC		*pCF)
{
	// Call fio_fiod_inputs_trans_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	ARG_P		argTo;
	unsigned char	data[FIO_INPUT_POINTS_BYTES];
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argTo.argType = ARG_TMP;
	argTo.varType = VAR_PUCHAR;
	argTo.arg.data.value.pUcharValue = data;
	argTo.arg.data.size = sizeof(data);
	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastPuchar(lineNumber, pCF->arg[2], &argTo))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[3], &count))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_inputs_trans_set(argHandle, argDh, data, count);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_mmu_flash_bit_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodMmuFlashBitGet(uint16_t	lineNumber,
						  C_FUNC		*pCF)
{
	// Call fio_fiod_mmu_flash_bit_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		argView = -1;
	FIO_MMU_FLASH_BIT	argFb;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_mmu_flash_bit_get(argHandle, argDh, argView, &argFb);
	argErrno = errno;

	if (pCF->arg[3])
	{
		pCF->arg[3]->arg.data.value.fioFb = argFb;
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_mmu_flash_bit_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodMmuFlashBitSet(uint16_t	lineNumber,
						  C_FUNC		*pCF)
{
	// Call fio_fiod_mmu_flash_bit_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_MMU_FLASH_BIT	argFb = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFmfb(lineNumber, pCF->arg[2], &argFb))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_mmu_flash_bit_set(argHandle, argDh, argFb);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_outputs_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodOutputsGet(uint16_t	lineNumber,
					  C_FUNC		*pCF)
{
	// Call fio_fiod_outputs_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		argView = -1;
	unsigned char	ls_plus[FIO_OUTPUT_POINTS_BYTES];
	unsigned char	ls_minus[FIO_OUTPUT_POINTS_BYTES];
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[5])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[5], &count))
		{
			return(STATUS_FAIL);
		}
	}


	memset(ls_plus, 0, sizeof(ls_plus));
	memset(ls_minus, 0, sizeof(ls_minus));

	argReturn = fio_fiod_outputs_get(argHandle,
									 argDh,
									 argView,
									 ls_plus,
									 ls_minus,
									 count);
	argErrno = errno;

	if (pCF->arg[3])
	{
		memset(pCF->arg[3]->arg.data.value.pUcharValue, 0, pCF->arg[3]->arg.data.size);
		memcpy(pCF->arg[3]->arg.data.value.pUcharValue,
			   ls_plus,
			   MIN(pCF->arg[3]->arg.data.size, count));
	}

	if (pCF->arg[4])
	{
		memset(pCF->arg[4]->arg.data.value.pUcharValue, 0, pCF->arg[4]->arg.data.size);
		memcpy(pCF->arg[4]->arg.data.value.pUcharValue,
			   ls_minus,
			   MIN(pCF->arg[4]->arg.data.size, count));
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_outputs_reservation_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodOutputsReservationGet(uint16_t	lineNumber,
								 C_FUNC		*pCF)
{
	// Call fio_fiod_outputs_reservation_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		argView = -1;
	unsigned char	data[FIO_OUTPUT_POINTS_BYTES];
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[4], &count))
		{
			return(STATUS_FAIL);
		}
	}

	memset(data, 0, sizeof(data));

	argReturn = fio_fiod_outputs_reservation_get(argHandle,
							argDh,
							argView,
							data,
							count);
	argErrno = errno;

	if (pCF->arg[3])
	{
		memset(pCF->arg[3]->arg.data.value.pUcharValue, 0, pCF->arg[3]->arg.data.size);
		memcpy(pCF->arg[3]->arg.data.value.pUcharValue,
			   data,
			   MIN(pCF->arg[3]->arg.data.size, sizeof(data)));
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_outputs_reservation_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodOutputsReservationSet(uint16_t	lineNumber,
								 C_FUNC		*pCF)
{
	// Call fio_fiod_outputs_reservation_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	ARG_P			argTo;
	unsigned char	data[FIO_OUTPUT_POINTS_BYTES];
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argTo.argType = ARG_TMP;
	argTo.varType = VAR_PUCHAR;
	argTo.arg.data.value.pUcharValue = data;
	argTo.arg.data.size = sizeof(data);
	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastPuchar(lineNumber, pCF->arg[2], &argTo))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[3], &count))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_outputs_reservation_set(argHandle, argDh, data, count);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_begin_outputs_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodBeginOutputsSet(uint16_t	lineNumber,
					  C_FUNC		*pCF)
{
	// Call fio_fiod_begin_outputs_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_begin_outputs_set(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_commit_oututs_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodCommitOutputsSet(uint16_t	lineNumber,
					  C_FUNC		*pCF)
{
	// Call fio_fiod_deregister()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_commit_outputs_set(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_outputs_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodOutputsSet(uint16_t	lineNumber,
					  C_FUNC		*pCF)
{
	// Call fio_fiod_outputs_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	ARG_P			argTo;
	unsigned char	ls_plus[FIO_OUTPUT_POINTS_BYTES];
	unsigned char	ls_minus[FIO_OUTPUT_POINTS_BYTES];
	unsigned int	count = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argTo.argType = ARG_TMP;
	argTo.varType = VAR_PUCHAR;
	argTo.arg.data.value.pUcharValue = ls_plus;
	argTo.arg.data.size = sizeof(ls_plus);
	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastPuchar(lineNumber, pCF->arg[2], &argTo))
		{
			return(STATUS_FAIL);
		}
	}

	argTo.arg.data.value.pUcharValue = ls_minus;
	argTo.arg.data.size = sizeof(ls_minus);
	if (pCF->arg[3])
	{
		if (STATUS_FAIL == argCastPuchar(lineNumber, pCF->arg[3], &argTo))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[4])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[4], &count))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_outputs_set(argHandle, argDh, ls_plus, ls_minus, count);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_register()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodRegister(uint16_t	lineNumber,
					C_FUNC		*pCF)
{
	// Call fio_fiod_register()
	int				argErrno;
	FIO_DEV_HANDLE	argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_PORT		argPort = -1;
	FIO_DEVICE_TYPE	argDt = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFport(lineNumber, pCF->arg[1], &argPort))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFdevt(lineNumber, pCF->arg[2], &argDt))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_register(argHandle, argPort, argDt);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.fioDevHandle = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_status_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodStatusGet(uint16_t	lineNumber,
					 C_FUNC		*pCF)
{
	// Call fio_fiod_status_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_FIOD_STATUS	argStatus;

	memset(&argStatus, 0, sizeof(argStatus));

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_status_get(argHandle, argDh, &argStatus);
	argErrno = errno;

	if (pCF->arg[2])
	{
		pCF->arg[2]->arg.data.value.fioFiodStatus = argStatus;
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_status_reset()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodStatusReset(uint16_t	lineNumber,
					   C_FUNC		*pCF)
{
	// Call fio_fiod_status_reset()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_status_reset(argHandle, argDh);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_ts_fault_monitor_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodTsFaultMonitorGet(uint16_t	lineNumber,
							 C_FUNC		*pCF)
{
	// Call fio_fiod_ts_fault_monitor_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		argView = -1;
	FIO_TS_FM_STATE argFms = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_ts_fault_monitor_get(argHandle,
											  argDh,
											  argView,
											  &argFms);
	argErrno = errno;

	if (pCF->arg[3])
	{
		pCF->arg[3]->arg.data.value.fioFms = argFms;
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_ts_fault_monitor_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodTsFaultMonitorSet(uint16_t	lineNumber,
							 C_FUNC		*pCF)
{
	// Call fio_fiod_ts_fault_monitor_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_TS_FM_STATE argFms = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFfmst(lineNumber, pCF->arg[2], &argFms))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_ts_fault_monitor_set(argHandle, argDh, argFms);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_ts1_volt_monitor_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodTs1VoltMonitorGet(uint16_t	lineNumber,
							 C_FUNC		*pCF)
{
	// Call fio_fiod_ts1_volt_monitor_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_VIEW		argView = -1;
	FIO_TS1_VM_STATE argVms = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFview(lineNumber, pCF->arg[2], &argView))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_ts1_volt_monitor_get(argHandle,
											  argDh,
											  argView,
											  &argVms);
	argErrno = errno;

	if (pCF->arg[3])
	{
		pCF->arg[3]->arg.data.value.fioVms = argVms;
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_ts1_volt_monitor_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodTs1VoltMonitorSet(uint16_t	lineNumber,
							 C_FUNC		*pCF)
{
	// Call fio_fiod_ts1_volt_monitor_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	FIO_TS1_VM_STATE argVms = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFvmst(lineNumber, pCF->arg[2], &argVms))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_ts1_volt_monitor_set(argHandle, argDh, argVms);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_wd_deregister()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodWdDeregister(uint16_t	lineNumber,
						C_FUNC		*pCF)
{
	// Call fio_fiod_wd_deregister()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_wd_deregister(argHandle, argDh);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}

//=============================================================================
/**
 * \brief This function handles call fio_fiod_wd_heartbeat()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodWdHeartbeat(uint16_t	lineNumber,
					   C_FUNC		*pCF)
{
	// Call fio_fiod_wd_heartbeat()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_wd_heartbeat(argHandle, argDh);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_fiod_wd_register()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodWdRegister(uint16_t	lineNumber,
				   C_FUNC		*pCF)
{
	// Call fio_fiod_wd_register()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_wd_register(argHandle, argDh);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_fiod_wd_reservation_get()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodWdReservationGet(uint16_t	lineNumber,
							C_FUNC		*pCF)
{
	// Call fio_fiod_wd_reservation_get()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	unsigned int	argPoint = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_wd_reservation_get(argHandle, argDh, &argPoint);
	argErrno = errno;

	if (pCF->arg[2])
	{
		pCF->arg[2]->arg.data.value.uintValue = argPoint;
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_fiod_wd_reservation_set()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioFiodWdReservationSet(uint16_t	lineNumber,
							C_FUNC		*pCF)
{
	// Call fio_fiod_wd_reservation_set()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_DEV_HANDLE	argDh = -1;
	unsigned int	argPoint = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFdevh(lineNumber, pCF->arg[1], &argDh))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[2], &argPoint))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_fiod_wd_reservation_set(argHandle, argDh, argPoint);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_hm_deregister()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioHmDeregister(uint16_t	lineNumber,
					C_FUNC		*pCF)
{
	// Call fio_hm_deregister()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_hm_deregister(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_hm_fault_reset()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioHmFaultReset(uint16_t	lineNumber,
					C_FUNC		*pCF)
{
	// Call fio_hm_fault_reset()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_hm_fault_reset(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_hm_heartbeat()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioHmHeartbeat(uint16_t	lineNumber,
				   C_FUNC		*pCF)
{
	// Call fio_hm_heartbeat()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_hm_heartbeat(argHandle);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_hm_register()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioHmRegister(uint16_t	lineNumber,
				  C_FUNC		*pCF)
{
	// Call fio_hm_register()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	unsigned int	argTimeout = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastUint(lineNumber, pCF->arg[1], &argTimeout))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_hm_register(argHandle, argTimeout);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_query_fiod()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioQueryFiod(uint16_t	lineNumber,
				 C_FUNC		*pCF)
{
	// Call fio_query_fiod()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_PORT		argPort = -1;
	FIO_DEVICE_TYPE	argDevice = -1;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL == argCastFport(lineNumber, pCF->arg[1], &argPort))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[2])
	{
		if (STATUS_FAIL == argCastFdevt(lineNumber, pCF->arg[2], &argDevice))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_query_fiod(argHandle, argPort, argDevice);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_query_frame_notify_status()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioQueryFrameNotifyStatus(uint16_t	lineNumber,
							  C_FUNC		*pCF)
{
	// Call fio_query_frame_notify_status()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE	argHandle = -1;
	FIO_NOTIFY_INFO	argInfo;

	memset(&argInfo, 0, sizeof(argInfo));

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_query_frame_notify_status(argHandle, &argInfo);
	argErrno = errno;

	if (pCF->arg[1])
	{
		pCF->arg[1]->arg.data.value.fioInfo = argInfo;
	}

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_register()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioRegister(uint16_t	lineNumber,
				C_FUNC		*pCF)
{
	// Call fio_register()
	int				argErrno;
	FIO_APP_HANDLE	argHandle;

	argHandle = fio_register();
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.fioAppHandle = argHandle;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
//=============================================================================
/**
 * \brief This function handles call fio_set_local_time_offset()
 *
 * \param[in]	lineNumber - Line number in XML file
 * \param[in]	pCF - Pointer to function calling info
 * \return		STATUS_PASS - Test conformance (PASS)
 * 				STATUS_FAIL - Test nonconformance (FAIL)
 */
static int16_t
funcFioSetLocalTimeOffset(uint16_t	lineNumber,
				C_FUNC		*pCF)
{
	// Call fio_set_local_time_offset()
	int				argErrno;
	int				argReturn;
	FIO_APP_HANDLE			argHandle = -1;
	int				argTzsec_offset = 0;

	if (pCF->arg[0])
	{
		if (STATUS_FAIL == argCastFapph(lineNumber, pCF->arg[0], &argHandle))
		{
			return(STATUS_FAIL);
		}
	}

	if (pCF->arg[1])
	{
		if (STATUS_FAIL ==
				argCastInt(lineNumber, pCF->arg[1], &argTzsec_offset))
		{
			return(STATUS_FAIL);
		}
	}

	argReturn = fio_set_local_time_offset(argHandle, &argTzsec_offset);
	argErrno = errno;

	if (pCF->returnValue)
	{
		pCF->returnValue->arg.data.value.intValue = argReturn;
	}

	if (pCF->errnoValue)
	{
		pCF->errnoValue->arg.data.value.intValue = argErrno;
	}

	return(STATUS_PASS);
}
