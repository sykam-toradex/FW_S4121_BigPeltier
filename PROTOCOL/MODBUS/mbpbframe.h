//!	frame defines of MBPB (ModBusPortBus)
/**
		\file		mbpbframe.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBFRAME.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-07-30 initial version
// 2009-08-19 re-named and re-grouped the defines
// 2011-10-27 added public doxygen documentation
// 2013-01-24 updated public doxygen documentation
// 2013-04-17 added doxygen example documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBFRAME_H
#define SYKAM_PROTOCOL_MODBUS_MBPBFRAME_H 1


//!   mbpb command/transfer limits due to the preprocessing at once before transmission
/**
      \example MBPB_CommandTransferLimits.txt
*/

// Constants which defines the format of a modbus frame. The example is
// shown for a Modbus RTU/ASCII frame. Note that the Modbus PDU is not
// dependent on the underlying transport.
//
// <------------------------ MODBUS SERIAL LINE PDU (1) ------------------->
//              <----------- MODBUS PDU (1') ---------------->
//  +-----------+---------------+----------------------------+-------------+
//  | Address   | Function Code | Data                       | CRC/LRC     |
//  +-----------+---------------+----------------------------+-------------+
//  |           |               |                                   |
// (2)        (3/2')           (3')                                (4)
//
// (1)  ... MB_SER_PDU_SIZE_MAX = 256
// (2)  ... MB_SER_PDU_ADDR_OFF = 0
// (3)  ... MB_SER_PDU_PDU_OFF  = 1
// (4)  ... MB_SER_PDU_SIZE_CRC = 2
//
// (1') ... MB_PDU_SIZE_MAX     = 253
// (2') ... MB_PDU_FUNC_OFF     = 0
// (3') ... MB_PDU_DATA_OFF     = 1
 
#define MBPB_PDU_SIZE_MAX          253   // Maximum size in bytes of a PDU
#define MBPB_PDU_SIZE_MIN          1     // Function code in bytes
#define MBPB_PDU_FUNC_OFF          0     // Offset in bytes of function code in PDU
#define MBPB_PDU_DATA_OFF          1     // Offset in bytes for response data in PDU

#define MBPB_SER_PDU_SIZE_MAX      256   //!<  Maximum size in bytes of a serial Modbus frame
#define MBPB_SER_PDU_ADDR_OFF      0     // Offset in bytes of slave address in Ser-PDU
#define MBPB_SER_PDU_PDU_OFF       1     // Offset in bytes of Modbus-PDU in Ser-PDU


// Default 'carriage return' code
#define MBPB_ASC_DEFAULT_CR        '\r'  //!< Default CR character for Modbus ASCII

// Default 'line feed' code
#define MBPB_ASC_DEFAULT_LF        '\n'  //!< Default LF character for Modbus ASCII


#define MBPB_ASC_PDU_SIZE_MIN      3     // Minimum size in bytes of a Modbus ASCII frame
#define MBPB_ASC_PDU_SIZE_LRC      1     // Size in bytes of LRC field in PDU

#define MBPB_RTU_PDU_SIZE_MIN      4     // Minimum size in bytes of a Modbus RTU (remote terminal unit) frame
#define MBPB_RTU_PDU_SIZE_CRC      2     // Size in bytes of CRC field in PDU

#endif
