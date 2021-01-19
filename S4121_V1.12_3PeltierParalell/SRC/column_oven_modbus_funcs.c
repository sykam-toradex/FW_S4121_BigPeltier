/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_modbus_funcs.c
*
*  \brief               This file handles the modbus communication. It begins with initialization of modbus for RS232 and SPI.
*                       Afterwards a trigger filter is set to receive and execute modbus commands. Be careful if
*                       setting other trigger conditions. \n
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
#include "..\..\shared\src\common_modbus_funcs.h"
#include "..\..\shared\src\common_registers.h"
#include "column_oven_registers.h"
#include "column_oven_cmds_owner.h"
#include "column_oven_cmds_user.h"
#include "column_oven_cmds_reg.h"
#include "column_oven_modbus_funcs.h"
#include "column_oven_modbus_tasks.h"
#include "column_oven_support_tasks.h"
//---------------------------------------------- global declarations/definitions ---------------------------------------------------------------
OS_TID   TID_ColumnOvenModbusFuncs_CmdExecution;                        /**<  OS_TID = UINT = U32 = task identifier */
os_mbx_declare(MBX_ColumnOvenModbusFuncs_CmdExecution, 500);             /**<  define(!) mailbox for modbus command execution task -
                                                                              defines mailbox object (arrays for intertask communication)
                                                                              for \ref TSK_ColumnOvenModbusFuncs_CmdExecution with size
                                                                              of (20 + 4) * [4 byte] */
OS_TSK_STACK_USER(STK_TSK_ColumnOvenModbusFuncs_CmdExecution, 8000);    /**<  define stack for \ref TSK_ColumnOvenModbusFuncs_CmdExecution
                                                                              in [bytes] */
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern const   ushort   cu16_TimeoutLong;
extern const   ushort   cu16_TimeoutMedium;
//---------------------------------------------- functions -------------------------------------------------------------------------------------
ulong ColumnOvenModbusFuncs_InitDriver (void)
{
   struct   _SMBPBCfgDriver  S_CfgDriver[MBPB_PORT_COUNT];
   uchar    u8_ModbusSlaveID;
   ulong    u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // configure modbus for SPI slave -> receiver timeouts are defined by MBPB_TIMEOUT... in mbpbcfg.h
   S_CfgDriver[MBPB_INDEX_SPI].ucUnit                          = 1;
   S_CfgDriver[MBPB_INDEX_SPI].ucChannel                       = 0;
   S_CfgDriver[MBPB_INDEX_SPI].ulBaud                          = 0;
   S_CfgDriver[MBPB_INDEX_SPI].ulOption                        = (AT91C_SPI_BITS_8 | AT91C_SPI_NCPHA);
   S_CfgDriver[MBPB_INDEX_SPI].u16To                           = cu16_TimeoutMedium;
   S_CfgDriver[MBPB_INDEX_SPI].bRTU                            = FALSE;
   S_CfgDriver[MBPB_INDEX_SPI].bDisabled                       = FALSE;
   S_CfgDriver[MBPB_INDEX_SPI].STranslator[MBPB_MODE_ASC].Rx   = NULL;
   S_CfgDriver[MBPB_INDEX_SPI].STranslator[MBPB_MODE_ASC].Tx   = NULL;
   S_CfgDriver[MBPB_INDEX_SPI].STranslator[MBPB_MODE_RTU].Rx   = NULL;
   S_CfgDriver[MBPB_INDEX_SPI].STranslator[MBPB_MODE_RTU].Tx   = NULL;
   // configure modbus for RS232 -> receiver timeouts are defined by MBPB_TIMEOUT... in mbpbcfg.h
   S_CfgDriver[MBPB_INDEX_RS232].ucUnit                        = 0;
   S_CfgDriver[MBPB_INDEX_RS232].ucChannel                     = 0;
   S_CfgDriver[MBPB_INDEX_RS232].ulBaud                        = (ulong)AT91SAM7X__BAUDRATE_RS232;
   S_CfgDriver[MBPB_INDEX_RS232].ulOption                      = (ulong)(AT91C_US_USMODE_HWHSH | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT);
   S_CfgDriver[MBPB_INDEX_RS232].u16To                         = cu16_TimeoutMedium;
   S_CfgDriver[MBPB_INDEX_RS232].bRTU                          = FALSE;
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   S_CfgDriver[MBPB_INDEX_RS232].bDisabled                     = FALSE;
#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_AUTOSAMPLER
   S_CfgDriver[MBPB_INDEX_RS232].bDisabled                     = TRUE;
#endif
   S_CfgDriver[MBPB_INDEX_RS232].STranslator[MBPB_MODE_ASC].Rx = NULL;
   S_CfgDriver[MBPB_INDEX_RS232].STranslator[MBPB_MODE_ASC].Tx = NULL;
   S_CfgDriver[MBPB_INDEX_RS232].STranslator[MBPB_MODE_RTU].Rx = NULL;
   S_CfgDriver[MBPB_INDEX_RS232].STranslator[MBPB_MODE_RTU].Tx = NULL;
   // configure modbus for RS485 -> receiver timeouts are defined by MBPB_TIMEOUT... in mbpbcfg.h
   S_CfgDriver[MBPB_INDEX_RS485].ucUnit                        = 1;
   S_CfgDriver[MBPB_INDEX_RS485].ucChannel                     = 0;
   S_CfgDriver[MBPB_INDEX_RS485].ulBaud                        = (ulong)AT91SAM7X__BAUDRATE_RS485;
   S_CfgDriver[MBPB_INDEX_RS485].ulOption                      = (ulong)(AT91C_US_USMODE_RS485 | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT);
   S_CfgDriver[MBPB_INDEX_RS485].u16To                         = cu16_TimeoutMedium;
   S_CfgDriver[MBPB_INDEX_RS485].bRTU                          = FALSE;
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   S_CfgDriver[MBPB_INDEX_RS485].bDisabled                     = FALSE;
#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_AUTOSAMPLER
   S_CfgDriver[MBPB_INDEX_RS485].bDisabled                     = TRUE;
#endif
   S_CfgDriver[MBPB_INDEX_RS485].STranslator[MBPB_MODE_ASC].Rx = NULL;
   S_CfgDriver[MBPB_INDEX_RS485].STranslator[MBPB_MODE_ASC].Tx = NULL;
   S_CfgDriver[MBPB_INDEX_RS485].STranslator[MBPB_MODE_RTU].Rx = NULL;
   S_CfgDriver[MBPB_INDEX_RS485].STranslator[MBPB_MODE_RTU].Tx = NULL;
   // initialize modbus for RS232, RS485 and SPI
   if (SUCCESS (MBPBInit (S_CfgDriver, &AT91SAM7X_FilterForModbusRequests, OS_T_MS(200)), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__INIT))
   {
      // set the slave identifier (none standard modbus)
      // MSB defines if modbus translating is active (1 == active)
      // bits 6-4 are used for routing between sykam devices;
      // bits 3-0 are used as slave identifier (own device address)
      u8_ModbusSlaveID = (uchar)AT91SAM7X__MODBUS_DEFAULT_SLAVE_ID;
      if (SUCCESS (MBPBProductAddrSet (u8_ModbusSlaveID, cu16_TimeoutLong),
               u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_ADDRESS_SET))
      {
         // set the product state to "off" (modbus is always active after initialization - this is just a state flag to be read out with modbus cmd 0x11)
         TRY_AND_CATCH  (MBPBProductStateSet (FALSE, cu16_TimeoutLong), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_STATE_SET);
      }
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MODBUS_FUNCS__INIT_DRIVER);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenModbusFuncs_InitTasks (void)
{
   ulong u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // initialize mailbox "MBX_ColumnOvenModbusFuncs_CmdExecution"
   os_mbx_init (MBX_ColumnOvenModbusFuncs_CmdExecution, sizeof(MBX_ColumnOvenModbusFuncs_CmdExecution));
   // Set a trigger filter to receive and filter commands from MBPB. Settings:
   // watch holding registers, create a new filter, filter is enabled, trigger on write actions, trigger on addresses ...
   // and on incoming values ..., timeout value ... for MBX and the function itself
   if (FAIL (MBPBTrgFilterSetEx  ((uchar)MBPB_TRGTYPE_HOLDING_WRITE, (ushort)NULL, TRUE, (uchar)(MBPB_TRGMSG_RWMASK_WRITE |
                  MBPB_TRGMSG_RWMASK_VALZERO),
               (ushort)COLUMN_OVEN_MODBUS_FUNCS__MODBUS_ADDR_MASK__COLUMN_OVEN_CMDS,
               (ushort)COLUMN_OVEN_MODBUS_FUNCS__MODBUS_VAL_MASK__COLUMN_OVEN_CMDS,
               (ushort)COLUMN_OVEN_MODBUS_FUNCS__MODBUS_ADDR_MIN__COLUMN_OVEN_CMDS,
               (ushort)COLUMN_OVEN_MODBUS_FUNCS__MODBUS_ADDR_MAX__COLUMN_OVEN_CMDS,
               (ushort)COLUMN_OVEN_MODBUS_FUNCS__MODBUS_VAL_MIN__COLUMN_OVEN_CMDS,
               (ushort)COLUMN_OVEN_MODBUS_FUNCS__MODBUS_VAL_MAX__COLUMN_OVEN_CMDS,
               MBX_ColumnOvenModbusFuncs_CmdExecution,
               cu16_TimeoutLong, cu16_TimeoutLong),
            u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_FILTER_SET))
   {
      u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MODBUS_FUNCS__INIT_TASKS);
   }
   // create tasks with user defined stack
   TID_ColumnOvenModbusFuncs_CmdExecution = os_tsk_create_user (TSK_ColumnOvenModbusTasks_CmdExecution, (uchar)OS_P_NORMAL,
   (void *)STK_TSK_ColumnOvenModbusFuncs_CmdExecution,
   (ushort)sizeof(STK_TSK_ColumnOvenModbusFuncs_CmdExecution));
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
bool ColumnOvenModbusFuncs_FilterForModbusRequests  (uchar u8_ModbusDataType, ushort u16_ModbusAddress, ushort *pu16_ModbusData)
{
   bool  b_Result = (bool)TRUE;
   // Caution: Driver context!
   // Default action is TRUE to accept incoming write requests on holding register or coils.
   // Setting the b_Result to FALSE declines the request for u16_ModbusAddress and does NOT change the holding register or coil.
   // Furthermore an exception message will be send back to the requester.
   if (pu16_ModbusData)
   {
      // Currently only holding type requires user validation (other write requests are already validated by modbus)
      if (u8_ModbusDataType == (uchar)MBPB_REGTYPE_HOLDING)
      {
         switch (u16_ModbusAddress)
         {
            // column oven device params filter
//         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_OFFSET:
//            {
//               if (*pu16_ModbusData > (ushort)AT91SAM7X__MAX_TEMPERATURE_OFFSET_MUL_TEN)
//               b_Result = (bool)FALSE;
//               break;
//            }
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_UNIT:
            {
               if (     (*pu16_ModbusData == (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__NONE)
                     || (*pu16_ModbusData >= (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNITS_MAX)
                     )
               {
                  b_Result = (bool)FALSE;
               }
               break;
            }
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND:
            {
               if (     (*pu16_ModbusData < (ushort)AT91SAM7X__MIN_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN)
                     || (*pu16_ModbusData > (ushort)AT91SAM7X__MAX_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN)
                     )
               {
                  b_Result = (bool)FALSE;
               }
               break;
            }
            // column oven OWNER command argument filter
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__OCMD_CONTROL_SECTIONS_PARAMS_SAVE:
            {
               if (*pu16_ModbusData != (ushort)COLUMN_OVEN_CMDS_OWNER__CMD_ARG__CONTROL_SECTION_PARAMS_SAVE)
               b_Result = (bool)FALSE;
               break;
            }
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__OCMD_DEVICE_PARAMS_LOAD:
            {
               if (*pu16_ModbusData != (ushort)COLUMN_OVEN_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD)
               b_Result = (bool)FALSE;
               break;
            }
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__OCMD_STATUS_DATA_LOAD:
            {
               if (*pu16_ModbusData != (ushort)COLUMN_OVEN_CMDS_OWNER__CMD_ARG__STATUS_DATA_LOAD)
               b_Result = (bool)FALSE;
               break;
            }
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__OCMD_HEAT_COOL_PROGRAMS_RESET:
            {
               if (*pu16_ModbusData != (ushort)COLUMN_OVEN_CMDS_OWNER__CMD_ARG__HEAT_COOL_PROGRAMS_RESET)
               b_Result = (bool)FALSE;
               break;
            }
            // column oven USER command argument filter
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_PROGRAM_STOP:
            {
               if (*pu16_ModbusData != (ushort)COLUMN_OVEN_CMDS_USER__CMD_ARG__HEAT_COOL_PROGRAM_STOP)
               b_Result = (bool)FALSE;
               break;
            }
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_PROGRAM_DELETE:
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_PROGRAM_SAVE:
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_PROGRAM_SELECT_TO_MODIFY:
            {
               if (     (*pu16_ModbusData > (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID)
                     || (*pu16_ModbusData == (ushort)0x0000)
                     )
               {
                  b_Result = (bool)FALSE;
               }
               break;
            }
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_CALIBRATION_START:
            {
               if (*pu16_ModbusData != (ushort)COLUMN_OVEN_CMDS_USER__CMD_ARG__HEAT_COOL_CALIBRATION_START)
               b_Result = (bool)FALSE;
               break;
            }
         case (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_ENABLED:
            {
               if (     (*pu16_ModbusData != (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_ENABLED)
                     && (*pu16_ModbusData != (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_DISABLED)
                     )
               {
                  b_Result = (bool)FALSE;
               }
               break;
            }
         }
      }
   }
   return b_Result;
}
