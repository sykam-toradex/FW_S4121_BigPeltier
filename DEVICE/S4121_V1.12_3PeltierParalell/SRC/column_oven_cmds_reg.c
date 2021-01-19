/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_cmds_reg.c
*
*  \brief               This file contains the \em register command functions which will be executed if sending the appropriate
*                       command via modbus (MBPB). Further details for commands of this file are listed in \ref column_oven_cmds_reg.h.
*
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2010-02-03     Initial version imported from S6510_cmd_functions_reg.c.
*  \date                2011-08-10     Renamed file from S4121_cmd_functions_reg.c
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                2012-12-11     Tested external register commands.
*
*  \todo
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\ext\spi\nvram\fm25clss\fm25clssext.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\utils_funcs.h"
#include "column_oven_registers.h"
#include "column_oven_cmds_reg.h"
#include "column_oven_cmds_owner.h"
#include "column_oven_signal_processing_funcs.h"
#include "column_oven_support_funcs.h"
#include "column_oven_support_tasks.h"
//---------------------------------------------- global definitions ---------------------------------------------------------------------------
// The following register command functions have a specific order which is dependend on \ref TSK_ColumnOvenModbusTasks_CmdExecution.
// NOTE: Adapt the number of array elements as well as size for this init.
tfp_ColumnOvenCmdsReg_REGCMD   fpA_ColumnOvenCmdsReg_REGCMDs_DeviceParamsIntern[4] =
{
   { &ColumnOvenCmdsReg_ControlSectionParams                   },
   { &ColumnOvenCmdsReg_GasSensorAlarmThresholdPPM             },
   { &ColumnOvenCmdsReg_GasSensorCalibrationResistance         },
   { &ColumnOvenCmdsReg_GasSensorAlarmEnabled                  }
};
tfp_ColumnOvenCmdsReg_REGCMD   fpA_ColumnOvenCmdsReg_REGCMDs_DeviceParamsExtern[3] =
{
   { &ColumnOvenCmdsReg_TemperatureOffset                      },
   { &ColumnOvenCmdsReg_TemperatureUnit                        },
   { &ColumnOvenCmdsReg_HeatCoolProgramSetPointReachedBand     }
};
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern            bool     b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified;
extern   const    ushort   cu16_TimeoutShort;
//---------------------------------------------- command functions activated through changed params --------------------------------------------
ushort ColumnOvenCmdsReg_ControlSectionParams (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_StateHeatCoolProgram;
   ushort   u16_ControlDacOuputVoltage;
   ushort   u16_DeviceParamsLoaded;
   ushort   u16_ColumnOvenState;
   ushort   u16_StateControlSectionParams;
   ushort   u16_ErrorCodeSW_Base;
   ulong    u32_HeatCoolProgramTimeRemain;
   ulong    u32_HeatCoolProgramTimeSectionRemain;
   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub   =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // set global flag that signal processing task does refresh status data
   b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
   // reset and write device params loaded flag - that means the control section params are not valid anymore until save or device restart
   u16_DeviceParamsLoaded = 0;
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__DEVICE_PARAMS_LOADED,
               (ushort)(AT91SAM7X__NUM_REGS_1), (void *)(&u16_DeviceParamsLoaded), NULL, cu16_TimeoutShort),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
   }
   // write heat/cool program state
   u16_StateHeatCoolProgram = (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__STOP;
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_HEAT_COOL_PROGRAM,
               (ushort)(AT91SAM7X__NUM_REGS_1), (void *)(&u16_StateHeatCoolProgram), NULL, cu16_TimeoutShort),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
   }
   // write DAC output voltage
   u16_ControlDacOuputVoltage = 0;
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_DAC_OUTPUT_VOLTAGE,
               (ushort)(AT91SAM7X__NUM_REGS_1), (void *)(&u16_ControlDacOuputVoltage), NULL, cu16_TimeoutShort),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
   }
   // write remaining time of heat/cool program
   u32_HeatCoolProgramTimeRemain = 0;
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_TIME_REMAIN,
               (ushort)(AT91SAM7X__NUM_REGS_2), (void *)(&u32_HeatCoolProgramTimeRemain), NULL, cu16_TimeoutShort),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
   }
   // write remaining section time of heat/cool program
   u32_HeatCoolProgramTimeSectionRemain = 0;
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_TIME_SECTION_REMAIN,
               (ushort)(AT91SAM7X__NUM_REGS_2), (void *)(&u32_HeatCoolProgramTimeSectionRemain), NULL, cu16_TimeoutShort),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
   }
   // write NEW state of control sections params
   u16_StateControlSectionParams = (ushort)COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__MODIFIED;
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_CONTROL_SECTION_PARAMS,
               (ushort)(AT91SAM7X__NUM_REGS_1), (void *)(&u16_StateControlSectionParams), NULL, cu16_TimeoutShort),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // write NEW column oven state into register bank
      u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP;
      TRY_AND_CATCH
      (
      MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_STATE,
      (ushort)(AT91SAM7X__NUM_REGS_1), (void *)(&u16_ColumnOvenState), NULL, cu16_TimeoutShort),
      *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
      );
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__CONTROL_SECTIONS_PARAMS;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsReg_GasSensorAlarmThresholdPPM (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_GasSensorAlarmEnabled;
   ushort   u16_GasSensorAlarmThresholdPPM         =  u16_ModbusRxData;
   ushort   u16_NumBytes                           =  (ushort)AT91SAM7X__NUM_BYTES_2;
   ushort   u16_DestAddress                        =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_THRESHOLD_PPM;
   ushort   u16_ErrorCodeSW_Base                   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // read last gas sensor enabled state from fixed memory
   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u16_GasSensorAlarmEnabled), u16_NumBytes,
               (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_ENABLED, (bool)UTILS_FUNCS__READ),
            *pu32_ErrorCodeSW_Sub))
   {
      // check for never stored gas sensor params
      if (!(((ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_MASK_ALARM_DISABLED & u16_GasSensorAlarmEnabled) == ((ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_CODE_PARAMS_STORED)))
      {
         TRY_AND_CATCH_APP_FUNC
         (
         ColumnOvenSupportFuncs_StoreGasSensorDefaultValues (), *pu32_ErrorCodeSW_Sub
         );
      }
      if (!(*pu32_ErrorCodeSW_Sub))
      {
         TRY_AND_CATCH_APP_FUNC
         (
         UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_GasSensorAlarmThresholdPPM), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
         *pu32_ErrorCodeSW_Sub
         );
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__GAS_SENSOR_ALARM_THRESHOLD_PPM;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsReg_GasSensorCalibrationResistance (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_GasSensorAlarmEnabled;
   ushort   u16_GasSensorCalibrationResistance     =  u16_ModbusRxData;
   ushort   u16_NumBytes                           =  (ushort)AT91SAM7X__NUM_BYTES_2;
   ushort   u16_DestAddress                        =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_CALIBRATION_RESISTANCE;
   ushort   u16_ErrorCodeSW_Base                   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // read last gas sensor enabled state from fixed memory
   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u16_GasSensorAlarmEnabled), u16_NumBytes,
               (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_ENABLED, (bool)UTILS_FUNCS__READ),
            *pu32_ErrorCodeSW_Sub))
   {
      // check for never stored gas sensor params
      if (!(((ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_MASK_ALARM_DISABLED & u16_GasSensorAlarmEnabled) == ((ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_CODE_PARAMS_STORED)))
      {
         TRY_AND_CATCH_APP_FUNC
         (
         ColumnOvenSupportFuncs_StoreGasSensorDefaultValues (), *pu32_ErrorCodeSW_Sub
         );
      }
      if (!(*pu32_ErrorCodeSW_Sub))
      {
         TRY_AND_CATCH_APP_FUNC
         (
         UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_GasSensorCalibrationResistance), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
         *pu32_ErrorCodeSW_Sub
         );
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__GAS_SENSOR_CALIBRATION_RESISTANCE;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsReg_GasSensorAlarmEnabled (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_GasSensorAlarmEnabled;
   ushort   u16_NumBytes                           =  (ushort)AT91SAM7X__NUM_BYTES_2;
   ushort   u16_DestAddress                        =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_ENABLED;
   ushort   u16_ErrorCodeSW_Base                   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // read last gas sensor enabled state from fixed memory
   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u16_GasSensorAlarmEnabled), u16_NumBytes,
               (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_ENABLED, (bool)UTILS_FUNCS__READ),
            *pu32_ErrorCodeSW_Sub))
   {
      // check for never stored gas sensor params
      if (!(((ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_MASK_ALARM_DISABLED & u16_GasSensorAlarmEnabled) == ((ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_CODE_PARAMS_STORED)))
      {
         TRY_AND_CATCH_APP_FUNC
         (
         ColumnOvenSupportFuncs_StoreGasSensorDefaultValues (), *pu32_ErrorCodeSW_Sub
         );
      }
      else
      {
         u16_GasSensorAlarmEnabled  =  u16_ModbusRxData;
         u16_GasSensorAlarmEnabled |=  (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_CODE_PARAMS_STORED;
         TRY_AND_CATCH_APP_FUNC
         (
         UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_GasSensorAlarmEnabled), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
         *pu32_ErrorCodeSW_Sub
         );
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__GAS_SENSOR_ALARM_ENABLED;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsReg_TemperatureOffset (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_TemperatureOffset;
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_DeviceParamsLoaded;
   ushort   u16_ErrorCodeSW_Base;
   // set start values of local variables
   u16_TemperatureOffset   =  u16_ModbusRxData;
   u16_DeviceParamsLoaded  =  0;
   u16_NumBytes            =  (ushort)AT91SAM7X__NUM_BYTES_2;
   u16_DestAddress         =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__TEMPERATURE_OFFSET;
   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub   =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // write new temperature offset into fixed memory
   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_TemperatureOffset), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
            *pu32_ErrorCodeSW_Sub))
   {
      // load "device params loaded" code and flags from fixed memory
      u16_NumBytes      =  sizeof (u16_DeviceParamsLoaded);
      u16_DestAddress   = (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOADED;
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_DeviceParamsLoaded), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
         if ((u16_DeviceParamsLoaded & (~(ushort)COLUMN_OVEN_CMDS_REG__MASK_DEVICE_PARAMS_STORED)) != ((ushort)COLUMN_OVEN_CMDS_REG__CODE_DEVICE_PARAMS_STORED))
         {
            // "device params loaded" code was never saved before -- set it
            u16_DeviceParamsLoaded  = 0;
            u16_DeviceParamsLoaded |= (ushort)COLUMN_OVEN_CMDS_REG__CODE_DEVICE_PARAMS_STORED;
         }
         // ensure that already saved params flags remain fixed
         u16_DeviceParamsLoaded |=  (ushort)COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_OFFSET_STORED;
         // misuse "device params loaded" address to store info that temperature offset was saved one time
         u16_DestAddress = (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOADED;
         TRY_AND_CATCH_APP_FUNC
         (
         UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_DeviceParamsLoaded), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
         *pu32_ErrorCodeSW_Sub
         );
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__TEMPERATURE_OFFSET;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsReg_TemperatureUnit (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_TemperatureUnit;
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_DeviceParamsLoaded;
   ushort   u16_ErrorCodeSW_Base;
   // set start values of local variables
   u16_TemperatureUnit     =  u16_ModbusRxData;
   u16_DeviceParamsLoaded  =  0;
   u16_NumBytes            =  (ushort)AT91SAM7X__NUM_BYTES_2;
   u16_DestAddress         =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__TEMPERATURE_UNIT;
   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub   =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // write new temperature unit into fixed memory
   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_TemperatureUnit), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
            *pu32_ErrorCodeSW_Sub))
   {
      // load "device params loaded" code and flags from fixed memory
      u16_NumBytes      =  sizeof (u16_DeviceParamsLoaded);
      u16_DestAddress   = (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOADED;
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_DeviceParamsLoaded), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
         if ((u16_DeviceParamsLoaded & (~(ushort)COLUMN_OVEN_CMDS_REG__MASK_DEVICE_PARAMS_STORED)) != ((ushort)COLUMN_OVEN_CMDS_REG__CODE_DEVICE_PARAMS_STORED))
         {
            // "device params loaded" code was never saved before -- set it
            u16_DeviceParamsLoaded  = 0;
            u16_DeviceParamsLoaded |= (ushort)COLUMN_OVEN_CMDS_REG__CODE_DEVICE_PARAMS_STORED;
         }
         // ensure that already saved params flags remain fixed
         u16_DeviceParamsLoaded |=  (ushort)COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_UNIT_STORED;
         // misuse "device params loaded" address to store info that temperature unit was saved one time
         u16_DestAddress = (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOADED;
         TRY_AND_CATCH_APP_FUNC
         (
         UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_DeviceParamsLoaded), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
         *pu32_ErrorCodeSW_Sub
         );
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__TEMPERATURE_UNIT;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsReg_HeatCoolProgramSetPointReachedBand (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_HeatCoolProgramSetPointReachedBand;
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_DeviceParamsLoaded;
   ushort   u16_ErrorCodeSW_Base;
   // set start values of local variables
   u16_HeatCoolProgramSetPointReachedBand    =  u16_ModbusRxData;
   u16_DeviceParamsLoaded                    =  0;
   u16_NumBytes                              =  (ushort)AT91SAM7X__NUM_BYTES_2;
   u16_DestAddress                           =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND;
   u16_ErrorCodeSW_Base                      =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub                     =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // write new temperature unit into fixed memory
   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_HeatCoolProgramSetPointReachedBand), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
            *pu32_ErrorCodeSW_Sub))
   {
      // load "device params loaded" code and flags from fixed memory
      u16_NumBytes      =  sizeof (u16_DeviceParamsLoaded);
      u16_DestAddress   = (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOADED;
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_DeviceParamsLoaded), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
         if ((u16_DeviceParamsLoaded & (~(ushort)COLUMN_OVEN_CMDS_REG__MASK_DEVICE_PARAMS_STORED)) != ((ushort)COLUMN_OVEN_CMDS_REG__CODE_DEVICE_PARAMS_STORED))
         {
            // "device params loaded" code was never saved before -- set it
            u16_DeviceParamsLoaded  = 0;
            u16_DeviceParamsLoaded |= (ushort)COLUMN_OVEN_CMDS_REG__CODE_DEVICE_PARAMS_STORED;
         }
         // ensure that already saved params flags remain fixed
         u16_DeviceParamsLoaded |=  (ushort)COLUMN_OVEN_CMDS_REG__FLAG_HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND_STORED;
         // misuse "device params loaded" address to store info that temperature set point reached band was saved one time
         u16_DestAddress = (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOADED;
         TRY_AND_CATCH_APP_FUNC
         (
         UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_DeviceParamsLoaded), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
         *pu32_ErrorCodeSW_Sub
         );
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
