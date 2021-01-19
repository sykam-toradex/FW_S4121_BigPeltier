//!	external interface of DAC_GENERIC peripheral driver (abstraction layer for specific dacs)
/**
		\file		dac_generic_ext.h
      \todo    hs: documentation for the options of the DAC8552
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/DAC/DAC_GENERIC/DAC_GENERIC_EXT.H
// Purpose: PERIPHERAL SUPPORT
// Author:  rradtke, hschoettner
//----------------------------------------------------------------------------
// 2012-01-16 rr: initial version imported from "dacrrcs085ext.h"
// 2013-07-31 added doxygen documentation
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_DAC_GENERIC_DAC_GENERIC_EXT_H
#define SYKAM_EXT_SPI_DAC_GENERIC_DAC_GENERIC_EXT_H


// Defines

//! 1st generic channel
#define  DAC_GENERIC__CHANNEL_A     (0)

//! 2nd generic channel
#define  DAC_GENERIC__CHANNEL_B     (1)

//! 3rd generic channel
#define  DAC_GENERIC__CHANNEL_C     (2)

//! 4th generic channel
#define  DAC_GENERIC__CHANNEL_D     (3)


// Enums

//! Supported dac types
enum _EDacGeneric_DacTypes
{  
   DAC102S085,                                     //!< DAC[102]S085 (10 bits, 2 channels) (refer to dacrrcs085.h)
   DAC104S085,                                     //!< DAC[104]S085 (10 bits, 4 channels) (refer to dacrrcs085.h)
   DAC8552,                                        //!< DAC8552
   DAC_TYPE_COUNT
};

//! Options for DAC[RRC]S085 (RR= resulution, C= channel)
enum _EDacGeneric_ModesDacxxxS085
{
   DACxxxS085_MODE_WRITE_ONLY			      =  0,    //!< <b>Write mode:</b> write to specified register but do not update outputs (refer to DAC102S085.pdf figure 4 page 15)
   DACxxxS085_MODE_WRITE_AND_UPDATE		   =  1,    //!< <b>Write mode:</b> write to specified register and update outouts <b>[standard]</b> (refer to DAC102S085.pdf figure 4 page 15)
   DACxxxS085_MODE_WRITE_AND_UPDATE_ALL   =  2,    //!< <b>Write mode:</b> write to all registers and update outputs (refer to DAC102S085.pdf figure 4 page 15)
   DACxxxS085_MODE_POWER_DOWN			      =  3     //!< <b>Control option:</b> power down outputs (refer to DAC102S085.pdf figure 4 page 15)
};

// The mode enums define the layout of control bits without channel bits -  frame fitting is done in "dac_generic.c"
//! Options for DAC8552
enum _EDacGeneric_ModesDac8552
{
   DAC8552_MODE_WRITE_CHANNEL_X                                =  0x00,       //!< tbd
   DAC8552_MODE_WRITE_CHANNEL_X_LOAD_A		                     =  0x10,       //!< tbd
   DAC8552_MODE_WRITE_CHANNEL_X_LOAD_B                         =  0x20,       //!< tbd
   DAC8552_MODE_WRITE_CHANNEL_X_LOAD_AB                        =  0x30,       //!< tbd
   
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_1KOHM                     =  0x01,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_100KOHM                   =  0x02,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_HIGH_IMPEDANCE            =  0x03,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_1KOHM_LOAD_A              =  0x11,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_1KOHM_LOAD_B              =  0x21,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_1KOHM_LOAD_AB             =  0x31,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_100KOHM_LOAD_A            =  0x12,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_100KOHM_LOAD_B            =  0x22,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_100KOHM_LOAD_AB           =  0x32,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_HIGH_IMPEDANCE_LOAD_A     =  0x13,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_HIGH_IMPEDANCE_LOAD_B     =  0x23,       //!< tbd
   DAC8552_MODE_POWER_DOWN_CHANNEL_X_HIGH_IMPEDANCE_LOAD_AB    =  0x33        //!< tbd
};


// Structures

//! Hardware descriptor data
struct _SDacGeneric_GenericDacType
{
   UCHAR    ucNumDataBits;                                                    //!< Resolution in bits of device
   UCHAR    ucNumFrameBytes;                                                  //!< Number of bytes for the packet
   UCHAR    ucNumChannels;                                                    //!< Number of channels of device
   UCHAR    ucSpiUnit;                                                        //!< Unit of the spi controller connected to
   UCHAR    ucSpiChannel;                                                     //!< Channel of the spi controller connected to
};


// Externals used by application

//!   Configuration array of all supported dac types
extern   const struct   _SDacGeneric_GenericDacType cSDacGeneric_DacTypes[];


// Prototypes
//!   initialize a unit
/**
      \param	SemSpiNewMsgDac				OS_ID of <b>pre-initialized (to 0!)</b> semaphore which is assigned to the spi driver channel this peripheral is connected to

      \return										> 0 for success, == 0 for failure
*/
UCHAR  DacGenericInit      (OS_ID SemSpiNewMsgDac);

//!   set output channel
/**
      \param	ucDacType					   type of dac (refer to #_EDacGeneric_DacTypes)
      \param	ucChannel					   number of the channel of the specified unit
      \param	ucMode				         the output mode of the peripheral
      \param   ulData                     the value to output to the specified unit and channel
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

*/
USHORT DacGenericSetOutput (UCHAR ucDacType, UCHAR ucChannel, UCHAR ucMode, ULONG ulData, U16 u16Timeout);

//!   retrieve previous output value for specified unit and channel
/**
      \param	ucDacType					   type of dac (refer to #_EDacGeneric_DacTypes)
      \param	ucChannel					   number of the channel of the specified unit
      \param   *pulDataLast               storage for the previous output value of the specified unit and channel
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. NO previous output value set

      \note
               - setting ALL storages to NULL will return always == 0 == failure<br>
*/
USHORT DacGenericGetOutput (UCHAR ucDacType, UCHAR ucChannel, ULONG *pulDataLast, U16 u16Timeout);

#endif
