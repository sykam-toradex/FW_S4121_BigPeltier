/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
**  \file                column_oven_cmds_owner.c
**  \brief               This file contains the \em owner (manufacturer) command functions which will be executed if sending the appropriate
*                       command via modbus (MBPB). Execution of these functions is only allowed to owner (manufacturer) or service technician.
*                       Further details for commands in this file are listed externally. \n
*                       Every function returns an base and sub error code which is greater than zero in error case.
**  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
**  \date                2011-02-03     Initial version imported from S6510_cmd_functions_owner.c.
*  \date                2011-08-10     Renamed file from \em S4121_cmd_functions_owner.c to \em column_oven_cmds_owner.c.
**
*  \bug                 No bugs known at moment. \n
**  \test
**  \todo
**
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
#include "..\..\shared\src\lib_exceptions_funcs.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\utils_funcs.h"
#include "column_oven_registers.h"
#include "column_oven_cmds_owner.h"
#include "column_oven_cmds_user.h"
#include "column_oven_cmds_reg.h"
#include "column_oven_signal_processing_funcs.h"
#include "column_oven_control_params.h"
#include "column_oven_support_funcs.h"
//---------------------------------------------- global definitions ---------------------------------------------------------------------------
// The size and order must be the same like in documentation (excel table)
tfp_ColumnOvenCmdsOwner_CMD   fpA_ColumnOvenCmdsOwner_CMDs[] =
{
   { &ColumnOvenCmdsOwner_ControlParamsSave           },
   { &ColumnOvenCmdsOwner_DeviceParamsLoad            },
   { &ColumnOvenCmdsOwner_StatusDataLoad              },
   { &ColumnOvenCmdsOwner_HeatCoolProgramsReset       }
};
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern            bool     b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified;
extern   const    ushort   cu16_TimeoutShort;
extern   const    struct   _S_ColumnOvenRegisters_ControlSectionParams cSSA_ColumnOvenControlParams[][AT91SAM7X__MAX_CONTROL_PARAM_SECTIONS];
//---------------------------------------------- owner (manufacturer) command functions --------------------------------------------------------
ushort ColumnOvenCmdsOwner_ControlParamsSave (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_DeviceParamsInternStored;
   ushort   u16_StateControlSectionParams;
   ushort   u16_ControlSectionsNum;
   ushort   u16_ErrorCodeSW_Base;
   struct   _S_ColumnOvenRegisters_DeviceParamsIntern    S_DeviceParamsIntern;
   struct   _SMBPBRegAccess                              S_ModbusAccessParams;
   *pu32_ErrorCodeSW_Sub   =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   // read column oven state
   S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
   S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
   S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START;
   S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_DeviceParamsIntern)/2);
   S_ModbusAccessParams.pvData       =  (void *) (&S_DeviceParamsIntern);
   S_ModbusAccessParams.pvIntern     =  NULL;
   S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // set modbus access params
      S_ModbusAccessParams.ucType     =  (uchar)  MBPB_REGTYPE_INPUT;
      S_ModbusAccessParams.bRW        =  (uchar)  MBPB_REGRW_READ;
      S_ModbusAccessParams.usAddr     =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_SECTIONS_NUM;
      S_ModbusAccessParams.usAddrOffs =  (ushort) 0x0000;
      S_ModbusAccessParams.usCount    =  (ushort) (AT91SAM7X__NUM_REGS_1);
      S_ModbusAccessParams.pvData     =  (void *) (&u16_ControlSectionsNum);
      S_ModbusAccessParams.pvIntern   =  NULL;
      S_ModbusAccessParams.u16To      =  cu16_TimeoutShort;
      // READ number of control sections from intern status data
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub,
               (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         // check loaded device params (this function returns also the number of sections)
         if (SUCCESS_APP_FUNC (ColumnOvenSupportFuncs_DeviceParamsInternCheck ((void *)&S_DeviceParamsIntern, &u16_ControlSectionsNum, (bool)COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_WRITE),
                  *pu32_ErrorCodeSW_Sub))
         {
            u16_NumBytes      =  (ushort)(sizeof (tS_ColumnOvenRegisters_ControlSectionParams) * u16_ControlSectionsNum);
            u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START;
            // write new control section params into fixed memory
            if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_DeviceParamsIntern), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
                     *pu32_ErrorCodeSW_Sub))
            {
               u16_NumBytes      =  (ushort)AT91SAM7X__NUM_BYTES_2;
               u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_SECTIONS_NUM;
               // write new control section number into fixed memory
               if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_ControlSectionsNum), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
                        *pu32_ErrorCodeSW_Sub))
               {
                  // write control section params stored code into fixed memory
                  u16_DeviceParamsInternStored  =  (ushort)COLUMN_OVEN_SUPPORT_FUNCS__CODE_DEVICE_PARAMS_INTERN_STORED;
                  u16_NumBytes                  =  sizeof (u16_DeviceParamsInternStored);
                  u16_DestAddress               =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__OCMD_CONTROL_SECTIONS_PARAMS_SAVE;
                  // write control section params storage code into fixed memory
                  if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_DeviceParamsInternStored), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
                           *pu32_ErrorCodeSW_Sub))
                  {
                     // set global flag that signal processing task does refresh status data
                     b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
                     // write flag that control section params were stored
                     u16_StateControlSectionParams    =  (ushort) COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__SAVED;
                     S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_INPUT;
                     S_ModbusAccessParams.bRW         =  (uchar)  MBPB_REGRW_WRITE;
                     S_ModbusAccessParams.usAddr      =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_CONTROL_SECTION_PARAMS;
                     S_ModbusAccessParams.usCount     =  (ushort) (sizeof(u16_StateControlSectionParams)/2);
                     S_ModbusAccessParams.pvData      =  (void *) (&u16_StateControlSectionParams);
                     TRY_AND_CATCH
                     (
                     MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
                     *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
                     );
                  }
               }
            }
         }
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__CONTROL_PARAMS_SAVE;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsOwner_DeviceParamsLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_TemperatureOffset;
   ushort   u16_TemperatureUnit;
   ushort   u16_HeatCoolProgramSetPointReachedBand;
   ushort   u16_ColumnOvenState;
   ushort   u16_DeviceParamsLoaded;
   ushort   u16_StateControlSectionParams;
   ushort   u16_ControlSectionsNum;
   ushort   u16_SykamDeviceOptions;
   ushort   u16_SykamDeviceClass;
   ushort   u16_ErrorCodeSW_Base;
   struct   _S_ColumnOvenRegisters_DeviceParamsIntern S_DeviceParamsIntern;
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   tS_LibExceptions_Exceptions                        S_Exceptions;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub   =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // read column oven state
   S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
   S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
   S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_STATE;
   S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_ColumnOvenState)/2);
   S_ModbusAccessParams.pvData       =  (void *) (&u16_ColumnOvenState);
   S_ModbusAccessParams.pvIntern     =  NULL;
   S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
   }
   // load of status data shall be only possible if column oven is inactive
   if (     (u16_ColumnOvenState != (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD)
         && (u16_ColumnOvenState != (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN)
         )
   {
      u16_NumBytes      =  sizeof (S_DeviceParamsIntern);
      u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START;
      // read internal device params from fixed memory
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_DeviceParamsIntern), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
      }
      // read last user temperature offset from fixed memory
      u16_NumBytes      =  sizeof (u16_TemperatureOffset);
      u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__TEMPERATURE_OFFSET;
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_TemperatureOffset), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
      }
      // read last user temperature unit (this is only a flag to indicate that PC or another remote control must send temperature
      // values in appropriate manner)
      u16_NumBytes      =  sizeof (u16_TemperatureUnit);
      u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__TEMPERATURE_UNIT;
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_TemperatureUnit), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
      }
      // read last user temperature set point reached band
      u16_NumBytes      =  sizeof (u16_HeatCoolProgramSetPointReachedBand);
      u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND;
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_HeatCoolProgramSetPointReachedBand), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
      }
      // load "device params loaded" code and flags from fixed memory
      u16_NumBytes      =  sizeof (u16_DeviceParamsLoaded);
      u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOADED;
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_DeviceParamsLoaded), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
      }
      // check for never stored device params code (once one flag was saved the code is available)
      if ((u16_DeviceParamsLoaded & (~(ushort)COLUMN_OVEN_CMDS_REG__MASK_DEVICE_PARAMS_STORED)) != ((ushort)COLUMN_OVEN_CMDS_REG__CODE_DEVICE_PARAMS_STORED))
      {
         u16_TemperatureOffset                     =  (ushort)AT91SAM7X__DEFAULT_TEMPERATURE_OFFSET_MUL_TEN;
         u16_TemperatureUnit                       =  (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__CELSIUS;
         u16_HeatCoolProgramSetPointReachedBand    =  (ushort)AT91SAM7X__DEFAULT_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN;
         if (FAIL_APP_FUNC (ColumnOvenCmdsReg_TemperatureOffset (u16_TemperatureOffset, &S_Exceptions.u32_ErrorCodeSW_Sub),
                  S_Exceptions.u16_ErrorCodeSW_Base))
         {
            // set exception codes to register bank
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
         }
         if (FAIL_APP_FUNC (ColumnOvenCmdsReg_TemperatureUnit (u16_TemperatureUnit, &S_Exceptions.u32_ErrorCodeSW_Sub),
                  S_Exceptions.u16_ErrorCodeSW_Base))
         {
            // set exception codes to register bank
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
         }
         if (FAIL_APP_FUNC (ColumnOvenCmdsReg_HeatCoolProgramSetPointReachedBand (u16_HeatCoolProgramSetPointReachedBand, &S_Exceptions.u32_ErrorCodeSW_Sub),
                  S_Exceptions.u16_ErrorCodeSW_Base))
         {
            // set exception codes to register bank
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
         }
      }
      else
      {
         // check if device param flags were saved before - if not set default values
         if ((u16_DeviceParamsLoaded & (ushort)COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_OFFSET_STORED) != ((ushort)COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_OFFSET_STORED))
         {
            u16_TemperatureOffset = (ushort)AT91SAM7X__DEFAULT_TEMPERATURE_OFFSET_MUL_TEN;
         }
         if ((u16_DeviceParamsLoaded & (ushort)COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_UNIT_STORED) != ((ushort)COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_UNIT_STORED))
         {
            u16_TemperatureUnit = (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__CELSIUS;
         }
         if ((u16_DeviceParamsLoaded & (ushort)COLUMN_OVEN_CMDS_REG__FLAG_HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND_STORED) != ((ushort)COLUMN_OVEN_CMDS_REG__FLAG_HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND_STORED))
         {
            u16_HeatCoolProgramSetPointReachedBand = (ushort)AT91SAM7X__DEFAULT_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN;
         }
      }
      // set global flag that signal processing task does refresh status data
      b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
      // check temperature offset value and set default if necessary
      if(!(u16_TemperatureOffset & 0x8000)) // if offset is positive
      {
         if (u16_TemperatureOffset > (ushort)AT91SAM7X__MAX_TEMPERATURE_OFFSET_MUL_TEN)
            u16_TemperatureOffset = (ushort)AT91SAM7X__DEFAULT_TEMPERATURE_OFFSET_MUL_TEN;
      }
      else // if offset is negative
         if((u16_TemperatureOffset - 0x8000) > (ushort)AT91SAM7X__MAX_TEMPERATURE_OFFSET_MUL_TEN)
            u16_TemperatureOffset = (ushort)AT91SAM7X__DEFAULT_TEMPERATURE_OFFSET_MUL_TEN;
      // write temperature offset into register bank
      S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
      S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
      S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_OFFSET;
      S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_TemperatureOffset)/2);
      S_ModbusAccessParams.pvData       =  (void *) (&u16_TemperatureOffset);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
      }
      // check temperature unit and set default if necessary
      if ((u16_TemperatureUnit >= (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNITS_MAX) || (u16_TemperatureUnit == (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__NONE))
      u16_TemperatureUnit = (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__CELSIUS;
      // write temperature unit into register bank
      S_ModbusAccessParams.usAddr = (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_UNIT;
      S_ModbusAccessParams.pvData = (void *)(&u16_TemperatureUnit);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
      }
      // check set point reached band value and set default if necessary
      if (     (u16_HeatCoolProgramSetPointReachedBand > (ushort)AT91SAM7X__MAX_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN)
            && (u16_HeatCoolProgramSetPointReachedBand < (ushort)AT91SAM7X__MIN_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN))
      {
         u16_HeatCoolProgramSetPointReachedBand = (ushort)AT91SAM7X__DEFAULT_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN;
      }
      // write value of set point reached band into register bank
      S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND;
      S_ModbusAccessParams.pvData       =  (void *) (&u16_HeatCoolProgramSetPointReachedBand);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
      }
      // check loaded device params (this function returns also the number of sections)
      if (SUCCESS_APP_FUNC (ColumnOvenSupportFuncs_DeviceParamsInternCheck ((void *)&S_DeviceParamsIntern, &u16_ControlSectionsNum, (bool)COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_READ),
               *pu32_ErrorCodeSW_Sub))
      {
      }
      // validation failed: set default control section params into register bank dependend on device option
      else
      {
         // read out sykam device data option field
         S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
         S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceOptions)/2);
         S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceOptions);
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            // read out sykam device data option field
            S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
            S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START;
            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceClass)/2);
            S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceClass);
            if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
               // check device option which default control section params shall be set
               if ((u16_SykamDeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN) && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL))
               {
                  memcpy ((void *)(& S_DeviceParamsIntern.SA_ControlSectionParams[0]), (const void *)(& cSSA_ColumnOvenControlParams[COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL][0]), sizeof(S_DeviceParamsIntern.SA_ControlSectionParams));
               }
               else if ((u16_SykamDeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN) && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
               {
                  memcpy ((void *)(& S_DeviceParamsIntern.SA_ControlSectionParams[0]), (const void *)(& cSSA_ColumnOvenControlParams[COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT][0]), sizeof(S_DeviceParamsIntern.SA_ControlSectionParams));
               }
               else if ((u16_SykamDeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN) && ((u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL) && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)))
#endif
               {
                  memcpy ((void *)(& S_DeviceParamsIntern.SA_ControlSectionParams[0]), (const void *)(& cSSA_ColumnOvenControlParams[COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER][0]), sizeof(S_DeviceParamsIntern.SA_ControlSectionParams));
               }
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
               else
               {
                  *pu32_ErrorCodeSW_Sub         = (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__DEVICE_OPTION_IS_NOT_ACTIVATED;
                  return u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD;
               }
#endif
               // check default control section params and count them
               if (FAIL_APP_FUNC (ColumnOvenSupportFuncs_DeviceParamsInternCheck ((void *)&S_DeviceParamsIntern, &u16_ControlSectionsNum, (bool)COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_WRITE),
                        *pu32_ErrorCodeSW_Sub))
               {
                  return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD;
               }
            }
            else
            return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD;
         }
         else
         return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD;
      }
      // write validated device params into register bank
      S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
      S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START;
      S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_DeviceParamsIntern)/2);
      S_ModbusAccessParams.pvData       =  (void *) (&S_DeviceParamsIntern);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
      }
      // write flag that device params intern were loaded
      u16_DeviceParamsLoaded           =  (ushort) COLUMN_OVEN_REGISTERS__DEVICE_PARAMS_LOADED;
      S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_INPUT;
      S_ModbusAccessParams.usAddr      =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__DEVICE_PARAMS_LOADED;
      S_ModbusAccessParams.usCount     =  (ushort) (sizeof(u16_DeviceParamsLoaded)/2);
      S_ModbusAccessParams.pvData      =  (void *) (&u16_DeviceParamsLoaded);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
      }
      S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_INPUT;
      S_ModbusAccessParams.usAddr      =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_SECTIONS_NUM;
      S_ModbusAccessParams.usCount     =  (ushort) (AT91SAM7X__NUM_REGS_1);
      S_ModbusAccessParams.pvData      =  (void *) (&u16_ControlSectionsNum);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         // write flag that control sections params were loaded
         u16_StateControlSectionParams    =  (ushort) COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__LOADED;
         S_ModbusAccessParams.usAddr      =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_CONTROL_SECTION_PARAMS;
         S_ModbusAccessParams.usCount     =  (ushort) (sizeof(u16_StateControlSectionParams)/2);
         S_ModbusAccessParams.pvData      =  (void *) (&u16_StateControlSectionParams);
         TRY_AND_CATCH
         (
         MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
         *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
         );
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsOwner_StatusDataLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_FixedMemData;
   ushort   u16_SykamDeviceOptions;
   ushort   u16_SykamDeviceClass;
   ushort   u16_HeatCoolProgramState;
   ushort   u16_StatusDataLoaded;
   ushort   u16_ErrorCodeSW_Base;
   struct   _S_ColumnOvenRegisters_StatusDataExtern   S_StatusDataExtern;
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub   =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // read extern status data
   S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
   S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
   S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
   S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);
   S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
   S_ModbusAccessParams.pvIntern     =  NULL;
   S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
   }
   // load of status data shall be only possible if no heat/cool program is active
   if (     (S_StatusDataExtern.u16_ColumnOvenState != (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD)
         && (S_StatusDataExtern.u16_ColumnOvenState != (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN)
         )
   {
      // read out sykam device data option field
      S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
      S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START;
      S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceOptions)/2);
      S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceOptions);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
      }
      // read out sykam device data option field
      S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
      S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START;
      S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceClass)/2);
      S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceClass);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
      }
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
      // check device option whether to set maximum number heat/cool programs standard or extended
      if (     (u16_SykamDeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
            && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM)
            && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
            && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
            && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
            )
      {
         S_StatusDataExtern.u16_HeatCoolProgramMaxId        =  (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID;
         S_StatusDataExtern.u16_HeatCoolProgramMaxSections  =  (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_SECTIONS;
         // read last used heat/cool program identifier from fixed memory
         u16_NumBytes      =  sizeof (u16_FixedMemData);
         u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_CURRENT_ID;
         if (FAIL_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_FixedMemData), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
                  *pu32_ErrorCodeSW_Sub))
         {
            return u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__STATUS_DATA_LOAD;
         }
      }
      else
#endif
      {
         S_StatusDataExtern.u16_HeatCoolProgramMaxId        =  (ushort)AT91SAM7X__MAX_HEAT_PROGRAM_ID;
         S_StatusDataExtern.u16_HeatCoolProgramMaxSections  =  (ushort)AT91SAM7X__MAX_HEAT_PROGRAM_SECTIONS;
         // set default heat/cool program identifier to load
         u16_FixedMemData                                   =  (ushort)COLUMN_OVEN_CMDS_USER__DEFAULT_HEAT_COOL_PROGRAM_ID;
      }
      // check last used heat/cool program identifier
      if ((u16_FixedMemData == 0) || (u16_FixedMemData > S_StatusDataExtern.u16_HeatCoolProgramMaxId))
      {
         // set default heat/cool program identifier to load
         u16_FixedMemData = (ushort)COLUMN_OVEN_CMDS_USER__DEFAULT_HEAT_COOL_PROGRAM_ID;
         // delete/reset first heat/cool program if wrong identifier was saved on fixed memory
         if (FAIL_APP_FUNC (ColumnOvenCmdsUser_HeatCoolProgramDelete (u16_FixedMemData, pu32_ErrorCodeSW_Sub),
                  u16_ErrorCodeSW_Base))
         {
            return u16_ErrorCodeSW_Base;
         }
      }
      // load last used heat/cool program
      else
      {
         if (FAIL_APP_FUNC (ColumnOvenSupportFuncs_HeatCoolProgramLoad (u16_FixedMemData, (void *)&S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0]),
                  *pu32_ErrorCodeSW_Sub))
         {
            // reset heat/cool program inside register bank if profile could not be loaded from fixed memory
            memset ((void *)&S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0], 0, sizeof (tS_ColumnOvenRegisters_HeatCoolProgram));
            // set defaults
            S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint =  (ushort)AT91SAM7X__DEFAULT_HEAT_COOL_PROGRAM_SET_POINT_IN_DEGREE_MUL_TEN;
            S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_HoldTime =  0;
         }
      }
      // set entries of external status data which are appropriate
      S_StatusDataExtern.u32_HeatCoolProgramTimeTotal                =  S_StatusDataExtern.S_HeatCoolProgramInUse.u16_TimeTotal;
      S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined   =  S_StatusDataExtern.S_HeatCoolProgramInUse.u16_NumActiveSections;
      S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
      S_StatusDataExtern.u16_HeatCoolProgramCurrentId                =  u16_FixedMemData;
      // set global flag that signal processing task does refresh status data
      b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
      // write the loaded status data into register bank
      S_ModbusAccessParams.ucType   =  (uchar)  MBPB_REGTYPE_INPUT;
      S_ModbusAccessParams.bRW      =  (uchar)  MBPB_REGRW_WRITE;
      S_ModbusAccessParams.usAddr   =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
      S_ModbusAccessParams.usCount  =  (ushort) (sizeof(S_StatusDataExtern)/2);
      S_ModbusAccessParams.pvData   =  (void *) (&S_StatusDataExtern);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
      }
      // set and write heat/cool program state
      u16_HeatCoolProgramState   =  (ushort) COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__STOP;
      S_ModbusAccessParams.usAddr   =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_HEAT_COOL_PROGRAM;
      S_ModbusAccessParams.usCount  =  (ushort) (sizeof(u16_HeatCoolProgramState)/2);
      S_ModbusAccessParams.pvData   =  (void *) (&u16_HeatCoolProgramState);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         // set and write status data loaded flag
         u16_StatusDataLoaded          =  (ushort)COLUMN_OVEN_REGISTERS__STATUS_DATA_LOADED;
         S_ModbusAccessParams.usAddr   =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_LOADED;
         S_ModbusAccessParams.usCount  =  (ushort) (sizeof(u16_StatusDataLoaded)/2);
         S_ModbusAccessParams.pvData   =  (void *) (&u16_StatusDataLoaded);
         TRY_AND_CATCH
         (
         MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
         *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
         );
      }
   }  // place a warning here if desired
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__STATUS_DATA_LOAD;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
////----------------------------------------------------------------------------------------------------------------------------------------------
//ushort ColumnOvenCmdsOwner_DeviceParamsLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
//{
//   ushort   u16_NumBytes;
//   ushort   u16_DestAddress;
//   ushort   u16_TemperatureOffset;
//   ushort   u16_TemperatureUnit;
//   ushort   u16_HeatCoolProgramSetPointReachedBand;
//   ushort   u16_ColumnOvenState;
//   ushort   u16_DeviceParamsLoaded;
//   ushort   u16_StateControlSectionParams;
//   ushort   u16_ControlSectionsNum;
//   ushort   u16_SykamDeviceOptions;
//   ushort   u16_SykamDeviceClass;
//   ushort   u16_ErrorCodeSW_Base;
//   struct   _S_ColumnOvenRegisters_DeviceParamsIntern S_DeviceParamsIntern;
//   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
//   tS_LibExceptions_Exceptions                        S_Exceptions;
//   // reset of structure is necessary!
//   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
//   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
//   *pu32_ErrorCodeSW_Sub   =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
//   // read column oven state
//   S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
//   S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
//   S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_STATE;
//   S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
//   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_ColumnOvenState)/2);
//   S_ModbusAccessParams.pvData       =  (void *) (&u16_ColumnOvenState);
//   S_ModbusAccessParams.pvIntern     =  NULL;
//   S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
//   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//   {
//      // load of status data shall be only possible if column oven is inactive
//      if (     (u16_ColumnOvenState != (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD)
//            && (u16_ColumnOvenState != (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN)
//            )
//      {
//         u16_NumBytes      =  sizeof (S_DeviceParamsIntern);
//         u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START;
//         // read internal device params from fixed memory
//         if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_DeviceParamsIntern), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
//                  *pu32_ErrorCodeSW_Sub))
//         {
//            // read last user temperature offset from fixed memory
//            u16_NumBytes      =  sizeof (u16_TemperatureOffset);
//            u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__TEMPERATURE_OFFSET;
//            if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_TemperatureOffset), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
//                     *pu32_ErrorCodeSW_Sub))
//            {
//               // read last user temperature unit (this is only a flag to indicate that PC or another remote control must send temperature
//               // values in appropriate manner)
//               u16_NumBytes      =  sizeof (u16_TemperatureUnit);
//               u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__TEMPERATURE_UNIT;
//               if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_TemperatureUnit), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
//                        *pu32_ErrorCodeSW_Sub))
//               {
//                  // read last user temperature set point reached band
//                  u16_NumBytes      =  sizeof (u16_HeatCoolProgramSetPointReachedBand);
//                  u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND;
//                  if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_HeatCoolProgramSetPointReachedBand), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
//                           *pu32_ErrorCodeSW_Sub))
//                  {
//                     // load "device params loaded" code and flags from fixed memory
//                     u16_NumBytes      =  sizeof (u16_DeviceParamsLoaded);
//                     u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOADED;
//                     if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_DeviceParamsLoaded), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
//                              *pu32_ErrorCodeSW_Sub))
//                     {
//                        // check for never stored device params code (once one flag was saved the code is available)
//                        if ((u16_DeviceParamsLoaded & (~(ushort)COLUMN_OVEN_CMDS_REG__MASK_DEVICE_PARAMS_STORED)) != ((ushort)COLUMN_OVEN_CMDS_REG__CODE_DEVICE_PARAMS_STORED))
//                        {
//                           u16_TemperatureOffset                     =  (ushort)AT91SAM7X__DEFAULT_TEMPERATURE_OFFSET_MUL_TEN;
//                           u16_TemperatureUnit                       =  (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__CELSIUS;
//                           u16_HeatCoolProgramSetPointReachedBand    =  (ushort)AT91SAM7X__DEFAULT_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN;
//                           if (FAIL_APP_FUNC (ColumnOvenCmdsReg_TemperatureOffset (u16_TemperatureOffset, &S_Exceptions.u32_ErrorCodeSW_Sub),
//                                    S_Exceptions.u16_ErrorCodeSW_Base))
//                           {
//                              // set exception codes to register bank
//                              LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
//                           }
//                           if (FAIL_APP_FUNC (ColumnOvenCmdsReg_TemperatureUnit (u16_TemperatureUnit, &S_Exceptions.u32_ErrorCodeSW_Sub),
//                                    S_Exceptions.u16_ErrorCodeSW_Base))
//                           {
//                              // set exception codes to register bank
//                              LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
//                           }
//                           if (FAIL_APP_FUNC (ColumnOvenCmdsReg_HeatCoolProgramSetPointReachedBand (u16_HeatCoolProgramSetPointReachedBand, &S_Exceptions.u32_ErrorCodeSW_Sub),
//                                    S_Exceptions.u16_ErrorCodeSW_Base))
//                           {
//                              // set exception codes to register bank
//                              LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
//                           }
//                        }
//                        else
//                        {
//                           // check if device param flags were saved before - if not set default values
//                           if ((u16_DeviceParamsLoaded & (ushort)COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_OFFSET_STORED) != ((ushort)COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_OFFSET_STORED))
//                           {
//                              u16_TemperatureOffset = (ushort)AT91SAM7X__DEFAULT_TEMPERATURE_OFFSET_MUL_TEN;
//                           }
//                           if ((u16_DeviceParamsLoaded & (ushort)COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_UNIT_STORED) != ((ushort)COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_UNIT_STORED))
//                           {
//                              u16_TemperatureUnit = (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__CELSIUS;
//                           }
//                           if ((u16_DeviceParamsLoaded & (ushort)COLUMN_OVEN_CMDS_REG__FLAG_HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND_STORED) != ((ushort)COLUMN_OVEN_CMDS_REG__FLAG_HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND_STORED))
//                           {
//                              u16_HeatCoolProgramSetPointReachedBand = (ushort)AT91SAM7X__DEFAULT_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN;
//                           }
//                        }
//                        // set global flag that signal processing task does refresh status data
//                        b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
//                        // check temperature offset value and set default if necessary
//                        if (u16_TemperatureOffset > (ushort)AT91SAM7X__MAX_TEMPERATURE_OFFSET_MUL_TEN)
//                        u16_TemperatureOffset = (ushort)AT91SAM7X__DEFAULT_TEMPERATURE_OFFSET_MUL_TEN;
//                        // write temperature offset into register bank
//                        S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
//                        S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
//                        S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_OFFSET;
//                        S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_TemperatureOffset)/2);
//                        S_ModbusAccessParams.pvData       =  (void *) (&u16_TemperatureOffset);
//                        if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//                        {
//                           // check temperature unit and set default if necessary
//                           if ((u16_TemperatureUnit >= (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNITS_MAX) || (u16_TemperatureUnit == (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__NONE))
//                           u16_TemperatureUnit = (ushort)COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__CELSIUS;
//                           // write temperature unit into register bank
//                           S_ModbusAccessParams.usAddr = (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_UNIT;
//                           S_ModbusAccessParams.pvData = (void *)(&u16_TemperatureUnit);
//                           if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//                           {
//                              // check set point reached band value and set default if necessary
//                              if (     (u16_HeatCoolProgramSetPointReachedBand > (ushort)AT91SAM7X__MAX_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN)
//                                    && (u16_HeatCoolProgramSetPointReachedBand < (ushort)AT91SAM7X__MIN_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN))
//                              {
//                                 u16_HeatCoolProgramSetPointReachedBand = (ushort)AT91SAM7X__DEFAULT_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN;
//                              }
//                              // write value of set point reached band into register bank
//                              S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND;
//                              S_ModbusAccessParams.pvData       =  (void *) (&u16_HeatCoolProgramSetPointReachedBand);
//                              if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//                              {
//                                 // check loaded device params (this function returns also the number of sections)
//                                 if (SUCCESS_APP_FUNC (ColumnOvenSupportFuncs_DeviceParamsInternCheck ((void *)&S_DeviceParamsIntern, &u16_ControlSectionsNum, (bool)COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_READ),
//                                          *pu32_ErrorCodeSW_Sub))
//                                 {
//                                 }
//                                 // validation failed: set default control section params into register bank dependend on device option
//                                 else
//                                 {
//                                    // read out sykam device data option field
//                                    S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
//                                    S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START;
//                                    S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceOptions)/2);
//                                    S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceOptions);
//                                    if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//                                    {
//                                       // read out sykam device data option field
//                                       S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
//                                       S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START;
//                                       S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceClass)/2);
//                                       S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceClass);
//                                       if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//                                       {
//#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
//                                          // check device option which default control section params shall be set
//                                          if ((u16_SykamDeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN) && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL))
//                                          {
//                                             memcpy ((void *)(& S_DeviceParamsIntern.SA_ControlSectionParams[0]), (const void *)(& cSSA_ColumnOvenControlParams[COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL][0]), sizeof(S_DeviceParamsIntern.SA_ControlSectionParams));
//                                          }
//                                          else if ((u16_SykamDeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN) && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
//                                          {
//                                             memcpy ((void *)(& S_DeviceParamsIntern.SA_ControlSectionParams[0]), (const void *)(& cSSA_ColumnOvenControlParams[COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT][0]), sizeof(S_DeviceParamsIntern.SA_ControlSectionParams));
//                                          }
//                                          else if ((u16_SykamDeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN) && ((u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL) && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)))
//#endif
//                                          {
//                                             memcpy ((void *)(& S_DeviceParamsIntern.SA_ControlSectionParams[0]), (const void *)(& cSSA_ColumnOvenControlParams[COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER][0]), sizeof(S_DeviceParamsIntern.SA_ControlSectionParams));
//                                          }
//#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
//                                          else
//                                          {
//                                             *pu32_ErrorCodeSW_Sub         = (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__DEVICE_OPTION_IS_NOT_ACTIVATED;
//                                             return u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD;
//                                          }
//#endif
//                                          // check default control section params and count them
//                                          if (FAIL_APP_FUNC (ColumnOvenSupportFuncs_DeviceParamsInternCheck ((void *)&S_DeviceParamsIntern, &u16_ControlSectionsNum, (bool)COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_WRITE),
//                                                   *pu32_ErrorCodeSW_Sub))
//                                          {
//                                             return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD;
//                                          }
//                                       }
//                                       else
//                                       return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD;
//                                    }
//                                    else
//                                    return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD;
//                                 }
//                                 // write validated device params into register bank
//                                 S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
//                                 S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START;
//                                 S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_DeviceParamsIntern)/2);
//                                 S_ModbusAccessParams.pvData       =  (void *) (&S_DeviceParamsIntern);
//                                 if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//                                 {
//                                    // write flag that device params intern were loaded
//                                    u16_DeviceParamsLoaded           =  (ushort) COLUMN_OVEN_REGISTERS__DEVICE_PARAMS_LOADED;
//                                    S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_INPUT;
//                                    S_ModbusAccessParams.usAddr      =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__DEVICE_PARAMS_LOADED;
//                                    S_ModbusAccessParams.usCount     =  (ushort) (sizeof(u16_DeviceParamsLoaded)/2);
//                                    S_ModbusAccessParams.pvData      =  (void *) (&u16_DeviceParamsLoaded);
//                                    if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//                                    {
//                                       S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_INPUT;
//                                       S_ModbusAccessParams.usAddr      =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_SECTIONS_NUM;
//                                       S_ModbusAccessParams.usCount     =  (ushort) (AT91SAM7X__NUM_REGS_1);
//                                       S_ModbusAccessParams.pvData      =  (void *) (&u16_ControlSectionsNum);
//                                       if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//                                       {
//                                          // write flag that control sections params were loaded
//                                          u16_StateControlSectionParams    =  (ushort) COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__LOADED;
//                                          S_ModbusAccessParams.usAddr      =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_CONTROL_SECTION_PARAMS;
//                                          S_ModbusAccessParams.usCount     =  (ushort) (sizeof(u16_StateControlSectionParams)/2);
//                                          S_ModbusAccessParams.pvData      =  (void *) (&u16_StateControlSectionParams);
//                                          TRY_AND_CATCH
//                                          (
//                                          MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
//                                          *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
//                                          );
//                                       }
//                                    }
//                                 }
//                              }
//                           }
//                        }
//                     }
//                  }
//               }
//            }
//         }
//      }
//   }
//   if (*pu32_ErrorCodeSW_Sub)
//   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD;
//   return u16_ErrorCodeSW_Base;
//}
//----------------------------------------------------------------------------------------------------------------------------------------------
//ushort ColumnOvenCmdsOwner_StatusDataLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
//{
//   ushort   u16_NumBytes;
//   ushort   u16_DestAddress;
//   ushort   u16_FixedMemData;
//   ushort   u16_SykamDeviceOptions;
//   ushort   u16_SykamDeviceClass;
//   ushort   u16_HeatCoolProgramState;
//   ushort   u16_StatusDataLoaded;
//   ushort   u16_ErrorCodeSW_Base;
//   struct   _S_ColumnOvenRegisters_StatusDataExtern   S_StatusDataExtern;
//   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
//   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
//   *pu32_ErrorCodeSW_Sub   =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
//   // read extern status data
//   S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
//   S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
//   S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
//   S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
//   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);
//   S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
//   S_ModbusAccessParams.pvIntern     =  NULL;
//   S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
//   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//   {
//      // load of status data shall be only possible if no heat/cool program is active
//      if (     (S_StatusDataExtern.u16_ColumnOvenState != (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD)
//            && (S_StatusDataExtern.u16_ColumnOvenState != (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN)
//            )
//      {
//         // read out sykam device data option field
//         S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
//         S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START;
//         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceOptions)/2);
//         S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceOptions);
//         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//         {
//            // read out sykam device data option field
//            S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
//            S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START;
//            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceClass)/2);
//            S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceClass);
//            if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//            {
//#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
//               // check device option whether to set maximum number heat/cool programs standard or extended
//               if (     (u16_SykamDeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
//                     && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM)
//                     && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
//                     && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
//                     && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
//                     )
//               {
//                  S_StatusDataExtern.u16_HeatCoolProgramMaxId        =  (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID;
//                  S_StatusDataExtern.u16_HeatCoolProgramMaxSections  =  (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_SECTIONS;
//                  // read last used heat/cool program identifier from fixed memory
//                  u16_NumBytes      =  sizeof (u16_FixedMemData);
//                  u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_CURRENT_ID;
//                  if (FAIL_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_FixedMemData), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
//                           *pu32_ErrorCodeSW_Sub))
//                  {
//                     return u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__STATUS_DATA_LOAD;
//                  }
//               }
//               else
//#endif
//               {
//                  S_StatusDataExtern.u16_HeatCoolProgramMaxId        =  (ushort)AT91SAM7X__MAX_HEAT_PROGRAM_ID;
//                  S_StatusDataExtern.u16_HeatCoolProgramMaxSections  =  (ushort)AT91SAM7X__MAX_HEAT_PROGRAM_SECTIONS;
//                  // set default heat/cool program identifier to load
//                  u16_FixedMemData                                   =  (ushort)COLUMN_OVEN_CMDS_USER__DEFAULT_HEAT_COOL_PROGRAM_ID;
//               }
//               // check last used heat/cool program identifier
//               if ((u16_FixedMemData == 0) || (u16_FixedMemData > S_StatusDataExtern.u16_HeatCoolProgramMaxId))
//               {
//                  // set default heat/cool program identifier to load
//                  u16_FixedMemData = (ushort)COLUMN_OVEN_CMDS_USER__DEFAULT_HEAT_COOL_PROGRAM_ID;
//                  // delete/reset first heat/cool program if wrong identifier was saved on fixed memory
//                  if (FAIL_APP_FUNC (ColumnOvenCmdsUser_HeatCoolProgramDelete (u16_FixedMemData, pu32_ErrorCodeSW_Sub),
//                           u16_ErrorCodeSW_Base))
//                  {
//                     return u16_ErrorCodeSW_Base;
//                  }
//               }
//               // load last used heat/cool program
//               else
//               {
//                  if (FAIL_APP_FUNC (ColumnOvenSupportFuncs_HeatCoolProgramLoad (u16_FixedMemData, (void *)&S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0]),
//                           *pu32_ErrorCodeSW_Sub))
//                  {
//                     // reset heat/cool program inside register bank if profile could not be loaded from fixed memory
//                     memset ((void *)&S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0], 0, sizeof (tS_ColumnOvenRegisters_HeatCoolProgram));
//                     // set defaults
//                     S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint =  (ushort)AT91SAM7X__DEFAULT_HEAT_COOL_PROGRAM_SET_POINT_IN_DEGREE_MUL_TEN;
//                     S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_HoldTime =  0;
//                  }
//               }
//               // set entries of external status data which are appropriate
//               S_StatusDataExtern.u32_HeatCoolProgramTimeTotal                =  S_StatusDataExtern.S_HeatCoolProgramInUse.u16_TimeTotal;
//               S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined   =  S_StatusDataExtern.S_HeatCoolProgramInUse.u16_NumActiveSections;
//               S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
//               S_StatusDataExtern.u16_HeatCoolProgramCurrentId                =  u16_FixedMemData;
//               // set global flag that signal processing task does refresh status data
//               b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
//               // write the loaded status data into register bank
//               S_ModbusAccessParams.ucType   =  (uchar)  MBPB_REGTYPE_INPUT;
//               S_ModbusAccessParams.bRW      =  (uchar)  MBPB_REGRW_WRITE;
//               S_ModbusAccessParams.usAddr   =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
//               S_ModbusAccessParams.usCount  =  (ushort) (sizeof(S_StatusDataExtern)/2);
//               S_ModbusAccessParams.pvData   =  (void *) (&S_StatusDataExtern);
//               if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//               {
//                  // set and write heat/cool program state
//                  u16_HeatCoolProgramState   =  (ushort) COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__STOP;
//                  S_ModbusAccessParams.usAddr   =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_HEAT_COOL_PROGRAM;
//                  S_ModbusAccessParams.usCount  =  (ushort) (sizeof(u16_HeatCoolProgramState)/2);
//                  S_ModbusAccessParams.pvData   =  (void *) (&u16_HeatCoolProgramState);
//                  if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//                  {
//                     // set and write status data loaded flag
//                     u16_StatusDataLoaded          =  (ushort)COLUMN_OVEN_REGISTERS__STATUS_DATA_LOADED;
//                     S_ModbusAccessParams.usAddr   =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_LOADED;
//                     S_ModbusAccessParams.usCount  =  (ushort) (sizeof(u16_StatusDataLoaded)/2);
//                     S_ModbusAccessParams.pvData   =  (void *) (&u16_StatusDataLoaded);
//                     TRY_AND_CATCH
//                     (
//                     MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
//                     *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
//                     );
//                  }
//               }
//            }
//         }
//      }  // place a warning here if desired
//   }
//   if (*pu32_ErrorCodeSW_Sub)
//   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__STATUS_DATA_LOAD;
//   return u16_ErrorCodeSW_Base;
//}
////----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsOwner_HeatCoolProgramsReset (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_Idx;
   ushort   u16_ErrorCodeSW_Base;
   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub   =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   for (u16_Idx = 1; u16_Idx <= (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID; u16_Idx++)
   {
      if (FAIL_APP_FUNC (ColumnOvenCmdsUser_HeatCoolProgramDelete (u16_Idx, pu32_ErrorCodeSW_Sub), u16_ErrorCodeSW_Base))
      break;
   }
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
