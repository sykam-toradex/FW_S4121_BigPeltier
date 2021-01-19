//!	defines and configuration of AT91SAM7X STPCLKDIR driver
/**
		\file		stpclkdir.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIR.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-15 initial version
// 2010-01-21 fixed macro STPCLKDIR_CHID2UNIT
// 2010-01-21 added defines STPCLKDIR_CLOCKWISE and STPCLKDIR_COUNTERCLOCKWISE
// 2010-01-21 added defines STPCLKDIR_MBXCNTACC and STPCLKDIR_MBXCNTBRK
// 2010-02-05 added prototypes for StpClkDirTccIrqHandler... functions
// 2010-02-10 changed prototype of StpClkDirTccIrqHandlerStop
// 2010-02-11 increased STPCLKDIR_MBXCNTACC & STPCLKDIR_MBXCNTNRK to 128
// 2010-02-12 increased STPCLKDIR_MBXCNTACC & STPCLKDIR_MBXCNTNRK to 512
// 2010-02-26 added STPCLKDIR_FLAGS_AUTOPOWEROFF
// 2010-03-12 added STPCLKDIR_FLAGS_INVERSE & STPCLKDIR_FLAGS_NONE
// 2010-03-12 added STPCLKDIR_CLKSxxx_MIN
// 2010-07-01 moved STPCLKDIR_CHID2UNIT/UNIT2CHID to pwm.h
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-10-20 added optional define STPCLKDIR_PWM_UPDATE_FIX
// 2010-11-16 added optional define STPCLKDIR_FULLSTEPSHIFT
// 2010-11-16 added new re-configurable define STPCLKDIR_MBXOFFSETCNTMSG
// 2010-11-19 moved IrqHandler support function to stpclkdirint.h
// 2010-11-10 added optional define STPCLKDIR_OFFSETBYDIRECTION
// 2011-01-27 raised stack size STPCLKDIR_STACK_STKTSKSTPCLKDIRHOLD to 512
// 2011-09-16 added optional define STPCLKDIR_EDGEDRIFTMAX
// 2011-06-16 added optional define STPCLKDIR_POWERONRESET
// 2011-09-28 added optional define STPCLKDIR_PWM_2NDCHANCEUPDATE_FIX
// 2011-09-28 removed obsolete optional define STPCLKDIR_EDGEDRIFTMAX
// 2011-09-28 added optional define STPCLKDIR_OFFSETAUTOSHIFT
// 2011-10-19 added public doxygen documentation
// 2013-08-21 changed default of STPCLKDIR_PWM_2NDCHANCEUPDATE_FIX to 0
// 2013-08-21 updated public doxygen documentation
// 2014-11-25 added masking & uchar cast in wrapper to StpClkDirPwmIrqHandler
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIR_H
#define SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIR_H 1


// Defines

//! <b>Direction:</b> clockwise
#define STPCLKDIR_CLOCKWISE            (TRUE)

//! <b>Direction:</b> counter-clockwise
#define STPCLKDIR_COUNTERCLOCKWISE     (FALSE)

//! <b>Move:</b> min acceleration clocks
#define STPCLKDIR_CLKSACC_MIN          (2)

//! <b>Move:</b> min brake clocks
#define STPCLKDIR_CLKSBRK_MIN          (1)

//! <b>Move:</b> min run clocks (at least the sum of acceleration and brake)
#define STPCLKDIR_CLKSRUN_MIN          (STPCLKDIR_CLKSACC_MIN+ STPCLKDIR_CLKSBRK_MIN)

//! <b>Flag/option:</b> all options off/disabled
#define STPCLKDIR_FLAGS_NONE           (0)

//! <b>Flag/option:</b> power off drive after move
#define STPCLKDIR_FLAGS_AUTOPOWEROFF   (1 << 0)

//! <b>Flag/option:</b> inverse motor direction (e.g. clockwise bit will turn counter-clockwise)
#define STPCLKDIR_FLAGS_INVERSE        (1 << 1)


// Macros

// Function template IsrStpClkDirTcc
#define STPCLKDIR_ISRTCC_FUNC(unit) void IsrStpClkDirTcc##unit(void) __irq { StpClkDirTccIrqHandler(unit); }

// Function prototype IsrStpClkDirTcc
#define STPCLKDIR_ISRTCC_PROTO(unit) void IsrStpClkDirTcc##unit(void) __irq

// Function name IsrStpClkDirTcc
#define STPCLKDIR_ISRTCC_NAME(unit) IsrStpClkDirTcc##unit


// Function template IsrStpClkDirPwm
#define STPCLKDIR_ISRPWM_FUNC void IsrStpClkDirPwm(U32 u32ISR, U32 u32Pin) { StpClkDirPwmIrqHandler((UCHAR)(u32Pin & BITS_ULONGLL)); }

// Function prototype IsrStpClkDirPwm
#define STPCLKDIR_ISRPWM_PROTO void IsrStpClkDirPwm(U32 u32ISR, U32 u32Pin)

// Function name IsrStpClkDirPwm
#define STPCLKDIR_ISRPWM_NAME IsrStpClkDirPwm

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

//! <b>Overrideable configuration:</b> power on reset of hardware driver before every move
#ifndef STPCLKDIR_POWERONRESET
   #define STPCLKDIR_POWERONRESET 0
#endif

//! <b>Overrideable configuration:</b> full step shift to avoid position error at power off
#ifndef STPCLKDIR_FULLSTEPSHIFT
   #define STPCLKDIR_FULLSTEPSHIFT 1
#endif

//! <b>Overrideable configuration:</b> control auto offset shift after move from received edge offset(s)
#ifndef STPCLKDIR_OFFSETAUTOSHIFT
   #define STPCLKDIR_OFFSETAUTOSHIFT 1
#endif

//! <b>Overrideable configuration:</b> offset inversion by direction
#ifndef STPCLKDIR_OFFSETBYDIRECTION
   #define STPCLKDIR_OFFSETBYDIRECTION 0
#endif

//! <b>Overrideable configuration:</b> pwm 2nd chance preventive update error fix
#ifndef STPCLKDIR_PWM_2NDCHANCEUPDATE_FIX
   #define STPCLKDIR_PWM_2NDCHANCEUPDATE_FIX 0
#endif

//! <b>Overrideable configuration:</b> pwm update fix (Errata rev. a 41.3.6 p.653) [count of retries]
#ifndef STPCLKDIR_PWM_UPDATE_FIX
   #define STPCLKDIR_PWM_UPDATE_FIX 0
#endif

//! <b>Overrideable configuration:</b> default stack size for TskStpClkDirHold
#ifndef STPCLKDIR_STACK_STKTSKSTPCLKDIRHOLD
   #define STPCLKDIR_STACK_STKTSKSTPCLKDIRHOLD 512
#endif

//! <b>Overrideable configuration:</b> number of offset corrections buffered
#ifndef STPCLKDIR_MBXOFFSETCNTMSG
   #define STPCLKDIR_MBXOFFSETCNTMSG 25
#endif

//! <b>Overrideable configuration:</b> number of state changes buffered
#ifndef STPCLKDIR_MBXCNTSTATE
   #define STPCLKDIR_MBXCNTSTATE 4
#endif

//! <b>Overrideable configuration:</b> number of units supported simultaneously
#ifndef STPCLKDIR_UNIT_MAX
   #define STPCLKDIR_UNIT_MAX 1
#endif

//! <b>Overrideable configuration:</b> number of entries for acceleration mailbox
#ifndef STPCLKDIR_MBXCNTACC
   #define STPCLKDIR_MBXCNTACC 512
#endif

//! <b>Overrideable configuration:</b> number of entries for brake mailbox
#ifndef STPCLKDIR_MBXCNTBRK
   #define STPCLKDIR_MBXCNTBRK 512
#endif

// Prototypes
void StpClkDirTccIrqHandler(UCHAR ucUnit);
void StpClkDirPwmIrqHandler(UCHAR ucChid);

__task void TskStpClkDirHold(void *pvParam);

// Each supported unit requires a prototype definition here
#if STPCLKDIR_UNIT_MAX > 0
   STPCLKDIR_ISRTCC_PROTO(0);
   STPCLKDIR_ISRPWM_PROTO;
#endif

#if STPCLKDIR_UNIT_MAX > 1
   STPCLKDIR_ISRTCC_PROTO(1);
#endif

#if STPCLKDIR_UNIT_MAX > 2
   STPCLKDIR_ISRTCC_PROTO(2);
#endif

#endif
