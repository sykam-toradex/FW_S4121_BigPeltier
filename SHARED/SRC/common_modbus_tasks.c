/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_modbus_tasks.c
*
*  \brief               This file handles the modbus communication for common commands. The functional principle is that
*                       someone (user/service technician/owner) writes an valid command argument to a modbus holding register.
*                       The common command functions are externally documentated. \n
*                       At the beginning of \ref common_cmds_reg.c, \ref common_cmds_user.c and \ref common_cmds_owner.c
*                       the software function addresses are set into a function pointer array which addresses are called by
*                       task \ref TSK_CommonModbusTasks_CmdExecution. \n
*                       If user communicates via modbus with the device, a specific command or register command
*                       function is called dependent on requested modbus address. The called function passes the original
*                       modbus PDU data. \n
*                       Every function returns a software base and sub error code which is greater than zero in any error case.
*
*
*  \author              Ruediger Radtke
*  \version             1.2
*
*  \date                2011-08-08     Initial version imported from S4121_modbus_communication.c.
*  \date                2012-08-21     <em>File version 1.1</em>. Added a while loop to wait for executing commands if device
*                                      is in "busy" state. Added device state check and changed exception handling.
*  \date                2012-11-15     <em>File version 1.2</em>. Removed unnecessary variable in while-loop from task
*                                      \ref TSK_CommonModbusTasks_CmdExecution.
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
#include <AT91SAM7X512.H>
#include <lib_AT91SAM7X256.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"

#include "lib_exceptions.h"
#include "lib_exceptions_funcs.h"
#include "common_modbus_tasks.h"
#include "common_registers.h"
#include "common_cmds_owner.h"
#include "common_cmds_user.h"
#include "common_cmds_reg.h"
#include "common_support_funcs.h"
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern         OS_MBX                     MBX_CommonModbusFuncs_CmdExecution;
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
extern         OS_MBX                     MBX_CommonModbusFuncs_ResponseOnDisplayRead;
#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif
extern         tfp_CommonCmdsOwner_CMD    fpA_CommonCmdsOwner_CMDs[];
extern         tfp_CommonCmdsUser_CMD     fpA_CommonCmdsUser_CMDs[];
extern         tfp_CommonCmdsReg_REGCMD   fpA_CommonCmdsReg_REGCMDs_DeviceParams[];
extern const   ushort                     cu16_UtilsFuncs_TimeoutLong;
//---------------------------------------------- tasks -----------------------------------------------------------------------------------------
__task void TSK_CommonModbusTasks_CmdExecution (void)
{
   ushort                        u16A_TempStorage[MBPB_TRGTYPE_COUNT]   =  { 0 };
   ushort                        u16_ModbusAddr;
   ushort                        u16_ModbusRxData;
//   ushort                        u16_DeviceActiveInterface;
//   ushort                        u16_DeviceState;
   tS_LibExceptions_Exceptions   S_Exceptions;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   FOREVER
   {
      // pending on new modbus register actions and return decoded values (TRUE = hide address base messages)
      if (SUCCESS (MBPBTrgWaitDecode   (&u16A_TempStorage[0], TRUE, NULL, NULL, NULL, &u16_ModbusAddr, &u16_ModbusRxData,
                  MBX_CommonModbusFuncs_CmdExecution, OS_T_FOREVER),
               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_WAIT_DECODE ) )
      {
            FOREVER//while (1)
            {
                   
               // OWNER command functions - array starts from zero, remove offset and call command functions from function pointer array
               if (     (u16_ModbusAddr >= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START)
                     && (u16_ModbusAddr <= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__END)  )
               {
                  TRY_AND_CATCH_APP_FUNC
                  (
                  (*fpA_CommonCmdsOwner_CMDs[u16_ModbusAddr - (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                  S_Exceptions.u16_ErrorCodeSW_Base
                  );
               }
               // REGISTER functions - array starts from zero, remove offset and call command functions from function pointer array
               else if  (     (u16_ModbusAddr >= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS__START)
               && (u16_ModbusAddr <= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS__END)  )
               {
                  TRY_AND_CATCH_APP_FUNC
                  (
                  (*fpA_CommonCmdsReg_REGCMDs_DeviceParams[u16_ModbusAddr - (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS__START])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                  S_Exceptions.u16_ErrorCodeSW_Base
                  );
               }
               // USER command functions - array starts from zero, remove offset and call command functions from function pointer array
               else if  (     (u16_ModbusAddr >= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START)
               && (u16_ModbusAddr <= (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__END)   )
               {
                  TRY_AND_CATCH_APP_FUNC
                  (
                  (*fpA_CommonCmdsUser_CMDs[u16_ModbusAddr - (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START])(u16_ModbusRxData, &S_Exceptions.u32_ErrorCodeSW_Sub),
                  S_Exceptions.u16_ErrorCodeSW_Base
                  );
               }
               // set exception codes to register bank
               if (S_Exceptions.u32_ErrorCodeSW_Sub || S_Exceptions.u16_ErrorCodeSW_Base)
               LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
               break; // for while loop
               RESCHEDULE(1);
            } // end of while
      }
      else
      S_Exceptions.u32_ErrorCodeSW_Sub = (S_Exceptions.u32_ErrorCodeSW_Sub << 16) | (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_MODBUS_DATA_DISASSEMLING_FAILED;
      if (S_Exceptions.u32_ErrorCodeSW_Sub)
      {
         if (!S_Exceptions.u16_ErrorCodeSW_Base)
         S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_MODBUS_TASKS__CMD_EXECUTION;
         // set exception codes to register bank
         LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
      }
   RESCHEDULE(5);
   }
   DETACH;
   
}
//----------------------------------------------------------------------------------------------------------------------------------------------
//#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY

//__task void TSK_CommonModbusTasks_ResponseOnDisplayRead (void)
//{
//   ushort                        u16A_TempStorage[MBPB_TRGTYPE_COUNT] = { 0 };
//   ushort                        u16_ModbusAddr;
//   ushort                        u16_ModbusRxData;
//   tS_LibExceptions_Exceptions   S_Exceptions;
//   // reset of structure is necessary!
//   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
//   #ifndef OEM_LED //If NOT defined              
//   {
//      FOREVER
//      {
//         // pending on new address/value trigger message and return decoded values (TRUE = hide address base messages)
//         if (SUCCESS (MBPBTrgWaitDecode   (&u16A_TempStorage[0], TRUE, NULL, NULL, NULL, &u16_ModbusAddr, &u16_ModbusRxData,
//                     MBX_CommonModbusFuncs_ResponseOnDisplayRead, OS_T_FOREVER ),
//                  S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_WAIT_DECODE ) )
//         {
////            // if device status register bank was read by display, set display attention signal
////            if(    (u16_ModbusAddr >= (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START)
////                || (u16_ModbusAddr <= (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__END) )
////            {
////               // update display
////               AT91F_PIO_SetOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
////               RESCHEDULE(50);
////               // update display
////               AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
////               RESCHEDULE(50);
////           }
//         }
//         else
//         {
//            S_Exceptions.u32_ErrorCodeSW_Sub    =  (S_Exceptions.u32_ErrorCodeSW_Sub << 16) | (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_MODBUS_DATA_DISASSEMLING_FAILED;
//            S_Exceptions.u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COMMON_MODBUS_TASKS__RESPONSE_ON_DISPLAY_READ;
//         }
//         // set exceptions codes to register bank
//         LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
//         RESCHEDULE(100);
//      }
//   }
//   #endif               
//}
//#endif
//#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
//#endif
