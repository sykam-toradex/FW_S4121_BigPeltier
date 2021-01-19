//!	hardware descriptor(s) of AT91SAM7X RTT driver
/**
		\file		rttdev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/RTT/RTTDEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-10-13 initial version
// 2010-10-14 changed structures
// 2010-10-21 added RTT_IRQ irq option
// 2011-03-23 removed usCycleMS, usPres & usAdjTicks of _SRttRO
// 2011-03-23 added usSCK to _SRttRO
// 2011-03-23 changed default calibration time span from 250ms=1/4s to 500ms= 1/2s
// 2011-03-24 removed usSCK of _SRttRO (obsolete)
// 2011-10-20 added public doxygen documentation
// 2011-11-17 added digraphs of data for public doxygen documentation
// 2013-04-04 fixed define condition name for descriptor of unit 1
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_RTT_RTTDEV_H
#define SYKAM_CPU_AT91_RTT_RTTDEV_H 1


#if RTT_IRQ > 0
   // Declare arrays of mailboxes (size definitions are located inside rtt.h)

   //!   unit receive message queue
   /**
      data flow thru MbxRttMsg
      \dot
      digraph MbxRttMsg {
         node [shape="record" fontname="FreeSans" fontsize="10"];
         
         Rtt [label="Rtt peripheral" shape="parallelogram"];
         RttIrqHandler [label="RttIrqHandler"];
         MbxRttMsg [label="MbxRttMsg" style="filled"];
         TskRttMsg [label="TskRttMsg" shape="ellipse"];
         xMsgCounter [label="SRttRW.xMsgCounter" URL="\ref _SRttRW"];
         RttMsg [label="RttMsg" shape="invtrapezium" URL="\ref RttMsg"];

         Rtt -> RttIrqHandler [arrowhead="odot"];
         RttIrqHandler -> MbxRttMsg;
         MbxRttMsg -> TskRttMsg [arrowhead="odot"];
         TskRttMsg -> xMsgCounter;
         xMsgCounter -> RttMsg;
      }
      \enddot
   */
   os_mbx_declare_ex(MbxRttMsg, RTT_MBXCNTMSG, [RTT_UNIT_MAX]);
#endif


//! Hardware descriptors
struct _SRttRO SRttRO[RTT_UNIT_MAX] =
{
   {
      /* pBase          */ AT91C_BASE_RTTC,
      /* u32IrqMask     */ AT91C_RTTC_RTTINCIEN | AT91C_RTTC_ALMIEN,
      /* usAdjCycleDiv  */ 2,
      /* u8TskPriMsg    */ OS_P_ABOVEHIGH,
   }
#if RTT_UNIT_MAX > 1
   ,
   {
      /* pBase          */ AT91C_BASE_RTTC,
      /* u32IrqMask     */ AT91C_RTTC_RTTINCIEN | AT91C_RTTC_ALMIEN,
      /* usAdjCycleDiv  */ 2,
      /* u8TskPriMsg    */ OS_P_ABOVEHIGH,
   }   
#endif
};

#endif
