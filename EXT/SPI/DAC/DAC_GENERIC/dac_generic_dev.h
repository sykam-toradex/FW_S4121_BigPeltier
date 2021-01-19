//!	hardware descriptor(s) of DAC_GENERIC peripheral driver (abstraction layer for specific dacs)
/**
		\file		dac_generic_dev.h
      \todo    hs: contents of this file should be moved to AT91SAM7X.h (too application specific)!
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/DAC/DAC_GENERIC/DAC_GENERIC_DEV.H
// Purpose: PERIPHERAL SUPPORT
// Author:  rradtke, hschoettner
//----------------------------------------------------------------------------
// 2012-01-16 rr: initial version imported from "dacrrcs085dev.h"
// 2013-07-31 added doxygen documentation
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_DAC_GENERIC_DAC_GENERIC_DEV_H
#define SYKAM_EXT_SPI_DAC_GENERIC_DAC_GENERIC_DEV_H


// Hardware definitions

//! <b>Overrideable configuration:</b> spi unit dac is connected to
#ifndef AT91SAM7X__DAC102S085_SPI_UNIT
   #define AT91SAM7X__DAC102S085_SPI_UNIT       (0xFF)
#endif

//! <b>Overrideable configuration:</b> spi channel dac is connected to
#ifndef AT91SAM7X__DAC102S085_SPI_CHANNEL
   #define AT91SAM7X__DAC102S085_SPI_CHANNEL    (0xFF)
#endif


//! <b>Overrideable configuration:</b> spi unit dac is connected to
#ifndef AT91SAM7X__DAC104S085_SPI_UNIT
   #define AT91SAM7X__DAC104S085_SPI_UNIT       (0xFF)
#endif

//! <b>Overrideable configuration:</b> spi channel dac is connected to
#ifndef AT91SAM7X__DAC104S085_SPI_CHANNEL
   #define AT91SAM7X__DAC104S085_SPI_CHANNEL    (0xFF)
#endif


//! <b>Overrideable configuration:</b> spi unit dac is connected to
#ifndef AT91SAM7X__DAC8552_SPI_UNIT
   #define AT91SAM7X__DAC8552_SPI_UNIT          (0xFF)
#endif

//! <b>Overrideable configuration:</b> spi channel dac is connected to
#ifndef AT91SAM7X__DAC8552_SPI_CHANNEL
   #define AT91SAM7X__DAC8552_SPI_CHANNEL       (0xFF)
#endif

// extend here default SPI unit and channel for every type of "_EDacTypes"

#endif
