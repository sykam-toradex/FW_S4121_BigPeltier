/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_modbus_tasks.c
*
*  \brief               This file handles the valve actuator modbus communication.
*                       Generally communication with user, service technician and owner is done by means of commands
*                       which are send via modbus. A write inquiry on holding register which was send to device via Modbus 
*                       triggers a specific command/register command function dependent on requested modbus address.
*                       This functions can be seen at the beginning of \ref valve_act_cmds_reg.c, \ref valve_act_cmds_user.c 
*                       and \ref valve_act_cmds_owner.c. \n
*                       Every function returns an software base and sub error code which is greater than zero in any error case.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-11     Initial version imported from S4121_modbus_communication.c.
*  \date                2012-11-07     Changed/added intern and extern device parameters for register commands.
*  \date                2012-11-08     Changed exception handling after and pre-check before executing any command.
*  \date                2012-11-15     Removed unnecessary variable from task \ref TSK_ValveActModbusTasks_CmdExecution.

*
*
*  \bug                 2012-11-08     Removed wrong overwriting of "error" device state to "busy" device state.
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
#include "valve_act_cmds_owner.h"
#include "valve_act_cmds_user.h"
#include "valve_act_cmds_reg.h"


//---------------------------------------------- externals used in this file -------------------------------------------------------------------

extern         OS_MBX                     MBX_ValveActModbusFuncs_CmdExecution;

extern         tfp_ValveActCmdsOwner_CMD  fpA_ValveActCmdsOwner_CMDs[];
extern         tfp_ValveActCmdsUser_CMD   fpA_ValveActCmdsUser_CMDs[];
extern         tfp_ValveActCmdsReg_REGCMD fpA_ValveActCmdsReg_REGCMDs_DeviceParamsExtern[];
extern         tfp_ValveActCmdsReg_REGCMD fpA_ValveActCmdsReg_REGCMDs_DeviceParamsIntern[];

extern const   ushort                     cu16_TimeoutShort;

//------------------------------------------------ tasks ---------------------------------------------------------------------------------------

__task void TSK_ValveActModbusTasks_CmdExecution (void)
{
   ushort   u16A_TempStorage[MBPB_TRGTYPE_COUNT] = { 0 };
   ushort   u16_ModbusAddr;
   ushort   u16_ModbusRxData;
   ushort   u16_DeviceActiveInterface;
//   ushort   u16_DeviceState;

   tS_LibExceptions_Exceptions S_Exceptions;
   
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));      
      
   FOREVER
   {
      // pending on new modbus register actions and return decoded values (TRUE = hide address base messages)
      if( SUCCESS (MBPBTrgWaitDecode   (&u16A_TempStorage[0], TRUE, NULL, NULL, NULL, &u16_ModbusAddr, &u16_ModbusRxData, 
                                        MBX_ValveActModbusFuncs_CmdExecution, (ushort)OS_T_FOREVER),
                   S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_WAIT_DECODE))
      {
//         // read out device state
//         if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__READ),
//                               S_Exceptions.u32_ErrorCodeSW_Sub))
//         {
//            // check if any command is currently executed or is in init state
//            if (     (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY) 
//                  && (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__INIT)
//                  && (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__NONE))
//            {
//               // check for error state (shall not be overwritten)
//               if (u16_DeviceState == (ushort)COMMON_REGISTERS__DEVICE_STATE__ERROR)
//                  u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__ERROR;
//               else
//                  u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY;
//               
//               // set new device state and therefore block execution of any other command as long as current command is executed
//               if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                                     S_Exceptions.u32_ErrorCodeSW_Sub))
//               {
                  // read out active device interface
                  if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
                                              (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_ACTIVE_INTERFACE, 
                                              (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_DeviceActiveInterface), NULL, cu16_TimeoutShort),
                               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS) )
                  {
							if (u16_DeviceActiveInterface == (uchar)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__NONE)//Added to prevent lock up when extern signal is triggered.
								u16_DeviceActiveInterface = (uchar)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__DISPLAY_KEYS;	                        

                     // OWNER command functions - array starts from zero, remove offset and call command functions from function pointer array
                     if    (    (u16_ModbusAddr >= (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START)
                             && (u16_ModbusAddr <= (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__END)  )
                     {
                        TRY_AND_CATCH_APP_FUNC
                        (
                           (*fpA_ValveActCmdsOwner_CMDs[u16_ModbusAddr - (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                           S_Exceptions.u16_ErrorCodeSW_Base
                        );
                     }
                     // REGISTER functions - array starts from zero, remove offset and call command functions from function pointer array
                     else if  (    (u16_ModbusAddr >= (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START) 
                                && (u16_ModbusAddr <= (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__END)   )
                     {
                        TRY_AND_CATCH_APP_FUNC
                        (                     
                           (*fpA_ValveActCmdsReg_REGCMDs_DeviceParamsIntern[u16_ModbusAddr - (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                           S_Exceptions.u16_ErrorCodeSW_Base
                        );
                     }
                     else if  (    (u16_ModbusAddr >= (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START) 
                                && (u16_ModbusAddr <= (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__END)   )
                     {
                        TRY_AND_CATCH_APP_FUNC
                        (                     
                           (*fpA_ValveActCmdsReg_REGCMDs_DeviceParamsExtern[u16_ModbusAddr - (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                           S_Exceptions.u16_ErrorCodeSW_Base
                        );
                     }
                     // USER command functions - array starts from zero, remove offset and call command functions from function pointer array      
                     else if  (    (u16_ModbusAddr >= (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START) 
                                && (u16_ModbusAddr <= (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__END)   )
                     {
								// Block execution of USER commands if RS232 or display keys are not selected as active interface.
                        // If no active interface was selected by user, user command functions must not be executed.
                        if (     (u16_DeviceActiveInterface == (uchar)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__RS232)
                              || (u16_DeviceActiveInterface == (uchar)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__DISPLAY_KEYS))
                        {
                           TRY_AND_CATCH_APP_FUNC
                           (
                              (*fpA_ValveActCmdsUser_CMDs[u16_ModbusAddr - (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                              S_Exceptions.u16_ErrorCodeSW_Base
                           );
                        }
                        else
                           S_Exceptions.u16_WarningCode = (ushort)LIB_EXCEPTIONS__WARNING_SPECIAL__DEVICE_ACTIVE_INTERFACE_WRONG;
                     }
                     
                     // set exception codes to register bank
                     if (S_Exceptions.u32_ErrorCodeSW_Sub || S_Exceptions.u16_ErrorCodeSW_Base)
                        LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
                     
                     
//                     if (u16_DeviceState == (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY)
//                     {
//                        // set new device state to be able to receive and execute commands
//                        u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__IDLE;
//                     
//                        TRY_AND_CATCH_APP_FUNC
//                        (
//                           CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                           S_Exceptions.u32_ErrorCodeSW_Sub
//                        );
//                     }
                  }
//               }
//            }
//            else
//               S_Exceptions.u16_WarningCode = (ushort)LIB_EXCEPTIONS__WARNING_SPECIAL__DEVICE_BUSY;
//         }
      }
      else
         S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MODBUS_TASKS__CMD_EXECUTION;

      if (S_Exceptions.u32_ErrorCodeSW_Sub)
      {
         if (!S_Exceptions.u16_ErrorCodeSW_Base)
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_MODBUS_TASKS__CMD_EXECUTION;
         
         // set exception codes to register bank
         LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
         // update display      
         AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
      }      
      RESCHEDULE(10);      
   }
   // update display      
   AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
}

//----------------------------------------------------------------------------------------------------------------------------------------------
