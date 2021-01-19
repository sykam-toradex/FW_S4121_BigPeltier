/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \mainpage            Project:                   Column Oven - Temperature and Valve Control
*
*  \par                 Description:
*
*                       From an external temperature sensor we got samples which are converted with the help
*                       of an external ADC. The actual temperature is compared to an reference temperature.
*                       The PID control is able to calculate optimum parameters which are translated to an
*                       adequate pwm signal which one task applies to an column oven. Furthermore the signal
*                       of a gas sensor is evaluated. In case of a broken column an expanding liquid (gas)
*                       is detected. Consequently the heating is stopped immediately.
*
*
*                       Total project code delivers several drivers for peripherals, two drivers for the step motor(s) and the application.
*                       The purpose of this application is to control the valve of device S4121 with the help of an electric step motor. \n
*                       Initializing of complete system requires approximately 1.39 s (362 ms) (externally measured). In this time all drivers will
*                       be set up and calibration data as well as device/Sykam parameters are loaded. Calibration data means static data
*                       from module "encgapdisk" or in other words calibration data comprises the amount of detected edges and distances in
*                       microsteps between them.\n
*                       If load of calibration data or device params fails during initialization, calibration routine is executed with default
*                       device parameters. In case of success calibration data and device parameters are saved. In case of procedure failure
*                       (calibration -> calibration data and parameter storage) calibration routine is executed again after every device restart
*                       as long as not successful.\n
*                       For any error occurred during system initialization or at run-time an error code is set to register bank.\n
*                       Furthermore certain \em commands exist which are buffered by means of drivers. An detailed description can be found in
*                       excel table <em>2011_xx_xx__S4121_register_bank_description.pdf</em>. Any command execution is blocked during initialization.
*                       At run-time blocking occurs as long as last command was not finished. However, commands are buffered by means of drivers and
*                       executed successively.\n
*                       The valve control is actual possible via interfaces named in \ref _E_ActiveInterfaceList. Only one interface can be active
*                       at run time.
*
*  \see                 2011_02_XX__S4121_register_bank_description.pdf
*
*
*  Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*
*//*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_main.c
*
*  \brief               The main file initializes several driver modules, namely the \em USART, \em PWM, \em SPI, \em StpClkDir (step motor),
*                       \em EncGapDisk (encoder gap disk) and the externals \em DAC, \em MCP23S17 (IO-expander) as well as the \em Fm25CLSS (FRAM).
*                       Commands from any device interface are blocked during initialization. The active device interface is set to "none" at beginning
*                       of \ref TSK_Init, i.e. it must be selected by user via RS232 or display keys if device parameters could not be load during
*                       initialization or if the device is new and used for the first time.\n
*                       Be careful if setting the timeout constants \ref cu16_TimeoutShort or \ref cu16_TimeoutLong; OS_TICK from \ref RTX_config.c
*                       might have changed! \n
*                       Every function returns an base and sub error code which is greater than zero in error case.
*
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*
*  \date                2010-05-27  	Initial version imported from evaluation.c of hschoettner. \n
*                       2011-02-01  	Imported S6510_main.c to continue project S4121 \n
*                       2011-02-07     Renamed task, mailbox and error code from "S6X1Xi_OutputOnPinContactStrip" to "TSK_ValveAct_GenerateInjectSignal". \n
*                                      Changed error handling in \ref TSK_Init and \ref LoadCalibrationDataAndParams.
*                       2011-05-17     Updated to MDK-ARM 4.13a and updated MCP2317 driver
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                -->> correct date>> 2010-11-03  	Tested whole initialization and corresponding errors cases. \n
*
*  \todo
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// Includes
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.h>
#include <RTL.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\cpu\at91\adcle\adcle.h"
#include "..\..\..\cpu\at91\pio\piodata.h"
#include "..\..\..\cpu\at91\pio\pioext.h"
#include "..\..\..\cpu\at91\pwm\pwm.h"
#include "..\..\..\cpu\at91\pwm\pwmext.h"
#include "..\..\..\cpu\at91\rtt\rttext.h"
#include "..\..\..\cpu\at91\spi\spi.h"
#include "..\..\..\cpu\at91\spi\spiext.h"
#include "..\..\..\cpu\at91\usart\usart.h"
#include "..\..\..\cpu\at91\usart\usartext.h"
#include "..\..\..\cpu\at91\adc\adc.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"
#include "..\..\..\cpu\at91\misc\stpclkdir\stpclkdir.h"
#include "..\..\..\cpu\at91\misc\stpclkdir\stpclkdirext.h"
#include "..\..\..\ext\misc\encgapdisk\encgapdiskext.h"
#include "..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085.h"
#include "..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085ext.h"
#include "..\..\..\ext\spi\nvram\fm25clss\fm25clss.h"
#include "..\..\..\ext\spi\nvram\fm25clss\fm25clssext.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
//#include "..\..\..\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\lib_exceptions_funcs.h"
#include "..\..\shared\src\common_cmds_owner.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\common_registers_funcs.h"
#include "..\..\shared\src\common_modbus_funcs.h"
#include "..\..\shared\src\common_support_funcs.h"
#include "..\..\shared\src\utils_funcs.h"
#include "..\src\S6510\valve_act_modbus_funcs.h"
#include "..\src\S6510\valve_act_registers_funcs.h"
#include "..\src\S6510\valve_act_support_funcs.h"
#include "..\src\S6510\valve_act_cmds_owner.h"
#include "..\src\S6510\valve_act_cmds_reg.h"
#include "..\src\S6510\valve_act_valvetypes.h"
#include "..\src\S6510\valve_act_registers.h"
#include "column_oven_registers.h"
#include "column_oven_modbus_funcs.h"
#include "column_oven_registers_funcs.h"
#include "column_oven_cmds_owner.h"
#include "column_oven_support_tasks.h"
#include "column_oven_signal_processing_tasks.h"
#include "column_oven_signal_processing_funcs.h"
#include "column_oven_support_funcs.h"
//---------------------------------------------- global constants ------------------------------------------------------------------------------
const ushort cu16_TimeoutVeryShort  = AT91SAM7X__TICK_1;             // 1 system tick
const ushort cu16_TimeoutShort      = AT91SAM7X__TICK_10;            // 10 ticks
const ushort cu16_TimeoutMedium     = AT91SAM7X__TICK_20;            // 20 ticks
const ushort cu16_TimeoutLong       = AT91SAM7X__TICK_30;            // 30 ticks

const ushort cu16_ColumnOvenMain_TimeoutVeryShort  = AT91SAM7X__TICK_1;             // 1 system tick
const ushort cu16_ColumnOvenMain_TimeoutShort      = AT91SAM7X__TICK_10;            // 10 ticks
const ushort cu16_ColumnOvenMain_TimeoutMedium     = AT91SAM7X__TICK_20;            // 20 ticks
const ushort cu16_ColumnOvenMain_TimeoutLong       = AT91SAM7X__TICK_30;            // 30 ticks

const ushort cu16_ValveActMain_TimeoutVeryShort    = AT91SAM7X__TICK_1;             // 1 system tick
const ushort cu16_ValveActMain_TimeoutShort        = AT91SAM7X__TICK_10;            // 10 ticks
const ushort cu16_ValveActMain_TimeoutMedium       = AT91SAM7X__TICK_20;            // 20 ticks
const ushort cu16_ValveActMain_TimeoutLong         = AT91SAM7X__TICK_30;            // 30 ticks


//---------------------------------------------- global declarations/definitions ---------------------------------------------------------------
bool  b_ColumnOvenMain_InitDone  =  FALSE;
// tasks
OS_TID   TID_UtilsTasks_Heartbeat;
// customized stacks
OS_TSK_STACK_USER(STK_TSK_ColumnOvenMain_Init, 1000);                              /**< define 3000 byte stack for \ref TSK_Init */
// semaphores
OS_SEM   SEM_ColumnOvenMain_SpiNewMsg_Unit[SPI_UNIT_MAX];
OS_SEM   SEM_ColumnOvenMain_SpiNewMsg_Channel[SPI_CHANNEL_MAX];

//OS_TID   TID_CheckCommunication;
//OS_TSK_STACK_USER(STK_TSK_CheckCommunication, 1000);  

//---------------------------------------------- externals -------------------------------------------------------------------------------------
extern            bool     b_ColumnOvenMain_InitDone;
extern            OS_SEM   SEM_CommonSupportFuncs_DeviceRunTimeCounter;
extern            OS_SEM   SEM_ColumnOvenMain_SpiNewMsg_Channel[SPI_CHANNEL_MAX];
extern            struct   _SMcp2317Item                       SMcp2317List[MCP2317_LIST_SIZE];
extern            struct   _SPioItem                           SPioListA[AT91F_PIOLIST_SIZEA];
extern            struct   _SPioItem                           SPioListB[AT91F_PIOLIST_SIZEB];
extern            struct   _SPwmItem                           SPwmList[AT91F_PWMLIST_SIZE];
extern   const    ushort   cu16_TimeoutVeryShort;
extern   const    ushort   cu16_TimeoutShort;
extern   const    ushort   cu16_TimeoutMedium;
extern   const    ushort   cu16_TimeoutLong;
extern   const    ushort   cu16_TimeoutVeryShort;
extern   const    ushort   cu16_TimeoutShort;
extern   const    ushort   cu16_TimeoutMedium;
extern   const    ushort   cu16_TimeoutLong;
extern   const    ushort   cu16_CommonRegistersFuncs_DefaultActiveInterface;
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
__task   void     TSK_ColumnOvenMainInit  (void);
__task void TSK_CheckCommunication (void);
int      main                    (void);
void  MBPBBWriteHolding(unsigned short usDeviceAddress, unsigned short usAddress, unsigned short usValue);
//---------------------------------------------- functions -------------------------------------------------------------------------------------
__task void TSK_ColumnOvenMain_Init (void)
{
   uchar    u8_Unit                    =  0;
   uchar    u8_ChannelsToEnable;
//   ushort   u16_DeviceState;
   ushort   u16_ModbusSlaveID;
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   ushort   u16_DeviceActiveInterface  =  cu16_CommonRegistersFuncs_DefaultActiveInterface;   // default needed if load from permanent memory fails
   ushort   u16_SykamDeviceOptions;
   ushort   u16_RTT_Prescaler          =  0x8000;                        /**<  RTT increment every x [s] with x = prescaler / 32768 Hz (slow crystal) */
   ulong    u32_RTT_PeriodForMsgGen    =  0x0000000A;                    /**<  calling corresponding user task periodically every u32_RTT_PeriodForMsgGen * [prescaler / 32768 Hz] */
   ulong    u32_RTT_AlarmValue         =  0x00000000;
   ulong    u32_DeviceRunTimeCounter   =  0;
#endif
   ulong    u32_Idx;
   ulong    u32_SPI_MsgMask            =  0xFFFFFFFF;
   ushort   u16_CurrentValveType       =  cu16_ValveActValveTypes_DefaultValveType;
   tS_LibExceptions_Exceptions S_Exceptions;
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   // configure PIO A
   if (FAIL (PioInit(u8_Unit, SPioListA, AT91F_PIOLIST_SIZEA), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__PIO__INIT))
   {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
   
   }  // initialize PIO A
   // configure PIO B
   u8_Unit = 1;
   if (FAIL (PioInit(u8_Unit, SPioListB, AT91F_PIOLIST_SIZEB), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__PIO__INIT))
   {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
   
   }  // initialize PIO B
   // configure PWM (dispatcher for shared irq of all channels)
   u8_Unit = 0;
   if (FAIL (PwmInitUnit (u8_Unit, SPwmList, AT91F_PWMLIST_SIZE), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__PWM__INIT_UNIT))
   {
   }  // initialize PWM
   // initialize message semaphores for spi
   for (u32_Idx = 0; u32_Idx < SPI_UNIT_MAX; u32_Idx++)
   {
      os_sem_init (SEM_ColumnOvenMain_SpiNewMsg_Unit[u32_Idx], 0);
   }
   // initialize channel semaphores for spi
   for (u32_Idx = 0; u32_Idx < SPI_CHANNEL_MAX; u32_Idx++)
   {
      os_sem_init (SEM_ColumnOvenMain_SpiNewMsg_Channel[u32_Idx], 0);
   }
   // init spi unit zero (SPI0)
   if (FAIL (SpiInitUnit (u8_Unit, (uchar)AT91SAM7X__DUMMY_ARG, (ulong)(AT91C_SPI_PCS | AT91C_SPI_MODFDIS |
                  AT91C_SPI_PCSDEC | AT91C_SPI_MSTR), (OS_ID)(&SEM_ColumnOvenMain_SpiNewMsg_Unit[0]), u32_SPI_MsgMask),
            S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_UNIT))
   {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
   
   }  // initialize SPI0
   //    unit, channel, clock and options, max bytes per cs-cycle, max delay, job priority,
   //    spi driver flags, initialized sem for new msg triggering, msg mask for counters
   if (FAIL (SpiInitChannel (u8_Unit, (uchar)FM25CLSS_SPI_CHANNEL, (ulong)(SPI_CSR_SCBR(AT91B_MCK, FM25CLSS_SPI_BAUDRATE) |
                  AT91C_SPI_BITS_8 | AT91C_SPI_NCPHA), (ushort)FM25CLSS_BLOCKSIZEMAX, (ushort)OS_T_FOREVER,
               (uchar)SPI_P_NORMAL, (uchar)SPI_CHANNEL_FLAGS_CSRUPDATE,
               (OS_ID)(&SEM_ColumnOvenMain_SpiNewMsg_Channel[FM25CLSS_SPI_CHANNEL]), u32_SPI_MsgMask),
            S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_CHANNEL))
   {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
   
   }  // initialize SPI channel
   if (FAIL (SpiInitChannel (u8_Unit, (uchar)AT91SAM7X__DACRRCS085_SPI_CHANNEL, (ulong)(SPI_CSR_SCBR(AT91B_MCK,
                     AT91SAM7X__DACRRCS085_SPI_BAUDRATE) | AT91C_SPI_BITS_8| AT91C_SPI_CPOL | AT91C_SPI_NCPHA),
               (ushort)DACRRCS085_BLOCKSIZEMAX, (ushort)OS_T_FOREVER, (uchar)SPI_P_NORMAL,
               (uchar)SPI_CHANNEL_FLAGS_CSRUPDATE, (OS_ID)(&SEM_ColumnOvenMain_SpiNewMsg_Channel[AT91SAM7X__DACRRCS085_SPI_CHANNEL]),
               u32_SPI_MsgMask),
            S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_CHANNEL))
   {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
   
   }  // initialize SPI channel
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   if (FAIL (SpiInitChannel (u8_Unit, (uchar)MCP2317_SPI_CHANNEL, (ulong)(SPI_CSR_SCBR(AT91B_MCK, MCP2317_SPI_BAUDRATE ) |
                  AT91C_SPI_BITS_8 | AT91C_SPI_NCPHA), (ushort)MCP2317_BLOCKSIZEMAX,
               (ushort)OS_T_FOREVER, (uchar)SPI_P_NORMAL, (uchar) SPI_CHANNEL_FLAGS_CSRUPDATE,
               (OS_ID)(&SEM_ColumnOvenMain_SpiNewMsg_Channel[MCP2317_SPI_CHANNEL]), u32_SPI_MsgMask),
            S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_CHANNEL))
#endif
   {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
   
   }  // initialize SPI channel
   if (FAIL (SpiInitChannel (u8_Unit, (uchar)AT91SAM7X__ADC_EXT_AD7788__SPI_CHANNEL, (ulong)(SPI_CSR_SCBR(AT91B_MCK,
                     AT91SAM7X__ADC_EXT_AD7788__SPI_BAUDRATE )| AT91C_SPI_BITS_8 | AT91C_SPI_CPOL),
               (ushort)AT91SAM7X__ADC_EXT_AD7788__BYTES_PER_TRANSFER, (ushort)OS_T_FOREVER,
               (uchar)SPI_P_NORMAL, (uchar) SPI_CHANNEL_FLAGS_CSRUPDATE,
               (OS_ID)(&SEM_ColumnOvenMain_SpiNewMsg_Channel[AT91SAM7X__ADC_EXT_AD7788__SPI_CHANNEL]), u32_SPI_MsgMask),
            S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_CHANNEL))
   {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
   
   }  // initialize SPI channel
   // init spi slave unit one (SPI1)
   u8_Unit = 1;
   TRY_AND_CATCH
   (
   SpiInitUnit (u8_Unit, (uchar)AT91SAM7X__DUMMY_ARG, (AT91C_SPI_MODFDIS),
   (OS_ID)(&SEM_ColumnOvenMain_SpiNewMsg_Unit[1]), u32_SPI_MsgMask),
   S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_UNIT
   );
   u8_Unit = 0;
   if (S_Exceptions.u32_ErrorCodeSW_Sub == LIB_EXCEPTIONS__ERROR_NONE)
   {
      // configure DAC104S085 (four channels)
      if (FAIL (Dacrrcs085Init (u8_Unit, (uchar)AT91SAM7X__DUMMY_ARG, &SEM_ColumnOvenMain_SpiNewMsg_Channel[AT91SAM7X__DACRRCS085_SPI_CHANNEL]),
               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__DACRRCS085__INIT_UNIT))
      {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank               
      }  // initialize DAC      
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
         // initialize endoder gap disk
         if (FAIL (EncGapDiskInit(u8_Unit), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__INIT_UNIT))
#endif
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank                  
         }  // initialize encoder gap disk
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
         // configure port expander MCP2317
         if (FAIL (Mcp2317Init (u8_Unit, SMcp2317List, MCP2317_LIST_SIZE, &SEM_ColumnOvenMain_SpiNewMsg_Channel[MCP2317_SPI_CHANNEL]),
                  S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MCP2317__INIT_UNIT))
#endif
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank                  
         }  // initialize IO expander
         // wait for finishing of started tasks
         os_dly_wait (AT91SAM7X__TICK_1);
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
         // initialize step motor
         if ( FAIL (StpClkDirInit (u8_Unit, (ushort)STPCLKDIR0_DAC_HOLD, (ushort)STPCLKDIR0_DAC_RUN),
                  S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__INIT_UNIT))
#endif
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
         
         }  // initialize step motor
         // initialize FRAM FM25CL64
         if (FAIL (Fm25clssInit (u8_Unit, &SEM_ColumnOvenMain_SpiNewMsg_Channel[FM25CLSS_SPI_CHANNEL]),
                  S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__INIT))
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
         
         }
         // set user ADC channels to enable
         u8_ChannelsToEnable = ((uchar)AT91SAM7X__ADC_CHANNEL_24000_MILLIVOLT | (uchar)AT91SAM7X__ADC_CHANNEL_5000_MILLIVOLT | (uchar)AT91SAM7X__ADC_CHANNEL_GAS_SENSING | (uchar)AT91SAM7X__ADC_CHANNEL_GAS_HEAT_SENSE);
         // initialize ADC
         if (FAIL (AdcInit (u8_ChannelsToEnable), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ADC__INIT))
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
         }

   }  // check for errors
   // check for errors
   if ((S_Exceptions.u32_ErrorCodeSW_Sub) && (!S_Exceptions.u16_ErrorCodeSW_Base))
   {
      S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
      LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
   }
   // ### initialize application dependent driver modules
   if (SUCCESS_APP_FUNC (ColumnOvenModbusFuncs_InitDriver(), S_Exceptions.u32_ErrorCodeSW_Sub))
   {
      os_dly_wait (AT91SAM7X__TICK_1);
      // set the product state to "on" (modbus is always active after initialization -
      // this is just a state flag to be read out with modbus cmd 0x11)
      if (FAIL (MBPBProductStateSet (TRUE, cu16_TimeoutLong),
               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_STATE_SET))
      {
      }
   }
   // ### initialization of common parts ###
   if (S_Exceptions.u32_ErrorCodeSW_Sub == LIB_EXCEPTIONS__ERROR_NONE)
   {
      // initialize common register bank
      if (FAIL_APP_FUNC (CommonRegistersFuncs_InitRegisterBank(), S_Exceptions.u32_ErrorCodeSW_Sub))
      {
      }  // initialize common register bank
      // initialize common modbus tasks
      if (FAIL_APP_FUNC (CommonModbusFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
      {
      }  // initialize common modbus tasks
      // initialize common support tasks
      if (FAIL_APP_FUNC (CommonSupportFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
      {
      }  // initialize common support tasks
      // wait for finishing of started tasks
      os_dly_wait (AT91SAM7X__TICK_2);
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
      // load Sykam device data -- go ahead if load data fails
      if (FAIL_APP_FUNC (CommonCmdsOwner_SykamDeviceDataLoad ((ushort)COMMON_CMDS_OWNER__CMD_ARG__SYKAM_DEVICE_DATA_LOAD,
                  &S_Exceptions.u32_ErrorCodeSW_Sub),
               S_Exceptions.u16_ErrorCodeSW_Base))
      {
         LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
      }
#endif
//      // set device state
//      u16_DeviceState = (uchar)COMMON_REGISTERS__DEVICE_STATE__IDLE;
//      if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//               S_Exceptions.u32_ErrorCodeSW_Sub))
//      {
//      }  // set device state
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
      // read current active interface from permanent memory
      if (FAIL_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)&u16_DeviceActiveInterface, (ushort)AT91SAM7X__NUM_BYTES_2,
                  (ushort)COMMON_REGISTERS__FRAM_ADDR__DEVICE_ACTIVE_INTERFACE,
                  (bool)UTILS_FUNCS__READ),
               S_Exceptions.u32_ErrorCodeSW_Sub))
      {
         // check and clear errors
         S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
         LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
      }
      if ( 		(u16_DeviceActiveInterface >= (ushort)COMMON_REGISTERS__ACTIVE_INTERFACES_COUNTER)
            ||	(u16_DeviceActiveInterface == (ushort)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__NONE)
            )
      {
         u16_DeviceActiveInterface = cu16_CommonRegistersFuncs_DefaultActiveInterface;
      }
      // set current active interface
      if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_WRITE,
                  (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_ACTIVE_INTERFACE, (ushort)AT91SAM7X__NUM_REGS_1,
                  (void *)(&u16_DeviceActiveInterface), NULL, cu16_TimeoutShort),
               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
#endif
      {
      }  // set current active interface
      // read modbus slave ID from permanent memory
      if (FAIL_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u16_ModbusSlaveID), (ushort)AT91SAM7X__NUM_BYTES_2,
                  (ushort)COMMON_REGISTERS__FRAM_ADDR__MODBUS_SET_SLAVE_ADDRESS,
                  (bool)COMMON_CMDS_OWNER__READ),
               S_Exceptions.u32_ErrorCodeSW_Sub))
      {
         // set and clear errors
         S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
         LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
      }
      // set modbus slave ID if valid - if NOT the default address from modbus init is used
      if ((u16_ModbusSlaveID <= (ushort)AT91SAM7X__MODBUS_MAX_SLAVE_ID) && (u16_ModbusSlaveID != 0))
      {
         if (FAIL (MBPBProductAddrSet (u16_ModbusSlaveID, cu16_TimeoutLong),
                  S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_ADDRESS_SET))
         {
            // set and clear errors
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
         }
      }
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
      // read run-time counter from permanent memory
      if (FAIL_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u32_DeviceRunTimeCounter), (ushort)AT91SAM7X__NUM_BYTES_4,
                  (ushort)COMMON_REGISTERS__FRAM_ADDR__DEVICE_RUN_TIME_COUNTER,
                  (bool)UTILS_FUNCS__READ),
               S_Exceptions.u32_ErrorCodeSW_Sub))
      {
         // set and clear errors
         S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
         LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
      }
      // initialize real-time timer
      if (SUCCESS (RttInit (u8_Unit, SEM_CommonSupportFuncs_DeviceRunTimeCounter, (ulong)AT91C_RTTC_RTTINC),
               S_Exceptions.u16_ErrorCodeSW_Base, (ulong)LIB_EXCEPTIONS_DRV__ERROR__RTT__INIT_UNIT))
      {
         // wait for finishing of started tasks
         os_dly_wait (AT91SAM7X__TICK_1);
         // start real-time timer (calibration of RTT)
         if (FAIL (RttRestart (u8_Unit, u16_RTT_Prescaler, u32_RTT_AlarmValue, u32_DeviceRunTimeCounter,
                     u32_RTT_PeriodForMsgGen, cu16_TimeoutMedium),
                  S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__RTT__RESTART))
         {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
         
         }  // start real-time timer (calibration of RTT)
      }  // initialize real-time timer
#endif
   }  // check for errors
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   // ### initialization of valve actuator ###
   if (S_Exceptions.u32_ErrorCodeSW_Sub == LIB_EXCEPTIONS__ERROR_NONE)
   {
   }  // check for errors
   if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START,
               (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_SykamDeviceOptions), NULL, cu16_TimeoutShort),
            S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
   
   } // read option field of sykam device data
   // check device options -  if not "autosampler" skip valve actuator init
   if (u16_SykamDeviceOptions & (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_VALVE_ACTUATOR)
   {
      // initialize valve actuator register bank
      if (FAIL_APP_FUNC (ValveActRegistersFuncs_InitRegisterBank(), S_Exceptions.u32_ErrorCodeSW_Sub))
      {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
      
      } // initialize valve actuator modbus tasks
      // initialize valve actuator modbus tasks
      if (FAIL_APP_FUNC (ValveActModbusFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
      {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
      
      } // initialize valve actuator register bank
      // wait for finishing of started tasks
      os_dly_wait ((ushort)AT91SAM7X__TICK_100);
      // initialize valve actuator support tasks
      if (FAIL_APP_FUNC (ValveActSupportFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
      {
            S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
            LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank         
      
      }
      // wait for finishing of started tasks
      os_dly_wait (AT91SAM7X__TICK_100);
      // load device params from fixed memory
      if (FAIL_APP_FUNC (ValveActCmdsOwner_DeviceParamsLoad ((ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD, &S_Exceptions.u32_ErrorCodeSW_Sub),
               S_Exceptions.u16_ErrorCodeSW_Base))
      {
         // load device params from fixed memory
         if (FAIL_APP_FUNC (ValveActCmdsOwner_DeviceParamsLoad ((ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD, &S_Exceptions.u32_ErrorCodeSW_Sub),
                  S_Exceptions.u16_ErrorCodeSW_Base))
         {
            if (SUCCESS_APP_FUNC
                  (ValveActCmdsOwner_DeviceParamsLoadDefault ((ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD_DEFAULT, &S_Exceptions.u32_ErrorCodeSW_Sub),
                     S_Exceptions.u16_ErrorCodeSW_Base))
            {
               S_Exceptions.u32_ErrorCodeSW_Sub=0;
               S_Exceptions.u16_ErrorCodeSW_Base=0;
               LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
            }
         }
      }
      if (S_Exceptions.u32_ErrorCodeSW_Sub)
      LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
      // load status data from fixed memory
      TRY_AND_CATCH_APP_FUNC
      (
      ValveActCmdsOwner_StatusDataLoad ((ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__STATUS_DATA_LOAD, &S_Exceptions.u32_ErrorCodeSW_Sub),
      S_Exceptions.u16_ErrorCodeSW_Base
      );
      if (S_Exceptions.u32_ErrorCodeSW_Sub)
      LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
      if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE, (ushort)AT91SAM7X__NUM_REGS_1,
                  (void *)(&u16_CurrentValveType), NULL, cu16_TimeoutShort),
               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         u16_CurrentValveType = cu16_ValveActValveTypes_DefaultValveType;
      }
      // calibrate device with loaded device params (or default if not available) and store data
      TRY_AND_CATCH_APP_FUNC
      (
      ValveActCmdsReg_CurrentValveType ((ushort)u16_CurrentValveType, &S_Exceptions.u32_ErrorCodeSW_Sub),
      S_Exceptions.u16_ErrorCodeSW_Base
      );
      if (S_Exceptions.u32_ErrorCodeSW_Sub)
      LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank            }  // initialize valve actuator support tasks
   } // check set valve actuator option in sykam device data
#endif
   // ### initialization of column oven ###
   if (S_Exceptions.u32_ErrorCodeSW_Sub == LIB_EXCEPTIONS__ERROR_NONE)
   {
      // initialize column oven register bank
      if (SUCCESS_APP_FUNC (ColumnOvenRegistersFuncs_InitRegisterBank(), S_Exceptions.u32_ErrorCodeSW_Sub))
      {
         // initialize column oven modbus tasks
         if (SUCCESS_APP_FUNC (ColumnOvenModbusFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
         {
            // wait for finishing of started tasks
            os_dly_wait ((ushort)AT91SAM7X__TICK_1);
            // initialize column oven support tasks
            if (SUCCESS_APP_FUNC (ColumnOvenSupportFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
            {
               // wait for finishing of started tasks
               os_dly_wait (AT91SAM7X__TICK_1);
               // initialize column oven signal processing tasks
               if (SUCCESS_APP_FUNC (ColumnOvenSignalProcessingFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
               {
                  // wait for finishing of started tasks
                  os_dly_wait (AT91SAM7X__TICK_1);
                  if (SUCCESS_APP_FUNC (ColumnOvenCmdsOwner_DeviceParamsLoad ((ushort)COLUMN_OVEN_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD, &S_Exceptions.u32_ErrorCodeSW_Sub),
                           S_Exceptions.u16_ErrorCodeSW_Base))
                  {
                     TRY_AND_CATCH_APP_FUNC
                     (
                     ColumnOvenCmdsOwner_StatusDataLoad ((ushort)COLUMN_OVEN_CMDS_OWNER__CMD_ARG__STATUS_DATA_LOAD, &S_Exceptions.u32_ErrorCodeSW_Sub),
                     S_Exceptions.u16_ErrorCodeSW_Base
                     );
                  }
               }
            }
         }
      }
   }
   // check for errors
   if ((S_Exceptions.u32_ErrorCodeSW_Sub) && (!S_Exceptions.u16_ErrorCodeSW_Base))
   {
      S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
      LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
   }
   // start ALIVE LED
   if (FAIL_APP_FUNC (UtilsFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
   {
      S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
      LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
   }
   //### debug
   // temporary reset of DAC voltage because debugger allows no true reset of DAC
   u8_Unit = 0;                                                                                                 // output value
   Dacrrcs085SetOutput (u8_Unit, AT91SAM7X__DACRRCS085_CHANNEL_POWER_HEAT_COOL, (uchar)DACRRCS085_MODE_WRITEANDUPDATE, 0, cu16_TimeoutLong);
   //### debug
   
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
//   // set system ready pin low and refresh display (level = second argument)
//   if (FAIL (Mcp2317SetOutput (u8_Unit, (ushort)AT91SAM7X__MCP2317_SYS0_RDY, (ushort)~AT91SAM7X__MCP2317_SYS0_RDY,
//               cu16_TimeoutShort),
//            S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MCP2317__SET_OUTPUT))
//   {
//      S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT;
//      // set exception codes to register bank
//      LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
//   }
#endif   
   #ifndef OEM_LED//If NOT Defined
   {
      Mcp2317SetOutput (0, (ushort)AT91SAM7X__MCP2317_SYS0_RDY, (ushort) AT91SAM7X__MCP2317_SYS0_RDY, OS_T_MS(500));      
      RESCHEDULE(100);   
      Mcp2317SetOutput (0, (ushort)AT91SAM7X__MCP2317_SYS0_RDY, (ushort)~AT91SAM7X__MCP2317_SYS0_RDY, OS_T_MS(500));
      RESCHEDULE(100);
      // update display
      AT91F_PIO_SetOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
      RESCHEDULE(50);
      // update display
      AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
      RESCHEDULE(50);
   }
   #endif    
           
   // allow forced runs (between configured tick interval) of scheduler for additional rtos queue processing
   UNLOCK_FORCECTRL;
   // set flag for gas sensing task
   b_ColumnOvenMain_InitDone = TRUE;

//   TID_CheckCommunication = os_tsk_create_user (TSK_CheckCommunication, (uchar)OS_P_NORMAL, (void*)STK_TSK_CheckCommunication,
//                                                  (ushort)(sizeof(STK_TSK_CheckCommunication)) );

   // quit init task (will also unlock scheduler again)
   os_tsk_delete_self();
}


////---------------------------------------------------------------------------------------------------------------------
////---------------------------------------------------------------------------------------------------------------------
////---------------------------------------------------------------------------------------------------------------------
////---------------------------------------------------------------------------------------------------------------------
////---------------------------------------------------------------------------------------------------------------------
////---------------------------------------------------------------------------------------------------------------------
//////////////////// Check Mainboards Inter Communication is working
//__task void TSK_CheckCommunication (void)
//{
//	unsigned short data_in, data_out, n;
//   ulong    u32_SPI_MsgMask            =  0xFFFFFFFF;
//   
//   #ifndef OEM_LED//If NOT Defined
//   Mcp2317SetOutput (0, (ushort)AT91SAM7X__MCP2317_SYS0_RDY, (ushort) AT91SAM7X__MCP2317_SYS0_RDY, OS_T_MS(500));      
//   RESCHEDULE(10);   
//   AT91F_PIO_SetOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
//   RESCHEDULE(50);            
//   FOREVER
//   {
//      MBPBRegAccess(MBPB_REGTYPE_HOLDING, MBPB_REGRW_READ, MODBUS_ADDR_HREG__DEVICE_COMMUNICATION_STATUS , 1, &data_in, NULL, OS_T_MS(100));               
//      if (! (data_in==0x00FF || data_in==0x0FF0 ) )
//      {
////         SpiInitUnit (1, (uchar)AT91SAM7X__DUMMY_ARG, (AT91C_SPI_MODFDIS),
////         (OS_ID)(&SEM_ColumnOvenMain_SpiNewMsg_Unit[1]), u32_SPI_MsgMask);
//         RESCHEDULE(50);
//      }         
//      data_out=0x0FF0;
//      MBPBRegAccess(MBPB_REGTYPE_HOLDING, MBPB_REGRW_WRITE, MODBUS_ADDR_HREG__DEVICE_COMMUNICATION_STATUS , 1, &data_out, NULL, OS_T_MS(100));                              
//      Mcp2317SetOutput (0, (ushort)AT91SAM7X__MCP2317_SYS0_RDY, (ushort)~AT91SAM7X__MCP2317_SYS0_RDY, OS_T_MS(500));
//      RESCHEDULE(50);
//      AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
//      RESCHEDULE(50);                  
//
//      RESCHEDULE(500);            
//   }
//   #endif               
//   DETACH;
//   // quit init task (will also unlock scheduler again)
//   os_tsk_delete_self();
//   
//}
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
int main (void)
{
   // TSK_ColumnOvenMain_Init          function pointer to init task
   // OS_P_NORMAL       					priority level of the init task
   // STK_TSK_ColumnOvenMain_Init      ulong pointer to an defined array (stack)
   os_sys_init_user (TSK_ColumnOvenMain_Init, (uchar)OS_P_NORMAL, (void *)STK_TSK_ColumnOvenMain_Init, (ushort)(sizeof(STK_TSK_ColumnOvenMain_Init)) );
   return 1;
}
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         int main (void)
*
*  \brief      Initialize and start RTX kernel as well as \ref TSK_ColumnOvenMain_Init.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         __task void TSK_ColumnOvenMain_Init (void)
*
*  \brief      Initialize complete system, see docu of \ref column_oven_main.c.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
