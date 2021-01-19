//!	hardware descriptor(s) of AT91SAM7X ALIVE driver
/**
		\file		alivedev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/ALIVE/ALIVEDEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-01-31 initial version
// 2013-02-07 fixed comment for ucWdtFactor
// 2013-02-07 ALIVE_STACK_TSKALIVE is now an option
// 2013-04-04 fixed define condition name for descriptor of unit 1
// 2013-08-22 changed cycles from milliseconds to ticks (_OS_T_MS for compatibility)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_ALIVE_ALIVEDEV_H
#define SYKAM_CPU_AT91_MISC_ALIVE_ALIVEDEV_H 1


// Declare arrays of mailboxes (size definitions are located inside alive.h)


// Customized stacks
#if ALIVE_STACK_TSKALIVE > 0
   OS_TSK_STACK_USER_EX(StkTskAlive, ALIVE_STACK_TSKALIVE, [ALIVE_UNIT_MAX]);
#endif

//! Hardware descriptors
struct _SAliveRO SAliveRO[ALIVE_UNIT_MAX] =
{
   {
      /* pPioBase       */ AT91D_BASE_PIO_LED,
      /* u32PinMask     */ AT91B_LED_ALIVE,
      /* SAliveFactor[] */ { /* on, off trigger (pin!) - led may be inverted! */
                              { 3+ 9, 3 }, /* inverted-led on for 900 ms, off for 300 ms -> on after 3 runs at 100ms , off after 3+ 9 runs at 100ms */
#if ALIVE_FACTORCNT > 1
                              { 2+ 3, 2 }  /* inverted-led  on for 300 ms, off for 200 ms -> on after 2 runs at 100ms, off after 2+ 3 runs at 100ms */
#endif
                           },
      /* pWdtBase       */ AT91C_BASE_WDTC,
      /* ucWdtFactor    */ 40,             /* set factor delay longer* than WDD (and WDD < WDV) [WDV= abs reload {max time for confirmation}, WDD= abs delay {forbid too fast confirmation}, 1s* = ~256 counter-down-ticks {*:add suitable drift to avoid unexspected reset}, refer to SAM7.s and AT91SAM7X manual 16.2 page 85] */
      /* u16TskCycle    */ _OS_T_MS(100, OS_TICK_RATE),
      /* u8TskPriMsg    */ OS_P_LOW,
   }
#if ALIVE_UNIT_MAX > 1
   ,
   {
      /* pPioBase       */ AT91D_BASE_PIO_LED2,
      /* u32PinMask     */ AT91B_LED_ALIVE2,
      /* SAliveHold[]   */ { /* on, off trigger (pin!) - led may be inverted! */
                              { 3+ 9, 3 }, /* inverted-led on for 900 ms, off for 300 ms -> on after 3 runs at 100ms off after 3+ 9 runs at 100ms */
#if ALIVE_FACTORCNT > 1
                              { 2+ 3, 2 }  /* inverted-led on for 300 ms, off for 200 ms -> on after 2 runs at 100ms, off after 3+ 3 runs at 100ms */
#endif
                           },
      /* pWdtBase       */ NULL,
      /* ucWdtFactor    */ 40,             /* set factor delay longer* than WDD (and WDD < WDV) [WDV= abs reload {max time for confirmation}, WDD= abs delay {forbid too fast confirmation}, 1s* = ~256 counter-down-ticks {*:add suitable drift to avoid unexspected reset}, refer to SAM7.s and AT91SAM7X manual 16.2 page 85] */
      /* u16TskCycle    */ _OS_T_MS(100, OS_TICK_RATE),
      /* u8TskPriMsg    */ OS_P_LOW,
   }
#endif
};

#endif
