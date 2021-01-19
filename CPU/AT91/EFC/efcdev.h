//!	hardware descriptor(s) of AT91SAM7X EFC driver
/**
		\file		efcdev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/EFC/EFCDEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-02-28 initial version
// 2013-03-21 changed constants to defines for usBlockSize and usPageSize
// 2013-04-04 fixed define condition name for descriptor of unit 1
// 2013-08-22 changed timeout from milliseconds to ticks (_OS_T_MS for compatibility)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_EFC_EFCDEV_H
#define SYKAM_CPU_AT91_EFC_EFCDEV_H 1


// Declare arrays of mailboxes (size definitions are located inside efc.h)


// Customized stacks
#if EFC_STACK_TSKMSG > 0
   OS_TSK_STACK_USER_EX(StkTskEfc, EFC_STACK_TSKMSG, [EFC_UNIT_MAX]);
#endif

//!   notification message queue
/**
   data flow thru MbxEfcMsg
   \dot
   digraph MbxEfcMsg {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Efc [label="Efc peripheral" shape="parallelogram"];
      EfcIrqHandler [label="EfcIrqHandler"];
      MbxEfcMsg [label="MbxEfcMsg" style="filled"];
      TskEfcMsg [label="TskEfcMsg" shape="ellipse"];
      xMsgCounter [label="SEfcRW.xMsgCounter" URL="\ref _SEfcRW"];
      EfcMsg [label="EfcMsg" shape="invtrapezium" URL="\ref EfcMsg"];

      Efc -> EfcIrqHandler [arrowhead="odot"];
      EfcIrqHandler -> MbxEfcMsg;
      MbxEfcMsg -> TskEfcMsg [arrowhead="odot"];
      TskEfcMsg -> xMsgCounter;
      xMsgCounter -> EfcMsg;
   }
   \enddot
*/
os_mbx_declare_ex(MbxEfcMsg, EFC_MBXCNTMSG, [EFC_UNIT_MAX]);


//! Hardware descriptors
struct _SEfcRO SEfcRO[EFC_UNIT_MAX] =
{
   {
      /* pEfcBase       */ AT91C_BASE_EFC0,
      /* usBlockSize    */ AT91C_IFLASH_LOCK_REGION_SIZE,
      /* usPageSize     */ AT91C_IFLASH_PAGE_SIZE,
      /* ucBlockCount   */ AT91C_IFLASH_UNIT_NB_OF_LOCK_BITS,
      /* ucPageCount    */ AT91C_IFLASH_LOCK_REGION_SIZE / AT91C_IFLASH_PAGE_SIZE,   /* pages per block */
      /* u16FWS         */ AT91C_MC_FWS_1FWS,   /* 2 cycles for read, 3 for write operations */
      /* u8FMCNGPNVM    */ 50,   /* (MCK/ 1000000)* 1,05) or (MCK/ 1000000)+ 2 */
      /* u8FMCNDATA     */ 72,   /* (MCK/ 1000000)* 1,50) */
      /* u16GPNVMSGPB   */ AT91C_MC_GPNVM0 /* Brown-out mgr enable */ | AT91C_MC_GPNVM1 /* Brown-out mgr reset enable */ /* refer to AT91SAM7X manual 8.5.4 - 8.7. page 21-23 */,
      /* u16GPNVMCGPB   */ 0 /* AT91C_MC_GPNVM0 | AT91C_MC_GPNVM1 */,
      /* u32LOCKSLB     */ NULL,
      /* u32LOCKCLB     */ NULL,
      /* u8TskPriMsg    */ OS_P_REALTIME,
      /* u16Timeout     */ _OS_T_MS(100, OS_TICK_RATE)
   }
#if EFC_UNIT_MAX > 1
   ,
   {
      /* pEfcBase       */ AT91C_BASE_EFC1,
      /* usBlockSize    */ AT91C_IFLASH_LOCK_REGION_SIZE,
      /* usPageSize     */ AT91C_IFLASH_PAGE_SIZE,
      /* ucBlockCount   */ AT91C_IFLASH_UNIT_NB_OF_LOCK_BITS,
      /* ucPageCount    */ AT91C_IFLASH_LOCK_REGION_SIZE / AT91C_IFLASH_PAGE_SIZE,   /* pages per block */
      /* u16WS          */ AT91C_MC_FWS_1FWS,   /* 2 cycles for read, 3 for write operations */
      /* u8FMCNGPNVM    */ 50,   /* (MCK/ 1000000)* 1,05) or (MCK/ 1000000)+ 2 */
      /* u8FMCNDATA     */ 72,   /* (MCK/ 1000000)* 1,50) */
      /* u16GPNVMSGPB   */ 0,    /* only supported on unit 0 */
      /* u16GPNVMCGPB   */ 0,    /* only supported on unit 0 */
      /* u32LOCKSLB     */ NULL,
      /* u32LOCKCLB     */ NULL,
      /* u8TskPriMsg    */ OS_P_REALTIME,
      /* u16Timeout     */ _OS_T_MS(100, OS_TICK_RATE)
   }
#endif
};

#endif
