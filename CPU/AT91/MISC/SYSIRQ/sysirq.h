//!	defines and configuration of SYSIRQ peripheral driver
/**
		\file		sysirq.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/SYSIRQ/SYSIRQ.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-10-13 initial version
// 2011-10-19 added public doxygen documentation
// 2013-02-14 changed order of irq sources to AT91SAM7X manual 9.0 page 25 (figure 9.1)
// 2013-02-14 completed define AT91F_SYSIRQLIST_SIZE to cover all user sources
// 2013-03-07 added individual defines for EFC0 and EFC1 flash controllers
// 2013-04-11 renamed AT91F_SYSIRQLIST_SIZE to AT91C_SYSIRQLIST_SIZE
// 2013-04-11 added fallback definition for old name define AT91F_SYSIRQLIST_SIZE
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_SYSIRQ_SYSIRQ_H
#define SYKAM_CPU_AT91_MISC_SYSIRQ_SYSIRQ_H 1


//! <b>Overrideable peripheral control:</b> RealTimeTimer
#ifndef SYSIRQ_RTT_ENABLE
   #define SYSIRQ_RTT_ENABLE  (0)
#endif

//! <b>Overrideable peripheral control:</b> WatchDogTimer
#ifndef SYSIRQ_WDT_ENABLE
   #define SYSIRQ_WDT_ENABLE  (0)
#endif

//! <b>Overrideable peripheral control:</b> DeBuGUnit
#ifndef SYSIRQ_DBGU_ENABLE
   #define SYSIRQ_DBGU_ENABLE (0)
#endif

//! <b>Overrideable peripheral control:</b> PowerManagementControllerUnit
#ifndef SYSIRQ_PMC_ENABLE
   #define SYSIRQ_PMC_ENABLE  (0)
#endif

//! <b>Overrideable peripheral control:</b> ReSeTController
#ifndef SYSIRQ_RSTC_ENABLE
   #define SYSIRQ_RSTC_ENABLE (0)
#endif

//! <b>Overrideable peripheral control:</b> EmbeddedFlashController 0
#ifndef SYSIRQ_EFC0_ENABLE
   #define SYSIRQ_EFC0_ENABLE (0)
#endif

//! <b>Overrideable peripheral control:</b> EmbeddedFlashController 1
#ifndef SYSIRQ_EFC1_ENABLE
   #define SYSIRQ_EFC1_ENABLE (0)
#endif

//! Total size of the list
#define AT91C_SYSIRQLIST_SIZE (SYSIRQ_RTT_ENABLE+ SYSIRQ_WDT_ENABLE+ SYSIRQ_DBGU_ENABLE+ SYSIRQ_PMC_ENABLE+ SYSIRQ_RSTC_ENABLE+ SYSIRQ_EFC0_ENABLE+ SYSIRQ_EFC1_ENABLE)

//! Fallback for old define name
#define AT91F_SYSIRQLIST_SIZE AT91C_SYSIRQLIST_SIZE

// Prototypes
__irq void irq_sys_handler (void);

#endif
