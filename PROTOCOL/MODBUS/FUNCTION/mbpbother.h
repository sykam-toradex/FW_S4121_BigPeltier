//!	\internal \brief <b>INTERNAL</b> command handlers for other/miscellaneous of MBPB (ModBusPortBus)
/**
		\internal
      \file		mbpbother.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBOTHER.H
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

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBOTHER_H
#define SYKAM_PROTOCOL_MODBUS_MBPBOTHER_H 1

// Prototypes

//!   \internal \brief <b>Internal function:</b> report slave identification command handler #MBPB_CODE_OTHER_REPORT_SLAVEID
/**
      \param   *pucFrame                  pointer to <b>start of</b> internal raw (binary) telegram data
      \param   *usLen                     size of internal raw (binary) telegram data <b>in bytes</b>
      \param   *pvParam                   param structure #_SMBPBTskPara for task(s) <b>[*pvParam == struct _SMBPBTskPara *pSMBPBTskPara]</b>
      
      \return										#EMBPBException
      
      \see                                MBPBProductDescSet
*/
EMBPBException MBPBFuncReportSlaveID(UCHAR *pucFrame, USHORT *usLen, void *pvParam);

#endif
