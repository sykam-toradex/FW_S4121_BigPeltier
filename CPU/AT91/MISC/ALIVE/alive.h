//!	defines and configuration of AT91SAM7X ALIVE driver
/**
		\file		alive.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/ALIVE/ALIVE.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-01-31 initial version
// 2013-02-07 ALIVE_STACK_TSKALIVE is now an option
// 2016-03-31 added defaults for AT91D_BASE_PIO_LED & AT91B_LED_ALIVE
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_ALIVE_ALIVE_H
#define SYKAM_CPU_AT91_MISC_ALIVE_ALIVE_H 1


// Macros

// Defines

#define ALIVE_EVENT_PIN_OFF   (1 << 0)
#define ALIVE_EVENT_PIN_ON    (1 << 1)
#define ALIVE_EVENT_PIN_MASK  (ALIVE_EVENT_PIN_ON | ALIVE_EVENT_PIN_OFF)

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

//! <b>Overrideable configuration:</b> default pio base for alive led
#ifndef AT91D_BASE_PIO_LED
   #define AT91D_BASE_PIO_LED AT91C_BASE_PIOA
#endif

//! <b>Overrideable configuration:</b> default bit mask for active alive led
#ifndef AT91B_LED_ALIVE
   #define AT91B_LED_ALIVE AT91C_PIO_PA2
#endif

//! <b>Overrideable configuration:</b> default stack size for TskAlive (set to 0 to use default system stack and default size, refer to RTX_Config.c)
#ifndef ALIVE_STACK_TSKALIVE
   #define ALIVE_STACK_TSKALIVE 300
#endif

//! <b>Overrideable configuration:</b> hold delay count for pin(s)
#ifndef ALIVE_FACTORCNT
   #define ALIVE_FACTORCNT 2
#endif

//! <b>Overrideable configuration:</b> number of units supported simultaneously
#ifndef ALIVE_UNIT_MAX
   #define ALIVE_UNIT_MAX 1
#endif

// Prototypes

__task void TskAlive(void *pvParam);

#endif
