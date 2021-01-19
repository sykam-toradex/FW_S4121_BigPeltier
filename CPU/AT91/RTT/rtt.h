//!	defines and configuration of AT91SAM7X RTT driver
/**
		\file		rtt.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/RTT/RTT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-10-13 initial version
// 2010-10-21 added RTT_IRQ define for irq option
// 2011-10-20 added public doxygen documentation
// 2011-10-26 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_RTT_RTT_H
#define SYKAM_CPU_AT91_RTT_RTT_H 1


// Macros

// Function template IsrRtt
#define RTT_ISR_FUNC(unit) void IsrRtt##unit(void) { RttIrqHandler(unit); }

// Function prototype IsrRtt
#define RTT_ISR_PROTO(unit) void IsrRtt##unit(void)

// Function name IsrRtt
#define RTT_ISR_NAME(unit) IsrRtt##unit

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

//! <b>Overrideable configuration:</b> use sysirq <b>[required (== 1) for tick and alarm message generation]</b>
#ifndef RTT_IRQ
   #define RTT_IRQ 0
#endif

//! <b>Overrideable configuration:</b> maximum bits used for message data (24, 16 or 8)
#ifndef RTT_MSGBITS
   #define RTT_MSGBITS 24
#endif

//! <b>Overrideable configuration:</b> type of each message counter
#ifndef RTT_MSGCOUNTERTYPE
   #define RTT_MSGCOUNTERTYPE USHORT
#endif

//! <b>Overrideable configuration:</b> max value of each message counter (limits are defined in .\\Keil\\ARM\\RV31\\Inc\\limits.h)
#ifndef RTT_MSGCOUNTERMAX
   #define RTT_MSGCOUNTERMAX USHRT_MAX
#endif

//! <b>Overrideable configuration:</b> maximum msg counter index (2 for upto AT91C_RTTC_RTTINC == 2^1, 1 for upto AT91C_RTTC_ALMS == 2^0) <b>[messages with index value above are ignored]</b>
#ifndef RTT_MSGMAX
   #define RTT_MSGMAX 2
#endif

//! <b>Overrideable configuration:</b> number of messages buffered for IRQ to task
#ifndef RTT_MBXCNTMSG
   #define RTT_MBXCNTMSG 16
#endif

//! <b>Overrideable configuration:</b> number of units supported simultaneously
#ifndef RTT_UNIT_MAX
   #define RTT_UNIT_MAX 1
#endif

// Prototypes
#if RTT_IRQ > 0
   __task void TskRttMsg(void *pvParam);
   void RttIrqHandler(UCHAR ucUnit);
   
   // Each supported unit requires a prototype definition here
   #if RTT_UNIT_MAX > 0
      RTT_ISR_PROTO(0);
   #endif
   
   #if RTT_UNIT_MAX > 1
      RTT_ISR_PROTO(1);
   #endif
#endif

#endif
