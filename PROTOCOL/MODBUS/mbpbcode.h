//!	command code defines of MBPB (ModBusPortBus)
/**
		\file		mbpbcode.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBCODE.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-19 initial version
// 2011-10-27 added public doxygen documentation
// 2013-04-24 added new commands MBPB_CODE_READ/WRITE_GENERAL_REFERENCE
// 2013-05-02 updated public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBCODE_H
#define SYKAM_PROTOCOL_MODBUS_MBPBCODE_H 1


// The following defines represent the official function codes for modbus (dezimal)
// See "Modbus Application Protocol Specification V1.1b.pdf"

//!   <b>Command code:</b> no command (required for syncronous ports)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_5.1_p11.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 5.1 page 11</a><br>
*/
#define MBPB_CODE_NONE                          0

//!   <b>Command code:</b> read multiple coils (single read/write bits) (refer to MODBUS Application Protocol Specification 1.1b at 6.1 page 12)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.1_p12-13_01.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.1 page 12: 01 (0x01)</a><br>
*/
#define MBPB_CODE_READ_COILS                    1

//!   <b>Command code:</b> read discrete inputs (single read ONLY bits) (refer to MODBUS Application Protocol Specification 1.1b at 6.2 page 13)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.2_p13-14_02.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.2 page 13: 02 (0x02)</a><br>
*/
#define MBPB_CODE_READ_DISCRETE_INPUTS          2

//!   <b>Command code:</b> write single coil (single read/write bit) (refer to MODBUS Application Protocol Specification 1.1b at 6.5 page 17)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.5_p17-18_05.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.5 page 17: 05 (0x05)</a><br>
*/
#define MBPB_CODE_WRITE_SINGLE_COIL             5

//!   <b>Command code:</b> write multiple coils (single read/write bits) (refer to MODBUS Application Protocol Specification 1.1b at 6.11 page 29)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.11_p29-30_15.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.11 page 29: 15 (0x0F)</a><br>
*/
#define MBPB_CODE_WRITE_MULTIPLE_COILS          15

//!   <b>Command code:</b> read single/multiple holding (read/write 16 bit registers) register(s) (refer to MODBUS Application Protocol Specification 1.1b at 6.3 page 15)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.3_p15-16_03.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.3 page 15: 03 (0x03)</a><br>
*/
#define MBPB_CODE_READ_HOLDING_REGISTER         3

//!   <b>Command code:</b> read single/multiple input (read ONLY 16 bit registers) register(s) (refer to MODBUS Application Protocol Specification 1.1b at 6.4 page 16)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.4_p16-17_04.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.4 page 16: 04 (0x04)</a><br>
*/
#define MBPB_CODE_READ_INPUT_REGISTER           4

//!   <b>Command code:</b> write single holding (read/write 16 bit registers) register (refer to MODBUS Application Protocol Specification 1.1b at 6.6 page 19)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.6_p19-20_06.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.6 page 19: 06 (0x06)</a><br>
*/
#define MBPB_CODE_WRITE_REGISTER                6

//!   <b>Command code:</b> write multiple holding (read/write 16 bit registers) register(s) (refer to MODBUS Application Protocol Specification 1.1b at 6.12 page 30)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.12_p30-31_16.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.12 page 30: 16 (0x10)</a><br>
*/
#define MBPB_CODE_WRITE_MULTIPLE_REGISTERS      16

//!   <b>Command code:</b> read AND write multiple holding (read/write 16 bit registers) register(s) (refer to MODBUS Application Protocol Specification 1.1b at 6.17 page 38)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.17_p38-40_23.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.17 page 38: 23 (0x17)</a><br>
*/
#define MBPB_CODE_READWRITE_MULTIPLE_REGISTERS  23

//!   <b>Command code:</b> read multiple system memory data (read/write 16 bit system memory data) register(s) (refer to MODBUS Application Protocol Specification 1.1b at 6.14 page 32)
/**
      \note
               - the newer name for this command is 'write file record', but the older one describes the implemented function better
               - <b>file number hi/lo</b> and <b>starting address</b> (newer name 'record number') <b>hi/lo </b> of the original command are used here to <b>build a 32 bit address</b>:<br>
                 (MSB) file number hi, file number lo, starting address hi, starting address lo (LSB)
               - <b>CAUTION:</b> in this <b>special case</b> addresses are <b>0 based</b> for memory access <b>AND 0 based</b> for external cooked (asc/rtu) telegram style!!!
               - <b>only a single sub-request per telegram is supported</b> (support for multi sub-requests is omitted yet!)
               - a existing <b>user validate function is required to allow any write request</b> for safety (refer to #MBPBInit)

      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.14_p32-34_20.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.14 page 32: 20 (0x14)</a><br>
*/
#define MBPB_CODE_READ_GENERAL_REFERENCE        20

//!   <b>Command code:</b> write multiple system memory data (read/write 16 bit system memory data) register(s) (refer to MODBUS Application Protocol Specification 1.1b at 6.15 page 34)
/**
      \note
               - the newer name for this command is 'write file record', but the older one describes the implemented function better
               - <b>file number hi/lo</b> and <b>starting address</b> (newer name 'record number') <b>hi/lo </b> of the original command are used here to <b>build a 32 bit address</b>:<br>
                 (MSB) file number hi, file number lo, starting address hi, starting address lo (LSB)
               - <b>CAUTION:</b> in this <b>special case</b> addresses are <b>0 based</b> for memory access <b>AND 0 based</b> for external cooked (asc/rtu) telegram style!!!
               - <b>only a single sub-request per telegram is supported</b> (support for multi sub-requests is omitted yet!)
               - a existing <b>user validate function is required to allow any write request</b> for safety (refer to #MBPBInit)

      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.15_p34-36_21.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.15 page 34: 21 (0x15)</a><br>
*/
#define MBPB_CODE_WRITE_GENERAL_REFERENCE       21

//!   <b>Unsupported command code:</b> read execption status (8 bits) (refer to MODBUS Application Protocol Specification 1.1b at 6.7 page 20)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.7_p20-21_07.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.7 page 20: 07 (0x07)</a><br>
*/
#define MBPB_CODE_DIAG_READ_EXCEPTION           7

//!   <b>Unsupported command code:</b> communication diagnostics (refer to MODBUS Application Protocol Specification 1.1b at 6.8 page 21)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.8_p21-25_08.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.8 page 21: 08 (0x08)</a><br>
*/
#define MBPB_CODE_DIAG_DIAGNOSTIC               8

//!   <b>Unsupported command code:</b> get communication event counter (refer to MODBUS Application Protocol Specification 1.1b at 6.9 page 25)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.9_p25-26_11.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.9 page 25: 11 (0x0B)</a><br>
*/
#define MBPB_CODE_DIAG_GET_COM_EVENT_CNT        11

//!   <b>Unsupported command code:</b> get communication event log (refer to MODBUS Application Protocol Specification 1.1b at 6.10 page 26)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.10_p26-29_12.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.10 page 26: 12 (0x0C)</a><br>
*/
#define MBPB_CODE_DIAG_GET_COM_EVENT_LOG        12

//!   <b>Command code:</b> report slave identification (for enumeration of slaves) (refer to MODBUS Application Protocol Specification 1.1b at 6.13 page 32)
/**
      <b>Custom reply:</b><br>
      \include MBPBFuncReportSlaveID.txt
      
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_6.13_p32_17.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 6.13 page 32: 17 (0x11)</a><br>
*/
#define MBPB_CODE_OTHER_REPORT_SLAVEID          17

//!   <b>Error reply:</b> unsupported command
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/PROTOCOL/MODBUS/MODBUS_Application_Protocol_Specification1.1b_5.1_p11.pdf" type="application/pdf">MODBUS Application Protocol Specification 1.1b at 5.1 page 11</a><br>
*/
#define MBPB_CODE_ERROR                         128

#endif
