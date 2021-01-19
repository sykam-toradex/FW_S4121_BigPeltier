/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_support_tasks.c
*
*  \brief               The file contains a task \ref TSK_ColumnOvenSupportTasks_GlobalTimer for calling temperature control task
*                       periodically with user settable tick time of \ref AT91SAM7X__REPETITION_TICKS_FOR_TSK_GLOBAL_TIMER.
*                       Furthermore it contains a task for handling extern control contact inquiries and a task for handling the
*                       column oven gas sensor (retrieving data, filter data, release an alarm if threshold is exceeded. \n
*
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2011-02-03     Initial version imported from S6510_valves.c.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                2012-12-14     Tested function \ref ColumnOvenSupportTasks_ExternControlContactHandler.
*
*  \todo
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <math.h>
#include <stdio.h>                              // temp for debugging
#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\cpu\at91\usart\usart.h"      // temp for debugging
#include "..\..\..\cpu\at91\usart\usartext.h"   // temp for debugging
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"
#include "..\..\..\cpu\at91\adc\adc.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\lib_exceptions_funcs.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\common_support_funcs.h"
#include "..\..\shared\src\utils_funcs.h"
#include "column_oven_registers.h"
#include "column_oven_cmds_user.h"
#include "column_oven_support_tasks.h"
#include "column_oven_signal_processing_funcs.h"
#include "column_oven_support_funcs.h"
#include "..\src\S6510\valve_act_cmds_user.h"
#include "..\..\..\cpu\at91\ADCLE\ADCLE.h"      // temp for debugging
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern            bool     b_ColumnOvenMain_InitDone;
extern            OS_SEM   SEM_ColumnOvenSupportFuncs_GasSensing;
extern            OS_TID   TID_ColumnOvenSignalProcessingFuncs_TemperatureControl;
extern   const    ushort   cu16_TimeoutShort;
extern   const    ushort   cu16_TimeoutLong;
//------------------------------------------------ tasks ---------------------------------------------------------------------------------------
// function runs in driver task context
void ColumnOvenSupportTasks_ExternControlContactHandler (U16 u16INTF, U16 u16INTCAP, U16 u16Pin, bool bLevel)
{
   //ushort   u16_DeviceState;
   ushort   u16_DeviceActiveInterface;
   ushort   u16_NumRegsToRW                           =  1;
   ushort   u16_SykamDeviceOptions;
   ushort   u16_HeatCoolProgramCurrentId              =  1;
   ushort   u16_HeatCoolProgramCurrentSectionSetPoint;
   ushort   u16_ColumnOvenState;
   ulong    u32_ErrorCodeSW_Sub                       =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   tS_LibExceptions_Exceptions S_Exceptions;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   // execute USER commands
   if (     (!bLevel)
         && (     (u16Pin == (ushort)AT91SAM7X__MCP2317_COLUMN_OVEN_START)
            || (u16Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_HOME)
            || (u16Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT)
            )
         )
   {
//      // read out device state
//      if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__READ),
//               S_Exceptions.u32_ErrorCodeSW_Sub))
//      {
//         // check if any command is currently executed, device detected an error or is in init state
//         if (     (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY)
//               && (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__INIT)
//               && (u16_DeviceState != (ushort)COMMON_REGISTERS__DEVICE_STATE__NONE))
//         {
//            // check for error state (shall not be overwritten)
//            if (u16_DeviceState == (ushort)COMMON_REGISTERS__DEVICE_STATE__ERROR)
//            u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__ERROR;
//            else
//            u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__BUSY;
//            // set new device state and therefore block execution of any other command as long as current command is executed
//            if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                     S_Exceptions.u32_ErrorCodeSW_Sub))
//            {
               // read out active device interface
               if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
                           (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_ACTIVE_INTERFACE,
                           u16_NumRegsToRW, (void *)(&u16_DeviceActiveInterface), NULL, cu16_TimeoutShort),
                        S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS) )
               {
                  // check if device interface is correct
                  if (u16_DeviceActiveInterface == (ushort)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__EXTERN_CONTROL_CONTACT)
                  {
                     if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START,
                                 (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_SykamDeviceOptions), NULL, cu16_TimeoutShort),
                              S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                     {
                        if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_CURRENT_ID,
                                    (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_HeatCoolProgramCurrentId), NULL, cu16_TimeoutShort),
                                 S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                        {
                           if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_CURRENT_SECTION_SET_POINT,
                                       (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_HeatCoolProgramCurrentSectionSetPoint), NULL, cu16_TimeoutShort),
                                    S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                           {
                              if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_STATE,
                                          (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_ColumnOvenState), NULL, cu16_TimeoutShort),
                                       S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                              {
                                 // check if valve actuator option is available - calibration is done with column selection valve
                                 if (     (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_VALVE_ACTUATOR)
                                       && (u16Pin != (ushort)AT91SAM7X__MCP2317_COLUMN_OVEN_START)
                                       )
                                 {
                                    // go to home position
                                    if (u16Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_HOME)
                                    {
                                       TRY_AND_CATCH_APP_FUNC
                                       (
                                       ValveActCmdsUser_GoToHomePos ((ushort)VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_HOME_POS, &S_Exceptions.u32_ErrorCodeSW_Sub),
                                       S_Exceptions.u16_ErrorCodeSW_Base
                                       );
                                    }
                                    // go to next position
                                    else if (u16Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT)
                                    {
                                       TRY_AND_CATCH_APP_FUNC
                                       (
                                       ValveActCmdsUser_GoToNextPos ((ushort)VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_NEXT_POS, &S_Exceptions.u32_ErrorCodeSW_Sub),
                                       S_Exceptions.u16_ErrorCodeSW_Base
                                       );
                                    }
                                 }
                                 else if (u16Pin == (ushort)AT91SAM7X__MCP2317_COLUMN_OVEN_START)
                                 {
                                    // start heat/cool program - change from "hold" to "run" mode now
                                    if (     (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
                                          && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                                          && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM)
                                          && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
                                          )
                                    {
                                       // check if user started oven manually (hold mode) - if not start directly into "run" mode (one extern impulse only!)
                                       if ((u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP) || (u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__FINISHED))
                                       u16_HeatCoolProgramCurrentId = u16_HeatCoolProgramCurrentId | (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_START_DIRECTLY;
                                       TRY_AND_CATCH_APP_FUNC
                                       (
                                       ColumnOvenCmdsUser_HeatCoolProgramStart (u16_HeatCoolProgramCurrentId, &S_Exceptions.u32_ErrorCodeSW_Sub),
                                       S_Exceptions.u16_ErrorCodeSW_Base
                                       );
                                    }
                                    // start heat/cool program - change from "stop" to "run" state now
                                    else if (   ((u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP) || (u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__FINISHED))
                                          && (     (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT)
                                             || (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
                                          && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL))
                                          && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM))
                                          )
                                    {
                                       TRY_AND_CATCH_APP_FUNC
                                       (
                                       ColumnOvenCmdsUser_HeatCoolProgramStart (u16_HeatCoolProgramCurrentSectionSetPoint, &S_Exceptions.u32_ErrorCodeSW_Sub),
                                       S_Exceptions.u16_ErrorCodeSW_Base
                                       );
                                    }
                                 }
                              }
                           }
                        }
                     }
//                     // set new device state
//                     u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__IDLE;
//                     TRY_AND_CATCH_APP_FUNC
//                     (
//                     CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                     u32_ErrorCodeSW_Sub
//                     );
                  }
                  else
                  S_Exceptions.u16_WarningCode = (ushort)LIB_EXCEPTIONS__WARNING_SPECIAL__DEVICE_ACTIVE_INTERFACE_WRONG;
               }
//            }
//         }
//         else
//         S_Exceptions.u16_WarningCode = (ushort)LIB_EXCEPTIONS__WARNING_SPECIAL__DEVICE_BUSY;
//      }
   }
   if ((S_Exceptions.u32_ErrorCodeSW_Sub) || (u32_ErrorCodeSW_Sub))
   {
      if (!S_Exceptions.u16_ErrorCodeSW_Base)
      {
         if ((!S_Exceptions.u32_ErrorCodeSW_Sub) || (u32_ErrorCodeSW_Sub))
         S_Exceptions.u32_ErrorCodeSW_Sub    =  u32_ErrorCodeSW_Sub;
         else if ((S_Exceptions.u32_ErrorCodeSW_Sub) && (u32_ErrorCodeSW_Sub))
         S_Exceptions.u32_ErrorCodeSW_Sub    =  (u32_ErrorCodeSW_Sub << 16) | S_Exceptions.u32_ErrorCodeSW_Sub;
         S_Exceptions.u16_ErrorCodeSW_Base   =  (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__EXTERN_CONTROL_CONTACT_HANDLER;
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
__task void TSK_ColumnOvenSupportTasks_GasSensing (void)
{
   //   static   char     s8_UsartDataTx[128];                         // debug purpose
   //   static   uchar    u8_Unit                       =  0;          // debug
   uchar    u8_Channel;
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_StateTemperatureSensor;
   ushort   u16_GasSensorCalibrationResistance;          // gas sensor value at 300ppm and 20°C in Ohms
   ushort   u16_GasSensorAlarmThresholdPPM;              // gas threshold concentraction in ppm to release an alarm
   ushort   u16_GasSensorAlarmEnabled;                   // 0 = disabled, 1 = enabled
   ushort   u16_StateGasSensor;                          // 1 = connected, 2 = disconnected, 3 = warm up
   ushort   u16_GasSensorAlarmDetected;                  // 0 = no alarm, 1 = alarm
   ushort   u16A_AdcData[AT91SAM7X__ADC_CHANNELS_MAX];   // array for reading ADC channels 0-7
   ulong    u32_ColumnOvenTemperature;                   // temperature in [°C]*10000
   ulong    u32_GasSensorCurrentResistance;              // resistance in Ohms
   ulong    u32_GasSensorCurrentPPM;
   float    f_GasSensorCurrentResistance;                // resistance in Ohms
   //float    f_Temp;
   //float    f_Kt;                                        // temperature correction factor
   tS_LibExceptions_Exceptions   S_Exceptions;
   static   bool        sb_TemperatureSensorFailureSet   =  FALSE;
   static   bool        sb_GasSensorFailureSet           =  FALSE;
   static   bool        sb_GasSensorWarmUpFlag           =  FALSE;
   static   ushort      su16_CycleCounter                =  0;
   static   ushort      su16_TimeoutSampleAndHold        =  (ushort)AT91SAM7X__TICK_1;
   static   ushort      su16_Timeout                     =  (ushort)AT91SAM7X__TICK_10;
   static   ulong       su32_TaskTicks                   =  0;
   static   ulong       su32_GasSensorWarmupTasksTicks   =  180000/ AT91SAM7X__TICK_500;// must be always 3 min!
//   static   ulong       su32_GasSensorWarmupTasksTicks   =  2000/ AT91SAM7X__TICK_500;//Reduce time for debugging
   static   AT91PS_ADC  spS_Adc                          =  AT91C_BASE_ADC;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   FOREVER
   {
      if (b_ColumnOvenMain_InitDone)
      {
         // read last enabled state from fixed memory
         TRY_AND_CATCH_APP_FUNC
         (
         UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u16_GasSensorAlarmEnabled), (ushort)AT91SAM7X__NUM_BYTES_2,
         (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_ENABLED, (bool)UTILS_FUNCS__READ),
         S_Exceptions.u32_ErrorCodeSW_Sub
         );
         // do not check for errors here because this can be the first time of read from fixed memory
         // check for never stored gas sensor params
         if (!(((ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_MASK_ALARM_DISABLED & u16_GasSensorAlarmEnabled) == ((ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_CODE_PARAMS_STORED)))
         {
            // set defaults for gas sensor params
            u16_GasSensorAlarmThresholdPPM      =  (ushort)COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_ALARM_THRESHOLD_PPM;
            u16_GasSensorCalibrationResistance  =  (ushort)COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_CALIBRATION_RESISTANCE;
            u16_GasSensorAlarmEnabled           =  (ushort)COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_ALARM_ENABLED;
            // set error code to indicate that gas sensor params are not stored (NO defaults here because
            // "calibration resistance" must be determined in calibration routine
            S_Exceptions.u32_ErrorCodeSW_Sub    =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_GAS_SENSOR_PARAMS_MISSING;
            S_Exceptions.u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
            // no go ahead with default params - error "missing gas sensor params" is set at every device start as long as
            // they are not stored
         }
         else
         {
            // read threshold ppm value
            u16_NumBytes      =  (ushort)AT91SAM7X__NUM_BYTES_2;
            u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_THRESHOLD_PPM;
            if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_GasSensorAlarmThresholdPPM), u16_NumBytes,
                        u16_DestAddress, (bool)UTILS_FUNCS__READ),
                     S_Exceptions.u32_ErrorCodeSW_Sub))
            {
               // read gas sensor calibration resistance value
               u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_CALIBRATION_RESISTANCE;
               if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_GasSensorCalibrationResistance),
                           u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
                        S_Exceptions.u32_ErrorCodeSW_Sub))
               {
               }
            }
            // if read of params failed, inform user and use defaults for task
            if (S_Exceptions.u32_ErrorCodeSW_Sub)
            {
               S_Exceptions.u32_ErrorCodeSW_Sub    =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_GAS_SENSOR_PARAMS_MISSING;
               S_Exceptions.u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING;
               LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
            }
         }
         // write enabled value to register bank
         if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_ENABLED,
                     (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_GasSensorAlarmEnabled), NULL, cu16_TimeoutShort),
                  S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
         }
         // write gas sensor threshold ppm value to register bank
         if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_THRESHOLD_PPM,
                     (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_GasSensorAlarmThresholdPPM), NULL, cu16_TimeoutShort),
                  S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
         }
         // write gas sensor calibration resistance to register bank
         if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_CALIBRATION_RESISTANCE,
                     (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_GasSensorCalibrationResistance), NULL, cu16_TimeoutShort),
                  S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
         }
         // check for initialized ADC channels
         if ((((uchar)AT91SAM7X__ADC_CHANNEL_GAS_SENSING | (uchar)AT91SAM7X__ADC_CHANNEL_GAS_HEAT_SENSE) & ~(uchar)spS_Adc->ADC_CHSR) == 0)
         {
            FOREVER
            {
               // read enabled state from register bank again (owner/service technician might disable sensor during run time)
               if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_ENABLED,
                           (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_GasSensorAlarmEnabled), NULL, su16_Timeout),
                        S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  if ((ushort)~COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_MASK_ALARM_DISABLED & u16_GasSensorAlarmEnabled)
                  {
                     // set channels to retrieve data from
                     u8_Channel = (uchar)AT91SAM7X__ADC_CHANNEL_GAS_SENSING | (uchar)AT91SAM7X__ADC_CHANNEL_GAS_HEAT_SENSE;
                     // retrieve ADC data
                     if (FAIL(AdcGetConvertedData (u8_Channel, &u16A_AdcData[0], su16_TimeoutSampleAndHold, cu16_TimeoutShort),
                              S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ADC__GET_CONVERTED_DATA))
                     {
                        S_Exceptions.u32_ErrorCodeSW_Sub=0;                        
                        if (FAIL(AdcGetConvertedData (u8_Channel, &u16A_AdcData[0], su16_TimeoutSampleAndHold, cu16_TimeoutShort),
                                 S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ADC__GET_CONVERTED_DATA))
                        {
                           S_Exceptions.u32_ErrorCodeSW_Sub=0;                           
                           if (SUCCESS (AdcGetConvertedData (u8_Channel, &u16A_AdcData[0], su16_TimeoutSampleAndHold, cu16_TimeoutShort),
                                    S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ADC__GET_CONVERTED_DATA))
                           {   
                              S_Exceptions.u32_ErrorCodeSW_Sub=0;                           
                           }  // ADC data retrieval                     
                        }
                     }
                        // check if gas sensor is connected or broken
                        if (
//                              (su32_TaskTicks > su32_GasSensorWarmupTasksTicks) &&
                              (
//                                 (u16A_AdcData[6] == 1023)  ||       // sensor resistance > 400k?  => sensor disconnected
                                 (u16A_AdcData[3] < 16)           // heat current < 50mA?       => sensor heat broken
                              )
                           )
                        {
                           // check for set failure code - error message is set once if sensor is disconnected
                           if (!sb_GasSensorFailureSet)
                           {
                              sb_GasSensorFailureSet              =  TRUE;
                              // set error "gas sensor failure" only once per device run
                              S_Exceptions.u32_ErrorCodeSW_Sub    =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_GAS_SENSOR_FAILURE;
                              S_Exceptions.u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING;
                              // set error codes to register bank
                              LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
                              // set oven state to "stop" once at device start if sensor is disconnected
                              if (u16_GasSensorAlarmDetected == (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_ALARM)
                              {
                                 u16_GasSensorAlarmDetected = (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_NO_ALARM;
                                 // set gas alarm detection state
                                 if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__GAS_SENSOR_ALARM_DETECTED,
                                             (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_GasSensorAlarmDetected), NULL, cu16_TimeoutShort),
                                          S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                                 {
                                    // set error codes to register bank
                                    LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
                                 }
                              }
                           }
                           // set new sensor state and show error only once
                           u16_StateGasSensor = (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__DISCONNECTED;
                           // Set Error at Phoenix Contact (PIN 7 -> LOW)
                           Mcp2317SetOutput(0, (ushort)MCP2317_PB7, (ushort)~MCP2317_PB7, OS_T_MS(500));
                           TRY_AND_CATCH
                           (
                           MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_GAS_SENSOR,
                           (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_StateGasSensor), NULL, cu16_TimeoutShort),
                           S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
                           );
                           // reset warm up time
                           su32_TaskTicks = 0;
                        }
                        // sensor hardware is working
                        else
                        {
                           // reset boolean failure set/warm up flag
                           sb_GasSensorFailureSet  =  FALSE;
                           sb_GasSensorWarmUpFlag  =  FALSE;
                           // warm up gas sensor
                           if (su32_TaskTicks < su32_GasSensorWarmupTasksTicks)
                           {
                              // set new sensor state
                              u16_StateGasSensor = (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__WARM_UP;
                              if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_GAS_SENSOR,
                                          (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_StateGasSensor), NULL, cu16_TimeoutShort),
                                       S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                              {
                              }
                           }
                           // if warm up time is oven set column oven state to "stop"
                           else if (su32_TaskTicks == su32_GasSensorWarmupTasksTicks)
                           {
                              // set new sensor state
                              u16_StateGasSensor = (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__CONNECTED;
                              TRY_AND_CATCH
                              (
                              MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_GAS_SENSOR,
                              (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_StateGasSensor), NULL, cu16_TimeoutShort),
                              S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
                              );
                           }
                           else
                           {
                              // read current column oven temperature value
                              if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_TEMPERATURE,
                                          (ushort)AT91SAM7X__NUM_REGS_2, (void *)(&u32_ColumnOvenTemperature), NULL, cu16_TimeoutShort),
                                       S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                              {
                              }  // read current column oven temperature value
                              
                                 // read current gas sensor alarm threshold value
                                 if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_THRESHOLD_PPM,
                                             (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_GasSensorAlarmThresholdPPM), NULL, cu16_TimeoutShort),
                                          S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                                 {
                                 }  // read current gas sensor alarm threshold value
                                 
                                    // read current gas sensor calibration resistance (on mainboard)
                                    if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_CALIBRATION_RESISTANCE,
                                                (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_GasSensorCalibrationResistance), NULL, cu16_TimeoutShort),
                                             S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                                    {
                                    }  // read current gas sensor calibration resistance (on mainboard)                                    
                                       // read current temperature sensor state
                                       if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_TEMPERATURE_SENSOR,
                                                   (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_StateTemperatureSensor), NULL, cu16_TimeoutShort),
                                                S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                                       {
                                       }  // read temperature sensor state
                                       
                                          //                                          // check temperature value and temperature sensor state
                                          //                                          if (u32_ColumnOvenTemperature && (u16_StateTemperatureSensor == (ushort)COLUMN_OVEN_REGISTERS__STATE_TEMPERATURE_SENSOR__CONNECTED))
                                          //                                          {
                                          if (sb_TemperatureSensorFailureSet)
                                          {
                                             // reset temperature sensor failure boolean
                                             sb_TemperatureSensorFailureSet = FALSE;
                                          }
                                          // try to set states as long as no gas alarm is active
                                          FOREVER//while (1)
                                          {
                                             // set new sensor state
                                             u16_StateGasSensor = (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__CONNECTED;
                                             // Reset Error at Phoenix Contact (PIN 7 -> HIGH)
                                             Mcp2317SetOutput(0, (ushort)MCP2317_PB7, (ushort)MCP2317_PB7, OS_T_MS(500));
                                             if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_GAS_SENSOR,
                                                         (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_StateGasSensor), NULL, cu16_TimeoutShort),
                                                      S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                                             {                                             
                                                break;   // step out of while loop
                                             }
                                             RESCHEDULE(100);                                                                                                                                                                                    
                                          }
                                          f_GasSensorCurrentResistance     =  ((float)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_SERIES_RESISTANCE_IN_OHM * (float)u16A_AdcData[6]) / ((float)1024.0 - (float)u16A_AdcData[6]);
                                          u32_GasSensorCurrentResistance   =  (ulong)(rintf (f_GasSensorCurrentResistance));
                                          // write current gas sensor resistance
                                          if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__GAS_SENSOR_CURRENT_RESISTANCE,
                                                      (ushort)AT91SAM7X__NUM_REGS_2, (void *)(&u32_GasSensorCurrentResistance), NULL, cu16_TimeoutShort),
                                                   S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                                          {
                                             S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING;
                                             LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
                                          }
                                          // convert temperature to degree celsius -- DO NOT USE
                                          //f_Temp            =  (float)u32_ColumnOvenTemperature / (float)10000.0;
                                          // calc gas sensor temperature constant - THE TEMPERATURE DEPENDENCY FROM GAS SENSOR HAS BEEN PROVEN TO BE UNIMPORTANT - DON'T USE IT
                                          //f_Kt              =  expf ((((float)250.0 - f_Temp) * ((float)250.0 - f_Temp) * ((float)250.0 - f_Temp) * (float)1.502e-7) - (float)1.76);
                                          u32_GasSensorCurrentPPM =  (ulong)(rintf (((float)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_CALIBRATION_CONCENTRATION_IN_PPM *
                                          expf ((float)-1.526 * logf (f_GasSensorCurrentResistance / (/*f_Kt */ (float)u16_GasSensorCalibrationResistance))))));
                                          // check if gas alarm is present
                                          if (u32_GasSensorCurrentPPM > (ulong)u16_GasSensorAlarmThresholdPPM)
                                          {
                                             u16_GasSensorAlarmDetected = (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_ALARM;
                                             // Set Error at Phoenix Contact (PIN 7 -> LOW)
                                             Mcp2317SetOutput(0, (ushort)MCP2317_PB7, (ushort)~MCP2317_PB7, OS_T_MS(500));
                                          }
                                          else
                                          {
                                             u16_GasSensorAlarmDetected = (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_NO_ALARM;
                                             // Reset Error at Phoenix Contact (PIN 7 -> HIGH)
                                             Mcp2317SetOutput(0, (ushort)MCP2317_PB7, (ushort)MCP2317_PB7, OS_T_MS(500));
                                             // new column oven state "stop" is set in next measurement cycle
                                          }
                                          // set gas alarm detection state
                                          TRY_AND_CATCH
                                          (
                                          MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__GAS_SENSOR_ALARM_DETECTED,
                                          (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_GasSensorAlarmDetected), NULL, cu16_TimeoutShort),
                                          S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
                                          );
                                          //                                             // debug purpose ####
                                          //                                             sprintf ((char *)s8_UsartDataTx, "count           %d\r", su32_TaskTicks);
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "ADC 6 data    = %d\r", u16A_AdcData[6]);
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "ADC 3 data    = %d\r", u16A_AdcData[3]);
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "enabled       = %d\r", u16_GasSensorAlarmEnabled);
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "gas state     = %d\r", u16_StateGasSensor);
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "alarm detec   = %d\r", u16_GasSensorAlarmDetected);
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "sensor curr R = %d\r", u32_GasSensorCurrentResistance);
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "sensor cal  R = %d\r", u16_GasSensorCalibrationResistance);
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "ppmThreshold  = %d\r", u16_GasSensorAlarmThresholdPPM);
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "T in [C]      = %d\r", (int)(f_Temp*1000.0));
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "Kt*1000       = %d\r", (int)(f_Kt*1000.0));
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                             sprintf ((char *)s8_UsartDataTx, "sensor currPPM= %d\r", u32_GasSensorCurrentPPM);
                                          //                                             UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
                                          //                                          }  // end of "check temperature value and temperature sensor state"
                                          //                                          else
                                          //                                          {
                                          //                                             // check for set failure code
                                          //                                             if ((!sb_GasSensorFailureSet) && (!sb_TemperatureSensorFailureSet))
                                          //                                             {
                                          //                                                sb_GasSensorFailureSet              =  TRUE;
                                          //                                                sb_TemperatureSensorFailureSet      =  TRUE;
                                          //
                                          //                                                // set error "gas sensor failure" only once per device run
                                          //                                                S_Exceptions.u32_ErrorCodeSW_Sub    =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_GAS_SENSOR_FAILURE;
                                          //                                                S_Exceptions.u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING;
                                          //
                                          //                                                // set error codes to register bank
                                          //                                                LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
                                          //                                             }
                                          //
                                          //                                             // set new sensor state - disconnected is shown as "Err" in display menue and removed if temperature sensor is connected
                                          //                                             u16_StateGasSensor = (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__DISCONNECTED;
                                          //
                                          //                                             if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_GAS_SENSOR,
                                          //                                                                         (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_StateGasSensor), NULL, cu16_TimeoutShort),
                                          //                                                          S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                                          //                                             {
                                          //                                             }
                                          //                                          }

                           }  // sensor warm up time expired
                        }  // gas sensor connected - values ok

                  }
                  else  // gas alarm disabled
                  {
                     if (     (sb_GasSensorFailureSet)
                           && (!sb_GasSensorWarmUpFlag)
                           && (u16_StateGasSensor == (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__DISCONNECTED)
                           )
                     {
                        // reset warm up time if sensor is in state "disconnected" before "alarm disabled"
                        su32_TaskTicks            =  0;
                     }
                     // sensor is in state "connected" and now "disconnected" because of alarm disabling
                     if (!sb_GasSensorFailureSet)
                     {
                        sb_GasSensorFailureSet  =  TRUE;
                        sb_GasSensorWarmUpFlag  =  TRUE;
                        u16_GasSensorAlarmDetected = (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_NO_ALARM;
                        // set gas alarm detection state to "no alarm"
                        if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__GAS_SENSOR_ALARM_DETECTED,
                                    (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_GasSensorAlarmDetected), NULL, cu16_TimeoutShort),
                                 S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                        {
                           // we do not set directly an error code when alarm is disabled - we set gas sensor state to "disconnected"
                           // which is shown as "Err" in display menue
                           u16_StateGasSensor = (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__DISCONNECTED;
                           TRY_AND_CATCH
                           (
                           MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_GAS_SENSOR,
                           (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_StateGasSensor), NULL, cu16_TimeoutShort),
                           S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
                           );
                        }
                     }
                  }
               }
               else
               {
                  // check if more than ten times the register bank access for read out gas sensor "enabled" value failed
                  if (su16_CycleCounter < 10)
                  {
                     // set error codes to "no error"
                     memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
                     su16_CycleCounter++;
                  }
                  else if (su16_CycleCounter == 10)
                  {
                     // set error codes to "no error"
                     memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
                     // try to incease timeout value for further access
                     su16_Timeout = cu16_TimeoutLong;
                     su16_CycleCounter++;
                  }
                  else
                  {
                     // set error codes to "no error"
                     memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
                     // try to incease timeout value for further access
                     su16_Timeout = cu16_TimeoutLong;
                     su16_CycleCounter=su16_CycleCounter;
                  }
               }
               // check if gas sensor is connected or broken
               if ((su32_TaskTicks <= su32_GasSensorWarmupTasksTicks))
               {
                  // increment time counter
                  su32_TaskTicks++;
               }
               if (S_Exceptions.u32_ErrorCodeSW_Sub)
               {
                  S_Exceptions.u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING;
                  // set error codes to register bank
                  LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
               }
//               #ifndef OEM_LED//If NOT Defined
//               {
                  // update display
                  AT91F_PIO_SetOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
                  RESCHEDULE(50);
                  // update display
                  AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
                  RESCHEDULE(50);
//               } 
//               #endif
                             
               // sensor cycle/sample time
               //os_dly_wait ((ushort)AT91SAM7X__TICK_500);
               RESCHEDULE((ushort)AT91SAM7X__TICK_500);//NEW               
            }  // end of inner FOREVER loop
         }
         else
         {
            S_Exceptions.u32_ErrorCodeSW_Sub    =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__ADC_INTERNAL_NOT_INITIALIZED;
            S_Exceptions.u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING;
            // set error codes to register bank
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
            // update display
            AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
            // quit this task because of not initialized ADC user channels
            os_tsk_delete_self();
         }
      }  // device init done
      //os_dly_wait ((ushort)AT91SAM7X__TICK_500);
      RESCHEDULE((ushort)AT91SAM7X__TICK_500);
   }
}
//-----------------------------------------------------------------------------------------------------------------------------------------
__task void TSK_ColumnOvenSupportTasks_GlobalTimer (void)
{
   // this task is called periodically with "AT91SAM7X__REPETITION_TICKS_FOR_TSK_GLOBAL_TIMER"
   os_itv_set ((ushort)AT91SAM7X__REPETITION_TICKS_FOR_TSK_GLOBAL_TIMER);
   FOREVER
   {
      // wait for finishing fixed delay
      os_itv_wait ();
      os_evt_set ((ushort)COLUMN_OVEN_SUPPORT_TASKS__EVENT_FOR_TSK_TEMPERATURE_CONTROL, (OS_TID)TID_ColumnOvenSignalProcessingFuncs_TemperatureControl);
   }
}
//-----------------------------------------------------------------------------------------------------------------------------------------
