//!	external interface of MBPB (ModBusPortBus)
/**
		\file		mbpbext.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBEXT.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-07-29 initial version
// 2009-08-26 renamed demo to init
// 2010-04-22 added MBPBTrgWaitDecode
// 2010-04-23 added MBPBTrgRebuild and MBPBTrgFilterSet
// 2010-04-23 added MBPBTrgFilterStateSet and MBPBTrgFilterStateGet
// 2010-04-23 added MBPBRegAccess
// 2010-04-30 added MBPBProductSet
// 2010-04-23 added u16Timeout to MBPBRegAccess
// 2010-04-30 added MBPBProduct... DescSet, StateSet and AddrSet
// 2010-05-05 added *pSCfgDriver to MBPBInit
// 2010-06-16 imported and fixed doxygen section of rradtke
// 2010-06-18 changed param names of MBPBRegAccess to avoid further confusion
// 2010-06-18 added MBPBRegAccessStruct based on a suggestion of rradtke
// 2010-10-07 changed return data type of MBPBInit
// 2011-07-14 added user validate function pointer argument to MBPBInit prototype
// 2011-08-10 added prototype for MBPBTmrHandler
// 2011-08-17 added prototype for MBPBTrgFilterSetEx
// 2011-08-31 added u16Timeout para to MBPBInit prototype
// 2011-10-27 added public doxygen documentation
// 2011-10-27 changed param type of bRW of MBPBRegAccess from BOOL to bool
// 2011-10-27 changed type of bRx and bTx of MBPBMsgCtrl from BOOL to bool
// 2011-10-27 changed type of bActive of MBPBTrgFilterSet(Ex) from BOOL to bool
// 2011-10-27 changed type of bActive of MBPBTrgFilterStateSet from BOOL to bool
// 2011-12-01 added example tutorials
// 2011-12-16 changed MBPBProductDescSet prototype (ucClass -> ulClass)
// 2013-04-25 changed address argument of validate function pointer to U32
// 2013-04-25 updated internal doxygen documentation
// 2013-05-02 updated public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBEXT_H
#define SYKAM_PROTOCOL_MODBUS_MBPBEXT_H 1


// Prototypes

//!   feature for write access to off memory registers
/**
      \example MBPB_TriggerOnWriteToNonExistingRegisters.txt
*/

//!   limited routing (forwarding / back warding) and translation features
/**
      \example MBPB_ForwardingBackwardingLimitedRouting.txt
*/

//!   initialize the ModBusPortBus system
/**
      \param	*pSCfgDriver					array with size #MBPB_PORT_COUNT of port driver options structure <b>struct _SMBPBCfgDriver SMBPBCfgDriver[MBPB_PORT_COUNT]</b>
      \param	*Validate                  custom validation function to verify incoming register write accesses for holding & general register type <b>(required for general write access)</b>
      \param	u16Timeout   	            maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure

      \note
               - per default all ports listed in mbpbdev.h are reserved for ModBusPortBus (to disable ports, set their bDisabled entry of _SMBPBCfgDriver to #TRUE)
               - usage of ports already used by ModBusPortBus is dangerous and not stable for production use (because all drivers support single client only) <b>but may work for debug purpose</b>
               - a existing <b>user validate function is required to allow any write request to general register type</b> for safety
               
      <b>Example code:</b> custom validation function to verify incoming register write accesses for holding register type<br>
      \include ModbusValidate.c
*/
bool           MBPBInit(struct _SMBPBCfgDriver *pSCfgDriver, bool (*Validate) (UCHAR ucType, U32 u32Address, USHORT *pusValue), U16 u16Timeout);

//!   set device/product descriptor (class and indentification string)
/**
      \param	ulClass					      device/product class
      \param	pvDesc                     device/product indentification string
      \param   ucDescLen                  length of device/product indentification string
      \param	u16Timeout   	            maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure

      \note
               - device/product class code IS NOT FIXED for ModBusPortBus, so you may define your own device/product class codes<br>
               - the specified device/product descriptor (class and indentification string) is returned by ModBusPortBus for any #MBPB_CODE_OTHER_REPORT_SLAVEID request<br>
*/
bool           MBPBProductDescSet(ULONG ulClass, void *pvDesc, UCHAR ucDescLen, U16 u16Timeout);

//!   set the device/product state
/**
      \param	bState					      #TRUE == initialized and running, #FALSE == not initialized, not running or unrecoverable error
      \param	u16Timeout   	            maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - the specified device/product state is returned by ModBusPortBus for any #MBPB_CODE_OTHER_REPORT_SLAVEID request<br>
*/
bool           MBPBProductStateSet(bool bState, U16 u16Timeout);

//!   set the device/product address
/**
      \param	ucAddr					      address of the device to respond to
      \param	u16Timeout   	            maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - the address of the device to respond to is limited by #MBPB_ADDR_MIN and #MBPB_ADDR_MAX (usually 1...15)<br>
*/
bool           MBPBProductAddrSet(UCHAR ucAddr, U16 u16Timeout);

//!   set state of receive and transmit parser (activate / inactivate ModBusPortBus system)
/**
      \param	*pSMBPBPara					   param structure for task(s)
      \param	bRx   	                  #TRUE == enable receive, #FALSE == disable receive
      \param   bTx                        #TRUE == enable transmit, #FALSE == disable transmit
      
      \return										> 0 for success, == 0 for failure

      \note
               - this function is usually INTERNALLY used to enable the ModBusPortBus system after all sub systems are ready<br>
*/
EMBPBErrorCode MBPBMsgCtrl(struct _SMBPBPara *pSMBPBPara, bool bRx, bool bTx);

//!   application task pending on register access
/**
      \param	*pusAddrBase					local address base array (used by ModBusPortBus!), <b>e.g. USHORT usAddrBase[MBPB_TRGTYPE_COUNT]= { 0 }</b>
      \param	bHideBaseType   	         (option, <b>set #TRUE for application default</b>) hide (== #TRUE) or show (== FALSE) base address messages 
      \param   *pulRaw                    (option, <b>set NULL for application default</b>) storage for raw message data
      \param   *pucType                   (option, set NULL to disable) storage for type of decoded register (#EMBPBRegType)
      \param   *pbRW                      (option, set NULL to disable) storage for decoded access type (#EMBPBRegRW)
      \param   *pusAddr                   (option, set NULL to disable) storage for decoded address
      \param   *pusVal                    (option, set NULL to disable) storage for decoded value
      \param   oiID                       OS_ID of the <b>preinitialzed</b> mailbox associated with the filter triggering this function
      \param	u16To   	                  maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - this function is for application AND internal use, therefore for typical application you'll set bHideBaseType= #FALSE and *pulRaw= NULL<br>
               
      <b>*)</b> base address messages are used here to reduce the number of messages which will usually use short relative address messages after a base address message<br>
      
      <b>Example code:</b><br>
      \include MBPBTrgFilterSet_MBPBTrgWaitDecode.c
*/
bool           MBPBTrgWaitDecode(USHORT *pusAddrBase, bool bHideBaseType, ULONG *pulRaw, UCHAR *pucType, bool *pbRW, USHORT *pusAddr, USHORT *pusVal, OS_ID oiID, U16 u16To);

//!   rebuild trigger/filter masks (after activating/deactivating filter items)
/**
      \param	ucType					      register type (#EMBPBRegType)
      \param	u16To   	                  maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - this function rebuilds the - by logic 'OR' - built trigger masks what is necessary after activating/deactivating filter items by #MBPBTrgFilterStateSet<br>
               - new filter items created by #MBPBTrgFilterSet or #MBPBTrgFilterSetEx call this function by themselves
*/
bool           MBPBTrgRebuild(UCHAR ucType, U16 u16To);

//!   set, change (and create) a filter item
/**
      \param	ucType					      register type (#EMBPBRegType)
      \param   usIndex                    filter item number retrieved by #MBPBTrgFilterSet <b>(0 == new filter item, >0 change existing filter item 'usIndex')</b>
      \param   bActive                    activate == #TRUE, deactivate == #FALSE the specified filter item
      \param   ucRWMask                   access mask for supervised address (range) (refer to MBPB_TRGMSG_RWMASK_...  of mbpbmsg.h)
      \param   usAddrMask                 address mask to supervise <b>((usAddr & usAddrMask) == usAddr)</b>
      \param   usValMask                  value mask for supervised address <b>((usVal & usValMask) == usVal)</b>
      \param   oiMbx                      OS_ID of <b>pre-initialized</b> mailbox to write the trigger/filter messages to
      \param   u16ToMbx                   maximum cpu-load free mailbox pending time before any resource claim is canceled by the rtos (in ticks)
      \param	u16To   	                  maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										<b>> 0 == filter index == success</b>, == 0 for failure

      \note
               - use usIndex == 0 to create <b>new filter item</b><br>
               - use usIndex > 0 to <b>change filter item 'usIndex'</b><br>
               - you may use THE SAME mailbox for multiple filter items to process multiple filter events within a single task pending on #MBPBTrgWaitDecode<br>
               
      <b>Example code:</b><br>
      \include MBPBTrgFilterSet_MBPBTrgWaitDecode.c
*/
USHORT         MBPBTrgFilterSet(UCHAR ucType, USHORT usIndex, bool bActive, UCHAR ucRWMask, USHORT usAddrMask, USHORT usValMask, OS_ID oiMbx, U16 u16ToMbx, U16 u16To);

//!   set, change (and create) a filter item (with additional range filter support) to supervise specified address (range) and value (range)
/**
      \param	ucType					      register type (#EMBPBRegType)
      \param   usIndex                    filter item number retrieved by #MBPBTrgFilterSet <b>(0 == new filter item, >0 change existing filter item 'usIndex')</b>
      \param   bActive                    activate == #TRUE, deactivate == #FALSE the specified filter item
      \param   ucRWMask                   access mask for supervised address (range) (refer to #MBPB_TRGMSG_RWMASK_VALZERO and MBPB_TRGMSG_RWMASK_...  of mbpbmsg.h)
      \param   usAddrMask                 address mask to supervise <b>((usAddr & usAddrMask) == usAddr)</b>
      \param   usValMask                  value mask for supervised address <b>((usVal & usValMask) == usVal)</b>
      \param   usAddrMin                  minimum address to trigger for (more specific than usAddrMask can be)
      \param   usAddrMax                  maximum address to trigger for (more specific than usAddrMask can be)
      \param   usValMin                   minimum value to trigger for (more specific than usValMask can be)
      \param   usValMax                   maximum value to trigger for (more specific than usValMask can be)
      \param   oiMbx                      OS_ID of <b>pre-initialized</b> mailbox to write the trigger/filter messages to (mailbox on which #MBPBTrgWaitDecode will pend later)
      \param   u16ToMbx                   maximum cpu-load free mailbox pending time before any resource claim is canceled by the rtos (in ticks)
      \param	u16To   	                  maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										<b>> 0 == filter index == success</b>, == 0 for failure

      \note
               - use usIndex == 0 to create <b>new filter item</b><br>
               - use usIndex > 0 to <b>change filter item 'usIndex'</b><br>
               - you may use THE SAME mailbox for multiple filter items to process all their events within a single task pending on #MBPBTrgWaitDecode<br>
*/
USHORT         MBPBTrgFilterSetEx(UCHAR ucType, USHORT usIndex, bool bActive, UCHAR ucRWMask, USHORT usAddrMask, USHORT usValMask, USHORT usAddrMin, USHORT usAddrMax, USHORT usValMin, USHORT usValMax, OS_ID oiMbx, U16 u16ToMbx, U16 u16To);

//!   (de-) activate a filter item
/**
      \param	ucType					      register type (#EMBPBRegType)
      \param   usIndex                    filter item number returned by #MBPBTrgFilterSet <b>(>0)</b>
      \param   bActive                    activate == #TRUE, deactivate == #FALSE the specified filter item
      \param	u16To   	                  maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - use #MBPBTrgRebuild after activating/deactivating filter items to increase/reduce the number of triggers to fit the requirements<br>
*/
bool           MBPBTrgFilterStateSet(UCHAR ucType, USHORT usIndex, bool bActive, U16 u16To);

//!   entry point for system TMR dispatcher (for handling port time outs)
/**
      \param	u16Info					      timer number of the system tmr module

      \note
               - <b>although this function is public (required for tmr module) you'll NEVER CALL IT FROM YOUR APPLICATION DIRECTLY!!!</b><br>
*/
void           MBPBTmrHandler(U16 u16Info);


// Public prototypes of mbpbregcb.c (THIS IS A COPY OF DOCUMENTION)

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

#endif
