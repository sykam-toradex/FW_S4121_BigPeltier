//!	data definitions of AT91SAM7X UDPRAW driver
/**
		\file		udprawdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/UDP/RAW/UDPRAWDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2014-09-10 initial version
// 2014-11-05 changes in doxygen documentation
// 2014-11-13 cleanup of ro channel data
// 2015-02-05 added OS_TIDs TskUdprawRx & TskUdprawTx in _SUdprawChannelRW
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_UDP_RAW_UDPRAWDATA_H
#define SYKAM_CPU_AT91_UDP_RAW_UDPRAWDATA_H 1


//! \internal \brief <b>Internal data:</b> Task parameter data for each unit
struct _SUdprawTsk
{
   UCHAR ucUnit;                                                     //!< Unit task is assigned to
   UCHAR ucChannel;                                                  //!< Channel task is assigned to
};

//! \internal \brief <b>Internal data:</b> Task parameter data for each channel
struct _SUdprawTskChannel
{
   UCHAR ucUnit;                                                     //!< Unit task is assigned to
   UCHAR ucChannel;                                                  //!< Channel task is assigned to
};

//! Hardware descriptor data (read only) for each unit
struct _SUdprawRO
{
   AT91PS_UDP              pBase;                                    //!< Base address of peripheral to handle
   U32                     u32PeriphId;                              //!< Bit-Id (!) of peripheral to handle
   U32                     u32Irq;                                   //!< Mask of irqs for data and function (UDP_ISR & SOFINT details FRM_OK/FRM_ERR of UDP_FRM_NUM at unused 15...14)
   U8                      u8TskPriMsg;                              //!< Priority of msg tasks (unit & all channel)
   void (*Isr) (void)__irq;                                          //!< Isr handler to call
};

//! Hardware descriptor data (read only) for each channel
struct _SUdprawChannelRO
{
   U16                     u16CSR;                                   //!< Mask of irqs for data and function (UDP_CSR[x], fallback for AT91C_UDP_ENDBUSRES)
   U8                      u8TskPriRx;                               //!< Priority of rx task (for consumer/handler data handshake)
   U8                      u8TskPriTx;                               //!< Priority of tx task (for consumer/handler data handshake)
                                                                     //!< Isr handler by own function but called only by summarized unit's isr
};

//! <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SUdprawRW
{
   UCHAR                   ucUnit;                                   //!< Unit task is assigned to
   UCHAR                   ucChannelIdx;                             //!< Udpraw unit next channel storage
   UCHAR                   ucFlags;                                  //!< Udpraw unit flags (refer to udpraw.h)
   UDPRAW_MSGCOUNTERTYPE   xMsgCounter[UDPRAW_UNIT_MSGMAX+ 1];       //!< Udpraw unit msg array with msg counters
   OS_ID                   oiSemMsgNew;                              //!< Udpraw unit os_id of external taken semaphore to give for new msg available
   ULONG                   ulMsgMask;                                //!< Udpraw unit msg mask
   OS_SEM                  SemMsg;                                   //!< Udpraw unit msg data (counter) lock access
   OS_TID                  tidTskMsg;                                //!< Udpraw unit msg task OS_TID
   OS_TID                  tidTskMsgChannel;                         //!< Udpraw unit all channel msg task OS_TID
};

//! <b>Internal data:</b> Channel storage mapping (read/write) to map each channel to a contiguous array storage index
struct _SUdprawChannelMapRW
{
   UCHAR                ucStorageIdx;                                //!< Udpraw channel storage array index
};

//! <b>Internal data:</b> Runtime data (read/write) for each channel
struct _SUdprawChannelRW
{
   UCHAR                   ucUnit;                                   //!< Unit task is assigned to
   USHORT                  usType;                                   //!< Udpraw channel type to enable rx/host-out (#UDPRAW_IS_EPTYPE_OUT() != 0) or tx/host-in (#UDPRAW_IS_EPTYPE_IN() != 0) or both (#UDPRAW_IS_EPTYPE_BOTH() != 0), refer to #AT91C_UDP_EPTYPE and udpraw.h
   UDPRAW_MSGCOUNTERTYPE   xMsgCounter[UDPRAW_CHANNEL_MSGMAX+ 1];    //!< Udpraw channel msg array with msg counters

   UCHAR                   *pucRx;                                   //!< Low latency data rx: external supplied data buffer (fifo copied & freed by isr to assigned buffer)
   OS_ID                   oiMbxRxQ;                                 //!< Low latency data rx: os_id of external supplied mailbox to annouce new data rx by its UDP_CSR[x] (27 bit, reduced u8RxBkFirst (0= BK0/SETUP, 1= BK1) at unused 14 and 26...0) and channel id (4 bit, 4...1) plus count (1 bit, at 0) (5 bit as msg count)
   OS_SEM                  SemRx;                                    //!< Low latency data rx: internal taken semaphore to unlock data rx (triggers channel rx task to arm for next rx) after processing by consumer/handler give to prepare/arm for next data

   UCHAR                   ucTxBankQueue;                            //!< Low latency data tx: bank queue counter
   UCHAR                   *pucTx;                                   //!< Low latency data tx: external supplied data buffer \note data tx triggered by MbxUdprawTxChannel in udprawdev.h
   OS_ID                   oiMbxTxQ;                                 //!< Low latency data tx: os_id of external mailbox to unlock data buffer tx after processing and to send result code to (27 bit, success at 6 (0= UDPRAW_TXRES_ERR, 1= UDPRAW_TXRES_OK)) and channel id (4 bit, 4...1) plus count (1 bit, at 0) (5 bit as msg count)
   
   OS_ID                   oiSemMsgNew;                              //!< Udpraw channel os_id of external taken semaphore to give for new channel msg
   ULONG                   ulMsgMask;                                //!< Udpraw channel msg mask
   OS_SEM                  SemMsg;                                   //!< Udpraw channel semaphore to lock msg data (counter) access
   
   OS_SEM                  SemTxComp;                                //!< Udpraw channel initially taken semaphore to enable new data tx after all tx bank complete
     
   U8                      u8RxBkFirst;                              //!< Udpraw channel data rx dual port ram bank by UDP_CSR[x] (8 bit, set bit 6 / 1) to acknowledge first (RX_DATA_BK0 / RX_DATA_BK1)
   U8                      u8RxBkQueue;                              //!< Udpraw channel data rx dual port ram bank by UDP_CSR[x] (8 bit, set bit 6 / 1) to acknowledge queue (RX_DATA_BK0 / RX_DATA_BK1)
   U8                      u8RxBkAck;                                //!< Udpraw channel data rx dual port ram bank by UDP_CSR[x] (8 bit, set bit 6 / 1) acknowledge done (RX_DATA_BK0 / RX_DATA_BK1)
   UCHAR                   ucFlags;                                  //!< Udpraw channel flags (refer to udpraw.h)
   
   OS_TID                  tidTskRx;                                 //!< Udpraw channel task rx OS_TID
   OS_TID                  tidTskTx;                                 //!< Udpraw channel task tx OS_TID
};

// Number of units and channels located inside udpraw.h
struct _SUdprawChannelRW SUdprawChannelRW[UDPRAW_UNIT_MAX][UDPRAW_CHANNEL_MAX];
struct _SUdprawChannelMapRW SUdprawChannelMapRW[UDPRAW_UNIT_MAX][UDPRAW_CHANNELID_MAX];
struct _SUdprawTskChannel SUdprawTskChannel[UDPRAW_UNIT_MAX][UDPRAW_CHANNEL_MAX];
struct _SUdprawRW SUdprawRW[UDPRAW_UNIT_MAX];
struct _SUdprawTsk SUdprawTsk[UDPRAW_UNIT_MAX];

#endif
