//!	data definitions of AT91SAM7X ALIVE driver
/**
		\file		alivedata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/ALIVE/ALIVEDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-01-31 initial version
// 2013-02-06 added tidTskMsg to _SAliveRW for task cycle by WDT clock approximation
// 2013-02-07 renamed u8TskPriMsg as u8TskPriAlive
// 2013-02-07 renamed tidTskMsg as tidTskAlive
// 2013-08-22 changed cycles from milliseconds to ticks
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_ALIVE_ALIVEDATA_H
#define SYKAM_CPU_AT91_MISC_ALIVE_ALIVEDATA_H 1


//! \internal \brief <b>Internal data:</b> Task parameter data
struct _SAliveTsk
{
   UCHAR ucUnit;                                         //!< Unit task is assigned to
};

//! \internal \brief <b>Internal data:</b> Triggers for pin(s)
struct _SAliveFactor
{
   UCHAR                ucOn;                            //!< Trigger for pin on <b>(time= factor* u16TskCycle)</b>
   UCHAR                ucOff;                           //!< Trigger for pin off <b>(time= factor* u16TskCycle)</b>
   
};

//! Hardware descriptor data (read only) for each unit
struct _SAliveRO
{
   AT91PS_PIO           pPioBase;                        //!< Base address of pio peripheral to handle
   U32                  u32PinMask;                      //!< Mask for pin(s) to handle
   struct _SAliveFactor SAliveFactor[ALIVE_FACTORCNT];   //!< Triggers factors for pin(s) <b>(time= factor* u16TskCycle)</b>
   AT91PS_WDTC          pWdtBase;                        //!< (optional, set NULL) Base address of wdt peripheral to handle
   UCHAR                ucWdtFactor;                     //!< (optional, set 0) Watchdog confirm trigger factor <b>(interval= factor* u16TskCycle)</b>
   U16                  u16TskCycle;                     //!< Task interval for factors, usually _OS_T_MS(100, OS_TICK_RATE) (used for task interval approximation and direct when SLCK para set to 0) <b>(in ticks)</b>
   U8                   u8TskPriAlive;                   //!< Priority of alive task
};

//! <b>Internal data:</b> Runtime data (read/write)
struct _SAliveRW
{
   OS_SEM               SemRW;                           //!< Semaphore to protect SAliveRW
   USHORT               usAliveIdx;                      //!< Alive index for selecting current #_SAliveFactor values
   USHORT               usPinOffCnt;                     //!< Counter to trigger pin off event when decremeted to zero
   USHORT               usPinOnCnt;                      //!< Counter to trigger pin on event when decremeted to zero
   USHORT               usWdtCnt;                        //!< Counter to trigger watchdog confirmation event when decremeted to zero
   U8                   u8PinEventMask;                  //!< Mask for triggered pin events
   U16                  u16TskCycle;                     //!< True determined task interval (WDT counter clock approximated to specified task interval) <b>(in ticks)</b>
   OS_TID               tidTskAlive;                     //!< TID of alive task
};

struct _SAliveRW SAliveRW[ALIVE_UNIT_MAX];
struct _SAliveTsk SAliveTsk[ALIVE_UNIT_MAX];

#endif
