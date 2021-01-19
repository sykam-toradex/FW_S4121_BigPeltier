//!	defines and configuration of DACRRCS085 peripheral driver
/**
		\file		dacrrcs085.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/DAC/DACRRCS085/DACRRCS085.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-28 initial version
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-04-14 added optional define DACRRCS085_NUM_CHANNELS
// 2011-09-22 added forced data casting to ushort in DACRRCS085_SETOUTPUT macro
// 2011-11-03 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_DAC_DACRRCS085_DACRRCS085_H
#define SYKAM_EXT_SPI_DAC_DACRRCS085_DACRRCS085_H


// Macros

// Create dac telegram packet by specified channel, mode, out and bits
#define DACRRCS085_SETOUTPUT(channel, mode, out, bits) (((((USHORT)channel) & 0x03) << 14) | ((((USHORT)mode) & 0x03) << 12) | ((((USHORT)out) & ((1 << (bits))- 1)) << (12- (bits))))


// Defines

//! <b>Write mode:</b> write to specified register but do not update outputs (refer to DAC102S085.pdf figure 4 page 15)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/DAC/DACrrcS085/DACrrcS085_1.5_p14-15_fig4.pdf" type="application/pdf">DAC102S085.pdf figure 4 page 15: INPUT SHIFT REGISTER</a><br>
*/
#define DACRRCS085_MODE_WRITEONLY			(0)

//! <b>Write mode:</b> write to specified register and update outouts <b>[standard]</b> (refer to DAC102S085.pdf figure 4 page 15)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/DAC/DACrrcS085/DACrrcS085_1.5_p14-15_fig4.pdf" type="application/pdf">DAC102S085.pdf figure 4 page 15: INPUT SHIFT REGISTER</a><br>
*/
#define DACRRCS085_MODE_WRITEANDUPDATE		(1)

//! <b>Write mode:</b> write to all registers and update outputs (refer to DAC102S085.pdf figure 4 page 15)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/DAC/DACrrcS085/DACrrcS085_1.5_p14-15_fig4.pdf" type="application/pdf">DAC102S085.pdf figure 4 page 15: INPUT SHIFT REGISTER</a><br>
*/
#define DACRRCS085_MODE_WRITEANDUPDATEALL (2)

//! <b>Control option:</b> power down outputs (refer to DAC102S085.pdf figure 4 page 15)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/DAC/DACrrcS085/DACrrcS085_1.5_p14-15_fig4.pdf" type="application/pdf">DAC102S085.pdf figure 4 page 15: INPUT SHIFT REGISTER</a><br>
*/
#define DACRRCS085_MODE_POWERDOWN			(3)


//! <b>Overrideable configuration:</b> number of units supported simultaneously
#ifndef DACRRCS085_UNIT_MAX
   #define DACRRCS085_UNIT_MAX (1)
#endif

//! <b>Overrideable configuration:</b> number of channels per units supported simultaneously
#ifndef DACRRCS085_NUM_CHANNELS
   #define DACRRCS085_NUM_CHANNELS (2)
#endif

//! <b>Overrideable configuration:</b> maximum block length in bytes which can be transfered within a single chip select cycle
#ifndef DACRRCS085_BLOCKSIZEMAX
   #define DACRRCS085_BLOCKSIZEMAX (2)
#endif

#endif
