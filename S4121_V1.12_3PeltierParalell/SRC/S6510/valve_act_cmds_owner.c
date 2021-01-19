/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_cmds_owner.c
*
*  \brief               This file contains the valve actuator \em owner (manufacturer) command functions which will be executed if
*                       sending the appropriate command via modbus (MBPB). Execution of these functions is only allowed by owner
*                       (manufacturer) or service technician. \n
*                       It is recommended to add new command functions \b only to the end of function pointer array since
*                       there exist a documentation about commands and corresponding modbus addresses. If the array is extended,
*                       have a look at current address boarders in \ref valve_act_registers.h.
*                       Every function returns an software base and sub error code which is greater than zero in error case.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-10     Initial version extracted from S6510_cmd_functions_owner.c
*  \date                2011-11-24     Removed code intern word swaps for 32-bit register bank values.
*  \date                2012-11-06     Removed unused commands "ValveActCmdsOwner_CalDataLoad" and "ValveActCmdsOwner_CalDataLoad".
*                                      Removed security checks for saved calibration data in \ref ValveActCmdsOwner_DeviceParamsLoad,
*                                      \ref ValveActCmdsOwner_DeviceParamsLoadDefault, \ref ValveActCmdsOwner_DeviceParamsSave,
*                                      \ref ValveActCmdsOwner_CalWithDeviceParams and \ref ValveActCmdsOwner_CalWithDefaultParams.
*  \date                2012-11-07     Changed several variable names, adapted functions to use internal and external device
*                                      parameters/status data.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                2011-09-13     Tested new timeouts and commands.
*
*  \todo
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <AT91SAM7X256.H>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\cpu\at91\misc\stpclkdir\stpclkdirext.h"
#include "..\..\..\ext\misc\encgapdisk\encgapdisk.h"
#include "..\..\..\ext\misc\encgapdisk\encgapdiskdataext.h"
#include "..\..\..\ext\misc\encgapdisk\encgapdiskext.h"
#include "..\..\..\ext\spi\nvram\fm25clss\fm25clssext.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\utils_funcs.h"
#include "valve_act_registers.h"
#include "valve_act_registers_funcs.h"
#include "valve_act_cmds_owner.h"
#include "valve_act_cmds_reg.h"
#include "valve_act_support_funcs.h"
#include "valve_act_valvetypes.h"
//---------------------------------------------- globals definitions ---------------------------------------------------------------------------
// The size and order must be the same like in documentation (excel table)!
tfp_ValveActCmdsOwner_CMD   fpA_ValveActCmdsOwner_CMDs[] =
{
   { &ValveActCmdsOwner_DeviceParamsLoad           },
   { &ValveActCmdsOwner_DeviceParamsLoadDefault    },
   { &ValveActCmdsOwner_DeviceParamsSave           },
   { &ValveActCmdsOwner_StatusDataLoad             },
   { &ValveActCmdsOwner_CalWithDeviceParams        },
   { &ValveActCmdsOwner_CalWithDefaultParams       },
   { &ValveActCmdsOwner_ResetTotalSwitchCounter    },
};
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern const   struct   _S_ValveActValveTypes_SykamValves   cSA_SykamValve[VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER];
extern         struct   _SEncGapDiskRunParams               S_EncGapDiskRunParams;
extern const   ushort   cu16_TimeoutShort;
extern const   ushort   cu16_TimeoutLong;
//---------------------------------------------- owner (manufacturer) command functions --------------------------------------------------------
ushort ValveActCmdsOwner_DeviceParamsLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   struct   _S_ValveActRegisters_DeviceParamsExtern   S_DeviceParamsExtern;
   struct   _S_ValveActRegisters_DeviceParamsIntern   S_DeviceParamsIntern;
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   struct   _SEncGapDiskRunParams                     S_EncGapDiskRunParams;
   uchar    u8_Result                  =  0;
   uchar    u8_Unit                    =  0;
   ushort   u16_Result                 =  0;
   ushort   u16_FixedMemNumBytesRw;
   ushort   u16_FixedMemCmpBuffer[2 * (FM25CLSS_NUM_MULTI_STORAGES - 1)];
   ushort   u16_FixedMemData;
   ushort   u16_FixedMemAddress;
   ushort   u16_ErrorCodeSW_Base       =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   ulong    u32_CurrentPosInUSteps;
   // load EXtern device params from fixed memory
   memset ((void *)(&S_DeviceParamsExtern), 0, sizeof(S_DeviceParamsExtern));
   // load intern device params from fixed memory
   memset ((void *)(&S_DeviceParamsIntern), 0, sizeof(S_DeviceParamsIntern));
   *pu32_ErrorCodeSW_Sub   =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   u16_FixedMemNumBytesRw  =  (ushort)AT91SAM7X__NUM_BYTES_2;
   ////////////////////////////////////Read external PARAMETERS/////////////////////////////////////	
   // check if params were stored on fixed memory
   u16_Result = Fm25clssReadSafe (u8_Unit, (void *)(&u16_FixedMemData), (void *)(&u16_FixedMemCmpBuffer), u16_FixedMemNumBytesRw,
   (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_SAVED, cu16_TimeoutLong);
   if (u16_Result)
   {
      if ((u16_FixedMemData != (ushort)VALVE_ACT_CMDS_OWNER__FRAM_CODE__DEVICE_PARAMS_SAVED) || (u16_Result != u16_FixedMemNumBytesRw)) // error case return from function
      {
         *pu32_ErrorCodeSW_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__READ_SAFE;
         return u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_LOAD;
      }
      u16_FixedMemAddress     =  (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__START;
      u16_FixedMemNumBytesRw  =  (ushort)(sizeof(S_DeviceParamsExtern));
      UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_DeviceParamsExtern), u16_FixedMemNumBytesRw, u16_FixedMemAddress, (bool)UTILS_FUNCS__READ);
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_DeviceParamsExtern), u16_FixedMemNumBytesRw, u16_FixedMemAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
         // now set motor params of loaded external device params from fixed memory to deriver internal structure
         S_DeviceParamsExtern.u16_CurrentValveType          =  (ulong)  S_DeviceParamsExtern.u16_CurrentValveType;
         // additional check for external device params
         if (S_DeviceParamsExtern.u16_CurrentValveType >= (ushort)VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER)
         {
            *pu32_ErrorCodeSW_Sub         =  (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_VALVE_TYPE_NOT_SUPPORTED;
            u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_LOAD;
         }
         // now set motor params of loaded external device params from fixed memory to deriver internal structure
         S_DeviceParamsExtern.u16_CurrentValveType          =  (ulong)  S_DeviceParamsExtern.u16_CurrentValveType;
         // set EXternal device params into corresponding modbus registers
         S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_HOLDING;
         S_ModbusAccessParams.bRW         =  (uchar)  MBPB_REGRW_WRITE;
         S_ModbusAccessParams.usAddr      =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START;
         S_ModbusAccessParams.usAddrOffs  =  (ushort) 0x0000;
         S_ModbusAccessParams.usCount     =  (ushort) (sizeof(S_DeviceParamsExtern)/2);      // word size needed!
         S_ModbusAccessParams.pvData      =  (void *) (&S_DeviceParamsExtern);
         S_ModbusAccessParams.pvIntern    =  NULL;                                           // debug purpose, set to a valid pointer if needed
         S_ModbusAccessParams.u16To       =  cu16_TimeoutShort;
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            S_ModbusAccessParams.usAddr      =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START;
            S_ModbusAccessParams.usCount     =  (ushort) (sizeof(S_DeviceParamsExtern)/2);
            S_ModbusAccessParams.pvData      =  (void *) (&S_DeviceParamsExtern);
         }
      }
      ///////////////////////////////////Read Internal PARAMETERS////////////////////////////////////
      u16_FixedMemAddress     =  (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START;
      u16_FixedMemNumBytesRw  =  (ushort)(sizeof(S_DeviceParamsIntern));
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_DeviceParamsIntern), u16_FixedMemNumBytesRw, u16_FixedMemAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {			
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            // test and correct internal device params loaded from fixed memory
            if (SUCCESS_APP_FUNC (ValveActSupportFuncs_TestAndCorrectInternDeviceParams ((void *)&S_DeviceParamsIntern), *pu32_ErrorCodeSW_Sub))
            {
               S_ModbusAccessParams.usAddr      =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START;
               S_ModbusAccessParams.usCount     =  (ushort) (sizeof(S_DeviceParamsIntern)/2);
               S_ModbusAccessParams.pvData      =  (void *) (&S_DeviceParamsIntern);
               // set internal device params into corresponding modbus registers
               if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  // now set motor params of loaded internal device params from fixed memory to deriver internal structure
                  S_EncGapDiskRunParams.ulClksInit          =  (ulong)  S_DeviceParamsIntern.u16_Cal_UStepsInit;
                  S_EncGapDiskRunParams.ulClkInitHz         =  (ulong)  S_DeviceParamsIntern.u16_Cal_USPS_Init;
                  S_EncGapDiskRunParams.ulClkLowHz          =  (ulong)  S_DeviceParamsIntern.u16_Cal_USPS_Start;
                  S_EncGapDiskRunParams.ulClkHighHz         =  (ulong)  S_DeviceParamsIntern.u16_Cal_USPS_Run;
                  S_EncGapDiskRunParams.ulClks              =  (ulong)  S_DeviceParamsIntern.u16_Cal_UStepsFullCircle;
                  S_EncGapDiskRunParams.usClksAcc           =           S_DeviceParamsIntern.u16_Cal_UStepsAcc;
                  S_EncGapDiskRunParams.usClksBrk           =           S_DeviceParamsIntern.u16_Cal_UStepsBreak;
                  S_EncGapDiskRunParams.bContinous          =  (bool)   S_DeviceParamsIntern.u16_Cal_Mode;
                  S_EncGapDiskRunParams.usDacValueHold      =           S_DeviceParamsIntern.u16_DAC_HoldMotorDriver;
                  S_EncGapDiskRunParams.usDacValueRun       =           S_DeviceParamsIntern.u16_DAC_RunMotorDriver;
                  u16_FixedMemNumBytesRw = (ushort)AT91SAM7X__NUM_BYTES_4;
                  // load last used valve actuator position in usteps from fixed memory
                  u16_Result = Fm25clssReadSafe (u8_Unit, (void *)(&u32_CurrentPosInUSteps), (void *)(&u16_FixedMemCmpBuffer),
                  u16_FixedMemNumBytesRw, (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__CURRENT_POS_IN_USTEPS,
                  cu16_TimeoutLong);
                  if (u16_Result != u16_FixedMemNumBytesRw)
                  {
                     *pu32_ErrorCodeSW_Sub       = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__READ_SAFE;
                     u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_LOAD;
                  }
                  // set last engapdisk run param (value is set into SStpClkDirRW)
                  S_EncGapDiskRunParams.u32ClkCntAbsolute = u32_CurrentPosInUSteps;
                  // write encgapdisk run params (true == set/restore data)
                  if (SUCCESS (EncGapDiskRunParamsRW(u8_Unit, (void *)(&S_EncGapDiskRunParams), (bool)TRUE, cu16_TimeoutShort),
                           *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__RUN_PARAMS_RW))
                  {
                     // set flag param data loaded to register and fixed memory
                     TRY_AND_CATCH_APP_FUNC
                     (
                     ValveActSupportFuncs_DeviceParamFlagsRW (( ((uchar)VALVE_ACT_SUPPORT_FUNCS__SET_FLAG_LOADED) | ((uchar)VALVE_ACT_SUPPORT_FUNCS__CLEAR_FLAG_SAVED) ), (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE, &u8_Result ),
                     *pu32_ErrorCodeSW_Sub
                     );
                  }
               }
            }
         }
      }	
   }
   else
   *pu32_ErrorCodeSW_Sub   = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__READ_SAFE;
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_LOAD;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
// This functions loads default device params from MC code into modbus registers. It does NOT load param data from fixed memory.
// It keeps the current active interface and the actual true gap number without setting them to default data.
ushort ValveActCmdsOwner_DeviceParamsLoadDefault (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // use normal init function to restore the default values
   if (SUCCESS_APP_FUNC (ValveActRegistersFuncs_InitDeviceParams(), *pu32_ErrorCodeSW_Sub))
   {
      // set default DAC values to internal structs
      if (FAIL_APP_FUNC (ValveActCmdsReg_DAC_HoldMotorDriver ((ushort)STPCLKDIR0_DAC_HOLD, pu32_ErrorCodeSW_Sub), u16_ErrorCodeSW_Base))
      {
         return u16_ErrorCodeSW_Base;
      }
      if (FAIL_APP_FUNC (ValveActCmdsReg_DAC_RunMotorDriver ((ushort)STPCLKDIR0_DAC_RUN, pu32_ErrorCodeSW_Sub), u16_ErrorCodeSW_Base))
      {
         return u16_ErrorCodeSW_Base;
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_LOAD_DEFAULT;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ValveActCmdsOwner_DeviceParamsSave (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   struct   _S_ValveActRegisters_DeviceParamsExtern   S_DeviceParamsExtern;
   struct   _S_ValveActRegisters_DeviceParamsIntern   S_DeviceParamsIntern;
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   ushort   u16_Result                    =  0;
   uchar    u8_Result                     =  0;
   uchar    u8_Unit                       =  0;
   ushort   u16_FixedMemData;
   ushort   u16_FixedMemAddress;
   ushort   u16_FixedMemNumBytesRw;
   ushort   u16_ErrorCodeSW_Base          =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   ushort   u16_CurrentValveType          =  0;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save external device params to fixed memory
   u16_FixedMemAddress     =  (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__START;
   u16_FixedMemNumBytesRw  =  (ushort)(sizeof(S_DeviceParamsExtern));
   // clear flag param data saved on fixed memory as long as function was not executed in total
   u16_FixedMemData        =  (ushort)0x0000;
   u16_FixedMemNumBytesRw  =  (ushort)AT91SAM7X__NUM_BYTES_2;
   u16_Result              =  Fm25clssWrite (u8_Unit, (void *)(&u16_FixedMemData), u16_FixedMemNumBytesRw, (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_SAVED), cu16_TimeoutLong);
   if (u16_Result != u16_FixedMemNumBytesRw)  // error case, return from function
   {
      *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_SAVE;
   }
   // Read given valve type
   (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
   (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE, (ushort)AT91SAM7X__NUM_REGS_1,
   (void *)(&u16_CurrentValveType), NULL, cu16_TimeoutShort),
   *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS);
   u16_CurrentValveType=u16_CurrentValveType;
   // set modbus access params
   S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_HOLDING;
   S_ModbusAccessParams.bRW         =  (uchar)  MBPB_REGRW_READ;
   S_ModbusAccessParams.usAddr      =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START;
   S_ModbusAccessParams.usAddrOffs  =  (ushort) 0x0000;
   S_ModbusAccessParams.usCount     =  (ushort) (sizeof(S_DeviceParamsExtern)/2);   // word size needed!
   S_ModbusAccessParams.pvData      =  (void *) (&S_DeviceParamsExtern);
   S_ModbusAccessParams.pvIntern    =  NULL;                                        // debug purpose, set to a valid pointer if needed
   S_ModbusAccessParams.u16To       =  cu16_TimeoutShort;
   S_ModbusAccessParams.usAddr   =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START;
   S_ModbusAccessParams.usCount  =  (ushort) (sizeof(S_DeviceParamsExtern)/2);   // word size needed!
   S_ModbusAccessParams.pvData   =  (void *) (&S_DeviceParamsExtern);
   // read the content of EXternal device params from register bank
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_DeviceParamsExtern), u16_FixedMemNumBytesRw, u16_FixedMemAddress, (bool)UTILS_FUNCS__WRITE),
               *pu32_ErrorCodeSW_Sub))
      {
         // set flag param data saved on fixed memory
         u16_FixedMemData        =  (ushort)VALVE_ACT_CMDS_OWNER__FRAM_CODE__DEVICE_PARAMS_SAVED;
         u16_FixedMemNumBytesRw  =  (ushort)AT91SAM7X__NUM_BYTES_2;
         u16_Result              =  Fm25clssWrite (u8_Unit, (void *)(&u16_FixedMemData), u16_FixedMemNumBytesRw,
         (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_SAVED), cu16_TimeoutLong);
         if (u16_Result != u16_FixedMemNumBytesRw)  // error case, return from function
         {
            *pu32_ErrorCodeSW_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
            return u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_SAVE;
         }
         // set flag "param data saved" to register and fixed memory
         if ((SUCCESS_APP_FUNC (ValveActSupportFuncs_DeviceParamFlagsRW( (((uchar)VALVE_ACT_SUPPORT_FUNCS__CLEAR_FLAG_LOADED) | ((uchar)VALVE_ACT_SUPPORT_FUNCS__SET_FLAG_SAVED)), (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE, &u8_Result),
                     *pu32_ErrorCodeSW_Sub)) && (!u8_Result))
         {}
      }
   }
   // clear flag param data saved on fixed memory as long as function was not executed in total
   u16_FixedMemData        =  (ushort)0x0000;
   u16_FixedMemNumBytesRw  =  (ushort)AT91SAM7X__NUM_BYTES_2;
   u16_Result              =  Fm25clssWrite (u8_Unit, (void *)(&u16_FixedMemData), u16_FixedMemNumBytesRw, (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_SAVED), cu16_TimeoutLong);
   if (u16_Result != u16_FixedMemNumBytesRw)  // error case, return from function
   {
      *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_SAVE;
   }
   // set modbus access params
   S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_HOLDING;
   S_ModbusAccessParams.bRW         =  (uchar)  MBPB_REGRW_READ;
   S_ModbusAccessParams.usAddr      =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START;
   S_ModbusAccessParams.usAddrOffs  =  (ushort) 0x0000;
   S_ModbusAccessParams.usCount     =  (ushort) (sizeof(S_DeviceParamsIntern)/2);   // word size needed!
   S_ModbusAccessParams.pvData      =  (void *) (&S_DeviceParamsIntern);
   S_ModbusAccessParams.pvIntern    =  NULL;                                        // debug purpose, set to a valid pointer if needed
   S_ModbusAccessParams.u16To       =  cu16_TimeoutShort;
   // read the content of Internal device params from register bank
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      S_ModbusAccessParams.usAddr   =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START;
      S_ModbusAccessParams.usCount  =  (ushort) (sizeof(S_DeviceParamsIntern)/2);   // word size needed!
      S_ModbusAccessParams.pvData   =  (void *) (&S_DeviceParamsIntern);
      // read the content of internal device params from register bank
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         // save Intern device params to fixed memory
         u16_FixedMemAddress     =  (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START;
         u16_FixedMemNumBytesRw  =  (ushort)(sizeof(S_DeviceParamsIntern));
         if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_DeviceParamsIntern), u16_FixedMemNumBytesRw, u16_FixedMemAddress, (bool)UTILS_FUNCS__WRITE),
                  *pu32_ErrorCodeSW_Sub))
         {
            // save internal device params to fixed memory
            u16_FixedMemAddress     =  (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START;
            u16_FixedMemNumBytesRw  =  (ushort)(sizeof(S_DeviceParamsIntern));
            if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW  ((void *)(&S_DeviceParamsIntern), u16_FixedMemNumBytesRw, u16_FixedMemAddress, (bool)UTILS_FUNCS__WRITE),
                     *pu32_ErrorCodeSW_Sub))
            {
               // set flag param data saved on fixed memory
               u16_FixedMemData        =  (ushort)VALVE_ACT_CMDS_OWNER__FRAM_CODE__DEVICE_PARAMS_SAVED;
               u16_FixedMemNumBytesRw  =  (ushort)AT91SAM7X__NUM_BYTES_2;
               u16_Result              =  Fm25clssWrite (u8_Unit, (void *)(&u16_FixedMemData), u16_FixedMemNumBytesRw,
               (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_SAVED), cu16_TimeoutLong);
               if (u16_Result != u16_FixedMemNumBytesRw)  // error case, return from function
               {
                  *pu32_ErrorCodeSW_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
                  return u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_SAVE;
               }
               // set flag "param data saved" to register and fixed memory
               if ((SUCCESS_APP_FUNC (ValveActSupportFuncs_DeviceParamFlagsRW( (((uchar)VALVE_ACT_SUPPORT_FUNCS__CLEAR_FLAG_LOADED) | ((uchar)VALVE_ACT_SUPPORT_FUNCS__SET_FLAG_SAVED)), (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE, &u8_Result),
                           *pu32_ErrorCodeSW_Sub)) && (!u8_Result))
               {}
            }
         }
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_SAVE;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ValveActCmdsOwner_StatusDataLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   struct   _S_ValveActRegisters_StatusDataExtern     S_StatusDataExtern;
   struct   _S_ValveActRegisters_StatusDataIntern     S_StatusDataIntern;
   struct   _S_ValveActRegisters_StatusDataExtern     S_StatusDataExternFixedMem;
   struct   _S_ValveActRegisters_StatusDataIntern     S_StatusDataInternFixedMem;
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   ushort   u16_FixedMemNumBytesRw;
   ushort   u16_FixedMemAddress;
   ushort   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub   = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   u16_FixedMemAddress     = (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START;
   u16_FixedMemNumBytesRw  = (ushort)(sizeof(S_StatusDataExternFixedMem));
   // load EXternal status data from fixed memory
   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&S_StatusDataExternFixedMem), u16_FixedMemNumBytesRw, u16_FixedMemAddress, (bool)UTILS_FUNCS__READ),
            *pu32_ErrorCodeSW_Sub))
   {
      u16_FixedMemAddress     =  (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START;
      u16_FixedMemNumBytesRw  =  (ushort)(sizeof(S_StatusDataExternFixedMem));
      // load external status data from fixed memory
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&S_StatusDataExternFixedMem), u16_FixedMemNumBytesRw, u16_FixedMemAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
         // set modbus access params
         S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
         S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
         S_ModbusAccessParams.usAddr       =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
         S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);     // word size needed!
         S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
         S_ModbusAccessParams.pvIntern     =  NULL;                                        // debug purpose, set to a valid pointer if needed
         S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
         // load current EXternal status data
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            // set modbus access params
            S_ModbusAccessParams.usAddr       =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);
            S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
            // load current EXternal status data
            if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               // set status data
               // DON'T set calibration and device params flags here because they are set in other appropriate command functions.
               // This applies also to valve actuator state.
               S_StatusDataExtern.u32_TotalSwitchCounter       =  S_StatusDataExternFixedMem.u32_TotalSwitchCounter;
               S_StatusDataExtern.u16_ValveActState           =  S_StatusDataExternFixedMem.u16_ValveActState;
               S_StatusDataExtern.u16_ValidPositions           =  S_StatusDataExternFixedMem.u16_ValidPositions;
               S_StatusDataExtern.u16_CurrentPosition          =  S_StatusDataExternFixedMem.u16_CurrentPosition;
               // set status data into corresponding modbus inputs
               S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
               S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
               S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);
               S_ModbusAccessParams.usAddr       =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
               S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
               S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
               S_ModbusAccessParams.pvIntern     =  NULL;
               S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
               if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
                        *pu32_ErrorCodeSW_Sub,(ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataExtern)/2);
                  S_ModbusAccessParams.usAddr       =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
                  S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataExtern);
                  TRY_AND_CATCH
                  (
                  MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
                  *pu32_ErrorCodeSW_Sub,
                  (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
                  );
               }
            }
         }
      }
   }
   // load Internal status data from fixed memory
   if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&S_StatusDataInternFixedMem), u16_FixedMemNumBytesRw, u16_FixedMemAddress, (bool)UTILS_FUNCS__READ),
            *pu32_ErrorCodeSW_Sub))
   {
      u16_FixedMemAddress     =  (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START;
      u16_FixedMemNumBytesRw  =  (ushort)(sizeof(S_StatusDataInternFixedMem));
      // load internal status data from fixed memory
      if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(&S_StatusDataInternFixedMem), u16_FixedMemNumBytesRw, u16_FixedMemAddress, (bool)UTILS_FUNCS__READ),
               *pu32_ErrorCodeSW_Sub))
      {
         // set modbus access params
         S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
         S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_READ;
         S_ModbusAccessParams.usAddr       =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
         S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
         S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);     // word size needed!
         S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
         S_ModbusAccessParams.pvIntern     =  NULL;                                        // debug purpose, set to a valid pointer if needed
         S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
         // load current Internal status data
         if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            // set modbus access params
            S_ModbusAccessParams.usAddr       =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
            S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
            S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
            // load current Internal status data
            if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               // set status data
               // DON'T set calibration and device params flags here because they are set in other appropriate command functions.
               // This applies also to valve actuator state.
               S_StatusDataIntern.u32_CurrentPosInUSteps       =  S_StatusDataInternFixedMem.u32_CurrentPosInUSteps;
               S_StatusDataIntern.u16_CurrentEncoderEdge       =  S_StatusDataInternFixedMem.u16_CurrentEncoderEdge;
               S_StatusDataIntern.u16_CalibrationFinished      =  S_StatusDataInternFixedMem.u16_CalibrationFinished;
               S_StatusDataIntern.u16_DeviceParamsSaved        =  S_StatusDataInternFixedMem.u16_DeviceParamsSaved;
               S_StatusDataIntern.u16_DeviceParamsLoaded       =  S_StatusDataInternFixedMem.u16_DeviceParamsLoaded;
               S_StatusDataIntern.u16_StatusDataLoaded         =  0x0001;  // set status flag 'status data loaded'
               // set status data into corresponding modbus inputs
               S_ModbusAccessParams.ucType       =  (uchar)  MBPB_REGTYPE_INPUT;
               S_ModbusAccessParams.bRW          =  (uchar)  MBPB_REGRW_WRITE;
               S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
               S_ModbusAccessParams.usAddr       =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
               S_ModbusAccessParams.usAddrOffs   =  (ushort) 0x0000;
               S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
               S_ModbusAccessParams.pvIntern     =  NULL;
               S_ModbusAccessParams.u16To        =  cu16_TimeoutShort;
               if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
                        *pu32_ErrorCodeSW_Sub,(ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  S_ModbusAccessParams.usCount      =  (ushort) (sizeof(S_StatusDataIntern)/2);
                  S_ModbusAccessParams.usAddr       =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
                  S_ModbusAccessParams.pvData       =  (void *) (&S_StatusDataIntern);
                  TRY_AND_CATCH
                  (
                  MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
                  *pu32_ErrorCodeSW_Sub,
                  (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
                  );
               }
            }
         }
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__STATUS_DATA_LOAD;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ValveActCmdsOwner_CalWithDeviceParams (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   struct   _S_ValveActRegisters_DeviceParamsIntern   S_DeviceParamsIntern;
   uchar    u8_HomeEdgeOffset;
   uchar    u8_Result                           = 0;
   uchar    u8_Unit                             = 0;
   ushort   u16_ValveActState;
   ushort   u16_CurrentEncoderEdge;
   ushort   u16_CurrentPosition;
   ulong    u32_EncGapDisk_ScanAndVerifyCycles  = (ulong)ENCGAPDISK0_SCAN_CYCLES + (ulong)ENCGAPDISK0_VERIFY_CYCLES;
   ushort   u16_ErrorCodeSW_Base                = (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   u16_ValveActState = (ushort)VALVE_ACT_REGISTERS__VALVE_ACT_STATE__CALIBRATION;
   // set system state to state "calibration"
   if (SUCCESS_APP_FUNC (ValveActSupportFuncs_ValveActStateRW((ushort *)&u16_ValveActState, (bool)VALVE_ACT_SUPPORT_FUNCS__WRITE), *pu32_ErrorCodeSW_Sub))
   {
      // get whole content of current device params; use struct S_DeviceParams to avoid multiple single access
      if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
                  (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START, (ushort)(sizeof(S_DeviceParamsIntern)/2),
                  (void *)(&S_DeviceParamsIntern), NULL, cu16_TimeoutShort),
               *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         // DAC values of S_DeviceParamsIntern are set by engapdisk module and implicitly used by EncGapDiskTeachIn
         // Execute calibration with user data!
         S_DeviceParamsIntern.u16_Cal_UStepsInit             =S_DeviceParamsIntern.u16_Cal_UStepsInit             ;
         S_DeviceParamsIntern.u16_Cal_USPS_Init              =S_DeviceParamsIntern.u16_Cal_USPS_Init              ;
         S_DeviceParamsIntern.u16_Cal_UStepsAcc              =S_DeviceParamsIntern.u16_Cal_UStepsAcc              ;
         S_DeviceParamsIntern.u16_Cal_UStepsBreak            =S_DeviceParamsIntern.u16_Cal_UStepsBreak            ;
         S_DeviceParamsIntern.u16_Cal_USPS_Start             =S_DeviceParamsIntern.u16_Cal_USPS_Start             ;
         S_DeviceParamsIntern.u16_Cal_USPS_Run               =S_DeviceParamsIntern.u16_Cal_USPS_Run               ;
         S_DeviceParamsIntern.u16_Cal_Mode                   =S_DeviceParamsIntern.u16_Cal_Mode                   ;
         S_DeviceParamsIntern.u16_Cal_UStepsFullCircle       =S_DeviceParamsIntern.u16_Cal_UStepsFullCircle       ;
         S_DeviceParamsIntern.u16_Cal_TrueGapNumber          =S_DeviceParamsIntern.u16_Cal_TrueGapNumber          ;
         S_DeviceParamsIntern.u16_Cal_MinUStepsThreshold     =S_DeviceParamsIntern.u16_Cal_MinUStepsThreshold     ;
         S_DeviceParamsIntern.u16_Cal_EdgeTypeTriggeredOn    =S_DeviceParamsIntern.u16_Cal_EdgeTypeTriggeredOn    ;
         S_DeviceParamsIntern.u16_Cal_Tol_UStepsSameGapsCal  =S_DeviceParamsIntern.u16_Cal_Tol_UStepsSameGapsCal  ;
         S_DeviceParamsIntern.u16_Cal_Tol_UStepsSameGapsRun  =S_DeviceParamsIntern.u16_Cal_Tol_UStepsSameGapsRun  ;
         S_DeviceParamsIntern.u16_DAC_HoldMotorDriver        =S_DeviceParamsIntern.u16_DAC_HoldMotorDriver        ;
         S_DeviceParamsIntern.u16_DAC_RunMotorDriver         =S_DeviceParamsIntern.u16_DAC_RunMotorDriver         ;
         // set DAC default values
         if (S_DeviceParamsIntern.u16_DAC_HoldMotorDriver<1)
         (ValveActCmdsReg_DAC_HoldMotorDriver((ushort)STPCLKDIR0_DAC_HOLD, pu32_ErrorCodeSW_Sub), u16_ErrorCodeSW_Base);
         if (S_DeviceParamsIntern.u16_DAC_RunMotorDriver<1)
         (ValveActCmdsReg_DAC_RunMotorDriver((ushort)STPCLKDIR0_DAC_RUN, pu32_ErrorCodeSW_Sub), u16_ErrorCodeSW_Base);		
         if (SUCCESS (EncGapDiskTeachIn (          u8_Unit,
                     (uchar)   S_DeviceParamsIntern.u16_Cal_EdgeTypeTriggeredOn,
                     (bool)    S_DeviceParamsIntern.u16_Cal_Mode,
                     (uchar)   S_DeviceParamsIntern.u16_Cal_TrueGapNumber,
                     (ulong)   S_DeviceParamsIntern.u16_Cal_UStepsFullCircle,
                     S_DeviceParamsIntern.u16_Cal_UStepsAcc,
                     S_DeviceParamsIntern.u16_Cal_UStepsBreak,
                     (ulong)   S_DeviceParamsIntern.u16_Cal_USPS_Start,
                     (ulong)   S_DeviceParamsIntern.u16_Cal_USPS_Run,
                     (ulong)   S_DeviceParamsIntern.u16_Cal_UStepsInit,
                     (ulong)   S_DeviceParamsIntern.u16_Cal_USPS_Init,
                     (uchar)   S_DeviceParamsIntern.u16_Cal_MinUStepsThreshold,
                     S_DeviceParamsIntern.u16_Cal_Tol_UStepsSameGapsCal,
                     S_DeviceParamsIntern.u16_Cal_Tol_UStepsSameGapsRun,
                     (ushort)  STPCLKDIR0_CLKSMICRO,
                     cu16_TimeoutShort),
                  *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__TEACH_IN) )
         {
            // wait if motor movement has not finished yet
            if (SUCCESS (StpClkDirWait (u8_Unit, OS_T_MS(AT91SAM7X__STPCLKDIRWAIT_TIMEOUT_MS)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__WAIT))
            {
               // switch to other tasks and return immediately
               os_dly_wait ((ushort)AT91SAM7X__TICK_1000);
               // get the offset from actual position to home pattern (three gaps) --> offset is saved in u8_HomeEdgeOffset as number of edges
               if (SUCCESS (EncGapDiskHomeEdgeGet (u8_Unit, (uchar)S_DeviceParamsIntern.u16_Cal_EdgeTypeTriggeredOn,
                           u32_EncGapDisk_ScanAndVerifyCycles, (uchar)ENCGAPDISK0_MIN_DIST_REPEAT_CNT,
                           TRUE, &u8_HomeEdgeOffset, cu16_TimeoutShort),
                        *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__HOME_EDGE_GET))
               {
                  // set new offset home position index
                  if (SUCCESS (EncGapDiskEdgeZeroSet (u8_Unit, (uchar)S_DeviceParamsIntern.u16_Cal_EdgeTypeTriggeredOn,
                              u8_HomeEdgeOffset, (signed char)-1, cu16_TimeoutShort),
                           *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_ZERO_SET))
                  {
                     // move valve to new home position
                     u16_CurrentEncoderEdge  =  0;
                     u16_CurrentPosition     =  1;
                     // wait for finishing former movements + time for task execution.
                     if (SUCCESS (StpClkDirWait (u8_Unit, OS_T_MS(AT91SAM7X__STPCLKDIRWAIT_TIMEOUT_MS)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__WAIT))
                     {
                        if (SUCCESS (EncGapDiskEdgeGoto  (u8_Unit, (uchar)u16_CurrentEncoderEdge,
                                    (uchar)S_DeviceParamsIntern.u16_Cal_EdgeTypeTriggeredOn,
                                    (ushort)STPCLKDIR0_CLKSMICRO, cu16_TimeoutShort),
                                 *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_GOTO))
                        {
                           // switch to other tasks and return immediately
                           os_dly_wait ((ushort)AT91SAM7X__TICK_1);
                           // write current valve position/position index as well as motor position to registers and fixed memory
                           if (SUCCESS_APP_FUNC (ValveActSupportFuncs_StatusDataSet(&u16_CurrentEncoderEdge, &u16_CurrentPosition),
                                    *pu32_ErrorCodeSW_Sub))
                           {
                              // set flag calibration finished
                              if (SUCCESS_APP_FUNC (ValveActSupportFuncs_CalibrationFlagsRW  ((uchar)VALVE_ACT_SUPPORT_FUNCS__SET_FLAG_FINISHED, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE, &u8_Result),
                                       *pu32_ErrorCodeSW_Sub))
                              {
                                 // set system state to state "idle"
                                 u16_ValveActState = (ushort)VALVE_ACT_REGISTERS__VALVE_ACT_STATE__IDLE;
                                 TRY_AND_CATCH_APP_FUNC
                                 (
                                 ValveActSupportFuncs_ValveActStateRW ((ushort *)&u16_ValveActState, (bool)VALVE_ACT_SUPPORT_FUNCS__WRITE), *pu32_ErrorCodeSW_Sub
                                 );
                              }
                           }
                        }
                     }
                  }
               }
            }
         }         			
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   {
      u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__CAL_WITH_DEVICE_PARAMS;
      pu32_ErrorCodeSW_Sub = pu32_ErrorCodeSW_Sub;
   }
   else
   {
      u16_ErrorCodeSW_Base = 0;
      pu32_ErrorCodeSW_Sub = 0;
   }
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ValveActCmdsOwner_CalWithDefaultParams (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   uchar    u8_HomeEdgeOffset                   = 0;
   uchar    u8_Result                           = 0;
   uchar    u8_Unit                             = 0;
   uchar    u16_ValveActState;
   ushort   u16_CurrentEncoderEdge;
   ushort   u16_CurrentPosition;
   ulong    u32_EncGapDisk_ScanAndVerifyCycles  = (ulong)ENCGAPDISK0_SCAN_CYCLES + (ulong)ENCGAPDISK0_VERIFY_CYCLES;
   ushort   u16_ErrorCodeSW_Base                = (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // set system state to state "calibration"
   u16_ValveActState = (ushort)VALVE_ACT_REGISTERS__VALVE_ACT_STATE__CALIBRATION;
   if (SUCCESS_APP_FUNC (ValveActSupportFuncs_ValveActStateRW((ushort *)(&u16_ValveActState), (bool)VALVE_ACT_SUPPORT_FUNCS__WRITE), *pu32_ErrorCodeSW_Sub))
   {
      // set DAC default values
      if (SUCCESS_APP_FUNC (ValveActCmdsReg_DAC_HoldMotorDriver((ushort)STPCLKDIR0_DAC_HOLD, pu32_ErrorCodeSW_Sub), u16_ErrorCodeSW_Base))
      {
         if (SUCCESS_APP_FUNC (ValveActCmdsReg_DAC_RunMotorDriver((ushort)STPCLKDIR0_DAC_RUN, pu32_ErrorCodeSW_Sub), u16_ErrorCodeSW_Base))
         {
            // execute calibration with default data
            if (SUCCESS (EncGapDiskTeachIn (          u8_Unit,
                        (uchar)  ENCGAPDISK0_DEFAULT_EDGE_TYPE,
                        (bool)   ENCGAPDISK0_CONTINUOUS,
                        (uchar)  ENCGAPDISK0_TRUE_GAP_NUMBER,
                        (ulong)  ENCGAPDISK0_CLKS,
                        (ushort) ENCGAPDISK0_CLKSACC,
                        (ushort) ENCGAPDISK0_CLKSBRK,
                        (ulong)  ENCGAPDISK0_CLKLOWHZ,
                        (ulong)  ENCGAPDISK0_CLKHIGHHZ,
                        (ulong)  ENCGAPDISK0_CLKSINIT,
                        (ulong)  ENCGAPDISK0_CLKHIGHHZ,
                        (uchar)  ENCGAPDISK0_THRESHOLD,
                        (ushort) ENCGAPDISK0_TOLERANCESTEPS,
                        (ushort) ENCGAPDISK0_GAPDRIFTMAX,
                        (ushort) STPCLKDIR0_CLKSMICRO,
                        cu16_TimeoutShort ),
                     *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__TEACH_IN ) )
            {
               // wait if motor movement has not finished yet
               if (SUCCESS (StpClkDirWait (u8_Unit, OS_T_MS(AT91SAM7X__STPCLKDIRWAIT_TIMEOUT_MS)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__WAIT))
               {
                  // switch to other tasks and return immediately
                  os_dly_wait ((ushort)AT91SAM7X__TICK_1);
                  // get the offset from actual position to home pattern (three gaps) --> offset is saved in u8_HomeEdgeOffset as number of edges
                  if (SUCCESS (EncGapDiskHomeEdgeGet (u8_Unit, (uchar)ENCGAPDISK0_DEFAULT_EDGE_TYPE, u32_EncGapDisk_ScanAndVerifyCycles,
                              (uchar)ENCGAPDISK0_MIN_DIST_REPEAT_CNT, TRUE, &u8_HomeEdgeOffset, cu16_TimeoutShort),
                           *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__HOME_EDGE_GET) )
                  {
                     // set new offset home position index
                     if (SUCCESS (EncGapDiskEdgeZeroSet  (u8_Unit, (uchar)ENCGAPDISK0_DEFAULT_EDGE_TYPE,
                                 u8_HomeEdgeOffset, (signed char)-1, cu16_TimeoutShort),
                              *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_ZERO_SET) )
                     {
                        // move valve to new home position
                        u16_CurrentEncoderEdge  =  0;
                        u16_CurrentPosition     =  1;
                        // Wait for finishing former movements + time for task execution. At maximum speed 409 ms are needed for a full circle.
                        if (SUCCESS (StpClkDirWait (u8_Unit, OS_T_MS(AT91SAM7X__STPCLKDIRWAIT_TIMEOUT_MS)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__WAIT))
                        {
                           if (SUCCESS (EncGapDiskEdgeGoto (u8_Unit, (uchar)u16_CurrentEncoderEdge,
                                       (uchar)ENCGAPDISK0_DEFAULT_EDGE_TYPE, cu16_TimeoutShort,
                                       STPCLKDIR0_CLKSMICRO),
                                    *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_GOTO))
                           {
                              // switch to other tasks and return immediately
                              os_dly_wait ((ushort)AT91SAM7X__TICK_1);
                              // write current valve position/position index as well as motor position to registers and fixed memory ------> maybe change to no arg's
                              if (SUCCESS_APP_FUNC (ValveActSupportFuncs_StatusDataSet(&u16_CurrentEncoderEdge, &u16_CurrentPosition),
                                       *pu32_ErrorCodeSW_Sub))
                              {
                                 // set flag calibration finished
                                 if (SUCCESS_APP_FUNC (ValveActSupportFuncs_CalibrationFlagsRW ((uchar)VALVE_ACT_SUPPORT_FUNCS__SET_FLAG_FINISHED, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE, &u8_Result),
                                          *pu32_ErrorCodeSW_Sub) )
                                 {
                                    // set system state to state "idle"
                                    u16_ValveActState = (ushort)VALVE_ACT_REGISTERS__VALVE_ACT_STATE__IDLE;
                                    TRY_AND_CATCH_APP_FUNC
                                    (
                                    ValveActSupportFuncs_ValveActStateRW ((ushort *)&u16_ValveActState, (bool)VALVE_ACT_SUPPORT_FUNCS__WRITE), *pu32_ErrorCodeSW_Sub
                                    );
                                 }
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__CAL_WITH_DEFAULT_PARAMS;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ValveActCmdsOwner_ResetTotalSwitchCounter (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ulong    u32_TotalSwitchCounter     =  0x00000000;
   uchar    u8_Unit                    =  0;
   ushort   u16_FixedMemNumBytesRw     =  (ushort)AT91SAM7X__NUM_BYTES_4;
   ushort   u16_ErrorCodeSW_Base       =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   ushort   u16_Result;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // store number total switching operations to fixed memory
   u16_Result = Fm25clssWrite (u8_Unit, (void *)(&u32_TotalSwitchCounter), u16_FixedMemNumBytesRw,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__TOTAL_SWITCH_COUNTER), cu16_TimeoutLong);
   if (u16_Result != u16_FixedMemNumBytesRw)  // error case
   {
      *pu32_ErrorCodeSW_Sub   = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      u16_ErrorCodeSW_Base    = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__RESET_TOTAL_SWITCH_COUNTER;
   }
   else
   {
      // write ZERO as total switching operations to register bank
      if (FAIL (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE,
                  (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__TOTAL_SWITCH_COUNTER, (ushort)AT91SAM7X__NUM_REGS_2,
                  (void *)(&u32_TotalSwitchCounter), NULL, cu16_TimeoutShort),
               *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__RESET_TOTAL_SWITCH_COUNTER;
      }
   }
   return   u16_ErrorCodeSW_Base;
}
