//!	hardware descriptor(s) of AT91SAM7X PCKO driver
/**
		\file		pckodev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PMC/PCKO/PCKODEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-10-23 initial version
// 2013-10-30 initial revision
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PMC_PCKO_PCKODEV_H
#define SYKAM_CPU_AT91_PMC_PCKO_PCKODEV_H 1


//! Hardware descriptors for unit(s)
struct _SPckoRO SPckoRO[PCKO_UNIT_MAX] =
{
   {
      /* pBase          */ AT91C_BASE_PMC,
   }
#if PCKO_UNIT_MAX > 1
   ,
   {
      /* pBase          */ AT91C_BASE_PMC,
   }
#endif
};

//! Hardware descriptors for channel(s)
struct _SPckoChannelRO SPckoChannelRO[PCKO_UNIT_MAX][PCKO_CHANNEL_MAX] = 
{
   {
      {
         /* u32ChannelId   */ AT91C_PMC_PCK1,
         /* u8PCR          */ AT91C_PMC_CSS_MAIN_CLK | AT91C_PMC_PRES_CLK_4,
         /* bAutostart     */ FALSE,
      }
#if PCKO_CHANNEL_MAX > 1
      ,
      {
         /* u32ChannelId   */ AT91C_PMC_PCK3,
         /* u8PCR          */ AT91C_PMC_CSS_MAIN_CLK | AT91C_PMC_PRES_CLK_4,
         /* bAutostart     */ FALSE,
      }
#endif
#if PCKO_CHANNEL_MAX > 2
      ,
      {
         /* u32ChannelId   */ AT91C_PMC_PCK2,
         /* u8PCR          */ AT91C_PMC_CSS_MAIN_CLK | AT91C_PMC_PRES_CLK_4,
         /* bAutostart     */ FALSE,
      }
#endif
#if PCKO_CHANNEL_MAX > 3
      ,
      {
         /* u32ChannelId   */ AT91C_PMC_PCK0,
         /* u8PCR          */ AT91C_PMC_CSS_MAIN_CLK | AT91C_PMC_PRES_CLK_4,
         /* bAutostart     */ FALSE,
      }
#endif
   }
#if PCKO_UNIT_MAX > 1
   ,
   {
      {
         /* u32ChannelId   */ AT91C_PMC_PCK1,
         /* u8PCR          */ AT91C_PMC_CSS_MAIN_CLK | AT91C_PMC_PRES_CLK_4,
         /* bAutostart     */ FALSE,
      }
#if PCKO_CHANNEL_MAX > 1
      ,
      {
         /* u32ChannelId   */ AT91C_PMC_PCK3,
         /* u8PCR          */ AT91C_PMC_CSS_MAIN_CLK | AT91C_PMC_PRES_CLK_4,
         /* bAutostart     */ FALSE,
      }
#if PCKO_CHANNEL_MAX > 2
      ,
      {
         /* u32ChannelId   */ AT91C_PMC_PCK2,
         /* u8PCR          */ AT91C_PMC_CSS_MAIN_CLK | AT91C_PMC_PRES_CLK_4,
         /* bAutostart     */ FALSE,
      }
#endif
#if PCKO_CHANNEL_MAX > 3
      ,
      {
         /* u32ChannelId   */ AT91C_PMC_PCK0,
         /* u8PCR          */ AT91C_PMC_CSS_MAIN_CLK | AT91C_PMC_PRES_CLK_4,
         /* bAutostart     */ FALSE,
      }
#endif
   }
#endif
#endif
};

// Number of channels located inside pcko.h
struct _SPckoRO SPckoRO[PCKO_UNIT_MAX];
struct _SPckoRW SPckoRW[PCKO_UNIT_MAX];

struct _SPckoChannelRO SPckoChannelRO[PCKO_UNIT_MAX][PCKO_CHANNEL_MAX];
struct _SPckoChannelRW SPckoChannelRW[PCKO_UNIT_MAX][PCKO_CHANNEL_MAX];

#endif
