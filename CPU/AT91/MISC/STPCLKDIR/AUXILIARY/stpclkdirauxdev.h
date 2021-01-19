//!	hardware descriptor(s) of auxiliary package of AT91SAM7X STPCLKDIR driver
/**
		\file		stpclkdirauxdev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIRAUXDEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-03-23 initial version
// 2011-03-30 added u16TimeoutMS to _SStpClkDirAuxPwrMcp2317RO
// 2011-03-30 added u16TimeoutMS to _SStpClkDirAuxDacRrcs085RO
// 2011-10-19 added public doxygen documentation
// 2013-04-24 changed default port masks for ...AuxDirAt91PioRO / ...AuxPwrAt91PioRO
// 2013-08-22 changed timeouts from milliseconds to ticks (_OS_T_MS for compatibility)
// 2014-11-25 changed defaults of SStpClkDirAuxDacRrcs085RO for unit [1] & [2]
// 2014-11-25 changed defaults of SStpClkDirAuxDirAt91PioRO for unit [1]
// 2014-11-25 changed defaults of SStpClkDirAuxPwrMcp2317RO for unit [1] & [2]
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRAUXDEV_H
#define SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRAUXDEV_H 1


// Hardware descriptors

//! Hardware descriptor data - Direction control - At91Pio
struct _SStpClkDirAuxDirAt91PioRO SStpClkDirAuxDirAt91PioRO[STPCLKDIR_UNIT_MAX] =
{
   {
      /* pBase		   	     */ AT91C_BASE_PIOB,
      /* u32PinDirMask       */ AT91C_PIO_PB24,
      /* bPinDirClockwise    */ 1
   }
#if STPCLKDIR_UNIT_MAX > 1
   ,
   {
      /* pBase		   	     */ AT91C_BASE_PIOB,
      /* u32PinDirMask       */ AT91C_PIO_PB26,
      /* bPinDirClockwise    */ 1
   }
#endif
#if STPCLKDIR_UNIT_MAX > 2
   ,
   {
      /* pBase		   	     */ AT91C_BASE_PIOB,
      /* u32PinDirMask       */ AT91C_PIO_PB28,
      /* bPinDirClockwise    */ 1
   }
#endif
};

//! Hardware descriptor data - Power control - At91Pio
struct _SStpClkDirAuxPwrAt91PioRO SStpClkDirAuxPwrAt91PioRO[STPCLKDIR_UNIT_MAX] =
{
   {
      /* pBase		   	     */ AT91C_BASE_PIOB,
      /* u32PinPwrMask       */ AT91C_PIO_PB26,
      /* bPinPwrRun          */ 1
   }
#if STPCLKDIR_UNIT_MAX > 1
   ,
   {
      /* pBase		   	     */ AT91C_BASE_PIOB,
      /* u32PinPwrMask       */ AT91C_PIO_PB29,
      /* bPinPwrRun          */ 1
   }
#endif
#if STPCLKDIR_UNIT_MAX > 2
   ,
   {
      /* pBase		   	     */ AT91C_BASE_PIOB,
      /* u32PinPwrMask       */ AT91C_PIO_PB29,
      /* bPinPwrRun          */ 1
   }
#endif
};

//! Hardware descriptor data - Power control - Mcp2317
struct _SStpClkDirAuxPwrMcp2317RO SStpClkDirAuxPwrMcp2317RO[STPCLKDIR_UNIT_MAX] =
{
   {
      /* ucUnit              */ 0,
      /* u16PinPwrMask       */ /* 0, */ MCP2317_PA7, 
      /* u16PinPwrRun        */ MCP2317_PA7,
      /* u16Timeout          */ _OS_T_MS(600, OS_TICK_RATE)
   }
#if STPCLKDIR_UNIT_MAX > 1
   ,
   {
      /* ucUnit              */ 0,
      /* u16PinPwrMask       */ /* 0, */ MCP2317_PB0, 
      /* u16PinPwrRun        */ MCP2317_PB0,
      /* u16Timeout          */ _OS_T_MS(600, OS_TICK_RATE)
   }
#endif
#if STPCLKDIR_UNIT_MAX > 2
   ,
   {
      /* ucUnit              */ 0,
      /* u16PinPwrMask       */ /* 0, */ MCP2317_PB1, 
      /* u16PinPwrRun        */ MCP2317_PB1,
      /* u16Timeout          */ _OS_T_MS(600, OS_TICK_RATE)
   }
#endif
};

//! Hardware descriptor data - Current control - DacRrcs085
struct _SStpClkDirAuxDacRrcs085RO SStpClkDirAuxDacRrcs085RO[STPCLKDIR_UNIT_MAX] =
{
   {
      /* ucUnit              */ 0,
      /* ucChannel           */ 0,
      /* u16Timeout          */ _OS_T_MS(600, OS_TICK_RATE)
   }
#if STPCLKDIR_UNIT_MAX > 1
   ,
   {
      /* ucUnit              */ 0,
      /* ucChannel           */ 1,
      /* u16Timeout          */ _OS_T_MS(600, OS_TICK_RATE)
   }
#endif
#if STPCLKDIR_UNIT_MAX > 2
   ,
   {
      /* ucUnit              */ 0,
      /* ucChannel           */ 2,
      /* u16Timeout          */ _OS_T_MS(600, OS_TICK_RATE)
   }
#endif
};

#endif
