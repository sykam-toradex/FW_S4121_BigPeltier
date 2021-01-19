//!	data definitions of AT91SAM7X SPI driver
/**
		\file		spidata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/SPI/SPIDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-10-07 initial version
// 2009-10-14 added ucMsgCounter to _SSpiChannelRW
// 2009-10-15 added u16AgeStepTxMS to _SSpiRO
// 2009-10-15 added __packed attribute for _SSpiPDCItem
// 2009-10-21 added ulMsgMask to _SSpiChannelRW
// 2009-11-04 moved SpiTsk data into _SSpiRW and _SSpiChannelRW
// 2009-11-06 changed msg counter array to xMsgCounter with user data type
// 2009-12-18 added usRxPendingCnt to _SSpiChannelRW
// 2009-12-21 added ucRxPendingTag to _SSpiChannelRW
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-08-27 added _SCircular structure definition
// 2010-08-27 added adjustable dimension to buffer ulPDCBufferRx
// 2010-09-22 renamed u16AgeStepTxMS to u16AgeStepTx (ticks)
// 2010-09-22 renamed usMaxAgeTxMS to u16MaxAgeTx
// 2010-09-22 renamed usAgeTxMS to u16AgeTx
// 2010-09-23 added SemTickTx to _SSpiRW for re-triggerabel cycle of TskSpiTx
// 2010-11-23 removed obsolete data item ucChannel of _SSpiChannelRW
// 2010-11-23 added data comments
// 2010-12-08 added storage modifier volatile for ulPDCBufferRx
// 2010-12-09 added ucADRBufferRx to _SSpiRW for hardware rx address fix
// 2010-12-09 re-introduced the define SPI_HWFIX_PDCRX
// 2011-01-19 removed SPI_HWFIX_PDCRX dependend code in this file
// 2011-04-06 added ucIrqTxState to _SSpiRW
// 2011-04-26 added SemRx to _SSpiRW
// 2011-07-07 SPI_UNIT/CHANNEL_MSGMAX are now a zero based settings
// 2011-10-26 added public doxygen documentation
// 2013-08-21 changed delays from milliseconds to ticks
// 2013-12-05 added ucChannelIdx to _SSpiRW, added struct _SSpiChannelMapRW
// 2016-02-10 removed data exclusions for SPI_HWFIX_PDCRX define
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_SPI_SPIDATA_H
#define SYKAM_CPU_AT91_SPI_SPIDATA_H 1


//! Hardware descriptor data (read only) for each unit
struct _SSpiRO
{
   AT91PS_SPI           pBase;                                    //!< Base address of peripheral to handle
   U32                  u32PeriphId;                              //!< Bit-Id (!) of peripheral to handle
   U32                  u32IrqRx;                                 //!< Mask of rx irq
   U32                  u32IrqTx;                                 //!< Mask of tx irq
   U32                  u32IrqMsg;                                //!< Mask of irqs to monitor and count
   U8                   u8TskPriMsg;                              //!< Priority of msg task
   U8                   u8TskPriRx;                               //!< Priority of rx task
   U8                   u8TskPriTx;                               //!< Priority of tx task
   U16                  u16DelayRx;                               //!< Delay after rx cycle <b>(in ticks)</b>
   U16                  u16DelayTx;                               //!< Delay after tx cycle <b>(in ticks)</b>
   U16                  u16AgeStepTx;                             //!< Aging amount for each data scheduler's delay cycle
   U16                  u16PDCMax;                                //!< Size of dma (pdc) buffer (true maximum transfer block size)
   void (*Isr) (void)__irq;                                       //!< Isr handler to call
};

//! <b>Internal data:</b> Circular buffer handling (pdc rx buffer array handling)
struct _SCircular
{
   UCHAR                ucCntEntries;                             //!< Count of used circular buffer entries
   UCHAR                ucIdxRead;                                //!< Index for next read access
   UCHAR                ucIdxWrite;                               //!< Index for next write access
   UCHAR                ucSize;                                   //!< Number of available circular buffer entries
};

//! <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SSpiRW
{
   UCHAR                ucUnit;                                   //!< Unit task is assigned to
   volatile ULONG       ulPDCBufferRx[SPI_PDCDEPTH][SPI_PDCMAX];  //!< Spi unit rx buffer (32 Bit)
   ULONG                ulPDCBufferTx[SPI_PDCMAX];                //!< Spi unit tx buffer (32 Bit)
   SPI_MSGCOUNTERTYPE   xMsgCounter[SPI_UNIT_MSGMAX+ 1];          //!< Spi unit msg array with msg counters
   UCHAR                ucPrioMask;                               //!< Spi unit prio mask for all existing priority levels (0, 2^0 - 2^7)
   UCHAR                ucIrqTxState;                             //!< Spi unit tx irq state
   OS_ID                oiSemMsgNew;                              //!< Spi unit os_id of external taken semaphore to give for new msg available
   ULONG                ulMsgMask;                                //!< Spi unit msg mask
   OS_SEM               SemMsg;                                   //!< Spi unit msg data lock
   OS_SEM               SemRx;                                    //!< Spi unit rx data lock
   OS_SEM               SemTx;                                    //!< Spi unit tx data lock
   OS_SEM               SemTickTx;                                //!< Spi unit tx re-triggerable tick cycle
   OS_TID               tidTskMsg;                                //!< Spi unit msg task OS_ID
   OS_TID               tidTskMsgChannel;                         //!< Spi unit all channel msg task OS_ID
   OS_TID               tidTskRx;                                 //!< Spi unit tx task OS_ID
   OS_TID               tidTskTx;                                 //!< Spi unit rx task OS_ID
   UCHAR                ucFlags;                                  //!< Spi unit flags (refer to spi.h)
   UCHAR                ucChannelIdx;                             //!< Spi unit next channel storage
   struct _SCircular    SCircular;                                //!< Circular buffer handling (pdc rx buffer array handling)
};

//! <b>Internal data:</b> Channel storage mapping (read/write) to map each channel to a contiguous array storage index
struct _SSpiChannelMapRW
{
   UCHAR                ucStorageIdx;                             //!< Spi channel storage array index
};

//! <b>Internal data:</b> Runtime data (read/write) for each channel
struct _SSpiChannelRW
{
   UCHAR                ucUnit;                                   //!< Unit task is assigned to
   SPI_MSGCOUNTERTYPE   xMsgCounter[SPI_CHANNEL_MSGMAX+ 1];       //!< Spi channel msg array with msg counters
   OS_ID                oiSemMsgNew;                              //!< Spi channel os_id of external taken semaphore to give for new channel msg (e.g. data available)
   ULONG                ulMsgMask;                                //!< Spi channel msg mask
   OS_SEM               SemMsg;                                   //!< Spi channel semaphore to lock msg data
   OS_SEM               SemRx;                                    //!< Spi channel semaphore to lock rx data
   OS_SEM               SemTx;                                    //!< Spi channel semaphore to lock tx data
   U32                  u32CSR;                                   //!< Spi channel individual CSR register content (allows incompatible channel configs within a csr-block)
   USHORT               usBytesPerXfer;                           //!< Spi channel insert LASTXFER flag after this number of sent bytes (to close cycle for slaves using ext. cycles by CSAAT)
   U16                  u16MaxAge;                                //!< Spi channel maxium age data may obtain until transmission is forced regardless of the priority
   U16                  u16AgeTx;                                 //!< Spi channel current age of queued tx data
   USHORT               usRxPendingCnt;                           //!< Spi channel pending count of partially ready rx job data
   UCHAR                ucRxPendingTag;                           //!< Spi channel pending tag of partially ready rx job data
   UCHAR                ucPrio;                                   //!< Spi channel priority for transmission data order
   UCHAR                ucFlags;                                  //!< Spi channel flags (refer to spi.h)
};

// Number of units and channels located inside spi.h
struct _SSpiChannelRW SSpiChannelRW[SPI_UNIT_MAX][SPI_CHANNEL_MAX];
struct _SSpiChannelMapRW SSpiChannelMapRW[SPI_UNIT_MAX][SPI_CHANNELID_MAX];
struct _SSpiRW SSpiRW[SPI_UNIT_MAX];

#endif
