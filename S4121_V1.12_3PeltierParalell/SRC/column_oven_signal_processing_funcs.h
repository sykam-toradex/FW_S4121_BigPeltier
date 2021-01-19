/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_signal_processing_funcs.h
*
*  \brief               Header file for \ref column_oven_signal_processing_funcs.c
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2010-07-29     Initial version
*  \date                2011-02-03     Imported S6510_valves.c.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test
*
*  \todo                check DOCU!!!
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__H
#define COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__H
//---------------------------------------------- defines ---------------------------------------------------------------------------------------
#define  COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__POWER_MODE_SLOPE_DOWN                                (1)
#define  COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__POWER_MODE_SLOPE_UP                                  (2)
#define  COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_INCR                                      (25)                             // max. value determined in prototype measurements -> 31; subtract 20 % for part tolerances
//#define  COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_VOLTAGE_MAX                               (DACRRCS085_OUTPUT_VOLTAGE_MAX)
//#define  COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_VOLTAGE_MIN                               (DACRRCS085_OUTPUT_VOLTAGE_MIN)
//#define  COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_VOLTAGE_MAX_START                         (1112)                           // in [mV]
#define  COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_MAX_START                                 (345)                            // max. DAC value to switch power on
#define  COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__DAC_OUTPUT_MAX_START_PELTIER                         (150)                            // max. DAC value to switch power on
//---------------------------------------------- structures an enums ---------------------------------------------------------------------------
typedef struct _S_ColumnOvenSignalProcessing_TemperatureRamp
{
   ushort   u16_SetPoint;
   ulong    u32_CyclesToRefreshSetPoint;
   ulong    u32_CycleCounter;
} tS_ColumnOvenSignalProcessing_TemperatureRamp;
//---------------------------------------------- externals defined in this module --------------------------------------------------------------
extern            OS_TID   TID_ColumnOvenSignalProcessingFuncs_TemperatureControl;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaBLow;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaBHigh;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaCLow;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_DacValueAtAreaCHigh;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_NewSlopeAreaB;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_NewSlopeAreaC;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_OffsetAreaB;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_OffsetAreaC;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_IntegrationTimeBase;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_HeatingFoilHeatRate;
extern   const    float    cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate;
extern   const    struct   _S_ColumnOvenRegisters_ControlSectionParams cSSA_ColumnOvenSignalProcessingFuncs_ControlSectionParams[][AT91SAM7X__MAX_CONTROL_PARAM_SECTIONS];
//extern   OS_SEM   SEM_ColumnOvenSupportFuncs_GasSensing;
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
ulong ColumnOvenSignalProcessingFuncs_InitTasks                            (void);
ulong ColumnOvenSignalProcessingFuncs_GetTemperature    (ushort u16_AdcValue, float* pf_TemperatureInDegree);
ulong ColumnOvenSignalProcessingFuncs_CalcControlParams (tS_ColumnOvenRegisters_StatusDataExtern *pS_StatusDataExtern, tS_ColumnOvenRegisters_StatusDataIntern *pS_StatusDataIntern, bool *pb_CalcControlParams, bool *pb_SignalSmoothing, tS_ColumnOvenSignalProcessing_TemperatureRamp *psS_TemperatureRamp, bool b_CurrentSectionIsRamp, ushort u16_SykamDeviceOptions);
ulong ColumnOvenSignalProcessingFuncs_GetControlOutput  (tS_ColumnOvenRegisters_StatusDataExtern *pS_StatusDataExtern, tS_ColumnOvenRegisters_StatusDataIntern *pS_StatusDataIntern,
float *pf_ControlOutput, ushort u16_ControlType, tS_ColumnOvenSignalProcessing_TemperatureRamp *psS_TemperatureRamp,
bool b_FlagCalcControlParams, bool b_FlagCurrentSectionIsRamp);
#endif
// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ColumnOvenSignalProcessingFuncs_InitTasks (void)
*
*  \brief      This function .
*
*  \return     Returns an ulong <b>sub error code</b> which is greater than zero if any error occurred. See \ref _E_.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// comment for function SetProfile.... --> other oven states must be blocked before calling this function
