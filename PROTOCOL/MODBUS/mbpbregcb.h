//!	<b>INTERNAL</b> interface of MBPB register call backs (ModBusPortBus)
/**
		\file		mbpbregcb.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBREGCB.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-20 initial version
// 2009-09-02 changed names inside prototypes
// 2010-04-21 added MBPBRegMsg function
// 2010-04-23 added MBPBRegAccess function
// 2010-04-30 added u16Timeout to MBPBRegAccess
// 2010-06-18 added MBPBRegAccessStruct based on a suggestion of rradtke
// 2010-06-18 changed param names of MBPBRegAccess to avoid further confusion
// 2011-10-27 changed param type of bRW of MBPBRegAccess from BOOL to bool
// 2011-11-30 added public doxygen documentation
// 2011-11-30 added internal doxygen documentation
// 2013-04-25 added new callback function MBPBRegGeneralReferenceCB
// 2013-05-02 updated internal doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBREGCB_H
#define SYKAM_PROTOCOL_MODBUS_MBPBREGCB_H 1


// Prototypes

//! \internal \brief <b>Internal function:</b>  create and send trigger message from specified (bVal8 == 8 bit value to allow compressed message) to MBPB application's mailbox
/**
      \param   *bAddrBase                 pointer to base address flag (<b>default == #FALSE == try relative address method</b>, #TRUE == force absolute address method)
      \param   bVal8                      compression flag (#TRUE == usVal contains 8 bit data only, #FALSE == usVal contains full 16 bit data)
      \param   ucType                     register type (#EMBPBRegType)
      \param   bRW                        access type (#EMBPBRegRW)
      \param   usAddr                     <b>1 based</b> address of register type
      \param   usAddrIdx                  offset (index) for consecutive register addresses
      \param   usVal                      register data value
      
      \return										> 0 for success, == 0 for failure
      
      \note
               - <b>*bAddrBase is set</b> to #TRUE when this function determines the absolute address method is necessary at time of this call (usAddr > 255)
*/
bool MBPBRegMsg(bool *bAddrBase, bool bVal8, UCHAR ucType, bool bRW, USHORT usAddr, USHORT usAddrIdx, USHORT usVal);

//!   access <b>local</b> registers (WITHOUT access triggering)
/**
      \param	ucType					      register type (#EMBPBRegType)
      \param   bRW                        access type (#EMBPBRegRW)
      \param   usAddress                  <b>1 based</b> address of register type
      \param   usCount                    count of consecutive registers
      \param   *pvData                    USHORT/UCHAR storage for data (read or write)
      \param   **pvIntern                 (optional, set NULL for default) storage for internal RAM address of the specified base register address (void pointer)
      \param	u16Timeout   	            maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - accessing local registers will NOT TRIGGER any filter events!!!
               - ModBus addresses are <b>1 based</b> for memory access <b>BUT 0 based</b> for external cooked (asc/rtu) telegram style!!!
               - <b>0</b> external cooked (asc/rtu) telegram style address will access address <b>1</b> of register memory <b>BUT 0 memory location</b> when MBPB_REG_xxx_ZEROBASED == 1)!!!
*/
bool MBPBRegAccess(UCHAR ucType, bool bRW, USHORT usAddress, USHORT usCount, void *pvData, void **pvIntern, U16 u16Timeout);

//!   access <b>local</b> registers (WITHOUT access triggering) using a structure argument
/**
      \param	*pSMBPBRegAccess				param struct _SMBPBRegAccess
      
      \return										> 0 for success, == 0 for failure

      \note
               - accessing local registers will NOT TRIGGER any filter events!!!
               - ModBus addresses are <b>1 based</b> for memory access <b>BUT 0 based</b> for external cooked (asc/rtu) telegram style!!!
               - <b>0</b> external cooked (asc/rtu) telegram style address will access address <b>1</b> of register memory <b>BUT 0 memory location</b> when MBPB_REG_xxx_ZEROBASED == 1)!!!
*/
bool MBPBRegAccessStruct(struct _SMBPBRegAccess *pSMBPBRegAccess);


// Prototypes of register access functions

//! \internal \brief <b>Internal function:</b> read usNRegs [read only 16 bit] input register(s) from usAddress to pucRegBuffer
/**
      \param	*pucRegBuffer				   pointer to internal raw (binary) telegram data <b>(already pointing to data by skipping header)</b>
      \param   usAddress                  <b>1 based</b> address of register type
      \param   usNRegs                    number of consecutive registers to access
      
      \return										#EMBPBErrorCode

      \note
               - ModBus addresses are <b>1 based</b> for memory access <b>BUT 0 based</b> for external cooked (asc/rtu) telegram style!!!
               - <b>0</b> external cooked (asc/rtu) telegram style address will access address <b>1</b> of register memory <b>BUT 0 memory location</b> when #MBPB_REG_INPUT_ZEROBASED == 1)!!!
*/
EMBPBErrorCode MBPBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs);

//! \internal \brief <b>Internal function:</b> read/write (EMode) usNRegs [16 bit] holding register(s) from usAddress to/from pucRegBuffer
/**
      \param	*pucRegBuffer				   pointer to internal raw (binary) telegram data <b>(already pointing to data by skipping header)</b>
      \param   usAddress                  <b>1 based</b> address of register type
      \param   usNRegs                    number of consecutive registers to access
      \param   EMode                      access mode to the specified register address range (#EMBPBRegisterMode)
      
      \return										#EMBPBErrorCode

      \note
               - ModBus addresses are <b>1 based</b> for memory access <b>BUT 0 based</b> for external cooked (asc/rtu) telegram style!!!
               - <b>0</b> external cooked (asc/rtu) telegram style address will access address <b>1</b> of register memory <b>BUT 0 memory location</b> when #MBPB_REG_HOLDING_ZEROBASED == 1)!!!
*/
EMBPBErrorCode MBPBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, EMBPBRegisterMode EMode);

//! \internal \brief <b>Internal function:</b> read/write (EMode) usNCoils coil(s) [read/write single bit(s)] from usAddress to/from pucRegBuffer
/**
      \param	*pucRegBuffer				   pointer to internal raw (binary) telegram data <b>(already pointing to data by skipping header)</b>
      \param   usAddress                  <b>1 based</b> address of register type
      \param   usNCoils                   number of consecutive registers to access
      \param   EMode                      access mode to the specified register address range (#EMBPBRegisterMode)
      
      \return										#EMBPBErrorCode

      \note
               - ModBus addresses are <b>1 based</b> for memory access <b>BUT 0 based</b> for external cooked (asc/rtu) telegram style!!!
               - <b>0</b> external cooked (asc/rtu) telegram style address will access address <b>1</b> of register memory <b>BUT 0 memory location</b> when #MBPB_REG_COILS_ZEROBASED == 1)!!!
*/
EMBPBErrorCode MBPBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils, EMBPBRegisterMode EMode);

//! \internal \brief <b>Internal function:</b> read usNDiscrete discrete(s) [read-only bit(s)] from usAddress to/from pucRegBuffer
/**
      \param	*pucRegBuffer				   pointer to internal raw (binary) telegram data <b>(already pointing to data by skipping header)</b>
      \param   usAddress                  <b>1 based</b> address of register type
      \param   usNDiscrete                number of consecutive registers to access
      
      \return										#EMBPBErrorCode

      \note
               - ModBus addresses are <b>1 based</b> for memory access <b>BUT 0 based</b> for external cooked (asc/rtu) telegram style!!!
               - <b>0</b> external cooked (asc/rtu) telegram style address will access address <b>1</b> of register memory <b>BUT 0 memory location</b> when #MBPB_REG_DISCRETE_ZEROBASED == 1)!!!
*/
EMBPBErrorCode MBPBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete);

//! \internal \brief <b>Internal function:</b> read/write (EMode) usNRegs [16 bit] system memory data reference(s) from usAddress to/from pucRegBuffer
/**
      \param	*pucRegBuffer				   pointer to internal raw (binary) telegram data <b>(already pointing to data by skipping header)</b>
      \param   u32Address                  <b>0 based</b> address of reference type (0x00000000 - 0xFFFFFFFF)
      \param   usNRegs                    number of consecutive references to access
      \param   EMode                      access mode to the specified reference address range (#EMBPBRegisterMode)
      
      \return										#EMBPBErrorCode

      \note
               - for this <b>special case</b> addresses are <b>0 based</b> for memory access <b>AND 0 based</b> for external cooked (asc/rtu) telegram style!!!
               - a existing <b>user validate function is required to allow any write request</b> for safety (refer to #MBPBInit)
*/
EMBPBErrorCode MBPBRegGeneralReferenceCB(UCHAR *pucRegBuffer, U32 u32Address, USHORT usNRegs, EMBPBRegisterMode EMode);

#endif
