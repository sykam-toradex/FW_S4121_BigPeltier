/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_modbus_funcs.c
*
*  \brief               This file handles the valve actuator modbus tasks initialization as well as modbus driver configuration
*                       and initialization. One modbus "trigger" filter is set-up to trigger corresponding modbus tasks. If any
*                       modbus command fits into address space the task \ref TSK_ValveActModbusTasks_CmdExecution is called
*                       and consequently the desired command. \n
*                       Furthermore this file contains a modbus prefilter to handle write accesses to holding registers.
*                       This means if any invalid modbus data value is set by user/owner or service technician, it is 
*                       rejected by modbus prefilter. The prefilter must be configured by SW engineer.
*                       Every function returns a software base and sub error code which is greater than zero in any error case.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-11     Initial version derived from S4121_modbus_communication.c.
*  \date                2011-08-11     Renamed file from S4121_modbus_communication.c and included init function prototypes.
*  \date                2011-08-26     Added modbus prefilter function.
*  \date                2012-11-09     Removed modbus prefilter check for removed owner commands "ValveActCmdsOwner_CalDataLoad" 
*                                      and "ValveActCmdsOwner_CalDataLoad".
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

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbmsg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbdev.h"
#include "..\..\..\protocol\modbus\mbpbext.h"

#include "..\..\..\ext\misc\encgapdisk\encgapdisk.h"

#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"

#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\common_modbus_funcs.h"
#include "..\..\shared\src\common_registers.h"

#include "valve_act_cmds_owner.h"
#include "valve_act_cmds_user.h"
#include "valve_act_cmds_reg.h"
#include "valve_act_registers.h"
#include "valve_act_valvetypes.h"
#include "valve_act_modbus_funcs.h"
#include "valve_act_modbus_tasks.h"


//---------------------------------------------- global declarations/definitions ---------------------------------------------------------------

OS_TID   TID_ValveActModbusFuncs_CmdExecution;                        /**< OS_TID = UINT = U32 = task identifier */

os_mbx_declare(MBX_ValveActModbusFuncs_CmdExecution, 100);             /**< define(!) mailbox for modbus command execution task -
                                                                           defines mailbox object (arrays for intertask communication) 
                                                                           for \ref TSK_ValveActModbusTasks_CmdExecution with size 
                                                                           of (20 + 4) * [4 byte] */

OS_TSK_STACK_USER(STK_TSK_ValveActModbusFuncs_CmdExecution, 2000);    /**< define stack for \ref TSK_ValveActModbusTasks_CmdExecution 
                                                                           in [bytes] - access to engapdisk structures requires appr. 
                                                                           1300 byte stack */ 

//---------------------------------------------- externals used in this file -------------------------------------------------------------------

extern const   ushort   cu16_TimeoutLong;
extern const   ushort   cu16_TimeoutMedium;

//---------------------------------------------- functions -------------------------------------------------------------------------------------

ulong ValveActModbusFuncs_InitDriver (void) 
{
   struct   _SMBPBCfgDriver  S_CfgDriver[MBPB_PORT_COUNT];  // MBPB_PORT_COUNT = 5 -> configuration structure for several drivers

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
   S_CfgDriver[MBPB_INDEX_RS232].bDisabled                     = FALSE;
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
   S_CfgDriver[MBPB_INDEX_RS485].bDisabled                     = FALSE;
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
         // set the product state to "off" (modbus is allways active after initialization - this is just a state flag to be read out with modbus cmd 0x11)
         TRY_AND_CATCH  (MBPBProductStateSet (FALSE, cu16_TimeoutLong), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_STATE_SET);
      }

   }
  
   if(u32_ErrorCodeSW_Sub)
      u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MODBUS_FUNCS__INIT_DRIVER);


   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong ValveActModbusFuncs_InitTasks (void)
{
   ulong u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;


   // initialize mailbox "MBX_ValveActModbusFuncs__CmdExecution"
   os_mbx_init( MBX_ValveActModbusFuncs_CmdExecution, sizeof(MBX_ValveActModbusFuncs_CmdExecution) );

   // Set a trigger filter to receive common commands via modbus (MBPB).
   // Settings:
   // Watch holding registers, create a new filter, filter is enabled, trigger on write actions, trigger on addresses 
   // inside min-max mask, trigger on addresses inside min-max (must be inside mask), trigger on values inside min-max
   // mailbox for tasks which receives a mail in case a command was send, timeout value for MBX and the function itself.
   if( FAIL ( MBPBTrgFilterSetEx ((uchar)MBPB_TRGTYPE_HOLDING_WRITE, (ushort)NULL, TRUE, (uchar)(MBPB_TRGMSG_RWMASK_WRITE | 
                                   MBPB_TRGMSG_RWMASK_VALZERO), 
                                  (ushort)VALVE_ACT_MODBUS_FUNCS__MODBUS_ADDR_MASK__VALVE_ACT_CMDS, 
                                  (ushort)VALVE_ACT_MODBUS_FUNCS__MODBUS_VAL_MASK__VALVE_ACT_CMDS, 
                                  (ushort)VALVE_ACT_MODBUS_FUNCS__MODBUS_ADDR_MIN__VALVE_ACT_CMDS,
                                  (ushort)VALVE_ACT_MODBUS_FUNCS__MODBUS_ADDR_MAX__VALVE_ACT_CMDS,
                                  (ushort)VALVE_ACT_MODBUS_FUNCS__MODBUS_VAL_MIN__VALVE_ACT_CMDS,
                                  (ushort)VALVE_ACT_MODBUS_FUNCS__MODBUS_VAL_MAX__VALVE_ACT_CMDS,
                                   MBX_ValveActModbusFuncs_CmdExecution, 
                                   cu16_TimeoutLong, cu16_TimeoutLong ),
              u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_FILTER_SET ) )
   {
      u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MODBUS_FUNCS__INIT_TASKS);
   }

   // create tasks with user defined stack
   TID_ValveActModbusFuncs_CmdExecution = os_tsk_create_user   (TSK_ValveActModbusTasks_CmdExecution, (uchar)OS_P_NORMAL, 
                                                                (void *)STK_TSK_ValveActModbusFuncs_CmdExecution,
                                                                (ushort)sizeof(STK_TSK_ValveActModbusFuncs_CmdExecution));


   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

bool ValveActModbusFuncs_FilterForModbusRequests  (uchar u8_ModbusDataType, U32 u32_ModbusAddress, ushort *pu16_ModbusData)
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
         switch (u32_ModbusAddress)
         {
            // valve actuator device params filter
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_MODE:
            {
               if (*pu16_ModbusData > (ushort)ENCGAPDISK0_CONTINUOUS)
                  b_Result = (bool)FALSE;

               break;
            }            
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_EDGE_TYPE_TRIGGERED_ON:
            {
               if (*pu16_ModbusData > (ushort)ENCGAPDISK_EDGETYPE_MAX - 1)
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DAC_HOLD_MOTOR_DRIVER:
            {
               if ((*pu16_ModbusData < (ushort)AT91SAM7X__STPCLKDIR0_DAC_MIN) && (*pu16_ModbusData > (ushort)AT91SAM7X__STPCLKDIR0_DAC_MAX))
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DAC_RUN_MOTOR_DRIVER:
            {
               if ((*pu16_ModbusData < (ushort)AT91SAM7X__STPCLKDIR0_DAC_MIN) && (*pu16_ModbusData > (ushort)AT91SAM7X__STPCLKDIR0_DAC_MAX))
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE:
            {
               if (*pu16_ModbusData >= (ushort)VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER)
                  b_Result = (bool)FALSE;

               break;
            }
            
            // valve actuator OWNER command argument filter
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_LOAD:
            {
               if (*pu16_ModbusData != (ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD)
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_LOAD_DEFAULT:
            {
               if (*pu16_ModbusData != (ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD_DEFAULT)
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_SAVE:
            {
               if (*pu16_ModbusData != (ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_SAVE)
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__STATUS_DATA_LOAD:
            {
               if (*pu16_ModbusData != (ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__STATUS_DATA_LOAD)
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_WITH_DEVICE_PARAMS:
            {
               if (*pu16_ModbusData != (ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__CAL_WITH_DEVICE_PARAMS)
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_WITH_DEFAULT_PARAMS:
            {
               if (*pu16_ModbusData != (ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__CAL_WITH_DEFAULT_PARAMS)
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__RESET_TOTAL_SWITCH_COUNTER:
            {
               if (*pu16_ModbusData != (ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__RESET_TOTAL_SWITCH_COUNTER)
                  b_Result = (bool)FALSE;

               break;
            }
            
            // valve actuator USER command argument filter
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__GO_TO_HOME_POS:
            {
               if (*pu16_ModbusData != (ushort)VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_HOME_POS)
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__GO_TO_NEXT_POS:
            {
               if (*pu16_ModbusData != (ushort)VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_NEXT_POS)
                  b_Result = (bool)FALSE;

               break;
            }
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__GO_TO_PREVIOUS_POS:
            {
               if (*pu16_ModbusData != (ushort)VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_PREVIOUS_POS)
                  b_Result = (bool)FALSE;

               break;
            }            
            case (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__GO_TO_INQUIRED_POS:
            {
               if ((*pu16_ModbusData == 0) || (*pu16_ModbusData > (ushort)VALVE_ACT_VALVE_TYPES__MAX_VALVE_POSITIONS))
                  b_Result = (bool)FALSE;

               break;
            }
         }
      }
   }


   return b_Result;
}
