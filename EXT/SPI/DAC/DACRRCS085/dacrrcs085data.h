//!	data definitions of DACRRCS085 peripheral driver
/**
		\file		dacrrcs085data.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/DAC/DACRRCS085/DACRRCS085DATA.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-28 initial version
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-11-23 added/fixed data comments
// 2011-02-03 added semaphore SemStart to _SDacrrcs085RW
// 2011-03-29 rr: added usOutLast to _SDacrrcs085RW
// 2011-04-14 moved usOutLast to new struct _SDacrrcs085Cache
// 2011-04-14 added bOutLast valid flag to _SDacrrcs085Cache
// 2011-11-03 added public doxygen documentation
// 2011-11-24 added internal doxygen documentation
// 2013-08-22 changed timeout from milliseconds to ticks
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_DAC_DACRRCS085_DACRRCS085DATA_H
#define SYKAM_EXT_SPI_DAC_DACRRCS085_DACRRCS085DATA_H


//! Hardware descriptor data (read only) for each unit
struct _SDacrrcs085RO
{
   UCHAR                ucUnit;                             //!< Unit of the spi controller connected to
   UCHAR                ucChannel;                          //!< Channel of the spi controller connected to
   USHORT               usBlockSizeMax;                     //!< Maximum block length in bytes which can be transfered within a single chip select cycle
   ULONG                ulSCBR;                             //!< Serial clock baud rate
   UCHAR						ucBits;                             //!< Resolution in bits of device
   UCHAR						ucChannels;                         //!< Number of channels of device
   U16                  u16Timeout;                         //!< Maximum cpu-load free resource pending time before any resource claim is canceled by the rtos <b>(in ticks)</b>
};

//! \internal \brief <b>Internal data:</b> Output cache for read-back feature of output-only value
struct _SDacrrcs085Cache
{
   USHORT               usValue[DACRRCS085_NUM_CHANNELS];   //!< Last output value
   bool                 bValid[DACRRCS085_NUM_CHANNELS];    //!< Last output valid flag
};

//! \internal \brief <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SDacrrcs085RW
{
   OS_ID                oiSemMsgNew;                        //!< OS_ID of the semaphore to give for any new message
   OS_SEM               SemStart;                           //!< Semaphore to lock device
   struct _SDacrrcs085Cache SCacheOut;                      //!< Output cache for read-back feature of output-only value
};

#endif
