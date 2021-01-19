//!	hardware descriptor(s) of AT91SAM7X USART driver
/**
		\file		usartdev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/USART/USARTDEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-09-25 initial version
// 2009-10-08 changed for new os_mbx_declare_ex with x-dim array support
// 2010-05-21 reduced u16DelayRxMS to 0 (= 10ms minimum reschule delay)
// 2010-05-27 changed u8TskPriMsg/Rx/Tx
// 2010-06-18 changed register data type of ucUsartRx/Tx to USHORT
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-11-04 fixed comments of hardware definitions
// 2011-06-01 renamed USART_UNITMAX to USART_UNIT_MAX
// 2011-10-20 added public doxygen documentation
// 2011-11-17 added digraphs of data for public doxygen documentation
// 2013-08-21 changed delays from milliseconds to ticks (_OS_T_MS for compatibility)
// 2016-03-03 added double bank pdc/dma support for latency free bank switching
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_USART_USARTDEV_H
#define SYKAM_CPU_AT91_USART_USARTDEV_H 1


// Declare arrays of circular buffers (size definitions are located inside usart.h)

//!   receive data circular buffer
/**
   data flow thru ucUsartRx
   \dot
   digraph ucUsartRx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Usart [label="Usart PDC (DMA)" shape="parallelogram"];
      UsartIrqHandler [label="UsartIrqHandler"];
      SCircular [label="SUsartRW.SCircular" URL="\ref _SCircular"];
      BufferRx [label="SUsartRW.ucPDCBufferRx" URL="\ref _SUsartRW"];
      MbxUsartRx [label="MbxUsartRx"];
      TskUsartRx [label="TskUsartRx" shape="ellipse"];
      ucUsartRx [label="ucUsartRx" style="filled"];
      UsartRx [label="UsartRx" shape="invtrapezium" URL="\ref UsartRx"];

      Usart -> BufferRx;
      Usart -> UsartIrqHandler [arrowhead="odot"];
      UsartIrqHandler -> MbxUsartRx;
      UsartIrqHandler -> SCircular;
      
      MbxUsartRx -> TskUsartRx [arrowhead="odot"];
      SCircular -> TskUsartRx;
      BufferRx -> TskUsartRx;
      
      TskUsartRx -> ucUsartRx;
      ucUsartRx -> UsartRx;
   }
   \enddot
*/
circular_declare_ex(UCHAR, USHORT, ucUsartRx, USART_BUFFERCNTRX, [USART_UNIT_MAX]);

//!   send data circular buffer
/**
   data flow thru ucUsartTx
   \dot
   digraph ucUsartTx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      UsartTx [label="UsartTx" shape="trapezium" URL="\ref UsartTx"];
      Usart [label="Usart PDC (DMA)" shape="parallelogram"];
      UsartIrqHandler [label="UsartIrqHandler"];
      SemTickTx [label="SUsartRW.SemTickTx" URL="\ref _SUsartRW"];
      BufferTx [label="SUsartRW.ucPDCBufferTx" URL="\ref _SUsartRW"];
      TskUsartTx [label="TskUsartTx" shape="ellipse"];
      ucUsartTx [label="ucUsartTx" style="filled"];

      UsartTx -> ucUsartTx;
      UsartTx -> SemTickTx;
      
      ucUsartTx -> TskUsartTx;
      SemTickTx -> TskUsartTx [arrowhead="odot"];
      
      TskUsartTx -> UsartIrqHandler;
      UsartIrqHandler -> SemTickTx;
      UsartIrqHandler -> BufferTx;
      BufferTx -> Usart;
      UsartIrqHandler -> Usart [dir="both"];
   }
   \enddot
*/
circular_declare_ex(UCHAR, USHORT, ucUsartTx, USART_BUFFERCNTTX, [USART_UNIT_MAX]);


// Declare arrays of mailboxes (size definitions are located inside usart.h)

//!   receive data message queue
/**
   data flow thru MbxUsartRx
   \dot
   digraph MbxUsartRx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Usart [label="Usart PDC (DMA)" shape="parallelogram"];
      UsartIrqHandler [label="UsartIrqHandler"];
      SCircular [label="SUsartRW.SCircular" URL="\ref _SCircular"];
      BufferRx [label="SUsartRW.ucPDCBufferRx" URL="\ref _SUsartRW"];
      MbxUsartRx [label="MbxUsartRx" style="filled"];
      TskUsartRx [label="TskUsartRx" shape="ellipse"];
      ucUsartRx [label="ucUsartRx"];
      UsartRx [label="UsartRx" shape="invtrapezium" URL="\ref UsartRx"];

      Usart -> BufferRx;
      Usart -> UsartIrqHandler [arrowhead="odot"];
      UsartIrqHandler -> MbxUsartRx;
      UsartIrqHandler -> SCircular;
      
      MbxUsartRx -> TskUsartRx [arrowhead="odot"];
      SCircular -> TskUsartRx;
      BufferRx -> TskUsartRx;
      
      TskUsartRx -> ucUsartRx;
      ucUsartRx -> UsartRx;
   }
   \enddot
*/
os_mbx_declare_ex(MbxUsartRx, USART_MBXCNTRX, [USART_UNIT_MAX]);

//!   notification message queue
/**
   data flow thru MbxUsartMsg
   \dot
   digraph MbxUsartMsg {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Usart [label="Usart peripheral" shape="parallelogram"];
      UsartIrqHandler [label="UsartIrqHandler"];
      MbxUsartMsg [label="MbxUsartMsg" style="filled"];
      TskUsartMsg [label="TskUsartMsg" shape="ellipse"];
      xMsgCounter [label="SUsartRW.xMsgCounter" URL="\ref _SUsartRW"];
      UsartMsg [label="UsartMsg" shape="invtrapezium" URL="\ref UsartMsg"];

      Usart -> UsartIrqHandler [arrowhead="odot"];
      UsartIrqHandler -> MbxUsartMsg;
      MbxUsartMsg -> TskUsartMsg [arrowhead="odot"];
      TskUsartMsg -> xMsgCounter;
      xMsgCounter -> UsartMsg;
   }
   \enddot
*/
os_mbx_declare_ex(MbxUsartMsg, USART_MBXCNTMSG, [USART_UNIT_MAX]);


//! Hardware descriptors
struct _SUsartRO SUsartRO[USART_UNIT_MAX] =
{
   {
      /* pBase          */ AT91C_BASE_US0,
      /* u32PeriphId    */ AT91C_ID_US0,
      /* u32IrqRx       */ AT91C_US_RXBUFF | AT91C_US_ENDRX,
      /* u32IrqTx       */ AT91C_US_TXBUFE,
      /* u32IrqMsg      */ AT91C_US_RXBUFF | AT91C_US_ENDRX | AT91C_US_TXBUFE | AT91C_US_TIMEOUT | AT91C_US_OVRE | AT91C_US_FRAME | AT91C_US_PARE,
      /* u8TskPriMsg    */ OS_P_REALTIME,
      /* u8TskPriRx     */ OS_P_ABOVEHIGH,
      /* u8TskPriTx     */ OS_P_HIGH,
      /* u16DelayRx     */ _OS_T_MS(0, OS_TICK_RATE),
      /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE),
      /* u16PDCMaxRx    */ USART_PDCMAXRX,
      /* u16PDCMaxTx    */ USART_PDCMAXTX,
      /* Isr            */ IsrUsart0
   }
#if USART_UNIT_MAX > 1
   ,
   {
      /* pBase          */ AT91C_BASE_US1,
      /* u32PeriphId    */ AT91C_ID_US1,
      /* u32IrqRx       */ AT91C_US_RXBUFF | AT91C_US_ENDRX,
      /* u32IrqTx       */ AT91C_US_TXBUFE,
      /* u32IrqMsg      */ AT91C_US_RXBUFF | AT91C_US_ENDRX | AT91C_US_TXBUFE | AT91C_US_TIMEOUT | AT91C_US_OVRE | AT91C_US_FRAME | AT91C_US_PARE,
      /* u8TskPriMsg    */ OS_P_REALTIME,
      /* u8TskPriRx     */ OS_P_ABOVEHIGH,
      /* u8TskPriTx     */ OS_P_HIGH,
      /* u16DelayRx     */ _OS_T_MS(0, OS_TICK_RATE),
      /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE),
      /* u16PDCMaxRx    */ USART_PDCMAXRX,
      /* u16PDCMaxTx    */ USART_PDCMAXTX,
      /* Isr            */ IsrUsart1
   }
#endif
};

#endif
