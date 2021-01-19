//!	\internal \brief <b>INTERNAL</b> command handlers for discretes (read only single bit registers) of MBPB (ModBusPortBus)
/**
		\internal
      \file		mbpbdisc.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBDISC.H
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

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBDISC_H
#define SYKAM_PROTOCOL_MODBUS_MBPBDISC_H 1


// Prototypes

//!   \internal \brief <b>Internal function:</b> read discretes (read only single bit registers) command handler #MBPB_CODE_READ_DISCRETE_INPUTS
/**
      \param   *pucFrame                  pointer to <b>start of</b> internal raw (binary) telegram data
      \param   *usLen                     size of internal raw (binary) telegram data <b>in bytes</b>
      \param   *pvParam                   param structure #_SMBPBTskPara for task(s) <b>[*pvParam == struct _SMBPBTskPara *pSMBPBTskPara]</b>
      
      \return										#EMBPBException
*/
EMBPBException MBPBFuncReadDiscreteInputs(UCHAR *pucFrame, USHORT *usLen, void *pvParam);

#endif
