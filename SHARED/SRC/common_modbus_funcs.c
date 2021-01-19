/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_modbus_funcs.c
*
*  \brief               This file handles the common modbus tasks initialization. It set two modbus "trigger" filters 
*                       to trigger corresponding modbus tasks if any modbus command fits into address space. \n
*                       Furthermore this file contains a modbus prefilter to handle write accesses to holding register.
*                       This means if any invalid modbus data value is set by user/owner or service technician, it is 
*                       reclined by modbus prefilter. The prefilter must be configured by SW engineer.
*
*
*  \author              Ruediger Radtke
*  \version             1.2
*
*  \date                2011-08-08     Initial version imported from S4121_modbus_communication.c.
*  \date                2012-07-16     <em>File version 1.1</em>. Added modbus prefilter check for 
*                                      mainboard voltage monitoring register argument.
*  \date                2012-11-08     <em>File version 1.2</em>. Added check for zero argument as active device 
*                                      interface in \ref CommonModbusFuncs_FilterForModbusRequests.
*  \date                2012-11-09     Removed modbus prefilter for "mainboard voltage monitoring disabling" 
*                                      because of feature removal.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                
*
*  \todo                
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbmsg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbdev.h"
#include "..\..\..\protocol\modbus\mbpbext.h"

#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"

#include "lib_exceptions.h"

#include "common_modbus_tasks.h"
#include "common_registers.h"
#include "common_cmds_owner.h"
#include "common_modbus_funcs.h"

//---------------------------------------------- global declarations/definitions ---------------------------------------------------------------

OS_TID  TID_CommonModbusFuncs_CmdExecution;                                   /**< OS_TID = UINT = U32 = task identifier */

OS_TSK_STACK_USER(STK_TSK_CommonModbusFuncs_CmdExecution, 1700);              /**< define stack for \ref TSK_CommonModbusTasks_CmdExecution in [bytes] */ 

os_mbx_declare(MBX_CommonModbusFuncs_CmdExecution, 128);                       /**< mailbox for command execution function - defines mailbox object (arrays for intertask communication) for 
                                                                                   \ref TSK_CommonModbusTasks_CmdExecution with size of (20 + 4) * [4 byte] */

//OS_TSK_STACK_USER(STK_TSK_CommonModbusFuncs_CmdExecution, 1700);              /**< define stack for \ref TSK_CommonModbusTasks_CmdExecution in [bytes] */ 

//os_mbx_declare(MBX_CommonModbusFuncs_CmdExecution, 20);                       /**< mailbox for command execution function - defines mailbox object (arrays for intertask communication) for 
//                                                                                   \ref TSK_CommonModbusTasks_CmdExecution with size of (20 + 4) * [4 byte] */

//#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
//   OS_TID  TID_CommonModbusFuncs_ResponseOnDisplayRead;
//
//   OS_TSK_STACK_USER(STK_TSK_CommonModbusFuncs_ResponseOnDisplayRead, 1200);  /**< define stack for \ref TSK_CommonModbusTasks_ResponseOnDisplayRead in [bytes] */
//
//   os_mbx_declare(MBX_CommonModbusFuncs_ResponseOnDisplayRead, 20);
//#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif

//---------------------------------------------- externals used in this file -------------------------------------------------------------------

extern const   ushort   cu16_UtilsFuncs_TimeoutLong;
extern         ushort   u16_CommonRegistersFuncs_SykamDeviceDataWriteEnableCode;

//---------------------------------------------- functions -------------------------------------------------------------------------------------

ulong CommonModbusFuncs_InitTasks (void) 
{
   ulong u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;

   // initialize mailbox "MBX_CommonModbusFuncs_CmdExecution"
   os_mbx_init( MBX_CommonModbusFuncs_CmdExecution, sizeof(MBX_CommonModbusFuncs_CmdExecution) );

//#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
//   // initialize mailbox "MBX_CommonModbusFuncs_ResponseOnDisplayRead"
//   os_mbx_init( MBX_CommonModbusFuncs_ResponseOnDisplayRead, sizeof(MBX_CommonModbusFuncs_ResponseOnDisplayRead) );
//#endif
//#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
//#endif

   // Set a trigger filter to receive common commands via modbus (MBPB).
   // Settings:
   // Watch holding registers, create a new filter, filter is enabled, trigger on write actions, trigger on addresses 
   // inside min-max mask, trigger on addresses inside min-max (must be inside mask), trigger on values inside min-max
   // mailbox for tasks which receives a mail in case a command was send, timeout value for MBX and the function itself.
   if (SUCCESS (MBPBTrgFilterSetEx  ((uchar)MBPB_TRGTYPE_HOLDING_WRITE, (ushort)NULL, TRUE, (uchar)(MBPB_TRGMSG_RWMASK_WRITE | 
                                      MBPB_TRGMSG_RWMASK_VALZERO), 
                                     (ushort)COMMON_MODBUS_FUNCS__MODBUS_ADDR_MASK__COMMON_CMDS, 
                                     (ushort)COMMON_MODBUS_FUNCS__MODBUS_VAL_MASK__COMMON_CMDS,
                                     (ushort)COMMON_MODBUS_FUNCS__MODBUS_ADDR_MIN__COMMON_CMDS,
                                     (ushort)COMMON_MODBUS_FUNCS__MODBUS_ADDR_MAX__COMMON_CMDS,
                                     (ushort)COMMON_MODBUS_FUNCS__MODBUS_VAL_MIN__COMMON_CMDS,
                                     (ushort)COMMON_MODBUS_FUNCS__MODBUS_VAL_MAX__COMMON_CMDS,
                                      MBX_CommonModbusFuncs_CmdExecution,
                                      cu16_UtilsFuncs_TimeoutLong, cu16_UtilsFuncs_TimeoutLong),
                u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_FILTER_SET) )
   {

//#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
// 
//      // Set a trigger filter to recognize display read actions on modbus (MBPB).
//      if (SUCCESS (MBPBTrgFilterSetEx  ((uchar)(MBPB_TRGTYPE_INPUT_READ), (ushort)NULL, TRUE, (uchar)(MBPB_TRGMSG_RWMASK_READ | 
//                                         MBPB_TRGMSG_RWMASK_VALZERO), 
//                                        (ushort)COMMON_MODBUS_FUNCS__MODBUS_ADDR_MASK__RESPONSE_ON_DISPLAY_READ, 
//                                        (ushort)COMMON_MODBUS_FUNCS__MODBUS_VAL_MASK__RESPONSE_ON_DISPLAY_READ,
//                                        (ushort)COMMON_MODBUS_FUNCS__MODBUS_ADDR_MIN__RESPONSE_ON_DISPLAY_READ,
//                                        (ushort)COMMON_MODBUS_FUNCS__MODBUS_ADDR_MAX__RESPONSE_ON_DISPLAY_READ,
//                                        (ushort)COMMON_MODBUS_FUNCS__MODBUS_VAL_MIN__RESPONSE_ON_DISPLAY_READ,
//                                        (ushort)COMMON_MODBUS_FUNCS__MODBUS_VAL_MAX__RESPONSE_ON_DISPLAY_READ,
//                                         MBX_CommonModbusFuncs_ResponseOnDisplayRead, cu16_UtilsFuncs_TimeoutLong, cu16_UtilsFuncs_TimeoutLong),
//                   u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_FILTER_SET ) )
//      {
//         // create tasks with user defined stack
//         TID_CommonModbusFuncs_ResponseOnDisplayRead = os_tsk_create_user  (TSK_CommonModbusTasks_ResponseOnDisplayRead, (uchar)OS_P_NORMAL,
//                                                                            (void *)STK_TSK_CommonModbusFuncs_ResponseOnDisplayRead, 
//                                                                            (ushort)sizeof(STK_TSK_CommonModbusFuncs_ResponseOnDisplayRead));
//      }
//#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif

      TID_CommonModbusFuncs_CmdExecution          = os_tsk_create_user  (TSK_CommonModbusTasks_CmdExecution, (uchar)OS_P_NORMAL,
                                                                         (void *)(&STK_TSK_CommonModbusFuncs_CmdExecution[0]),
                                                                         (ushort)sizeof(STK_TSK_CommonModbusFuncs_CmdExecution));
   }

   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

bool CommonModbusFuncs_FilterForModbusRequests  (uchar u8_ModbusDataType, U32 u32_ModbusAddress, ushort *pu16_ModbusData)
{
   bool  b_Result =  (bool)TRUE;

   // Caution: Driver context!
   // Default action is TRUE to accept incoming write requests on holding register or coils.
   // Setting the b_Result to FALSE declines the request for u32_ModbusAddress and does NOT change the holding register or coil.
   // Furthermore an exception message will be send back to the requester.

   if (pu16_ModbusData)
   {
      if (u8_ModbusDataType == (uchar)MBPB_REGTYPE_HOLDING)
      {
         // inspect writes on Sykam device data (fixed)
         if (     (u32_ModbusAddress >= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START)
               && (u32_ModbusAddress <= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__VERSION_END)
            )
         {
            b_Result = (bool)FALSE;
         }
         // inspect writes on Sykam device data (changeable)
         else if (   ((u32_ModbusAddress >= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__MANUFACTURER_DEVICE_ID_START)
                       && (u32_ModbusAddress <= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__SERIAL_END))
                  ||
                     ((u32_ModbusAddress >= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START)
                       && (u32_ModbusAddress <= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_END))
                 )
         {
            if (u16_CommonRegistersFuncs_SykamDeviceDataWriteEnableCode != (ushort)COMMON_REGISTERS__SYKAM_DEVICE_DATA_WRITE_ENABLE_CODE)
            {
               b_Result = (bool)FALSE;
            }
         }
         else         
         {
            switch (u32_ModbusAddress)
            {
               case (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_ACTIVE_INTERFACE:
               {
                  if ((*pu16_ModbusData >= (ushort)COMMON_REGISTERS__ACTIVE_INTERFACES_COUNTER) && (*pu16_ModbusData != 0))
                     b_Result = (bool)FALSE;

                  break;
               }
               case (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA_LOAD:
               {
                  if (*pu16_ModbusData != (ushort)COMMON_CMDS_OWNER__CMD_ARG__SYKAM_DEVICE_DATA_LOAD)
                     b_Result = (bool)FALSE;

                  break;
               }
               case (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA_SAVE:
               {
                  if (*pu16_ModbusData != (ushort)COMMON_CMDS_OWNER__CMD_ARG__SYKAM_DEVICE_DATA_SAVE)
                     b_Result = (bool)FALSE;

                  break;
               }
               case (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__MODBUS_SET_SLAVE_ADDRESS:
               {
                  if ((*pu16_ModbusData == 0) || (*pu16_ModbusData > (ushort)AT91SAM7X__MODBUS_MAX_SLAVE_ID))
                     b_Result = (bool)FALSE;
          
                  break;
               }
               case (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_RUN_TIME_COUNTER_RESET:
               {
                  if (*pu16_ModbusData != COMMON_CMDS_OWNER__CMD_ARG__DEVICE_RUN_TIME_COUNTER_RESET)
                     b_Result = (bool)FALSE;
          
                  break;
               }
               case (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__ERROR_CODE_SW_CLEAR_CURR:
               {
                  if (*pu16_ModbusData != (ushort)COMMON_CMDS_OWNER__CMD_ARG__ERROR_CODE_SW_CLEAR_CURR)
                     b_Result = (bool)FALSE;
            
                  break;
               }
               case (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__ERROR_CODE_SW_CLEAR_ALL:
               {
                  if (*pu16_ModbusData != (ushort)COMMON_CMDS_OWNER__CMD_ARG__ERROR_CODE_SW_CLEAR_ALL)
                     b_Result = (bool)FALSE;

                  break;
               }
            }
         }
      }
   }


   return b_Result;
}
