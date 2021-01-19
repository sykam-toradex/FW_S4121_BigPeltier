//!	data definitions of auxiliary package of AT91SAM7X STPCLKDIR driver
/**
		\file		stpclkdirauxdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIRAUXDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-03-24 initial version
// 2011-03-30 added u16TimeoutMS to _SStpClkDirAuxPwrMcp2317RO
// 2011-03-30 added u16TimeoutMS to _SStpClkDirAuxDacRrcs085RO
// 2011-10-19 added public doxygen documentation
// 2013-08-22 changed timeouts from milliseconds to ticks
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRAUXDATA_H
#define SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRAUXDATA_H 1


// Hardware descriptor data (read only) for each unit

//! Hardware descriptor data - Direction control - At91Pio
struct _SStpClkDirAuxDirAt91PioRO
{
   AT91PS_PIO        	pBase;                           //!< Pio base to handle
   U32						u32PinDirMask;                   //!< Pin mask of direction control
   bool                 bPinDirClockwise;                //!< Pin direction level for clockwise
};

//! Hardware descriptor data - Power control - At91Pio
struct _SStpClkDirAuxPwrAt91PioRO
{
   AT91PS_PIO        	pBase;                           //!< Pio base to handle
   U32						u32PinPwrMask;                   //!< Pin mask of power stage control
   bool                 bPinPwrRun;                      //!< Pin run level of reset control  
};

//! Hardware descriptor data - Power control - Mcp2317
struct _SStpClkDirAuxPwrMcp2317RO
{
   UCHAR                ucUnit;                          //!< Gpio unit number to access
   U16                  u16PinPwrMask;                   //!< Gpio pin mask of reset control
   U16                  u16PinPwrRun;                    //!< Gpio pin run level of reset control
   U16                  u16Timeout;                      //!< Gpio maximum cpu-load free resource pending time before any resource claim is canceled by the rtos <b>(in ticks)</b>
};

//! Hardware descriptor data - Current control - DacRrcs085
struct _SStpClkDirAuxDacRrcs085RO
{
   UCHAR                ucUnit;                          //!< Dac unit number to access
   UCHAR                ucChannel;                       //!< Dac channel number to access
   U16                  u16Timeout;                      //!< Dac maximum cpu-load free resource pending time before any resource claim is canceled by the rtos <b>(in ticks)</b>
};

#endif
