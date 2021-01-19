/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_registers_funcs.c
*
*  \brief               This file initializes the column_oven register bank. Initialization comprises device params and status
*                       data. For some entries of register bank defaults are set.
*                       Every function returns an sub error code which is greater than zero in any error case.
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*
*  \date                2011-02-03     Initial version imported S6510_registers.c.
*  \date                2011-12-05     Extended initialization functions.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                2012-12-06     Checked error cases and byte swap for sykam device params.
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
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\common_cmds_owner.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\utils_funcs.h"
#include "column_oven_registers.h"
#include "column_oven_support_tasks.h"
#include "column_oven_signal_processing_funcs.h"
#include "column_oven_registers_funcs.h"
#include "column_oven_control_params.h"
#include "column_oven_support_funcs.h"
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern   const    ushort   cu16_TimeoutShort;
extern   const    ushort   cu16_SykamCfg_DeviceClass;
extern   const    struct   _S_ColumnOvenRegisters_ControlSectionParams cSSA_ColumnOvenControlParams[][AT91SAM7X__MAX_CONTROL_PARAM_SECTIONS];
//---------------------------------------------- functions -------------------------------------------------------------------------------------
ulong ColumnOvenRegistersFuncs_InitDeviceParams (void)
{
   struct   _SMBPBRegAccess                              S_ModbusAccessParams;
   struct   _S_ColumnOvenRegisters_DeviceParamsIntern    S_DeviceParamsIntern;
   struct   _S_ColumnOvenRegisters_DeviceParamsExtern    S_DeviceParamsExtern;
   ushort   u16_ControlSectionsNum;
   ushort   u16_Idx;
   ushort   u16_SykamDeviceDataOptions;
   ulong    u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // preset temperature profile sections with defaults
   for (u16_Idx = 0; u16_Idx < (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_SECTIONS; u16_Idx++)
   {
      S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[u16_Idx].u16_SetPoint =  0x0000;
      S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[u16_Idx].u16_HoldTime =  0x0000;
   }
   S_DeviceParamsExtern.S_HeatCoolProgramToModify.u16_NumActiveSections =  0;
   S_DeviceParamsExtern.S_HeatCoolProgramToModify.u16_TimeTotal         =  0;
   // set defaults for other extern device params
   S_DeviceParamsExtern.u16_TemperatureOffset                           =  0;
   S_DeviceParamsExtern.u16_TemperatureUnit                             =  (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__CELSIUS;
   S_DeviceParamsExtern.u16_HeatCoolProgramSetPointReachedBand          =  (ushort)AT91SAM7X__DEFAULT_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN;
   // set modbus access params
   S_ModbusAccessParams.ucType     =  (uchar)  MBPB_REGTYPE_HOLDING;
   S_ModbusAccessParams.bRW        =  (uchar)  MBPB_REGRW_WRITE;
   S_ModbusAccessParams.usAddr     =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START;  // register start address
   S_ModbusAccessParams.usAddrOffs =  (ushort) 0x0000;
   S_ModbusAccessParams.usCount    =  (ushort) (sizeof(S_DeviceParamsExtern)/2);                                      // total words of S_DeviceParamsExtern;
   S_ModbusAccessParams.pvData     =  (void *) (&S_DeviceParamsExtern);
   S_ModbusAccessParams.pvIntern   =  NULL;                                                                           // debug purpose, set to &pv_BufferAddress if needed
   S_ModbusAccessParams.u16To      =  cu16_TimeoutShort;
   // map the content of params into holding registers
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // reset control sections params (without load of these params from permanent memory no temperature control is possible)
      memset ((void *)(& S_DeviceParamsIntern.SA_ControlSectionParams[0]), 0, sizeof(tS_ColumnOvenRegisters_ControlSectionParams) * AT91SAM7X__MAX_CONTROL_PARAM_SECTIONS);
      // load Sykam option field from fixed memory
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u16_SykamDeviceDataOptions), (ushort)AT91SAM7X__NUM_BYTES_2,
                  ((ushort)COMMON_REGISTERS__FRAM_ADDR__SYKAM_DEVICE_DATA__END - 1), (bool)COMMON_CMDS_OWNER__READ),
               u32_ErrorCodeSW_Sub))
      {
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
         // check device option which default control section params shall be set
         if ((cu16_SykamCfg_DeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN) && (u16_SykamDeviceDataOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL) && (!(u16_SykamDeviceDataOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)))
         {
            memcpy ((void *)(& S_DeviceParamsIntern.SA_ControlSectionParams[0]), (const void *)(& cSSA_ColumnOvenControlParams[COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL][0]), sizeof(S_DeviceParamsIntern.SA_ControlSectionParams));
         }
         else if ((cu16_SykamCfg_DeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN) && (u16_SykamDeviceDataOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
         {
            memcpy ((void *)(& S_DeviceParamsIntern.SA_ControlSectionParams[0]), (const void *)(& cSSA_ColumnOvenControlParams[COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT][0]), sizeof(S_DeviceParamsIntern.SA_ControlSectionParams));
         }
         else if ((cu16_SykamCfg_DeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN) && ((u16_SykamDeviceDataOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL) && (u16_SykamDeviceDataOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)))
#endif
         {
            memcpy ((void *)(& S_DeviceParamsIntern.SA_ControlSectionParams[0]), (const void *)(& cSSA_ColumnOvenControlParams[COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER][0]), sizeof(S_DeviceParamsIntern.SA_ControlSectionParams));
         }
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
         else
         {
            return u32_ErrorCodeSW_Sub =  (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_REGISTERS_FUNCS__INIT_DEVICE_PARAMS);
         }
#endif
         // check default control section params and count them
         if (SUCCESS_APP_FUNC (ColumnOvenSupportFuncs_DeviceParamsInternCheck ((void *)&S_DeviceParamsIntern, &u16_ControlSectionsNum, (bool)COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_INIT),
                  u32_ErrorCodeSW_Sub))
         {
            // set defaults for other intern device params
            S_DeviceParamsIntern.u32_GasSensorAlarmThresholdPPM      =  (ushort)COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_ALARM_THRESHOLD_PPM;
            S_DeviceParamsIntern.u16_GasSensorAlarmEnabled           =  (ushort)COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_ALARM_ENABLED;
            S_DeviceParamsIntern.u16_GasSensorCalibrationResistance  =  (ushort)COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_CALIBRATION_RESISTANCE;
            // set modbus access params
            S_ModbusAccessParams.ucType     =  (uchar)  MBPB_REGTYPE_HOLDING;
            S_ModbusAccessParams.bRW        =  (uchar)  MBPB_REGRW_WRITE;
            S_ModbusAccessParams.usAddr     =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START;
            S_ModbusAccessParams.usAddrOffs =  (ushort) 0x0000;
            S_ModbusAccessParams.usCount    =  (ushort) (sizeof(S_DeviceParamsIntern)/2);
            S_ModbusAccessParams.pvData     =  (void *) (&S_DeviceParamsIntern);
            S_ModbusAccessParams.pvIntern   =  NULL;
            S_ModbusAccessParams.u16To      =  cu16_TimeoutShort;
            // map the content of params into holding registers
            if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), u32_ErrorCodeSW_Sub,
                     (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               // set modbus access params
               S_ModbusAccessParams.ucType     =  (uchar)  MBPB_REGTYPE_INPUT;
               S_ModbusAccessParams.bRW        =  (uchar)  MBPB_REGRW_WRITE;
               S_ModbusAccessParams.usAddr     =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_SECTIONS_NUM;
               S_ModbusAccessParams.usAddrOffs =  (ushort) 0x0000;
               S_ModbusAccessParams.usCount    =  (ushort) (AT91SAM7X__NUM_REGS_1);
               S_ModbusAccessParams.pvData     =  (void *) (&u16_ControlSectionsNum);
               S_ModbusAccessParams.pvIntern   =  NULL;
               S_ModbusAccessParams.u16To      =  cu16_TimeoutShort;
               // write number of control sections to intern status data
               if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), u32_ErrorCodeSW_Sub,
                        (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
               }
            }
         }
      }
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_REGISTERS_FUNCS__INIT_DEVICE_PARAMS);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenRegistersFuncs_InitStatusData (void)
{
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   struct   _S_ColumnOvenRegisters_StatusDataIntern   S_StatusDataIntern;
   struct   _S_ColumnOvenRegisters_StatusDataExtern   S_StatusDataExtern;
   ushort   u16_Idx;
   ushort   u16_SykamDeviceOptions;
   ulong    u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // load Sykam option field from fixed memory
   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u16_SykamDeviceOptions), (ushort)AT91SAM7X__NUM_BYTES_2,
               ((ushort)COMMON_REGISTERS__FRAM_ADDR__SYKAM_DEVICE_DATA__END - 1), (bool)COMMON_CMDS_OWNER__READ),
            u32_ErrorCodeSW_Sub))
   {
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
      // set profiles max and sections max dependend on device option
      if (     (cu16_SykamCfg_DeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
            && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM)
            && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
            && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
            && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
            )
      {
         S_StatusDataExtern.u16_HeatCoolProgramMaxSections  =  (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_SECTIONS;
         S_StatusDataExtern.u16_HeatCoolProgramMaxId        =  (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID;
      }
      else
#endif
      {
         S_StatusDataExtern.u16_HeatCoolProgramMaxSections  =  (ushort)AT91SAM7X__MAX_HEAT_PROGRAM_SECTIONS;
         S_StatusDataExtern.u16_HeatCoolProgramMaxId        =  (ushort)AT91SAM7X__MAX_HEAT_PROGRAM_ID;
      }
      // preset temperature profile sections with defaults
      for (u16_Idx = 1; u16_Idx < (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_SECTIONS; u16_Idx++)
      {
         S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[u16_Idx].u16_SetPoint =  0x0000;
         S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[u16_Idx].u16_HoldTime =  0x0000;
      }
      // if no profile is loaded after register bank init because there is no data stored, set default set point to enable user to start a program
      S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint =  (ushort)AT91SAM7X__DEFAULT_HEAT_COOL_PROGRAM_SET_POINT_IN_DEGREE_MUL_TEN;
      S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_HoldTime =  0;
      S_StatusDataExtern.S_HeatCoolProgramInUse.u16_NumActiveSections      =  0;
      // set defaults to some extern status data
      S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed                    =  0x00000000;
      S_StatusDataExtern.u32_HeatCoolProgramTimeRemain                     =  0x00000000;
      S_StatusDataExtern.u32_HeatCoolProgramTimeTotal                      =  0x00000000;
      S_StatusDataExtern.u32_HeatCoolProgramTimeSectionRemain              =  0x00000000;
      S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal               =  0x00000000;
      S_StatusDataExtern.u32_ColumnOvenTemperature                         =  0x00000000;
      S_StatusDataExtern.u16_ColumnOvenState                               =  (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__INIT;
      S_StatusDataExtern.u16_HeatCoolProgramCurrentId                      =  0x0000;
      S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId               =  0x0000;
      S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined         =  0x0000;
      S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint         =  0x0000;
      S_StatusDataExtern.u16_HeatCoolProgramModifiedId                     =  0x0000;
      S_StatusDataExtern.u16_HeatCoolProgramModifiedSectionsDefined        =  0x0000;
      // set modbus access params
      S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
      S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
      S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;  // register start address
      S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
      S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);                                      // total words of S_StatusDataExtern
      S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
      S_ModbusAccessParams.pvIntern     =  NULL;                                                                         // debug purpose, set to &pv_BufferAddress if needed
      S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
      // map the content of params into input registers
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), u32_ErrorCodeSW_Sub,
               (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         // set defaults to some internal status data
         S_StatusDataIntern.u32_GasSensorCurrentResistance        =  0x00000000;
         S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount      =  0x00000000;
         S_StatusDataIntern.u32_ControlDeviationAccumulated       =  0x00000000;
         S_StatusDataIntern.u32_ControlDeviationLast              =  0x00000000;
         S_StatusDataIntern.u32_ControlIntegrationTime            =  0x00000000;
         S_StatusDataIntern.u32_ControlDifferentialTime           =  (ushort)COLUMN_OVEN_REGISTERS__DEFAULT_CONTROL_DIFFERENTIAL_TIME_IN_SECONDS;
         S_StatusDataIntern.u32_ControlIntegrationGain            =  0x00000000;
         S_StatusDataIntern.u32_ControlDifferentialGain           =  (ushort)COLUMN_OVEN_REGISTERS__DEFAULT_CONTROL_DIFFERENTIAL_GAIN;
         S_StatusDataIntern.u32_ControlProportionalGain           =  0x00000000;
         S_StatusDataIntern.u16_StateHeatCoolProgram              =  (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__NONE;
         S_StatusDataIntern.u16_StateTemperatureSensor            =  (ushort)COLUMN_OVEN_REGISTERS__STATE_TEMPERATURE_SENSOR__CONNECTED;
         S_StatusDataIntern.u16_StateGasSensor                    =  (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__WARM_UP;
         S_StatusDataIntern.u16_StateControlSectionParams         =  (ushort)COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__NONE;
         S_StatusDataIntern.u16_DeviceParamsLoaded                =  0x0000;
         S_StatusDataIntern.u16_StatusDataLoaded                  =  0x0000;
         S_StatusDataIntern.u16_ControlDacOuputVoltage            =  0x0000;
         S_StatusDataIntern.u16_HeatCoolProgramStored             =  0x0000;
         S_StatusDataIntern.u16_GasSensorAlarmDetected            =  0x0000;
         // set modbus access params
         S_ModbusAccessParams.ucType     =  (uchar)  MBPB_REGTYPE_INPUT;
         S_ModbusAccessParams.bRW        =  (uchar)  MBPB_REGRW_READ;
         S_ModbusAccessParams.usAddr     =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_SECTIONS_NUM;
         S_ModbusAccessParams.usAddrOffs =  (ushort) 0x0000;
         S_ModbusAccessParams.usCount    =  (ushort) (AT91SAM7X__NUM_REGS_1);
         S_ModbusAccessParams.pvData     =  (void *) (&S_StatusDataIntern.u16_ControlSectionsNum);
         S_ModbusAccessParams.pvIntern   =  NULL;
         S_ModbusAccessParams.u16To      =  cu16_TimeoutShort;
         // READ number of control sections from intern status data
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), u32_ErrorCodeSW_Sub,
                  (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            // set modbus access params
            S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
            S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
            S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
            S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
            S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
            S_ModbusAccessParams.pvIntern     =  NULL;
            S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
            // map the content of params into input registers
            if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), u32_ErrorCodeSW_Sub,
                     (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
            }
         }
      }
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_REGISTERS_FUNCS__INIT_STATUS_DATA);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenRegistersFuncs_InitRegisterBank (void)
{
   ulong u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // initialize the complete register bank of column oven
   if (SUCCESS_APP_FUNC (ColumnOvenRegistersFuncs_InitDeviceParams(), u32_ErrorCodeSW_Sub))
   {
      TRY_AND_CATCH_APP_FUNC (ColumnOvenRegistersFuncs_InitStatusData(), u32_ErrorCodeSW_Sub);
   }
   return u32_ErrorCodeSW_Sub;
}
