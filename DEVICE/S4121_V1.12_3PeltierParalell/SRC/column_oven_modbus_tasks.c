/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_modbus_communication.c
*
*  \brief               The task \ref TSK_ColumnOvenModbusTasks_CmdExecution handles the modbus communication. \n
*                       Generally device communication with user/service technician/owner is done by means of command
*                       functions via modbus. This functions can be seen at the beginning of \ref column_oven_cmds_reg.c,
*                       \ref column_oven_cmds_user.c and \ref column_oven_cmds_owner.c. \n
*                       It is recommended to add new command functions \b only to the end of function pointer arrays since
*                       there exist a documentation about commands and corresponding modbus addresses. If array is extended
*                       have a look at current address boarders in \ref column_oven_registers.h. \n
*                       If user communicates via modbus with device, the task \ref TSK_ColumnOvenModbusTasks_CmdExecution
*                       calls a specific command/register command function dependent on requested modbus address and passes
*                       the original modbus PDU to corresponding function. \n
*                       Every function returns an base and sub error code which is greater than zero in any error case.
*
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2010-02-03     Initial version imported from S6510_modbus_communication.c.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test
*
*  \todo                FILE OK
*
*			
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbmsg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbdev.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\lib_exceptions_funcs.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\common_support_funcs.h"
#include "column_oven_registers.h"
#include "column_oven_cmds_owner.h"
#include "column_oven_cmds_user.h"
#include "column_oven_cmds_reg.h"
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern         OS_MBX                        MBX_ColumnOvenModbusFuncs_CmdExecution;
extern         tfp_ColumnOvenCmdsOwner_CMD   fpA_ColumnOvenCmdsOwner_CMDs[];
extern         tfp_ColumnOvenCmdsUser_CMD    fpA_ColumnOvenCmdsUser_CMDs[];
extern         tfp_ColumnOvenCmdsReg_REGCMD  fpA_ColumnOvenCmdsReg_REGCMDs_DeviceParamsExtern[];
extern         tfp_ColumnOvenCmdsReg_REGCMD  fpA_ColumnOvenCmdsReg_REGCMDs_DeviceParamsIntern[];
extern const   ushort                        cu16_TimeoutShort;
//------------------------------------------------ tasks ---------------------------------------------------------------------------------------
__task void TSK_ColumnOvenModbusTasks_CmdExecution (void)
{
   ushort   u16A_TempStorage[MBPB_TRGTYPE_COUNT] = { 0 };
   ushort   u16_ModbusAddr;
   ushort   u16_ModbusRxData;
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   ushort   u16_NumRegsToRW   =  1;
   ushort   u16_DeviceActiveInterface;
#endif
//   ushort   u16_DeviceState;
   tS_LibExceptions_Exceptions S_Exceptions;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   FOREVER
   {
      // pending on new modbus register actions and return decoded values (TRUE = hide address base messages)
      if( SUCCESS (MBPBTrgWaitDecode   (&u16A_TempStorage[0], TRUE, NULL, NULL, NULL, &u16_ModbusAddr, &u16_ModbusRxData,
                  MBX_ColumnOvenModbusFuncs_CmdExecution, (ushort)OS_T_FOREVER),
               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_WAIT_DECODE))
      {
//         // read out device state
//         if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__READ),
//                  S_Exceptions.u32_ErrorCodeSW_Sub))
//         {
            // stay in loop as long as command has been executed successfully
//               // check if any command is currently executed or device is in undefined state
//               if (     (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY)
//                     && (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__NONE))
//               {
//                  // check for init/error state (shall not be overwritten)
//                  if (u16_DeviceState == (ushort)COMMON_REGISTERS__DEVICE_STATE__INIT)
//                  u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__INIT;
//                  else if (u16_DeviceState == (ushort)COMMON_REGISTERS__DEVICE_STATE__ERROR)
//                  u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__ERROR;
//                  else
//                  u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY;
                  // set new device state and therefore block execution of any other command as long as current command is executed
//                  if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                           S_Exceptions.u32_ErrorCodeSW_Sub))
//                  {
//                  }
//               }
//         }
            FOREVER//while (1)
            {

#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
                     // read out active device interface
                     if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
                                 (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_ACTIVE_INTERFACE,
                                 u16_NumRegsToRW, (void *)(&u16_DeviceActiveInterface), NULL, cu16_TimeoutShort),
                              S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS) )
#endif
                     {
                     }                     
                        // OWNER command functions - array starts from zero, remove offset and call command functions from function pointer array
                        if    (    (u16_ModbusAddr >= (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START)
                        && (u16_ModbusAddr <= (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__END)  )
                        {
                           TRY_AND_CATCH_APP_FUNC
                           (
                           (*fpA_ColumnOvenCmdsOwner_CMDs[u16_ModbusAddr - (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                           S_Exceptions.u16_ErrorCodeSW_Base
                           );
                        }
                        // REGISTER functions - array starts from zero, remove offset and call command functions from function pointer array
                        else if  (    (u16_ModbusAddr >= (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CONTROL_SECTION_PARAMS)
                        && (u16_ModbusAddr < (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_THRESHOLD_PPM)   )
                        {
                           // The regcmd function pointer array index is zero (0) for total control section params.
                           TRY_AND_CATCH_APP_FUNC
                           (
                           (*fpA_ColumnOvenCmdsReg_REGCMDs_DeviceParamsIntern[0])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                           S_Exceptions.u16_ErrorCodeSW_Base
                           );
                        }
                        else if  ((u16_ModbusAddr >= (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_THRESHOLD_PPM)
                        && (u16_ModbusAddr <= (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_ENABLED))
                        {
                           // Caution: The first array index here must be one "1"!
                           TRY_AND_CATCH_APP_FUNC
                           (
                           (*fpA_ColumnOvenCmdsReg_REGCMDs_DeviceParamsIntern[u16_ModbusAddr + 1 - (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_THRESHOLD_PPM])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                           S_Exceptions.u16_ErrorCodeSW_Base
                           );
                        }
                        else if  (    (u16_ModbusAddr >= (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_OFFSET)
                        && (u16_ModbusAddr <= (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND)   )
                        {
                           TRY_AND_CATCH_APP_FUNC
                           (
                           (*fpA_ColumnOvenCmdsReg_REGCMDs_DeviceParamsExtern[u16_ModbusAddr - (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_OFFSET])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                           S_Exceptions.u16_ErrorCodeSW_Base
                           );
                        }
                        // USER command functions - array starts from zero, remove offset and call command functions from function pointer array
                        else if  (    (u16_ModbusAddr >= (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START)
                        && (u16_ModbusAddr <= (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__END)   )
                        {
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
                           // Block execution of USER commands if RS232 or display keys are not selected as active interface.
                           // If no active interface was selected by user, user command functions must not be executed.
                           if (     (u16_DeviceActiveInterface == (uchar)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__RS232)
                                 || (u16_DeviceActiveInterface == (uchar)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__DISPLAY_KEYS))
#endif
                           {
                              TRY_AND_CATCH_APP_FUNC
                              (
                              (*fpA_ColumnOvenCmdsUser_CMDs[u16_ModbusAddr - (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                              S_Exceptions.u16_ErrorCodeSW_Base
                              );
                           }
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
                           else
                           S_Exceptions.u16_WarningCode = (ushort)LIB_EXCEPTIONS__WARNING_SPECIAL__DEVICE_ACTIVE_INTERFACE_WRONG;
#endif
                        }
                        // set exceptions codes into register bank
                        if (S_Exceptions.u32_ErrorCodeSW_Sub || S_Exceptions.u16_ErrorCodeSW_Base)
                        LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
//                        if (u16_DeviceState == (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY)
//                        {
//                           // set new device state to be able to receive and execute commands
//                           u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__IDLE;
//                           TRY_AND_CATCH_APP_FUNC
//                           (
//                           CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                           S_Exceptions.u32_ErrorCodeSW_Sub
//                           );
//                        }
                        break; // for while loop
               RESCHEDULE(50);
            }
      }
      else
      S_Exceptions.u32_ErrorCodeSW_Sub = (S_Exceptions.u32_ErrorCodeSW_Sub << 16) | (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__MODBUS_DATA_DISASSEMLING_FAILED;
      if (S_Exceptions.u32_ErrorCodeSW_Sub)
      {
         if (!S_Exceptions.u16_ErrorCodeSW_Base)
         S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MODBUS_TASKS__CMD_EXECUTION;
         // set exception codes to register bank
         LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
      }
//      #ifndef OEM_LED//If NOT Defined
//      {
         // update display
         AT91F_PIO_SetOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
         RESCHEDULE(50);
         // update display
         AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
         RESCHEDULE(50);
//      }
//      #endif
   }
}
//----------------------------------------------------------------------------------------------------------------------------------------------
