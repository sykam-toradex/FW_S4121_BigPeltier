//!	hardware descriptor(s) of AT91SAM7X PIO driver
/**
		\file		piodev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PIO/PIODEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-11-06 initial version
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-10-20 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PIO_PIODEV_H
#define SYKAM_CPU_AT91_PIO_PIODEV_H 1


//! Hardware descriptors
struct _SPioRO SPioRO[PIO_UNIT_MAX] =
{
   {
      /* pBase          */ AT91C_BASE_PIOA,
      /* u32PeriphId    */ AT91C_ID_PIOA,
      /* pSPioList      */ NULL,
      /* SPioListSize   */ 0,
      /* Isr            */ IsrPio0
   }
#if PIO_UNIT_MAX > 1
   ,
   {
      /* pBase          */ AT91C_BASE_PIOB,
      /* u32PeriphId    */ AT91C_ID_PIOB,
      /* pSPioList      */ NULL,
      /* SPioListSize   */ 0,
      /* Isr            */ IsrPio1
   }
#endif
};

#endif
