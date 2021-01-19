//!	defines and configuration of AT91SAM7X PIO driver
/**
		\file		pio.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PIO/PIO.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-11-06 initial version
// 2009-12-23 added board\AT91SAM7X.h for board specific configuration
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-10-20 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PIO_PIO_H
#define SYKAM_CPU_AT91_PIO_PIO_H 1


// Macros

// Function template IsrPio
#define PIO_ISR_FUNC(unit) void IsrPio##unit(void) __irq { PioIrqHandler(unit); }

// Function prototype IsrPio
#define PIO_ISR_PROTO(unit) void IsrPio##unit(void) __irq

// Function name IsrPio
#define PIO_ISR_NAME(unit) IsrPio##unit


//! <b>Overrideable configuration:</b> Number of units supported simultaneously
#ifndef PIO_UNIT_MAX
   #define PIO_UNIT_MAX 2
#endif

// Each supported unit requires a prototype definition here
#if PIO_UNIT_MAX > 0
   PIO_ISR_PROTO(0);
#endif

#if PIO_UNIT_MAX > 1
   PIO_ISR_PROTO(1);
#endif

struct _SPioRO SPioRO[PIO_UNIT_MAX];

#endif
