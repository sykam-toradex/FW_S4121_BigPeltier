//!	data definitions of AT91SAM7X RTT driver
/**
		\file		rttdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/RTT/RTTDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-10-13 initial version
// 2010-10-14 changed structures
// 2010-11-23 added data comment
// 2011-03-23 removed usPres, usAdjTicks & usCycleMS of _SRttRO
// 2011-03-23 removed u16Pres of _SRttRW
// 2011-03-23 added usSCK to _SRttRO
// 2011-03-23 added u32SCK to _SRttRW
// 2011-03-24 removed usSCK of _SRttRO (obsolete)
// 2011-03-24 removed sDiff of _SRttRW (obsolete)
// 2011-04-14 added u32SCKValid to _SRttRW
// 2011-10-20 added public doxygen documentation
// 2011-11-24 added internal doxygen documentation
// 2013-01-23 renamed u32SCK & u32SCKValid to u32SLCK & u32SLCKValid (SLow ClocK)
// 2013-01-30 renamed boolean u32SLCKValid to bSLCKValid in _SRttRW
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_RTT_RTTDATA_H
#define SYKAM_CPU_AT91_RTT_RTTDATA_H 1


//! \internal \brief <b>Internal data:</b> Task parameter data
struct _SRttTsk
{
   UCHAR ucUnit;                                         //!< Unit task is assigned to
};

//! Hardware descriptor data (read only) for each unit
struct _SRttRO
{
   AT91PS_RTTC          pBase;                           //!< Base address
   U32                  u32IrqMask;                      //!< Mask for irqs to count
   USHORT               usAdjCycleDiv;                   //!< Duration of adjustment cycle in fractions of a second <b>('1/usAdjCycleDiv' s)</b>
   U8                   u8TskPriMsg;                     //!< Priority of message task
};

//! <b>Internal data:</b> Runtime data (read/write)
struct _SRttRW
{
   RTT_MSGCOUNTERTYPE   xMsgCounter[RTT_MSGMAX];         //!< Array of message counters
   ULONG                ulOffset;                        //!< Offset for zero-up / read-only counter value
   ULONG                ulIncDiv;                        //!< Clock increment irq divider for msg
   OS_ID                oiSemMsgNew;                     //!< User supplied samaphore (ID) to give for each new msg
   ULONG                ulMsgMask;                       //!< User msg mask to count msgs and to trigger given semaphore
   U32                  u32SLCK;                         //!< Determined value of SLCK (RTT base clock)
   bool                 bSLCKValid;                      //!< Valid flag for determined value of SLCK (RTT base clock)
   bool                 bIrq;                            //!< Irq enable flag
   OS_SEM               SemRW;                           //!< Semaphore to protect SRttRW
   OS_SEM               SemMsg;                          //!< Semaphore to protect SRttRW.xMsgCounter[]
   OS_TID               tidTskMsg;                       //!< TID of msg task
};

struct _SRttRW SRttRW[RTT_UNIT_MAX];
struct _SRttTsk SRttTsk[RTT_UNIT_MAX];

#endif
