/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_support_tasks.c
*
*  \brief               The file contains a task which is responsible for counting the run-time seconds of every Sykam device. Another task is
*                       to observe the mainboard voltages dependend on user configuration in \ref AT91SAM7X.h.
*
*
*  \author              Ruediger Radtke
*  \version             1.4
*
*  \date                2011-08-30     Initial version.
*  \date                2011-11-24     Removed code intern word swaps for 32-bit register bank values (NOW they are shown swaped externally).
*  \date                2012-07-13     <em>File version 1.1</em>. Added task \ref TSK_CommonSupportTasks_MainboardVoltageMonitoring
*                                      which observes the mainboard voltages.
*  \date                2012-11-09     <em>File version 1.2</em>. Changed disabled flag to static flag to set an voltage limit exceeding
*                                      error only once in task \ref TSK_CommonSupportTasks_MainboardVoltageMonitoring.
*                                      Used floats instead of long values for mainboard voltage calculation to calculate more precise.
*                                      Added device specific check for ADC channel with original voltage level of 24 V which adds 0.7 V
*                                      diode stolen voltage.
*  \date                2012-11-14     Changed distinction between ADC channels for setting modbus register address to channel identifier
*                                      instead of voltage levels.
*  \date                2012-11-15     <em>File version 1.3</em>. Added missing break statement for register bank access and a moved reset
*                                      for ADC channel number to beginning of task \ref TSK_CommonSupportTasks_MainboardVoltageMonitoring.
*                                      Moved also init of variables to task head because task variables are static.
*  \date                2012-11-23     <em>File version 1.4</em>. Deactived setting an error in case of mainboard voltage limit exceeding.
*
*
*  \bug                 2012-11-09     For-loop of task \ref TSK_CommonSupportTasks_MainboardVoltageMonitoring leads to overwriting
*                                      the error code, if other than last voltage monitoring channel has an inproper voltage.
*
*  \test                2012-07-13     Tested task \ref TSK_CommonSupportTasks_MainboardVoltageMonitoring for retrieving a mainboard
*                                      voltage every second and disabling feature.
*  \test                2012-11-09     Tested task \ref TSK_CommonSupportTasks_MainboardVoltageMonitoring for retrieving a mainboard
*                                      voltage every second and "error code set once" feature.
*
*  \todo
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <math.h>
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\..\cpu\at91\adcle\adcle.h"
#include "..\..\..\cpu\at91\rtt\rttext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"


#include "lib_exceptions.h"
#include "lib_exceptions_funcs.h"
#include "common_registers.h"
#include "utils_funcs.h"
#include "common_support_tasks.h"
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern            OS_SEM                                    SEM_CommonSupportFuncs_DeviceRunTimeCounter;
extern   const    ushort                                    cu16_UtilsFuncs_TimeoutShort;
extern   const    ushort                                    cu16_UtilsFuncs_TimeoutMedium;
extern   const    ushort                                    cu16_UtilsFuncs_TimeoutLong;
extern   const    U32                                       os_clockrate;
extern   tS_AT91SAM7X_MainboardVoltageMonitoring   SA_AT91SAM7X_MainboardVoltageMonitoring[];
//extern            tS_AT91SAM7X_MainboardVoltageMonitoring   SA_AT91SAM7X_MainboardVoltageMonitoring[];
/////*Copy to AT91SAM7X.c*/
//tS_AT91SAM7X_MainboardVoltageMonitoring SA_AT91SAM7X_MainboardVoltageMonitoring[AT91SAM7X__ADC_CHANNELS_MAX] =
//{
//                        // voltage divider resistance near                                // voltage divider resistance far                           // voltage threshold low                                          // voltage threshold high                                         // reference voltage
//   /* ADC channel 0 */  {0,                                                               0,                                                          0,                                                                0,                                                                0,                                           },
//   /* ADC channel 1 */  {0,                                                               0,                                                          0,                                                                0,                                                                0,                                           },
//   /* ADC channel 2 */  {0,                                                               0,                                                          0,                                                                0,                                                                0,                                           },
//   /* ADC channel 3 */  {0,                                                               0,                                                          0,                                                                0,                                                                0,                                           },
//   /* ADC channel 4 */  {(ulong)AT91SAM7X__ADC_CH4_VOLTAGE_DIVIDER_RESISTANCE_NEAR,       (ulong)AT91SAM7X__ADC_CH4_VOLTAGE_DIVIDER_RESISTANCE_FAR,   (ushort)AT91SAM7X__ADC_CH4_VOLTAGE_THRESHOLD_IN_MILLIVOLT_LOW,    (ushort)AT91SAM7X__ADC_CH4_VOLTAGE_THRESHOLD_IN_MILLIVOLT_HIGH,   (ushort)AT91SAM7X__ADC_CH4_VOLTAGE_REFERENCE },
//   /* ADC channel 5 */  {(ulong)AT91SAM7X__ADC_CH5_VOLTAGE_DIVIDER_RESISTANCE_NEAR,       (ulong)AT91SAM7X__ADC_CH5_VOLTAGE_DIVIDER_RESISTANCE_FAR,   (ushort)AT91SAM7X__ADC_CH5_VOLTAGE_THRESHOLD_IN_MILLIVOLT_LOW,    (ushort)AT91SAM7X__ADC_CH5_VOLTAGE_THRESHOLD_IN_MILLIVOLT_HIGH,   (ushort)AT91SAM7X__ADC_CH5_VOLTAGE_REFERENCE },
//   /* ADC channel 6 */  {0,                                                               0,                                                          0,                                                                0,                                                                0,                                           },
//   /* ADC channel 7 */  {0,                                                               0,                                                          0,                                                                0,                                                                0,                                           },
//
//};
//------------------------------------------------ tasks ---------------------------------------------------------------------------------------
__task void TSK_CommonSupportTasks_DeviceRunTimeCounter (void)
{
   U32                           u32_DeviceRunTimeCounter;
   uchar                         u8_Unit                    =  0;
   tS_LibExceptions_Exceptions   S_Exceptions;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   FOREVER
   {
      // wait for semaphore
      if (OS_SEM_TAKE_CHK(os_sem_take(SEM_CommonSupportFuncs_DeviceRunTimeCounter, OS_T_FOREVER)))
      {
         if (SUCCESS (RttRead (u8_Unit, &u32_DeviceRunTimeCounter, cu16_UtilsFuncs_TimeoutMedium),
                  S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_TASKS__TSK_DEVICE_RUN_TIME_COUNTER))
         {
            // store device run-time to permanent memory
            if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u32_DeviceRunTimeCounter), (ushort)AT91SAM7X__NUM_REGS_4,
                        (ushort)(COMMON_REGISTERS__FRAM_ADDR__DEVICE_RUN_TIME_COUNTER),
                        (bool)UTILS_FUNCS__WRITE),
                     S_Exceptions.u32_ErrorCodeSW_Sub))
            {
               // write total run-time to register bank
               TRY_AND_CATCH
               (
               MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE,
               (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_RUN_TIME_COUNTER, (ushort)AT91SAM7X__NUM_REGS_2,
               (void *)(&u32_DeviceRunTimeCounter), NULL, cu16_UtilsFuncs_TimeoutShort),
               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
               );
            }
         }
         if (S_Exceptions.u32_ErrorCodeSW_Sub)
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_TASKS__TSK_DEVICE_RUN_TIME_COUNTER;
            // set error codes to register bank
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
         }
      }
      RESCHEDULE(10);
   }
   DETACH;
   
}
//----------------------------------------------------------------------------------------------------------------------------------------------
__task void TSK_CommonSupportTasks_MainboardVoltageMonitoring (void)
{
   //   static   bool                          sb_VoltageLimitErrorNeverSet              =  (bool)TRUE;
   uchar                         u8_AdcChannel;
   uchar                         u8_AdcChannelId;
   uchar                         u8_Result                                 =  0;
   ushort                        u16_CurrentMainboardVoltageInMilliVolt;
   ushort                        u16_AdcResolutionSteps;
   ushort                        u16_Address;
   ushort                        u16A_AdcData[AT91SAM7X__ADC_CHANNELS_MAX];
   ushort                        u16_TimeoutSampleAndHold                  =  (ushort)AT91SAM7X__TICK_1;
   ushort                        u16_TimeoutDriver                         =  cu16_UtilsFuncs_TimeoutShort;
   ushort                        u16_DeviceClass;
   AT91PS_ADC                    pS_Adc                                    =  AT91C_BASE_ADC;
   tS_LibExceptions_Exceptions   S_Exceptions;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   // check if at least selected user channels, to be enabled during init, are enabled (init done)
   for (u8_AdcChannelId = 1; u8_AdcChannelId < (uchar)AT91C_ADC_CH7; u8_AdcChannelId = u8_AdcChannelId << 1)
   {
      if (u8_AdcChannelId & (uchar)pS_Adc->ADC_CHSR)
      u8_Result += u8_AdcChannelId;
   }
   if ((uchar)AT91SAM7X__ADC_CHANNELS_FOR_MAINBOARD_VOLTAGE_MONITORING != 0)
   {
      if (u8_Result >= (uchar)AT91SAM7X__ADC_CHANNELS_FOR_MAINBOARD_VOLTAGE_MONITORING)
      {
         FOREVER
         {
            // reset task variables
            u8_AdcChannel = 0;
            // retrieve ADC data
            if (FAIL (AdcGetConvertedData ((uchar)AT91SAM7X__ADC_CHANNELS_FOR_MAINBOARD_VOLTAGE_MONITORING, &u16A_AdcData[0],
                        u16_TimeoutSampleAndHold, u16_TimeoutDriver),
                     S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ADC__GET_CONVERTED_DATA))
            {
               S_Exceptions.u32_ErrorCodeSW_Sub=0;
               if (FAIL (AdcGetConvertedData ((uchar)AT91SAM7X__ADC_CHANNELS_FOR_MAINBOARD_VOLTAGE_MONITORING, &u16A_AdcData[0],
                           u16_TimeoutSampleAndHold, u16_TimeoutDriver),
                        S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ADC__GET_CONVERTED_DATA))
               {
                  S_Exceptions.u32_ErrorCodeSW_Sub=0;
                  if (SUCCESS (AdcGetConvertedData ((uchar)AT91SAM7X__ADC_CHANNELS_FOR_MAINBOARD_VOLTAGE_MONITORING, &u16A_AdcData[0],
                              u16_TimeoutSampleAndHold, u16_TimeoutDriver),
                           S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ADC__GET_CONVERTED_DATA))
                  {
                     S_Exceptions.u32_ErrorCodeSW_Sub=0;
                  }
               }
            }
            // search for active voltage monitoring channels
            for (u8_AdcChannelId = 1; u8_AdcChannelId < (uchar)AT91C_ADC_CH7; u8_AdcChannelId = u8_AdcChannelId << 1)
            {
               if (u8_AdcChannelId & (uchar)AT91SAM7X__ADC_CHANNELS_FOR_MAINBOARD_VOLTAGE_MONITORING)
               {
                  // set ADC resolution steps
                  if (!(pS_Adc->ADC_MR & (ulong)AT91C_ADC_LOWRES_10_BIT))     // caution: mask is zero
                  u16_AdcResolutionSteps = 0x03FF;
                  else if (pS_Adc->ADC_MR & (ulong)AT91C_ADC_LOWRES_8_BIT)
                  u16_AdcResolutionSteps = 0x00FF;
                  else
                  {
                     S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__ADC_INTERNAL_NOT_INITIALIZED;
                     break;
                  }
                  // convert current ADC value into voltage [mV]
                  if (u16A_AdcData[u8_AdcChannel] > u16_AdcResolutionSteps)
                  u16_CurrentMainboardVoltageInMilliVolt = (ushort)AT91SAM7X__ADC_VREF_IN_MILLIVOLT;
                  else
                  u16_CurrentMainboardVoltageInMilliVolt = (ushort)(rintf (((float)(u16_AdcResolutionSteps & u16A_AdcData[u8_AdcChannel]) * (float)AT91SAM7X__ADC_VREF_IN_MILLIVOLT) / (float)u16_AdcResolutionSteps));
                  // calc the present voltage at reference value according to voltage divider rule (no floats necessary)
                  u16_CurrentMainboardVoltageInMilliVolt =  (ushort)rintf ((float)u16_CurrentMainboardVoltageInMilliVolt *
                  ((float)(SA_AT91SAM7X_MainboardVoltageMonitoring[u8_AdcChannel].u32_VoltageDividerResistanceFar  +
                  SA_AT91SAM7X_MainboardVoltageMonitoring[u8_AdcChannel].u32_VoltageDividerResistanceNear   ) /
                  (float)SA_AT91SAM7X_MainboardVoltageMonitoring[u8_AdcChannel].u32_VoltageDividerResistanceNear));
                  // read current device id from register bank
                  u16_Address = (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START;
                  if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, u16_Address,
                              (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_DeviceClass),
                              NULL, cu16_UtilsFuncs_TimeoutShort),
                           S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                  {
                     // determine correct register bank address for mainboard voltage
                     if (u8_AdcChannelId == (uchar)AT91SAM7X__ADC_CHANNEL_24000_MILLIVOLT)
                     {
                        u16_Address = (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__MAINBOARD_CURRENT_VOLTAGE_AT_24000_MV;
                     }
                     else if (u8_AdcChannelId == (uchar)AT91SAM7X__ADC_CHANNEL_5000_MILLIVOLT)
                     {
                        u16_Address = (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__MAINBOARD_CURRENT_VOLTAGE_AT_5000_MV;
                     }
                     else if (u8_AdcChannelId == (uchar)AT91SAM7X__ADC_CHANNEL_3300_MILLIVOLT)
                     {
                        u16_Address = (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__MAINBOARD_CURRENT_VOLTAGE_AT_3300_MV;
                     }
                     else
                     {
                        S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__ADC_INTERNAL_NOT_INITIALIZED;
                        break;
                     }
                     // write mainboard voltage value into register bank
                     if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, u16_Address,
                                 (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_CurrentMainboardVoltageInMilliVolt),
                                 NULL, cu16_UtilsFuncs_TimeoutShort),
                              S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                     {
                     }
                     else
                     break;
                  }
                  else
                  break;
               }
               // increment channel number
               u8_AdcChannel++;
            }
            if (S_Exceptions.u32_ErrorCodeSW_Sub)
            {
               S_Exceptions.u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_TASKS__TSK_MAINBOARD_VOLTAGE_MONITORING;
               // set error codes to register bank
               LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
            }
            //os_dly_wait ((ushort)_OS_T_MS((ulong)AT91SAM7X__ADC_MAINBOARD_VOLTAGE_MONITORING_CLOCK_IN_MS, os_clockrate));
            RESCHEDULE((ulong)AT91SAM7X__ADC_MAINBOARD_VOLTAGE_MONITORING_CLOCK_IN_MS);//new
         }
      }
      else
      {
         S_Exceptions.u32_ErrorCodeSW_Sub    = (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__ADC_INTERNAL_NOT_INITIALIZED;
         S_Exceptions.u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_TASKS__TSK_MAINBOARD_VOLTAGE_MONITORING;
      }
   }
   // set error codes to register bank
   LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
   // quit this task because of not initialized ADC user channels	
   DETACH;   
}
