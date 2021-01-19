//!	\internal \brief <b>INTERNAL</b> command handlers for input registers (read only 16 bit registers) of MBPB (ModBusPortBus)
/**
		\internal
      \file		mbpbinput.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBINPUT.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-27 initial version
// 2009-08-27 added void pointer to code function calls
// 2011-11-30 added internal doxygen documentation
// 2013-04-25 updated internal doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBINPUT_H
#define SYKAM_PROTOCOL_MODBUS_MBPBINPUT_H 1


// Prototypes

//!   \internal \brief <b>Internal function:</b> read input registers (read only 16 bit registerss) command handler #MBPB_CODE_READ_INPUT_REGISTER
/**
      \param   *pucFrame                  pointer to <b>start of</b> internal raw (binary) telegram data
      \param   *usLen                     size of internal raw (binary) telegram data <b>in bytes</b>
      \param   *pvParam                   param structure #_SMBPBTskPara for task(s) <b>[*pvParam == struct _SMBPBTskPara *pSMBPBTskPara]</b>
      
      \return										#EMBPBException
*/
EMBPBException MBPBFuncReadInputRegister(UCHAR *pucFrame, USHORT *usLen, void *pvParam);

#endif
