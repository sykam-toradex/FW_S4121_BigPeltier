//!	defines and configuration of FM25CLSS peripheral driver
/**
		\file		fm25clss.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/NVRAM/FM25CLSS/FM25CLSS.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-04-07 initial version
// 2010-04-09 added FM25CLSS_TAG_CTRL, FM25CLSS_TAG_SR and FM25CLSS_TAG_MEM
// 2010-04-09 changed FM25CLSS_BLOCKSIZEMAX to 0x1FFF+ FM25CLSS_SIZE_HEADERMEM
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-10-06 added FM25CLSS_SPI_FIX define for easier spi debugging
// 2011-11-03 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_NVRAM_FM25CLSS_FM25CLSS_H
#define SYKAM_EXT_SPI_NVRAM_FM25CLSS_FM25CLSS_H


// Defines


// FM25CLSS command codes [refer to FM25CL64.PDF page 5-6 for further details]

//! <b>Command code:</b> WRite ENable
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 5: Op-Code</a><br>
*/
#define FM25CLSS_CODE_WREN    (6)

//! <b>Command code:</b> WRite DIsable
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 5: Op-Code</a><br>
*/#define FM25CLSS_CODE_WRDI    (4)

// <b>Command code:</b> ReaD Status Register
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 5: Op-Code</a><br>
*/
#define FM25CLSS_CODE_RDSR    (5)

// <b>Command code:</b> WRite Status Register
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 5: Op-Code</a><br>
*/
#define FM25CLSS_CODE_WRSR    (1)

// <b>Command code:</b> READ Memory Data
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 5: Op-Code</a><br>
*/
#define FM25CLSS_CODE_READ    (3)

// <b>Command code:</b> WRITE Memory Data
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 5: Op-Code</a><br>
*/
#define FM25CLSS_CODE_WRITE   (2)


// FM25CLSS StatusRegister bits [refer to FM25CL64.PDF page 6-7 for further details]

//! <b>Status register bit:</b> Write Protect ENable (controls the effect of the hardware /WP pin)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 6: Status Register & Write Protection</a><br>
*/
#define FM25CLSS_SR_WPEN      (128)

//! <b>Status register bit:</b> Memory block write protection bit 1
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 6: Status Register & Write Protection</a><br>
*/
#define FM25CLSS_SR_BP1       (8)

//! <b>Status register bit:</b> Memory block write protection bit 0
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 6: Status Register & Write Protection</a><br>
*/
#define FM25CLSS_SR_BP0       (4)

//! <b>Status register bit:</b> Write Enable Latch (flag indicates the state of the Write EnableLatch)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 6: Status Register & Write Protection</a><br>
*/
#define FM25CLSS_SR_WEL       (2)


//! <b>Header size:</b> Status Register access
#define FM25CLSS_SIZE_HEADERSR   (2)

//! <b>Header size:</b> MEMory access
#define FM25CLSS_SIZE_HEADERMEM  (3)


// Transfer tag definitions for spi
#define FM25CLSS_TAG_CTRL     (8)
#define FM25CLSS_TAG_SR       (16)
#define FM25CLSS_TAG_MEM      (128)

//! <b>Overrideable configuration:</b> number of units supported simultaneously
#ifndef FM25CLSS_UNIT_MAX
   #define FM25CLSS_UNIT_MAX (1)
#endif

//! <b>Overrideable configuration:</b> maximum block length in bytes which can be transfered within a single chip select cycle <b>[usually size of chip ram + size of memory header]</b>
#ifndef FM25CLSS_BLOCKSIZEMAX
   #define FM25CLSS_BLOCKSIZEMAX (0x1FFF+ FM25CLSS_SIZE_HEADERMEM)
#endif

//! <b>Overrideable configuration:</b> reduce transfer block length (reserve specified number of bytes in spi's buffer) for debugging
#ifndef FM25CLSS_SPI_FIX
   #define FM25CLSS_SPI_FIX (0)
#endif

#endif
