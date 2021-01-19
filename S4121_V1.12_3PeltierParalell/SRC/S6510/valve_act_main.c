/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \mainpage            Project:                   Valve Actuator Control
*
*  \par                 Description:
*
*                       The purpose of this application is to control a valve actuator with the help of an electric step motor. \n
*                       Initializing of complete system requires approximately 1.3 s including initialization of RTT (externally measured). 
*                       In this time all drivers will be set up and calibration data as well as diverse parameters and Sykam device data are loaded.
*                       Calibration data means static data from module "encgapdisk" or in other words calibration data comprises the amount of 
*                       detected edges and distances in microsteps between them from a gap disk.\n
*                       If load of valve actuator calibration data or params fails during initialization, calibration routine is executed with 
*                       device parameters currently written in valve actuator register bank. In case of success calibration data and 
*                       device parameters are saved. In case of procedure failure calibration routine is executed again during every device restart
*                       as long as not successful.\n
*                       For any error occurred during system initialization or at run-time a software base and sub error code is set to valve actuator
*                       register bank. \n
*                       Furthermore certain \em commands exist which are buffered by means of drivers. An detailed description can be found in
*                       project corresponding documentation. \n
*                       At run-time a command blocking occurs as long as last command was not finished. However, commands are buffered by means
*                       of drivers and executed successively.\n
*                       The valve actuator control is actual possible via interfaces named in \ref _E_CommonRegisters_DeviceActiveInterfaceList.
*                       Only one interface can be active at run time.
*
*
*  Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*
*//*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_main.c
*
*  \brief               The main file initializes several driver modules, namely the \em USART, \em PWM, \em SPI, \em StpClkDir (step motor),
*                       \em EncGapDisk (encoder gap disk) and the externals \em DAC, \em MCP23S17 (IO-expander) as well as the \em Fm25CLSS (FRAM), etc.
*                       The active device interface is set to "none" at beginning of \ref TSK_Init, i.e. it must be selected by user via RS232 or 
*                       display keys if device parameters could not be load during initialization or if the device is new and used for the first time.\n
*                       Every function returns whether a software base and sub error code which is greater than zero in any error case.
*
*
*  \author              Ruediger Radtke
*  \version             1.10
*
*
*  \date                2010-06-28     Initial version of S6510_main.c imported from evaluation.c of hschoettner.
*  \date                2010-08-12     Changed init order of modbus (after spi now).
*  \date                2010-11-08     Final project version 1.00.
*  \date                2011-02-07     Renamed task, mailbox and error code from "S6X1Xi_OutputOnPinContactStrip" to "InjectSignal".
*                                      Changed error handling in \em TSK_Init and \em LoadCalibrationDataAndParams.
*  \date                2011-05-17     Updated to MDK-ARM 4.13a and updated MCP2317 driver.
*  \date                2011-07-22     Adapted register bank for included driver RTT, included SysIRQ driver too, changed system tick.
*                                      time to 1 ms, moved some files into new directories, changed device states and adapted timeout 
*                                      values in S6510_main.c.
*  \date                2011-07-25     Final project version 1.02. See external documentation for software changes. Main difference 
*                                      is an driver update.
*  \date                2011-07-25     Final project version 1.03. Added missing correct initialization of SPI channel for DAC. 
*                                      Calibration now at every device startup.
*  \date                2012-11-06     Added initialization of internal ADC for common voltage monitoring task.
*  \date                2012-11-20     Final project version 1.10. See external documentation or file changes list for changes.
*  
*  
*  \bug                 2011-07-25     Removed false valve positioning after long term execution with driver update.
*
*  \test                2010-11-03  	Tested whole initialization and corresponding errors cases.
*  \test                2011-07-25  	Tested whole initialization and corresponding errors cases.
*  \test                2011-09-07  	Tested whole initialization and corresponding errors cases.
*  \test                2012-11-09  	Tested whole initialization and corresponding errors cases.
*
*  \todo                
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
// Includes
#include <limits.h>

#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\..\cpu\at91\pio\pioext.h"

#include "..\..\..\cpu\at91\pwm\pwmext.h"

#include "..\..\..\cpu\at91\spi\spi.h"
#include "..\..\..\cpu\at91\spi\spiext.h"

#include "..\..\..\cpu\at91\usart\usartext.h"

#include "..\..\..\cpu\at91\adc\adc.h"

#include "..\..\..\cpu\at91\adcle\adcle.h"

#include "..\..\..\cpu\at91\rtt\rttext.h"

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

#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"

#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\lib_exceptions_funcs.h"
#include "..\..\shared\src\common_cmds_owner.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\common_registers_funcs.h"
#include "..\..\shared\src\common_modbus_funcs.h"
#include "..\..\shared\src\common_support_funcs.h"
#include "..\..\shared\src\utils_funcs.h"

#include "valve_act_modbus_funcs.h"
#include "valve_act_modbus_tasks.h"
#include "valve_act_registers.h"
#include "valve_act_registers_funcs.h"
#include "valve_act_cmds_owner.h"
#include "valve_act_cmds_reg.h"
#include "valve_act_support_funcs.h"
#include "valve_act_valvetypes.h"


//---------------------------------------------- global constants ------------------------------------------------------------------------------

const ushort cu16_TimeoutVeryShort    = AT91SAM7X__TICK_1;             // 1 system tick
const ushort cu16_TimeoutShort        = AT91SAM7X__TICK_10;            // 10 ticks
const ushort cu16_TimeoutMedium       = AT91SAM7X__TICK_20;            // 20 ticks
const ushort cu16_TimeoutLong         = AT91SAM7X__TICK_30;            // 30 ticks

//---------------------------------------------- global declarations/definitions ---------------------------------------------------------------

// customized stacks
OS_TSK_STACK_USER(STK_TSK_ValveActMain_Init, 1536);            /**< define 1536 byte stack for \ref TSK_Init */ 

// semaphores
OS_SEM   SEM_ValveActMain_SpiNewMsg_Unit[SPI_UNIT_MAX];
OS_SEM   SEM_ValveActMain_SpiNewMsg_Channel[SPI_CHANNEL_MAX];

//---------------------------------------------- externals -------------------------------------------------------------------------------------

extern         OS_SEM   SEM_CommonSupportFuncs_DeviceRunTimeCounter;

extern         struct   _SMcp2317Item                       SMcp2317List[MCP2317_LIST_SIZE];
extern         struct   _SPioItem                           SPioListA[AT91F_PIOLIST_SIZEA];
extern         struct   _SPioItem                           SPioListB[AT91F_PIOLIST_SIZEB];
extern         struct   _SPwmItem                           SPwmList[AT91F_PWMLIST_SIZE];

extern const   ushort   cu16_TimeoutVeryShort;
extern const   ushort   cu16_TimeoutShort;
extern const   ushort   cu16_TimeoutMedium;
extern const   ushort   cu16_TimeoutLong;
extern const   ushort   cu16_ValveActValveTypes_DefaultValveType;
extern const   ushort   cu16_CommonRegistersFuncs_DefaultActiveInterface;


//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

__task   void     TSK_Init    (void);
         int      main        (void);

//---------------------------------------------- functions -------------------------------------------------------------------------------------

__task void TSK_ValveActMain_Init (void)
{
   uchar    u8_Unit                    =  0;
   uchar    u8_ChannelsToEnable;
   ushort   u16_DeviceState;
   ushort   u16_ModbusSlaveID;
   ushort   u16_DeviceActiveInterface  =  cu16_CommonRegistersFuncs_DefaultActiveInterface;   // default needed if load from permanent memory fails
   ushort   u16_CurrentValveType       =  cu16_ValveActValveTypes_DefaultValveType;
   ushort   u16_RTT_Prescaler          =  0x8000;                        /**<  RTT increment every x [s] with x = prescaler / 32768 Hz (slow crystal) */
   ulong    u32_RTT_PeriodForMsgGen    =  0x0000000A;                    /**<  calling corresponding user task periodically every u32_RTT_PeriodForMsgGen * [prescaler / 32768 Hz] */
   ulong    u32_RTT_AlarmValue         =  0x00000000;
   ulong    u32_Idx;
   ulong    u32_SPI_MsgMask            =  (ulong)0xFFFFFFFF;
   ulong    u32_DeviceRunTimeCounter   =  0;
   ulong    u32_ErrorCodeSW_Sub;
   
   tS_LibExceptions_Exceptions S_Exceptions;
   
   
   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   
   // wait for power up to default mainboard voltages
   os_dly_wait (AT91SAM7X__TICK_500);
   
   
   // configure PIO A
   if (SUCCESS (PioInit(u8_Unit, SPioListA, AT91F_PIOLIST_SIZEA), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__PIO__INIT))
   {  
      // configure PIO B
      u8_Unit = 1;
      if (SUCCESS (PioInit(u8_Unit, SPioListB, AT91F_PIOLIST_SIZEB), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__PIO__INIT))
      {
         // configure PWM (dispatcher for shared irq of all channels)
         u8_Unit = 0;
         if (SUCCESS (PwmInitUnit (u8_Unit, SPwmList, AT91F_PWMLIST_SIZE), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__PWM__INIT_UNIT))
         {
            // initialize message semaphores for spi
            for (u32_Idx = 0; u32_Idx < SPI_UNIT_MAX; u32_Idx++)
            {
               os_sem_init (SEM_ValveActMain_SpiNewMsg_Unit[u32_Idx], 0);
            }   
            // initialize channel semaphores for spi
            for (u32_Idx = 0; u32_Idx < SPI_CHANNEL_MAX; u32_Idx++)
            {
               os_sem_init (SEM_ValveActMain_SpiNewMsg_Channel[u32_Idx], 0);
            }    

            // init spi unit zero (SPI0)
            if (SUCCESS (SpiInitUnit (u8_Unit, (uchar)AT91SAM7X__DUMMY_ARG, (ulong)(AT91C_SPI_PCS | AT91C_SPI_MODFDIS |
                                       AT91C_SPI_PCSDEC | AT91C_SPI_MSTR), (OS_ID)(&SEM_ValveActMain_SpiNewMsg_Unit[0]), u32_SPI_MsgMask),
                         S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_UNIT))
            {
                                       //    unit, channel, clock and options, max bytes per cs-cycle, max delay, job priority,  
                                       //    spi driver flags, initialized sem for new msg triggering, msg mask for counters
               if (SUCCESS (SpiInitChannel (u8_Unit, (uchar)FM25CLSS_SPI_CHANNEL, (ulong)(SPI_CSR_SCBR(AT91B_MCK, FM25CLSS_SPI_BAUDRATE) | 
                                            AT91C_SPI_BITS_8 | AT91C_SPI_NCPHA), (ushort)FM25CLSS_BLOCKSIZEMAX, (ushort)OS_T_FOREVER, 
                                            (uchar)SPI_P_NORMAL, (uchar)SPI_CHANNEL_FLAGS_CSRUPDATE, 
                                            (OS_ID)(&SEM_ValveActMain_SpiNewMsg_Channel[FM25CLSS_SPI_CHANNEL]), u32_SPI_MsgMask),
                            S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_CHANNEL))
               {
                  if (SUCCESS (SpiInitChannel (u8_Unit, (uchar)AT91SAM7X__DACRRCS085_SPI_CHANNEL, (ulong)(SPI_CSR_SCBR(AT91B_MCK, 
                                               AT91SAM7X__DACRRCS085_SPI_BAUDRATE) | AT91C_SPI_BITS_8 /*|  AT91C_SPI_NCPHA*/), 
                                               (ushort)DACRRCS085_BLOCKSIZEMAX, (ushort)OS_T_FOREVER, (uchar)SPI_P_NORMAL, 
                                               (uchar)SPI_CHANNEL_FLAGS_CSRUPDATE, (OS_ID)(&SEM_ValveActMain_SpiNewMsg_Channel[AT91SAM7X__DACRRCS085_SPI_CHANNEL]),
                                               u32_SPI_MsgMask),
                               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_CHANNEL))
                  {
                     if (SUCCESS (SpiInitChannel (u8_Unit, (uchar)MCP2317_SPI_CHANNEL, (ulong)(SPI_CSR_SCBR(AT91B_MCK, MCP2317_SPI_BAUDRATE ) | 
                                                  AT91C_SPI_BITS_8 | AT91C_SPI_NCPHA), (ushort)MCP2317_BLOCKSIZEMAX,
                                                  (ushort)OS_T_FOREVER, (uchar)SPI_P_NORMAL, (uchar) SPI_CHANNEL_FLAGS_CSRUPDATE,
                                                  (OS_ID)(&SEM_ValveActMain_SpiNewMsg_Channel[MCP2317_SPI_CHANNEL]), u32_SPI_MsgMask),
                                  S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_CHANNEL))
                     {
                        // init spi slave unit one (SPI1)
                        u8_Unit = 1;
                        TRY_AND_CATCH
                        (
                           SpiInitUnit (u8_Unit, (uchar)AT91SAM7X__DUMMY_ARG, (AT91C_SPI_MODFDIS), 
                                        (OS_ID)(&SEM_ValveActMain_SpiNewMsg_Unit[1]), u32_SPI_MsgMask),
                           S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_UNIT
                        );
                        u8_Unit = 0;

                     }  // initialize SPI channel
                  }  // initialize SPI channel
               }  // initialize SPI channel
            }  // initialize SPI0
         }  // initialize PWM
      }  // initialize PIO B
   }  // initialize PIO A
   
   if (S_Exceptions.u32_ErrorCodeSW_Sub == LIB_EXCEPTIONS__ERROR_NONE)
   {
      // configure DAC102S085 (two channels)
      if (SUCCESS (Dacrrcs085Init (u8_Unit, (uchar)AT91SAM7X__DUMMY_ARG, &SEM_ValveActMain_SpiNewMsg_Channel[AT91SAM7X__DACRRCS085_SPI_CHANNEL]),
                   S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__DACRRCS085__INIT_UNIT))
      {
         // initialize endoder gap disk           
         if (SUCCESS (EncGapDiskInit(u8_Unit), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__INIT_UNIT))
         {
            // configure port expander MCP2317
            if (SUCCESS (Mcp2317Init (u8_Unit, SMcp2317List, MCP2317_LIST_SIZE, &SEM_ValveActMain_SpiNewMsg_Channel[MCP2317_SPI_CHANNEL]), 
                         S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MCP2317__INIT_UNIT))
            {
               // wait for finishing of started tasks
               os_dly_wait (AT91SAM7X__TICK_1);

               // initialize step motor
               if ( SUCCESS (StpClkDirInit (u8_Unit, (ushort)STPCLKDIR0_DAC_HOLD, (ushort)STPCLKDIR0_DAC_RUN), 
                             S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__INIT_UNIT))
               {
                  // initialize FRAM FM25CL64
                  if (SUCCESS (Fm25clssInit (u8_Unit, &SEM_ValveActMain_SpiNewMsg_Channel[FM25CLSS_SPI_CHANNEL]), 
                               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__INIT))
                  {
                     // set user ADC channels to enable (mainboard voltage monitoring)
                     u8_ChannelsToEnable = (AT91C_ADC_CH4 | AT91C_ADC_CH5);
                     
                     // initialize ADC
                     if (SUCCESS (AdcInit (u8_ChannelsToEnable), S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ADC__INIT))
                     {}
                  }
               }  // initialize step motor
            }  // initialize IO expander
         }  // initialize encoder gap disk
      }  // initialize DAC
   }  // check for errors
   
   // ### initialize application dependent modbus driver
   if (SUCCESS_APP_FUNC (ValveActModbusFuncs_InitDriver(), u32_ErrorCodeSW_Sub))
   {
      os_dly_wait (AT91SAM7X__TICK_1);
      // set the product state to "on" (modbus is always active after initialization -
      // this is just a state flag to be read out with modbus cmd 0x11)
      TRY_AND_CATCH
      (
         MBPBProductStateSet (TRUE, cu16_TimeoutLong),
         u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_STATE_SET
      );
   }
   
   
   // ### initialization of common parts ###
   
   if ((S_Exceptions.u32_ErrorCodeSW_Sub == LIB_EXCEPTIONS__ERROR_NONE) || (u32_ErrorCodeSW_Sub == LIB_EXCEPTIONS__ERROR_NONE))
   {
      // initialize common register bank
      if (SUCCESS_APP_FUNC (CommonRegistersFuncs_InitRegisterBank(), S_Exceptions.u32_ErrorCodeSW_Sub))
      {
         // initialize common modbus tasks
         if (SUCCESS_APP_FUNC (CommonModbusFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
         {
            // initialize common support tasks
            if (SUCCESS_APP_FUNC (CommonSupportFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
            {
               // wait for finishing of started tasks
               os_dly_wait (AT91SAM7X__TICK_2);
               
               // load Sykam device data -- go ahead if load data fails
               if (FAIL_APP_FUNC (CommonCmdsOwner_SykamDeviceDataLoad ((ushort)COMMON_CMDS_OWNER__CMD_ARG__SYKAM_DEVICE_DATA_LOAD, 
                                                                        &S_Exceptions.u32_ErrorCodeSW_Sub),
                                  S_Exceptions.u16_ErrorCodeSW_Base))
               {
                  LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
               }

//               // set device state
//               u16_DeviceState = (uchar)COMMON_REGISTERS__DEVICE_STATE__IDLE;
//                     
//               if (SUCCESS_APP_FUNC (CommonSupportFuncs_DeviceStateRW ((ushort *)(&u16_DeviceState), (bool)COMMON_SUPPORT_FUNCS__WRITE),
//                                     S_Exceptions.u32_ErrorCodeSW_Sub))
//               {
                  // read current active interface from permanent memory
                  if (FAIL_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)&u16_DeviceActiveInterface, (ushort)AT91SAM7X__NUM_BYTES_2,
                                                                        (ushort)COMMON_REGISTERS__FRAM_ADDR__DEVICE_ACTIVE_INTERFACE,
                                                                        (bool)UTILS_FUNCS__READ),
                                     S_Exceptions.u32_ErrorCodeSW_Sub))
                  {
                     // check and clear errors
                     S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN__TSK_INIT;
                     LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
                  }
                  
                  // check active interface
                  if (     (u16_DeviceActiveInterface >= (ushort)COMMON_REGISTERS__ACTIVE_INTERFACES_COUNTER)
                        || (u16_DeviceActiveInterface == (ushort)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__NONE)
                     )
                  {
                     u16_DeviceActiveInterface = cu16_CommonRegistersFuncs_DefaultActiveInterface;
                  }
                  
                  // set current active interface
                  if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_WRITE, 
                                              (ushort)COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_ACTIVE_INTERFACE, (ushort)AT91SAM7X__NUM_REGS_1, 
                                              (void *)(&u16_DeviceActiveInterface), NULL, cu16_TimeoutShort),
                               S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
                  {
                     // read modbus slave ID from permanent memory
                     if (FAIL_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u16_ModbusSlaveID), (ushort)AT91SAM7X__NUM_BYTES_2, 
                                                                           (ushort)COMMON_REGISTERS__FRAM_ADDR__MODBUS_SET_SLAVE_ADDRESS,
                                                                           (bool)COMMON_CMDS_OWNER__READ),
                                        S_Exceptions.u32_ErrorCodeSW_Sub))
                     {
                        // set and clear errors
                        S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN__TSK_INIT;
                        LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
                     }

                     // set modbus slave ID if valid - if NOT the default address from modbus init is used
                     if ((u16_ModbusSlaveID <= (ushort)AT91SAM7X__MODBUS_MAX_SLAVE_ID) && (u16_ModbusSlaveID != 0))
                     {
                        if (FAIL (MBPBProductAddrSet (u16_ModbusSlaveID, cu16_TimeoutLong),
                                  S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_ADDRESS_SET))
                        {
                           // set and clear errors
                           S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN__TSK_INIT;
                           LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
                        }
                     }
                     
                     // read run-time counter from permanent memory
                     if (FAIL_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u32_DeviceRunTimeCounter), (ushort)AT91SAM7X__NUM_BYTES_4,
                                                                           (ushort)COMMON_REGISTERS__FRAM_ADDR__DEVICE_RUN_TIME_COUNTER,
                                                                           (bool)UTILS_FUNCS__READ), 
                                        S_Exceptions.u32_ErrorCodeSW_Sub))
                     {
                        // set and clear errors
                        S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN__TSK_INIT;
                        LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
                     }

                     // initialize real-time timer
                     if (SUCCESS (RttInit (u8_Unit, SEM_CommonSupportFuncs_DeviceRunTimeCounter, (ulong)AT91C_RTTC_RTTINC),
                                  S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__RTT__INIT_UNIT))
                     {
                        // wait for finishing of started tasks
                        os_dly_wait (AT91SAM7X__TICK_1);
                        
                        // start real-time timer (calibration of RTT)
                        if (SUCCESS (RttRestart (u8_Unit, u16_RTT_Prescaler, u32_RTT_AlarmValue, u32_DeviceRunTimeCounter, 
                                                 u32_RTT_PeriodForMsgGen, cu16_TimeoutMedium),
                                     S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__RTT__RESTART))
                        {
                        }  // start real-time timer (calibration of RTT)
                     }  // initialize real-time timer
                  }  // set current active interface
//               }  // set device state
            }  // initialize common support tasks
         }  // initialize common modbus tasks
      }  // initialize common register bank
   }  // check for errors
   
   
   // ### initialization of valve actuator ###

   if (S_Exceptions.u32_ErrorCodeSW_Sub == LIB_EXCEPTIONS__ERROR_NONE)
   {
      // initialize valve actuator modbus tasks
      if (SUCCESS_APP_FUNC (ValveActModbusFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
      {
         // wait for finishing of started tasks
         os_dly_wait ((ushort)AT91SAM7X__TICK_1);
         
         // initialize valve actuator register bank
         if (SUCCESS_APP_FUNC (ValveActRegistersFuncs_InitRegisterBank(), S_Exceptions.u32_ErrorCodeSW_Sub))
         {
            // initialize valve actuator support tasks
            if (SUCCESS_APP_FUNC (ValveActSupportFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
            {
               // wait for finishing of started tasks
               os_dly_wait (AT91SAM7X__TICK_1);
               
               // load device params from fixed memory
               if (FAIL_APP_FUNC (ValveActCmdsOwner_DeviceParamsLoad ((ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD, &S_Exceptions.u32_ErrorCodeSW_Sub),
                                  S_Exceptions.u16_ErrorCodeSW_Base))
               {
                  TRY_AND_CATCH_APP_FUNC
                  (
                     ValveActCmdsOwner_DeviceParamsLoadDefault ((ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD_DEFAULT, &S_Exceptions.u32_ErrorCodeSW_Sub),
                     S_Exceptions.u16_ErrorCodeSW_Base
                  );
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
               
               if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ, 
                                        (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE, (ushort)AT91SAM7X__NUM_REGS_1, 
                                        (void *)(&u16_CurrentValveType), NULL, cu16_TimeoutShort),
                         S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  u16_CurrentValveType = cu16_ValveActValveTypes_DefaultValveType;
               }
               
               // calibrate device with loaded device params (or default if not available) and store data
               TRY_AND_CATCH_APP_FUNC
               (
                  ValveActCmdsReg_CurrentValveType (u16_CurrentValveType, &S_Exceptions.u32_ErrorCodeSW_Sub),
                  S_Exceptions.u16_ErrorCodeSW_Base
               );
               
               if (S_Exceptions.u32_ErrorCodeSW_Sub)
                  LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
               
            }  // initialize valve actuator support tasks
         }  // initialize valve actuator register bank
      }  // initialize valve actuator modbus tasks
   }  // check for errors
   
   
   // check for errors
   if ((S_Exceptions.u32_ErrorCodeSW_Sub || u32_ErrorCodeSW_Sub) && (!S_Exceptions.u16_ErrorCodeSW_Base))
   {
      S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN__TSK_INIT;
      LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
      
      if (u32_ErrorCodeSW_Sub)
      {
         S_Exceptions.u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN__TSK_INIT;
         S_Exceptions.u32_ErrorCodeSW_Sub    =  u32_ErrorCodeSW_Sub;
         LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
      }
   }
   
   
   // start ALIVE LED
   if (FAIL_APP_FUNC (UtilsFuncs_InitTasks(), S_Exceptions.u32_ErrorCodeSW_Sub))
   {
      S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN__TSK_INIT;
      LibExceptionsFuncs_ExceptionSet (&S_Exceptions);   // set exception codes to register bank
   }
   
   
   // set system ready pin low and refresh display (level = second argument)
   if (FAIL (Mcp2317SetOutput (u8_Unit, (ushort)AT91SAM7X__MCP2317_SYS0_RDY, (ushort)~AT91SAM7X__MCP2317_SYS0_RDY,
                               cu16_TimeoutShort),
             S_Exceptions.u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MCP2317__SET_OUTPUT))
   {
      S_Exceptions.u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN__TSK_INIT;
      // set exception codes to register bank
      LibExceptionsFuncs_ExceptionSet (&S_Exceptions);
   }
   
   // update display
   AT91F_PIO_ClearOutput (AT91SAM7X__BASE_PIO__DISP0_ATN, AT91SAM7X__PIN__DISP0_ATN);
   
   // allow forced runs (between configured tick interval) of scheduler for additional rtos queue processing
   UNLOCK_FORCECTRL;
   
   // quit init task (will also unlock scheduler again)
   os_tsk_delete_self();
}

//---------------------------------------------------------------------------------------------------------------------

int main (void)
{
   // TSK_ValveActMain_Init         function pointer to init task
   // OS_P_NORMAL                   priority level of the init task
   // STK_TSK_ValveActMain_Init     ulong pointer to an defined array (stack)
   os_sys_init_user (TSK_ValveActMain_Init, (uchar)OS_P_NORMAL, (void *)STK_TSK_ValveActMain_Init, (ushort)(sizeof(STK_TSK_ValveActMain_Init)));

   return 1;
}


/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         int main (void)
*
*  \brief      Initialize and start RTX kernel as well as \ref TSK_ValveActMain_Init.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         __task void TSK_ValveActMain_Init (void)
*
*  \brief      Initialize complete system, see docu of \ref valve_act_main.c.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
