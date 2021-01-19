//!	hardware descriptor(s) of AT91SAM7X SPI driver
/**
		\file		spidev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/SPI/SPIDEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-10-07 initial version
// 2009-10-08 changed for new os_mbx_declare_ex with x-dim array support
// 2009-11-11 changed define SPI_UNITMAX to SPI_UNIT_MAX
// 2009-12-21 added circular buffers ucSpiChannelTagRx and ucSpiChannelTagTx
// 2009-12-22 changed u8TskPriMsg from OS_P_BELOWNORMAL to OS_P_HIGH
// 2009-12-23 added customized user stacks for StkTskSpiRx and StkTskSpiTx
// 2010-05-26 reduced u16DelayRxMS to 0 (= 10ms min reschule delay)
// 2010-05-27 changed u8TskPriMsg/Rx/Tx
// 2010-06-04 added AT91C_SPI_NSSR irq source to 2nd unit for slave support
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-01 added intermediate circular buffer usSpiChannelCntRxQ
// 2010-09-09 changed u16AgeStepTxMS to 10ms (current minimum os delay)
// 2010-09-22 renamed u16AgeStepTxMS to u16AgeStepTx (ticks)
// 2010-12-09 raised task priority u8TskPriTx to OS_P_ABOVEHIGH (same as Rx)
// 2011-04-26 added circular buffer ucSpiAddrRx
// 2011-10-26 added public doxygen documentation
// 2011-11-17 added digraphs of data for public doxygen documentation
// 2011-11-23 added digraphs of data for public doxygen documentation
// 2013-08-21 changed delays from milliseconds to ticks (_OS_T_MS for compatibility)
// 2016-03-03 added double bank pdc/dma support for latency free bank switching
//----------------------------------------------------------------------------
// CAUTION: Priority levels needs to be u8TskPriMsg > u8TskPriRx > u8TskPriTx
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_SPI_SPIDEV_H
#define SYKAM_CPU_AT91_SPI_SPIDEV_H 1


// Declare arrays of circular buffers (size definitions are located inside spi.h)

//!   send data circular buffer (pdc prepared data items that are copied later to pdc block buffer for transmission)
/**
   data flow thru ulSpiTx
   \dot
   digraph ulSpiTx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      SpiTx [label="SpiTxTag / SpiTx" shape="trapezium" URL="\ref SpiTxTag"];
      SemTickTx [label="SSpiRW.SemTickTx" URL="\ref _SSpiRW"];
      ucSpiChannelTx [label="ucSpiChannelTx"];
      usSpiChannelCntTx [label="usSpiChannelCntTx"];
      ucSpiChannelTagTx [label="ucSpiChannelTagTx"];
      BufferTx [label="SSpiRW.ulPDCBufferTx" URL="\ref _SSpiRW"];
      TskSpiTx [label="TskSpiTx" shape="ellipse"];
      ulSpiTx [label="ulSpiTx" style="filled"];
      ulSpiCntTx [label="ulSpiCntTx"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx"];
      ucSpiAddrRx [label="ucSpiAddrRx"];
      SpiIrqHandler [label="SpiIrqHandler"];
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      
      SpiTx -> SemTickTx;
      SpiTx -> ucSpiChannelTx;
      SpiTx -> usSpiChannelCntTx;
      SpiTx -> ucSpiChannelTagTx;
          
      SemTickTx -> TskSpiTx [arrowhead="odot"];
      ucSpiChannelTx -> TskSpiTx;
      usSpiChannelCntTx -> TskSpiTx;
      ucSpiChannelTagTx -> TskSpiTx;
          
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ulSpiCntTx;
      TskSpiTx -> ucSpiAddrRx;
      TskSpiTx -> ulSpiTx;
      
      ulSpiTx -> SpiIrqHandler;
      ulSpiCntTx -> SpiIrqHandler;
      SpiIrqHandler -> BufferTx;
      BufferTx -> Spi;
      SpiIrqHandler -> Spi;
      Spi -> SpiIrqHandler [arrowhead="odot"];
      SpiIrqHandler -> SemTickTx;
   }
   \enddot
*/
circular_declare_ex(ULONG, USHORT, ulSpiTx, SPI_BUFFERCNTTX, [SPI_UNIT_MAX]);

//!   send data packet size circular buffer for auto job segmentation of pdc prepared data items of ulSpiTx
/**
   data flow thru ulSpiCntTx
   \dot
   digraph ulSpiCntTx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      SpiTx [label="SpiTxTag / SpiTx" shape="trapezium" URL="\ref SpiTxTag"];
      SemTickTx [label="SSpiRW.SemTickTx" URL="\ref _SSpiRW"];
      ucSpiChannelTx [label="ucSpiChannelTx"];
      usSpiChannelCntTx [label="usSpiChannelCntTx"];
      ucSpiChannelTagTx [label="ucSpiChannelTagTx"];
      BufferTx [label="SSpiRW.ulPDCBufferTx" URL="\ref _SSpiRW"];
      TskSpiTx [label="TskSpiTx" shape="ellipse"];
      ulSpiTx [label="ulSpiTx"];
      ulSpiCntTx [label="ulSpiCntTx" style="filled"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx"];
      ucSpiAddrRx [label="ucSpiAddrRx"];
      SpiIrqHandler [label="SpiIrqHandler"];
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      
      SpiTx -> SemTickTx;
      SpiTx -> ucSpiChannelTx;
      SpiTx -> usSpiChannelCntTx;
      SpiTx -> ucSpiChannelTagTx;
          
      SemTickTx -> TskSpiTx [arrowhead="odot"];
      ucSpiChannelTx -> TskSpiTx;
      usSpiChannelCntTx -> TskSpiTx;
      ucSpiChannelTagTx -> TskSpiTx;
          
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ulSpiCntTx;
      TskSpiTx -> ucSpiAddrRx;
      TskSpiTx -> ulSpiTx;
      
      ulSpiTx -> SpiIrqHandler;
      ulSpiCntTx -> SpiIrqHandler;
      SpiIrqHandler -> BufferTx;
      BufferTx -> Spi;
      SpiIrqHandler -> Spi;
      Spi -> SpiIrqHandler [arrowhead="odot"];
      SpiIrqHandler -> SemTickTx;
   }
   \enddot
*/
circular_declare_ex(ULONG, USHORT, ulSpiCntTx, SPI_BUFFERCNTTX, [SPI_UNIT_MAX]);

#if SPI_HWFIX_PDCRX > 0
   //!   receive data address circular buffer (stores true hardware slave channel for each data item because hardware determined addresses are spuriously wrong)
   /**
      data flow thru ucSpiAddrRx
      \dot
      digraph ucSpiAddrRx {
         node [shape="record" fontname="FreeSans" fontsize="10"];
         
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      SpiIrqHandler [label="SpiIrqHandler"];
      MbxSpiRx [label="MbxSpiRx"];
      ucSpiAddrRx [label="ucSpiAddrRx" style="filled"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx"];
      ucSpiChannelRx [label="ucSpiChannelRx"];
      usSpiChannelCntRx [label="usSpiChannelCntRx"];
      SCircular [label="SSpiRW.SCircular" URL="\ref _SCircular"];
      BufferRx [label="SSpiRW.ulPDCBufferRx" URL="\ref _SSpiRW"];
      TskSpiRx [label="TskSpiRx" shape="ellipse"];
      TskSpiTx [label="TskSpiTx" style="dashed" shape="ellipse"];
      SpiRx [label="SpiRxTag / SpiRx" shape="invtrapezium" URL="\ref SpiRxTag"];
      
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ucSpiAddrRx;
      
      Spi -> SpiIrqHandler [arrowhead="odot"];
      Spi -> BufferRx;
      
      SpiIrqHandler -> Spi;
      SpiIrqHandler -> MbxSpiRx;
      SpiIrqHandler -> SCircular;
      
      MbxSpiRx -> TskSpiRx [arrowhead="odot"];
      SCircular -> TskSpiRx;
      BufferRx -> TskSpiRx;
      ucSpiAddrRx -> TskSpiRx;
      usSpiChannelCntRxQ -> TskSpiRx;
      
      TskSpiRx -> ucSpiChannelRx;
      TskSpiRx -> usSpiChannelCntRx;
      
      usSpiChannelCntRx -> SpiRx;
      ucSpiChannelTagRx -> SpiRx;
      ucSpiChannelRx -> SpiRx;
   }
   \enddot
*/
   circular_declare_ex(UCHAR, USHORT, ucSpiAddrRx, SPI_BUFFERCNTADDRRX, [SPI_UNIT_MAX]);
#endif

//!   send data circular buffer (application character packets)
/**
   data flow thru ucSpiChannelRx
   \dot
   digraph ucSpiChannelRx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      SpiIrqHandler [label="SpiIrqHandler"];
      MbxSpiRx [label="MbxSpiRx"];
      ucSpiAddrRx [label="ucSpiAddrRx"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx"];
      ucSpiChannelRx [label="ucSpiChannelRx" style="filled"];
      usSpiChannelCntRx [label="usSpiChannelCntRx"];
      SCircular [label="SSpiRW.SCircular" URL="\ref _SCircular"];
      BufferRx [label="SSpiRW.ulPDCBufferRx" URL="\ref _SSpiRW"];
      TskSpiRx [label="TskSpiRx" shape="ellipse"];
      TskSpiTx [label="TskSpiTx" style="dashed"];
      SpiRx [label="SpiRxTag / SpiRx" shape="invtrapezium" URL="\ref SpiRxTag"];
      
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ucSpiAddrRx;
      
      Spi -> SpiIrqHandler [arrowhead="odot"];
      Spi -> BufferRx;
      
      SpiIrqHandler -> Spi;
      SpiIrqHandler -> MbxSpiRx;
      SpiIrqHandler -> SCircular;
      
      MbxSpiRx -> TskSpiRx [arrowhead="odot"];
      SCircular -> TskSpiRx;
      BufferRx -> TskSpiRx;
      ucSpiAddrRx -> TskSpiRx;
      usSpiChannelCntRxQ -> TskSpiRx;
      
      TskSpiRx -> ucSpiChannelRx;
      TskSpiRx -> usSpiChannelCntRx;
      
      usSpiChannelCntRx -> SpiRx;
      ucSpiChannelTagRx -> SpiRx;
      ucSpiChannelRx -> SpiRx;
   }
   \enddot
*/
circular_declare_ex(UCHAR, USHORT, ucSpiChannelRx, SPI_BUFFERCNTRX, [SPI_UNIT_MAX][SPI_CHANNEL_MAX]);

//!   receive data circular buffer (application character packets)
/**
   data flow thru ucSpiChannelTx
   \dot
   digraph ucSpiChannelTx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      SpiTx [label="SpiTxTag / SpiTx" shape="trapezium" URL="\ref SpiTxTag"];
      SemTickTx [label="SSpiRW.SemTickTx" URL="\ref _SSpiRW"];
      ucSpiChannelTx [label="ucSpiChannelTx" style="filled"];
      usSpiChannelCntTx [label="usSpiChannelCntTx"];
      ucSpiChannelTagTx [label="ucSpiChannelTagTx"];
      BufferTx [label="SSpiRW.ulPDCBufferTx" URL="\ref _SSpiRW"];
      TskSpiTx [label="TskSpiTx" shape="ellipse"];
      ulSpiTx [label="ulSpiTx"];
      ulSpiCntTx [label="ulSpiCntTx"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx"];
      ucSpiAddrRx [label="ucSpiAddrRx"];
      SpiIrqHandler [label="SpiIrqHandler"];
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      
      SpiTx -> SemTickTx;
      SpiTx -> ucSpiChannelTx;
      SpiTx -> usSpiChannelCntTx;
      SpiTx -> ucSpiChannelTagTx;
          
      SemTickTx -> TskSpiTx [arrowhead="odot"];
      ucSpiChannelTx -> TskSpiTx;
      usSpiChannelCntTx -> TskSpiTx;
      ucSpiChannelTagTx -> TskSpiTx;
          
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ulSpiCntTx;
      TskSpiTx -> ucSpiAddrRx;
      TskSpiTx -> ulSpiTx;
      
      ulSpiTx -> SpiIrqHandler;
      ulSpiCntTx -> SpiIrqHandler;
      SpiIrqHandler -> BufferTx;
      BufferTx -> Spi;
      SpiIrqHandler -> Spi;
      Spi -> SpiIrqHandler [arrowhead="odot"];
      SpiIrqHandler -> SemTickTx;
   }
   \enddot
*/
circular_declare_ex(UCHAR, USHORT, ucSpiChannelTx, SPI_BUFFERCNTTX, [SPI_UNIT_MAX][SPI_CHANNEL_MAX]);

//!   receive data packet size (application character count) for auto segmenentation of current job packet retrieval by application
/**
   data flow thru usSpiChannelCntRx
   \dot
   digraph usSpiChannelCntRx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      SpiIrqHandler [label="SpiIrqHandler"];
      MbxSpiRx [label="MbxSpiRx"];
      ucSpiAddrRx [label="ucSpiAddrRx"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx"];
      ucSpiChannelRx [label="ucSpiChannelRx"];
      usSpiChannelCntRx [label="usSpiChannelCntRx" style="filled"];
      SCircular [label="SSpiRW.SCircular" URL="\ref _SCircular"];
      BufferRx [label="SSpiRW.ulPDCBufferRx" URL="\ref _SSpiRW"];
      TskSpiRx [label="TskSpiRx" shape="ellipse"];
      TskSpiTx [label="TskSpiTx" style="dashed"];
      SpiRx [label="SpiRxTag / SpiRx" shape="invtrapezium" URL="\ref SpiRxTag"];
      
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ucSpiAddrRx;
      
      Spi -> SpiIrqHandler [arrowhead="odot"];
      Spi -> BufferRx;
      
      SpiIrqHandler -> Spi;
      SpiIrqHandler -> MbxSpiRx;
      SpiIrqHandler -> SCircular;
      
      MbxSpiRx -> TskSpiRx [arrowhead="odot"];
      SCircular -> TskSpiRx;
      BufferRx -> TskSpiRx;
      ucSpiAddrRx -> TskSpiRx;
      usSpiChannelCntRxQ -> TskSpiRx;
      
      TskSpiRx -> ucSpiChannelRx;
      TskSpiRx -> usSpiChannelCntRx;
      
      usSpiChannelCntRx -> SpiRx;
      ucSpiChannelTagRx -> SpiRx;
      ucSpiChannelRx -> SpiRx;
   }
   \enddot
*/
circular_declare_ex(USHORT, USHORT, usSpiChannelCntRx, SPI_BUFFERCNTRX, [SPI_UNIT_MAX][SPI_CHANNEL_MAX]);

//!   receive data packet size (application character count) for auto segmentation of current data retrieval by pdc
/**
   data flow thru usSpiChannelCntRxQ
   \dot
   digraph usSpiChannelCntRxQ {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      SpiIrqHandler [label="SpiIrqHandler"];
      MbxSpiRx [label="MbxSpiRx"];
      ucSpiAddrRx [label="ucSpiAddrRx"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ" style="filled"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx"];
      ucSpiChannelRx [label="ucSpiChannelRx"];
      usSpiChannelCntRx [label="usSpiChannelCntRx"];
      SCircular [label="SSpiRW.SCircular" URL="\ref _SCircular"];
      BufferRx [label="SSpiRW.ulPDCBufferRx" URL="\ref _SSpiRW"];
      TskSpiRx [label="TskSpiRx" shape="ellipse"];
      TskSpiTx [label="TskSpiTx" style="dashed" shape="ellipse"];
      SpiRx [label="SpiRxTag / SpiRx" shape="invtrapezium" URL="\ref SpiRxTag"];
      
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ucSpiAddrRx;
      
      Spi -> SpiIrqHandler [arrowhead="odot"];
      Spi -> BufferRx;
      
      SpiIrqHandler -> Spi;
      SpiIrqHandler -> MbxSpiRx;
      SpiIrqHandler -> SCircular;
      
      MbxSpiRx -> TskSpiRx [arrowhead="odot"];
      SCircular -> TskSpiRx;
      BufferRx -> TskSpiRx;
      ucSpiAddrRx -> TskSpiRx;
      usSpiChannelCntRxQ -> TskSpiRx;
      
      TskSpiRx -> ucSpiChannelRx;
      TskSpiRx -> usSpiChannelCntRx;
      
      usSpiChannelCntRx -> SpiRx;
      ucSpiChannelTagRx -> SpiRx;
      ucSpiChannelRx -> SpiRx;
   }
   \enddot
*/
circular_declare_ex(USHORT, USHORT, usSpiChannelCntRxQ, SPI_BUFFERCNTRX, [SPI_UNIT_MAX][SPI_CHANNEL_MAX]);

//!   send data packet size (application character count) for auto segmentation of application character packets of ucSpiChannelTx
/**
   data flow thru usSpiChannelCntTx
   \dot
   digraph usSpiChannelCntTx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      SpiTx [label="SpiTxTag / SpiTx" shape="trapezium" URL="\ref SpiTxTag"];
      SemTickTx [label="SSpiRW.SemTickTx" URL="\ref _SSpiRW"];
      ucSpiChannelTx [label="ucSpiChannelTx"];
      usSpiChannelCntTx [label="usSpiChannelCntTx" style="filled"];
      ucSpiChannelTagTx [label="ucSpiChannelTagTx"];
      BufferTx [label="SSpiRW.ulPDCBufferTx" URL="\ref _SSpiRW"];
      TskSpiTx [label="TskSpiTx" shape="ellipse"];
      ulSpiTx [label="ulSpiTx"];
      ulSpiCntTx [label="ulSpiCntTx"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx"];
      ucSpiAddrRx [label="ucSpiAddrRx"];
      SpiIrqHandler [label="SpiIrqHandler"];
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      
      SpiTx -> SemTickTx;
      SpiTx -> ucSpiChannelTx;
      SpiTx -> usSpiChannelCntTx;
      SpiTx -> ucSpiChannelTagTx;
          
      SemTickTx -> TskSpiTx [arrowhead="odot"];
      ucSpiChannelTx -> TskSpiTx;
      usSpiChannelCntTx -> TskSpiTx;
      ucSpiChannelTagTx -> TskSpiTx;
          
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ulSpiCntTx;
      TskSpiTx -> ucSpiAddrRx;
      TskSpiTx -> ulSpiTx;
      
      ulSpiTx -> SpiIrqHandler;
      ulSpiCntTx -> SpiIrqHandler;
      SpiIrqHandler -> BufferTx;
      BufferTx -> Spi;
      SpiIrqHandler -> Spi;
      Spi -> SpiIrqHandler [arrowhead="odot"];
      SpiIrqHandler -> SemTickTx;
   }
   \enddot
*/
circular_declare_ex(USHORT, USHORT, usSpiChannelCntTx, SPI_BUFFERCNTTX, [SPI_UNIT_MAX][SPI_CHANNEL_MAX]);

//!   receive data packet tag (color) for conditional data retrieval
/**
   data flow thru ucSpiChannelTagRx
   \dot
   digraph ucSpiChannelTagRx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      SpiTx [label="SpiTxTag / SpiTx" shape="trapezium" URL="\ref SpiTxTag"];
      SemTickTx [label="SSpiRW.SemTickTx" URL="\ref _SSpiRW"];
      ucSpiChannelTx [label="ucSpiChannelTx"];
      usSpiChannelCntTx [label="usSpiChannelCntTx"];
      ucSpiChannelTagTx [label="ucSpiChannelTagTx"];
      BufferTx [label="SSpiRW.ulPDCBufferTx" URL="\ref _SSpiRW"];
      TskSpiTx [label="TskSpiTx" shape="ellipse"];
      ulSpiTx [label="ulSpiTx"];
      ulSpiCntTx [label="ulSpiCntTx"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx" style="filled"];
      ucSpiAddrRx [label="ucSpiAddrRx"];
      SpiIrqHandler [label="SpiIrqHandler"];
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      
      SpiTx -> SemTickTx;
      SpiTx -> ucSpiChannelTx;
      SpiTx -> usSpiChannelCntTx;
      SpiTx -> ucSpiChannelTagTx;
          
      SemTickTx -> TskSpiTx [arrowhead="odot"];
      ucSpiChannelTx -> TskSpiTx;
      usSpiChannelCntTx -> TskSpiTx;
      ucSpiChannelTagTx -> TskSpiTx;
          
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ulSpiCntTx;
      TskSpiTx -> ucSpiAddrRx;
      TskSpiTx -> ulSpiTx;
      
      ulSpiTx -> SpiIrqHandler;
      ulSpiCntTx -> SpiIrqHandler;
      SpiIrqHandler -> BufferTx;
      BufferTx -> Spi;
      SpiIrqHandler -> Spi;
      Spi -> SpiIrqHandler [arrowhead="odot"];
      SpiIrqHandler -> SemTickTx;
   }
   \enddot
*/
circular_declare_ex(UCHAR, USHORT, ucSpiChannelTagRx, SPI_BUFFERCNTRX, [SPI_UNIT_MAX][SPI_CHANNEL_MAX]);

//!   send data packet tag (color) for conditional data retrieval (will be applied to simultaneous retrieved data)
/**
   data flow thru ucSpiChannelTagTx
   \dot
   digraph ucSpiChannelTagTx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      SpiTx [label="SpiTxTag / SpiTx" shape="trapezium" URL="\ref SpiTxTag"];
      SemTickTx [label="SSpiRW.SemTickTx" URL="\ref _SSpiRW"];
      ucSpiChannelTx [label="ucSpiChannelTx"];
      usSpiChannelCntTx [label="usSpiChannelCntTx"];
      ucSpiChannelTagTx [label="ucSpiChannelTagTx" style="filled"];
      BufferTx [label="SSpiRW.ulPDCBufferTx" URL="\ref _SSpiRW"];
      TskSpiTx [label="TskSpiTx" shape="ellipse"];
      ulSpiTx [label="ulSpiTx"];
      ulSpiCntTx [label="ulSpiCntTx"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx"];
      ucSpiAddrRx [label="ucSpiAddrRx"];
      SpiIrqHandler [label="SpiIrqHandler"];
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      
      SpiTx -> SemTickTx;
      SpiTx -> ucSpiChannelTx;
      SpiTx -> usSpiChannelCntTx;
      SpiTx -> ucSpiChannelTagTx;
          
      SemTickTx -> TskSpiTx [arrowhead="odot"];
      ucSpiChannelTx -> TskSpiTx;
      usSpiChannelCntTx -> TskSpiTx;
      ucSpiChannelTagTx -> TskSpiTx;
          
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ulSpiCntTx;
      TskSpiTx -> ucSpiAddrRx;
      TskSpiTx -> ulSpiTx;
      
      ulSpiTx -> SpiIrqHandler;
      ulSpiCntTx -> SpiIrqHandler;
      SpiIrqHandler -> BufferTx;
      BufferTx -> Spi;
      SpiIrqHandler -> Spi;
      Spi -> SpiIrqHandler [arrowhead="odot"];
      SpiIrqHandler -> SemTickTx;
   }
   \enddot
*/
circular_declare_ex(UCHAR, USHORT, ucSpiChannelTagTx, SPI_BUFFERCNTTX, [SPI_UNIT_MAX][SPI_CHANNEL_MAX]);


// Declare arrays of mailboxes (size definitions are located inside spi.h)

//!   receive data message queue
/**
   data flow thru MbxSpiRx
   \dot
   digraph MbxSpiRx {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Spi [label="Spi PDC (DMA)" shape="parallelogram"];
      SpiIrqHandler [label="SpiIrqHandler"];
      MbxSpiRx [label="MbxSpiRx" style="filled"];
      ucSpiAddrRx [label="ucSpiAddrRx"];
      usSpiChannelCntRxQ [label="usSpiChannelCntRxQ"];
      ucSpiChannelTagRx [label="ucSpiChannelTagRx"];
      ucSpiChannelRx [label="ucSpiChannelRx"];
      usSpiChannelCntRx [label="usSpiChannelCntRx"];
      SCircular [label="SSpiRW.SCircular" URL="\ref _SCircular"];
      BufferRx [label="SSpiRW.ulPDCBufferRx" URL="\ref _SSpiRW"];
      TskSpiRx [label="TskSpiRx" shape="ellipse"];
      TskSpiTx [label="TskSpiTx" style="dashed" shape="ellipse"];
      SpiRx [label="SpiRxTag / SpiRx" shape="invtrapezium" URL="\ref SpiRxTag"];
      
      TskSpiTx -> usSpiChannelCntRxQ;
      TskSpiTx -> ucSpiChannelTagRx;
      TskSpiTx -> ucSpiAddrRx;
      
      Spi -> SpiIrqHandler [arrowhead="odot"];
      Spi -> BufferRx;
      
      SpiIrqHandler -> Spi;
      SpiIrqHandler -> MbxSpiRx;
      SpiIrqHandler -> SCircular;
      
      MbxSpiRx -> TskSpiRx [arrowhead="odot"];
      SCircular -> TskSpiRx;
      BufferRx -> TskSpiRx;
      ucSpiAddrRx -> TskSpiRx;
      usSpiChannelCntRxQ -> TskSpiRx;
      
      TskSpiRx -> ucSpiChannelRx;
      TskSpiRx -> usSpiChannelCntRx;
      
      usSpiChannelCntRx -> SpiRx;
      ucSpiChannelTagRx -> SpiRx;
      ucSpiChannelRx -> SpiRx;
   }
   \enddot
*/
os_mbx_declare_ex(MbxSpiRx, SPI_MBXCNTRX, [SPI_UNIT_MAX]);

//!   unit notification message queue
/**
   data flow thru MbxSpiMsg
   \dot
   digraph MbxSpiMsg {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Spi [label="Spi peripheral" shape="parallelogram"];
      SpiIrqHandler [label="SpiIrqHandler"];
      MbxSpiMsg [label="MbxSpiMsg" style="filled"];
      TskSpiMsg [label="TskSpiMsg" shape="ellipse"];
      xMsgCounter [label="SSpiRW.xMsgCounter" URL="\ref _SSpiRW"];
      SpiMsgUnit [label="SpiMsgUnit" shape="invtrapezium" URL="\ref SpiMsgUnit"];

      Spi -> SpiIrqHandler [arrowhead="odot"];
      SpiIrqHandler -> MbxSpiMsg;
      MbxSpiMsg -> TskSpiMsg [arrowhead="odot"];
      TskSpiMsg -> xMsgCounter;
      xMsgCounter -> SpiMsgUnit;
   }
   \enddot
*/
os_mbx_declare_ex(MbxSpiMsg, SPI_MBXCNTMSG, [SPI_UNIT_MAX]);

//!   channel notification message queue
/**
   data flow thru MbxSpiMsgChannel
   \dot
   digraph MbxSpiMsgChannel {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Spi [label="Spi peripheral" shape="parallelogram"];
      SpiIrqHandler [label="SpiIrqHandler"];
      MbxSpiMsgChannel [label="MbxSpiMsgChannel" style="filled"];
      TskSpiMsgChannel [label="TskSpiMsgChannel" shape="ellipse"];
      xMsgCounter [label="SSpiChannelRW.xMsgCounter" URL="\ref _SSpiChannelRW"];
      SpiMsgChannel [label="SpiMsgChannel" shape="invtrapezium" URL="\ref SpiMsgChannel"];

      Spi -> SpiIrqHandler [arrowhead="odot"];
      SpiIrqHandler -> MbxSpiMsgChannel;
      MbxSpiMsgChannel -> TskSpiMsgChannel [arrowhead="odot"];
      TskSpiMsgChannel -> xMsgCounter;
      xMsgCounter -> SpiMsgChannel;
   }
   \enddot
*/
os_mbx_declare_ex(MbxSpiMsgChannel, SPI_MBXCNTMSG, [SPI_UNIT_MAX]);


// Customized stacks
OS_TSK_STACK_USER_EX(StkTskSpiRx, SPI_STACK_TSKSPIRX, [SPI_UNIT_MAX]);
OS_TSK_STACK_USER_EX(StkTskSpiTx, SPI_STACK_TSKSPITX, [SPI_UNIT_MAX]);

//! Hardware descriptors
struct _SSpiRO SSpiRO[SPI_UNIT_MAX] =
{
   {
      /* pBase          */ AT91C_BASE_SPI1,
      /* u32PeriphId    */ AT91C_ID_SPI1,
      /* u32IrqRx       */ AT91C_SPI_RXBUFF | AT91C_SPI_ENDRX,
      /* u32IrqTx       */ AT91C_SPI_TXBUFE,
      /* u32IrqMsg      */ AT91C_SPI_RXBUFF | AT91C_SPI_ENDRX | AT91C_SPI_TXBUFE | AT91C_SPI_OVRES,
      /* u8TskPriMsg    */ OS_P_REALTIME,
      /* u8TskPriRx     */ OS_P_ABOVEHIGH,
      /* u8TskPriTx     */ OS_P_ABOVEHIGH,
      /* u16DelayRx     */ _OS_T_MS(0, OS_TICK_RATE),
      /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE),
      /* u16AgeStepTx   */ 1,
      /* u16PDCMax      */ SPI_PDCMAX,
      /* Isr            */ IsrSpi0
   }
#if SPI_UNIT_MAX > 1
   ,
   {
      /* pBase          */ AT91C_BASE_SPI0,
      /* u32PeriphId    */ AT91C_ID_SPI0,
      /* u32IrqRx       */ AT91C_SPI_RXBUFF | AT91C_SPI_ENDRX,
      /* u32IrqTx       */ AT91C_SPI_TXBUFE,
      /* u32IrqMsg      */ AT91C_SPI_RXBUFF | AT91C_SPI_ENDRX | AT91C_SPI_TXBUFE | AT91C_SPI_OVRES | AT91C_SPI_NSSR,
      /* u8TskPriMsg    */ OS_P_REALTIME,
      /* u8TskPriRx     */ OS_P_ABOVEHIGH,
      /* u8TskPriTx     */ OS_P_ABOVEHIGH,
      /* u16DelayRx     */ _OS_T_MS(0, OS_TICK_RATE),
      /* u16DelayTx     */ _OS_T_MS(0, OS_TICK_RATE),
      /* u16AgeStepTx   */ 1,
      /* u16PDCMax      */ SPI_PDCMAX,
      /* Isr            */ IsrSpi1
   }
#endif
};

#endif
