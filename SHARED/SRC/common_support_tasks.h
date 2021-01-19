/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_support_tasks.h
*
*  \brief               Header file for \ref common_support_tasks.c
*
*  \author              Ruediger Radtke
*  \version             1.2
*
*  \date                2011-08-30     Initial version.
*  \date                2012-07-13     <em>File version 1.1</em>. Added task prototype \ref TSK_CommonSupportTasks_MainboardVoltageMonitoring.
*  \date                2012-11-12     <em>File version 1.2</em>. Changed documentation of task \ref TSK_CommonSupportTasks_MainboardVoltageMonitoring.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef COMMON_SUPPORT_TASKS__H
#define COMMON_SUPPORT_TASKS__H

//---------------------------------------------- Internal ADC Config---------------------------------------------------------------------------

//#define  AT91SAM7X__ADC_CHANNELS_MAX                              (8)
//#define  AT91SAM7X__ADC_CH4_VOLTAGE_DIVIDER_RESISTANCE_NEAR       (10000)                       // in Ohm
//#define  AT91SAM7X__ADC_CH4_VOLTAGE_DIVIDER_RESISTANCE_FAR        (100000)
//#define  AT91SAM7X__ADC_CH4_VOLTAGE_THRESHOLD_IN_MILLIVOLT_LOW    (22560)                       // set 6 % tolerance, VRef_max = (3% * 24V) + 24 V = 23.28 V; VAdc_nom = 24 V - (24 V * (100k/(100k+10k))) = 2.181 V; VAdc_max = VAdc_nom + tolerance_3%@24V_with_tolerance_1%@VoltageDivider = 2.18 V + 0.655 V = 2.836 V
//#define  AT91SAM7X__ADC_CH4_VOLTAGE_THRESHOLD_IN_MILLIVOLT_HIGH   (25440)                       // set 6 % tolerance, VRef_min = - (3% * 24V) + 24 V = 24.72 V; VAdc_min = 1.526 V;
//#define  AT91SAM7X__ADC_CH4_VOLTAGE_REFERENCE                     (24000)                       // in [mV]
//#define  AT91SAM7X__ADC_CH5_VOLTAGE_DIVIDER_RESISTANCE_NEAR       (10000)                       // in Ohm
//#define  AT91SAM7X__ADC_CH5_VOLTAGE_DIVIDER_RESISTANCE_FAR        (10000)
//#define  AT91SAM7X__ADC_CH5_VOLTAGE_REFERENCE                     (5000)                        // in [mV]
//#define  AT91SAM7X__ADC_CH5_VOLTAGE_THRESHOLD_IN_MILLIVOLT_LOW    (4850)                        // VRef_max = (3% * 5V) + 5 V = 5.15 V; VAdc_nom = 5 V - (5 V * (10k/(10k+10k))) = 2.500 V
//#define  AT91SAM7X__ADC_CH5_VOLTAGE_THRESHOLD_IN_MILLIVOLT_HIGH   (5150)                        // VRef_min = - (3% * 5V) + 5 V = 4.85 V
//#define  AT91SAM7X__ADC_CHANNEL_24000_MILLIVOLT                   (AT91C_ADC_CH4)
//#define  AT91SAM7X__ADC_CHANNEL_5000_MILLIVOLT                    (AT91C_ADC_CH5)
//#define  AT91SAM7X__ADC_CHANNEL_3300_MILLIVOLT                    (NULL)
//#define  AT91SAM7X__ADC_CHANNELS_FOR_MAINBOARD_VOLTAGE_MONITORING (AT91SAM7X__ADC_CHANNEL_24000_MILLIVOLT | AT91SAM7X__ADC_CHANNEL_5000_MILLIVOLT)
//#define  AT91SAM7X__ADC_CLOCK_IN_HZ                               (5000000)
//#define  AT91SAM7X__ADC_STARTUP_TIME_IN_MICRO_SECONDS             (10)
//#define  AT91SAM7X__ADC_SAMPLE_AND_HOLD_TIME_IN_NANO_SECONDS      (1200)
//#define  AT91SAM7X__ADC_SLEEP_ON_OFF                              (AT91C_ADC_SLEEP_NORMAL_MODE) // power save mode off
//#define  AT91SAM7X__ADC_RESOLUTION                                (AT91C_ADC_LOWRES_10_BIT)     // 10 bit resolution
//#define  AT91SAM7X__ADC_TRIGGER_ON_OFF                            (AT91C_ADC_TRGEN_DIS)         // ADC triggering disabled
//#define  AT91SAM7X__ADC_TRIGGER_SELECTION                         (0)                           // unsed as long as ADC triggering is disabled
//#define  AT91SAM7X__ADC_VREF_IN_MILLIVOLT                         (3300)
//#define  AT91SAM7X__ADC_MAINBOARD_VOLTAGE_MONITORING_CLOCK_IN_MS  (1000)                        // [ms]
////---------------------------------------------- function prototypes ---------------------------------------------------------------------------
//typedef struct _S_AT91SAM7X_MainboardVoltageMonitoring
//{
//   ulong    u32_VoltageDividerResistanceNear;
//   ulong    u32_VoltageDividerResistanceFar;
//   ushort   u16_VoltageThresholdInMilliVoltLow;
//   ushort   u16_VoltageThresholdInMilliVoltHigh;
//   ushort   u16_VoltageReferenceInMilliVolt;
//   
//}  tS_AT91SAM7X_MainboardVoltageMonitoring;
////---------------------------------------------- externals defined in this module --------------------------------------------------------------
//
//extern   tS_AT91SAM7X_MainboardVoltageMonitoring   SA_AT91SAM7X_MainboardVoltageMonitoring[];

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

__task   void     TSK_CommonSupportTasks_DeviceRunTimeCounter        (void);
__task   void     TSK_CommonSupportTasks_MainboardVoltageMonitoring  (void);

#endif


// docu of tasks
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         __task void TSK_CommonSupportTasks_DeviceRunTimeCounter (void)
*
*  \brief      This task is responsible to write periodically the run-time seconds of device into common register bank (status data)
*              and to permanent memory. The period of 10 s is set with function \ref RttRestart which is called at start-up and at 
*              the reset of run-time counter (command \ref CommonCmdsOwner_DeviceRunTimeCounterReset).
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         __task void TSK_CommonSupportTasks_MainboardVoltageMonitoring (void)
*
*  \brief      This task observes the mainboard voltages. For this purpose the internal ADC is used. The voltage monitoring must be 
*              configured by user in \ref SA_AT91SAM7X_MainboardVoltageMonitoring and in board file \ref AT91SAM7X.h. The user has to
*              set voltage threshold levels as well as available ADC input connections. The task is repeated cyclic with a user period
*              of \ref AT91SAM7X__ADC_MAINBOARD_VOLTAGE_MONITORING_CLOCK_IN_MS. An voltage limit exceeding is set as corresponding 
*              error code only \b once at runtime.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
