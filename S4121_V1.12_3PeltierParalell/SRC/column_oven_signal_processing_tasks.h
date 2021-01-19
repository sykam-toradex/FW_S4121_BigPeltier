/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_signal_processing_tasks.h
*
*  \brief               Header file for \ref column_oven_signal_processing_tasks.c
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2011-12-08     Initial version
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
#ifndef COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__H
#define COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__H
//---------------------------------------------- defines ---------------------------------------------------------------------------------------
//#define  COLUMN_OVEN_SUPPORT_TASKS__EVENT_TICK_TIME_200        (0x0001)
// defines for external ADC on chip registers
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__RESET_CMD                               (0xFFFFFFFFul)
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__REGS_ZMASK                              (0x00)
// use BIT0...BIT31 macros
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__WRITE_THIS                     ((uchar)(0 << 7))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__WRITE_BLOCKING                 ((uchar)(1 << 7))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__NEXT_CMD_READ                  ((uchar)(1 << 3))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__NEXT_CMD_WRITE                 ((uchar)(0 << 3))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__CONTINUOUS_READ                ((uchar)(1 << 2))                // is a mode option; continuous read of data register does not require a write access to communication register
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__SELECT_STATUS_COM_REG          ((uchar)((0 << 5) | (0 << 4)))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__SELECT_MODE_REG                ((uchar)((0 << 5) | (1 << 4)))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__SELECT_DATA_REG                ((uchar)((1 << 5) | (1 << 4)))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__ADC_CHANNEL_VDD_MONITOR        ((uchar)((1 << 1) | (1 << 0)))   // is a mode option
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__ADC_CHANNEL_AIN_MINUS_ONLY     ((uchar)((1 << 1) | (0 << 0)))   // is a mode option
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__ADC_CHANNEL_AIN_ALT            ((uchar)((0 << 1) | (0 << 0)))   // is a mode option
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__DATA_NOT_READY              ((uchar)(1 << 7))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__ERROR                       ((uchar)(1 << 6))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__DEV_ID_AD7788               ((uchar)~BIT2)                   // theoretically: ((uchar)(0 << 2))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__DEV_ID_AD7789               ((uchar)BIT2)
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__ADC_CHANNEL_VDD_MONITOR     ((uchar)((1 << 1) | (1 << 0)))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__ADC_CHANNEL_AIN_MINUS_ONLY  ((uchar)((1 << 1) | (0 << 0)))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__ADC_CHANNEL_RESERVED        ((uchar)((0 << 1) | (1 << 0)))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__ADC_CHANNEL_AIN_ALT         ((uchar)((0 << 1) | (0 << 0)))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MODE_REG__CONT_CONV                     ((uchar)((0 << 7) | (0 << 6)))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MODE_REG__SINGLE_CONV                   ((uchar)((1 << 7) | (0 << 6)))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MODE_REG__POWER_DOWN                    ((uchar)((1 << 7) | (1 << 6)))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MODE_REG__DOUT_UNIPOLAR                 ((uchar)(1 << 2))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MODE_REG__DOUT_BIPOLAR                  ((uchar)(0 << 2))
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MODE_REG__STATIC_BIT1                   ((uchar)(1 << 1))                // must be always set for correct device operation
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MAX_VALUE                               (52088)                          // corresponds to appr. 200 °C
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__TIME_SCALE_FACTOR_IN_SECONDS                            (6)
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__SET_POINT_REACHED_BAND_SAMPLE_COUNTER_MIN               (300) // with 200 ms cycle time, this time is equal to 1 min
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__SMALLEST_USER_TEMPERATURE_INCREMENT_IN_DEG              (0.1) // with 200 ms cycle time, this time is equal to 1 min
#define  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__SMALLEST_USER_TEMPERATURE_INCREMENT_IN_DEG_MUL_TEN      (1)
//---------------------------------------------- structures ------------------------------------------------------------------------------------
struct   _S_ColumnOvenSignalProcessingTasks_AdcExtAD7788RegistersRW
{
   uchar    u8_Communication;
   uchar    u8_Mode;
};
struct   _S_ColumnOvenSignalProcessingTasks_AdcExtAD7788RegistersRO
{
   uchar    u8_Status;
   ushort   u16_Data;
};
struct _S_ColumnOvenSignalProcessingTasks_AdcExtAD7788
{
   struct _S_ColumnOvenSignalProcessingTasks_AdcExtAD7788RegistersRW RegsRW;
   struct _S_ColumnOvenSignalProcessingTasks_AdcExtAD7788RegistersRO RegsRO;
};
//---------------------------------------------- externals defined in this module --------------------------------------------------------------
extern         bool  b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified;
extern   const float cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate;
extern   const float cf_ColumnOvenSignalProcessingTasks_TaskCyclesPerSecond;
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
__task   void     TSK_ColumnOvenSignalProcessingTasks_TemperatureControl     (void);
#endif
// docu of tasks
