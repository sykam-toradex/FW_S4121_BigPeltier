/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_support_funcs.h
*
*  \brief               Header file for \ref column_oven_support_funcs.c
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
#ifndef COLUMN_OVEN_SUPPORT_FUNCS__H
#define COLUMN_OVEN_SUPPORT_FUNCS__H
//---------------------------------------------- defines ---------------------------------------------------------------------------------------
#define  COLUMN_OVEN_SUPPORT_FUNCS__ADC_CHANNEL0                              (0)
#define  COLUMN_OVEN_SUPPORT_FUNCS__ADC_CHANNEL1                              (1)
#define  COLUMN_OVEN_SUPPORT_FUNCS__ADC_CHANNEL2                              (2)
#define  COLUMN_OVEN_SUPPORT_FUNCS__ADC_CHANNEL3                              (3)
#define  COLUMN_OVEN_SUPPORT_FUNCS__ADC_CHANNEL4                              (4)
#define  COLUMN_OVEN_SUPPORT_FUNCS__ADC_CHANNEL5                              (5)
#define  COLUMN_OVEN_SUPPORT_FUNCS__ADC_CHANNEL6                              (6)
#define  COLUMN_OVEN_SUPPORT_FUNCS__ADC_CHANNEL7                              (7)
#define  CONVERT_VOLTAGE_INTO_10BIT_DAC_VALUE( VoltageInMilliVolt )           (VoltageInMilliVolt > AT91SAM7X__DACRRCS085_OUTPUT_VOLTAGE_MAX) ?   \
   (AT91SAM7X__DACRRCS085_OUTPUT_VOLTAGE_MAX) :                        \
   ((VoltageInMilliVolt * 1024) / AT91SAM7X__DACRRCS085_VREF)
#define  CONVERT_10BIT_DAC_VALUE_INTO_VOLTAGE( DAC_Value )                    (DAC_Value > AT91SAM7X__DACRRCS085_OUTPUT_MAX) ?                    \
   (AT91SAM7X__DACRRCS085_OUTPUT_MAX) :                                \
   ((DAC_Value * AT91SAM7X__DACRRCS085_VREF) / 1024 )
#define  COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_WRITE    (1)
#define  COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_READ     (2)
#define  COLUMN_OVEN_SUPPORT_FUNCS__CONTROL_SECTION_PARAMS_CHECK_FOR_INIT     (3)
#define  COLUMN_OVEN_SUPPORT_FUNCS__CODE_DEVICE_PARAMS_INTERN_STORED          (0xA5E8)
//---------------------------------------------- externals defined in this module --------------------------------------------------------------
extern   OS_SEM   SEM_ColumnOvenSupportFuncs_GasSensing;
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
ulong ColumnOvenSupportFuncs_InitTasks                         (void);
ulong ColumnOvenSupportFuncs_HeatCoolProgramLoad               (ushort u16_HeatCoolProgramId, void *pv_HeatCoolProgram);
ulong ColumnOvenSupportFuncs_DeviceParamsInternCheck           (void *pv_DeviceParamsIntern, ushort *pu16_ControlSectionsNum, bool b_FlagValidationType);
ulong ColumnOvenSupportFuncs_SetDacOutput                      (float f_DacDesiredOutput);
ulong ColumnOvenSupportFuncs_CalcHeatCoolProgramTimes          (tS_ColumnOvenRegisters_StatusDataExtern *pS_StatusDataExtern, tS_ColumnOvenRegisters_StatusDataIntern *pS_StatusDataIntern, bool *pb_ReturnFlagCalcControlParams, bool *pb_ReturnFlagCurrentSectionIsRamp, ushort *pu16_Counter, tS_ColumnOvenSignalProcessing_TemperatureRamp *psS_TemperatureRamp, ushort u16_SykamDeviceOptions, ushort u16_SykamDeviceClass, ushort u16_SetPointReachedBand);
ulong ColumnOvenSupportFuncs_StoreGasSensorDefaultValues       (void);
#endif
// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ColumnOvenSupportFuncs_InitTasks (void)
*
*  \brief      This function .
*
*  \return     Returns an ulong <b>sub error code</b> which is greater than zero if any error occurred. See \ref _E_.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ColumnOvenSupportFuncs_HeatCoolProgramLoad (ushort u16_HeatCoolProgramId, void *pv_HeatCoolProgram)
*
*  \brief      This function .
*
*  \param      *pv_Dest                   The destination address at which the swap function shall start.
*
*  \param      u16_NumWords               The number of words in which a byte swap is done. E.g. to swap the bytes of a long
*                                         0xAABBCCDD \em u16_NumWords shall be equal to 0x0002. The result will be 0xBBAADDCC.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ColumnOvenSupportFuncs_DeviceParamsInternCheck (void *pv_DeviceParamsIntern, ushort *u16_ControlSectionsNum, bool b_FlagValidationType)
*
*  \brief      This function .
*
*  \param      *pv_Dest                   The destination address at which the swap function shall start.
*
*  \param      u16_NumWords               The number of words in which a byte swap is done. E.g. to swap the bytes of a long
*                                         0xAABBCCDD \em u16_NumWords shall be equal to 0x0002. The result will be 0xBBAADDCC.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
