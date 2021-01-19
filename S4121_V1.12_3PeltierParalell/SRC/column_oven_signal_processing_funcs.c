/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
**  \file                column_oven_temperature_control_funcs.c
**  \brief               The file contains  \n
**
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
**  \date                2011-12-08     Initial version.
**
*  \bug                 No bugs known at moment. \n
**  \test
**  \todo
**
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <math.h>
//#include <stdio.h>                              // temp for debugging
#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
//#include "..\..\..\cpu\at91\usart\usart.h"      // temp for debugging
//#include "..\..\..\cpu\at91\usart\usartext.h"   // temp for debugging
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\utils_funcs.h"
#include "column_oven_registers.h"
#include "column_oven_signal_processing_tasks.h"
#include "column_oven_signal_processing_funcs.h"
#include "column_oven_support_funcs.h"
//---------------------------------------------- global definitions ---------------------------------------------------------------------------
OS_TID   TID_ColumnOvenSignalProcessingFuncs_TemperatureControl;
OS_TSK_STACK_USER( STK_TSK_ColumnOvenSignalProcessingFuncs_TemperatureControl, 10000 );
const float cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaBLow    =  800.0;
const float cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaBHigh   =  898.0;
const float cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaCLow    =  898.0;
const float cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaCHigh   =  910.0;
const float cf_ColumnOvenSignalProcessingFuncs_NewSlopeAreaB         =  2.04;
const float cf_ColumnOvenSignalProcessingFuncs_NewSlopeAreaC         =  1.916;
const float cf_ColumnOvenSignalProcessingFuncs_OffsetAreaB           =  832.65;
const float cf_ColumnOvenSignalProcessingFuncs_OffsetAreaC           =  721.17;
/*const float cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaBLow    =  500.0;
const float cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaBHigh   =  598.0;
const float cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaCLow    =  598.0;
const float cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaCHigh   =  610.0;
const float cf_ColumnOvenSignalProcessingFuncs_NewSlopeAreaB         =  2.04;
const float cf_ColumnOvenSignalProcessingFuncs_NewSlopeAreaC         =  1.916;
const float cf_ColumnOvenSignalProcessingFuncs_OffsetAreaB           =  532.65;
const float cf_ColumnOvenSignalProcessingFuncs_OffsetAreaC           =  421.17;*/
const float cf_ColumnOvenSignalProcessingFuncs_IntegrationTimeBase   =  60.0;    // This is the base for integration time, i. e. the smallest step
// to increment is 0.1 °C and therefore resulting integration time 61 s.
const float cf_ColumnOvenSignalProcessingFuncs_HeatingFoilHeatRate   =  0.094;   // this is dependent on current heating foil
const float cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate   =  0.100;/*0.094*/;   // this is dependent on current peltier element
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern   const    float    cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate;
extern   const    float    cf_ColumnOvenSignalProcessingTasks_TaskCyclesPerSecond;
extern   const    float    cf_AT91SAM7X_TemperatureSampleTimeInSysTicks;
extern   const    ushort   cu16_TimeoutShort;
//------------------------------------------------ functions -----------------------------------------------------------------------------------
ulong ColumnOvenSignalProcessingFuncs_InitTasks (void)
{
   ulong    u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   TID_ColumnOvenSignalProcessingFuncs_TemperatureControl = os_tsk_create_user (TSK_ColumnOvenSignalProcessingTasks_TemperatureControl,
   (uchar)OS_P_NORMAL,
   (void *)STK_TSK_ColumnOvenSignalProcessingFuncs_TemperatureControl,
   (ushort)(sizeof(STK_TSK_ColumnOvenSignalProcessingFuncs_TemperatureControl)));
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenSignalProcessingFuncs_GetTemperature (ushort u16_AdcValue, float* pf_TemperatureInDegree)
{
   ulong u32_Tmp1, u32_Tmp2, u32_Tmp3;
   ulong u32_ResistancePTsensor;
   ulong u32_Temperature;
   ulong u32_ErrorCodeSW_Sub  = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // u16_AdcValue is converted to fixed point format! This enables faster temperature calculation than with floating
   // point operations.
   // PTsensor resistance ==> 10 kOhm * ((1 + u16_AdcValue) / (11 - u16_AdcValue))
   // Output fixed point format in: 0.0...999.999 = 0x00000000...0xFFFFFFFF => integer value in the upper word and
   // decimal values in the lower word.
   u32_Tmp1                   =  10 * (0x00010000 + (ulong)u16_AdcValue);  // dividend of upper equation is converted to
   // fixed point (voltage value must be lower than 1 V)
   u32_Tmp2                   =  0x000B0000 - (ulong)u16_AdcValue;         // divisor of upper equation
   u32_ResistancePTsensor     =  u32_Tmp1 / u32_Tmp2;
   u32_ResistancePTsensor     =  u32_ResistancePTsensor  << 8;
   u32_Tmp3                   =  (u32_Tmp1 % u32_Tmp2) << 8;
   u32_ResistancePTsensor    +=  u32_Tmp3 / u32_Tmp2;
   u32_ResistancePTsensor     =  u32_ResistancePTsensor  << 8;
   u32_Tmp3                   =  (u32_Tmp3 % u32_Tmp2) << 8;
   u32_ResistancePTsensor    +=  u32_Tmp3 / u32_Tmp2;
   if (u32_ResistancePTsensor < 0x00010000)                                // u32_ResistancePTsensor < 1 kOhm => t < 0°C!
   return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__GET_TEMPERATURE;
   // u32_ResistancePTsensor = (u32_ResistancePTsensor-r0)/r0 => now u32_ResistancePTsensor < 0x00010000
   u32_ResistancePTsensor  =  u32_ResistancePTsensor - 0x00010000;
   // for u32_ResistancePTsensor < 1.5 kOhm  (t ~= 130 °C) do more precise calculation
   if (u32_ResistancePTsensor < 0x00018000)
   {
      // calculate polynom with Horner-scheme
      u32_Temperature         =  0x00010000 + ((0x9AD4 * u32_ResistancePTsensor) >> 19);
      u32_Temperature         =  (u32_Temperature * u32_ResistancePTsensor) >> 16;
      u32_Temperature         =  0x00010000 + ((0x9ADC * u32_Temperature) >> 20);
      u32_Temperature         =  (u32_Temperature * u32_ResistancePTsensor) >> 16;
      u32_Temperature         =  (0xFFDD * u32_Temperature) >> 8;
   }
   // for u32_ResistancePTsensor >= 1.5 kOhm do less precise calculation to avoid overflow
   else
   {
      // calculate polynom with Horner-scheme
      u32_Temperature         =  0x00010000 + ((0x9AD4 * u32_ResistancePTsensor) >> 19);
      u32_Temperature         =  u32_Temperature >> 1;                                    // divisison by 2 to prevent overflow
      u32_Temperature         =  (u32_Temperature * u32_ResistancePTsensor) >> 16;
      u32_Temperature         =  0x00010000 + ((0x9ADC * u32_Temperature) >> 19);
      u32_Temperature         =  u32_Temperature >> 1;                                    // divisison by 2 to prevent overflow
      u32_Temperature         =  (u32_Temperature * u32_ResistancePTsensor) >> 16;
      u32_Temperature         =  (0xFFDD * u32_Temperature) >> 7;
   }
   // convert measured temperature from fixed point to float format
   u32_Tmp1                   =  ((((u32_Temperature & 0x0000FFFF) * 10000) & 0xFFFF0000) >> 16);  // get first four digits of fixed point temperature after decimal point
   u32_Tmp2                   =  ((u32_Temperature & 0xFFFF0000) >> 16);                           // get three digits of fixed point temperature before decimal point
   // set result
   *pf_TemperatureInDegree    = (float)u32_Tmp2 + ((float)u32_Tmp1/10000.0);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenSignalProcessingFuncs_CalcControlParams (tS_ColumnOvenRegisters_StatusDataExtern *pS_StatusDataExtern, tS_ColumnOvenRegisters_StatusDataIntern *pS_StatusDataIntern, bool *pb_FlagCalcControlParams, bool *pb_SignalSmoothing, tS_ColumnOvenSignalProcessing_TemperatureRamp *psS_TemperatureRamp, bool b_CurrentSectionIsRamp, ushort u16_SykamDeviceOptions)
{
   uchar    u8_Idx;
   uchar    u8_IdxKi       =  0;
   uchar    u8_IdxKp       =  0;
   struct   _SMBPBRegAccess                              S_ModbusAccessParams;
   struct   _S_ColumnOvenRegisters_DeviceParamsIntern    S_DeviceParamsIntern;
   float    f_dTKi         =  0.0;     // difference of two margin temperatures in which (or at the boarders) the absolute control deviation lies
   float    f_dTKp         =  0.0;     // difference of two margin temperatures in which (or at the boarders) the absolute control deviation lies
   float    f_dKi          =  0.0;     // difference of two integral gain factors which correspond to the margin temperatures
   float    f_dKp          =  0.0;     // difference of two proportional gain factors which correspond to the margin temperatures
   float    f_mKi          =  0.0;     // slope of integral gain
   float    f_mKp          =  0.0;     // slope of proportional gain
   float    f_ControlDeviationCurrent;
   ulong    u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // execute this funtion only if desired by preceeding functions - return without error if not desired
   if (*pb_FlagCalcControlParams)
   {
      // check for loaded or saved control sections params
      //      if (     (S_StatusDataIntern->u16_StateControlSectionParams == (ushort)COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__LOADED)
      //            || (S_StatusDataIntern->u16_StateControlSectionParams == (ushort)COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__SAVED))
      {
         // load internal device params from register bank
         S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
         S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
         S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START;  // register start address
         S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_DeviceParamsIntern)/2);                                      // total words of S_DeviceParamsIntern
         S_ModbusAccessParams.pvData       =  (void *) (&S_DeviceParamsIntern);
         S_ModbusAccessParams.pvIntern     =  NULL;                                                                           // debug purpose, set to &pv_BufferAddress if needed
         S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            if (!b_CurrentSectionIsRamp)  // control param calculation for ONE temperature set point WITHOUT temperature ramp
            {
               // debug
               *pb_SignalSmoothing = TRUE;
               *pb_FlagCalcControlParams  =  FALSE;
               // determine the control deviation (temperature difference between current measured temperature and set point temperature)
               f_ControlDeviationCurrent  =  ((float)pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionSetPoint / 10.0) -
               ((float)pS_StatusDataExtern->u32_ColumnOvenTemperature / 10000.0);
               f_ControlDeviationCurrent  =  fabsf (f_ControlDeviationCurrent);
            }
            else  // control param calculation for a temperature RAMP   start c0->c1, c2, c3, c4, c5 -> increment T
            {
               *pb_FlagCalcControlParams  =  TRUE;
               // debug
               *pb_SignalSmoothing = FALSE;
               if (psS_TemperatureRamp->u32_CycleCounter < psS_TemperatureRamp->u32_CyclesToRefreshSetPoint)
               {
                  // wait "u32_CyclesToRefreshSetPoint" to increment ramp temperature
                  psS_TemperatureRamp->u32_CycleCounter++;
               }
               else
               {
                  // do nothing because this is the final section of ramp (next cycle can be a new section or temperature profile finishes)
                  if (psS_TemperatureRamp->u16_SetPoint == pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionSetPoint)
                  {
                     *pb_FlagCalcControlParams  =  FALSE;
                     // debug
                     *pb_SignalSmoothing = TRUE;
                  }
                  // increment current ramp set point about 0.1 °C if the positive ramp has not reached the final section temperature
                  if (pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionSetPoint > psS_TemperatureRamp->u16_SetPoint)
                  {
                     psS_TemperatureRamp->u16_SetPoint++;
                  }
                  // DEcrement current negative ramp set point about 0.1 °C
                  else if (pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionSetPoint < psS_TemperatureRamp->u16_SetPoint)
                  {
                     psS_TemperatureRamp->u16_SetPoint--;
                  }
//                  // do nothing because this is the final section of ramp (next cycle can be a new section or temperature profile finishes)
//                  if (sS_TemperatureRamp->u16_SetPoint == S_StatusDataExtern->u16_HeatCoolProgramCurrentSectionSetPoint)
//                  {
//                     *pb_FlagCalcControlParams  =  FALSE;
//                     // debug
//                     *pb_SignalSmoothing = TRUE;
//                  }
                  // determine the control deviation (temperature difference between set point temperature and current measured temperature)
                  f_ControlDeviationCurrent  =  ((float)psS_TemperatureRamp->u16_SetPoint / 10.0) - ((float)pS_StatusDataExtern->u32_ColumnOvenTemperature / 10000.0);
                  f_ControlDeviationCurrent  =  fabsf (f_ControlDeviationCurrent);
                  // reset cycle counter (wait cycles are over => increment temperature and reset counter)
                  psS_TemperatureRamp->u32_CycleCounter = 1;
               }
            }
            // Search the control params (the index) which fit to temperature difference between current and next step. Control params are guaranteed
            // to be in ascending order by save function
            for (u8_Idx = 0; u8_Idx < (uchar)pS_StatusDataIntern->u16_ControlSectionsNum; u8_Idx++)
            {
               if (f_ControlDeviationCurrent >= ((float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx].u16_TKi / 10.0))
               {
                  if (     ((u8_Idx + 1)              <  (uchar)pS_StatusDataIntern->u16_ControlSectionsNum)
                        && (f_ControlDeviationCurrent <  ((float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx + 1].u16_TKi / 10.0))
                        )
                  {
                     u8_IdxKi = u8_Idx;
                     if (S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx].u16_TKi > S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx + 1].u16_TKi)
                     {
                        u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__CALC_CONTROL_PARAMS;   // this case is not allowed
                     }
                     else
                     {
                        f_dTKi = ((float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx + 1].u16_TKi - (float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx].u16_TKi) / 10.0;
                     }
                  }
                  else if ((u8_Idx + 1) == (uchar)pS_StatusDataIntern->u16_ControlSectionsNum)
                  {
                     u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__CALC_CONTROL_PARAMS;   // this case is not allowed and shall be notified with an error message by using "control param" save function
                     break;
                  }
               }
               // typically TKp is grouped in the same manner than TKi but this is not guaranteed if control params were modified after device delivery, therefore check separately
               if (f_ControlDeviationCurrent >= ((float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx].u16_TKp / 10.0))
               {
                  if (     ((u8_Idx + 1)              <  (uchar)pS_StatusDataIntern->u16_ControlSectionsNum)
                        && (f_ControlDeviationCurrent <  ((float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx + 1].u16_TKp / 10.0))
                        )
                  {
                     u8_IdxKp = u8_Idx;
                     if (S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx].u16_TKp > S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx + 1].u16_TKp)
                     {
                        u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__CALC_CONTROL_PARAMS;   // this case is not allowed
                     }
                     else
                     {
                        f_dTKp = ((float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx + 1].u16_TKp - (float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_Idx].u16_TKp) / 10.0;
                     }
                  }
                  else if ((u8_Idx + 1) == (uchar)pS_StatusDataIntern->u16_ControlSectionsNum)
                  {
                     u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__CALC_CONTROL_PARAMS;   // this case is not allowed and shall be notified with an error message by using "control param" save function
                     break;
                  }
               }
            }
            if (u32_ErrorCodeSW_Sub == (ulong)LIB_EXCEPTIONS__ERROR_NONE)
            {
               // get the difference of integral gain factors from two adjacent sections (exceeding indicees are blocked by preceeding "if's")
               f_dKi = (float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_IdxKi + 1].u16_Ki - (float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_IdxKi].u16_Ki;
               // get the difference of proportional gain factors from two adjacent sections
               f_dKp = (float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_IdxKp + 1].u16_Kp - (float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_IdxKp].u16_Kp;
               if ((f_dTKi > 0.0) && (f_dTKp > 0.0))
               {
                  // get the slope for Ki
                  f_mKi =  f_dKi / f_dTKi;
                  // get the slope for Kp
                  f_mKp =  f_dKp / f_dTKp;
                  // set Ki and Kp
                  pS_StatusDataIntern->u32_ControlIntegrationGain  =  (ulong)(rintf (((float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_IdxKi].u16_Ki + (f_mKi * (f_ControlDeviationCurrent - ((float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_IdxKi].u16_TKi / 10.0)))) * 10000.0));
                  pS_StatusDataIntern->u32_ControlProportionalGain =  (ulong)(rintf (((float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_IdxKp].u16_Kp + (f_mKp * (f_ControlDeviationCurrent - ((float)S_DeviceParamsIntern.SA_ControlSectionParams[u8_IdxKp].u16_TKp / 10.0)))) * 10000.0));
               }
               // for deviations smaller than 0.1°C (temperature already reached at begin of profile), use params for smallest deviations
               // absolute deviation was calculated above
               else if (   (f_ControlDeviationCurrent < ((float)S_DeviceParamsIntern.SA_ControlSectionParams[0].u16_TKi / 10.0))
                     && (f_ControlDeviationCurrent < ((float)S_DeviceParamsIntern.SA_ControlSectionParams[0].u16_TKp / 10.0)))
               {
                  f_mKi = 0.0;
                  f_mKp = 0.0;
                  // set Ki and Kp
                  pS_StatusDataIntern->u32_ControlIntegrationGain  =  (ulong)(rintf (((float)S_DeviceParamsIntern.SA_ControlSectionParams[0].u16_Ki) * 10000.0));
                  pS_StatusDataIntern->u32_ControlProportionalGain =  (ulong)(rintf (((float)S_DeviceParamsIntern.SA_ControlSectionParams[0].u16_Kp) * 10000.0));
               }
               else
               u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__CALC_CONTROL_PARAMS;   // this case is not allowed
               // set current control params to internal status data - Kd and Td remain constant for current control algorithm
               if (u32_ErrorCodeSW_Sub == (ulong)LIB_EXCEPTIONS__ERROR_NONE)
               {
                  // set Ti dependend on device option (Sykam device data)
                  if (     (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                        && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
                        && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
                        && (cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate > (float)0.0)
                        )
                  {
                     pS_StatusDataIntern->u32_ControlIntegrationTime =  (ulong)rintf (cf_ColumnOvenSignalProcessingFuncs_IntegrationTimeBase +
                     (f_ControlDeviationCurrent / cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate));
                  }
                  else if (   (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL))
                        && (     (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)
                           || (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))
                        && (cf_ColumnOvenSignalProcessingFuncs_HeatingFoilHeatRate > (float)0.0)
                        )
                  {
                     pS_StatusDataIntern->u32_ControlIntegrationTime =  (ulong)rintf (cf_ColumnOvenSignalProcessingFuncs_IntegrationTimeBase +
                     (f_ControlDeviationCurrent / cf_ColumnOvenSignalProcessingFuncs_HeatingFoilHeatRate));
                  }
                  else
                  u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__CALC_CONTROL_PARAMS;
               }
            }
//            /*// control param calculation WITH temperature ramp for ONE temperature set point
//            else if (b_CurrentSectionIsRamp)
//            {
//               // calculate params only once inside one profile time refresh and use the same params for all
//               // intermediate steps one a temperature ramp
//               //*pb_FlagCalcControlParams = FALSE;
//
//               sS_HeatCoolProgramSectionForRamp->u16_SetPoint  =  0;
//               // don't modify control params in register bank as long as ramp is active
//            }*/
         }
         else
         u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__CALC_CONTROL_PARAMS;
      }
   }
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ColumnOvenSignalProcessingFuncs_GetControlOutput (tS_ColumnOvenRegisters_StatusDataExtern *pS_StatusDataExtern, tS_ColumnOvenRegisters_StatusDataIntern *pS_StatusDataIntern,
float *pf_ControlOutput, ushort u16_ControlType, tS_ColumnOvenSignalProcessing_TemperatureRamp *psS_TemperatureRamp,
bool b_FlagCalcControlParams, bool b_FlagCurrentSectionIsRamp)
{
   static         float    f_IntegralTerm;                     // integrator
   static         float    f_DifferentialTerm;                 // differentiator
   static         float    f_ProportionalTerm;                 // proportional part
   float    f_ProportionalMult;                 // for peltier use, increase proportional term when deviation >0.5°
   float    f_IntegralFactor;                   // for peltier use, = Ki * sample_time, Ki is fixed
   float    f_IntegralTermMax;                  // for peltier use, max deviation of integrator from middle value
   float    f_IntegralTermMiddle;               // for peltier use, middle value for integrator
   float    f_ControlSampleTime = cf_AT91SAM7X_TemperatureSampleTimeInSysTicks / 1000.0;  // convert to [s]
   float    f_TemperatureCurrent;               // actual temperature
   float    f_ControlDeviationCurrent;          // actual deviation = set point - actual temperature
   float    f_ControlOutput;                    // output value for DAC before copied to *pf_ControlOutput
   static   float    sf_ControlDeviationAccumulated   =  0;                // for integrator
   static   float    sf_ControlDeviationLast          =  0;                // for differentiator heat only
   static   float    saf_ControlDeviationLast[10]     =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // for differentiator peltier element
   ulong    u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
//   long     i32_dT;                             // for Debug output only!
//   long     i32_DAC;                            // for Debug output only!
//   long     i32_Pterm;                          // for Debug output only!
//   long     i32_Iterm;                          // for Debug output only!
//   long     i32_Dterm;                          // for Debug output only!
//   static   char     s8_UsartDataTx[128];                // for Debug output only!
//   uchar    u8_Unit    =  0;                    // for Debug output only!
//   ulong    u32_InputPorts;                     // for Debug output only!
//   uchar    u8_Channel = (uchar)AT91SAM7X__DACRRCS085_CHANNEL_POWER_HEAT_COOL;   // for Debug output only!
   struct   _SMBPBRegAccess            S_ModbusAccessParams;
   static   ushort                              u16_SykamDeviceOptions;
   S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
   S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
   S_ModbusAccessParams.pvIntern     =  NULL;                                                                         // debug purpose, set to &pv_BufferAddress if needed
   S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;  // !!! check if timeout is suffcient if serial port is stressed!!
   S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
   S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START;
   S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceOptions)/2);
   S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceOptions);
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
   }
   else
   {
      u16_SykamDeviceOptions = 0;
   }
   // calc the control deviation dependend on customer bought hardware option (if no temperature ramp was detected
   // use the given profile set point for deviation calculation
   if ((b_FlagCalcControlParams) && (b_FlagCurrentSectionIsRamp))
   {
      f_ControlDeviationCurrent  =  ((float)psS_TemperatureRamp->u16_SetPoint / 10.0) - ((float)pS_StatusDataExtern->u32_ColumnOvenTemperature / 10000.0);
   }
   else
   {
      // ControlDeviationCurrent =  f_TemperatureSetPoint - f_TemperatureMeasured
      f_ControlDeviationCurrent  = ((float)pS_StatusDataExtern->u16_HeatCoolProgramCurrentSectionSetPoint / 10.0) -
      ((float)pS_StatusDataExtern->u32_ColumnOvenTemperature / 10000.0);
   }
   // check if local static variables must be reset ("local static" because this variables must be signed!)
   if (pS_StatusDataIntern->u32_ControlDeviationAccumulated == 0)
   {
      sf_ControlDeviationAccumulated   =  0.0;
      sf_ControlDeviationLast          =  0.0;
   }
   // calculate current temperature
   f_TemperatureCurrent = (float)pS_StatusDataExtern->u32_ColumnOvenTemperature / 10000.0;
   // integrator:
   // trapezium approach (digitalization) for accumulated control deviation =>
   // ControlDeviationAccumulated += ((ControlDeviationCurrent + ControlDeviationLast) / 2.0)
   // is more precise than rectangular approach type I with => ControlDeviationAccumulated += f_ControlDeviationCurrent
   pS_StatusDataIntern->u32_ControlDeviationAccumulated   +=  (ulong)(rintf (10000.0 * ((fabsf (f_ControlDeviationCurrent) + fabsf (sf_ControlDeviationLast)) / 2.0)));
   // update local value
   sf_ControlDeviationAccumulated += ((f_ControlDeviationCurrent + sf_ControlDeviationLast) / 2.0);
   // prevent division by zero
   if ((f_ControlSampleTime != 0.0) && (pS_StatusDataIntern->u32_ControlIntegrationTime != 0))
   {
      if ((u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL) != 0)
      {
         // integrator:
         f_IntegralFactor = 1.0 * f_ControlSampleTime;                  // factor for f_IntegralTerm, needed to limit also sf_ControlDeviationAccumulated
         f_IntegralTerm = f_IntegralFactor * sf_ControlDeviationAccumulated;
         f_IntegralTermMax = fabs(f_ControlDeviationCurrent) / 1.0;     // integration begins less than +/-1.0°C away from set point
         if (f_IntegralTermMax > 1.0)
         f_IntegralTermMax = 1.0;
         f_IntegralTermMax = 1.0 - f_IntegralTermMax;
         //f_IntegralTermMax *= (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX;
         f_IntegralTermMax *= (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_PELTIER;
         // calculate middle value of integrator dependent on actual temperature, it is estimated DAC value needed to hold this temperature
         // so integrator is set to estimated power and regulation can be done faster
         if (f_TemperatureCurrent > 25.0)                               // for temperature >25°C a linear estimation function is used
         f_IntegralTermMiddle = 7.8 * (f_TemperatureCurrent - 25.0);
         else if (f_TemperatureCurrent < 15.0)                          // for temperature <15°C a quadratic estimation function is used
         f_IntegralTermMiddle = -3.0*f_TemperatureCurrent*f_TemperatureCurrent + 90.4*f_TemperatureCurrent - 774.0;
         else                                                           // for 15°C <= T <= 25°C linear interpolation from 15°C to 25°C
         f_IntegralTermMiddle = -93.0 + (f_TemperatureCurrent-15.0)*93.0/10.0;
         // limitation/clipping of integrator
         if (f_IntegralTerm > f_IntegralTermMiddle + f_IntegralTermMax)
         {
            f_IntegralTerm = f_IntegralTermMiddle + f_IntegralTermMax;
            sf_ControlDeviationAccumulated = f_IntegralTerm / f_IntegralFactor;
         }
         else if (f_IntegralTerm < f_IntegralTermMiddle - f_IntegralTermMax)
         {
            f_IntegralTerm = f_IntegralTermMiddle - f_IntegralTermMax;
            sf_ControlDeviationAccumulated = f_IntegralTerm / f_IntegralFactor;
         }
         // differentiator:
         f_DifferentialTerm = 2500.0 * ((f_ControlDeviationCurrent - saf_ControlDeviationLast[9]) / (10.0 * f_ControlSampleTime));
         // set control deviation (register bank and local)
         pS_StatusDataIntern->u32_ControlDeviationLast   =  (ulong)(rintf ((fabsf (f_ControlDeviationCurrent)) * 10000.0));
         saf_ControlDeviationLast[9]                     =  saf_ControlDeviationLast[8];
         saf_ControlDeviationLast[8]                     =  saf_ControlDeviationLast[7];
         saf_ControlDeviationLast[7]                     =  saf_ControlDeviationLast[6];
         saf_ControlDeviationLast[6]                     =  saf_ControlDeviationLast[5];
         saf_ControlDeviationLast[5]                     =  saf_ControlDeviationLast[4];
         saf_ControlDeviationLast[4]                     =  saf_ControlDeviationLast[3];
         saf_ControlDeviationLast[3]                     =  saf_ControlDeviationLast[2];
         saf_ControlDeviationLast[2]                     =  saf_ControlDeviationLast[1];
         saf_ControlDeviationLast[1]                     =  saf_ControlDeviationLast[0];
         saf_ControlDeviationLast[0]                     =  f_ControlDeviationCurrent;
         // proportional part:
         // increase proportional part for deviation >0.5°
         f_ProportionalMult = fabs(f_ControlDeviationCurrent) / 0.5;
         if (f_ProportionalMult < 1.0)
         f_ProportionalMult = 1.0;
         f_ProportionalTerm = 200.0 * f_ControlDeviationCurrent * f_ProportionalMult;
         // set control output
         f_ControlOutput = f_ProportionalTerm + f_IntegralTerm + f_DifferentialTerm;
         // heat or cool unlimited with peltier elements exept following conditions:
         if (f_TemperatureCurrent >= 35.0)
         {
            if ((fabs(f_ControlDeviationCurrent) < 5.0) && (f_ControlOutput < 0.0))
            f_ControlOutput = 0.0;                                   // do not cool when abs(deviation) < 5° and current_temp >= 35°C
         }
         if (f_TemperatureCurrent <= 16.0)
         {
            if ((fabs(f_ControlDeviationCurrent) < 5.0) && (f_ControlOutput > 0.0))
            f_ControlOutput = 0.0;// do not heat when abs(deviation) < 5° and current_temp <= 16°C
         }
         if (   ((f_TemperatureCurrent) > 16.0) && ((f_TemperatureCurrent) < 35.0))
         {
            if (fabs(f_ControlDeviationCurrent) < 5.0)                  // limit peltier power when abs(deviation) < 5° and 16°C < current_temp < 35°C
            {
               if (f_ControlOutput > 200.0)
               f_ControlOutput = 200.0;
               if (f_ControlOutput < -200.0)
               f_ControlOutput = -200.0;
            }
         }
         // limited controlling range: 0...1023 DAC input value (Vout = 0 V ... 3.3 V) corresponds to Vheat_cool = 1.45 V ... 23.10 V
         if (*pf_ControlOutput > (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_PELTIER)
         *pf_ControlOutput = (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_PELTIER;
         else if (*pf_ControlOutput < (-1.0 * (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_PELTIER))
         *pf_ControlOutput = -1.0 * (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_PELTIER;
         *pf_ControlOutput = f_ControlOutput;                           // set control output of this function
      }
      else if ((u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT) != 0)
      {
         // f_IntegralTerm = (Ki / Ti) * Ts * ControlDeviationAccumulated
         f_IntegralTerm       = (((float)pS_StatusDataIntern->u32_ControlIntegrationGain / 10000.0) / (float)pS_StatusDataIntern->u32_ControlIntegrationTime) *
         f_ControlSampleTime * sf_ControlDeviationAccumulated;
         // limitation/clipping of accumulated error
         if (f_IntegralTerm  > (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL)
         f_IntegralTerm = (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL;
         else if (f_IntegralTerm  < (-1.0 * (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL))
         f_IntegralTerm = -1.0 * (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL;
         // DifferentialTerm = Kd * Td * ((ControlDeviationCurrent - ControlDeviationLast) / Ts)
         f_DifferentialTerm   =  ((float)pS_StatusDataIntern->u32_ControlDifferentialGain / 10000.0) *
         ((float)pS_StatusDataIntern->u32_ControlDifferentialTime) *
         ((f_ControlDeviationCurrent - sf_ControlDeviationLast) / f_ControlSampleTime);
         // differential term can be modified to prevent big differential value at set point jumps (change of profile sections), see Lutz/Wendt p.497:
         // DifferentialTerm        =  Kc * Td * ((TemperatureMeasuredLast - TemperatureMeasured) / Ts);
         // TemperatureMeasuredLast =  TemperatureMeasured;
         // ProportionalTerm = Kp * ControlDeviationCurrent
         f_ProportionalTerm   =  ((float)pS_StatusDataIntern->u32_ControlProportionalGain / 10000.0) * f_ControlDeviationCurrent;
         // set control output
         *pf_ControlOutput    =  f_ProportionalTerm + f_IntegralTerm + f_DifferentialTerm;
         // set control deviation (register bank and local)
         pS_StatusDataIntern->u32_ControlDeviationLast   =  (ulong)(rintf ((fabsf (f_ControlDeviationCurrent)) * 10000.0));
         sf_ControlDeviationLast                         =  f_ControlDeviationCurrent;
         // limited controlling range: 0...1023 DAC input value (Vout = 0 V ... 3.3 V) corresponds to Vheat_cool = 1.45 V ... 23.10 V
         if (*pf_ControlOutput > (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL)
         *pf_ControlOutput = (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL;
         else if (*pf_ControlOutput < (-1.0 * (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL))
         *pf_ControlOutput = -1.0 * (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL;
      }
      else
      {
         *pf_ControlOutput = 0.0;
      }
      // limited controlling range: 0...1023 DAC input value (Vout = 0 V ... 3.3 V) corresponds to Vheat_cool = 1.45 V ... 23.10 V
      if (*pf_ControlOutput > (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL)
      *pf_ControlOutput = (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL;
      else if (*pf_ControlOutput < (-1.0 * (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL))
      *pf_ControlOutput = -1.0 * (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL;
      
   }
   else
   u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__GET_CONTROL_OUTPUT;
   //   u32_InputPorts = AT91F_PIO_GetInput((AT91PS_PIO)AT91C_BASE_PIOB);
   //   u32_InputPorts = u32_InputPorts >> 11;
   //   u32_InputPorts = u32_InputPorts & 1;
   // debug output:
   //   sprintf ((char *)s8_UsartDataTx, "\fT   = %9d\r", pS_StatusDataExtern->u32_ColumnOvenTemperature);
   //   UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
   //   i32_dT = (long)(rintf(f_ControlDeviationCurrent*10000.0));
   //   sprintf ((char *)s8_UsartDataTx,   "dT  = %9d\r", i32_dT);
   //   UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
   //   i32_Pterm = (long)(rintf(f_ProportionalTerm*10000.0));
   //   sprintf ((char *)s8_UsartDataTx,   "P   = %9d\r", i32_Pterm);
   //   UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
   //   i32_Iterm = (long)(rintf(f_IntegralTerm*10000.0));
   //   sprintf ((char *)s8_UsartDataTx,   "I   = %9d\r", i32_Iterm);
   //   UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
   //   i32_Dterm = (long)(rintf(f_DifferentialTerm*10000.0));
   //   sprintf ((char *)s8_UsartDataTx,   "D   = %9d\r", i32_Dterm);
   //   UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
   //   i32_DAC = (long)(rintf(*pf_ControlOutput));
   //   sprintf ((char *)s8_UsartDataTx,   "DAC = %9d\r", i32_DAC);
   //   UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
   //   sprintf ((char *)s8_UsartDataTx,   "Overcurrent = %1d\r", u32_InputPorts);
   //   UsartTx (u8_Unit, u8_Channel, s8_UsartDataTx, strlen((const char *)s8_UsartDataTx), OS_T_MS(100));
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
