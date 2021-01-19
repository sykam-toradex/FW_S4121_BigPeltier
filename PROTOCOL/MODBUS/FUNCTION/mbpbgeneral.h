//!	\internal \brief <b>INTERNAL</b> command handlers for general references (read/write 16 bit system memory data) of MBPB (ModBusPortBus)
/**
		\internal
      \file		mbpbgeneral.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBGENERAL.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-03-24 initial version
// 2013-04-25 updated internal doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBGENERAL_H
#define SYKAM_PROTOCOL_MODBUS_MBPBGENERAL_H 1


// Prototypes

//!   \internal \brief <b>Internal function:</b> write multiple system memory data (read/write 16 bit system memory data) command handler #MBPB_CODE_WRITE_GENERAL_REFERENCE
/**
      \param   *pucFrame                  pointer to <b>start of</b> internal raw (binary) telegram data
      \param   *usLen                     size of internal raw (binary) telegram data <b>in bytes</b>
      \param   *pvParam                   param structure #_SMBPBTskPara for task(s) <b>[*pvParam == struct _SMBPBTskPara *pSMBPBTskPara]</b>
      
      \return										#EMBPBException
*/
EMBPBException MBPBFuncWriteGeneralReference(UCHAR *pucFrame, USHORT *usLen, void *pvParam);

//!   \internal \brief <b>Internal function:</b> read multiple system memory data (read/write 16 bit system memory data) command handler #MBPB_CODE_READ_GENERAL_REFERENCE
/**
      \param   *pucFrame                  pointer to <b>start of</b> internal raw (binary) telegram data
      \param   *usLen                     size of internal raw (binary) telegram data <b>in bytes</b>
      \param   *pvParam                   param structure #_SMBPBTskPara for task(s) <b>[*pvParam == struct _SMBPBTskPara *pSMBPBTskPara]</b>
      
      \return										#EMBPBException
*/
EMBPBException MBPBFuncReadGeneralReference(UCHAR *pucFrame, USHORT *usLen, void *pvParam);

#endif
