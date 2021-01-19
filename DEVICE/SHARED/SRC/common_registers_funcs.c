/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_registers_funcs.c
*
*  \brief               This file contains functions to initialize common register bank and a consistency check function for
*                       Sykam device data.
*
*  \author              Ruediger Radtke
*  \version             1.3
*
*
*  \date                2011-08-08     Initial version.
*  \date                2012-06-21     Fixed other firm data array sizes with defines for \ref u8A_SykamCfg_Serial array.
*  \date                2012-08-21     <em>File version 1.1</em>. Removed error in initialization of device status data.
*  \date                2012-11-09     <em>File version 1.2</em>. Removed initialization for removed feature "mainboard voltage
*                                      monitoring disabling" feature.
*  \date                2012-11-20     <em>File version 1.3</em>. Changed default active interface to display.
*
*
*  \bug                 2012-06-21     Wrong size of \ref u8A_SykamCfg_Serial array for sykam device data.
*  \bug                 2012-08-21     No zero-init in device status data.
*
*  \test                
*
*  \todo                
*
*			
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright (c) 2008-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include "..\..\..\rtl\support.h"

#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"

#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"

#include "lib_exceptions.h"
#include "common_registers.h"
#include "common_registers_funcs.h"
#include "utils_funcs.h"


//---------------------------------------------- global constants ------------------------------------------------------------------------------

const ushort cu16_CommonRegistersFuncs_DefaultActiveInterface  = (ushort)COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__DISPLAY_KEYS;

//---------------------------------------------- global declarations/definitions ---------------------------------------------------------------

ushort u16_CommonRegistersFuncs_SykamDeviceDataWriteEnableCode = 0;

//---------------------------------------------- externals used in this file -------------------------------------------------------------------

extern            uchar    u8A_SykamCfg_ManufacturerDeviceID[COMMON_REGISTERS__MAX_CHARS__MANUFACTURER_DEVICE_ID];
extern            uchar    u8A_SykamCfg_ManufacturerID[COMMON_REGISTERS__MAX_CHARS__MANUFACTURER_ID];
extern            uchar    u8A_SykamCfg_OEM_DeviceID[COMMON_REGISTERS__MAX_CHARS__OEM_DEVICE_ID];
extern            uchar    u8A_SykamCfg_OEM_ID[COMMON_REGISTERS__MAX_CHARS__OEM_ID];
extern            uchar    u8A_SykamCfg_Serial[COMMON_REGISTERS__MAX_CHARS__SERIAL];
extern            ushort   u16_SykamCfg_Options;

extern   const    ushort   cu16_SykamCfg_DeviceClass;
extern   const    ushort   cu16_SykamCfg_Version;

extern   const    ushort   cu16_UtilsFuncs_TimeoutShort;
extern   const    ushort   cu16_CommonRegistersFuncs_DefaultActiveInterface;

//---------------------------------------------- functions -------------------------------------------------------------------------------------

ulong CommonRegistersFuncs_InitRegisterBank (void)
{
   ulong u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;


   // initialize the common register bank
   if (SUCCESS_APP_FUNC (CommonRegistersFuncs_InitSykamDeviceData(), u32_ErrorCodeSW_Sub))
   {
      if (SUCCESS_APP_FUNC (CommonRegistersFuncs_InitDeviceParams(), u32_ErrorCodeSW_Sub))
      {
         TRY_AND_CATCH_APP_FUNC
         ( 
            CommonRegistersFuncs_InitDeviceStatusData(),
            u32_ErrorCodeSW_Sub
         );
      }
   }

   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong CommonRegistersFuncs_InitSykamDeviceData (void)
{
   struct   _SMBPBRegAccess               S_MBPB_AccessParams;
   tS_CommonRegisters_SykamDeviceData     S_CommonRegisters_SykamDeviceData;

   ulong    u32_ErrorCodeSW_Sub  = (ulong)LIB_EXCEPTIONS__ERROR_NONE;

   
   // disable write for Sykam device data (enable write protection)
   u16_CommonRegistersFuncs_SykamDeviceDataWriteEnableCode = 0;

   memset ((void *)(&S_CommonRegisters_SykamDeviceData), 0, sizeof(S_CommonRegisters_SykamDeviceData));
   
   memcpy ((void *)(& S_CommonRegisters_SykamDeviceData.u8A_ManufacturerDeviceID),  (const void *)(& u8A_SykamCfg_ManufacturerDeviceID), sizeof(u8A_SykamCfg_ManufacturerDeviceID));
   memcpy ((void *)(& S_CommonRegisters_SykamDeviceData.u8A_ManufacturerID),        (const void *)(& u8A_SykamCfg_ManufacturerID),       sizeof(u8A_SykamCfg_ManufacturerID));
   memcpy ((void *)(& S_CommonRegisters_SykamDeviceData.u8A_OEM_DeviceID),          (const void *)(& u8A_SykamCfg_OEM_DeviceID),         sizeof(u8A_SykamCfg_OEM_DeviceID));
   memcpy ((void *)(& S_CommonRegisters_SykamDeviceData.u8A_OEM_ID),                (const void *)(& u8A_SykamCfg_OEM_ID),               sizeof(u8A_SykamCfg_OEM_ID));
   memcpy ((void *)(& S_CommonRegisters_SykamDeviceData.u8A_Serial),                (const void *)(& u8A_SykamCfg_Serial),               sizeof(u8A_SykamCfg_Serial));

   S_CommonRegisters_SykamDeviceData.u16_DeviceClass = cu16_SykamCfg_DeviceClass;
   S_CommonRegisters_SykamDeviceData.u16_Version     = cu16_SykamCfg_Version;
   S_CommonRegisters_SykamDeviceData.u16_Options     = u16_SykamCfg_Options;

   // swap bytes because modbus registers are accessed with 16 bit pointers (this implies a correct left-right human read if data 
   // is read out via modbus)
   if (SUCCESS_APP_FUNC (UtilsFuncs_SwapBytes ((void *)&S_CommonRegisters_SykamDeviceData, (ushort)COMMON_REGISTERS__SYKAM_DEVICE_DATA_CHAR_ARRAYS_TOTAL_WORD_SIZE), 
                         u32_ErrorCodeSW_Sub))
   {
      S_MBPB_AccessParams.ucType     =  (uchar)  MBPB_REGTYPE_HOLDING;
	   S_MBPB_AccessParams.bRW        =  (uchar)  MBPB_REGRW_WRITE;
      S_MBPB_AccessParams.usAddr     =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__START;
	   S_MBPB_AccessParams.usAddrOffs =  (ushort) 0x0000;  
	   S_MBPB_AccessParams.usCount    =  (ushort) (sizeof(S_CommonRegisters_SykamDeviceData)/2); // total words of Common_SykamDeviceData
	   S_MBPB_AccessParams.pvData     =  (void *) (&S_CommonRegisters_SykamDeviceData);  
	   S_MBPB_AccessParams.pvIntern   =  NULL;                                                   // debug purpose, set to &pv_BufferAddress if needed
	   S_MBPB_AccessParams.u16To      =  cu16_UtilsFuncs_TimeoutShort;

      // map the content of params into input registers;
      if (SUCCESS (MBPBRegAccessStruct  ((void*)(&S_MBPB_AccessParams)), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS)  )
      {
         // set product (short) description with device class code and OEM device name string 
         TRY_AND_CATCH
         ( 
            MBPBProductDescSet ((ulong)COMMON_REGISTERS__DC_VALVE, S_CommonRegisters_SykamDeviceData.u8A_OEM_DeviceID, 
                                (uchar)MBPB_PRODUCT_DEVICE_LENGTH, OS_T_FOREVER),
            u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_DESCRIPTION_SET 
         );
      }
   }
   
   if (u32_ErrorCodeSW_Sub)
   {
      u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub  << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__COMMON_REGISTERS_FUNCS__INIT_SYKAM_DEVICE_DATA);
   }

   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong CommonRegistersFuncs_InitDeviceParams (void)
{
//   void  *pv_BufferAddress;    // debug purpose   
   struct   _SMBPBRegAccess                     S_MBPB_AccessParams;    
   struct   _S_CommonRegisters_DeviceParams     S_CommonRegisters_DeviceParams;

   ulong    u32_ErrorCodeSW_Sub  = (ulong)LIB_EXCEPTIONS__ERROR_NONE;


   // set device params
   S_CommonRegisters_DeviceParams.u16_DeviceActiveInterface       =  cu16_CommonRegistersFuncs_DefaultActiveInterface;
   
   S_MBPB_AccessParams.ucType       =  (uchar)  MBPB_REGTYPE_HOLDING;
	S_MBPB_AccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
   S_MBPB_AccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS__START;
	S_MBPB_AccessParams.usAddrOffs   =  (ushort) 0x0000;  
	S_MBPB_AccessParams.usCount      =  (ushort) (sizeof(S_CommonRegisters_DeviceParams)/2);  // total words of S_CommonRegisters_DeviceParams;
	S_MBPB_AccessParams.pvData       =  (void *) (&S_CommonRegisters_DeviceParams);  
	S_MBPB_AccessParams.pvIntern     =  NULL;                                                 // debug purpose, set to &pv_BufferAddress if needed
	S_MBPB_AccessParams.u16To        =  cu16_UtilsFuncs_TimeoutShort;

   // map the content of structure into register bank
   TRY_AND_CATCH
   (
      MBPBRegAccessStruct ((void*)(&S_MBPB_AccessParams)), 
      u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
   );

   if (u32_ErrorCodeSW_Sub)
   {
      u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__COMMON_REGISTERS_FUNCS__INIT_DEVICE_PARAMS);
   }


   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------

ulong CommonRegistersFuncs_InitDeviceStatusData (void)
{
//   void  *pv_BufferAddress;    // debug purpose   
   struct   _SMBPBRegAccess                        S_MBPB_AccessParams;    
   struct   _S_CommonRegisters_DeviceStatusData    S_CommonRegisters_DeviceStatusData;

   ulong    u32_ErrorCodeSW_Sub  = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   
   
   // reset status data structure
   memset ((void*)(&S_CommonRegisters_DeviceStatusData.u32_DeviceRunTimeCounter), 0, sizeof(S_CommonRegisters_DeviceStatusData));
   
   // set device status data
//   S_CommonRegisters_DeviceStatusData.u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__INIT;
//   S_CommonRegisters_DeviceStatusData.u16_DeviceState = (ushort)COMMON_REGISTERS__DEVICE_STATE__IDLE;
   
   S_MBPB_AccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
	S_MBPB_AccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
   S_MBPB_AccessParams.usAddr       =  (ushort) COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START;
	S_MBPB_AccessParams.usAddrOffs   =  (ushort) 0x0000;  
	S_MBPB_AccessParams.usCount      =  (ushort) (sizeof(S_CommonRegisters_DeviceStatusData)/2);    // total words of S_CommonRegisters_DeviceStatusData;
	S_MBPB_AccessParams.pvData       =  (void *) (&S_CommonRegisters_DeviceStatusData);  
	S_MBPB_AccessParams.pvIntern     =  NULL;                                                       // debug purpose, set to &pv_BufferAddress if needed
	S_MBPB_AccessParams.u16To        =  cu16_UtilsFuncs_TimeoutShort;

   // map the content of structure into register bank
   TRY_AND_CATCH
   (
      MBPBRegAccessStruct ((void*)(&S_MBPB_AccessParams)), 
      u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
   );

   if (u32_ErrorCodeSW_Sub)
   {
      u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__COMMON_REGISTERS_FUNCS__INIT_DEVICE_STATUS_DATA);
   }


   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong CommonRegistersFuncs_SykamDeviceDataConsistencyCheck (tpS_CommonRegisters_SykamDeviceData pS_CommonRegisters_SykamDeviceData)
{
   ulong    u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;


   // check Sykam device class and Sykam version for consistency (values must be fixed, i.e. no changes from outside are excepted)
   if(    (pS_CommonRegisters_SykamDeviceData->u16_DeviceClass != cu16_SykamCfg_DeviceClass) 
       || (pS_CommonRegisters_SykamDeviceData->u16_Version != cu16_SykamCfg_Version) )
   {
      u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__COMMON_REGISTERS_FUNCS__SYKAM_DEVICE_DATA_CONSISTENCY_CHECK;             // error case: given Sykam data do not fit to constants
   }


   return u32_ErrorCodeSW_Sub;
}
