//!	external interface of DACRRCS085 peripheral driver
/**
		\file		dacrrcs085ext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/DAC/DACRRCS085/DACRRCS085EXT.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-28 initial version
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-02-03 added u16Timeout param to function Dacrrcs085SetOutput
// 2011-03-29 rr: added function Dacrrcs085GetOutput
// 2011-04-14 added u16Timeout param to function Dacrrcs085GetOutput
// 2011-11-03 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_DAC_DACRRCS085_DACRRCS085EXT_H
#define SYKAM_EXT_SPI_DAC_DACRRCS085_DACRRCS085EXT_H


// Prototypes

//!   initialize a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of dacrrcs085dev.h)
      \param	ucChannel					   (unused, set 0) number of the sub-channel for units tree(s)
      \param	oiMsgNewSem				      OS_ID of <b>pre-initialized (to 0!)</b> semaphore which is assigned to the spi driver channel this peripheral is connected to

      \return										> 0 for success, == 0 for failure
*/
UCHAR  Dacrrcs085Init     (UCHAR ucUnit, UCHAR ucChannel, OS_ID oiMsgNewSem);

//!   set output channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of dacrrcs085dev.h)
      \param	ucChannel					   number of the channel of the specified unit
      \param	ucMode				         the output mode of the peripheral (refer to dacrrcs085.h)
      \param   usOut                      the value to output to the specified unit and channel (refer to <b>ucBits</b> of #_SDacrrcs085RO for maximum bit width)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - the standard output mode is #DACRRCS085_MODE_WRITEANDUPDATE (refer to dacrrcs085.h)<br>
*/
USHORT Dacrrcs085SetOutput(UCHAR ucUnit, UCHAR ucChannel, UCHAR ucMode, USHORT usOut, U16 u16Timeout);

//!   retrieve previous output value for specified unit and channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of dacrrcs085dev.h)
      \param	ucChannel					   number of the channel of the specified unit
      \param   *pusOutLast                storage for the previous output value of the specified unit and channel
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. NO previous output value set

      \note
               - setting ALL storages to NULL will return always == 0 == failure<br>
*/
USHORT Dacrrcs085GetOutput(UCHAR ucUnit, UCHAR ucChannel, USHORT *pusOutLast, U16 u16Timeout);

#endif
