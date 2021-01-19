//!	data definitions of SYSIRQ peripheral driver
/**
		\file		sysirqdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/SYSIRQ/SYSIRQDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-10-13 initial version
// 2010-11-23 added data comment
// 2011-10-19 added public doxygen documentation
// 2011-11-23 added digraphs of data for public doxygen documentation
// 2013-03-13 updated irq source doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_SYSIRQ_SYSIRQDATA_H
#define SYKAM_CPU_AT91_MISC_SYSIRQ_SYSIRQDATA_H 1


//! List structure item (for building list array struct _SSysIrqItem SSysIrqList[AT91F_SYSIRQLIST_SIZE] placed in "at91sam7x.c")
/**
   data flow thru SSysIrqList
   \dot
   digraph SSysIrqList {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      SysPeripheral [label="{System peripheral | {RTT\nReal Time Timer | WDT\nWatch Dog Timer | DBGU\nDeBuG Unit | PMC\nPower Management Controller | RSTC\nReSeT Controller | EFC0...1\nEmbedded Flash Controller}}"];
      SysIrq [label="System IRQ"];
      SysIrqHandler [label="irq_sys_handler"];
      SSysIrqList [label="SSysIrqList" style="filled"];
      User [label="User irq handler(s)" shape="ellipse" style="dashed" color="dimgray" style="dashed"];

      SysPeripheral -> SysIrq [arrowhead="odot"];
      SysIrq -> SysIrqHandler;
      SysIrqHandler -> SSysIrqList;
      SSysIrqList -> User;
   }
   \enddot
   
      <b>Example code: the following array is REQUIRED in "at91sam7x.c"</b><br>
      \include _SSysIrqItem.h
      
      \note
               - <b>user irq handler(s) have to check the source of the sys irq itsself BY CHECKING THEIR PERIPHERAL THEY'RE RESPONSIBLE FOR!!!</b>
               - <b>user irq handler(s) have to clear ALWAYS (!!!) the sys irq source they're responsible for (ALSO WHEN INACTIVE)</b>
               - <b>user irq handler(s) MUST _NOT_ ACKNOWLEDGE the sys irq!!!</b>
*/
struct _SSysIrqItem
{
   void (*Isr) (void);                    //!< Isr handler function to call
};

#endif
