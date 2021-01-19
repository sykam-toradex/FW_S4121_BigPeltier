//!	defines and configuration of AT91SAM7X PWM driver
/**
		\file		pwm.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PWM/PWM.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-15 initial version
// 2010-01-28 added PWM_CALC_DTYMIN
// 2010-07-01 added PWM_CHANNEL_MAX and PWM_CHANNEL_BUFFERCNTTX
// 2010-07-01 added PWM_CHANNEL_MAXTX
// 2010-07-01 added PWM32_CMR16_PRDDTY16, PWM32_GET_CMR16 & PWM32_GET_PRDDTY16
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-23 changed to _u(l)min & _u(l)max intrinsics
// 2010-10-15 added optional define PWM_UPDATE_FIX
// 2010-10-20 changed PWM_UPDATE_FIX to 0 (disabled)
// 2011-03-10 added new hardware bug fix option PWM_CPRE_FIX
// 2011-05-10 fixed macro PWM32_CMR16_PRDDTY16 to accept also UCHARs & USHORTs
// 2011-09-28 added optional define PWM_2NDCHANCEUPDATE_FIX
// 2011-10-20 added public doxygen documentation
// 2011-11-30 removed obsolete macro PWM_CALC_DTYMIN
// 2013-04-11 added fallback configuration for AT91C_MCK
// 2013-08-22 changed default of PWM_2NDCHANCEUPDATE_FIX to 0
// 2013-08-22 updated public doxygen documentation
// 2013-10-23 updated public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PWM_PWM_H
#define SYKAM_CPU_AT91_PWM_PWM_H 1


// Macros

//! <b>Macro:</b> convert CHID (1 << unit) to channel unit number (0...3)
#define PWM_CHID2UNIT(chid) (_umin(((chid) & 0x0F) >> 1, 0x03))

//! <b>Macro:</b> convert channel unit number (0...3) to CHID (1 << unit)
#define PWM_UNIT2CHID(unit) (1 << (unit))


// Function template IsrPwm
#define PWM_ISR_FUNC(unit) void IsrPwm##unit(void) __irq { PwmIrqHandler(unit); }

// Function prototype IsrPwm
#define PWM_ISR_PROTO(unit) void IsrPwm##unit(void) __irq

// Function name IsrPwm
#define PWM_ISR_NAME(unit) IsrPwm##unit


// Function template IsrPwmChannel
#define PWM_CHANNEL_ISR_FUNC void IsrPwmChannel(U32 u32ISR, U32 u32Pin) { PwmChannelIrqHandler(u32Pin); }

// Function prototype IsrPwmChannel
#define PWM_CHANNEL_ISR_PROTO void IsrPwmChannel(U32 u32ISR, U32 u32Pin)

// Function name IsrPwmChannel
#define PWM_CHANNEL_ISR_NAME IsrPwmChannel


//! <b>Macro:</b> pack lower 16 bit of channel-mode-register and the 16 bit of duty <b>OR</b> period into 32 bit value
#define PWM32_CMR16_PRDDTY16(cmr, prddty) ((((U32)(cmr)) << 16) | ((U32)(prddty)))

//! <b>Macro:</b> extract lower 16 bit of channel-mode-register from packed 32 bit value
#define PWM32_GET_CMR16(value) (((value) & BITS_ULONGHX) >> 16)

//! <b>Macro:</b> extract 16 bit of duty <b>OR</b> period from packed 32 bit value
#define PWM32_GET_PRDDTY16(value) ((value) & BITS_ULONGLX)

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

//! <b>Fallback configuration:</b> Define AT91C_MCK ([B]it, [C]onstant, [D]efinition, [F]unction) by obsolete AT91B_MCK
#ifndef AT91C_MCK
   #define AT91C_MCK AT91B_MCK
#endif

//! <b>Overrideable configuration:</b> hardware bug fix for cpre equal to zero stops and blocks pwm
#ifndef PWM_CPRE_FIX
   #define PWM_CPRE_FIX 1
#endif

//! <b>Overrideable configuration:</b> pwm 2nd chance preventive update error fix
#ifndef PWM_2NDCHANCEUPDATE_FIX
   #define PWM_2NDCHANCEUPDATE_FIX 0
#endif

//! <b>Overrideable configuration:</b> hardware bug fix for pwm update (Errata rev. a 41.3.6 p.653) [count of retries]
#ifndef PWM_UPDATE_FIX
   #define PWM_UPDATE_FIX 0
#endif

//! <b>Overrideable configuration:</b> number of units supported simultaneously
#ifndef PWM_UNIT_MAX
   #define PWM_UNIT_MAX 1
#endif

//! <b>Overrideable configuration:</b> number of channels supported simultaneously (0= none) per unit
#ifndef PWM_CHANNEL_MAX
   #define PWM_CHANNEL_MAX 0
#endif

//! <b>Overrideable configuration:</b> buffer holding task tx data
#ifndef PWM_CHANNEL_BUFFERCNTTX
   #define PWM_CHANNEL_BUFFERCNTTX 16
#endif

//! <b>Overrideable configuration:</b> maximum value count for each tx transfer
#ifndef PWM_CHANNEL_MAXTX
   #define PWM_CHANNEL_MAXTX 4
#endif

// Prototypes
void PwmChannelIrqHandler(UCHAR ucChid);
__task void TskPwmChannelTx(void *pvParam);

// Each supported unit requires a prototype definition here
#if PWM_UNIT_MAX > 0
   PWM_ISR_PROTO(0);
#if PWM_CHANNEL_MAX > 0   
   PWM_CHANNEL_ISR_PROTO;
#endif
#endif

#if PWM_UNIT_MAX > 1
   PWM_ISR_PROTO(1);
#endif

#endif
