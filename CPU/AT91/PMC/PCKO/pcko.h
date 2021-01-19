//!	defines and configuration of AT91SAM7X PCKO driver
/**
		\file		pcko.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PMC/PCKO/PCKO.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-10-23 initial version
// 2013-10-30 initial revision
// 2013-10-31 second revision
// 2013-11-20 reduced PCKO_CHANNEL_MAX to 3 (current boards supports CLK1...3 only)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PMC_PCKO_PCKO_H
#define SYKAM_CPU_AT91_PMC_PCKO_PCKO_H 1


// Macros

//! <b>Macro:</b> convert CHID (1 << (8+ unit)) to channel unit/number (0...3)
#define PCKO_CHID2UNIT(chid) (_umin(((chid) & 0x0F00) >> 9, 0x03))

//! <b>Macro:</b> convert channel unit/number (0...3) to CHID (1 << (unit+ 8))
#define PCKO_UNIT2CHID(unit) (1 << ((unit)+ 8))


// Function template IsrPcko
#define PCKO_ISR_FUNC(unit) void IsrPcko##unit(void) { PckoIrqHandler(unit); }

// Function prototype IsrPcko
#define PCKO_ISR_PROTO(unit) void IsrPcko##unit(void)

// Function name IsrPcko
#define PCKO_ISR_NAME(unit) IsrPcko##unit



//! <b>Macro:</b> pack 3 bit (P)(r)(e)(s)caler and 2 bit (C)lock (S)ource (S)election into 8 bit (P)rogrammable (C)lock (R)egister value
#define PCKO_PCR8_PRES3_CSS2(pres, css) ((((U8)((pres) & AT91C_PMC_PRES)) << 2) | ((U8)((css) & AT91C_PMC_CSS)))

//! <b>Macro:</b> extract lower 4...2 bit of (P)(r)(e)(s)caler from packed 8 bit value
#define PCKO_PCR8_GET_PRES3(value) (((value) & AT91C_PMC_PRES) >> 2)

//! <b>Macro:</b> extract lower 1...0 bit of (C)lock (S)ource (S)election from packed 8 bit value
#define PCKO_PCR8_GET_CSS2(value) ((value) & AT91C_PMC_CSS)


//! <b>Trigger message bit to send/give attached semaphore(s):</b> Summary of individual (R)ea(d)(y) flags
#define AT91C_PMC_PCKX (AT91C_PMC_PCK0RDY | AT91C_PMC_PCK1RDY | AT91C_PMC_PCK2RDY | AT91C_PMC_PCK3RDY)

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

//! <b>Fallback configuration:</b> Define AT91C_MCK ([B]it, [C]onstant, [D]efinition, [F]unction) by obsolete AT91B_MCK
#ifndef AT91C_MCK
   #define AT91C_MCK AT91B_MCK
#endif

//! <b>Overrideable configuration:</b> number of units supported simultaneously
#ifndef PCKO_UNIT_MAX
   #define PCKO_UNIT_MAX 1
#endif

//! <b>Overrideable configuration:</b> number of channels supported simultaneously per unit
#ifndef PCKO_CHANNEL_MAX
   #define PCKO_CHANNEL_MAX 3
#endif

// Prototypes
void PckoIrqHandler(UCHAR ucUnit);

// Each supported unit requires a prototype definition here
#if PCKO_UNIT_MAX > 0
   PCKO_ISR_PROTO(0);
#endif

#if PCKO_UNIT_MAX > 1
   PCKO_ISR_PROTO(1);
#endif

#endif
