//!	hardware descriptor(s) of ENCGAPDISK driver
/**
		\file		encgapdiskdev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/MISC/ENCGAPDISK/ENCGAPDISKDEV.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-03-03 initial version
// 2010-03-04 removed ulClksStableHz
// 2010-03-10 added ucGapCnt to _SEncGapDiskRO
// 2010-03-17 added ucThreshold to _SEncGapDiskRO
// 2010-04-01 added ulClkInitHz to _SEncGapDiskRO
// 2010-05-06 added defines for ENCGAPDISK
// 2010-05-07 increased ucTskPriMsg to OS_P_ABOVENORMAL
// 2010-05-12 moved bContinous from ...RO to ...RW
// 2010-05-12 moved ulClkLowHz, ulClkHighHz & ulClks from ...RO to ...RW
// 2010-05-12 moved usClksAcc & usClksBrk from ...RO to ...RW
// 2010-05-12 moved usToleranceSteps &  ucThreshold from ...RO to ...RW
// 2010-05-12 moved usEdgeDriftMax from ...RO to ...RW
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-16 moved ulClksInit & ulClkInitHz from ...RO to ...RW
// 2010-09-29 rr: added ENCGAPDISKx_SCAN_CYCLES, RETRY_CYCLES, VERIFY_CYCLES
// 2011-11-02 added public doxygen documentation
// 2011-11-23 added digraphs of data for public doxygen documentation
// 2013-08-22 changed timeout from milliseconds to ticks (_OS_T_MS for compatibility)
// 2016-06-08 added u32GateMask definitions to _SEncGapDiskRO
// 2016-06-09 added support for different port group signal gate source to _SEncGapDiskRO
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISKDEV_H
#define SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISKDEV_H


// Declare arrays of mailboxes (size definitions are located inside encgapdisk.h)

//!   edge queue from irq to TskEncGapDiskMsg
/**
   data flow thru MbxEncGapDiskMsg
   \dot
   digraph MbxEncGapDiskMsg {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Pin [label="I/O pin irq" shape="parallelogram"];
      IsrEncGapDiskIrqHandler [label="IsrEncGapDiskIrqHandler"];
      MbxEncGapDiskMsg [label="MbxEncGapDiskMsg" style="filled"];
      TskEncGapDiskMsg [label="TskEncGapDiskMsg" shape="ellipse"];
      
      Pin -> IsrEncGapDiskIrqHandler [arrowhead="odot"];
      IsrEncGapDiskIrqHandler -> MbxEncGapDiskMsg;
      MbxEncGapDiskMsg -> TskEncGapDiskMsg [arrowhead="odot"];      
   }
   \enddot
*/ 
os_mbx_declare_ex(MbxEncGapDiskMsg, ENCGAPDISK_MBXCNTMSG, [ENCGAPDISK_UNIT_MAX]);


// Customized stacks
OS_TSK_STACK_USER_EX(StkTskEncGapDiskMsg, ENCGAPDISK_STACK_STKTSKENCGAPDISKMSG, [ENCGAPDISK_UNIT_MAX]);

//! Hardware descriptors
struct _SEncGapDiskRO SEncGapDiskRO[ENCGAPDISK_UNIT_MAX] =
{
   {
      /* ucStpClkDirUnit    */ 0,
      /* bStpClkDirDir      */ STPCLKDIR_CLOCKWISE,
      /* bGapLevel          */ 0,
      /* ucScan             */ ENCGAPDISK0_SCAN_CYCLES,
      /* ucRetry            */ ENCGAPDISK0_RETRY_CYCLES,
      /* ucVerify           */ ENCGAPDISK0_VERIFY_CYCLES,
		/* ucTskPriMsg        */ OS_P_ABOVENORMAL,
      /* SGate {Mask, Port} */ {AT91C_PB23_TIOA0, AT91C_BASE_PIOB},
      /* u16Timeout         */ _OS_T_MS(100, OS_TICK_RATE)
   }
#if ENCGAPDISK_UNIT_MAX > 1
   ,
   {
      /* ucStpClkDirUnit    */ 1,
      /* bStpClkDirDir      */ STPCLKDIR_CLOCKWISE,
      /* bGapLevel          */ 0,
      /* ucScan             */ ENCGAPDISK1_SCAN_CYCLES,
      /* ucRetry            */ ENCGAPDISK1_RETRY_CYCLES,
      /* ucVerify           */ ENCGAPDISK1_VERIFY_CYCLES,
		/* ucTskPriMsg        */ OS_P_ABOVENORMAL,
      /* SGate {Mask, Port} */ {AT91C_PB25_TIOA1, AT91C_BASE_PIOB},
      /* u16Timeout         */ _OS_T_MS(100, OS_TICK_RATE)
   }
#endif
#if ENCGAPDISK_UNIT_MAX > 2
   ,
   {
      /* ucStpClkDirUnit    */ 2,
      /* bStpClkDirDir      */ STPCLKDIR_CLOCKWISE,
      /* bGapLevel          */ 0,
      /* ucScan             */ ENCGAPDISK2_SCAN_CYCLES,
      /* ucRetry            */ ENCGAPDISK2_RETRY_CYCLES,
      /* ucVerify           */ ENCGAPDISK2_VERIFY_CYCLES,
		/* ucTskPriMsg        */ OS_P_ABOVENORMAL,
      /* SGate {Mask, Port} */ {AT91C_PB27_TIOA2, AT91C_BASE_PIOB},
      /* u16Timeout         */ _OS_T_MS(100, OS_TICK_RATE)
   }
#endif
};

#endif
