/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_cmds_reg.c
*
*  \brief               This file contains the \em register command functions which will be executed automatically if someone 
*                       writes into an appropriate <b>holding register</b> of register bank. Therefore owner (manufacturer), 
*                       service technician or user can call this functions indirectly. Details about register bank holding
*                       register are externally documentated. \n
*                       Every function returns an software base and sub error code in case any error occured.
*                       It is recommended to add new register command functions in function pointer arrays \b only to the 
*                       correponding structure position. That means at moment the structure \ref _S_CommonRegisters_DeviceParams
*                       has entries in in a certein order to which fit holding register addresses for common device params in the 
*                       same order. It is compulsory to set new function addresses in function pointer arrays of this file into the
*                       same position and fill emtpy fields with dummy function \ref UtilsFuncs_Dummy.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-07-29     Initial version.
*  \date                2011-08-01     Added dummy function in \ref fpA_CommonCmdsReg_REGCMDs_DeviceParams
*  \date                2012-11-09     Removed dummy entry from \ref fpA_CommonCmdsReg_REGCMDs_DeviceParams.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                2011-08-01     Tested execution of dummy function in \ref fpA_CommonCmdsReg_REGCMDs_DeviceParams.
*
*  \todo                
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include "..\..\..\rtl\support.h"

#include "..\..\..\ext\spi\nvram\fm25clss\fm25clssext.h"

#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"

#include "lib_exceptions.h"
#include "common_cmds_reg.h"
#include "common_registers.h"
#include "utils_funcs.h"


//---------------------------------------------- globals definitions ---------------------------------------------------------------------------

// The follwing register command functions are called if corresponding holding registers are written. The size and order 
// of the function pointer array must be the same as the corresponding structure!

// Reason:  Moderate case (wrong order):  The manufacturer/service technician changes a device param via modbus
//                                        -> the wrong function is called (addresses must be the same). 
//          Bad case (wronge size):       No corresponding entry in function pointer array is found 
//                                        -> system might crash.

// For variables which shall be not linked with a register command function set a dummy function.


// define register function pointers
tfp_CommonCmdsReg_REGCMD   fpA_CommonCmdsReg_REGCMDs_DeviceParams[sizeof(struct _S_CommonRegisters_DeviceParams)/2] = 
{
   {  &CommonCmdsReg_DeviceActiveInterface            },
};

//---------------------------------------------- externals used in this file -------------------------------------------------------------------

extern const   ushort   cu16_UtilsFuncs_TimeoutLong;

//---------------------------------------------- command functions activated through changed params --------------------------------------------

ushort CommonCmdsReg_DeviceActiveInterface( ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub )
{
   ushort   u16_Result                 =  0;
   uchar    u8_Unit                    =  0;
   ushort   u16_DeviceActiveInterface  =  u16_ModbusRxData;
   ushort   u16_FRAM_NumBytesToWrite   =  2;
   ulong    u16_ErrorCodeSW_Base       =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;


   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   u16_Result=u16_Result;
   // store new device interface to FRAM (correponding register was allready written by driver)
   u16_Result = Fm25clssWrite (u8_Unit, (void *)(&u16_DeviceActiveInterface), u16_FRAM_NumBytesToWrite, 
                               (ushort)(COMMON_REGISTERS__FRAM_ADDR__DEVICE_ACTIVE_INTERFACE), cu16_UtilsFuncs_TimeoutLong);

  /* if (u16_Result != u16_FRAM_NumBytesToWrite)
   {
      *pu32_ErrorCodeSW_Sub  = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_REG__DEVICE_ACTIVE_INTERFACE;
   }*/


   return u16_ErrorCodeSW_Base;
}

//----------------------------------------------------------------------------------------------------------------------------------------------
