//!	data definitions of AT91SAM7X PCKO driver
/**
		\file		pckodata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PMC/PCKO/PCKODATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-10-23 initial version
// 2013-10-30 initial revision
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PMC_PCKO_PCKODATA_H
#define SYKAM_CPU_AT91_PMC_PCKO_PCKOMDATA_H 1


//! Hardware descriptor data (read only) for each unit
struct _SPckoRO
{
   AT91PS_PMC         pBase;               //!< Base address of peripheral to handle
};

//! Hardware descriptor data (read only) for each channel
/**
      <b>Manual reference(s):</b><br>
      <a name="_SPckoChannelRO">
      <a href="../../../MANUAL/AT91/AT91SAM7X/PMC/PCKO/AT91SAM7X_25.9.1_p191-193_PMC_SCxR.pdf" type="application/pdf">AT91SAM7X manual 25.9.11 pages 191-193: PMC_SCxR</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/PMC/PCKO/AT91SAM7X_25.9.11_p200_PMC_PCKx.pdf" type="application/pdf">AT91SAM7X manual 25.9.11 page 200: PMC_PCKx</a><br>
*/
struct _SPckoChannelRO
{
   U16               u16ChannelId;        //!< Bit-Id (!) of channel to handle (refer to <a href="#_SPckoChannelRO">AT91SAM7X manual 25.9.1 pages 191-193: PMC_SCxR</a>)
   U8                u8Pcr;               //!< Initial config for the (P)rogrammable (C)lock (R)egister (refer to <a href="#_SPckoChannelRO">AT91SAM7X manual 25.9.11 page 200: PMC_PCKx</a>)
   bool              bAutostart;          //!< Auto run flag for this channel with above config (<b>requires #PckoWait call to pend to channel running state!!!</b>)
};

//! <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SPckoRW
{
   bool              bInit;               //!< Init done flag
   bool              bIrq;                //!< Irq enable flag
};
   
//! <b>Internal data:</b> Runtime data (read/write) for each channel
struct _SPckoChannelRW
{
   OS_SEM            SemRun;              //!< Semaphore to indicate channel is operating yet
   bool              bRun;                //!< Flag to indicate channel is operating yet (set after sent semaphore, cleared by channel init, config, start and stop)
   bool              bInit;               //!< Init done flag
};

#endif
