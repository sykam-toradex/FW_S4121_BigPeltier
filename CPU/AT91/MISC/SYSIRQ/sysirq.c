//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/SYSIRQ/SYSIRQ.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-10-13 initial version
// 2010-10-14 added locked & checked sync re-triggering of async triggered irq
// 2010-10-15 removed all obsolete workarrounds because of fixed rtx_config.c
// 2010-10-29 changed LOCK/UNLOCK to DisableIt/EnableIt for better stability
// 2010-10-29 changed to LOCK/Disable UNLOCK/Enable for better stability
// 2010-10-30 added irq clear before re-enable/unlock like in os_irq_ack_lock
// 2010-10-30 added force_irq clear before re-enable like in os_irq_ack_lock
// 2010-12-09 removed timer tick lock-unlock for os scheduler
// 2010-12-09 added force_irq clear-set when set after AT91F_AIC_EnableIt
// 2010-12-16 added AT91F_AIC_ClearIt before AT91F_AIC_EnableIt when !force_irq
// 2011-01-19 removed everything else than EOI
// 2013-03-13 updated irq source comment and manual reference
// 2013-04-11 fixed function header's return data type comments
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "sysirq.h"
#include "sysirqdata.h"

// Externals (located inside AT91SAM7X...c)
extern struct _SSysIrqItem SSysIrqList[];

#if AT91F_SYSIRQLIST_SIZE > 0

//----------------------------------------------------------------------------
// void irq_sys_handler (requires enabled & external options of RTX_Config.c)
//----------------------------------------------------------------------------
// Common system interrupt handler for RTT, WDT, DBGU, PMC, RSTC & EFC0...1 irq
// Refer to AT91SAM7X manual 9.0 page 25 (figure 9.1)
//----------------------------------------------------------------------------
void irq_sys_handler (void) __irq
{
   register UINT uiIndex;
    
   for (uiIndex= 0; uiIndex < AT91F_SYSIRQLIST_SIZE; uiIndex++)
   {
      if (SSysIrqList[uiIndex].Isr)
      {
         SSysIrqList[uiIndex].Isr();
      }
   }
 
   EOI;
}

#endif
