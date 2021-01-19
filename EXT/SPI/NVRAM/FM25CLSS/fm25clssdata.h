//!	data definitions of FM25CLSSDATA peripheral driver
/**
		\file		fm25clssdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/NVRAM/FM25CLSS/FM25CLSSDATA.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-04-07 initial version
// 2010-04-09 added bMultiTransfer to SFm25clssRO
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-11-23 added data comments for _SFm25clssRO and _SFm25clssRW
// 2011-02-03 added semaphore SemStart to _SFm25clssRW
// 2011-11-03 added public doxygen documentation
// 2011-11-24 added internal doxygen documentation
// 2013-08-22 changed timeout from milliseconds to ticks
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_NVRAM_FM25CLSS_FM25CLSSDATA_H
#define SYKAM_EXT_SPI_NVRAM_FM25CLSS_FM25CLSSDATA_H


//! Hardware descriptor data (read only) for each unit
struct _SFm25clssRO
{
   UCHAR                ucUnit;           //!< Unit of the spi controller connected to
   UCHAR                ucChannel;        //!< Channel of the spi controller connected to
   ULONG                ulBlockSizeMax;   //!< Maximum block length in bytes which can be transfered within a single chip select cycle
   ULONG                ulSCBR;           //!< Serial data clock speed
   USHORT               usSizeBytes;      //!< Total <b>real</b> chip ram size in bytes
   UCHAR                ucMultiStore;     //!< Number of copies (logical blocks) to save data automatically multiple times for further validation process if some storage cells are defect during #Fm25clssReadSafe <b>[will reduce useable memory size by 1/ucMultiStore]</b>
   bool                 bMultiTransfer;   //!< When TRUE the read function places all multiple copies (blocks) sequentially <b>(without the real gaps that may exist)</b> at pvData, which may be easily compared by application (for majority checks) <b>[you need to multiply the size of your read buffer for this feature by ucMultiStore!]</b>
   U16                  u16Timeout;       //!< Maximum cpu-load free resource pending time before any resource claim is canceled by the rtos <b>(in ticks)</b>
};

//! \internal \brief <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SFm25clssRW
{
   OS_ID                oiSemMsgNew;      //!< OS_ID of the semaphore to give for any new message
   OS_SEM               SemStart;         //!< Semaphore to lock device
};

#endif
