/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_signal_processing_tasks.c
*
*  \brief               The file contains  \n
*
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2011-12-08     Initial version.
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
#include <stdio.h>                              // temp for debugging
#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\cpu\at91\spi\spi.h"
#include "..\..\..\cpu\at91\spi\spiext.h"
#include "..\..\..\cpu\at91\usart\usart.h"      // temp for debugging
#include "..\..\..\cpu\at91\usart\usartext.h"   // temp for debugging
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085.h"
#include "..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085ext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\lib_exceptions_funcs.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\common_support_funcs.h"
#include "column_oven_registers.h"
#include "column_oven_support_tasks.h"
#include "column_oven_signal_processing_tasks.h"
#include "column_oven_signal_processing_funcs.h"
#include "column_oven_control_params.h"
#include "column_oven_support_funcs.h"
//---------------------------------------------- global definitions ----------------------------------------------------------------------------
bool  b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified    =  FALSE;
const    float cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate  =  (((float)COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__TIME_SCALE_FACTOR_IN_SECONDS) /
(((float)OS_TICK_RATE / 1000000.0) * ((float)AT91SAM7X__REPETITION_TICKS_FOR_TSK_GLOBAL_TIMER)));
const    float cf_ColumnOvenSignalProcessingTasks_TaskCyclesPerSecond      =  (((float)1.0) /
(((float)OS_TICK_RATE / 1000000.0) * ((float)AT91SAM7X__REPETITION_TICKS_FOR_TSK_GLOBAL_TIMER)));
const    float cf_ColumnOvenSignalProcessingTasks_SmoothingFactorAlpha     =  0.1;
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern         OS_SEM   SEM_ColumnOvenMain_SpiNewMsg_Channel[SPI_CHANNEL_MAX];
//extern            OS_SEM   SEM_ColumnOvenSupportFuncs_GasSensing;
//extern            OS_TID   TID_TemperatureControl;
//extern   const float    cf_ColumnOvenSignalProcessingFuncs_PeltierHeatCoolRate;   // temp!!!!!
extern   const    ushort   cu16_TimeoutShort;
extern   const    ushort   cu16_TimeoutLong;
//------------------------------------------------ tasks ---------------------------------------------------------------------------------------
/*
default access order on AD7788:
   1) after device reset or power-up, device waits for a write access on communication register
      (a subsequent write/read access to other registers results in waiting for an access to communication register again)
   2) to be continued ...
*/
__task void TSK_ColumnOvenSignalProcessingTasks_TemperatureControl (void)
{
   uchar    u8_Unit                    =  0;
   uchar    u8_Channel                 =  0;
   uchar    u8_AdcNumBytesRxTx         =  0;
   uchar    u8A_AdcDataTx[3];
   uchar    u8A_AdcDataRx[3];
   ushort   u16_SykamDeviceOptions     =  0;
   ushort   u16_SykamDeviceClass       =  0;
   ushort   u16_Result                 =  0;
   static   ushort   u16_TemperatureOffset      =  0;
   ushort   u16_SetPointReachedBand    =  0;
   ushort   u16_GasSensorAlarmEnabled  =  0;
   ulong    u32_Msg                    =  0;
   static   float    f_TemperatureMeasured      =  0.0;
   static   float    f_ControlOutput            =  0.0;
   //float    f_ControlDeviation;
   /*
   // debug >>>>>>>>>>>>>>>>>
   double f_CoefficientA          = 0.0039083;
   double f_CoefficientB          = -0.0000005775;
   double f_ResistanceReference   =  1000.0;
   static double f_Tmp1, f_Tmp2, f_Tmp3;
   static ushort u16_Idx = 0;
   ushort u16_Idx2 = 0;
   static   double   f_ResistanceMeasured;
   static   double   f_TemperatureMeasuredAlt;
   static   double   fA_TemperatureMeasuredAltLast[5] = {0,0,0,0,0};
   static   double   f_TemperatureMeasuredAltFiltered;
   // <<<<<<<<<<<< debug end
   */
   bool     b_CalcControlParams        =  FALSE;
   // data structures
   struct   _S_ColumnOvenSignalProcessingTasks_AdcExtAD7788    S_AdcExt;
   struct   _SMBPBRegAccess                                    S_ModbusAccessParams;
   static   struct   _S_ColumnOvenRegisters_StatusDataIntern            S_StatusDataIntern;
   static   struct   _S_ColumnOvenRegisters_StatusDataExtern            S_StatusDataExtern;
   static   struct   _S_ColumnOvenSignalProcessing_TemperatureRamp   sS_TemperatureRamp = {0,0,0};
   static   ushort   su16_Counter                        =  0;
   static   float    sf_ControlOutputLast;
   static   bool     sb_CurrentSectionIsRamp             =  FALSE;   // must be static because ramp params must be calculated in every sample time
   //static   bool     sb_DisableHeat                      =  FALSE;
   static   bool     sb_TemperatureSensorFailureSet      =  FALSE;
   // debug
   bool     b_SignalSmoothing  =  TRUE;
   //char     s8A_UsartDataTx[180];
   //static float f_Tmp;
   tS_LibExceptions_Exceptions   S_Exceptions;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   FOREVER
   {
      // wait for event - if timeout expired an error is set because task calling was delayed
      if (OS_R_EVT == os_evt_wait_and((ushort)COLUMN_OVEN_SUPPORT_TASKS__EVENT_FOR_TSK_TEMPERATURE_CONTROL, (ushort)(AT91SAM7X__TICK_300)))
      {
         // read out EXtern status data
         S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
         S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
         S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;  // register start address
         S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);                                      // total words of S_StatusDataExtern
         S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
         S_ModbusAccessParams.pvIntern     =  NULL;                                                                         // debug purpose, set to &pv_BufferAddress if needed
         S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;  // !!! check if timeout is suffcient if serial port is stressed!!
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
         }
         // read out INtern status data
         S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
         S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
         }
         // read out sykam device data option field
         S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
         S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceOptions)/2);
         S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceOptions);
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
         }
         // read out sykam device class
         S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
         S_ModbusAccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SykamDeviceClass)/2);
         S_ModbusAccessParams.pvData       =  (void *) (&u16_SykamDeviceClass);
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
         }
         // read out temperature offset
         S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_OFFSET;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_TemperatureOffset)/2);
         S_ModbusAccessParams.pvData       =  (void *) (&u16_TemperatureOffset);
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
         }
         // read out set point reached band
         S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(u16_SetPointReachedBand)/2);
         S_ModbusAccessParams.pvData       =  (void *) (&u16_SetPointReachedBand);
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
         }
         // read enabled state from register bank again (owner/service technician might disable sensor during run time)
         if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_ENABLED,
                     (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_GasSensorAlarmEnabled), NULL, cu16_TimeoutShort),
                  S_Exceptions.u32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
         }
         // init structure for ADC communication
         // first communication reg value: write com reg + select alternate input channel + next command is a write command
         S_AdcExt.RegsRW.u8_Communication =
         (
         COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__WRITE_THIS            |
         COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__NEXT_CMD_WRITE        |
         COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__SELECT_MODE_REG       |
         COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__ADC_CHANNEL_AIN_ALT   |
         COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__REGS_ZMASK
         );
         // first mode reg value: trigger a single conversion + set output to unipolarity (0x0000...0xFFFF) + set "must" bit1
         S_AdcExt.RegsRW.u8_Mode       =
         (
         COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MODE_REG__SINGLE_CONV    |
         COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MODE_REG__DOUT_UNIPOLAR  |
         COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MODE_REG__STATIC_BIT1    |
         COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__REGS_ZMASK
         );
         S_AdcExt.RegsRO.u16_Data      =  0;
         S_AdcExt.RegsRO.u8_Status     =  0;
         u8A_AdcDataTx[0]              =  S_AdcExt.RegsRW.u8_Communication;
         u8A_AdcDataTx[1]              =  S_AdcExt.RegsRW.u8_Mode;
         u8_AdcNumBytesRxTx            =  2;
         u8_Channel                    =  (uchar)AT91SAM7X__ADC_EXT_AD7788__SPI_CHANNEL;
         // ##################################  write COMMUNICATION REG ###################################################
         // send new contents of communication and mode register to ADC (trigger a new conversion)
         u16_Result= SpiTx (u8_Unit, u8_Channel, u8A_AdcDataTx, u8_AdcNumBytesRxTx, cu16_TimeoutLong);
         if (u16_Result != u8_AdcNumBytesRxTx)
         {
            S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__TX;
            // set exception code to register bank
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
         }
         // retrieve ADC response only for successful SPI transmission
         if (OS_R_TMO != os_sem_wait (SEM_ColumnOvenMain_SpiNewMsg_Channel[u8_Channel], cu16_TimeoutLong))
         {
            // read error counter (from last transmit?)
            u16_Result = SpiMsgChannel (u8_Unit, u8_Channel, (ulong)AT91C_SPI_OVRES, FALSE, cu16_TimeoutShort);
            if (u16_Result)
            {
               S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL;
               // set exception code to register bank
               LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
            }
            // Read dummy bytes from ADC. No data is available since conversion was just started.
            u16_Result = SpiRx (u8_Unit, u8_Channel, &(S_AdcExt.RegsRO.u16_Data), u8_AdcNumBytesRxTx, cu16_TimeoutShort);
            if (u16_Result != u8_AdcNumBytesRxTx)
            {
               S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__RX;
               // set exception code to register bank
               LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
            }
            u32_Msg     =  (ulong)AT91C_SPI_RXBUFF;
            u16_Result  =  SpiMsgChannel (u8_Unit, u8_Channel, u32_Msg, u8_AdcNumBytesRxTx, cu16_TimeoutShort);
            if (u16_Result != u8_AdcNumBytesRxTx)
            {
               S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL;
               // set exception code to register bank
               LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
            }
            u32_Msg     =  (ulong)AT91C_SPI_TXBUFE;
            u16_Result  =  SpiMsgChannel (u8_Unit, u8_Channel, u32_Msg, u8_AdcNumBytesRxTx, cu16_TimeoutShort);
            if (u16_Result != u8_AdcNumBytesRxTx)
            {
               S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL;
               // set exception code to register bank
               LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
            }
            // wait for conversion of ADC
            // ADC conversion time = 120 ms (in power down mode), ADC wake-up time = 1 ms,
            os_dly_wait (OS_T_MS(130));
            // ##################################  read STATUS REG ###################################################
            // second communication reg value: write com reg + select alternate input channel + next command is read status reg
            S_AdcExt.RegsRW.u8_Communication = (
            COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__WRITE_THIS            |
            COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__NEXT_CMD_READ         |
            COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__SELECT_STATUS_COM_REG |
            COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__ADC_CHANNEL_AIN_ALT   |
            COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__REGS_ZMASK
            );
            u8_AdcNumBytesRxTx   =  2;
            u8A_AdcDataTx[0]     =  S_AdcExt.RegsRW.u8_Communication;
            u8A_AdcDataTx[1]     =  0;
            // read status register - send new contents of communication register to ADC
            u16_Result = SpiTx (u8_Unit, u8_Channel, u8A_AdcDataTx, u8_AdcNumBytesRxTx, cu16_TimeoutShort);
            if (u16_Result != u8_AdcNumBytesRxTx)
            {
               S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__TX;
               // set exception code to register bank
               LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
            }
            // retrieve ADC response only for successful SPI transmission
            if (OS_R_TMO != os_sem_wait (SEM_ColumnOvenMain_SpiNewMsg_Channel[u8_Channel], cu16_TimeoutLong))
            {
               // read error counter (from last transmit?)
               u16_Result = SpiMsgChannel (u8_Unit, u8_Channel, (ulong)AT91C_SPI_OVRES, FALSE, cu16_TimeoutShort);
               if (u16_Result)
               {
                  S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL;
                  // set exception code to register bank
                  LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
               }
               // retrieve ADC status register
               u16_Result = SpiRx (u8_Unit, u8_Channel, &(u8A_AdcDataRx[0]), u8_AdcNumBytesRxTx, cu16_TimeoutShort);
               if (u16_Result != u8_AdcNumBytesRxTx)
               {
                  S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__RX;
                  // set exception code to register bank
                  LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
               }
               u32_Msg     =  (ulong)AT91C_SPI_RXBUFF;
               u16_Result  =  SpiMsgChannel (u8_Unit, u8_Channel, u32_Msg, u8_AdcNumBytesRxTx, cu16_TimeoutShort);
               if (u16_Result != u8_AdcNumBytesRxTx)
               {
                  S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL;
                  // set exception code to register bank
                  LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
               }
               u32_Msg     =  (ulong)AT91C_SPI_TXBUFE;
               u16_Result  =  SpiMsgChannel (u8_Unit, u8_Channel, u32_Msg, u8_AdcNumBytesRxTx, cu16_TimeoutShort);
               // set status register value
               S_AdcExt.RegsRO.u8_Status = u8A_AdcDataRx[1];   // ADC status data is received AFTER sending request byte
               // Check status register for errors and correct settings.
               if((!(COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__ERROR                       &  S_AdcExt.RegsRO.u8_Status))   &&
                     (!(COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__DATA_NOT_READY              &  S_AdcExt.RegsRO.u8_Status))   &&
                     (!(COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__DEV_ID_AD7789               &  S_AdcExt.RegsRO.u8_Status))   &&
                     (!(COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__ADC_CHANNEL_AIN_MINUS_ONLY  &  S_AdcExt.RegsRO.u8_Status))   &&
                     (!(COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__ADC_CHANNEL_VDD_MONITOR     &  S_AdcExt.RegsRO.u8_Status))   &&
                     (!(COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__STATUS_REG__ADC_CHANNEL_RESERVED        &  S_AdcExt.RegsRO.u8_Status))        )
               {
                  // ##################################  read DATA REG ###################################################
                  // third communication reg value: write com reg + select alternate input channel +
                  //                                next command is read data reg
                  S_AdcExt.RegsRW.u8_Communication = (
                  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__WRITE_THIS            |
                  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__NEXT_CMD_READ         |
                  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__SELECT_DATA_REG       |
                  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__COM_REG__ADC_CHANNEL_AIN_ALT   |
                  COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__REGS_ZMASK
                  );
                  u8A_AdcDataTx[0]     =  S_AdcExt.RegsRW.u8_Communication;
                  u8A_AdcDataTx[1]     =  0;
                  u8A_AdcDataTx[2]     =  0;
                  u8_AdcNumBytesRxTx   =  3; // one request byte and two response data bytes from ADC
                  // send new contents of communication register and one dummy byte to ADC (read data register)
                  u16_Result= SpiTx (u8_Unit, u8_Channel, u8A_AdcDataTx, u8_AdcNumBytesRxTx, cu16_TimeoutShort);
                  if (u16_Result != u8_AdcNumBytesRxTx)
                  {
                     S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__TX;
                     // set exception code to register bank
                     LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
                  }
                  // retrieve ADC response only for successful SPI transmission
                  if (OS_R_TMO != os_sem_wait (SEM_ColumnOvenMain_SpiNewMsg_Channel[u8_Channel], cu16_TimeoutLong))
                  {
                     // read error counter (from last transmit?)
                     u16_Result = SpiMsgChannel (u8_Unit, u8_Channel, (ulong)AT91C_SPI_OVRES, FALSE, cu16_TimeoutShort);
                     if (u16_Result)
                     {
                        S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL;
                        // set exception code to register bank
                        LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
                     }
                     // retrieve ADC data register
                     u16_Result = SpiRx (u8_Unit, u8_Channel, &(u8A_AdcDataRx[0]), u8_AdcNumBytesRxTx, cu16_TimeoutShort);
                     if (u16_Result != u8_AdcNumBytesRxTx)
                     {
                        S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__RX;
                        // set exception code to register bank
                        LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
                     }
                     // decrement message counters about amount of transmitted/received bytes
                     u32_Msg     =  (ulong)AT91C_SPI_RXBUFF;
                     u16_Result  =  SpiMsgChannel (u8_Unit, u8_Channel, u32_Msg, u8_AdcNumBytesRxTx, cu16_TimeoutShort);
                     if (u16_Result != u8_AdcNumBytesRxTx)
                     {
                        S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL;
                        // set exception code to register bank
                        LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
                     }
                     u32_Msg     =  (ulong)AT91C_SPI_TXBUFE;
                     u16_Result  =  SpiMsgChannel (u8_Unit, u8_Channel, u32_Msg, u8_AdcNumBytesRxTx, cu16_TimeoutShort);
                     if (u16_Result != u8_AdcNumBytesRxTx)
                     {
                        S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL;
                        // set exception code to register bank
                        LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
                     }
                     // set ADC value to local storage (one request byte is ignored here)
                     S_AdcExt.RegsRO.u16_Data = (ushort)((u8A_AdcDataRx[1] << 8) | u8A_AdcDataRx[2]);
                     // ########## start data signal processing ##########
                     // set temperature sensor state to disconnected if ADC data exceeds limit
                     if (S_AdcExt.RegsRO.u16_Data < (ushort)COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__ADC_EXT_AD7788__MAX_VALUE)
                     {
                        if (sb_TemperatureSensorFailureSet)
                        {
                           // reset boolean variable for temperature sensor failure
                           sb_TemperatureSensorFailureSet = FALSE;
                           // set temperature sensor state
                           S_StatusDataIntern.u16_StateTemperatureSensor   =  (ushort)COLUMN_OVEN_REGISTERS__STATE_TEMPERATURE_SENSOR__CONNECTED;
                           //S_StatusDataExtern.u16_ColumnOvenState          =  (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__INIT;
                        }
//                        // check if column oven params and status data were loaded
//                        if(1)////if ((S_StatusDataIntern.u16_DeviceParamsLoaded) && (S_StatusDataIntern.u16_StatusDataLoaded))
//                        {
//                        }
//                        else
//                        {
//                           //S_Exceptions.u32_ErrorCodeSW_Sub  = ((ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__STATUS_DATA_NOT_LOADED << 16) | (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__DEVICE_PARAMS_NOT_LOADED;
//                        }                        
                        // convert ADC data to fixed point temperature                        
                        if (FAIL_APP_FUNC (ColumnOvenSignalProcessingFuncs_GetTemperature (S_AdcExt.RegsRO.u16_Data, &f_TemperatureMeasured),S_Exceptions.u32_ErrorCodeSW_Sub))
                        {
                           S_Exceptions.u32_ErrorCodeSW_Sub=0;
                           if (FAIL_APP_FUNC (ColumnOvenSignalProcessingFuncs_GetTemperature (S_AdcExt.RegsRO.u16_Data, &f_TemperatureMeasured),S_Exceptions.u32_ErrorCodeSW_Sub))
                           {
                              S_Exceptions.u32_ErrorCodeSW_Sub=0;
                              SUCCESS_APP_FUNC (ColumnOvenSignalProcessingFuncs_GetTemperature (S_AdcExt.RegsRO.u16_Data, &f_TemperatureMeasured),S_Exceptions.u32_ErrorCodeSW_Sub);
                           }
                        }
                        /*
                        // debug >>>>>>>>>>>>>>>>>>>>>>>>>
                        // PTsensor resistance ==> 10 kOhm * ((1 + u16_AdcValue) / (11 - u16_AdcValue))
                        f_ResistanceMeasured = (double)10000.0 * ((double)((ulong)0x00010000 + (ulong)S_AdcExt.RegsRO.u16_Data) / (double)((ulong)0x000B0000 - (ulong)S_AdcExt.RegsRO.u16_Data));
                        // calc now the temprature from resistance measured -- (4*b/a^2) = -0.15122938798352;
                        f_Tmp1 = (((double)4.0 * f_CoefficientB) / (f_CoefficientA * f_CoefficientA));
                        f_Tmp2 = ((f_ResistanceMeasured - f_ResistanceReference) / f_ResistanceReference);
                        f_Tmp3 = (f_CoefficientA / ((double)2.0 * f_CoefficientB));
                        f_TemperatureMeasuredAlt = f_Tmp3 * (sqrt ((double)1.0 + (f_Tmp2 * f_Tmp1)) - (double)1.0);
                        //f_TemperatureMeasuredAlt = (f_CoefficientA / ((float)2.0 * f_CoefficientB)) * sqrtf ((float)1.0 + (((f_ResistanceMeasured - f_ResistanceReference) / f_ResistanceMeasured) * (((float)4.0 * f_CoefficientB) / (f_CoefficientA * f_CoefficientA)))) - (float)1.0;
                        fA_TemperatureMeasuredAltLast[u16_Idx++]   =  f_TemperatureMeasuredAlt;
                        // check for necessary index reset
                        if (u16_Idx > 4)
                        u16_Idx = 0;
                        f_Tmp1 = 0.0;
                        for (u16_Idx2 = 0; u16_Idx2 < 5; u16_Idx2++)
                        f_Tmp1 += fA_TemperatureMeasuredAltLast[u16_Idx2];
                        f_Tmp1 /= 5;
                        f_TemperatureMeasuredAltFiltered =  f_Tmp1;//(0.9 * f_TemperatureMeasuredAltLast) + (f_TemperatureMeasuredAlt * 0.1);
                        //f_TemperatureMeasuredAltLast     =  f_TemperatureMeasuredAlt;
                        f_TemperatureMeasured =  f_Tmp1;
                        // <<<<<<<<<<<<<<<<< debug
                        */
                        // subtract/add user given temperature offset
                        u16_TemperatureOffset = u16_TemperatureOffset;
                        if (u16_TemperatureOffset & 0x8000)
                        {
//                           u16_TemperatureOffset = ((u16_TemperatureOffset ^ 0x7FFF));                // 25.03.2019 Taras
                           u16_TemperatureOffset = ((u16_TemperatureOffset ^ 0x8000)+1);
                           f_TemperatureMeasured -= ((float)(u16_TemperatureOffset/10.0));
                           if(f_TemperatureMeasured < 0.0)
                           {
                              f_TemperatureMeasured -= 2.0 * f_TemperatureMeasured;
                              S_StatusDataExtern.u32_ColumnOvenTemperature  =  ((ulong)(rintf(f_TemperatureMeasured * 10000.0))) | 0x80000000;
                           }
                           else
                              S_StatusDataExtern.u32_ColumnOvenTemperature  =  (ulong)(rintf(f_TemperatureMeasured * 10000.0));         // 25.03.2019 Taras
                        }
                        else
                        {
                           f_TemperatureMeasured += ((float)(u16_TemperatureOffset/10.0));
                           S_StatusDataExtern.u32_ColumnOvenTemperature  =  (ulong)(rintf(f_TemperatureMeasured * 10000.0));
                        }
                        // set current temperature into register bank
                        S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
                        S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
                        S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_TEMPERATURE;
                        S_ModbusAccessParams.usCount      =  (ushort) (1);
                        S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern.u32_ColumnOvenTemperature);
                        // write column oven temperature into register bank
                        if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                        {
                        }
                        // if temperature does not exceed the defined maximum and does not underrun the defined minimum => proceed
                        //if (     (f_TemperatureMeasured < (float)AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS)
                        //&& (f_TemperatureMeasured > (float)AT91SAM7X__MIN_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS))
                        if (  (     (f_TemperatureMeasured < (float)AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL)
                                 && (f_TemperatureMeasured > (float)AT91SAM7X__MIN_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL)
                                 && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                                 && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
                                 )
                              ||
                              (     (f_TemperatureMeasured < (float)AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER)
                                 && (f_TemperatureMeasured > (float)AT91SAM7X__MIN_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER)
                                 && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)
                                 && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                                 )
                              ||
                              (     (f_TemperatureMeasured < (float)AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT)
                                 && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT)
                                 )
                              )
                        {
                           // check for column oven state (other states are ignored and only the current oven temperature is set in this task)
                           if ( (   (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD)
                                    || (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN)
                                    || (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__FINISHED)
                                    )
                                 )
                           {
                              // check now for "gas alarm" oven state
                              if (   (S_StatusDataIntern.u16_StateGasSensor         == (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__CONNECTED)
                                    && (S_StatusDataIntern.u16_GasSensorAlarmDetected == (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_ALARM)
                                    && ((ushort)~COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_MASK_ALARM_DISABLED & u16_GasSensorAlarmEnabled)
                                    )
                              {
                                 // change to "stop" state in next measurement cycle
                                 S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP;
                              }
                              else if (   ((ushort)~COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_MASK_ALARM_DISABLED & u16_GasSensorAlarmEnabled)
                                    && (S_StatusDataIntern.u16_StateGasSensor == (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__DISCONNECTED)
                                    )
                              {
                                 // change to "init" state in next measurement cycle
                                 S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__INIT;
                              }
                              else
                              {
                                 // check for programmers error (sample time is not an integer multiple of min heat/cool program time -> 6 s)
                                 if (cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate == rintf (cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate))
                                 {
                                    // update cycle counter
                                    S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount++;
                                    // go ahead if internal cycle counter is an multiple integer of user time tick
                                    if (     (!(S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount % (ulong)cf_ColumnOvenSignalProcessingTasks_TaskCyclesForTimeUpdate))
                                          || (S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount == 1)
                                          )
                                    {
                                       // update heat/cool program times, column oven state and boolean flags
                                       if (SUCCESS_APP_FUNC (ColumnOvenSupportFuncs_CalcHeatCoolProgramTimes (&S_StatusDataExtern,
                                                   &S_StatusDataIntern, &b_CalcControlParams, &sb_CurrentSectionIsRamp, &su16_Counter,
                                                   &sS_TemperatureRamp, u16_SykamDeviceOptions, u16_SykamDeviceClass, u16_SetPointReachedBand),
                                                S_Exceptions.u32_ErrorCodeSW_Sub))
                                       {
                                          //sprintf ((char *)s8A_UsartDataTx, "\r%3.4f\t%3.1f\ttt=%d\tte=%d\ttr=%d\ttst=%d\ttsr=%d\tccp=%d\tcsisr=%d\t%d\r\r",
                                          //f_TemperatureMeasured, (float)sS_TemperatureRamp.u16_SetPoint/10.0, S_StatusDataExtern.u32_HeatCoolProgramTimeTotal,
                                          //S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed, S_StatusDataExtern.u32_HeatCoolProgramTimeRemain,
                                          //S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal, S_StatusDataExtern.u32_HeatCoolProgramTimeSectionRemain,
                                          //(ulong)b_CalcControlParams, (ulong)sb_CurrentSectionIsRamp, (ulong)S_StatusDataExtern.u16_ColumnOvenState);
                                          //
                                          //UsartTx (u8_Unit, u8_Channel, s8A_UsartDataTx,
                                          //strlen((const char *)s8A_UsartDataTx), OS_T_MS(100));
                                          //u8_Unit = u8_Unit;
                                          // debug end
                                       }
                                    }
                                    // check for errors
                                    if (!S_Exceptions.u32_ErrorCodeSW_Sub)
                                    {
                                       // use the accumulated control deviation to decide wether to set/reset boolean variable "b_DisableHeat"
                                       if (S_StatusDataIntern.u32_ControlDeviationAccumulated == 0)
                                       {
//                                          // calc current control deviation
//                                          f_ControlDeviation = (((float)S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId].u16_SetPoint / 10.0) - ((float)S_StatusDataExtern.u32_ColumnOvenTemperature / 10000.0));
//                                          {
//                                             // check if the heat or cool H-bridge side must be active
//                                             if (f_ControlDeviation < 0.0)
//                                             sb_DisableHeat = TRUE;
//                                             else
//                                             sb_DisableHeat = FALSE;
//                                          }
                                       }
                                       // calc control params
                                       if (SUCCESS_APP_FUNC (ColumnOvenSignalProcessingFuncs_CalcControlParams (&S_StatusDataExtern, &S_StatusDataIntern,
                                                   &b_CalcControlParams, &b_SignalSmoothing, &sS_TemperatureRamp, sb_CurrentSectionIsRamp, u16_SykamDeviceOptions),
                                                S_Exceptions.u32_ErrorCodeSW_Sub))
                                       {
                                       }
                                       // get control output
                                       if (SUCCESS_APP_FUNC (ColumnOvenSignalProcessingFuncs_GetControlOutput (&S_StatusDataExtern, &S_StatusDataIntern, &f_ControlOutput, (ushort)COLUMN_OVEN_CONTROL_PARAMS__CONTROL_TYPE_ADAPTIVE_PID_CONTROL, &sS_TemperatureRamp, b_CalcControlParams, sb_CurrentSectionIsRamp),
                                                S_Exceptions.u32_ErrorCodeSW_Sub))
                                       {
                                       }
                                       // debug "if" -- this is used for different signal smoothing on temperature ramps and none ramps
                                       if (1/*b_SignalSmoothing*/)
                                       {
                                          if ((u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT) != 0)
                                          {
                                             //f_ControlOutput = -1.0 * (float)AT91SAM7X__DACRRCS085_OUTPUT_MAX; // debug#################
                                             // smooth control output (exponential filtering)
                                             f_ControlOutput      = ((1.0 - cf_ColumnOvenSignalProcessingTasks_SmoothingFactorAlpha) * sf_ControlOutputLast) +
                                             (f_ControlOutput * cf_ColumnOvenSignalProcessingTasks_SmoothingFactorAlpha);
                                          }
                                          if ((u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL) != 0)
                                          {
                                             f_ControlOutput   = (0.5 * sf_ControlOutputLast) + (f_ControlOutput * 0.5);
                                          }
                                       }
                                       else  // "else" branch to delete
                                       {
                                          f_ControlOutput   = (0.7 * sf_ControlOutputLast) + (f_ControlOutput * 0.3);
                                       }
                                       // set static variable for next smoothing
                                       sf_ControlOutputLast  = f_ControlOutput;
                                       #ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
                                       // decide on Sykam device option to enable heat/cool (cut off negative values for heat only)
                                       if (   (u16_SykamDeviceClass   == (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
                                             && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL))
                                             && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
                                             && ( (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))           )
                                       {
                                          // limit output (indirect cooling with DAC-Wert zero = heat off)
                                          if (f_ControlOutput < 1.0)
                                          {
                                             f_ControlOutput = 0.0;
                                          }
                                          // enable heat (low active pin)
                                          AT91F_PIO_SetOutput ((AT91PS_PIO)AT91C_BASE_PIOB, (ulong)AT91SAM7X__PIN__COLUMN_OVEN_HEAT_COOL);
                                       }
                                       else if (   (u16_SykamDeviceClass   == (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN)
                                             && (   (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                                                || (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)   )
                                             && (  !(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT))           )
                                       {
                                          #endif
                                          if (f_ControlOutput <= -1.0)
                                          {
                                             // enable cool (low active pin)
                                             AT91F_PIO_ClearOutput ((AT91PS_PIO)AT91C_BASE_PIOB, (ulong)AT91SAM7X__PIN__COLUMN_OVEN_HEAT_COOL);
                                          }
                                          else if (f_ControlOutput >= 1.0)
                                          {
                                             // enable heat (low active pin)
                                             AT91F_PIO_SetOutput ((AT91PS_PIO)AT91C_BASE_PIOB, (ulong)AT91SAM7X__PIN__COLUMN_OVEN_HEAT_COOL);
                                          }
                                          else
                                          {
                                             // DAC-Wert zero 1) if control output is smaller than |1.0| or 2) if cooling/heat exceeds set point and
                                             // control output switches to positive/negative values. 2) Is useful to prevent peltier element to be
                                             // operated with changing current.
                                             f_ControlOutput = 0.0;
                                          }
                                          #ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
                                       }
                                       // Sykam device option unknown
                                       else
                                       S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__DEVICE_OPTION_IS_NOT_ACTIVATED;
                                       #endif
                                       // check for errors
                                       if ((!S_Exceptions.u32_ErrorCodeSW_Sub) && (!S_Exceptions.u16_ErrorCodeSW_Base))
                                       {
                                          // calc absolute control output value for DAC
                                          // f_ControlOutput = fabsf (f_ControlOutput);
                                          // set appropriate DAC value which is delivered by control algorithm
                                          if (SUCCESS_APP_FUNC (ColumnOvenSupportFuncs_SetDacOutput (f_ControlOutput),
                                                   S_Exceptions.u32_ErrorCodeSW_Sub))
                                          {
                                             // disable power circuit totally if DAC value is smaller than one ("1")
                                             if (fabsf(f_ControlOutput) < 1.0)
                                             AT91F_PIO_SetOutput ((AT91PS_PIO)AT91C_BASE_PIOB, (ulong)AT91SAM7X__PIN__COLUMN_OVEN_POWER);
                                          }
                                          // +++ set variables of status data register bank (heat/cool program state and control output value) to keep up to date +++
                                          if (((float)S_StatusDataIntern.u32_ControlDeviationLast / 10000.0) <= ((float)(u16_SetPointReachedBand) / 10.0))
                                          {
                                             su16_Counter++;
                                             // the following lines are only executed if the minimum amount of samples in sequence lie inside "set point reached band"
                                             if (su16_Counter >= (ushort)COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__SET_POINT_REACHED_BAND_SAMPLE_COUNTER_MIN)
                                             {
                                                S_StatusDataIntern.u16_StateHeatCoolProgram  =  (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__SET_POINT_REACHED;
                                                // reset counter to ensure state (COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__SET_POINT_REACHED_BAND_SAMPLE_COUNTER_MIN samples
                                                // in sequence indicate "set point reached state")
                                                su16_Counter                                 =  _umax((ushort)COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__SET_POINT_REACHED_BAND_SAMPLE_COUNTER_MIN, 1) - 1;
                                             }
                                          }
                                          // reset counter for samples which lie outside "set point reached band" only for preheat/precool here
                                          else
                                          su16_Counter = 0;
                                          // set internal coulumn oven state which is not set in other functions
                                          if (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD)
                                          S_StatusDataIntern.u16_StateHeatCoolProgram  =  (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__HOLD;
                                          if (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__FINISHED)
                                          S_StatusDataIntern.u16_StateHeatCoolProgram  =  (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__FINSIHED;
                                          S_StatusDataIntern.u16_ControlDacOuputVoltage = (ushort)(CONVERT_10BIT_DAC_VALUE_INTO_VOLTAGE((ushort)rintf(f_ControlOutput)));
                                       }
                                    }
                                 }
                                 // sample time is not an integer multiple of heat/cool program time
                                 else
                                 {
                                    S_Exceptions.u32_ErrorCodeSW_Sub =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_INTERNAL_ERROR;
                                 }
                              }
                           }
                           // if column oven was in "over/under temperature" state and temperature is ok now - we set state to "stop"
                           else if (   (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__OVER_TEMPERATURE)
                                 || (S_StatusDataExtern.u16_ColumnOvenState == (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__UNDER_TEMPERATURE)
                                 )
                           {
                              // restore last state for return to normal operation mode
                              S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP;
                           }
                           // column oven state is now "none", "init", "stop" or "gas alarm" => power down column oven
                           else
                           {
                              // check for enabled gas alarm
                              if ((ushort)~COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_MASK_ALARM_DISABLED & u16_GasSensorAlarmEnabled)
                              {
                                 // check for connected sensor (warm up finished) and NO gas alarm
                                 if (     (S_StatusDataIntern.u16_StateGasSensor          == (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__CONNECTED)
                                       && (S_StatusDataIntern.u16_GasSensorAlarmDetected  == (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_NO_ALARM)
                                       )
                                 {
                                    // change from "init" to "stop" state (user can work with the device now)
                                    S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP;
                                 }
                                 // check for connected sensor and a detected GAS ALARM
                                 else if (   (S_StatusDataIntern.u16_StateGasSensor          == (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__CONNECTED)
                                       && (S_StatusDataIntern.u16_GasSensorAlarmDetected  == (ushort)COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_ALARM)
                                       )
                                 {
                                    // change from "init/stop" state to "gas alarm" state (device not blocked but read display screen is active)
                                    S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__GAS_ALARM;
                                 }
                                 // sensor is in warm up
                                 else if (S_StatusDataIntern.u16_StateGasSensor == (ushort)COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__WARM_UP)
                                 {
                                    // change to "init" state were sensor can warm up (again)
                                    S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__INIT;
                                 }
                                 // sensor is disconnected
                                 else
                                 {
                                    // change to "stop" state - disconnected sensor due to broken sensor
                                    S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP;
                                 }
                              }
                              else
                              {
                                 // change from "init/gas alarm" to "stop" state (user can work with the device now)
                                 S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP;
                              }
                              // disable power with power-down DAC (soft switch off)
                              TRY_AND_CATCH_APP_FUNC
                              (
                              ColumnOvenSupportFuncs_SetDacOutput (0),
                              S_Exceptions.u32_ErrorCodeSW_Sub
                              );
                              S_StatusDataIntern.u16_ControlDacOuputVoltage                  =  0;
                              // set heat/cool program times (do not reset total time)
                              S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed              =  0;
                              S_StatusDataExtern.u32_HeatCoolProgramTimeRemain               =  0;
                              S_StatusDataExtern.u32_HeatCoolProgramTimeSectionRemain        =  0;
                              S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal         =  0;
                              // reset control deviation (not compulsory)
                              S_StatusDataIntern.u32_ControlDeviationAccumulated             =  0;
                              S_StatusDataIntern.u32_ControlDeviationLast                    =  0;
                              // reset internal cycle counter (not compulsory)
                              S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount            =  0;
                              // set heat/cool program section identifier (zero equals hold state) and number of sections
                              S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId         =  0;
                              S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
                              // disable power with tiny gate (hard switch off even if errors are present)
                              AT91F_PIO_SetOutput ((AT91PS_PIO)AT91C_BASE_PIOB, (ulong)AT91SAM7X__PIN__COLUMN_OVEN_POWER);
                           }
                           //sprintf ((char *)s8A_UsartDataTx, "%3.4f Ki=%d Kd=%d Kp=%d ti=%d td=%d dac_out=%.4d\rdevt=%d devl=%d pid_out=%.4d curr_setp=%d hc_state=%d\r",
                           //f_TemperatureMeasured, S_StatusDataIntern.u32_ControlIntegrationGain,
                           //S_StatusDataIntern.u32_ControlDifferentialGain, S_StatusDataIntern.u32_ControlProportionalGain,
                           //S_StatusDataIntern.u32_ControlIntegrationTime, S_StatusDataIntern.u32_ControlDifferentialTime, (ulong)S_StatusDataIntern.u16_ControlDacOuputVoltage,
                           //S_StatusDataIntern.u32_ControlDeviationAccumulated, S_StatusDataIntern.u32_ControlDeviationLast,
                           //(ulong)rintf(f_ControlOutput), (ulong)sS_TemperatureRamp.u16_SetPoint, (ulong)S_StatusDataExtern.u16_ColumnOvenState/*(ulong)S_StatusDataIntern.u16_StateHeatCoolProgram*/);
                           //
                           //UsartTx (u8_Unit, u8_Channel, s8A_UsartDataTx,
                           //strlen((const char *)s8A_UsartDataTx), OS_T_MS(100));
                           //sprintf ((char *)s8A_UsartDataTx, "%.7d \t%3.4f \t%d\r", (S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount * (ulong)AT91SAM7X__REPETITION_TICKS_FOR_TSK_GLOBAL_TIMER),
                           //f_TemperatureMeasured, (ushort)f_ControlOutput);
                           //sprintf ((char *)s8A_UsartDataTx, "%3.4f\r", f_TemperatureMeasured);
                           //
                           //UsartTx (u8_Unit, u8_Channel, s8A_UsartDataTx,
                           //strlen((const char *)s8A_UsartDataTx), OS_T_MS(100));
                           //sprintf ((char *)s8A_UsartDataTx, "%3.4f \t%3.1f\t%.2f\t%d\r", f_TemperatureMeasured, (float)sS_TemperatureRamp.u16_SetPoint/10.0, sf_ControlOutputLast, (ulong)S_StatusDataExtern.u16_ColumnOvenState);
                           //
                           //UsartTx (u8_Unit, u8_Channel, s8A_UsartDataTx, strlen((const char *)s8A_UsartDataTx), OS_T_MS(100));
                           //sprintf ((char *)s8A_UsartDataTx, "\r%3.4f\t%3.1f\ttt=%d\tte=%d\ttr=%d\ttst=%d\ttsr=%d\tccp=%d\tcsisr=%d\tstate=%d\r\r",
                           //f_TemperatureMeasured, (float)sS_TemperatureRamp.u16_SetPoint/10.0, S_StatusDataExtern.u32_HeatCoolProgramTimeTotal,
                           //S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed, S_StatusDataExtern.u32_HeatCoolProgramTimeRemain,
                           //S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal, S_StatusDataExtern.u32_HeatCoolProgramTimeSectionRemain,
                           //(ulong)b_CalcControlParams, (ulong)sb_CurrentSectionIsRamp, (ulong)S_StatusDataExtern.u16_ColumnOvenState);
                           //
                           //UsartTx (u8_Unit, u8_Channel, s8A_UsartDataTx,
                           //strlen((const char *)s8A_UsartDataTx), OS_T_MS(100));
                        }
                        // out of operating temperature range!
                        else
                        {
                           // disable power with power-down DAC (soft switch off) -
                           TRY_AND_CATCH_APP_FUNC
                           (
                           ColumnOvenSupportFuncs_SetDacOutput (0),
                           S_Exceptions.u32_ErrorCodeSW_Sub
                           );
                           //if (f_TemperatureMeasured > (float)AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS)
                           if (
                                 (     (f_TemperatureMeasured > (float)AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL)
                                    &&    (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                                    &&    (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
                                    )
                                 ||
                                 (     (f_TemperatureMeasured > (float)AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER)
                                    &&    (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)
                                    &&    (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                                    )
                                 ||
                                 (     (f_TemperatureMeasured > (float)AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT)
                                    &&    (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT)
                                    )
                                 )
                           {
                              S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__OVER_TEMPERATURE;
                           }
                           else if
                           (
                           (  (f_TemperatureMeasured < (float)AT91SAM7X__MIN_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL)
                           && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                           && (!(u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER))
                           )
                           ||
                           (  (f_TemperatureMeasured < (float)AT91SAM7X__MIN_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER)
                           && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER)
                           && (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL)
                           )
                           )
                           {
                              S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__UNDER_TEMPERATURE;
                           }
                        }
                     }
                     // temperature sensor not connected
                     else
                     {
                        if (!sb_TemperatureSensorFailureSet)
                        {
                           sb_TemperatureSensorFailureSet = TRUE;
                           // disable power with power-down DAC (soft switch off)
                           TRY_AND_CATCH_APP_FUNC
                           (
                           ColumnOvenSupportFuncs_SetDacOutput (0),
                           S_Exceptions.u32_ErrorCodeSW_Sub
                           );
                           S_StatusDataIntern.u16_ControlDacOuputVoltage                  =  0;
                           // set heat/cool program times (do not reset total time)
                           S_StatusDataExtern.u32_HeatCoolProgramTimeElapsed              =  0;
                           S_StatusDataExtern.u32_HeatCoolProgramTimeRemain               =  0;
                           S_StatusDataExtern.u32_HeatCoolProgramTimeSectionRemain        =  0;
                           S_StatusDataExtern.u32_HeatCoolProgramTimeSectionTotal         =  0;
                           // reset control deviation (not compulsory)
                           S_StatusDataIntern.u32_ControlDeviationAccumulated             =  0;
                           S_StatusDataIntern.u32_ControlDeviationLast                    =  0;
                           // reset internal cycle counter (not compulsory)
                           S_StatusDataIntern.u32_HeatCoolProgramIntCycleCount            =  0;
                           // set heat/cool program section identifier (zero equals hold state) and number of sections
                           S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionId         =  0;
                           S_StatusDataExtern.u16_HeatCoolProgramCurrentSectionSetPoint   =  S_StatusDataExtern.S_HeatCoolProgramInUse.SA_Section[0].u16_SetPoint;
                           // disable power with tiny gate (hard switch off even if errors are present)
                           AT91F_PIO_SetOutput ((AT91PS_PIO)AT91C_BASE_PIOB, (ulong)AT91SAM7X__PIN__COLUMN_OVEN_POWER);
                           // set oven "init" state (user can work with the device now)
                           S_StatusDataExtern.u16_ColumnOvenState = (ushort)COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__INIT;
                           // set other states
                           S_StatusDataIntern.u16_StateTemperatureSensor                  =  (ushort)COLUMN_OVEN_REGISTERS__STATE_TEMPERATURE_SENSOR__DISCONNECTED;
                           S_StatusDataIntern.u16_StateHeatCoolProgram                    =  (ushort)COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__STOP;
                           S_Exceptions.u32_ErrorCodeSW_Sub                               =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_TEMPERATURE_SENSOR_FAILURE;
                        }
                     }
                  }
                  else
                  S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_EXTERNAL_ADC_FAILURE;
               }
               else
               S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_EXTERNAL_ADC_FAILURE; // set a (special) error code or warning ERROR_SUB__ADC_EXT_AD7788_INCORRECT_STATUS_DATA;
            }
            else
            S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_EXTERNAL_ADC_FAILURE; // no ADC response
         }
         else
         S_Exceptions.u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_EXTERNAL_ADC_FAILURE; // "no ADC response"
         // set error codes to register bank
         if ((S_Exceptions.u32_ErrorCodeSW_Sub) || (S_Exceptions.u16_ErrorCodeSW_Base))
         {
            if (S_Exceptions.u32_ErrorCodeSW_Sub == (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_TEMPERATURE_SENSOR_FAILURE)
            {
               // disable power with power-down DAC (soft switch off)
               if (FAIL_APP_FUNC (ColumnOvenSupportFuncs_SetDacOutput (0), S_Exceptions.u32_ErrorCodeSW_Sub))
               {
                  // set exception code to register bank
                  LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
               }
               // disable power with tiny gate (hard switch off even if errors are present)
               AT91F_PIO_SetOutput ((AT91PS_PIO)AT91C_BASE_PIOB, (ulong)AT91SAM7X__PIN__COLUMN_OVEN_POWER);
            }
            // set exception codes to register bank
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
         }
         if (b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified)
         {
            // refresh local EXtern status data
            S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
            S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
            S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;  // register start address
            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);                                      // total words of S_StatusDataExtern
            S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
            if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
            }
            // refresh local INtern status data
            S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
            S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
            if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               b_ColumnOvenSignalProcessingTasks_FlagStatusDataModified = FALSE;
            }
         }
         // write EXtern status data to register bank
         S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
         S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
         S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);
         S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
         }
         // write INtern status data to register bank
         S_ModbusAccessParams.usAddr       =  (ushort) COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
         S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
         }
      }  // end of wait for periodically wake up call
      else
      {
         S_Exceptions.u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__TEMPERATURE_CONTROL;
         S_Exceptions.u32_ErrorCodeSW_Sub    =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_TRIGGER_FROM_TIMER_TASK_FAILED;
         // set exception codes to register bank
         LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
      }
      RESCHEDULE(50);//NEW
   }
}
//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------
