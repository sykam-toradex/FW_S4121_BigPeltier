/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
**  \file                column_oven_cmds_user.c
**  \brief               This file contains the \em user command functions which will be executed if sending the appropriate
*                       command via modbus (MBPB). Execution of these functions is only allowed to user, owner (manufacturer) or
*                       service technician. Further details for commands in this file are listed in \ref column_oven_cmds_user.h \n
*                       Every function returns an base and sub error code which is greater than zero in error case.
**  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
**  \date                2010-02-03     Initial version imported from S6510_cmd_functions_user.c.
*  \date                2011-08-10     Renamed file from S4121_cmd_functions_user.c
**
*  \bug                 No bugs known at moment. \n
**  \test
**  \todo
**
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.H>//debug
#include <stdlib.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\ext\spi\nvram\fm25clss\fm25clss.h"
#include "..\..\..\ext\spi\nvram\fm25clss\fm25clssext.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\utils_funcs.h"
#include "column_oven_registers.h"
#include "column_oven_cmds_user.h"
#include "column_oven_signal_processing_funcs.h"
#include "column_oven_support_funcs.h"
#include "column_oven_support_tasks.h"
#pragma import(__use_realtime_heap)             //< * this pragma causes the use of a faster memory allocation (internal) function */
//---------------------------------------------- global definitions ---------------------------------------------------------------------------
// The size and order must be the same like in documentation (excel table)
tfp_ColumnOvenCmdsUser_CMD   fpA_ColumnOvenCmdsUser_CMDs[] =
{   
   { &ColumnOvenCmdsUser_HeatCoolProgramStart               },
   { &ColumnOvenCmdsUser_HeatCoolProgramStop                },
   { &ColumnOvenCmdsUser_HeatCoolProgramDelete              },
   { &ColumnOvenCmdsUser_HeatCoolProgramSave                },
   { &ColumnOvenCmdsUser_HeatCoolProgramSelectToModify      },
   { &ColumnOvenCmdsUser_HeatCoolCalibrationStart           }
   //
};
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern            bool     b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified;
extern   const    ushort   cu16_TimeoutShort;
//---------------------------------------------- user command functions ------------------------------------------------------------------------
ushort ColumnOvenCmdsUser_HeatCoolProgramStart (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{   
   ushort   u16_Temp_setpoint;
   ushort   u16_HeatCoolProgramId = (u16_ModbusRxData & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED_ID);
   ushort   u16_FixedMemData;
   ushort   u16_SykamDeviceOptions;
   ushort   u16_SykamDeviceClass;
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_ErrorCodeSW_Base          =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   struct   _S_ColumnOvenRegisters_StatusDataIntern   S_StatusDataIntern;
   struct   _S_ColumnOvenRegisters_StatusDataExtern   S_StatusDataExtern;
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   u16_ModbusRxData=u16_ModbusRxData;
   ////*/*/*/*/*/*/*/*/**////////////////////////////////////////////////////////////////
   // subtract/add user given temperature offset
   if (u16_ModbusRxData & 0x8000)
   u16_Temp_setpoint = u16_ModbusRxData ^ 0x8000;
   else
   u16_Temp_setpoint = (u16_ModbusRxData);
   ////*/*/*/*/*/*/*/*/**////////////////////////////////////////////////////////////////
   //////*/*/*/*/*/*/*/*/**////////////////////////////////////////////////////////////////
   // read out whole status data from register bank to check different device states and to set some status data which must be set
   // in order to start heat/cool process
   S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
   S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
   S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;  // register start address
   S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);                                      // total words of S_StatusDataExtern
   S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
   S_ModbusAccessParams.pvIntern     =  NULL;                                                                         // debug purpose, set to &pv_BufferAddress if needed
   S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
   }   
   // read out INtern status data
   S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
   S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
   }      
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
   // check for gas alarm
   if (S_StatusDataIntern.u16_GasSensorAlarmDetected != (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_ALARM)
   {
      if (     !(S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD)            
         )
      {
         // reset program times
         S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed        =  0;
         S_StatusDataExtern.u32_HeatCoolProgramTimeRemain         =  0;
         S_StatusDataExtern.u32_HeatCoolProgramTimeSectionRemain  =  0;
         // reset control deviation
         S_StatusDataIntern.u32_ControlDeviationAccumulated       =  0x00000000;
         S_StatusDataIntern.u32_ControlDeviationLast              =  0x00000000;
         // reset internal cycle counter
         S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount      =  0x00000000;
         // set temperature program section identifier
         S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId   =  0;
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
         // check device option whether load a program or start with a passed temperature
         if (     (u16_SykamDeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
               && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM)
               && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
               && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
               && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
               )
         {                                                                                       
            if ((u16_ModbusRxData & ((ushort)~COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_START_DIRECTLY)) <= ((ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID))
            {
               // load temperature program with passed ID
               if (SUCCESS_APP_FUNC (ColumnOvenSupportFuncs_HeatCoolProgramLoad (u16_HeatCoolProgramId, (void *)&S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0]), *pu32_ErrorCodeSW_Sub))
               {
                  // set temperature program identifier
                  S_StatusDataExtern.u16_HeatCoolProgramCurrentId = u16_HeatCoolProgramId;
                  // set defined sections
                  if (S_StatusDataExtern.S_HeatCoolProgramInUse.u16_NumActiveSections < S_StatusDataExtern.u16_HeatCoolProgramMaxSections)
                  S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined = S_StatusDataExtern.S_HeatCoolProgramInUse.u16_NumActiveSections;
                  else
                  *pu32_ErrorCodeSW_Sub = (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_PROGRAM;
                  // set first set point
                  S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
                  // set program times (we change to hold state, i. e. there is no section time)
                  S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal         =  0;
                  S_StatusDataExtern.u32_HeatCoolProgramTimeTotal                =  S_StatusDataExtern.S_HeatCoolProgramInUse.u16_TimeTotal;
                  // check for direct program start
                  if (u16_ModbusRxData & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_START_DIRECTLY)
                  {
                     // set defined sections
                     S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined   =  S_StatusDataExtern.S_HeatCoolProgramInUse.u16_NumActiveSections;
                     // set section identifier
                     S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId         =  1;
                     // set section set point
                     S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId].u16_SetPoint;
                     S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed              =  0;
                     S_StatusDataExtern.u32_HeatCoolProgramTimeTotal                =  S_StatusDataExtern.S_HeatCoolProgramInUse.u16_TimeTotal;
                     S_StatusDataExtern.u32_HeatCoolProgramTimeRemain               =  S_StatusDataExtern.u32_HeatCoolProgramTimeTotal;
                     S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal         =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId].u16_HoldTime;
                     S_StatusDataExtern.u32_HeatCoolProgramTimeSectionRemain        =  S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal;
                     // reset control deviations at column oven start
                     S_StatusDataIntern.u32_ControlDeviationAccumulated             =  0x00000000;
                     S_StatusDataIntern.u32_ControlDeviationLast                    =  0x00000000;
                     // reset internal cycle counter
                     S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount            =  0x00000000;
                     // set column oven state "run"
                     S_StatusDataExtern.u16_ColumnOvenState                         =  (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN;
                  }
                  else
                  S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD; // normal start
               }
            }
            else
            {

               //*pu32_ErrorCodeSW_Sub = (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_FUNCTION_ARGUMENTS;
               // else => ignore start command without error, i. e. the column oven "hold state" is then always active
            }
            /*****************NEW************ Change Setpoint****************/
            // set section set point
            S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint =  u16_Temp_setpoint;
            S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   = u16_Temp_setpoint;					   						                              
         }
         else
#endif
         {
            if (  (     (u16_Temp_setpoint <= ((ushort)AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL * 10))
                     && (u16_Temp_setpoint >= ((ushort)AT91SAM7X__MIN_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL * 10))
                     && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                     )
                  ||
                  (     (u16_Temp_setpoint <= ((ushort)AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER * 10))
                     && (u16_Temp_setpoint >= ((ushort)AT91SAM7X__MIN_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER * 10))
                     && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)
                     )
                  ||
                  (     (u16_Temp_setpoint <= ((ushort)AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT * 10))
                     && (u16_Temp_setpoint >= ((ushort)AT91SAM7X__MIN_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT * 10))
                     && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT)
                     )
                  )
            {
               // ### check if "ColumnOvenSupportFuncs_HeatCoolProgramLoad" function can be used here! ###
               // Set temperature program id to one "1" because for current device option there exists only one temperature set point
               // which will be stored always into same location
               u16_HeatCoolProgramId                                                =  (ushort)COLUMN_OVEN_CMDS_USER__DEFAULT_HEAT_COOL_PROGRAM_ID;
               S_StatusDataExtern.u16_HeatCoolProgramCurrentId                      =  u16_HeatCoolProgramId;
               // set defined sections always to zero "0" (it exists only one 'hold' temperature set point == zero active sections)
               S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined         =  0;
               // reset program times
               S_StatusDataExtern.u32_HeatCoolProgramTimeTotal                      =  0;
               S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal               =  0;
               S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint =  u16_Temp_setpoint;
               S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_HoldTime =  0;
               S_StatusDataExtern.S_HeatCoolProgramInUse.u16_NumActiveSections      =  0;
               // set first set point
               S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint         =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
               // set section set point
               S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint =  u16_Temp_setpoint;
               S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   = u16_Temp_setpoint;					                                 
               // check for direct program start
               // set column oven state
               if (u16_ModbusRxData & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_START_DIRECTLY)
               S_StatusDataExtern.u16_ColumnOvenState = S_StatusDataExtern.u16_ColumnOvenState;
               else
               S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN;
               ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
               // set section set point
               S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint =  u16_Temp_setpoint;
               S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   = u16_Temp_setpoint;					
               // set global flag that signal processing task does refresh status data
               b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
               // write whole status data into register bank
               S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
               S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
               S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;  // register start address
               S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);                                      // total words of S_StatusDataExtern
               S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
               MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams));
               // write INtern status data
               S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
               S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
               S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
               MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams));
               /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////               
               // save current user passed set point temperature inside default program on fixed memory
               u16_NumBytes      =  sizeof (tS_ColumnOvenRegisters_HeatCoolProgram);
               u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_IN_USE;
               if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_StatusDataExtern.S_HeatCoolProgramInUse), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
                        *pu32_ErrorCodeSW_Sub))
               {
                  u16_NumBytes      =  (ushort)AT91SAM7X__NUM_BYTES_2;
                  u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_STORED;
                  // read last stored identifiers and resave them with new/reused identifier
                  if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_FixedMemData), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
                           *pu32_ErrorCodeSW_Sub))
                  {
                     if ((u16_FixedMemData & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED) != ((ushort)COLUMN_OVEN_CMDS_USER__CODE_HEAT_COOL_PROGRAM_STORED))
                     u16_FixedMemData = 0;   // prevent errors if save is done on new hardware
                     // set program identifier and special save code
                     u16_FixedMemData  |= ((ushort)COLUMN_OVEN_CMDS_USER__CODE_HEAT_COOL_PROGRAM_STORED & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED) | (1 << ((u16_HeatCoolProgramId & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED_ID) - 1));
                     u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_STORED;
                     u16_NumBytes      =  (ushort)AT91SAM7X__NUM_BYTES_2;
                     // save program identifier and special save code to corresponding place on fixed memory
                     TRY_AND_CATCH_APP_FUNC
                     (
                     UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_FixedMemData), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
                     *pu32_ErrorCodeSW_Sub
                     );
                  }
               }
            }
//            else
            //*pu32_ErrorCodeSW_Sub = (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_FUNCTION_ARGUMENTS;
         }
         if (!(*pu32_ErrorCodeSW_Sub))
         {
            // write current used program id to fixed memory (is loaded at startup)
            u16_NumBytes      =  (ushort)AT91SAM7X__NUM_BYTES_2;
            u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_CURRENT_ID;
            TRY_AND_CATCH_APP_FUNC
            (
            UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_HeatCoolProgramId), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
            *pu32_ErrorCodeSW_Sub
            );
         }
      }
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
      else if (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD)
      {
         // exclude some sykam devices from doing temperature ramps
         if (     (u16_SykamDeviceClass & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
               && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM)
               && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
               && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
               && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
               )
         {
            if ((u16_ModbusRxData & ((ushort)~COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_START_DIRECTLY)) <= ((ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID))
            {
               if (S_StatusDataExtern.u16_HeatCoolProgramCurrentId == u16_HeatCoolProgramId)
               {
                  // set defined sections
                  S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined   =  S_StatusDataExtern.S_HeatCoolProgramInUse.u16_NumActiveSections;
                  // set section identifier
                  S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId         =  1;
                  // set section set point
                  S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId].u16_SetPoint;
                  S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed              =  0;
                  S_StatusDataExtern.u32_HeatCoolProgramTimeTotal                =  S_StatusDataExtern.S_HeatCoolProgramInUse.u16_TimeTotal;
                  S_StatusDataExtern.u32_HeatCoolProgramTimeRemain               =  S_StatusDataExtern.u32_HeatCoolProgramTimeTotal;
                  S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal         =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId].u16_HoldTime;
                  S_StatusDataExtern.u32_HeatCoolProgramTimeSectionRemain        =  S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal;
                  // reset control deviations at column oven start
                  S_StatusDataIntern.u32_ControlDeviationAccumulated             =  0x00000000;
                  S_StatusDataIntern.u32_ControlDeviationLast                    =  0x00000000;
                  // reset internal cycle counter
                  S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount            =  0x00000000;
                  // set column oven state "run"
                  S_StatusDataExtern.u16_ColumnOvenState                         =  (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN;
               }
            }
//            else
            //*pu32_ErrorCodeSW_Sub = (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_FUNCTION_ARGUMENTS;
         }
         // else => ignore start command without error, i. e. the column oven "hold state" is then always active
      }
      else 
      {
         if (  (     (u16_Temp_setpoint <= ((ushort)AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL * 10))
                  && (u16_Temp_setpoint >= ((ushort)AT91SAM7X__MIN_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL * 10))
                  && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                  )
               ||
               (     (u16_Temp_setpoint <= ((ushort)AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER * 10))
                  && (u16_Temp_setpoint >= ((ushort)AT91SAM7X__MIN_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER * 10))
                  && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)
                  )
               ||
               (     (u16_Temp_setpoint <= ((ushort)AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT * 10))
                  && (u16_Temp_setpoint >= ((ushort)AT91SAM7X__MIN_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT * 10))
                  && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT)
                  )
               )
         {
            // set section set point
            S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint =  u16_Temp_setpoint;
            S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   = u16_Temp_setpoint;					
            // set global flag that signal processing task does refresh status data
            b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
            // write whole status data into register bank
            S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
            S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
            S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;  // register start address
            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);                                      // total words of S_StatusDataExtern
            S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
            MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams));
            // write INtern status data
            S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
            S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
            MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams));
         }      
      }
#endif
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_START;
   return u16_ErrorCodeSW_Base;
}//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsUser_HeatCoolProgramStop (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{   
   ushort   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   struct   _S_ColumnOvenRegisters_StatusDataIntern   S_StatusDataIntern;
   struct   _S_ColumnOvenRegisters_StatusDataExtern   S_StatusDataExtern;
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // load whole status data
   S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
   S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
   S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;  // register start address
   S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);                                      // total words of S_StatusDataExtern
   S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
   S_ModbusAccessParams.pvIntern     =  NULL;                                                                         // debug purpose, set to &pv_BufferAddress if needed
   S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // read out INtern status data
      S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
      S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
      S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         // reset control deviation (not compulsory)
         S_StatusDataIntern.u32_ControlDeviationAccumulated             =  0x00000000;
         S_StatusDataIntern.u32_ControlDeviationLast                    =  0x00000000;
         // reset internal cycle counter (not compulsory)
         S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount            =  0x00000000;
         // set temperature program section identifier (zero equals hold state) and number of sections
         S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId         =  0;
         S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
         // ### for heat/cool set hold mode if stop command was called once (use a global variable to count number of calls)
         // set program times (do not reset total time)
         S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed              =  0;
         S_StatusDataExtern.u32_HeatCoolProgramTimeRemain               =  0;
         S_StatusDataExtern.u32_HeatCoolProgramTimeSectionRemain        =  0;
         S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal         =  0;
         // set column oven state
         S_StatusDataExtern.u16_ColumnOvenState                         =  (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP;
         // set global flag that signal processing task does refresh status data
         b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified       =  TRUE;
         // write whole status data into register bank
         S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
         S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
         S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;  // register start address
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);                                      // total words of S_StatusDataExtern
         S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            // write INtern status data
            S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
            S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
            if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
            }
         }
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_STOP;
   return u16_ErrorCodeSW_Base;
}//----------------------------------------------------------------------------------------------------------------------------------------------
// #################### check mask for program deletion!!! ############################################################
ushort ColumnOvenCmdsUser_HeatCoolProgramDelete (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   bool     b_HeatCoolProgramCanBeDeleted;
   ushort   u16_HeatCoolProgramIdToDelete;
   ushort   u16_HeatCoolProgramCurrentId;
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_FixedMemData;
   ushort   u16_ColumnOvenState;
   struct   _S_ColumnOvenRegisters_StatusDataExtern S_StatusDataExtern;
#endif
   ushort   u16_SykamDeviceOptions;
   ushort   u16_SykamDeviceClass;
   ushort   u16_ErrorCodeSW_Base;
   *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   u16_ErrorCodeSW_Base          =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   u16_HeatCoolProgramIdToDelete =  u16_ModbusRxData;
   b_HeatCoolProgramCanBeDeleted =  FALSE;
#endif
   // read out sykam device data option field
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START,
               (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_SykamDeviceOptions), NULL, cu16_TimeoutShort),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // read out sykam device class field
      if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START,
                  (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_SykamDeviceClass), NULL, cu16_TimeoutShort),
               *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
         // execute command only if correct device class and option is present - otherwise ignore command
         if (     (u16_SykamDeviceClass   & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
               && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
               && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
               )
         {
            // read column oven state and program id
            if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_STATE,
                        (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_ColumnOvenState), NULL, cu16_TimeoutShort),
                     *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_CURRENT_ID,
                           (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_HeatCoolProgramCurrentId), NULL, cu16_TimeoutShort),
                        *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  // heating/cooling process must be stopped if passed program id is equal to current program id
                  // OR deletion is possible if passed id is not equal to current one
                  if (u16_HeatCoolProgramCurrentId == u16_HeatCoolProgramIdToDelete)
                  {
                     if (u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP)
                     {
                        b_HeatCoolProgramCanBeDeleted = TRUE;
                     }
                     else
                     {
                        // a warning could be set to inform user about "first stop the active program and delete it afterwards"
                        b_HeatCoolProgramCanBeDeleted = FALSE;
                     }
                  }
                  else { b_HeatCoolProgramCanBeDeleted = TRUE; }
                  // delete a temperature program
                  if (b_HeatCoolProgramCanBeDeleted)
                  {
                     // reset temperature program
                     memset ((void *)&S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0], 0, sizeof (tS_ColumnOvenRegisters_HeatCoolProgram));
                     // set default values (first set point correponds to a hold time only, not a temperature program)
                     S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint =  (ushort)AT91SAM7X__DEFAULT_HEAT_COOL_PROGRAM_SET_POINT_IN_DEGREE_MUL_TEN;
                     S_StatusDataExtern.S_HeatCoolProgramInUse.u16_NumActiveSections      =  0;
                     u16_NumBytes      =  sizeof (tS_ColumnOvenRegisters_HeatCoolProgram);
                     u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_IN_USE + (u16_NumBytes * (u16_HeatCoolProgramIdToDelete - 1));
                     // write deleted program to fixed memory
                     if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0]), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
                              *pu32_ErrorCodeSW_Sub))
                     {
                        // set global flag that signal processing task does refresh status data
                        b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
                        u16_NumBytes      =  sizeof (u16_FixedMemData);
                        u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_STORED;
                        // read last stored identifiers and resave them with new/reused identifier
                        if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_FixedMemData), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
                                 *pu32_ErrorCodeSW_Sub))
                        {
                           if ((u16_FixedMemData & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED) != ((ushort)COLUMN_OVEN_CMDS_USER__CODE_HEAT_COOL_PROGRAM_STORED))
                           u16_FixedMemData = 0;   // prevent errors if save is done on new hardware
                           // set program identifier and special save code
                           u16_FixedMemData  |= ((ushort)COLUMN_OVEN_CMDS_USER__CODE_HEAT_COOL_PROGRAM_STORED & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED) | (1 << ((u16_HeatCoolProgramIdToDelete & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED_ID) - 1));
                           u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_STORED;
                           u16_NumBytes      =  sizeof (ushort);
                           // save program ID to corresponding place on fixed memory (even if the desired program was never saved before it can be loaded
                           // from now with default values)
                           if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_FixedMemData), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
                                    *pu32_ErrorCodeSW_Sub))
                           {
                              if (u16_HeatCoolProgramCurrentId == u16_HeatCoolProgramIdToDelete)
                              {
                                 // read external status data without current temperature program values
                                 if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_TIME_ELAPSED,
                                             (ushort)((sizeof (tS_ColumnOvenRegisters_StatusDataExtern) / 2) - (sizeof (tS_ColumnOvenRegisters_HeatCoolProgram) / 2)),
                                             (void *)(&S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed), NULL, cu16_TimeoutShort),
                                          *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                                 {
                                    // reset deleted program times, section identifier, number of sections and current section set point
                                    S_StatusDataExtern.u32_HeatCoolProgramTimeRemain               =  0;
                                    S_StatusDataExtern.u32_HeatCoolProgramTimeTotal                =  0;
                                    S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed              =  0;
                                    S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal         =  0;
                                    S_StatusDataExtern.u32_HeatCoolProgramTimeSectionRemain        =  0;
                                    // may be reset current section defined and section set point as well as current program id
                                    S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId         =  0;
                                    S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined   =  S_StatusDataExtern.S_HeatCoolProgramInUse.u16_NumActiveSections;
                                    S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
                                    // Write extern status data including temperature program into "current program in use" position in register bank. Note: sizeof "S_HeatCoolProgram" must be an even number of bytes!
                                    if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START,
                                                (ushort)((sizeof (tS_ColumnOvenRegisters_StatusDataExtern) / 2)),
                                                (void *)(&S_StatusDataExtern), NULL, cu16_TimeoutShort),
                                             *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                                    {
                                    }
                                 }
                              }
                           }
                        }
                     }
                  } // a user warning like "column oven must be stopped first" could be set
               }
            }
         }
#endif
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_DELETE;
   return u16_ErrorCodeSW_Base;
}//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsUser_HeatCoolProgramSave (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   bool     b_HeatCoolProgramZeroEntriesInModified    =  FALSE;
   uchar    u8_FlagHeatCoolProgram                    =  0;
   ushort   u16_HeatCoolProgramSectionCount;
   ushort   u16_FixedMemData;
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_Idx, u16_Idx2;
   ushort   u16_HeatCoolProgramIdToSave               =  u16_ModbusRxData;
#endif
   ushort   u16_ErrorCodeSW_Base                      =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   ushort   u16_SykamDeviceOptions;
   ushort   u16_SykamDeviceClass;
   ulong    **pu32_DataAddress;
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   struct   _S_ColumnOvenRegisters_StatusDataExtern   S_StatusDataExtern;
   struct   _S_ColumnOvenRegisters_DeviceParamsExtern S_DeviceParamsExtern;
#endif
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   *pu32_ErrorCodeSW_Sub   = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   pu32_DataAddress        = (ulong **)calloc ((size_t)(1), (size_t)sizeof (ulong));
   // if no memory left on heap return with an error
   if (!pu32_DataAddress)
   {
      u16_ErrorCodeSW_Base          = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_SAVE;
      return *pu32_ErrorCodeSW_Sub  = ((ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__NO_SPACE_LEFT_ON_HEAP);
   }
   // read out sykam device data option field
   S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
   S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
   S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START;  // register start address
   S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceOptions)/2);                                    // total words of u16_SykamDeviceOptions
   S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceOptions);
   S_ModbusAccessParams.pvIntern     =  NULL;                                                                           // debug purpose, set to &pv_BufferAddress if needed
   S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // read out sykam device data option field
      S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START;
      S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceClass)/2);
      S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceClass);
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
         // execute command only if correct device class and option is present - otherwise ignore command
         if (     (u16_SykamDeviceClass   & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
               && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
               && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
               )
         {
            // read EXtern status data
            S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
            S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);
            S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
            if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               // read out EXtern device params
               S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
               S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START;
               S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_DeviceParamsExtern)/2);
               S_ModbusAccessParams.pvData       =  (void *) (&S_DeviceParamsExtern);
               if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  if (S_StatusDataExtern.u16_HeatCoolProgramModifiedId != 0)
                  {
                     // 1) check user selected and modified temperature program for zero entries
                     for (u16_Idx = 1, u16_HeatCoolProgramSectionCount = 0; u16_Idx < S_StatusDataExtern.u16_HeatCoolProgramMaxSections; u16_Idx++)
                     {
                        if (     (S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[u16_Idx].u16_HoldTime != 0)
                              &&  (S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[u16_Idx].u16_SetPoint != 0)
                              )
                        {
                           u16_HeatCoolProgramSectionCount++;
                           b_HeatCoolProgramZeroEntriesInModified = FALSE;
                        }
                        else
                        {
                           // possibility that only set point for hold mode (section 0) was set - this results in a defined function error after the for-loop
                           if ((u16_Idx == 1) && (S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[0].u16_SetPoint == 0))
                           {
                              b_HeatCoolProgramZeroEntriesInModified = TRUE;
                              break;
                           }
                           // check if temperature program has zero entries from now on (ok) OR if program has zero entries inside (error case)
                           else
                           {
                              u16_Idx2 = u16_Idx + 1;
                              while (u16_Idx2 < S_StatusDataExtern.u16_HeatCoolProgramMaxSections)
                              {
                                 if (     (S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[u16_Idx2].u16_HoldTime != 0)
                                       &&  (S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[u16_Idx2].u16_SetPoint != 0)
                                       )
                                 {
                                    // set break condition for function and outer "for loop"
                                    b_HeatCoolProgramZeroEntriesInModified =  TRUE;
                                    u16_Idx                                =  S_StatusDataExtern.u16_HeatCoolProgramMaxSections;
                                    break;
                                 }
                                 u16_Idx2++;
                              }
                           }
                        }
                     }
                     if ((b_HeatCoolProgramZeroEntriesInModified) || (S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[0].u16_SetPoint == 0))
                     {
                        // step out of function because user did modify a temperature program with zero entries between valid entries // TODO: check if a user warning would be better
                        u16_ErrorCodeSW_Base    = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_SAVE;
                        *pu32_ErrorCodeSW_Sub   = (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_PROGRAM;
                        return u16_ErrorCodeSW_Base;
                     }
                     // reset parts of modified program where one entry, that means hold time or set point, could be missing
                     if (u16_HeatCoolProgramSectionCount)
                     memset ((void *)&S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[u16_HeatCoolProgramSectionCount + 1], 0, sizeof (tS_ColumnOvenRegisters_HeatCoolProgram) - (sizeof (tS_ColumnOvenRegisters_HeatCoolProgramSection) * (u16_HeatCoolProgramSectionCount + 1)) - (size_t)COLUMN_OVEN_REGISTERS__HEAT_COOL_PROGRAM_SIZE_WITHOUT_SECTIONS_IN_BYTES);
                     // set counted number of sections for storage (plus one for section "0")
                     S_DeviceParamsExtern.S_HeatCoolProgramToModify.u16_NumActiveSections = u16_HeatCoolProgramSectionCount;
                     // count and set total program time
                     S_DeviceParamsExtern.S_HeatCoolProgramToModify.u16_TimeTotal = 0;
                     for (u16_Idx = 1; u16_Idx <= u16_HeatCoolProgramSectionCount; u16_Idx++)
                     {
                        S_DeviceParamsExtern.S_HeatCoolProgramToModify.u16_TimeTotal += S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[u16_Idx].u16_HoldTime;
                     }
                     // 2) check if passed program identifier is current used program identifier OR if it is another one
                     if (u16_HeatCoolProgramIdToSave != S_StatusDataExtern.u16_HeatCoolProgramCurrentId)
                     {
                        u8_FlagHeatCoolProgram  =  (uchar)COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_NOT_CURRENT;
                     }
                     else if (   (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP)
                           || (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__FINISHED)
                           )
                     {
                        u8_FlagHeatCoolProgram  =  (uchar)COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_CURRENT_BUT_INACTIVE;
                     }
                     else if (   (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN)
                           || (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD)
                           )
                     {
                        u8_FlagHeatCoolProgram  =  (uchar)COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_CURRENT_AND_ACTIVE;
                     }
                     else
                     {
                        *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__DEVICE_IS_NOT_READY;
                        return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_SAVE;
                     }
                     // 3a)Case: program modified is not current one => Save program ONLY to fixed memory
                     //    Caution: On fixed memory all programs will be stored from address "COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_IN_USE"
                     //    but in register bank we only have one program in use and on program to modify.
                     if (u8_FlagHeatCoolProgram == (uchar)COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_NOT_CURRENT)
                     {
                        u16_NumBytes      =  sizeof (tS_ColumnOvenRegisters_HeatCoolProgram);
                        u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_IN_USE + (u16_NumBytes * (u16_HeatCoolProgramIdToSave - 1));
                        *pu32_DataAddress =  (ulong *)(&S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[0]);
                     }
                     // 3b)Case: program modified is current and running or in hold state => Save is only possible after current section ID, i.e. at least one section must be remaining.
                     else if (u8_FlagHeatCoolProgram == (uchar)COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_CURRENT_AND_ACTIVE)
                     {
                        // current section ID must be lower than new counted sections amount
                        if (S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId < S_DeviceParamsExtern.S_HeatCoolProgramToModify.u16_NumActiveSections)
                        {
                           // caution with length's
                           S_ModbusAccessParams.usAddr      =  ((ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_IN_USE) + ((sizeof (tS_ColumnOvenRegisters_HeatCoolProgramSection) / 2) * (S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId + 1));
                           S_ModbusAccessParams.usCount     =  (sizeof (tS_ColumnOvenRegisters_HeatCoolProgram) / 2) - ((sizeof (tS_ColumnOvenRegisters_HeatCoolProgramSection) / 2) * (S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId + 1));
                           S_ModbusAccessParams.pvData      =  (void *) (&S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId + 1]);
                           // number of bytes: total program size - entries up to current section id
                           u16_NumBytes                     =  sizeof (tS_ColumnOvenRegisters_HeatCoolProgram) - (sizeof (tS_ColumnOvenRegisters_HeatCoolProgramSection) * (S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId + 1));
                           u16_DestAddress                  =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_IN_USE + (sizeof (tS_ColumnOvenRegisters_HeatCoolProgram) * (u16_HeatCoolProgramIdToSave - 1)) + (sizeof (tS_ColumnOvenRegisters_HeatCoolProgramSection) * (S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId + 1));
                           // caution: watch which address is set
                           *pu32_DataAddress                =  (ulong *)(&S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId + 1]);
                           // set new system data which affects the state machine (NOTE: for run state the remaining time is to be set again)
                           S_StatusDataExtern.u32_HeatCoolProgramTimeTotal                =  S_DeviceParamsExtern.S_HeatCoolProgramToModify.u16_TimeTotal;
                           S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined   =  S_DeviceParamsExtern.S_HeatCoolProgramToModify.u16_NumActiveSections;
                           if (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN)
                           {
                              for (u16_Idx = S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId, u16_Idx2 = 0; u16_Idx < S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined; u16_Idx++)
                              {
                                 u16_Idx2 += S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[u16_Idx + 1].u16_HoldTime;
                              }
                              S_StatusDataExtern.u32_HeatCoolProgramTimeRemain += u16_Idx2;
                           }
                        }
                        else
                        {
                           // set a warning here or just ignore the case where user tries to save a modified and running program in last section
                           *pu32_ErrorCodeSW_Sub         =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
                           return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
                        }
                     }
                     // 3c)Case: program modified is current one but inactive => Save of total program into "program in use" position and to fixed memory.
                     else if (u8_FlagHeatCoolProgram == (uchar)COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_CURRENT_BUT_INACTIVE)
                     {
                        S_ModbusAccessParams.usAddr   =  (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_IN_USE;
                        S_ModbusAccessParams.usCount  =  sizeof (tS_ColumnOvenRegisters_HeatCoolProgram) / 2;
                        S_ModbusAccessParams.pvData   =  (void *) (&S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[0]);
                        u16_NumBytes                  =  sizeof (tS_ColumnOvenRegisters_HeatCoolProgram);
                        u16_DestAddress               =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_IN_USE + (u16_NumBytes * (u16_HeatCoolProgramIdToSave - 1));
                        *pu32_DataAddress             =  (ulong *)&S_DeviceParamsExtern.S_HeatCoolProgramToModify.SA_Section[0];
                        // set new system data which affects the state machine
                        S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal         =  0;
                        S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed              =  0;
                        S_StatusDataExtern.u32_HeatCoolProgramTimeTotal                =  S_DeviceParamsExtern.S_HeatCoolProgramToModify.u16_TimeTotal;
                        S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionsDefined   =  S_DeviceParamsExtern.S_HeatCoolProgramToModify.u16_NumActiveSections;
                     }
                     // 4) save program to fixed memory (always)
                     if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(*pu32_DataAddress), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
                              *pu32_ErrorCodeSW_Sub))
                     {
                        u16_NumBytes      =  sizeof (u16_FixedMemData);
                        u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_STORED;
                        // read last stored identifiers and resave them with new/reused identifier
                        if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_FixedMemData), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
                                 *pu32_ErrorCodeSW_Sub))
                        {
                           if ((u16_FixedMemData & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED) != ((ushort)COLUMN_OVEN_CMDS_USER__CODE_HEAT_COOL_PROGRAM_STORED))
                           u16_FixedMemData = 0;   // prevent errors if save is done on new hardware
                           // set program identifier and special save code
                           u16_FixedMemData  |= ((ushort)COLUMN_OVEN_CMDS_USER__CODE_HEAT_COOL_PROGRAM_STORED & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED) | (1 << ((u16_HeatCoolProgramIdToSave & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED_ID) - 1));
                           u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_STORED;
                           u16_NumBytes      =  sizeof (ushort);
                           // save program identifier and special save code to corresponding place on fixed memory
                           if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_FixedMemData), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
                                    *pu32_ErrorCodeSW_Sub))
                           {
                              // set modified ID to zero to indicate that modification is finished
                              S_StatusDataExtern.u16_HeatCoolProgramModifiedId = 0;
                              //                              if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_MODIFIED_ID,
                              //                                                          (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&S_StatusDataExtern.u16_HeatCoolProgramModifiedId), NULL, cu16_TimeoutShort),
                              //                                           *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                              {
                                 if (u8_FlagHeatCoolProgram != (uchar)COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_NOT_CURRENT)
                                 {
                                    // set global flag that signal processing task does refresh status data
                                    b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
                                    // set remaining register bank access params (for temperature program) - other set in preceeding code
                                    S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_INPUT;
                                    S_ModbusAccessParams.bRW         =  (bool)   MBPB_REGRW_WRITE;
                                    // write NEW program (partly) into register bank
                                    if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                                    {
                                       // set register bank access params for remaining extern status data
                                       S_ModbusAccessParams.usAddr   =  (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_TIME_ELAPSED;
                                       S_ModbusAccessParams.usCount  =  (sizeof (tS_ColumnOvenRegisters_StatusDataExtern) / 2) - (sizeof (tS_ColumnOvenRegisters_HeatCoolProgram) / 2);
                                       S_ModbusAccessParams.pvData   =  (void *) (&S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed);
                                       TRY_AND_CATCH
                                       (
                                       MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
                                       *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
                                       );
                                    }
                                    // #### set temperature program stored into intern status data
                                 }
                              }
                           }
                        }
                     }
                  }
                  //                  else
                  //                     // place a warning here "no program selected"
               }
            }
         }
#endif
      }
   }
   // free allocated memory
   free (pu32_DataAddress);
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_SAVE;
   return u16_ErrorCodeSW_Base;
}//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsUser_HeatCoolProgramSelectToModify (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   ushort   u16_HeatCoolProgramIdToSelect    =  u16_ModbusRxData;
   ushort   u16_HeatCoolProgramCurrentId;
#endif
   ushort   u16_ErrorCodeSW_Base             =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   ushort   u16_SykamDeviceOptions;
   ushort   u16_SykamDeviceClass;
   struct   _S_ColumnOvenRegisters_HeatCoolProgram S_HeatCoolProgram;
   memset ((void *)&S_HeatCoolProgram, 0, sizeof (S_HeatCoolProgram));
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // read out sykam device data option field
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START,
               (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_SykamDeviceOptions), NULL, cu16_TimeoutShort),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // read out sykam device data option field
      if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START,
                  (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_SykamDeviceClass), NULL, cu16_TimeoutShort),
               *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
         // execute this command function only if correct device class and option is present - otherwise ignore command
         if (     (u16_SykamDeviceClass   & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
               && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
               && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
               )
         {
            // read current program identifier from register bank
            if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_CURRENT_ID,
                        (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_HeatCoolProgramCurrentId), NULL, cu16_TimeoutShort),
                     *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               // heating/cooling process must be stopped if passed program id is equal to current program id
               // OR deletion is possible if passed id is not equal to current one
               if (u16_HeatCoolProgramCurrentId == u16_HeatCoolProgramIdToSelect)
               {
                  // Read current inactive/running/finished program from register bank. Note: sizeof "S_HeatCoolProgram" must be an even number of bytes!
                  TRY_AND_CATCH
                  (
                  MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_IN_USE,
                  (ushort)(sizeof (S_HeatCoolProgram) / 2), (void *)(&S_HeatCoolProgram), NULL, cu16_TimeoutShort),
                  *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
                  );
               }
               else
               {
                  // read selected program from fixed memory
                  if (FAIL_APP_FUNC (ColumnOvenSupportFuncs_HeatCoolProgramLoad (u16_HeatCoolProgramIdToSelect, (void *)&S_HeatCoolProgram),
                           *pu32_ErrorCodeSW_Sub))
                  {
                     // if load fails because the program is not stored, we use default values which can be modified by user afterwards
                     if (((*pu32_ErrorCodeSW_Sub & 0xFFFF0000) >> 16) == (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_PROGRAM_ID)
                     {
                        *pu32_ErrorCodeSW_Sub                        =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
                        S_HeatCoolProgram.u16_NumActiveSections      =  0;
                        S_HeatCoolProgram.SA_Section[0].u16_HoldTime =  0;
                        S_HeatCoolProgram.SA_Section[0].u16_SetPoint =  (ushort)AT91SAM7X__DEFAULT_HEAT_COOL_PROGRAM_SET_POINT_IN_DEGREE_MUL_TEN;
                     }
                  }
               }
            }
            if (!(*pu32_ErrorCodeSW_Sub))
            {
               // set global flag that signal processing task does refresh status data
               b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = TRUE;
               // Copy program into "program to modify" position of register bank. Note: sizeof "S_HeatCoolProgram" must be an even number of bytes!
               if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__HEAT_COOL_PROGRAM_TO_MODIFY,
                           (ushort)(sizeof (S_HeatCoolProgram) / 2), (void *)(&S_HeatCoolProgram), NULL, cu16_TimeoutShort),
                        *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  // write selected program identifier into status register bank
                  TRY_AND_CATCH
                  (
                  MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_MODIFIED_ID,
                  (ushort)(sizeof (u16_HeatCoolProgramIdToSelect) / 2), (void *)(&u16_HeatCoolProgramIdToSelect), NULL, cu16_TimeoutShort),
                  *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
                  );
               }
            }
         }
#endif
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_SELECT_TO_MODIFY;
   return u16_ErrorCodeSW_Base;
}//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ColumnOvenCmdsUser_HeatCoolCalibrationStart (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{   
   ushort   u16_ErrorCodeSW_Base =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   return u16_ErrorCodeSW_Base;
}//----------------------------------------------------------------------------------------------------------------------------------------------
