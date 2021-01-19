/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_support_funcs.c
*
*  \brief               The file contains several support functions for temperature control. See \ref column_oven_support_funcs.h
*                       for further information. \n
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
*  \test
*
*  \todo
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <math.h>
//#include <stdio.h>
#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085.h"
#include "..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085ext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\utils_funcs.h"
#include "column_oven_cmds_owner.h"
#include "column_oven_cmds_user.h"
#include "column_oven_cmds_reg.h"
#include "column_oven_registers.h"
#include "column_oven_support_tasks.h"
#include "column_oven_signal_processing_tasks.h"
#include "column_oven_signal_processing_funcs.h"
#include "column_oven_support_funcs.h"
#include "column_oven_control_params.h"
//---------------------------------------------- global definitions ---------------------------------------------------------------------------
OS_TID   TID_ColumnOvenSupportFuncs_GasSensing;
OS_TID   TID_ColumnOvenSupportFuncs_GlobalTimer;
OS_SEM   SEM_ColumnOvenSupportFuncs_InternalAdcChannel1;
OS_SEM   SEM_ColumnOvenSupportFuncs_InternalAdcChannel2;
OS_SEM   SEM_ColumnOvenSupportFuncs_InternalAdcChannel3;
OS_SEM   SEM_ColumnOvenSupportFuncs_InternalAdcChannel4;
OS_SEM   SEM_ColumnOvenSupportFuncs_InternalAdcChannel5;
OS_SEM   SEM_ColumnOvenSupportFuncs_InternalAdcChannel6;
OS_SEM   SEM_ColumnOvenSupportFuncs_InternalAdcChannel7;
OS_SEM   SEM_ColumnOvenSupportFuncs_InternalAdcChannel8;
OS_SEM   SEM_ColumnOvenSupportFuncs_GasSensing;
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
OS_TSK_STACK_USER( STK_TSK_ColumnOvenSupportFuncs_GasSensing, 1000 );    /**< define stack for \ref TSK_ColumnOvenSupportFuncs_GasSensing in [bytes] */
#endif
OS_TSK_STACK_USER( STK_TSK_ColumnOvenSupportFuncs_GlobalTimer, 1000 );
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern   const    ushort   cu16_TimeoutShort;
extern   const    ushort   cu16_TimeoutLong;
//------------------------------------------------ functions -----------------------------------------------------------------------------------
ulong ColumnOvenSupportFuncs_InitTasks (void)
{
   ulong    u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // init tasks
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   TID_ColumnOvenSupportFuncs_GasSensing  = os_tsk_create_user (TSK_ColumnOvenSupportTasks_GasSensing, (uchar)OS_P_NORMAL,
   (void *)STK_TSK_ColumnOvenSupportFuncs_GasSensing,
   (ushort)(sizeof(STK_TSK_ColumnOvenSupportFuncs_GasSensing)));
#endif
   TID_ColumnOvenSupportFuncs_GlobalTimer = os_tsk_create_user (TSK_ColumnOvenSupportTasks_GlobalTimer, (uchar)OS_P_ABOVENORMAL,
   (void *)STK_TSK_ColumnOvenSupportFuncs_GlobalTimer,
   (ushort)(sizeof(STK_TSK_ColumnOvenSupportFuncs_GlobalTimer)));
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenSupportFuncs_HeatCoolProgramLoad (ushort u16_HeatCoolProgramId, void *pv_HeatCoolProgram)
{
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_HeatCoolProgramStored;
   ulong    u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   if (u16_HeatCoolProgramId <= (ushort)AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID)
   {
      u16_NumBytes      =  sizeof (u16_HeatCoolProgramStored);
      u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_STORED;
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_HeatCoolProgramStored), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
               u32_ErrorCodeSW_Sub))
      {
         if ((u16_HeatCoolProgramStored & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED) == ((ushort)COLUMN_OVEN_CMDS_USER__CODE_HEAT_COOL_PROGRAM_STORED))
         {
            // get the correct bit identifier of desired temperature profile id and compare if passed profile id is stored on fixed memory
            // get the correct bit identifier of desired heat/cool profile identifier and compare if passed identifier is stored on fixed memory
            if ((u16_HeatCoolProgramStored & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED_ID) & (1 << ((u16_HeatCoolProgramId & (ushort)COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED_ID) - 1)))
            {
               u16_NumBytes      =  sizeof (tS_ColumnOvenRegisters_HeatCoolProgram);
               u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_IN_USE + (u16_NumBytes * (u16_HeatCoolProgramId - 1));
               if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  (pv_HeatCoolProgram, u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
                        u32_ErrorCodeSW_Sub))
               {
               }
               else
               u32_ErrorCodeSW_Sub = ((ulong)u32_ErrorCodeSW_Sub << 16) | (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__HEAT_COOL_PROGRAM_LOAD;
            }
            else
            u32_ErrorCodeSW_Sub = ((ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_PROGRAM_ID << 16) | (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__HEAT_COOL_PROGRAM_LOAD; // profile identifier not stored
         }
         else
         u32_ErrorCodeSW_Sub = ((ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_PROGRAM_ID << 16) | (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__HEAT_COOL_PROGRAM_LOAD; // general save code is not stored
      }
      else
      u32_ErrorCodeSW_Sub = ((ulong)u32_ErrorCodeSW_Sub << 16) | (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__HEAT_COOL_PROGRAM_LOAD;
   }
   else
   u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__HEAT_COOL_PROGRAM_LOAD;
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenSupportFuncs_DeviceParamsInternCheck (void *pv_DeviceParamsIntern, ushort *pu16_ControlSectionsNum, bool b_FlagValidationType)
{
   bool     b_ControlParamsZeroEntries;
   ushort   u16_Idx1, u16_Idx2;
   ushort   u16_DeltaTKi, u16_DeltaTKp;
   ushort   u16_DeltaTKiSum, u16_DeltaTKpSum;
   ushort   u16_NumBytes;
   ushort   u16_DestAddress;
   ushort   u16_ControlParamsSectionCount;
   ushort   u16_DeviceParamsInternStored;
   ushort   u16_SykamDeviceOptions;
   ulong    u32_ErrorCodeSW_Sub;
   struct   _S_ColumnOvenRegisters_DeviceParamsIntern *pS_DeviceParamsIntern;
   u32_ErrorCodeSW_Sub        =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   b_ControlParamsZeroEntries =  FALSE;
   // null pointer check
   if (pv_DeviceParamsIntern)
   pS_DeviceParamsIntern  =  (tS_ColumnOvenRegisters_DeviceParamsIntern *)pv_DeviceParamsIntern;
   else
   return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__DEVICE_PARAMS_INTERN_CHECK;
   // read out sykam device data option field
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START,
               (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_SykamDeviceOptions), NULL, cu16_TimeoutShort),
            u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // read flag if control section params were stored on fixed memory
      u16_NumBytes      =  sizeof (u16_DeviceParamsInternStored);
      u16_DestAddress   =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__OCMD_CONTROL_SECTIONS_PARAMS_SAVE;
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_DeviceParamsInternStored), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__READ),
               u32_ErrorCodeSW_Sub))
      {
         // check if control section params "stored flag" is correct
         if (     (b_FlagValidationType == (bool)COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_WRITE)
               || ((u16_DeviceParamsInternStored == (ushort)COLUMN_OVEN_SUPPORT_FUNCS__CODE_DEVICE_PARAMS_INTERN_STORED)   && (b_FlagValidationType == (bool)COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_READ))
               || (b_FlagValidationType == (bool)COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_INIT)
               )
         {
            // check user selected and modified temperature profile for zero entries
            // check user selected and modified temperature profile for zero entries
            for (u16_Idx1 = 1, u16_ControlParamsSectionCount = 0; u16_Idx1 < (ushort)AT91SAM7X__MAX_CONTROL_PARAM_SECTIONS; u16_Idx1++)
            {
               if (     (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1].u16_Ki  != 0)
                     &&  (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1].u16_TKi != 0)
                     &&  (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1].u16_Kp  != 0)
                     &&  (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1].u16_TKp != 0)
                     )
               {
                  // check for ascending control section params
                  if (     (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1 - 1].u16_TKi < pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1].u16_TKi)
                        && (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1 - 1].u16_TKp < pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1].u16_TKp)
                        )
                  {
                     u16_ControlParamsSectionCount++;
                     b_ControlParamsZeroEntries = FALSE;
                  }
                  else
                  {
                     b_ControlParamsZeroEntries = TRUE;  // use this flag to indicate non-ascending control section params
                     break;
                  }
               }
               else
               {
                  // possibility that one entry of section zero (0) was set to zero - this results in a defined function error after the for-loop
                  if (     (u16_Idx1 == 1)
                        && (     (pS_DeviceParamsIntern->SA_ControlSectionParams[0].u16_Ki  == 0)
                           || (pS_DeviceParamsIntern->SA_ControlSectionParams[0].u16_TKi == 0)
                           || (pS_DeviceParamsIntern->SA_ControlSectionParams[0].u16_Kp  == 0)
                           || (pS_DeviceParamsIntern->SA_ControlSectionParams[0].u16_TKp == 0)
                           )
                        )
                  {
                     b_ControlParamsZeroEntries = TRUE;
                     break;
                  }
                  // check if control section params have zero entries from now on (ok) OR if zero entries are inside control sections (error case)
                  else if ((u16_Idx1 - u16_ControlParamsSectionCount) >= 1)
                  {
                     u16_Idx2 = u16_Idx1 + 1;
                     while (u16_Idx2 < (ushort)AT91SAM7X__MAX_CONTROL_PARAM_SECTIONS)
                     {
                        if (     (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx2].u16_Ki  != 0)
                              && (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx2].u16_TKi != 0)
                              && (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx2].u16_Kp  != 0)
                              && (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx2].u16_TKp != 0)
                              )
                        {
                           // set break condition for function and outer "for loop"
                           b_ControlParamsZeroEntries =  TRUE;
                           u16_Idx2                   =  (ushort)AT91SAM7X__MAX_CONTROL_PARAM_SECTIONS;
                           break;
                        }
                        u16_Idx2++;
                     }
                  }
               }
            }
            // check for preceedings errors and zero entries in control section zero (0)
            if (     (b_ControlParamsZeroEntries)
                  || (pS_DeviceParamsIntern->SA_ControlSectionParams[0].u16_Ki  == 0)
                  || (pS_DeviceParamsIntern->SA_ControlSectionParams[0].u16_TKi == 0)
                  || (pS_DeviceParamsIntern->SA_ControlSectionParams[0].u16_Kp  == 0)
                  || (pS_DeviceParamsIntern->SA_ControlSectionParams[0].u16_TKp == 0)
                  )
            {
               // step out of function because user did modify control section params in an invalid way // TODO: check if a user warning would be better
               return u32_ErrorCodeSW_Sub   = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__DEVICE_PARAMS_INTERN_CHECK;
            }
            u16_DeltaTKi      =  0;
            u16_DeltaTKp      =  0;
            u16_DeltaTKiSum   =  0;
            u16_DeltaTKpSum   =  0;
            // calculate temperature differences given in control section params
            u16_Idx1 = u16_ControlParamsSectionCount;
            while (u16_Idx1 >= 1)
            {
               // note: control section params (TKi/TKp) are sorted in ascending order => no overflow will occur
               u16_DeltaTKi = pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1].u16_TKi - pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1 - 1].u16_TKi;
               u16_DeltaTKp = pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1].u16_TKp - pS_DeviceParamsIntern->SA_ControlSectionParams[u16_Idx1 - 1].u16_TKp;
               if ((u16_DeltaTKi > 0) && (u16_DeltaTKp > 0))
               {
                  u16_DeltaTKiSum += u16_DeltaTKi;
                  u16_DeltaTKpSum += u16_DeltaTKp;
               }
               u16_Idx1--;
            }
            // final check:   1) at least two entries must exist with sum of differences between TKp/TKi equal to total device temperature range
            //                2) the lowest and highest control section params must be equal to given values
            if (     (pS_DeviceParamsIntern->SA_ControlSectionParams[0].u16_TKi == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MIN)
                  && (pS_DeviceParamsIntern->SA_ControlSectionParams[0].u16_TKp == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MIN)
                  && (  (     (u16_DeltaTKiSum == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM_FOR_HEAT)
                        && (u16_DeltaTKpSum == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM_FOR_HEAT)
                        && (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_ControlParamsSectionCount].u16_TKi == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MAX_FOR_HEAT)
                        && (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_ControlParamsSectionCount].u16_TKp == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MAX_FOR_HEAT)
                        && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT)
                        && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL))
                        && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
                        )
                     ||
                     (     (u16_DeltaTKiSum == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM_FOR_HEAT_AND_COOL)
                        && (u16_DeltaTKpSum == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM_FOR_HEAT_AND_COOL)
                        && (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_ControlParamsSectionCount].u16_TKi == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MAX_FOR_HEAT_AND_COOL)
                        && (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_ControlParamsSectionCount].u16_TKp == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MAX_FOR_HEAT_AND_COOL)
                        && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                        && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
                        )
                     ||
                     (     (u16_DeltaTKiSum == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM_FOR_AUTOSAMPLER)
                        && (u16_DeltaTKpSum == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM_FOR_AUTOSAMPLER)
                        && (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_ControlParamsSectionCount].u16_TKi == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MAX_FOR_AUTOSAMPLER)
                        && (pS_DeviceParamsIntern->SA_ControlSectionParams[u16_ControlParamsSectionCount].u16_TKp == (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MAX_FOR_AUTOSAMPLER)
                        && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)
                        && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                        )
                     )
                  )
            {
               // finally set number control sections
               *pu16_ControlSectionsNum  =  u16_ControlParamsSectionCount + 1;
            }
            // step out of function because user did modify control section params in an invalid way
            else
            u32_ErrorCodeSW_Sub   = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__DEVICE_PARAMS_INTERN_CHECK;
         }
         // control sections params might be never stored before
         else
         u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__DEVICE_PARAMS_INTERN_CHECK;
      }
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (ulong)(u32_ErrorCodeSW_Sub << 16) | (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__DEVICE_PARAMS_INTERN_CHECK;
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenSupportFuncs_SetDacOutput (float f_DacDesiredOutput)
{
   uchar    u8_Unit                    =  0;
   uchar    u8_Channel                 =  (uchar)AT91SAM7X__DACRRCS085_CHANNEL_POWER_HEAT_COOL;
   uchar    u8_DacMode                 =  (uchar)DACRRCS085_MODE_WRITEANDUPDATE;
   uchar    u8_PowerSlopeMode          =  0;
   static   uchar    u8_TimeCoef;
   uchar    u8_State;
   ushort   u16_DacCurrentOutput       =  0;
   ushort   u16_DacIntermediateOutput  =  0;
   ushort   u16_DacDesiredOutput       =  0;
   ushort   u16_DacOutputMaxStart;
   ushort   u16_WaitTimeTicks          =  10; /*20??*/                                // wait ticks between DAC level change
   ulong    u32_ErrorCodeSW_Sub        =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   float    f_DacMax;
   float    f_DacMin;
   ulong    u32_ColumnOvenTemperature;
   //   static   char     s8_UsartDataTx[128];                         // debug purpose
   struct   _SMBPBRegAccess            S_ModbusAccessParams;
   tS_LibExceptions_Exceptions         S_Exceptions;
   static   ushort                              u16_SykamDeviceOptions;
   //   if (fabsf(f_DacDesiredOutput) == 0.0)
   //   {
   //      // disable power circuit totally if DAC value is smaller than one ("1")    		
   //      AT91F_PIO_SetOutput ((AT91PS_PIO)AT91C_BASE_PIOB, (ulong)AT91SAM7X__PIN__COLUMN_OVEN_POWER);
   //      return u32_ErrorCodeSW_Sub;
   //   }
   //   else
   //   {
   //      AT91F_PIO_ClearOutput ((AT91PS_PIO)AT91C_BASE_PIOB, (ulong)AT91SAM7X__PIN__COLUMN_OVEN_POWER);
   //   }
   S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
   S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
   S_ModbusAccessParams.pvIntern     =  NULL;                                                                         // debug purpose, set to &pv_BufferAddress if needed
   S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;  // !!! check if timeout is suffcient if serial port is stressed!!
   S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
   S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START;
   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceOptions)/2);
   S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceOptions);
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      S_Exceptions.u32_ErrorCodeSW_Sub=S_Exceptions.u32_ErrorCodeSW_Sub;   
   }
   else
   {
      u16_SykamDeviceOptions = 0;
   }
   if ((u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT) != 0)
   {
      u16_DacOutputMaxStart = (ushort)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_MAX_START;
      f_DacMax = (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX;
      f_DacMin = 0.0;
   }
   if ((u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL) != 0)
   {
//      u16_DacOutputMaxStart = (ushort)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_MAX_START_PELTIER;
//      // Input variable f_DacDesiredOutput is signed: negative for cooling and positive for heating.
//      // Read actual temperature. Calculate min and max DAC value. Min DAC value is for cooling, max value is for heating.
//      // below 5°C max DAC value is constant 520. This is voltage for max. cooling power. Min DAC value is constant 10
//      // between 5°C and 15°C max DAC Value is linear interpolated from 520 to 500, min DAC value from 10 to 500
//      // between 15°C and 60°C max DAC value is constant 500, min DAC value is 500
//      // between 60°C and 100°C max DAC Value is linear interpolated from 500 to 750, min DAC valur from 500 to 10
//      // over 100°C max DAC value is constant 750. Max power at 100°C: 120W. Min DAC value is constant 10
//      if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_TEMPERATURE,
//                  (ushort)AT91SAM7X__NUM_REGS_2, (void *)(&u32_ColumnOvenTemperature), NULL, cu16_TimeoutShort),
//               S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
//      {
//         u32_ColumnOvenTemperature = 200000;                // set temperature to 20°C when error reading actual temperature
//      }
//      if (u32_ColumnOvenTemperature <  50000)                                                // temperature <5°C
//      {
//         f_DacMax =   10.0;                                                                  // max. DAC value heat =  10
//         f_DacMin = -520.0;                                                                  // max. DAC value cool = 520
//      }
//      if ((u32_ColumnOvenTemperature >=  50000) && (u32_ColumnOvenTemperature < 150000))     // temperature 5°C ... 15°C
//      {
//         // linear interpolation max. Dac value 520 to 500
//         f_DacMax =  500.0 - ((500.0 -  10.0) * (150000.0 - (float)u32_ColumnOvenTemperature)) / (150000.0 - 50000.0);
//         f_DacMin = -500.0 - ((520.0 - 500.0) * (150000.0 - (float)u32_ColumnOvenTemperature)) / (150000.0 - 50000.0);
//      }
//      if ((u32_ColumnOvenTemperature >= 150000) && (u32_ColumnOvenTemperature < 600000))     // temperature 15°C ... 60°C
//      {
//         f_DacMax =  500.0;                                                                  // max. DAC value heat = 500
//         f_DacMin = -500.0;                                                                  // max. DAC value cool = 500
//      }
//      if ((u32_ColumnOvenTemperature >= 600000) && (u32_ColumnOvenTemperature < 1000000))    // temperature 60°C ... 100°C
//      {
//         // linear interpolation max. DAC value 500 to 750
//         f_DacMax =  500.0 + ((750.0 - 450.0) * ((float)u32_ColumnOvenTemperature - 600000.0)) / (1000000.0 - 600000.0);
//         f_DacMin = -500.0 + ((450.0 -  10.0) * ((float)u32_ColumnOvenTemperature - 600000.0)) / (1000000.0 - 600000.0);
//      }
//      if (u32_ColumnOvenTemperature >= 1000000)                                              // temperature >100°C
//      {
//         f_DacMax = 750.0;                                                                   // max. DAC value heat = 750
//         f_DacMin = -10.0;                                                                   // max. DAC value cool = 10
//      }
//   }
      MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_READ, COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_STATE,1, &u8_State, NULL, cu16_TimeoutShort);
      if(u8_State == COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN)
      {
         if(u8_TimeCoef < 100)                  // 10 second ramp
         {
            u8_TimeCoef+=1;
         }
      }
      else
      {
         u8_TimeCoef = 0;
      }

      u16_DacOutputMaxStart = (ushort)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_MAX_START_PELTIER;
      if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_TEMPERATURE,
                  (ushort)AT91SAM7X__NUM_REGS_2, (void *)(&u32_ColumnOvenTemperature), NULL, cu16_TimeoutShort),
               S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         u32_ColumnOvenTemperature = 200000;                // set temperature to 20°C when error reading actual temperature
      }
      if (u32_ColumnOvenTemperature <  50000)                                                // temperature <5°C
      {
         f_DacMax =   10.0;                                                                  // max. DAC value heat =  10
         f_DacMin = -750.0 * (float)u8_TimeCoef / 100.0;                                                                  // max. DAC value cool = 520
      }
      if ((u32_ColumnOvenTemperature >=  50000) && (u32_ColumnOvenTemperature < 150000))     // temperature 5°C ... 15°C
      {
         // linear interpolation max. Dac value 520 to 500
         f_DacMax =  400.0;
         f_DacMin = -650.0 * (float)u8_TimeCoef / 100.0;
      }
      if ((u32_ColumnOvenTemperature >= 150000) && (u32_ColumnOvenTemperature < 400000))     // temperature 15°C ... 40°C
      {
         f_DacMax =  500.0 * (float)u8_TimeCoef / 100.0;
         f_DacMin = -650.0 * (float)u8_TimeCoef / 100.0;
      }
      if ((u32_ColumnOvenTemperature >= 400000) && (u32_ColumnOvenTemperature < 600000))     // temperature 40°C ... 60°C
      {
         f_DacMax =  (500.0 + (100.0 * ((float)u32_ColumnOvenTemperature - 400000.0)) / (600000.0 - 400000.0))* (float)u8_TimeCoef / 100.0;
         f_DacMin = - 650.0 * (float)u8_TimeCoef / 100.0;
      }
      if ((u32_ColumnOvenTemperature >= 600000) && (u32_ColumnOvenTemperature < 800000))    // temperature 60°C ... 80°C
      {
         f_DacMax =  (600.0 + (100.0 * ((float)u32_ColumnOvenTemperature - 600000.0)) / (800000.0 - 600000.0)) * (float)u8_TimeCoef / 100.0;
         f_DacMin = - 650.0 * (float)u8_TimeCoef / 100.0;
      }
      if ((u32_ColumnOvenTemperature >= 800000) && (u32_ColumnOvenTemperature < 1000000))    // temperature 80°C ... 100°C
      {
         f_DacMax = (700.0 + (50.0 * ((float)u32_ColumnOvenTemperature - 800000.0)) / (1000000.0 - 800000.0)) * (float)u8_TimeCoef / 100.0;
         f_DacMin = - 650.0 * (float)u8_TimeCoef / 100.0;
      }
      if (u32_ColumnOvenTemperature >= 1000000)                                              // temperature >100°C
      {
         f_DacMax = 750.0 * (float)u8_TimeCoef / 100.0;                                                                   // max. DAC value heat = 750
         f_DacMin = -10.0;                                                                   // max. DAC value cool = 10
      }
   }
   if (f_DacDesiredOutput > f_DacMax)
   f_DacDesiredOutput = f_DacMax;
   if (f_DacDesiredOutput < f_DacMin)
   f_DacDesiredOutput = f_DacMin;
   if (fabsf(f_DacDesiredOutput) > (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX)
   return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__SET_DAC_OUTPUT;
   if ((u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT) != 0)
   {
      // if (f_DacDesiredOutput < 0.0)    // is already done with f_DacMax, f_DacMin above
      //   f_DacDesiredOutput = 0.0;
      // adapt DAC values to power electronics characteristic (slope of heat output voltage)
      if (     (f_DacDesiredOutput > cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaBLow)
            && (f_DacDesiredOutput <= cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaBHigh))
      {
         f_DacDesiredOutput = (f_DacDesiredOutput * cf_ColumnOvenSignalProcessingFuncs_NewSlopeAreaB) - cf_ColumnOvenSignalProcessingFuncs_OffsetAreaB;
      }
      else if (   (f_DacDesiredOutput > cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaCLow)
            && (f_DacDesiredOutput <= cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaCHigh))
      {
         f_DacDesiredOutput = (f_DacDesiredOutput * cf_ColumnOvenSignalProcessingFuncs_NewSlopeAreaC) - cf_ColumnOvenSignalProcessingFuncs_OffsetAreaC;
      }
      else if (f_DacDesiredOutput > cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaCHigh)
      {
         f_DacDesiredOutput =  (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX;
      }
   }
   // set linearized and rounded DAC output to unsigned short value
   u16_DacDesiredOutput = (ushort)rintf (fabsf(f_DacDesiredOutput));
   if (SUCCESS (Dacrrcs085GetOutput(u8_Unit, u8_Channel, &u16_DacCurrentOutput, cu16_TimeoutLong),
            u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__DACRRCS085__GET_OUTPUT))
   {
      if (u16_DacDesiredOutput <= u16_DacCurrentOutput)
      {
         // case 1: set desired DAC value directly - no foldback characteristic (don't change - checked multiple!)
         if (u16_DacCurrentOutput <= u16_DacOutputMaxStart)
         {
            u8_PowerSlopeMode          =  (uchar)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__POWER_MODE_SLOPE_DOWN;
            u16_DacIntermediateOutput  =  u16_DacDesiredOutput;
         }
         // case 2: start decrementation from current DAC value (small steps)
         else
         {
            u8_PowerSlopeMode          =  (uchar)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__POWER_MODE_SLOPE_DOWN;
            u16_DacIntermediateOutput  =  u16_DacCurrentOutput;
         }
      }
      // desired output > current output
      else
      {
         // case 3: set intermediate DAC value to DAC max output start value and increment afterwards in small steps if needed
         if (     (u16_DacCurrentOutput <= u16_DacOutputMaxStart)
               && (u16_DacDesiredOutput >  u16_DacOutputMaxStart))
         {
            u16_DacIntermediateOutput  =  u16_DacOutputMaxStart;
            u8_PowerSlopeMode          =  (uchar)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__POWER_MODE_SLOPE_UP;
         }
         // case 4: set intermediate DAC value directly in one step
         else if (   (u16_DacCurrentOutput <= u16_DacOutputMaxStart)
               && (u16_DacDesiredOutput <= u16_DacOutputMaxStart))
         {
            u8_PowerSlopeMode          =  (uchar)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__POWER_MODE_SLOPE_UP;
            u16_DacIntermediateOutput  =  u16_DacDesiredOutput;
         }
         // case 5: set intermediate DAC value to current and increment afterwards in small steps - suppress foldback characteristic
         else
         {
            u8_PowerSlopeMode          =  (uchar)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__POWER_MODE_SLOPE_UP;
            u16_DacIntermediateOutput  =  u16_DacCurrentOutput;
         }
      }
      // enable power circuit
      AT91F_PIO_ClearOutput ((AT91PS_PIO)AT91C_BASE_PIOB, (ulong)AT91SAM7X__PIN__COLUMN_OVEN_POWER);
      while (u16_DacDesiredOutput != u16_DacCurrentOutput)
      {
         if (SUCCESS (Dacrrcs085SetOutput(u8_Unit, u8_Channel, u8_DacMode, u16_DacIntermediateOutput, cu16_TimeoutLong),
                  u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__DACRRCS085__SET_OUTPUT))
         {
            // additional delay to wait for power part settling
            if (u16_DacIntermediateOutput != u16_DacCurrentOutput)
            os_dly_wait (u16_WaitTimeTicks);
            // set current DAC output for comparison with desired DAC value
            u16_DacCurrentOutput = u16_DacIntermediateOutput;
            switch (u8_PowerSlopeMode)
            {
            case COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__POWER_MODE_SLOPE_UP:
               {
                  // set desired output directly because DAC INcrementation is not higher than defined DAC increment
                  if ((u16_DacCurrentOutput + (ushort)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_INCR) > u16_DacDesiredOutput)
                  {
                     u16_DacIntermediateOutput = u16_DacDesiredOutput;
                  }
                  // increment DAC output with defined incrementation value
                  else
                  {
                     u16_DacIntermediateOutput += (ushort)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_INCR;
                  }
                  break;
               }
               // Do NOT change execution order!
            case COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__POWER_MODE_SLOPE_DOWN:
               {
                  // set DAC value directly because its lower than a given threshold
                  if (     (u16_DacCurrentOutput <= u16_DacOutputMaxStart)
                        && (u16_DacDesiredOutput <= u16_DacOutputMaxStart))
                  {
                     u16_DacIntermediateOutput = u16_DacDesiredOutput;
                  }
                  // decrement DAC output with incrementation value
                  else if (    (u16_DacCurrentOutput >= (ushort)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_INCR)
                        &&  (u16_DacDesiredOutput <= (u16_DacCurrentOutput - (ushort)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_INCR)))
                  {
                     u16_DacIntermediateOutput -= (ushort)COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_INCR;
                  }
                  // DAC value after virtual DEcrementation is not higher than desired output - therefore set it directly
                  else
                  {
                     u16_DacIntermediateOutput = u16_DacDesiredOutput;
                  }
                  break;
               }
            default:
               break;
            }
         }
         else
         {
            break;
            return u32_ErrorCodeSW_Sub;
         }
      }
   }
   //         sprintf ((char *)s8_UsartDataTx, "%8d", u32_ColumnOvenTemperature);
   //         UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
   //         sprintf ((char *)s8_UsartDataTx, " %4d\r", u16_DacOutput);
   //         UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__SET_DAC_OUTPUT;
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenSupportFuncs_CalcHeatCoolProgramTimes (tS_ColumnOvenRegisters_StatusDataExtern *pS_StatusDataExtern, tS_ColumnOvenRegisters_StatusDataIntern *pS_StatusDataIntern, bool *pb_ReturnFlagCalcControlParams, bool *pb_ReturnFlagCurrentSectionIsRamp, ushort *pu16_Counter, tS_ColumnOvenSignalProcessing_TemperatureRamp *psS_TemperatureRamp, ushort u16_SykamDeviceOptions, ushort u16_SykamDeviceClass, ushort u16_SetPointReachedBand)
{
   float f_Tmp                =  0.0;
   ulong u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // first step for column oven state "run" (states and ids have been set before)
   if (     (pS_StatusDataIntern->u32_HeatCoolProgramIntCycleCount == 1)
         && (pS_StatusDataExtern->u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN))
   {
      // INFO: Profile times and control deviations are reset in "start command"
      // INFO: profile times and control deviations are reset in "start command"
      *pb_ReturnFlagCalcControlParams  =  TRUE;
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
      // determine if the first section is a temperature ramp (do this ramp only if a valid hardware option is set)
      if (     (pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint != pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[1].u16_SetPoint)
            && (u16_SykamDeviceClass   & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
            && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
            && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM)
            && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
            && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
            )
      {
         *pb_ReturnFlagCurrentSectionIsRamp  =  TRUE;
         // temperature difference dT in [°C] (macro "ABS" calculates the absolute value) - ever use the user given profile set points because if he wants to have a temperature
         // ramp which is not reachable by column oven, a correct given ramp after a wrong one must have the CORRECT slope - DON'T use
         // the current oven temperature (first step is preheat/precool == hold mode, were a stable start temperatur eis reached)
         f_Tmp = ABS((float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[1].u16_SetPoint, (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint);
         // MODIFIED line
         //            f_Tmp = (float)S_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[1].u16_SetPoint - ((float)S_StatusDataExtern->u32_ColumnOvenTemperature / 10000.0);
         // cooling rate / (dT/dt) => max_slope / temperature_slope => incrementation ticks when temperature is to be incremented for doing a ramp
         // though "floorf" we can have a positive offset "cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate" during heat ramp or a
         // negative offset "cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate" during cool ramp
         // set number of task cycles after which the temperature must be incremented about 0.1 °C and simplify divisions through multiplications
         //            psS_TemperatureRamp->u32_CyclesToRefreshSetPoint =  (ulong)(fabsf (ceilf ((cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate * cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate *
         //                                                                                     (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[1].u16_HoldTime) / (f_Tmp/10.0))));
         ////            f_Tmp =  (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_HoldTime;
         ////            f_Tmp =  ceilf (cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate * cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate * (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_HoldTime);
         ////            f_Tmp =  fabsf (f_Tmp);
         ////            f_Tmp = (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[1].u16_SetPoint - (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
         psS_TemperatureRamp->u32_CyclesToRefreshSetPoint =  (ulong)(fabsf (floorf ((cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate * cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate *
         (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_HoldTime) /
         (f_Tmp/10.0))));
         // MODIFIED lines follow
         //            psS_TemperatureRamp->u32_CyclesToRefreshSetPoint =  (ulong)(fabsf (floorf (((cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate/3.0) * cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate *
         //                                                                                      (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[1].u16_HoldTime) / (f_Tmp/10.0))));
         // up to here#####
         // set cycle counter to start value
         psS_TemperatureRamp->u32_CycleCounter = 0;
         // check if desired temperature ramp is processible
         if (psS_TemperatureRamp->u32_CyclesToRefreshSetPoint >= (ulong)(rintf (cf_ColumnOvenSignalProcessingTasks_TaskCyclesPerSecond)))
         {
            // set initial set point from where to start the ramp
            // 1a) negative ramp slope is equal to cool process
            if ((pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[1].u16_SetPoint - pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint) < 0)
            psS_TemperatureRamp->u16_SetPoint   =  pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint - 1;
            // 1b) positive ramp slope is equal to heat process
            else
            psS_TemperatureRamp->u16_SetPoint   =  pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint + 1;
         }
         // we exceeded heat/cool rate => temperature ramp slope is to high for hardware: set next set point immediately (but it will not be reached in user time)
         else
         {
            psS_TemperatureRamp->u16_SetPoint   =  pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[1].u16_SetPoint;
            *pb_ReturnFlagCurrentSectionIsRamp  =  FALSE;   // disable ramp functionality because the ramp slope is to high
         }
         // calc current control deviation
         f_Tmp = (float)(pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[1].u16_SetPoint - pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint) / 10.0;
      }
      else // No temperature ramp! Valid for autosampler heat/cool with Peltier element or column oven with heating foil only.
#endif
      {
         *pb_ReturnFlagCurrentSectionIsRamp  =  FALSE;
         // debug purpose
         psS_TemperatureRamp->u16_SetPoint   =  pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
         // calc current control deviation -> for heat or cool it exists only one set point temperature -> the first array entry
         f_Tmp = (((float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint / 10.0) - ((float)pS_StatusDataExtern->u32_ColumnOvenTemperature / 10000.0));
      }
      // set temperature profile state "heat", "cool" or "set point reached"
      if (fabsf(f_Tmp) > ((float)(u16_SetPointReachedBand) / 10.0))
      {
         if (f_Tmp < 0.0)
         pS_StatusDataIntern->u16_StateHeatCoolProgram   =  (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__HEATING;
         else
         pS_StatusDataIntern->u16_StateHeatCoolProgram   =  (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__COOLING;
      }
      // the profile is started with a temperature inside set point reached band
      else
      pS_StatusDataIntern->u16_StateHeatCoolProgram  =  (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__SET_POINT_REACHED;
   }
   // first step for column oven state "temperature profile hold" (preheat/precool) - times are updated if column oven is in "temperature profile run" state
   else if (   (pS_StatusDataIntern->u32_HeatCoolProgramIntCycleCount == 1)
         && (pS_StatusDataExtern->u16_ColumnOvenState              == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD)
         )
   {
      // debug purpose
      psS_TemperatureRamp->u16_SetPoint    =  pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
      //
      // INFO: Profile times and control deviations are reset in "start command"
      *pb_ReturnFlagCalcControlParams     =  TRUE;
      *pb_ReturnFlagCurrentSectionIsRamp  =  FALSE;
   }
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   // second and further tasks cycles after power up are intended for column oven "run" state - the "start command" will set the column oven to "run" state
   else if (   (pS_StatusDataIntern->u32_HeatCoolProgramIntCycleCount > 1)
         && (pS_StatusDataExtern->u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN)
         && (u16_SykamDeviceClass   & (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
         && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
         && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM)
         && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
         && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
         )
   {
      // temperature profile is running
      if (pS_StatusDataExtern->u32_HeatCoolProgramTimeRemain > 0)
      {
         pS_StatusDataExtern->u32_HeatCoolProgramTimeElapsed++;
         pS_StatusDataExtern->u32_HeatCoolProgramTimeRemain--;
         // profile was in run state and has finished
         if (pS_StatusDataExtern->u32_HeatCoolProgramTimeRemain == 0)
         {
            pS_StatusDataExtern->u16_ColumnOvenState                    =  (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__FINISHED;
            *pb_ReturnFlagCalcControlParams                             =  FALSE;
            *pb_ReturnFlagCurrentSectionIsRamp                          =  FALSE;
            pS_StatusDataExtern->u32_HeatCoolProgramTimeSectionRemain   =  0;
         }
         else
         {
            // check if a profile section is running
            if (pS_StatusDataExtern->u32_HeatCoolProgramTimeSectionRemain > 0)
            {
               pS_StatusDataExtern->u32_HeatCoolProgramTimeSectionRemain--;
               // a profile section has finished
               if (pS_StatusDataExtern->u32_HeatCoolProgramTimeSectionRemain == 0)
               {
                  if ((pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId) > pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionsDefined)
                  {
                     return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__CALC_HEAT_COOL_PROGRAM_TIMES;
                  }
                  pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId++;
                  // set times of section three (counting from zero) because temperature of section two must be reached at time of section three (see docu)
                  pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionSetPoint =  pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_SetPoint;
                  pS_StatusDataExtern->u32_HeatCoolProgramTimeSectionTotal       =  pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_HoldTime;
                  pS_StatusDataExtern->u32_HeatCoolProgramTimeSectionRemain      =  pS_StatusDataExtern->u32_HeatCoolProgramTimeSectionTotal;
                  pS_StatusDataIntern->u32_ControlDeviationAccumulated           =  0x00000000;
                  pS_StatusDataIntern->u32_ControlDeviationLast                  =  0x00000000;
                  pS_StatusDataExtern->u16_ColumnOvenState                       =  (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN;
                  *pb_ReturnFlagCalcControlParams                                =  TRUE;
                  // check for temperature ramp
                  if (     pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId - 1].u16_SetPoint
                        != pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_SetPoint
                        )
                  {
                     // dT in [°C] (macro "ABS" calculates the absolute value)
                     f_Tmp = ABS((float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_SetPoint, (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId - 1].u16_SetPoint);
                     // MODIFIED line
                     //                        f_Tmp = (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_SetPoint - ((float)pS_StatusDataExtern->u32_ColumnOvenTemperature / 10000.0);
                     // cooling rate / (dT/dt) => max_slope / temperature_slope => incrementation ticks when set point temperature is to be incremented for
                     // doing a ramp; though "floorf" we can have a positive offset "cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate" during heat ramp or a
                     // negative offset "cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate" during cool ramp
                     // set number of task cycles after which the temperature must be incremented about 0.1 °C and simplify divisions through multiplications
                     ////                        f_Tmp =  (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_HoldTime;
                     ////                        f_Tmp =  ceilf (cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate * cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate * (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_HoldTime);
                     ////                        f_Tmp =  fabsf (f_Tmp);
                     ////                        f_Tmp = (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[1].u16_SetPoint - (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
                     psS_TemperatureRamp->u32_CyclesToRefreshSetPoint   =  (ulong)(fabsf (floorf ((cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate * cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate *
                     (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_HoldTime) /
                     (f_Tmp/10.0))));
                     // MODIFIED lines follow
                     //                        psS_TemperatureRamp->u32_CyclesToRefreshSetPoint  =  (ulong)(fabsf (ceilf (((cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate/3.0) * cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate *
                     //                                                                                                   (float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_HoldTime) /
                     //                                                                                                    (f_Tmp/10.0))));
                     // up to here
                     // reset cycle counter to start value
                     psS_TemperatureRamp->u32_CycleCounter = 0;
                     // check if desired temprature ramp is processible
                     if (psS_TemperatureRamp->u32_CyclesToRefreshSetPoint >= (ulong)(rintf (cf_ColumnOvenSignalProcessingTasks_TaskCyclesPerSecond)))
                     {
                        // set initial set point from where to start the ramp
                        // 1a) negative ramp slope is equal to cool process
                        if ((pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_SetPoint - pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId - 1].u16_SetPoint) < 0)
                        psS_TemperatureRamp->u16_SetPoint   =  pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId - 1].u16_SetPoint - 1;
                        // 1b) positive ramp slope is equal to heat process
                        else
                        psS_TemperatureRamp->u16_SetPoint =  1 + pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId - 1].u16_SetPoint;
                        *pb_ReturnFlagCurrentSectionIsRamp =  TRUE;
                     }
                     // we exceeded heat/cool rate => temperature ramp slope is to high for hardware: set next set point immediately (but it will not be reached in user time)
                     else
                     {
                        psS_TemperatureRamp->u16_SetPoint    =  pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_SetPoint;
                        *pb_ReturnFlagCurrentSectionIsRamp  =  FALSE;
                     }
                  }
                  else
                  {
                     // debug purpose
                     psS_TemperatureRamp->u16_SetPoint    =  pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId/* + 1*/].u16_SetPoint;
                     //
                     // section finished - next section is no ramp, however control params will be calculated to react more precisely to disturbances
                     *pb_ReturnFlagCurrentSectionIsRamp  =  FALSE;
                  }
                  // calc current control deviation and reset counter (new section was started) which is used to set temperature profile state "set point reached"
                  if (fabsf(((float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_SetPoint / 10.0) -
                           ((float)pS_StatusDataExtern->u32_ColumnOvenTemperature / 10000.0)) > ((float)(u16_SetPointReachedBand) / 10.0))
                  {
                     *pu16_Counter = 0;
                  }
               }
               // a profile section is continuing (do nothing than control/hold current temperature)
               else
               {
               }
               // calc current control deviation
               f_Tmp = (((float)pS_StatusDataExtern->S_HeatCoolProgramInUse.SA_Section[pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionId].u16_SetPoint / 10.0) - ((float)pS_StatusDataExtern->u32_ColumnOvenTemperature / 10000.0));
               // do periodically -> set temperature profile state "heat/cool"
               if (fabsf(f_Tmp) > ((float)(u16_SetPointReachedBand) / 10.0))
               {
                  if (f_Tmp < 0.0)
                  pS_StatusDataIntern->u16_StateHeatCoolProgram   =  (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__COOLING;
                  else
                  pS_StatusDataIntern->u16_StateHeatCoolProgram   =  (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__HEATING;
               }
            }
            else
            u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__CALC_HEAT_COOL_PROGRAM_TIMES;   // section time must be greater than zero because total profile time is greater than zero
         }
      }
      else
      u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__CALC_HEAT_COOL_PROGRAM_TIMES;
   }
#endif
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenSupportFuncs_StoreGasSensorDefaultValues (void)
{
   ushort   u16_NumBytes                        =  (ushort)AT91SAM7X__NUM_BYTES_2;
   ushort   u16_DestAddress;
   ushort   u16_GasSensorAlarmThresholdPPM      =  (ushort)COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_ALARM_THRESHOLD_PPM;
   ushort   u16_GasSensorCalibrationResistance  =  (ushort)COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_CALIBRATION_RESISTANCE;
   ushort   u16_GasSensorAlarmEnabled           =  (ushort)COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_ALARM_ENABLED;
   ulong    u32_ErrorCodeSW_Sub                 =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   u16_DestAddress = (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_THRESHOLD_PPM;
   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_GasSensorAlarmThresholdPPM), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
            u32_ErrorCodeSW_Sub))
   {
      u16_DestAddress = (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_CALIBRATION_RESISTANCE;
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_GasSensorCalibrationResistance), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
               u32_ErrorCodeSW_Sub))
      {
         u16_DestAddress            =  (ushort)COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_ENABLED;
         // set special store code
         u16_GasSensorAlarmEnabled  |=  (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_CODE_PARAMS_STORED;
         TRY_AND_CATCH_APP_FUNC
         (
         UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&u16_GasSensorAlarmEnabled), u16_NumBytes, u16_DestAddress, (bool)UTILS_FUNCS__WRITE),
         u32_ErrorCodeSW_Sub
         );
      }
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__STORE_GAS_SENSOR_DEFAULT_VALUES;
   return u32_ErrorCodeSW_Sub;
}
