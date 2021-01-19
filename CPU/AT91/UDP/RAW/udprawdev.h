//!	hardware descriptor(s) of AT91SAM7X UDPRAW driver
/**
		\file		udprawdev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/UDP/RAW/UDPRAWDEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2014-11-13 initial version
// 2015-01-08 fixed declaration of MbxUdprawMsgChannel
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_UDP_RAW_UDPRAWDEV_H
#define SYKAM_CPU_AT91_UDP_RAW_UDPRAWDEV_H 1


// Declare message queues (size definitions are located inside udpraw.h)

//!   unit notification message queue
/**
   data flow thru MbxUdprawMsg
   \dot
   digraph MbxUdprawMsg {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Udpraw [label="Udpraw peripheral" shape="parallelogram"];
      UdprawIrqHandler [label="UdprawIrqHandler"];
      MbxUdprawMsg [label="MbxUdprawMsg" style="filled"];
      TskUdprawMsg [label="TskUdprawMsg" shape="ellipse"];
      xMsgCounter [label="SUdprawRW.xMsgCounter" URL="\ref _SUdprawRW"];
      UdprawMsg [label="UdprawMsg" shape="invtrapezium" URL="\ref UdprawMsg"];

      Udpraw -> UdprawIrqHandler [arrowhead="odot"];
      UdprawIrqHandler -> MbxUdprawMsg;
      MbxUdprawMsg -> TskUdprawMsg [arrowhead="odot"];
      TskUdprawMsg -> xMsgCounter;
      xMsgCounter -> UdprawMsg;
   }
   \enddot
*/
os_mbx_declare_ex(MbxUdprawMsg, UDPRAW_MBXCNTMSG, [UDPRAW_UNIT_MAX]);

//!   channel notification message queue
/**
   data flow thru MbxUdpRawMsgChannel
   \dot
   digraph MbxUdpRawMsgChannel {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Udpraw [label="Udpraw peripheral" shape="parallelogram"];
      UdprawIrqHandler [label="UdprawIrqHandler"];
      MbxUdprawMsgChannel [label="MbxUdprawMsgChannel" style="filled"];
      TskUdprawMsgChannel [label="TskUdprawMsgChannel" shape="ellipse"];
      xMsgCounter [label="SUdprawChannelRW.xMsgCounter" URL="\ref _SUdprawChannelRW"];
      UdprawMsgChannel [label="UdprawMsgChannel" shape="invtrapezium" URL="\ref UdprawMsgChannel"];

      Udpraw -> UdprawIrqHandler [arrowhead="odot"];
      UdprawIrqHandler -> MbxUdprawMsgChannel;
      MbxUdprawMsgChannel -> TskUdprawMsgChannel [arrowhead="odot"];
      TskUdprawMsgChannel -> xMsgCounter;
      xMsgCounter -> UdprawMsgChannel;
   }
   \enddot
*/
os_mbx_declare_ex(MbxUdprawMsgChannel, UDPRAW_MBXCNTMSG, [UDPRAW_UNIT_MAX]);

//!   channel annouce (prepare/arm for) new data tx
/**
   data flow thru MbxUdprawTxChannel
   \remark internal mailbox (triggers channel tx task to copy ext buffer to fifo) to annouce (prepare/arm for) new data tx by size in bytes (11 bit, 26...16 like UDP_CSR), timeout (16 bit, 15...0) and channel id (4 bit as msg count) plus count (1 bit, at 0) (5 bit as msg count)
   \dot
   digraph MbxUdpRawMsgChannel {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      TskClientTxChannel [label="TskClientTxChannel" shape="ellipse"];
      MbxUdprawTxChannel [label="MbxUdprawTxChannel" style="filled"];
      TskUdprawTx [label="TskUdprawTx" shape="ellipse"];
      
      TskClientTxChannel -> MbxUdprawTxChannel;
      MbxUdprawTxChannel -> TskUdprawTx [arrowhead="odot"];
   }
   \enddot
*/
os_mbx_declare_ex(MbxUdprawTxChannel, UDPRAW_MBXCNTTX, [UDPRAW_UNIT_MAX][UDPRAW_CHANNEL_MAX]);

//! Hardware descriptors of units
struct _SUdprawRO SUdprawRO[UDPRAW_UNIT_MAX] =
{
   {
      /* pBase          */ AT91C_BASE_UDP,
      /* u32PeriphId    */ AT91C_ID_UDP,
      /* u32Irq         */ AT91C_UDP_WAKEUP | AT91C_UDP_ENDBUSRES | AT91C_UDP_RXRSM | AT91C_UDP_RXSUSP,
      /* u8TskPriMsg    */ OS_P_REALTIME,
      /* Isr            */ IsrUdpraw0
   }
#if UDPRAW_UNIT_MAX > 1
   ,
   {
      /* pBase          */ AT91C_BASE_UDP1,
      /* u32PeriphId    */ AT91C_ID_UDP1,
      /* u32Irq         */ AT91C_UDP_WAKEUP | AT91C_UDP_ENDBUSRES | AT91C_UDP_RXRSM | AT91C_UDP_RXSUSP,
      /* u8TskPriMsg    */ OS_P_REALTIME,
      /* Isr            */ IsrUdpraw1

   }
#endif
};

//! Hardware descriptors of channels
struct _SUdprawChannelRO SUdprawChannelRO[UDPRAW_UNIT_MAX][UDPRAW_CHANNEL_MAX] =
{
   #if UDPRAW_UNIT_MAX > 0
   {
      #if UDPRAW_CHANNEL_MAX > 0
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
      #if UDPRAW_CHANNEL_MAX > 1
      ,
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
      #if UDPRAW_CHANNEL_MAX > 2
      ,
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
      #if UDPRAW_CHANNEL_MAX > 3
      ,
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
      #if UDPRAW_CHANNEL_MAX > 4
      ,
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
      #if UDPRAW_CHANNEL_MAX > 5
      ,
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
   }
   #endif
   ,
   #if UDPRAW_UNIT_MAX > 1
   {
      #if UDPRAW_CHANNEL_MAX > 0
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
      #if UDPRAW_CHANNEL_MAX > 1
      ,
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
      #if UDPRAW_CHANNEL_MAX > 2
      ,
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
      #if UDPRAW_CHANNEL_MAX > 3
      ,
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
      #if UDPRAW_CHANNEL_MAX > 4
      ,
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
      #if UDPRAW_CHANNEL_MAX > 5
      ,
      {
         /* u16CSR         */ AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP,
         /* u8TskPriRx     */ OS_P_ABOVEHIGH,
         /* u8TskPriTx     */ OS_P_ABOVEHIGH
      }
      #endif
   }
   #endif
};

#endif
