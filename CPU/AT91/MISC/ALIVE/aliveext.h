//!	external interface of AT91SAM7X ALIVE driver
/**
		\file		aliveext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/ALIVE/ALIVEEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-01-31 initial version
// 2013-02-06 added SLCK to AliveInit for task cycle by WDT clock approximation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_ALIVE_ALIVEEXT_H
#define SYKAM_CPU_AT91_MISC_ALIVE_ALIVEEXT_H 1


// Prototypes for functions

//!   init a unit
/**
      \param	ucUnit					      number of the unit (specifies descriptor array index <b>[not the hardware index]</b> of alivedev.h)
      \param	u32SLCK					      (optional, set to 0) <b>but highly recommended</b> frequency of the retrieved SLow ClocK for task cycle by WDT clock approximation

      \note
         - the frequency of the SLow ClocK is different from chip to chip and is determined by #RttRestart which offers the measured frequency by #RttSLCKGet
         - the task cycle by WDT clock approximation feature gives accurate matching for the watchdog timer confirmation (only ~4 ms resolution) to avoid unexspected system reset by WDT
         - 1 task is spanned by this function which triggers the specified led(s) with optional watchdog confirmation

      \return										> 0 for success, == 0 for failure      
*/
bool   AliveInit(UCHAR ucUnit, U32 u32SLCK);


#endif
