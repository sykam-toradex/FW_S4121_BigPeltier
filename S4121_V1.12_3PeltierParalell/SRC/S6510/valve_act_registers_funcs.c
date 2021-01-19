/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_registers_funcs.c
*
*  \brief               This file initializes the valve actuator register bank. Initialization comprises device parameter and status
*                       data. \n For the current valve type a default value is set which is defined in \ref valve_act_valvetypes.c. \n
*                       Defaults for remaining device params are also set. That's to be able to calibrate device in case no parameters
*                       can be load from permanent memory. \n Every function returns a software sub error code which is greater than
*                       zero in error case.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2010-06-18     Initial version
*	\date						2011-02-03 		Imported S6510_registers.c.
*	\date						2012-11-07 		Adapted init functions to external and internal device parameters/status data.
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
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include <AT91SAM7X256.H>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\..\ext\misc\encgapdisk\encgapdisk.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "valve_act_registers.h"
#include "valve_act_registers_funcs.h"
#include "valve_act_support_funcs.h"
#include "valve_act_valvetypes.h"
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern   const   ushort   cu16_ValveActValveTypes_DefaultValveType;
extern   const   ushort   cu16_TimeoutShort;
//---------------------------------------------- functions -------------------------------------------------------------------------------------
ulong ValveActRegistersFuncs_InitDeviceParams  (void)
{
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   struct   _S_ValveActRegisters_DeviceParamsExtern   S_DeviceParamsExtern;
   struct   _S_ValveActRegisters_DeviceParamsIntern   S_DeviceParamsIntern;
   ulong    u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // ENCGAPDISK0... values defined in AT91SAM7X.h
   S_DeviceParamsIntern.u16_Cal_UStepsInit             =  (ushort)ENCGAPDISK0_CLKSINIT;          // usteps to prepare motor movement before calibration starts
   S_DeviceParamsIntern.u16_Cal_USPS_Init              =  (ushort)ENCGAPDISK0_CLKINITHZ;         // motor speed in usteps/[s] before calibration starts
   S_DeviceParamsIntern.u16_Cal_UStepsAcc              =  (ushort)ENCGAPDISK0_CLKSACC;           // total usteps for acceleration ramp
   S_DeviceParamsIntern.u16_Cal_UStepsBreak            =  (ushort)ENCGAPDISK0_CLKSBRK;           // total usteps for breaking ramp
   S_DeviceParamsIntern.u16_Cal_USPS_Start             =  (ushort)ENCGAPDISK0_CLKLOWHZ;          // start(end) "frequency" of step motor for acceleration/breaking ramp
   S_DeviceParamsIntern.u16_Cal_USPS_Run               =  (ushort)ENCGAPDISK0_CLKHIGHHZ;         // run "frequency" of step motor -> 28160 usteps/[s]
   S_DeviceParamsIntern.u16_Cal_Mode                   =  (ushort)ENCGAPDISK0_CONTINUOUS;        // continuous driven motor (TRUE) or left-right (FALSE) for mechanical stop bar
   S_DeviceParamsIntern.u16_Cal_UStepsFullCircle       =  (ushort)ENCGAPDISK0_CLKS;              // clocks (microsteps) for a full circle
   S_DeviceParamsIntern.u16_Cal_TrueGapNumber          =  (ushort)ENCGAPDISK0_TRUE_GAP_NUMBER;   // amount of gaps in encoder gap disk
   S_DeviceParamsIntern.u16_Cal_MinUStepsThreshold     =  (ushort)ENCGAPDISK0_THRESHOLD;         // minimum amount of usteps between two neighboured gaps
   S_DeviceParamsIntern.u16_Cal_EdgeTypeTriggeredOn    =  (ushort)ENCGAPDISK_EDGE_FALL;          // edge type to trigger on during calibration and movement
   S_DeviceParamsIntern.u16_Cal_Tol_UStepsSameGapsCal  =  (ushort)ENCGAPDISK0_TOLERANCESTEPS;    // total deviance in usteps for SAME gaps during calibration (scan+averaging)
   S_DeviceParamsIntern.u16_Cal_Tol_UStepsSameGapsRun  =  (ushort)ENCGAPDISK0_GAPDRIFTMAX;       // total deviance in usteps for SAME gaps during calibration verify (run mode not implemented)
   S_DeviceParamsIntern.u16_DAC_HoldMotorDriver        =  (ushort)STPCLKDIR0_DAC_HOLD;           // DAC data input to hold motor driver
   S_DeviceParamsIntern.u16_DAC_RunMotorDriver         =  (ushort)STPCLKDIR0_DAC_RUN;            // DAC data input to run motor driver
   S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_HOLDING;
   S_ModbusAccessParams.bRW         =  (uchar)  MBPB_REGRW_WRITE;
   S_ModbusAccessParams.usAddr      =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START;
   S_ModbusAccessParams.usAddrOffs  =  (ushort) 0x0000;
   S_ModbusAccessParams.usCount     =  (ushort) (sizeof(S_DeviceParamsIntern)/2);     // total words of S_DeviceParamsIntern;
   S_ModbusAccessParams.pvData      =  (void *) (&S_DeviceParamsIntern);
   S_ModbusAccessParams.pvIntern    =  NULL;                                          // debug purpose, set to a valid pointer if needed
   S_ModbusAccessParams.u16To       =  cu16_TimeoutShort;
   // map the content of params into intern device params holding registers
   if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // set register bank values
      S_DeviceParamsExtern.u16_CurrentValveType = cu16_ValveActValveTypes_DefaultValveType;
      // map the content of params into EXtern device params holding registers
      S_ModbusAccessParams.usAddr      =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START;
      S_ModbusAccessParams.usCount     =  (ushort) (sizeof(S_DeviceParamsExtern)/2);  // total words of S_DeviceParamsExtern;
      S_ModbusAccessParams.pvData      =  (void *) (&S_DeviceParamsExtern);
      TRY_AND_CATCH
      (
      MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
      u32_ErrorCodeSW_Sub,
      (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
      );
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_REGISTERS_FUNCS__INIT_DEVICE_PARAMS);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ValveActRegistersFuncs_InitStatusData (void)
{
   struct   _SMBPBRegAccess                           S_ModbusAccessParams;
   struct   _S_ValveActRegisters_StatusDataExtern     S_StatusDataExtern;
   struct   _S_ValveActRegisters_StatusDataIntern     S_StatusDataIntern;
   ushort   u16_Positions;
   ulong    u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // get valve positions of default valve type --> if load of calibration data and params fails, calibration is executed
   // with valve type which was set in 'ValveActRegisters_InitDeviceParams'
   if (SUCCESS_APP_FUNC (ValveActSupportFuncs_PositionsValidGet (cu16_ValveActValveTypes_DefaultValveType, &u16_Positions),
            u32_ErrorCodeSW_Sub))
   {
      S_StatusDataExtern.u32_TotalSwitchCounter       =  0x00000000;
      S_StatusDataExtern.u16_ValveActState            =  (ushort)VALVE_ACT_REGISTERS__VALVE_ACT_STATE__IDLE;
      S_StatusDataExtern.u16_ValidPositions           =  u16_Positions;
      S_StatusDataExtern.u16_CurrentPosition          =  0x0001;
      S_ModbusAccessParams.ucType      =  (uchar)  MBPB_REGTYPE_INPUT;
      S_ModbusAccessParams.bRW         =  (uchar)  MBPB_REGRW_WRITE;
      S_ModbusAccessParams.usAddr      =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START;
      S_ModbusAccessParams.usAddrOffs  =  (ushort) 0x0000;
      S_ModbusAccessParams.usCount     =  (ushort) (sizeof(S_StatusDataExtern)/2);     // total words of S_StatusDataExtern;
      S_ModbusAccessParams.pvData      =  (void *) (&S_StatusDataExtern);
      S_ModbusAccessParams.pvIntern    =  NULL;                                        // debug purpose, set to a valid pointer if needed
      S_ModbusAccessParams.u16To       =  cu16_TimeoutShort;
      // map the content of params into input registers
      if (SUCCESS (MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         // set register bank values
         S_StatusDataIntern.u32_CurrentPosInUSteps       =  0x00000000;
         S_StatusDataIntern.u16_CurrentEncoderEdge       =  0x0000;
         S_StatusDataIntern.u16_CalibrationFinished      =  0x0000;
         S_StatusDataIntern.u16_DeviceParamsSaved        =  0x0000;
         S_StatusDataIntern.u16_DeviceParamsLoaded       =  0x0000;
         S_StatusDataIntern.u16_StatusDataLoaded         =  0x0000;
         // map the content of params into EXtern device params holding registers
         S_ModbusAccessParams.usAddr   =  (ushort) VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START;
         S_ModbusAccessParams.usCount  =  (ushort) (sizeof(S_StatusDataIntern)/2);    // total words of S_StatusDataIntern;
         S_ModbusAccessParams.pvData   =  (void *) (&S_StatusDataIntern);
         TRY_AND_CATCH
         (
         MBPBRegAccessStruct ((void*)(&S_ModbusAccessParams)),
         u32_ErrorCodeSW_Sub,
         (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS
         );
      }
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_REGISTERS_FUNCS__INIT_STATUS_DATA);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ValveActRegistersFuncs_InitRegisterBank   (void)
{
   ulong u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // initialize the complete register bank of valve actuator
   if (SUCCESS_APP_FUNC (ValveActRegistersFuncs_InitDeviceParams(), u32_ErrorCodeSW_Sub))
   {
      TRY_AND_CATCH_APP_FUNC  (ValveActRegistersFuncs_InitStatusData(), u32_ErrorCodeSW_Sub);
   }
   return u32_ErrorCodeSW_Sub;
}
