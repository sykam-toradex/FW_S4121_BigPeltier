/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_support_funcs.c
*
*  \brief               The file contains several support functions mostly used by commands functions.
*
*
*  \author              Ruediger Radtke
*  \version             1.2
*
*  \date                2011-08-23     Initial version.
*  \date                2012-08-21     <em>File version 1.1</em>. Changed function argument for \ref CommonSupportFuncs_ErrorStringSW_FindAndSet
*                                      and extended function to search for strings of a sub error code, in case no string is found 
*                                      for a base error code.
*  \date                2012-10-31     <em>File version 1.2</em>. Added check for array index end in function \ref CommonSupportFuncs_ErrorStringSW_FindAndSet.
*
*
*  \bug                 2012-10-31     No check for ending index of error strings array \ref cSA_LibExceptions_ErrorStringsSW in
*                                      function \ref CommonSupportFuncs_ErrorStringSW_FindAndSet.
*
*  \test                2011-09-06     Tested all available functions in this file for errors.
*  \test                2012-10-31     Tested ending index of error strings to result in an unknown error.
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

#include "..\..\..\ext\spi\nvram\fm25clss\fm25clssext.h"

#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"

#include "lib_exceptions.h"
#include "common_registers.h"
#include "common_support_funcs.h"
#include "common_support_tasks.h"
#include "utils_funcs.h"


//---------------------------------------------- global declarations/definitions ---------------------------------------------------------------

OS_TID   TID_CommonSupportFuncs_DeviceRunTimeCounter;
OS_TID   TID_CommonSupportFuncs_MainboardVoltageMonitoring;
OS_SEM   SEM_CommonSupportFuncs_DeviceRunTimeCounter;

OS_TSK_STACK_USER(STK_TSK_CommonSupportFuncs_DeviceRunTimeCounter, 1024);
OS_TSK_STACK_USER(STK_TSK_CommonSupportFuncs_MainboardVoltageMonitoring, 512);

//---------------------------------------------- externals used in this file -------------------------------------------------------------------

extern   const   ushort   cu16_UtilsFuncs_TimeoutShort;
extern   const   ushort   cu16_UtilsFuncs_TimeoutLong;

//---------------------------------------------- functions -------------------------------------------------------------------------------------

ulong CommonSupportFuncs_InitTasks (void)
{
   ulong u32_ErrorCodeSW_Sub = (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   
   // initialize semaphore with zero tokens
   os_sem_init (SEM_CommonSupportFuncs_DeviceRunTimeCounter, 0);

   // initialize run time counter task
   TID_CommonSupportFuncs_DeviceRunTimeCounter        =  os_tsk_create_user (TSK_CommonSupportTasks_DeviceRunTimeCounter, 
                                                                             (uchar)OS_P_BELOWNORMAL, 
                                                                             (void*)STK_TSK_CommonSupportFuncs_DeviceRunTimeCounter,
                                                                             (ushort)(sizeof(STK_TSK_CommonSupportFuncs_DeviceRunTimeCounter)));
   // initialize mainboard voltage monitoring task
   TID_CommonSupportFuncs_MainboardVoltageMonitoring  =  os_tsk_create_user (TSK_CommonSupportTasks_MainboardVoltageMonitoring, 
                                                                             (uchar)OS_P_BELOWNORMAL,
                                                                             (void*)STK_TSK_CommonSupportFuncs_MainboardVoltageMonitoring,
                                                                             (ushort)(sizeof(STK_TSK_CommonSupportFuncs_MainboardVoltageMonitoring)));				 
   return u32_ErrorCodeSW_Sub;   
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong CommonSupportFuncs_ErrorStringSW_FindAndSet (tS_LibExceptions_Exceptions *pS_Exceptions)
{
   uchar    u8_Count                =  0;
   ushort   u16_NumRegsToRW;
   ushort   u16_Idx                 =  0;
   ulong    u32_ErrorCodeSW;
   ulong    u32_ErrorCodeSW_Sub     =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   
   struct   _S_LibExceptions_ErrorStringsSW  S_ErrorStringSW;
   
      
   // Software base error code equal to zero is not allowed because corresponding string is an empty string.
   // Use function "CommonSupportFuncs_ErrorStringSW_Clear" instead.
   if ((pS_Exceptions->u16_ErrorCodeSW_Base) || (pS_Exceptions->u32_ErrorCodeSW_Sub))
   {
      u32_ErrorCodeSW = (ulong)pS_Exceptions->u16_ErrorCodeSW_Base;
      
      // check if given software error code matches an software error code in cSA_LibExceptions_ErrorStringsSW
      while (u16_Idx < (ushort)LIB_EXCEPTIONS__ERROR_STRINGS_SW_STRUCT_ENTRIES_MAX)
      {
         if ((cSA_LibExceptions_ErrorStringsSW[u16_Idx].u16_ErrorCodeSW == u32_ErrorCodeSW) && (!((u16_Idx != 0) && (u32_ErrorCodeSW == 0))))
         {
            // clear local software error string
            memset ((void *)(&S_ErrorStringSW.u8A_ErrorStringSW[0]), 0, (ushort)LIB_EXCEPTIONS__ERROR_STRING_SW_CHARS_MAX);

            u16_NumRegsToRW = (ushort)LIB_EXCEPTIONS__ERROR_STRING_SW_CHARS_MAX / 2;

            // delete former software string from register bank
            if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, 
                                        (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_STRING_SW, u16_NumRegsToRW,
                                        (void *)(&S_ErrorStringSW.u8A_ErrorStringSW[0]), NULL, cu16_UtilsFuncs_TimeoutShort),
                         u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               // copy software error string found in constant string array to local array for byte swap
               memcpy ((void *)(&S_ErrorStringSW.u8A_ErrorStringSW[0]), (void *)(&cSA_LibExceptions_ErrorStringsSW[u16_Idx].u8A_ErrorStringSW[0]), 
                       (ushort)LIB_EXCEPTIONS__ERROR_STRING_SW_CHARS_MAX);
            
               // swap string bytes to be able to read out it in left-right order from an extern host
               if (SUCCESS_APP_FUNC (UtilsFuncs_SwapBytes  ((void *)(&S_ErrorStringSW.u8A_ErrorStringSW[0]), u16_NumRegsToRW),
                                     u32_ErrorCodeSW_Sub))
               {
                  // set byte-swaped software error string to register bank
                  if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, 
                                              (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_STRING_SW, u16_NumRegsToRW,
                                              (void *)(&S_ErrorStringSW.u8A_ErrorStringSW[0]), NULL, cu16_UtilsFuncs_TimeoutShort),
                               u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                  {
                     break;      // function finished
                  }
                  else
                     break;   // error case
               }
               else
                  break;   // error case
            }
            else
               break;   // error case
         }
         u16_Idx++;

         // if no software error string was found, set a special string
         if (u16_Idx >= (ushort)LIB_EXCEPTIONS__ERROR_STRINGS_SW_STRUCT_ENTRIES_MAX)
         {
            u32_ErrorCodeSW         =  pS_Exceptions->u32_ErrorCodeSW_Sub & 0x0000FFFF;
            u8_Count++;
            u16_Idx                 =  0;
            
            if (u8_Count == 2)
            {
               if (pS_Exceptions->u32_ErrorCodeSW_Sub & 0xFFFF0000)
                  u32_ErrorCodeSW   =  (pS_Exceptions->u32_ErrorCodeSW_Sub >> 16) & 0x0000FFFF;  // second sub error code
               else
                  u8_Count++;
            }
            if (u8_Count > 2)
               u32_ErrorCodeSW      =  (ulong)(LIB_EXCEPTIONS__ERROR_SPECIAL__UNKNOWN_ERROR);
         }
      }
   }
   else
      return u32_ErrorCodeSW_Sub = (ulong)(LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__ERROR_STRING_SW_FIND_AND_SET);
   
   if  (u32_ErrorCodeSW_Sub)
      u32_ErrorCodeSW_Sub = ((u32_ErrorCodeSW_Sub << 16) | ((ulong)(LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__ERROR_STRING_SW_FIND_AND_SET)));


   return  u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong CommonSupportFuncs_ErrorCodeSW_ClearCurrPair (ushort *pu16_NumErrorsRemain)
{
   ushort   u16_NumRegsToRW         =  1;
   ushort   u16_ModbusAddress;
   ushort   u16_NumErrorsToDelete;
   ushort   u16_ErrorCnt;
   ushort   u16_ErrorCodeSW1;
   ushort   u16_Idx;
   ulong    u32_ErrorCodeSW_Sub     =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;


   *pu16_NumErrorsRemain = 0;
   
   // get software current error counter
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, 
                               (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW, u16_NumRegsToRW, 
                               (void *)(&u16_ErrorCnt), NULL, cu16_UtilsFuncs_TimeoutShort),
                u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS)  )
   {
      if ((u16_ErrorCnt >= 2) && (u16_ErrorCnt <= (ushort)LIB_EXCEPTIONS__ERROR_CNT_MAX))
      {
         if (u16_ErrorCnt >= 4)
         {
            u16_ModbusAddress = (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE1 + (u16_ErrorCnt/2) - 2;
         }
         // error count smaller than four (4)
         else
         {
            u16_ModbusAddress = (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE1 + (u16_ErrorCnt/2) - 1;
         }
         // read out (second) latest base error
         if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, u16_ModbusAddress,
                                      u16_NumRegsToRW, (void *)(&u16_ErrorCodeSW1), NULL, cu16_UtilsFuncs_TimeoutShort),
                      u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            // check if second latest base error code is equal to zero, if so, two corresponding software sub errors exist
            if (u16_ErrorCodeSW1 == 0)
               u16_NumErrorsToDelete = 2;
            else
               u16_NumErrorsToDelete = 1;

            u16_ErrorCodeSW1        =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;

            // delete two or one error pair(s) from register bank
            for (u16_Idx = 0; u16_Idx < u16_NumErrorsToDelete; u16_Idx++)
            {
               // clear current sub error(s)
               if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE,
                                           (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB1 + (u16_ErrorCnt/2) - 1, 
                                            u16_NumRegsToRW, (void *)(&u16_ErrorCodeSW1), NULL, cu16_UtilsFuncs_TimeoutShort),
                            u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS)  )
               {
                  // clear current base error(s) (twice the same if two software sub errors exist)
                  if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE,
                                              (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE1 + 
                                              (u16_ErrorCnt/2) - 1, u16_NumRegsToRW, (void *)(&u16_ErrorCodeSW1), 
                                               NULL, cu16_UtilsFuncs_TimeoutShort),
                               u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS) )
                  {
                     // Decrement software error counter about two. Software base errors and sub errors must occur pairwise in 
                     // register bank.
                     u16_ErrorCnt -= 2;

                     if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, 
                                                 (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW, 
                                                  u16_NumRegsToRW, (void *)(&u16_ErrorCnt), NULL, cu16_UtilsFuncs_TimeoutShort),
                                  u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS) )
                     {
                        *pu16_NumErrorsRemain = u16_ErrorCnt;
                     }
                  }
               }
            }
         }
      }
      else if(u16_ErrorCnt > (ushort)LIB_EXCEPTIONS__ERROR_CNT_MAX) // case that too many errors occurred
      {
         u16_ErrorCnt = (ushort)LIB_EXCEPTIONS__ERROR_CNT_MAX;
         
         TRY_AND_CATCH
         (
            MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE,
                           (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW, u16_NumRegsToRW, 
                           (void *)(&u16_ErrorCnt), NULL, cu16_UtilsFuncs_TimeoutShort),
            u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
         );
      }
   }

   if (u32_ErrorCodeSW_Sub)
      u32_ErrorCodeSW_Sub = ((u32_ErrorCodeSW_Sub << 16) | ((ushort)LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__ERROR_CODE_SW_CLEAR_CURR_PAIR));


   return   u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong CommonSupportFuncs_ErrorStringSW_Clear (void)
{
   ushort   u16_NumRegsToRW         =  (ushort)LIB_EXCEPTIONS__ERROR_STRING_SW_CHARS_MAX / 2;
   ulong    u32_ErrorCodeSW_Sub     =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;

   struct   _S_LibExceptions_ErrorStringsSW  S_ErrorStringSW;


   // clear local software error string
   memset ((void *)(&S_ErrorStringSW.u8A_ErrorStringSW[0]), 0, (ushort)LIB_EXCEPTIONS__ERROR_STRING_SW_CHARS_MAX);

   // delete former software string from register bank
   if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_STRING_SW,
                             u16_NumRegsToRW, (void *)(&S_ErrorStringSW.u8A_ErrorStringSW[0]), NULL, cu16_UtilsFuncs_TimeoutShort),
             u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      u32_ErrorCodeSW_Sub = ((u32_ErrorCodeSW_Sub << 16) | ((ulong)(LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__ERROR_STRING_SW_CLEAR)));
   }


   return  u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

//ulong CommonSupportFuncs_DeviceStateRW (ushort *pu16_DeviceState, bool b_RW)
//{
//   uchar    u8_Unit              =  0;
//   ushort   u16_NumRegs          =  1;
//   ushort   u16_FRAM_NumBytesRW  =  2;
//   ulong    u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
//   ushort   u16_Result;
//   ushort   u16_CmpBuffer[FM25CLSS_NUM_MULTI_STORAGES - 1];
//
//
//   switch (b_RW)
//   {
//      case COMMON_SUPPORT_FUNCS__READ:
//      {
//         if (pu16_DeviceState)   // for read accesses pointer must be available
//         {
//            u16_Result = Fm25clssReadSafe (u8_Unit, (void *)(pu16_DeviceState), (void *)(u16_CmpBuffer), u16_FRAM_NumBytesRW,
//                                           (ushort)COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATE, cu16_UtilsFuncs_TimeoutLong);
//
//            if (u16_Result != u16_FRAM_NumBytesRW)
//            {
//               u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__READ_SAFE;
//            }
//         }
//         else
//            return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__DEVICE_STATE_RW;
//
//         break;
//      }
//      
//      case COMMON_SUPPORT_FUNCS__WRITE:
//      {
//         if (*pu16_DeviceState < (uchar)COMMON_REGISTERS__DEVICE_STATES_COUNTER)
//         {
//            // set device state
//            if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATE, 
//                                        u16_NumRegs, (void *)(pu16_DeviceState), NULL, cu16_UtilsFuncs_TimeoutShort),
//                         u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//            {
//               u16_Result = Fm25clssWrite (u8_Unit, (void *)(pu16_DeviceState), u16_FRAM_NumBytesRW, 
//                                           (ushort)COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATE, cu16_UtilsFuncs_TimeoutLong);
//
//               if (u16_Result != u16_FRAM_NumBytesRW)
//               {
//                  u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
//               }
//            }
//         }
//         else
//            return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__DEVICE_STATE_RW;
//
//         break;
//      }
//      
//      default:
//         return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__DEVICE_STATE_RW;
//   }
//
//   if (u32_ErrorCodeSW_Sub)
//      u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__DEVICE_STATE_RW);
//
//
//   return u32_ErrorCodeSW_Sub;
//}

//----------------------------------------------------------------------------------------------------------------------------------------------
