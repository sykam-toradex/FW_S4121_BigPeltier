/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                lib_exceptions_funcs.c
*
*  \brief               This file contains a support function to set exceptions to register bank.
*
*
*  \author              Ruediger Radtke
*  \version             1.1
*
*  \date                2011-07-20     Initial version.c.
*  \date                2012-08-21     <em>File version 1.1</em>. Changed function argument for \ref CommonSupportFuncs_ErrorStringSW_FindAndSet.
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

//#include <stdio.h>
//#include <stdlib.h>
#include <AT91SAM7X512.H>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\..\cpu\at91\usart\usartext.h"

#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"

#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"

#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"

#include "lib_exceptions.h"
#include "common_registers.h"
#include "common_support_funcs.h"


//---------------------------------------------- externals used in this file -------------------------------------------------------------------

extern            OS_TID   TID_UtilsFuncs_Heartbeat;

extern   const    struct   _S_LibExceptions_ErrorStringsSW  cSA_LibExceptions_ErrorStringsSW[LIB_EXCEPTIONS__ERROR_STRINGS_SW_STRUCT_ENTRIES_MAX];
extern   const    ushort   cu16_UtilsFuncs_TimeoutShort;

//---------------------------------------------- functions -------------------------------------------------------------------------------------

void LibExceptionsFuncs_ExceptionSet (tpS_LibExceptions_Exceptions pS_Exceptions)
{
   ulong    u32_ErrorCodeSW_Sub  = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   ushort   u16_ErrorCnt;
   ushort   u16_NumErrorsToSet;
   ushort   u16_ErrorCodeSW1;
   ushort   u16_ErrorCodeSW2;
   ushort   u16_RegAddress1;
   ushort   u16_RegAddress2;
   ushort   u16_NumRegsToRW;
   ushort   u16_Idx;
   struct   _S_ErrorCodeSW_Sub{ ushort u16_ErrorCode; }  S_ErrorCodeSW_Sub[2];
//   ushort   u16_DeviceState;

//   char     *ps8_UsartTx;
//   uchar    u8_UsartTxSize = 200;   // [byte(s)]

#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY    
   uchar    u8_Unit = 0;
#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif
   
   
   // check if any base error code exists
   if ((pS_Exceptions->u16_ErrorCodeSW_Base) && (pS_Exceptions->u32_ErrorCodeSW_Sub))
   {
      u16_NumRegsToRW = 1;

//      // set new device state to show that an error occured
//      u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__ERROR;
//      
//      if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                                                               u32_ErrorCodeSW_Sub))
//      {
         // check if there is some free space in register bank to write at least one software error and one internal software error
         if (SUCCESS ( MBPBRegAccess((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, 
                                     (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW, u16_NumRegsToRW, 
                                     (void *)(&u16_ErrorCnt), NULL, cu16_UtilsFuncs_TimeoutShort),
                       u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            // check if software base and sub errors spaces are available inside register bank
            if (u16_ErrorCnt <= (ushort)LIB_EXCEPTIONS__ERROR_CNT_MAX_TO_SET_ENTRY)
            {
               if (SUCCESS_APP_FUNC (CommonSupportFuncs_ErrorStringSW_FindAndSet (pS_Exceptions), u32_ErrorCodeSW_Sub))
               {
                  // get the amount of sub errors before software error codes are set
                  GET_BITS_ULONGHX  (pS_Exceptions->u32_ErrorCodeSW_Sub, S_ErrorCodeSW_Sub[1].u16_ErrorCode);  // sub error2
                  GET_BITS_ULONGLX  (pS_Exceptions->u32_ErrorCodeSW_Sub, S_ErrorCodeSW_Sub[0].u16_ErrorCode);  // sub error1

                  if (S_ErrorCodeSW_Sub[1].u16_ErrorCode)
                     u16_NumErrorsToSet = 2;
                  else
                     u16_NumErrorsToSet = 1;

                  u16_NumRegsToRW = 1;

                  // set the software base errors and sub errors pairwise
                  for (u16_Idx = 0; u16_Idx < u16_NumErrorsToSet; u16_Idx++)
                  {                     
                     if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE,
                                                 (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB1 + (u16_ErrorCnt/2), 
                                                  u16_NumRegsToRW, (void *)(&S_ErrorCodeSW_Sub[u16_Idx].u16_ErrorCode), 
                                                  NULL, cu16_UtilsFuncs_TimeoutShort),
                                   u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                     {
                        if ((u16_NumErrorsToSet > 1) && (u16_Idx == 0))
                           u16_ErrorCodeSW1 = 0;
                        else
                           u16_ErrorCodeSW1 = pS_Exceptions->u16_ErrorCodeSW_Base;

                        // set software base error code (if two software sub errors exist the lower number space of base error 
                        // in register bank is equal to zero)
                        if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE,
                                                    (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE1 + (u16_ErrorCnt/2), 
                                                     u16_NumRegsToRW, (void *)(&u16_ErrorCodeSW1), NULL, cu16_UtilsFuncs_TimeoutShort),
                                     u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                        {
                           // Increment software base error counter about two. Software base errors and sub 
                           // errors must occur pairwise in register bank.
                           u16_ErrorCnt += 2;

                           if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, 
                                                       (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW, u16_NumRegsToRW, 
                                                       (void *)(&u16_ErrorCnt), NULL, cu16_UtilsFuncs_TimeoutShort),
                                        u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                           {
                              break;
                           }
                        }
                        else
                           break;
                     }
                     else
                        break;
                  }
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
                  // set MCP2317_NSYS_ERROR low as long as no reset is executed (level = second argument)
                  TRY_AND_CATCH
                  (
                     Mcp2317SetOutput (u8_Unit, (ushort)AT91SAM7X__MCP2317_N_SYS_ERROR, (ushort)~AT91SAM7X__MCP2317_N_SYS_ERROR,
                                       (ushort)AT91SAM7X__TICK_100),
                     u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MCP2317__SET_OUTPUT
                  );
#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif
               } 
            }
         }
//      }
   }
   // Only software base error or sub error(s) are available. (This case should never happen!)
   else if (   ((!pS_Exceptions->u16_ErrorCodeSW_Base) && (pS_Exceptions->u32_ErrorCodeSW_Sub))
            || ((pS_Exceptions->u16_ErrorCodeSW_Base) && (!pS_Exceptions->u32_ErrorCodeSW_Sub)) )
   {
//      // set new device state to show that an error occured
//      u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__ERROR;
//      
//      if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                                                               u32_ErrorCodeSW_Sub))
//      {
         u16_NumRegsToRW = 1;
         // check if there is some free space in register bank to write at least one software base and one software error
         if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, 
                                     (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW, u16_NumRegsToRW, 
                                     (void *)(&u16_ErrorCnt), NULL, cu16_UtilsFuncs_TimeoutShort),
                      u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            // check if software base and sub errors spaces are available inside register bank
            if (u16_ErrorCnt <= (ushort)LIB_EXCEPTIONS__ERROR_CNT_MAX_TO_SET_ENTRY)
            {
               // set special error string
               if (SUCCESS_APP_FUNC (CommonSupportFuncs_ErrorStringSW_FindAndSet (pS_Exceptions), u32_ErrorCodeSW_Sub))
               {
                  if ((!pS_Exceptions->u16_ErrorCodeSW_Base) && (pS_Exceptions->u32_ErrorCodeSW_Sub))
                  {
                     u16_RegAddress1   =  (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE1;
                     u16_RegAddress2   =  (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB1;
                     u16_ErrorCodeSW1  =  0xFFFF;
                     u16_ErrorCodeSW2  =  (ushort)pS_Exceptions->u32_ErrorCodeSW_Sub; // though an unexpected error occurred set only one sub code
                  }
                  else if ((pS_Exceptions->u16_ErrorCodeSW_Base) && (!pS_Exceptions->u32_ErrorCodeSW_Sub))
                  {
                     u16_RegAddress1   =  (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB1;
                     u16_RegAddress2   =  (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE1;
                     u16_ErrorCodeSW1  =  0xFFFF;
                     u16_ErrorCodeSW2  =  pS_Exceptions->u16_ErrorCodeSW_Base; // though an unexpected error occurred set only one base code
                  }
                  else  // this case should never happen!!!
                     u32_ErrorCodeSW_Sub = ((ushort)LIB_EXCEPTIONS__ERROR__LIB_EXCEPTIONS_FUNCS__EXCEPTION_SET);


                  // Set a special software base error code to free register bank entry
                  if (SUCCESS   (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, u16_RegAddress1 + (u16_ErrorCnt/2), 
                                                 u16_NumRegsToRW, (void *)(&u16_ErrorCodeSW1), NULL, cu16_UtilsFuncs_TimeoutShort),
                                 u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                  {
                     if (SUCCESS   (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, u16_RegAddress2 + (u16_ErrorCnt/2), 
                                                    u16_NumRegsToRW, (void *)(&u16_ErrorCodeSW2), NULL, cu16_UtilsFuncs_TimeoutShort),
                                    u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                     {

                        // Increment software base error counter about two. Software base errors and sub errors must occur
                        // pairwise in register bank.
                        u16_ErrorCnt += 2;
                        if (SUCCESS ( MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, 
                                                     (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW, 
                                                      u16_NumRegsToRW, (void *)(&u16_ErrorCnt), NULL, cu16_UtilsFuncs_TimeoutShort),
                                      u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                        {
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY                        
                           // set MCP2317_NSYS_ERROR low as long as no reset is executed (level = second argument)
                           TRY_AND_CATCH
                           (
                              Mcp2317SetOutput (u8_Unit, (ushort)AT91SAM7X__MCP2317_N_SYS_ERROR, (ushort)~AT91SAM7X__MCP2317_N_SYS_ERROR,
                                                (ushort)AT91SAM7X__TICK_100),
                              u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MCP2317__SET_OUTPUT
                           );
#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif
                        }
                     }
                  }
               }
            }
         }
//      }
   }
   // reset of structure is necessary!
   memset ((void *)(pS_Exceptions), 0, (size_t)sizeof( tS_LibExceptions_Exceptions));

   if (u32_ErrorCodeSW_Sub)
   {
//      os_tsk_delete ((OS_TID)TID_UtilsFuncs_Heartbeat);
//      GET_BITS_ULONGHX  (pS_Exceptions->u32_ErrorCodeSW_Sub, S_ErrorCodeSW_Sub[1].u16_ErrorCode);  // sub error2
//      GET_BITS_ULONGLX  (pS_Exceptions->u32_ErrorCodeSW_Sub, S_ErrorCodeSW_Sub[0].u16_ErrorCode);  // sub error1
//      
//      // allocate memory for a temporary buffer
//      ps8_UsartTx = (char *)calloc ((size_t)u8_UsartTxSize, (size_t)sizeof(char));
//      if(ps8_UsartTx)
//      {
//         memset ((void *)ps8_UsartTx, 0, (size_t)u8_UsartTxSize);
//         // print string to temporary buffer and get amount written bytes
//         sprintf  ((char *)ps8_UsartTx, "Error occurred inside 'LibExceptionsFuncs_ExceptionsSet'!\rOriginal errors ->\rSoftware base error code\t= 0x%.4X\rSoftware sub error code 1\t= 0x%.4X\rSoftware sub error code 2\t= 0x%.4X\r\n", 
//                   pS_Exceptions->u16_ErrorCodeSW_Base, S_ErrorCodeSW_Sub[0].u16_ErrorCode, S_ErrorCodeSW_Sub[1].u16_ErrorCode);
//      }
//      u8_Unit = 0;
//      // debug info via usart if write to register bank fails  
//      UsartTx  (u8_Unit, (uchar)AT91SAM7X__DUMMY_ARG, (void *)ps8_UsartTx, (ushort)u8_UsartTxSize, OS_T_MS(500));
//      // free memory of temporary buffer
//      free (ps8_UsartTx);
   }

//#ifdef  OEM_LED
//                  AT91F_PIO_ClearOutput (AT91C_BASE_PIOA, AT91SAM7X__PIN__LED_RUN);// RUN OFF
//                  Mcp2317SetOutput (0, (ushort)AT91SAM7X__MCP2317_LED_READY, (ushort)~AT91SAM7X__MCP2317_LED_READY,100);//READY OFF
//                  AT91F_PIO_SetOutput (AT91C_BASE_PIOA, AT91SAM7X__PIN__LED_ERROR);// ERROR ON
//#endif

   
}
