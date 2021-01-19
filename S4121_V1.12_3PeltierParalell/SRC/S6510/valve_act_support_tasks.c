/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_support_tasks.c
*
*  \brief               The file contains the current support tasks for valve actuator application. Important is the callback function
*                       \ref ValveActSupportTasks_ExternControlContactHandler which controls the user, owner or service technician
*                       demands on extern device interface (pin contact strip). Though this function is a callback function of
*                       \ref AT91SAM7X.c it runs inside driver task context! \n
*                       A similarly function like \ref ValveActSupportTasks_ExternControlContactHandler with the same function
*                       arguments can be used to react on other pins. For this, the function address must be set to \ref SMcp2317List
*                       in \ref AT91SAM7X.c.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-10     Initial version extracted from S6510_cmd_functions_utils.c and S6510_utils.c.
*  \date                2012-11-06     Changed name of function "TSK_ValveActSupportTasks_CmdExecutionPinContactStrip"
*                                      to \ref ValveActSupportTasks_ExternControlContactHandler.
*  \date                2012-11-08     Changed exception handling after and pre-check executing any command in task \ref
*                                      ValveActSupportTasks_ExternControlContactHandler.
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
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\lib_exceptions_funcs.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\common_support_funcs.h"
#include "valve_act_registers.h"
#include "valve_act_cmds_user.h"
#include "valve_act_support_funcs.h"
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern            OS_MBX   MBX_ValveActSupportFuncs_GenerateInjectSignal;
extern   const    ushort   cu16_TimeoutShort;
//------------------------------------------------ tasks ---------------------------------------------------------------------------------------
// function run in driver task context
void ValveActSupportTasks_ExternControlContactHandler (U16 u16INTF, U16 u16INTCAP, U16 u16Pin, bool bLevel)
{
   //ushort   u16_DeviceState;
   ushort   u16_DeviceActiveInterface;
   ushort   u16_NumRegsToRW      =  1;
   ulong    u32_ErrorCodeSW_Sub  = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   tS_LibExceptions_Exceptions S_Exceptions;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   // read out active device interface
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
               (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_ACTIVE_INTERFACE,
               (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_DeviceActiveInterface), NULL, cu16_TimeoutShort),
            S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS) )
   {
      if (u16_DeviceActiveInterface == (ushort)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__EXTERN_CONTROL_CONTACT)
      {
         // execute USER commands
         if ((!bLevel) && ((u16Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_HOME) || (u16Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT)))
         {
//            // read out device state
//            if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__READ),
//                     S_Exceptions.u32_ErrorCodeSW_Sub))
//            {
//               // check if any command is currently executed, device detected an error or is in init state
//               if (     (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY)
//                     && (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__INIT)
//                     && (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__NONE))
//               {
//                  // check for error state (shall not be overwritten)
//                  if (u16_DeviceState == (ushort)COMMON_REGISTERS__DEVICE_STATE__ERROR)
//                  u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__ERROR;
//                  else
//                  u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY;
//                  // set new device state and therefore block execution of any other command as long as current command is executed
//                  if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                           S_Exceptions.u32_ErrorCodeSW_Sub))
//                  {
                     // read out active device interface
                     if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
                                 (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_ACTIVE_INTERFACE,
                                 u16_NumRegsToRW, (void *)(&u16_DeviceActiveInterface), NULL, cu16_TimeoutShort),
                              S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS) )
                     {
                        // check if device interface is correct
                        if (u16_DeviceActiveInterface == (ushort)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__EXTERN_CONTROL_CONTACT)
                        {
                           if (u16Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_HOME)
                           {
                              TRY_AND_CATCH_APP_FUNC
                              (
                              ValveActCmdsUser_GoToHomePos ((ushort)VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_HOME_POS, &S_Exceptions.u32_ErrorCodeSW_Sub),
                              S_Exceptions.u16_ErrorCodeSW_Base
                              );
                           }
                           else if (u16Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT)
                           {
                              TRY_AND_CATCH_APP_FUNC
                              (
                              ValveActCmdsUser_GoToNextPos ((ushort)VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_NEXT_POS, &S_Exceptions.u32_ErrorCodeSW_Sub),
                              S_Exceptions.u16_ErrorCodeSW_Base
                              );
                           }
                           //                     else if (u16Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_PREVIOUS)	 /*New Perhaps instead of Home*/
                           //                     {
                           //                        TRY_AND_CATCH_APP_FUNC
                           //                        (
                           //                           ValveActCmdsUser_GoToPreviousPos ((ushort)VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_PREVIOUS_POS, &S_Exceptions.u32_ErrorCodeSW_Sub),
                           //                           S_Exceptions.u16_ErrorCodeSW_Base
                           //                        );							
                           //                     }
                           // set exception codes to register bank
                           if (S_Exceptions.u32_ErrorCodeSW_Sub || S_Exceptions.u16_ErrorCodeSW_Base)
                           LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
//                           if (u16_DeviceState == (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY)
//                           {
//                              // set new device state
//                              u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__IDLE;
//                              TRY_AND_CATCH_APP_FUNC
//                              (
//                              CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                              u32_ErrorCodeSW_Sub
//                              );
//                           }
                        }
                        else
                        S_Exceptions.u16_WarningCode = (ushort)LIB_EXCEPTIONS__WARNING_SPECIAL__DEVICE_ACTIVE_INTERFACE_WRONG;
                     }
//                  }
//               }
//               else
//               S_Exceptions.u16_WarningCode = (ushort)LIB_EXCEPTIONS__WARNING_SPECIAL__DEVICE_BUSY;
//            }
         }
      }
   }
   if ((S_Exceptions.u32_ErrorCodeSW_Sub) || (u32_ErrorCodeSW_Sub))
   {
      if (!S_Exceptions.u16_ErrorCodeSW_Base)
      {
         S_Exceptions.u32_ErrorCodeSW_Sub    =  u32_ErrorCodeSW_Sub;
         S_Exceptions.u16_ErrorCodeSW_Base   =  (ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS__TSK_CMD_EXECUTION_EXTERN_CONTROL_CONTACT_HANDLER;
      }
   }
   // set exception codes to register bank
   LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
   // prevent system locks
   os_dly_wait ((ushort)AT91SAM7X__TICK_10);
   // update display
   AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
__task void TSK_ValveActSupportTasks_GenerateInjectSignal (void)
{
   ushort   u16_CurrentPosition;
   uchar    u8_Unit                 = 0;
   tS_LibExceptions_Exceptions S_Exceptions;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   FOREVER
   {
      if (OS_R_TMO != os_mbx_wait ((OS_ID)MBX_ValveActSupportFuncs_GenerateInjectSignal, (void *)(&u16_CurrentPosition), OS_T_FOREVER))
      {
         if (u16_CurrentPosition == (ushort)AT91SAM7X__VALVE_ACT_POS_INDEX_INJECT)
         {
            // set MCP2317_NINJECT low (level = second argument)
            if (SUCCESS (Mcp2317SetOutput (u8_Unit, (ushort)AT91SAM7X__MCP2317_NINJECT, (ushort)~AT91SAM7X__MCP2317_NINJECT,
                        (ushort)AT91SAM7X__TICK_500),
                     S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MCP2317__SET_OUTPUT))
            {
               // wait one second (1s)
               os_dly_wait (OS_T_MS(1000));
               // set MCP2317_NINJECT high again (level = second argument)
               TRY_AND_CATCH
               (
               Mcp2317SetOutput (u8_Unit, (ushort)AT91SAM7X__MCP2317_NINJECT, (ushort)AT91SAM7X__MCP2317_NINJECT,
               (ushort)AT91SAM7X__TICK_500),
               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MCP2317__SET_OUTPUT
               );
               // task delay to prevent system locks
               os_dly_wait (cu16_TimeoutShort);
            }
         }
         if (S_Exceptions.u32_ErrorCodeSW_Sub )
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS__TSK_GENERATE_INJECT_SIGNAL;
            // set exception codes to register bank
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
            // update display
            AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
         }
         // task delay to prevent system locks
         os_dly_wait ((ushort)AT91SAM7X__TICK_0);
         // update display
         AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
      }
      RESCHEDULE(10);      
   }
}
