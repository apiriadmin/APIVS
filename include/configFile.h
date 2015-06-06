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
 * \file configFile.h
 * This file contains the definition of the \c #configFile
 *
 * © Copyright 2010 ITE
 *   All rights reserved.  Copying or other reproduction of this program
 *   except for archival purposes is prohibited without the prior written
 *   consent of ITE.
 *
 * \brief configFile
 *
 * \author Thomas E. Gauger
 *
 * \version 1.0
 *
 *****************************************************************************/
#ifndef	CONFIGFILE_H
#define	CONFIGFILE_H

//=============================================================================
/**
 * Includes
 */

//=============================================================================
/**
 * Extern
 */

/** \brief Definition for the config file processor */
extern	int16_t	configFileProcess(char *fileName);

/** \brief Get the configured XML Input File Path */
extern	const char	*configFileGetXIFP();

/** \brief Get the configured XML Output File Path */
extern	const char	*configFileGetXOFP();

/** \brief Get the configured Set File Path */
extern	const char	*configFileGetSFP();

/** \brief Get the configured Screen Height */
extern	int			configFileGetSH();

/** \brief Get the configured Screen Width */
extern	int			configFileGetSW();

/** \brief Get the configured FPUI Compare File Path */
extern	const char	*configFileGetFPUICFP();

/** \brief Get the configured FPUI Input File Path */
extern	const char	*configFileGetFPUIIFP();

/** \brief Get the configured FPUI Dump File Path */
extern	const char	*configFileGetFPUIDFP();

/** \brief Get the configured FPUI Loopback Device */
extern	const char	*configFileGetFPUILBDEV();

/** \brief Get the configured FIO Compare File Path */
extern	const char	*configFileGetFIOCFP();

/** \brief Get the configured FIO Response File Path */
extern	const char	*configFileGetFIORFP();

/** \brief Get the configured FIO Dump File Path */
extern	const char	*configFileGetFIODFP();

/** \brief Get the configured FIO Loopback Device */
extern	const char	*configFileGetFIOLBDEV();

//=============================================================================
/**
 * Local Defines
 */

#define	CF_SCREEN_HEIGHT	(8)
#define	CF_SCREEN_WIDTH		(40)

#define	CF_COMMENT_LINE_CHAR	'#'
#define	CF_EOL_CHAR				'\r'
#define	CF_SPACE_CHAR			' '
#define	CF_TAB_CHAR				'\t'
#define	CF_NULL_CHAR			'\0'
#define	CF_SEPARATOR_CHAR		'='

#define	CF_XIFP					"XMLInputFilePath"
#define	CF_XOFP					"XMLOutputFilePath"
#define	CF_SFP					"SetFilePath"
#define	CF_SH					"ScreenHeight"
#define	CF_SW					"ScreenWidth"
#define	CF_FPUICFP				"FPUICompareFilePath"
#define	CF_FPUIIFP				"FPUIInputFilePath"
#define	CF_FPUIDFP				"FPUIDumpFilePath"
#define	CF_FPUILBDEV			"FPUILoopbackDevice"
#define	CF_FIOCFP				"FIOCompareFilePath"
#define	CF_FIORFP				"FIOResponseFilePath"
#define	CF_FIODFP				"FIODumpFilePath"
#define	CF_FIOLBDEV				"FIOLoopbackDevice"

#endif	/* CONFIGFILE_H */

