//!	data definitions of AT91SAM7X EFC driver
/**
		\file		efcdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/EFC/EFCDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-02-27 initial version
// 2013-03-28 added SemPgBuff to _SEfcRW
// 2013-04-10 added bInit to _SEfcRW
// 2013-08-22 changed timeout from milliseconds to ticks
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_EFC_EFCDATA_H
#define SYKAM_CPU_AT91_EFC_EFCDATA_H 1


//! \internal \brief <b>Internal data:</b> Task parameter data
struct _SEfcTsk
{
   UCHAR ucUnit;                                         //!< Unit task is assigned to
};

//! Hardware descriptor data (read only) for each unit
struct _SEfcRO
{
   AT91PS_EFC           pEfcBase;                        //!< Base address of efc peripheral to handle
   USHORT               usBlockSize;                     //!< Size of a block (region) in bytes
   USHORT               usPageSize;                      //!< Size of a page in bytes
   UCHAR                ucBlockCount;                    //!< Count of available blocks (regions)
   UCHAR                ucPageCount;                     //!< Count of pages per block (regions)
   U16                  u16FWS;                          //!< Flash Wait State register (refer to AT91SAM7X manual 19.3.1. page 114)
   U8                   u8FMCNGPNVM;                     //!< Flash Microsecond Cycle Number register for GPNVM access (refer to AT91SAM7X manual 19.2.2. & 19.3.1. page 104 & 115)
   U8                   u8FMCNDATA;                      //!< Flash Microsecond Cycle Number register for DATA access (refer to AT91SAM7X manual 19.3.1. page 115)
   U16                  u16GPNVMSGPB;                    //!< General Purpose Non Volatile Memory bit(s) to set (Set General Purpose Bit, refer to AT91SAM7X manual 8.5.4. & 8.5.5. & 8.7. page 21-23)
   U16                  u16GPNVMCGPB;                    //!< General Purpose Non Volatile Memory bit(s) to clear (Clear General Purpose Bit, refer to AT91SAM7X manual 8.5.4. & 8.5.5. & 8.7. page 21-23)
   U32                  u32LOCKSLB;                      //!< Block (region) LOCK bit(s) to set (Set Lock Bit)
   U32                  u32LOCKCLB;                      //!< Block (region) LOCK bit(s) to clear (Clear Lock Bit)
   U8                   u8TskPriMsg;                     //!< Priority of message task
   U16                  u16Timeout;                      //!< Maximum cpu-load free resource pending time before any resource claim is canceled by the rtos <b>(in ticks)</b>
};

//! <b>Internal data:</b> Runtime data (read/write)
struct _SEfcRW
{
   EFC_MSGCOUNTERTYPE xMsgCounter[EFC_MSGMAX+ 1];        //!< Array with msg counters
   OS_ID                oiSemMsgNew;                     //!< Internal supplied samaphore (ID) to give for each new msg
   ULONG                ulMsgMask;                       //!< Mask to filter messages
   bool                 bInit;                           //!< Init done flag
   bool                 bIrq;                            //!< Irq enable flag
   OS_SEM               SemNew;                          //!< Semaphore for new msg
   OS_SEM               SemRdy;                          //!< Semaphore for ready state
   OS_SEM               SemMsg;                          //!< Semaphore to lock msg data
   OS_SEM               SemPgBuff;                       //!< Semaphore to lock hardware page buffer
   OS_TID               tidTskMsg;                       //!< TID of message task
};

struct _SEfcRW SEfcRW[EFC_UNIT_MAX];
struct _SEfcTsk SEfcTsk[EFC_UNIT_MAX];

#endif
