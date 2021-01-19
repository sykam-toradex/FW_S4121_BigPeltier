//!	hardware descriptor(s) of FM25CLSSDEV peripheral driver
/**
		\file		fm25clssdev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/NVRAM/FM25CLSS/FM25CLSSDEV.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-04-07 initial version
// 2010-04-09 added bMultiTransfer to SFm25clssRO
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-10-07 changed style of defines for 2nd device
// 2010-10-19 rr: changed ucMultiStore to define setting of application
// 2010-11-23 moved data comments to fm25clssdata.h
// 2011-11-03 added public doxygen documentation
// 2013-08-22 changed timeout from milliseconds to ticks (_OS_T_MS for compatibility)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_NVRAM_FM25CLSS_FM25CLSSDEV_H
#define SYKAM_EXT_SPI_NVRAM_FM25CLSS_FM25CLSSDEV_H


//! Hardware descriptors
struct _SFm25clssRO SFm25clssRO[FM25CLSS_UNIT_MAX] =
{
   {
      /* ucUnit             */ FM25CLSS_UNIT,
      /* ucChannel          */ FM25CLSS_CHANNEL,
      /* ulBlockSizeMax     */ FM25CLSS_BLOCKSIZEMAX,
      /* ulSCBR             */ FM25CLSS_SCBR,
      /* usSizeBytes        */ 8192,
      /* ucMultiStore       */ FM25CLSS_NUM_MULTI_STORAGES,
      /* bMultiTransfer     */ FALSE,
      /* u16Timeout         */ _OS_T_MS(100, OS_TICK_RATE)
   }
#if FM25CLSS_UNIT_MAX > 1
   ,
   {
      /* ucUnit             */ FM25CLSS_2_UNIT,
      /* ucChannel          */ FM25CLSS_2_CHANNEL,
      /* ulBlockSizeMax     */ FM25CLSS_BLOCKSIZEMAX,
      /* ulSCBR             */ FM25CLSS_SCBR,
      /* usSizeBytes        */ 8192,
      /* ucMultiStore       */ FM25CLSS_2_NUM_MULTI_STORAGES,
      /* bMultiTransfer     */ FALSE,
      /* u16Timeout         */ _OS_T_MS(100, OS_TICK_RATE)
   }
#endif
};

#endif
