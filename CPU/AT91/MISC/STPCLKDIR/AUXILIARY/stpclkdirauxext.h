//!	external interface of auxiliary package of AT91SAM7X STPCLKDIR driver
/**
		\file		stpclkdirauxext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIRAUXEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-03-24 initial version
// 2011-10-19 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRAUXEXT_H
#define SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRAUXEXT_H 1


// Prototypes for functions

//!   Direction control using AT91 PIO
/**
      \param	ucUnit					      number of the stpclkdir unit
      \param	bClockwise					   direction to set (#TRUE == clockwise)
      \return										> 0 for success, == 0 for failure
*/
ULONG StpClkDirAuxDirAt91Pio(UCHAR ucUnit, bool bClockwise);

//!   Power control using AT91 PIO
/**
      \param	ucUnit					      number of the stpclkdir unit
      \param	bEnable					      power state to set (#TRUE == active)
      \return										> 0 for success, == 0 for failure
*/
ULONG StpClkDirAuxPwrAt91Pio(UCHAR ucUnit, bool bEnable);

//!   Power control using MCP2317
/**
      \param	ucUnit					      number of the stpclkdir unit
      \param	bEnable					      power state to set (#TRUE == active)
      \return										> 0 for success, == 0 for failure
*/
ULONG StpClkDirAuxPwrMcp2317(UCHAR ucUnit, bool bEnable);

//!   Current control using DACRRCS085
/**
      \param	ucUnit					      number of the stpclkdir unit
      \param	bRun					         current to set (#TRUE == run, #FALSE == hold)
      \return										> 0 for success, == 0 for failure
*/
ULONG StpClkDirAuxDacRrcs085(UCHAR ucUnit, bool bRun);

#endif
