//!	defines and configuration of DAC_GENERIC peripheral driver (abstraction layer for specific dacs)
/**
		\file		dac_generic.h
      \todo    hs: contents of this file should be moved to AT91SAM7X.h (too application specific)!
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/DAC/DAC_GENERIC/DAC_GENERIC.H
// Purpose: PERIPHERAL SUPPORT
// Author:  rradtke, hschoettner
//----------------------------------------------------------------------------
// 2012-01-16 rr: initial version imported from "dacrrcs085.h"
// 2013-07-31 added doxygen documentation
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_DAC_GENERIC_DAC_GENERIC_H
#define SYKAM_EXT_SPI_DAC_GENERIC_DAC_GENERIC_H


//! <b>Overrideable configuration:</b> max spi unit count
#ifndef DAC_GENERIC__SPI_UNITS_MAX
   #define DAC_GENERIC__SPI_UNITS_MAX        (2)
#endif

//! <b>Overrideable configuration:</b> max spi channel count
#ifndef DAC_GENERIC__SPI_CHANNELS_MAX
   #define DAC_GENERIC__SPI_CHANNELS_MAX     (8)
#endif

//! <b>Overrideable configuration:</b> max dac channel count
#ifndef DAC_GENERIC__CHANNELS_MAX
   #define DAC_GENERIC__CHANNELS_MAX         (4)
#endif

//! <b>Overrideable configuration:</b> max packet byte count
#ifndef DAC_GENERIC__FRAME_BYTES_MAX
   #define DAC_GENERIC__FRAME_BYTES_MAX      (4)
#endif

//! <b>Overrideable configuration:</b> max resolution bits count
#ifndef DAC_GENERIC__DATA_BITS_MAX
   #define DAC_GENERIC__DATA_BITS_MAX        (24)
#endif


#endif
