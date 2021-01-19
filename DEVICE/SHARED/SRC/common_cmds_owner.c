/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_cmds_owner.c
*
*  \brief               This file contains the \em owner (manufacturer) command functions which will be executed if someone 
*                       writes the appropriate command argument to specified holding register of modbus (MBPB). Execution
*                       of these functions is only allowed to owner (manufacturer) or service technician. Further details
*                       about register bank layout are externally documentated. \n
*                       Every function returns an software base and sub error code in case any error occured.
*                       It is recommended to add new command functions in this file \b only to the end of function pointer
*                       arrays since there exists a documentation about common commands and corresponding modbus addresses.
*
*
*  \author              Ruediger Radtke
*  \version             1.3
*
*  \date                2011-07-29     Initial version imported from S4121_cmd_function_owner.c.
*  \date                2012-08-21     <em>File version 1.1</em>. Changed function argument for \ref CommonSupportFuncs_ErrorStringSW_FindAndSet
*                                      and removed obsolete variable in command \ref CommonCmdsOwner_ErrorCodeSW_ClearCurr.
*  \date                2012-10-31     <em>File version 1.2</em>. Added passing of sub error code in command \ref CommonCmdsOwner_ErrorCodeSW_ClearCurr.
*  \date                2012-11-26     <em>File version 1.3</em>. Removed consistency check from command \ref CommonCmdsOwner_SykamDeviceDataLoad. 
*                                      Device class and software version are now taken from configuration file \ref sykam_cfg.c.
*
*
*  \bug                 2012-10-31     No passing of software sub error code in command \ref CommonCmdsOwner_ErrorCodeSW_ClearCurr. \n
*
*  \test                2011-09-02     Tested if modbus slave ID is saved to permanent memory.
*  \test                2011-09-06     Tested \ref CommonCmdsOwner_ErrorCodeSW_ClearCurr and \ref CommonCmdsOwner_ErrorCodeSW_ClearAll
*                                      with single and multiple errors as well as with single errors with one software sub error.
*
*  \todo                
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <AT91SAM7X512.H>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"

#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"

#include "..\..\..\cpu\at91\rtt\rttext.h"

#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "lib_exceptions.h"
#include "common_cmds_owner.h"
#include "common_registers.h"
#include "common_registers_funcs.h"
#include "common_support_funcs.h"
#include "utils_funcs.h"


//---------------------------------------------- globals definitions ---------------------------------------------------------------------------

// The size and order must be the same like in documentation (excel table)!

tfp_CommonCmdsOwner_CMD   fpA_CommonCmdsOwner_CMDs[] = 
{
   { &CommonCmdsOwner_SykamDeviceDataLoad          },
   { &CommonCmdsOwner_SykamDeviceDataSave          },
   { &CommonCmdsOwner_ModbusSetSlaveAddress        },
   { &CommonCmdsOwner_DeviceRunTimeCounterReset    },
   { &CommonCmdsOwner_ErrorCodeSW_ClearCurr        },
   { &CommonCmdsOwner_ErrorCodeSW_ClearAll         },
   { &CommonCmdsOwner_SykamDeviceDataWriteEnable   }
};

//---------------------------------------------- externals used in this file -------------------------------------------------------------------

extern   const    ushort   cu16_UtilsFuncs_TimeoutShort;
extern   const    ushort   cu16_UtilsFuncs_TimeoutMedium;
extern   const    ushort   cu16_UtilsFuncs_TimeoutLong;

extern   const    ushort   cu16_SykamCfg_DeviceClass;
extern   const    ushort   cu16_SykamCfg_Version;

//---------------------------------------------- owner (manufacturer) command functions --------------------------------------------------------

ushort CommonCmdsOwner_SykamDeviceDataLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   tS_CommonRegisters_SykamDeviceData  S_CommonRegisters_SykamDeviceData;
   struct   _SMBPBRegAccess            S_MBPB_AccessParams;

   ushort   u16_DataSize;                       // in bytes
   ushort   u16_FRAM_BaseAddr;
   ushort   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   

   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;

   // load sykam device data from FRAM
   memset   ((void *)(&S_CommonRegisters_SykamDeviceData), 0, sizeof(S_CommonRegisters_SykamDeviceData));

   u16_FRAM_BaseAddr    = (ushort)  COMMON_REGISTERS__FRAM_ADDR__SYKAM_DEVICE_DATA__START;
   u16_DataSize         = (ushort)  (sizeof(S_CommonRegisters_SykamDeviceData));

   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&S_CommonRegisters_SykamDeviceData), u16_DataSize, 
                                                            u16_FRAM_BaseAddr, (bool)COMMON_CMDS_OWNER__READ),
                         *pu32_ErrorCodeSW_Sub))
   {
   }   
   // swap bytes of all array data for saving data to register bank (implies correct Sykam device data read out via modbus) 
   if (SUCCESS_APP_FUNC  (UtilsFuncs_SwapBytes  ((void *)(& S_CommonRegisters_SykamDeviceData), (ushort)(COMMON_REGISTERS__SYKAM_DEVICE_DATA_CHAR_ARRAYS_TOTAL_WORD_SIZE)),
                          *pu32_ErrorCodeSW_Sub) )
   {
   }      
   // set devices class and software version number from config file because they are constant and set with every new software version
   S_CommonRegisters_SykamDeviceData.u16_DeviceClass  =  cu16_SykamCfg_DeviceClass;
   S_CommonRegisters_SykamDeviceData.u16_Version      =  cu16_SykamCfg_Version;
   
   // set device params into corresponding modbus registers
   S_MBPB_AccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
   S_MBPB_AccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
   S_MBPB_AccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__START;
   S_MBPB_AccessParams.usAddrOffs   =  (ushort) 0x0000;  
   S_MBPB_AccessParams.usCount      =  (ushort) (sizeof(S_CommonRegisters_SykamDeviceData)/2);  // word size needed!
   S_MBPB_AccessParams.pvData       =  (void *) (&S_CommonRegisters_SykamDeviceData);  
   S_MBPB_AccessParams.pvIntern     =  NULL;                                                    // debug purpose, set to &pv_BufferAddress if needed
   S_MBPB_AccessParams.u16To        =  cu16_UtilsFuncs_TimeoutLong;

   TRY_AND_CATCH
   ( 
      MBPBRegAccessStruct  ((void*)(&S_MBPB_AccessParams)),
      *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
   );
   if (*pu32_ErrorCodeSW_Sub)
      u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__SYKAM_DEVICE_DATA_LOAD;

   return u16_ErrorCodeSW_Base;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ushort CommonCmdsOwner_SykamDeviceDataSave (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   tS_CommonRegisters_SykamDeviceData     S_CommonRegisters_SykamDeviceData; 
   struct   _SMBPBRegAccess               S_MBPB_AccessParams;

   ushort   u16_DataSize;                       // in bytes
   ushort   u16_FRAM_BaseAddr;
   ushort   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR_NONE;


   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;

   // disable write for Sykam device data (enable write protection)
   u16_CommonRegistersFuncs_SykamDeviceDataWriteEnableCode = 0;

   // read out modbus register bank (sykam params)
   S_MBPB_AccessParams.ucType     =  (uchar)  MBPB_REGTYPE_HOLDING;
	S_MBPB_AccessParams.bRW        =  (uchar)  MBPB_REGRW_READ;
   S_MBPB_AccessParams.usAddr     =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__START;
	S_MBPB_AccessParams.usAddrOffs =  (ushort) 0x0000;
	S_MBPB_AccessParams.usCount    =  (ushort) (sizeof(S_CommonRegisters_SykamDeviceData)/2);    // word size needed!
	S_MBPB_AccessParams.pvData     =  (void *) (&S_CommonRegisters_SykamDeviceData);
	S_MBPB_AccessParams.pvIntern   =  NULL;                                                      // debug purpose, set to &pv_BufferAddress if needed
	S_MBPB_AccessParams.u16To      =  cu16_UtilsFuncs_TimeoutLong;
   
   // map the content of device data into local structure; 
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_MBPB_AccessParams)),  *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS) )
   { 
   }    
      // swap bytes of all array data for data consistency check (correct byte order)
      if (SUCCESS_APP_FUNC  (UtilsFuncs_SwapBytes  ((void *)(& S_CommonRegisters_SykamDeviceData), (ushort)(COMMON_REGISTERS__SYKAM_DEVICE_DATA_CHAR_ARRAYS_TOTAL_WORD_SIZE)), *pu32_ErrorCodeSW_Sub) )
      {
      }      
         // check if params set do not interfere with const parts of sykam device params
         if( SUCCESS_APP_FUNC (CommonRegistersFuncs_SykamDeviceDataConsistencyCheck (&S_CommonRegisters_SykamDeviceData), *pu32_ErrorCodeSW_Sub) )
         {   
         }         
            // save sykam device params to FRAM    
            u16_FRAM_BaseAddr    = (ushort)COMMON_REGISTERS__FRAM_ADDR__SYKAM_DEVICE_DATA__START;
            u16_DataSize         = (ushort)(sizeof(S_CommonRegisters_SykamDeviceData));

            TRY_AND_CATCH_APP_FUNC
            ( 
               UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_CommonRegisters_SykamDeviceData), u16_DataSize, u16_FRAM_BaseAddr, (bool)COMMON_CMDS_OWNER__WRITE),
               *pu32_ErrorCodeSW_Sub
            );
   if (*pu32_ErrorCodeSW_Sub)
      u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__SYKAM_DEVICE_DATA_SAVE;

   return u16_ErrorCodeSW_Base;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ushort CommonCmdsOwner_ModbusSetSlaveAddress  (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   uchar    u8_MBPB_SlaveID            =  (uchar)u16_ModbusRxData;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_BaseAddress;
   ushort   u16_FRAM_NumBytesToWrite;
   ulong    u16_ErrorCodeSW_Base       =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;


   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;

   // set the slave address (own modbus address)
   if (SUCCESS (MBPBProductAddrSet  (u8_MBPB_SlaveID, cu16_UtilsFuncs_TimeoutLong), 
                *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_ADDRESS_SET) )
   {
      u16_FRAM_Data              =  (ushort)u8_MBPB_SlaveID;
      u16_FRAM_NumBytesToWrite   =  2;
      u16_FRAM_BaseAddress       =  (ushort)COMMON_REGISTERS__FRAM_ADDR__MODBUS_SET_SLAVE_ADDRESS;

      // write new modbus slave id to FRAM
      TRY_AND_CATCH_APP_FUNC
      (
         UtilsFuncs_FixedMemDataPackagesRW ((void *)&u16_FRAM_Data, u16_FRAM_NumBytesToWrite, u16_FRAM_BaseAddress, (bool)(bool)COMMON_CMDS_OWNER__WRITE),
         *pu32_ErrorCodeSW_Sub
      );
   }   
   if (*pu32_ErrorCodeSW_Sub)
      u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__MODBUS_SET_SLAVE_ADDRESS;

   return u16_ErrorCodeSW_Base;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ushort CommonCmdsOwner_DeviceRunTimeCounterReset (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ulong    u32_DeviceRunTimeCounter      =  0x00000000;
   ulong    u32_RTT_PeriodForMsgGen       =  0x0000000A;    /**<  calling corresponding user task periodically every u32_RTT_PeriodForMsgGen * [prescaler / 32768 Hz] */
   ulong    u32_RTT_AlarmValue            =  0x00000000;
   ushort   u16_RTT_Prescaler             =  0x8000;        /**<  RTT increment every x [s] with x = prescaler / 32768 Hz (slow crystal) */
   ushort   u16_ErrorCodeSW_Base          =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   ushort   u16_FRAM_NumBytesRW           =  4;
   uchar    u8_Unit                       =  0;

   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;


   // REstart real-time timer (new RTT calibration!)
   if( SUCCESS( RttRestart(u8_Unit, u16_RTT_Prescaler, u32_RTT_AlarmValue, u32_DeviceRunTimeCounter,
                           u32_RTT_PeriodForMsgGen, cu16_UtilsFuncs_TimeoutMedium),
                *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__RTT__RESTART ) )
   {
      // store ZERO as total device run-time to FRAM
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&u32_DeviceRunTimeCounter), u16_FRAM_NumBytesRW, 
                                                               (ushort)(COMMON_REGISTERS__FRAM_ADDR__DEVICE_RUN_TIME_COUNTER),
                                                               (bool)UTILS_FUNCS__WRITE),
                            *pu32_ErrorCodeSW_Sub))
      {               
         // write ZERO as total run-time to register bank
         if (FAIL (MBPBRegAccess((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, 
                                 (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_RUN_TIME_COUNTER, (ushort)AT91SAM7X__NUM_REGS_2,
                                 (void *)(&u32_DeviceRunTimeCounter), NULL, cu16_UtilsFuncs_TimeoutLong),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__DEVICE_RUN_TIME_COUNTER_RESET;
         }
      }
   }


   return   u16_ErrorCodeSW_Base;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ushort CommonCmdsOwner_ErrorCodeSW_ClearCurr (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   uchar                         u8_Unit;
#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif

   ushort                        u16_NumRegsToRW;
   ushort                        u16_NumErrorsRemain;
//   ushort                        u16_DeviceState;
   ushort                        u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   tS_LibExceptions_Exceptions   S_Exceptions;


   // reset of structure is necessary!
   memset ((void *)(&S_Exceptions), 0, (size_t)sizeof(S_Exceptions));
   // reset passed sub error code
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;


   if (SUCCESS_APP_FUNC (CommonSupportFuncs_ErrorCodeSW_ClearCurrPair (&u16_NumErrorsRemain), *pu32_ErrorCodeSW_Sub))
   {
      if (u16_NumErrorsRemain > 0)
      {
         u16_NumRegsToRW = 1;

         // read out current base error code
         if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ,
                                     (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE1 + (u16_NumErrorsRemain/2) - 1, 
                                      u16_NumRegsToRW, (void *)(&S_Exceptions.u16_ErrorCodeSW_Base), NULL, cu16_UtilsFuncs_TimeoutLong),
                      *pu32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS) )
         {
         }         
            // read out current SUB error code
            if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ,
                                        (ushort)COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB1 + (u16_NumErrorsRemain/2) - 1, 
                                         u16_NumRegsToRW, (void *)(&S_Exceptions.u32_ErrorCodeSW_Sub), NULL, cu16_UtilsFuncs_TimeoutLong),
                         *pu32_ErrorCodeSW_Sub, (ushort)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS) )
            {
            }            
               // search corresponding software base error string and replace the old one if a corresponding a string is found
               TRY_AND_CATCH_APP_FUNC
               (
                  CommonSupportFuncs_ErrorStringSW_FindAndSet (&S_Exceptions),
                  *pu32_ErrorCodeSW_Sub
               );
      }
      // clear software error string and set new device state in case no errors remain
      else
      {
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY

            u8_Unit = 0;
            // set MCP2317_NSYS_ERROR high though no errors left (level = second argument)
            if (SUCCESS (Mcp2317SetOutput (u8_Unit, (ushort)AT91SAM7X__MCP2317_N_SYS_ERROR, (ushort)AT91SAM7X__MCP2317_N_SYS_ERROR,
                                           cu16_UtilsFuncs_TimeoutLong),
                         *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MCP2317__SET_OUTPUT))
#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif
      
            {         
               TRY_AND_CATCH_APP_FUNC
               (
                  CommonSupportFuncs_ErrorStringSW_Clear(), *pu32_ErrorCodeSW_Sub
               );
            }
      }
   }
   
   if (*pu32_ErrorCodeSW_Sub)
      u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__ERROR_CODE_SW_CLEAR_CURR;


   return u16_ErrorCodeSW_Base;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ushort CommonCmdsOwner_ErrorCodeSW_ClearAll (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   uchar    u8_Unit;
#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif
   ushort   u16_NumErrorsRemain  = 1;
//   ushort   u16_DeviceState;
   ushort   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR_NONE;


   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;

   while (u16_NumErrorsRemain > 0)
   {
      if (FAIL_APP_FUNC (CommonSupportFuncs_ErrorCodeSW_ClearCurrPair (&u16_NumErrorsRemain), *pu32_ErrorCodeSW_Sub))
      {
         return u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__ERROR_CODE_SW_CLEAR_ALL;
      }
   }

   // clear current software base error string and set new device state
   if (SUCCESS_APP_FUNC (CommonSupportFuncs_ErrorStringSW_Clear(), *pu32_ErrorCodeSW_Sub))
   {
#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY      
      
         u8_Unit = 0;
         // set MCP2317_NSYS_ERROR high though no errors left (level = second argument)
         TRY_AND_CATCH
         (
            Mcp2317SetOutput (u8_Unit, (ushort)AT91SAM7X__MCP2317_N_SYS_ERROR, (ushort)AT91SAM7X__MCP2317_N_SYS_ERROR,
                              cu16_UtilsFuncs_TimeoutLong),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MCP2317__SET_OUTPUT
         );

#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif      
   }

   if (*pu32_ErrorCodeSW_Sub)
      u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__ERROR_CODE_SW_CLEAR_ALL;


   return u16_ErrorCodeSW_Base;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ushort CommonCmdsOwner_SykamDeviceDataWriteEnable (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR_NONE;


   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;

   // enable write for Sykam device data if write enable code is correct
   if (u16_ModbusRxData == (ushort)COMMON_REGISTERS__SYKAM_DEVICE_DATA_WRITE_ENABLE_CODE)
      u16_CommonRegistersFuncs_SykamDeviceDataWriteEnableCode = u16_ModbusRxData;


   return u16_ErrorCodeSW_Base;
}
