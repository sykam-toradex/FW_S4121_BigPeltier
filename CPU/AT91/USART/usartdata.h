//!	data definitions of AT91SAM7X USART driver
/**
		\file		usartdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/USART/USARTDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-09-25 initial version
// 2009-09-25 added _SUsartTsk
// 2009-09-30 cleaned up data structures
// 2009-10-06 msg counter type is now configurable by user via usart.h
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-08-27 added _SCircular structure definition
// 2010-11-23 removed obsolete data item ucChannel of _SUsartTsk
// 2010-12-08 added storage modifier volatile for ucPDCBufferRx
// 2011-06-01 renamed USART_UNITMAX to USART_UNIT_MAX
// 2011-06-30 added SemTickTx to _SUsartRW
// 2011-07-07 USART_MSGMAX is now a zero based setting
// 2011-10-20 added public doxygen documentation
// 2011-11-24 added internal doxygen documentation
// 2013-08-21 changed delays from milliseconds to ticks
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_USART_USARTDATA_H
#define SYKAM_CPU_AT91_USART_USARTDATA_H 1


//! \internal \brief <b>Internal data:</b> Task parameter data
struct _SUsartTsk
{
   UCHAR ucUnit;                                                           //!< Unit task is assigned to
};

//! Hardware descriptor data (read only) for each unit
struct _SUsartRO
{
   AT91PS_USART         pBase;                                             //!< Base address of peripheral to handle
   U32                  u32PeriphId;                                       //!< Bit-Id (!) of peripheral to handle
   U32                  u32IrqRx;                                          //!< Mask of rx irq
   U32                  u32IrqTx;                                          //!< Mask of tx irq
   U32                  u32IrqMsg;                                         //!< Mask of irqs to monitor and count
   U8                   u8TskPriMsg;                                       //!< Priority of msg task
   U8                   u8TskPriRx;                                        //!< Priority of rx task
   U8                   u8TskPriTx;                                        //!< Priority of tx task
   U16                  u16DelayRx;                                        //!< Delay after rx cycle <b>(in ticks)</b>
   U16                  u16DelayTx;                                        //!< Delay after tx cycle <b>(in ticks)</b>
   U16                  u16PDCMaxRx;                                       //!< Maximum bytes to receive in a single cycle
   U16                  u16PDCMaxTx;                                       //!< Maximum bytes to send in a single cycle
   void (*Isr) (void)__irq;                                                //!< Isr handler to call
};

//! <b>Internal data:</b> Circular buffer handling (pdc rx buffer array handling)
struct _SCircular
{
   UCHAR                ucCntEntries;                                      //!< Count of used circular buffer entries
   UCHAR                ucIdxRead;                                         //!< Index for next read access
   UCHAR                ucIdxWrite;                                        //!< Index for next write access
   UCHAR                ucSize;                                            //!< Number of available circular buffer entries
};

//! <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SUsartRW
{
   volatile UCHAR       ucPDCBufferRx[USART_PDCDEPTHRX][USART_PDCMAXRX];   //!< Pdc rx buffers
   UCHAR                ucPDCBufferTx[USART_PDCMAXTX];                     //!< Pdc tx buffer
   USART_MSGCOUNTERTYPE xMsgCounter[USART_MSGMAX+ 1];                      //!< Array with msg counters
   OS_ID                oiSemMsgNew;                                       //!< User supplied samaphore (ID) to give for each new msg
   ULONG                ulMsgMask;                                         //!< Mask to filter messages
   OS_SEM               SemMsg;                                            //!< Semaphore to lock msg data
   OS_SEM               SemRx;                                             //!< Semaphore to lock rx data
   OS_SEM               SemTx;                                             //!< Semaphore to lock tx data
   OS_SEM               SemTickTx;                                         //!< Semaphore re-triggerable tick cycle tx
   OS_TID               tidTskMsg;                                         //!< OS_ID of msg task
   OS_TID               tidTskRx;                                          //!< OS_ID of rx task
   OS_TID               tidTskTx;                                          //!< OS_ID of tx task
   struct _SCircular    SCircular;                                         //!< Circular buffer handling (pdc rx buffer array handling)
};

// Number of units located inside usart.h
struct _SUsartRW SUsartRW[USART_UNIT_MAX];
struct _SUsartTsk SUsartTsk[USART_UNIT_MAX];

#endif
