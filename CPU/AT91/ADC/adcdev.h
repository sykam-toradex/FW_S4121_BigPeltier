//!	hardware descriptor(s) of AT91SAM7X ADC peripheral driver
/**
		\file		adcdev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/ADC/ADCDEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-05-19 initial version
// 2011-05-25 added u8MR
// 2011-10-19 added public doxygen documentation
// 2011-11-17 added digraphs of data for public doxygen documentation
// 2013-08-07 moved AT91C_ADC_EOCX/OVREX defines to adc.h
// 2013-08-22 changed cycle from milliseconds to ticks (_OS_T_MS for compatibility)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_ADC_ADCDEV_H
#define SYKAM_CPU_AT91_ADC_ADCDEV_H 1


// Declare arrays of mailboxes (size definitions are located inside adc.h)

//!   receive data queue
/**
   data flow thru MbxAdcRx
   \dot
   digraph MbxAdcRx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Adc [label="Adc PDC (DMA)" shape="parallelogram"];
      AdcIrqHandler [label="AdcIrqHandler"];
      MbxAdcRx [label="MbxAdcRx" style="filled"];
      SCircular [label="SAdcRW.SCircular" URL="\ref _SCircular"];
      BufferRx [label="SAdcRW.xPDCBufferRx" URL="\ref _SAdcRW"];
      TskAdcRx [label="TskAdcRx" shape="ellipse"];
      Modify [label="SAdcCustom.Modify" URL="\ref _SAdcCustom"];
      Average [label="SAdcCustom.Average" URL="\ref _SAdcCustom"];
      Storage [label="SAdcChannelRW.SStorage" URL="\ref _SStorage"];
      AdcRx [label="AdcRx" shape="invtrapezium" URL="\ref AdcRx"];
      
      Adc -> AdcIrqHandler [arrowhead="odot"];
      Adc -> BufferRx;
      
      AdcIrqHandler -> MbxAdcRx;
      AdcIrqHandler -> SCircular;
      MbxAdcRx -> TskAdcRx [arrowhead="odot"];
      SCircular -> TskAdcRx;
      BufferRx -> TskAdcRx];
      TskAdcRx -> Modify;
      Modify -> Average;
      Average -> Storage;
      Storage -> AdcRx;
   }
   \enddot
*/
os_mbx_declare_ex(MbxAdcRx, ADC_MBXCNTRX, [ADC_UNIT_MAX]);

//!   unit notification message queue
/**
   data flow thru MbxAdcMsg
   \dot
   digraph MbxAdcMsg {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Adc [label="Adc peripheral" shape="parallelogram"];
      AdcIrqHandler [label="AdcIrqHandler"];
      MbxAdcMsg [label="MbxAdcMsg" style="filled"];
      TskAdcMsg [label="TskAdcMsg" shape="ellipse"];
      xMsgCounter [label="SAdcRW.xMsgCounter" URL="\ref _SAdcRW"];
      AdcMsgUnit [label="AdcMsgUnit" shape="invtrapezium" URL="\ref AdcMsgUnit"];

      Adc -> AdcIrqHandler [arrowhead="odot"];
      AdcIrqHandler -> MbxAdcMsg;
      MbxAdcMsg -> TskAdcMsg [arrowhead="odot"];
      TskAdcMsg-> xMsgCounter;
      xMsgCounter-> AdcMsgUnit;
   }
   \enddot
*/
os_mbx_declare_ex(MbxAdcMsg, ADC_MBXCNTMSG, [ADC_UNIT_MAX]);

//!   channel notification message queue
/**
   data flow thru MbxAdcMsgChannel
   \dot
   digraph MbxAdcMsgChannel {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Adc [label="Adc peripheral" shape="parallelogram"];
      AdcIrqHandler [label="AdcIrqHandler"];
      MbxAdcMsgChannel [label="MbxAdcMsgChannel" style="filled"];
      TskAdcMsgChannel [label="TskAdcMsgChannel" shape="ellipse"];
      xMsgCounter [label="SAdcChannelRW.xMsgCounter" URL="\ref _SAdcChannelRW"];
      AdcMsgChannel [label="AdcMsgChannel" shape="invtrapezium" URL="\ref AdcMsgChannel"];

      Adc -> AdcIrqHandler [arrowhead="odot"];
      AdcIrqHandler -> MbxAdcMsgChannel;
      MbxAdcMsgChannel ->TskAdcMsgChannel [arrowhead="odot"];
      TskAdcMsgChannel -> xMsgCounter;
      xMsgCounter -> AdcMsgChannel;
   }
   \enddot
*/
os_mbx_declare_ex(MbxAdcMsgChannel, ADC_MBXCNTMSG, [ADC_UNIT_MAX]);

// Defines


//! Hardware descriptors
struct _SAdcRO SAdcRO[ADC_UNIT_MAX] =
{
   {
      /* pBase           */ AT91C_BASE_ADC,
      /* u32PeriphId     */ AT91C_ID_ADC,
      /* u32IrqRx        */ AT91C_ADC_RXBUFF,
      /* u32IrqMsg       */ AT91C_ADC_RXBUFF | AT91C_ADC_GOVRE | AT91C_ADC_OVREX | AT91C_ADC_EOCX,
      /* u8TskPriMsg     */ OS_P_REALTIME,
      /* u8TskPriRx      */ OS_P_ABOVEHIGH,
      /* u8MR            */ 0,
      /* u32ClockHz      */ 5000000,     
      /* u16StartupUS    */ 10,
      /* u16SampleHoldNS */ 1200,
      /* u16CycleRx      */ _OS_T_MS(50, OS_TICK_RATE),
      /* Isr             */ IsrAdc0
   }
#if ADC_UNIT_MAX > 1
   ,
   {
      /* pBase           */ AT91C_BASE_ADC1,
      /* u32PeriphId     */ AT91C_ID_ADC,
      /* u32IrqRx        */ AT91C_ADC_RXBUFF,
      /* u32IrqMsg       */ AT91C_ADC_RXBUFF | AT91C_ADC_GOVRE | AT91C_ADC_OVREX | AT91C_ADC_EOCX,
      /* u8TskPriMsg     */ OS_P_REALTIME,
      /* u8TskPriRx      */ OS_P_ABOVEHIGH,
      /* u8MR            */ 0,
      /* u32ClockHz      */ 5000000,
      /* u16StartupUS    */ 10,
      /* u16SampleHoldNS */ 1200,
      /* u16CycleRx      */ _OS_T_MS(50, OS_TICK_RATE),
      /* Isr             */ IsrAdc1
   }
#endif
};

// Number of units and channels located inside adc.h
struct _SAdcChannelRW SAdcChannelRW[ADC_UNIT_MAX][ADC_CHANNEL_MAX];
struct _SAdcRW SAdcRW[ADC_UNIT_MAX];

#endif
