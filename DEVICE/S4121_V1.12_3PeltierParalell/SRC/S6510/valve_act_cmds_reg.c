/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_cmds_reg.c
*
*  \brief               This file contains the \em register command functions which will be executed if a value is written to a
*                       valve actuator device params holding register via modbus (MBPB) \em and which register address is linked
*                       with a function of this file. Execution of these functions is allowed to user, owner (manufacturer) or
*                       service technicians. \n
*                       It is recommended to add new command functions \b only into the correct position of function pointer array.
*                       Have a look at external documentation about valve actuator device params register bank. The position of a new
*                       register command function inside function pointer array must be the same as in device params register bank.
*                       If the function pointer array is extended at the end have a look at current address boarders in
*                       \ref valve_act_registers.h. \n
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-10     Initial version extracted from S6510_cmd_functions_reg.c.
*  \date                2012-11-09     Separated function array "fpA_ValveActCmdsReg_REGCMDs_DeviceParams" for extern and intern
*                                      device params according to new register bank layout with inter/extern device parameters and
*                                      status data. Adapted argument names for new support functions.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                2011-08-30     Tested all functions of this file.
*
*  \todo
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "..\..\..\cpu\at91\misc\stpclkdir\stpclkdirext.h"
#include "..\..\..\ext\spi\nvram\fm25clss\fm25clssext.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\utils_funcs.h"
#include "valve_act_registers.h"
#include "valve_act_cmds_reg.h"
#include "valve_act_cmds_owner.h"
#include "valve_act_support_funcs.h"
//---------------------------------------------- globals definitions ---------------------------------------------------------------------------
// The follwing register command functions write the given modbus data to corresponding register bank variables. The size and order
// of the function pointer array must be the same as the corresponding structure!
// Reason:  Moderate case (wrong order):  The manufacturer/service technician changes a device param via modbus
//                                         -> the wrong function is called (addresses must be the same).
//          Bad case (wronge size):       No corresponding entry in function pointer array is found
//                                         -> system might crash.
// For variables which shall be not linked with a register command function set a dummy function and
// nothing will be done except that the new value is written into the register bank
// Define register function pointers
tfp_ValveActCmdsReg_REGCMD   fpA_ValveActCmdsReg_REGCMDs_DeviceParamsIntern[sizeof(struct _S_ValveActRegisters_DeviceParamsIntern)/2] =
{
   { &ValveActCmdsReg_Cal_UStepsInit            },
   { &ValveActCmdsReg_Cal_USPS_Init             },
   { &ValveActCmdsReg_Cal_UStepsAcc             },
   { &ValveActCmdsReg_Cal_UStepsBreak           },
   { &ValveActCmdsReg_Cal_USPS_Start            },
   { &ValveActCmdsReg_Cal_USPS_Run              },
   { &ValveActCmdsReg_Cal_Mode                  },
   { &ValveActCmdsReg_Cal_UStepsFullCircle      },
   { &ValveActCmdsReg_Cal_TrueGapNumber         },
   { &ValveActCmdsReg_Cal_MinUStepsThreshold    },
   { &ValveActCmdsReg_Cal_EdgeTypeTriggeredOn   },
   { &ValveActCmdsReg_Cal_Tol_UStepsSameGapsCal },
   { &ValveActCmdsReg_Cal_Tol_UStepsSameGapsRun },
   { &ValveActCmdsReg_DAC_HoldMotorDriver },
   { &ValveActCmdsReg_DAC_RunMotorDriver  },
};
tfp_ValveActCmdsReg_REGCMD   fpA_ValveActCmdsReg_REGCMDs_DeviceParamsExtern[sizeof(struct _S_ValveActRegisters_DeviceParamsExtern)/2] =
{
   { &ValveActCmdsReg_CurrentValveType          },
};
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern const   ushort   cu16_TimeoutShort;
extern const   ushort   cu16_TimeoutLong;
//---------------------------------------------- command functions activated through changed params --------------------------------------------
ushort ValveActCmdsReg_Cal_UStepsInit(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USTEPS_INIT), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_USPS_Init(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USPS_INIT), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_UStepsAcc(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USTEPS_ACC), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_UStepsBreak(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USTEPS_BREAK), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_USPS_Start(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USPS_START), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_USPS_Run(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USPS_RUN), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_Mode(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_MODE), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_UStepsFullCircle(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USTEPS_FULL_CIRCLE), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_TrueGapNumber(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_TRUE_GAP_NUMBER), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_MinUStepsThreshold(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_MIN_USTEPS_THRESHOLD), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_EdgeTypeTriggeredOn(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_EDGE_TYPE_TRIGGERED_ON), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_Tol_UStepsSameGapsCal(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_TOL_USTEPS_SAME_GAPS_CAL), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
ushort ValveActCmdsReg_Cal_Tol_UStepsSameGapsRun(ushort u16_ModbusRxData, ulong *pu32_ErrorCode_Sub)
{
   uchar    u8_Unit0 = 0;
   ushort   u16_FRAM_Data;
   ushort   u16_FRAM_NumBytesToWrite;
   ushort   u16_Result;
   ushort   u16_ErrorCode_Base   =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCode_Sub           =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // save changed param on FRAM
   u16_FRAM_Data                 = (ushort) u16_ModbusRxData;
   u16_FRAM_NumBytesToWrite      = 2;  // --> write one short values
   u16_Result                    = Fm25clssWrite( u8_Unit0, (void *)(&u16_FRAM_Data), u16_FRAM_NumBytesToWrite,
   (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_TOL_USTEPS_SAME_GAPS_RUN), cu16_TimeoutLong );
   // error case:
   if( u16_Result != u16_FRAM_NumBytesToWrite )
   {
      *pu32_ErrorCode_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      return u16_ErrorCode_Base   = 0;
   }
   return u16_ErrorCode_Base;
}
//---------------------------------------------- command functions activated through changed params --------------------------------------------
ushort ValveActCmdsReg_DAC_HoldMotorDriver (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   uchar    u8_Unit                    =  0;
   ushort   u16_Result                 =  0;
   ushort   u16_DAC_RunMotorDriver;
   ushort   u16_DAC_HoldMotorDriver    =  u16_ModbusRxData;
   ushort   u16_FixedMemNumBytesRw        =  (ushort)AT91SAM7X__NUM_BYTES_2;
   ushort   u16_ErrorCodeSW_Base          =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // read current DAC data input value from holding register
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
               (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DAC_RUN_MOTOR_DRIVER, (ushort)AT91SAM7X__NUM_REGS_1,
               (void *)(&u16_DAC_RunMotorDriver), NULL, cu16_TimeoutShort),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // check if hold value is inside ranges according
      if (u16_DAC_HoldMotorDriver <= AT91SAM7X__STPCLKDIR0_DAC_MAX)
      {
         TRY_AND_CATCH
         (
         StpClkDirDacSet (u8_Unit, u16_DAC_HoldMotorDriver, NULL, cu16_TimeoutLong),
         *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__DAC_SET
         );
         // store new valve type to fixed memory
         u16_Result = Fm25clssWrite (u8_Unit, (void *)(&u16_DAC_HoldMotorDriver), u16_FixedMemNumBytesRw,
         (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__DAC_HOLD_MOTOR_DRIVER), cu16_TimeoutLong);
         if (u16_Result != u16_FixedMemNumBytesRw)  // error case, return from function
         {
            // store new valve type to fixed memory
            u16_Result = Fm25clssWrite (u8_Unit, (void *)(&u16_DAC_HoldMotorDriver), u16_FixedMemNumBytesRw,
            (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__DAC_HOLD_MOTOR_DRIVER), cu16_TimeoutLong);
            if (u16_Result != u16_FixedMemNumBytesRw)  // error case, return from function
            {
               *pu32_ErrorCodeSW_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
               return u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__DAC_HOLD_MOTOR_DRIVER;
            }
         }	
      }
      else
      {
         *pu32_ErrorCodeSW_Sub         =  (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_DAC_HOLD_MOTOR_DRIVER_VALUE_TO_BIG;
         return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__DAC_HOLD_MOTOR_DRIVER;
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__DAC_HOLD_MOTOR_DRIVER;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ValveActCmdsReg_DAC_RunMotorDriver (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   uchar    u8_Unit                    = 0;
   ushort   u16_Result                 =  0;
   ushort   u16_DAC_RunMotorDriver     = u16_ModbusRxData;
   ushort   u16_DAC_HoldMotorDriver;
   ushort   u16_FixedMemNumBytesRw        =  (ushort)AT91SAM7X__NUM_BYTES_2;
   ushort   u16_ErrorCodeSW_Base          =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // read current DAC data input value from holding register
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
               (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DAC_HOLD_MOTOR_DRIVER, (ushort)AT91SAM7X__NUM_REGS_1,
               (void *)(&u16_DAC_HoldMotorDriver), NULL, cu16_TimeoutShort),
            *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // check if run value is inside ranges according
      if (u16_DAC_RunMotorDriver >= AT91SAM7X__STPCLKDIR0_DAC_MIN)
      {
         TRY_AND_CATCH
         (
         StpClkDirDacSet (u8_Unit, NULL, u16_DAC_RunMotorDriver, cu16_TimeoutLong),
         *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__DAC_SET
         );
         // store new value
         u16_Result = Fm25clssWrite (u8_Unit, (void *)(&u16_DAC_RunMotorDriver), u16_FixedMemNumBytesRw,
         (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__DAC_RUN_MOTOR_DRIVER), cu16_TimeoutLong);         
         if (u16_Result != u16_FixedMemNumBytesRw)  // error case, return from function
         {
            // store new value
            u16_Result = Fm25clssWrite (u8_Unit, (void *)(&u16_DAC_RunMotorDriver), u16_FixedMemNumBytesRw,
            (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__DAC_RUN_MOTOR_DRIVER), cu16_TimeoutLong);         
            if (u16_Result != u16_FixedMemNumBytesRw)  // error case, return from function
            {
               *pu32_ErrorCodeSW_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
               return u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__DAC_RUN_MOTOR_DRIVER;
            }            
         }
      }
      else
      {
         *pu32_ErrorCodeSW_Sub         =  (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_DAC_RUN_MOTOR_DRIVER_VALUE_TOO_SMALL;
         return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__DAC_RUN_MOTOR_DRIVER;
      }
   }
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__DAC_RUN_MOTOR_DRIVER;
   return u16_ErrorCodeSW_Base;
}
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------
ushort ValveActCmdsReg_CurrentValveType (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   uchar    u8_Unit                       =  0;
   uchar    u8_Result;
   ushort   u16_Result                    =  0;
   ushort   u16_CurrentValveType          =  u16_ModbusRxData;
   ushort   u16_ValidPositions;
   ushort   u16_FixedMemNumBytesRw        =  (ushort)AT91SAM7X__NUM_BYTES_2;
   ushort   u16_ErrorCodeSW_Base          =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;

   // Clear flag calibration finished
   ValveActSupportFuncs_DeviceParamFlagsRW (((uchar)VALVE_ACT_SUPPORT_FUNCS__CLEAR_FLAG_FINISHED), (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE, &u8_Result );
   ValveActSupportFuncs_CalibrationFlagsRW ((uchar)VALVE_ACT_SUPPORT_FUNCS__CLEAR_FLAG_FINISHED, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE, &u8_Result);   
   // Read given valve type
   (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,(ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE, 
   (ushort)AT91SAM7X__NUM_REGS_1,(void *)(&u16_CurrentValveType), NULL, cu16_TimeoutLong),*pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS);      
   u16_CurrentValveType=u16_CurrentValveType;
   // set given valve type
   MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_WRITE,(ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE, 
   (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_CurrentValveType), NULL, cu16_TimeoutLong);   
   u16_CurrentValveType=u16_CurrentValveType;
     
   // calibrate device and store calibration data as (user) calibration data (DON'T change execution order)
   if (SUCCESS_APP_FUNC (ValveActCmdsOwner_CalWithDeviceParams ((ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__CAL_WITH_DEVICE_PARAMS, pu32_ErrorCodeSW_Sub), u16_ErrorCodeSW_Base))
   {
      if (SUCCESS_APP_FUNC (ValveActCmdsOwner_DeviceParamsSave ((ushort)VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_SAVE, pu32_ErrorCodeSW_Sub), u16_ErrorCodeSW_Base))
      {
         // Read given valve type
         (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
         (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE, (ushort)AT91SAM7X__NUM_REGS_1,
         (void *)(&u16_CurrentValveType), NULL, cu16_TimeoutShort),
         *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS);
         u16_CurrentValveType=u16_CurrentValveType;
         // set given valve type
         if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_WRITE,
                     (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE, (ushort)AT91SAM7X__NUM_REGS_1,
                     (void *)(&u16_CurrentValveType), NULL, cu16_TimeoutShort),
                  *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            // get available positions of given valve type
            if (SUCCESS_APP_FUNC (ValveActSupportFuncs_PositionsValidGet (u16_CurrentValveType, &u16_ValidPositions), *pu32_ErrorCodeSW_Sub))
            {
               // write valid positions of current valve type to input register
               if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE,
                           (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__VALID_POSITIONS, (ushort)AT91SAM7X__NUM_REGS_1,
                           (void *)(&u16_ValidPositions), NULL, cu16_TimeoutShort),
                        *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  // store new valve type to fixed memory
                  u16_Result = Fm25clssWrite (u8_Unit, (void *)(&u16_CurrentValveType), u16_FixedMemNumBytesRw,
                  (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CURRENT_VALVE_TYPE), cu16_TimeoutLong);
                  if (u16_Result != u16_FixedMemNumBytesRw)  // error case, return from function
                  {
                     *pu32_ErrorCodeSW_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
                     return u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__CURRENT_VALVE_TYPE;
                  }
                  // store valid valve positions to fixed memory
                  u16_Result = Fm25clssWrite (u8_Unit, (void *)(&u16_ValidPositions), u16_FixedMemNumBytesRw,
                  (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__VALID_POSITIONS), cu16_TimeoutLong);
                  if (u16_Result != u16_FixedMemNumBytesRw)  // error case, return from function
                  {
                     *pu32_ErrorCodeSW_Sub         = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
                     return u16_ErrorCodeSW_Base   = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__CURRENT_VALVE_TYPE;
                  }
               }
            }  // get available positions of given valve type
         }  // set given valve type
      }  // store device params
   }  // calibrate device
   if ( (*pu32_ErrorCodeSW_Sub) && (!u16_ErrorCodeSW_Base))
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__CURRENT_VALVE_TYPE;
   // return a double error base code in one return value if one of the executed base functions returns with an error
   return u16_ErrorCodeSW_Base;
}
