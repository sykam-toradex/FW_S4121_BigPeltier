//!	data definitions of DAC_GENERIC peripheral driver (abstraction layer for specific dacs)
/**
		\file		dac_generic_data.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/DAC/DAC_GENERIC/DAC_GENERIC_DATA.H
// Purpose: PERIPHERAL SUPPORT
// Author:  rradtke, hschoettner
//----------------------------------------------------------------------------
// 2012-01-16 rr: initial version imported from "dacrrcs085data.h"
// 2013-07-21 renamed DACxxxS085_SETOUTPUT to DACRRCS085_SETOUTPUT
// 2013-07-31 added doxygen documentation
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_DAC_GENERIC_DAC_GENERIC_DATA_H
#define SYKAM_EXT_SPI_DAC_GENERIC_DAC_GENERIC_DATA_H


// Dac type specific macros to set control bits

//! DAC[RRC]S085 (RR= resulution, C= channel) packet macro (refer to dacrrcs085.h)
#define  DACRRCS085_SETOUTPUT(channel, mode, data, num_data_bits) (((((USHORT)channel) & 0x03) << 14) | ((((USHORT)mode) & 0x03) << 12) | ((((USHORT)data) & ((1 << (num_data_bits))- 1)) << (12- (num_data_bits))))


// Structures

//! \internal \brief <b>Internal data:</b> Output cache for read-back feature of output-only value
struct _SDacGenericCache
{
   ULONG                ulValue[DAC_GENERIC__CHANNELS_MAX]; //!< Last output value
   bool                 bValid[DAC_GENERIC__CHANNELS_MAX];  //!< Last output valid flag
};

//! \internal \brief <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SDacGenericRW
{
   OS_ID                SemSpiNewMsgDac;                    //!< OS_ID of the semaphore to give for any new message
   OS_SEM               SemStart;                           //!< Semaphore to lock device
   struct _SDacGenericCache SDataCache;                     //!< Output cache for read-back feature of output-only value
};


#endif
