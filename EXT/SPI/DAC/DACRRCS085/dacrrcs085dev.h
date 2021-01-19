//!	hardware descriptor(s) of DACRRCS085 peripheral driver
/**
		\file		dacrrcs085dev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/DAC/DACRRCS085/DACRRCS085DEV.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-28 initial version
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-04-14 ucChannels now based on define DACRRCS085_NUM_CHANNELS
// 2011-11-03 added public doxygen documentation
// 2013-08-22 changed timeout from milliseconds to ticks (_OS_T_MS for compatibility)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_DAC_DACRRCS085_DACRRCS085DEV_H
#define SYKAM_EXT_SPI_DAC_DACRRCS085_DACRRCS085DEV_H


//! Hardware descriptors
struct _SDacrrcs085RO SDacrrcs085RO[DACRRCS085_UNIT_MAX] =
{
   {
      /* ucUnit             */ DACRRCS085_UNIT,
      /* ucChannel          */ DACRRCS085_CHANNEL,
      /* usBlockSizeMax     */ DACRRCS085_BLOCKSIZEMAX,
      /* ulSCBR             */ DACRRCS085_SCBR,
		/* ucBits             */ 10,
		/* ucChannels         */ DACRRCS085_NUM_CHANNELS,
      /* u16Timeout         */ _OS_T_MS(100, OS_TICK_RATE)
   }
#if DACRRCS085_UNIT_MAX > 1
   ,
   {
      /* ucUnit             */ DACRRCS085_2_UNIT,
      /* ucChannel          */ DACRRCS085_2_CHANNEL,
      /* usBlockSizeMax     */ DACRRCS085_BLOCKSIZEMAX,
      /* ulSCBR             */ DACRRCS085_SCBR,
		/* ucBits             */ 10,
		/* ucChannels         */ DACRRCS085_NUM_CHANNELS,
      /* u16Timeout         */ _OS_T_MS(100, OS_TICK_RATE)
   }
#endif
};

#endif
