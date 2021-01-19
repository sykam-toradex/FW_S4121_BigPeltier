//!	hardware descriptor(s) of AT91SAM7X PWM driver
/**
		\file		pwmdev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PWM/PWMDEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-11-15 initial version
// 2010-07-01 added channel support (PWM_CHANNEL_MAX)
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-10-20 added public doxygen documentation
// 2011-11-17 added digraphs of data for public doxygen documentation
// 2013-08-22 changed delays from milliseconds to ticks (_OS_T_MS for compatibility)
// 2013-11-06 updated public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PWM_PWMDEV_H
#define SYKAM_CPU_AT91_PWM_PWMDEV_H 1


#if PWM_CHANNEL_MAX > 0
// Declare arrays of circular buffers (size definitions are located inside pwm.h)

//!   pwm send data circular buffer
/**
   data flow thru ulPwmChannelTx
   \dot
   digraph ulPwmChannelTx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      PwmChannelTx [label="PwmTx" URL="\ref PwmTx"];
      PwmChannelTxValue [label="PwmTxValue" URL="\ref PwmTxValue"];
      ulPwmChannelTx [label="ulPwmChannelTx" style="filled"];
      SemTx [label="SPwmChannelRW.SemTx" URL="\ref _SPwmChannelRW"];
      TskPwmChannelTx [label="TskPwmChannelTx" shape="ellipse"];
      PwmChannel [label="PwmChannel" shape="parallelogram"];
      PwmChannelIrqHandler [label="PwmChannelIrqHandler"];
      SPwmList [label=" SPwmList"]
      User [label="User pwm channel handler(s)" style="dashed"];

      PwmChannelTx -> ulPwmChannelTx;
      PwmChannelTx -> SemTx;
      
      PwmChannelTxValue -> ulPwmChannelTx;
      PwmChannelTxValue -> SemTx;
      
      SemTx -> TskPwmChannelTx [arrowhead="odot"];
      ulPwmChannelTx -> TskPwmChannelTx;
      
      TskPwmChannelTx -> PwmChannel;
      PwmChannel -> PwmChannelIrqHandler;
      PwmChannelIrqHandler -> SemTx;
      PwmChannelIrqHandler -> SPwmList;
      SPwmList -> User;
   }
   \enddot
*/
circular_declare_ex(ULONG, USHORT, ulPwmChannelTx, PWM_CHANNEL_BUFFERCNTTX, [PWM_UNIT_MAX][PWM_CHANNEL_MAX]);
#endif


//! Hardware descriptors for unit(s)
struct _SPwmRO SPwmRO[PWM_UNIT_MAX] =
{
   {
      /* pBase          */ AT91C_BASE_PWMC,
      /* u32PeriphId    */ AT91C_ID_PWMC,
      /* pSPioList      */ NULL,
      /* SPioListSize   */ 0,
      /* Isr            */ IsrPwm0
   }
#if PWM_UNIT_MAX > 1
   ,
   {
      /* pBase          */ AT91C_BASE_PWMC,
      /* u32PeriphId    */ AT91C_ID_PWMC,
      /* pSPioList      */ NULL,
      /* SPioListSize   */ 0,
      /* Isr            */ IsrPwm1
   }
#endif
};

#if PWM_CHANNEL_MAX > 0
//! Hardware descriptors for channel(s)
struct _SPwmChannelRO SPwmChannelRO[PWM_UNIT_MAX][PWM_CHANNEL_MAX] = 
{
   {
      {
         /* ucChannelId    */ AT91C_PWMC_CHID0,
         /* u8TskPriTx     */ OS_P_HIGH,
         /* u16MaxTx       */ PWM_CHANNEL_MAXTX,
         /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE)
      }
#if PWM_CHANNEL_MAX > 1
      ,
      {
         /* ucChannelId    */ AT91C_PWMC_CHID0,
         /* u8TskPriTx     */ OS_P_HIGH,
         /* u16MaxTx       */ PWM_CHANNEL_MAXTX,
         /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE)
      }
#endif
#if PWM_CHANNEL_MAX > 2
      ,
      {
         /* ucChannelId    */ AT91C_PWMC_CHID0,
         /* u8TskPriTx     */ OS_P_HIGH,
         /* u16MaxTx       */ PWM_CHANNEL_MAXTX,
         /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE)
      }
#endif
#if PWM_CHANNEL_MAX > 3
      ,
      {
         /* ucChannelId    */ AT91C_PWMC_CHID0,
         /* u8TskPriTx     */ OS_P_HIGH,
         /* u16MaxTx       */ PWM_CHANNEL_MAXTX,
         /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE)
      }
#endif
   }
#if PWM_UNIT_MAX > 1
   ,
   {
      {
         /* ucChannelId    */ AT91C_PWMC_CHID0,
         /* u8TskPriTx     */ OS_P_HIGH,
         /* u16MaxTx       */ PWM_CHANNEL_MAXTX,
         /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE)
      }
#if PWM_CHANNEL_MAX > 1
      ,
      {
         /* ucChannelId    */ AT91C_PWMC_CHID0,
         /* u8TskPriTx     */ OS_P_HIGH,
         /* u16MaxTx       */ PWM_CHANNEL_MAXTX,
         /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE)
      }
#if PWM_CHANNEL_MAX > 2
      ,
      {
         /* ucChannelId    */ AT91C_PWMC_CHID0,
         /* u8TskPriTx     */ OS_P_HIGH,
         /* u16MaxTx       */ PWM_CHANNEL_MAXTX,
         /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE)
      }
#endif
#if PWM_CHANNEL_MAX > 3
      ,
      {
         /* ucChannelId    */ AT91C_PWMC_CHID0,
         /* u8TskPriTx     */ OS_P_HIGH,
         /* u16MaxTx       */ PWM_CHANNEL_MAXTX,
         /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE)
      }
#endif
   }
#endif
#endif
};
#endif

// Number of channels located inside pwm.h
struct _SPwmRO SPwmRO[PWM_UNIT_MAX];

#if PWM_CHANNEL_MAX > 0
struct _SPwmChannelRW SPwmChannelRW[PWM_UNIT_MAX][PWM_CHANNEL_MAX];
struct _SPwmChannelTsk SPwmChannelTsk[PWM_UNIT_MAX][PWM_CHANNEL_MAX];
#endif

#endif
