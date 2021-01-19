/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_cmds_user.c
*
*  \brief               This file contains the valve actuator \em user command functions which will be executed if sending
*                       the appropriate command via modbus (MBPB). Execution of these functions is allowed to user,
*                       owner (manufacturer) or service technicians. \n
*                       It is recommended to add new command functions \b only to the end of function pointer array since
*                       there exist a documentation about commands and corresponding modbus addresses. If the array is extended,
*                       have a look at current address boarders in \ref valve_act_registers.h.
*                       Every function returns an software base and sub error code which is greater than zero in error case.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-10     Initial version extracted from S6510_cmd_functions_user.c.
*  \date                2012-11-06     Changed checks for available calibration data in all user commands to
*                                      "calibration finished" flag only.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                2011-09-12     Tested new function contents of \ref ValveActCmdsUser_GoToInquiredPos.
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
#include "..\..\..\ext\misc\encgapdisk\encgapdiskext.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "valve_act_registers.h"
#include "valve_act_cmds_user.h"
#include "valve_act_support_funcs.h"
#include "valve_act_valvetypes.h"
//---------------------------------------------- globals definitions ---------------------------------------------------------------------------
// The size and order must be the same like in documentation (see excel table)!
tfp_ValveActCmdsUser_CMD   fpA_ValveActCmdsUser_CMDs[] =
{
   { &ValveActCmdsUser_GoToHomePos      },
   { &ValveActCmdsUser_GoToNextPos      },
   { &ValveActCmdsUser_GoToPreviousPos  },
   { &ValveActCmdsUser_GoToInquiredPos  }
};
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern const   struct   _S_ValveActValveTypes_SykamValves  cSA_ValveActValveTypes_SykamValves[VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER];
extern const   ushort   cu16_TimeoutShort;
//---------------------------------------------- user command functions ------------------------------------------------------------------------
ushort ValveActCmdsUser_GoToHomePos (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_CurrentEncoderEdge;
   ushort   u16_CurrentPosition;
   ushort   u16_CurrEdgeType;
   ushort   u16_ValidPositions;
   ushort   u16_CurrentValveType;
   uchar    u8_Result               = 0;
   uchar    u8_Unit                 = 0;
   ushort   u16_ErrorCodeSW_Base    = (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // check if calibration data is available
   if (SUCCESS_APP_FUNC (ValveActSupportFuncs_CalibrationFlagsRW ((uchar)VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_FINISHED, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ, &u8_Result),
            *pu32_ErrorCodeSW_Sub))
   {
      if (!u8_Result)  // break condition if no flags are set
      {
         *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_CALIBRATION_DATA_NOT_AVAILABLE;
         return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_HOME;
      }
      // check if param data is available
      if (SUCCESS_APP_FUNC (ValveActSupportFuncs_DeviceParamFlagsRW (NULL, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ, &u8_Result), *pu32_ErrorCodeSW_Sub))
      {
         if (!u8_Result)  // break condition if no flags are set
         {
            *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_DEVICE_PARAMS_NOT_AVAILABLE;
            return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_HOME;
         }
         //////////////////////////////////////////////////////////////////
         //addded to prevent movement if iit is already at home position.
         //////////////////////////////////////////////////////////////////
         // read position and position index
         if (SUCCESS_APP_FUNC (ValveActSupportFuncs_PositionsRW (&u16_CurrentEncoderEdge, &u16_CurrentPosition, (bool)(VALVE_ACT_SUPPORT_FUNCS__READ)), *pu32_ErrorCodeSW_Sub))
         {
            // read amount of valid positions from input register
            if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ,
                        (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__VALID_POSITIONS, (ushort)AT91SAM7X__NUM_REGS_1,
                        (void *)(&u16_ValidPositions), NULL, cu16_TimeoutShort),
                     *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               // check if max position index is reached to avoid an array access error AND set home position
               if ((u16_CurrentPosition != 1 )||(u16_CurrentPosition >= u16_ValidPositions))
               {
                  u16_CurrentPosition     =  u16_ValidPositions;
                  u16_CurrentEncoderEdge  =  cSA_ValveActValveTypes_SykamValves[u16_CurrentValveType].u8A_Positions[(uchar)u16_CurrentPosition - 1];
                  u16_CurrentPosition     =  1;                  
               }
               // read current edge type from holding register
               if( SUCCESS( MBPBRegAccess( (uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
                           (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_EDGE_TYPE_TRIGGERED_ON, (ushort)AT91SAM7X__NUM_REGS_1,
                           (void *)(&u16_CurrEdgeType), NULL, cu16_TimeoutShort ),
                        *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS ) )
               {
                  u16_CurrentEncoderEdge  =  u16_CurrentEncoderEdge; 			
                  u16_CurrentPosition     =  1;
                  u16_CurrentEncoderEdge  =  0;
                  // Wait for finishing former movements + time for task execution. At maximum speed 409 ms are needed for a full circle.
                  if (SUCCESS (StpClkDirWait (u8_Unit, OS_T_MS(AT91SAM7X__STPCLKDIRWAIT_TIMEOUT_MS)), *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__WAIT))
                  {
                     if (SUCCESS (EncGapDiskEdgeGoto  (u8_Unit, (uchar)u16_CurrentEncoderEdge, (uchar)u16_CurrEdgeType, (ushort)STPCLKDIR0_CLKSMICRO, cu16_TimeoutShort),
                              *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_GOTO))
                     {
                        // switch to other tasks and return immediately
                        os_dly_wait ((ushort)AT91SAM7X__TICK_1);
                        // write current valve position/position index as well as motor position to registers and fixed memory
                        TRY_AND_CATCH_APP_FUNC (ValveActSupportFuncs_StatusDataSet (&u16_CurrentEncoderEdge, &u16_CurrentPosition), *pu32_ErrorCodeSW_Sub);
                     }
                  }
               }  // read current edge type
            }//read valid positionns
         }
      }  // check param flags
   }  // check calibration flags
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_HOME;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ValveActCmdsUser_GoToNextPos (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_CurrentEncoderEdge;
   ushort   u16_CurrentPosition;
   ushort   u16_ValidPositions;
   ushort   u16_CurrentValveType;
   uchar    u8_Result               =  0;
   ulong    u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // check if calibration data is available
   if (SUCCESS_APP_FUNC (ValveActSupportFuncs_CalibrationFlagsRW  ((uchar)VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_FINISHED, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ, &u8_Result),
            *pu32_ErrorCodeSW_Sub))
   {
      if (!u8_Result)  // break condition if no flags are set
      {
         *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_CALIBRATION_DATA_NOT_AVAILABLE;
         return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_NEXT;
      }
      // check if param data is available
      if (SUCCESS_APP_FUNC (ValveActSupportFuncs_DeviceParamFlagsRW  (NULL, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ, &u8_Result), *pu32_ErrorCodeSW_Sub))
      {
         if (!u8_Result)  // break condition if no flags are set
         {
            *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_DEVICE_PARAMS_NOT_AVAILABLE;
            return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_NEXT;
         }
         // read current valve type from holding register
         if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
                     (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE, (ushort)AT91SAM7X__NUM_REGS_1,
                     (void *)(&u16_CurrentValveType), NULL, cu16_TimeoutShort),
                  *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            if (u16_CurrentValveType >= (ushort)VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER)
            {
               *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_VALVE_TYPE_NOT_SUPPORTED;
               return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_NEXT;
            }
            // read position and position index
            if (SUCCESS_APP_FUNC (ValveActSupportFuncs_PositionsRW (&u16_CurrentEncoderEdge, &u16_CurrentPosition, (bool)(VALVE_ACT_SUPPORT_FUNCS__READ)), *pu32_ErrorCodeSW_Sub))
            {
               // read amount of valid positions from input register
               if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ,
                           (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__VALID_POSITIONS, (ushort)AT91SAM7X__NUM_REGS_1,
                           (void *)(&u16_ValidPositions), NULL, cu16_TimeoutShort),
                        *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  // check if max position index is reached to avoid an array access error AND set home position
                  if (u16_CurrentPosition >= u16_ValidPositions)
                  {
                     //u16_CurrentPosition     =  1;
                     u16_CurrentPosition     =  u16_ValidPositions;
                     u16_CurrentEncoderEdge  =  cSA_ValveActValveTypes_SykamValves[u16_CurrentValveType].u8A_Positions[(uchar)u16_CurrentPosition - 1];
                  }
                  else
                  {
                     u16_CurrentPosition     =  u16_CurrentPosition + 1;
                     u16_CurrentEncoderEdge  =  cSA_ValveActValveTypes_SykamValves[u16_CurrentValveType].u8A_Positions[(uchar)u16_CurrentPosition - 1];
                  }
                  TRY_AND_CATCH_APP_FUNC
                  (
                  ValveActSupportFuncs_ExecuteMovement (&u16_CurrentEncoderEdge, &u16_CurrentPosition, u16_CurrentValveType),
                  *pu32_ErrorCodeSW_Sub
                  );
               }  // read amount of valid positions
            }  // read position and position index
         }  // read current valve type
      }  // check param flags
   }  // check calibration flags
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_NEXT;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ValveActCmdsUser_GoToPreviousPos (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   ushort   u16_CurrentEncoderEdge;
   ushort   u16_CurrentPosition;
   ushort   u16_CurrentValveType;
   uchar    u8_Result               =  0;
   ushort   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // check if calibration data is available
   if (SUCCESS_APP_FUNC (ValveActSupportFuncs_CalibrationFlagsRW ((uchar)VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_FINISHED, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ, &u8_Result),
            *pu32_ErrorCodeSW_Sub))
   {
      if (!u8_Result)  // break condition if no flags are set
      {
         *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_CALIBRATION_DATA_NOT_AVAILABLE;
         return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_PREVIOUS;
      }
      // check if param data is available
      if (SUCCESS_APP_FUNC (ValveActSupportFuncs_DeviceParamFlagsRW (NULL, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ, &u8_Result), *pu32_ErrorCodeSW_Sub))
      {
         if (!u8_Result)  // break condition if no flags are set
         {
            *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_DEVICE_PARAMS_NOT_AVAILABLE;
            return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_PREVIOUS;
         }
         // read current valve type from holding register
         if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
                     (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE, (ushort)AT91SAM7X__NUM_REGS_1,
                     (void *)(&u16_CurrentValveType), NULL, cu16_TimeoutShort),
                  *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            if (u16_CurrentValveType >= (ushort)VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER)
            {
               *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_VALVE_TYPE_NOT_SUPPORTED;
               return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_PREVIOUS;
            }
            // read position and position index
            if (SUCCESS_APP_FUNC (ValveActSupportFuncs_PositionsRW (&u16_CurrentEncoderEdge, &u16_CurrentPosition, (bool)(VALVE_ACT_SUPPORT_FUNCS__READ)), *pu32_ErrorCodeSW_Sub))
            {
               // check if current valve position index is the home position, if true, find max index to go to previous edge
               if (u16_CurrentPosition == 1)
               {
                  if (FAIL_APP_FUNC (ValveActSupportFuncs_FindMaxPosition ((uchar)u16_CurrentValveType, &u16_CurrentPosition), *pu32_ErrorCodeSW_Sub))
                  {
                     *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__FIND_MAX_POS_IDX;
                     return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_PREVIOUS;
                  }
                  u16_CurrentEncoderEdge = cSA_ValveActValveTypes_SykamValves[u16_CurrentValveType].u8A_Positions[(uchar)u16_CurrentPosition - 1];
               }
               else
               {
                  u16_CurrentPosition     =  u16_CurrentPosition - 1;
                  u16_CurrentEncoderEdge  =  cSA_ValveActValveTypes_SykamValves[u16_CurrentValveType].u8A_Positions[(uchar)u16_CurrentPosition - 1];
               }
               TRY_AND_CATCH_APP_FUNC
               (
               ValveActSupportFuncs_ExecuteMovement (&u16_CurrentEncoderEdge, &u16_CurrentPosition, u16_CurrentValveType), *pu32_ErrorCodeSW_Sub
               );
            }  // read position and position index
         }  // read current valve type
      }  // check param flags
   } //  check calibration flags
   if (*pu32_ErrorCodeSW_Sub)
   u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_PREVIOUS;
   return u16_ErrorCodeSW_Base;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ushort ValveActCmdsUser_GoToInquiredPos (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
{
   static   ushort   u16_CurrentPosition;
   static   ushort   u16_CurrentValveType;
   static   ushort   u16_ErrorCodeSW_Base    =  (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   static   ushort   u16_CurrentEncoderEdge  =  0;
   static   ushort   u16_InquiredPosition    =  0;
   static   uchar    u8_Result               =  0;
   *pu32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   u16_InquiredPosition    =  u16_ModbusRxData;
   // check if calibration data is available
   if (SUCCESS_APP_FUNC (ValveActSupportFuncs_CalibrationFlagsRW  ((uchar)VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_FINISHED, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ, &u8_Result),
            *pu32_ErrorCodeSW_Sub))
   {
      if (!u8_Result)  // break condition if no flags are set
      {
         *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_CALIBRATION_DATA_NOT_AVAILABLE;
         return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_INQUIRED;
      }
      // check if param data is available
      if (SUCCESS_APP_FUNC (ValveActSupportFuncs_DeviceParamFlagsRW (NULL, (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ, &u8_Result), *pu32_ErrorCodeSW_Sub))
      {
         if (!u8_Result)  // break condition if no flags are set
         {
            *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_DEVICE_PARAMS_NOT_AVAILABLE;
            return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_INQUIRED;
         }
         // read current valve type from holding register
         if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
                     (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE, (ushort)AT91SAM7X__NUM_REGS_1,
                     (void *)(&u16_CurrentValveType), NULL, cu16_TimeoutShort),
                  *pu32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
         {
            if (u16_CurrentValveType >= (ushort)VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER)
            {
               *pu32_ErrorCodeSW_Sub         =  (ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_VALVE_TYPE_NOT_SUPPORTED;
               return u16_ErrorCodeSW_Base   =  (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_INQUIRED;
            }
            // read position and position index
            ValveActSupportFuncs_PositionsRW (&u16_CurrentEncoderEdge, &u16_CurrentPosition, (bool)(VALVE_ACT_SUPPORT_FUNCS__READ));
            // check if inquired valve position is valid for current valve type
            ValveActSupportFuncs_PositionValidCheck (u16_CurrentValveType, &u16_CurrentEncoderEdge, u16_InquiredPosition);
            if (SUCCESS_APP_FUNC (ValveActSupportFuncs_PositionValidCheck (u16_CurrentValveType, &u16_CurrentEncoderEdge, u16_InquiredPosition), *pu32_ErrorCodeSW_Sub))
            {
               if (u16_InquiredPosition!=u16_CurrentPosition)
               {
                  if (u16_CurrentEncoderEdge <= (ushort)ENCGAPDISK0_TRUE_GAP_NUMBER)
                  {
                     TRY_AND_CATCH_APP_FUNC
                     (ValveActSupportFuncs_ExecuteMovement (&u16_CurrentEncoderEdge, &u16_InquiredPosition, u16_CurrentValveType),*pu32_ErrorCodeSW_Sub);                        
                  }
               }
            }
         }  // read current valve type
      }  // check param flags
   }  // check calibration flags
   if (*pu32_ErrorCodeSW_Sub)
      u16_ErrorCodeSW_Base = (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_INQUIRED;
   return u16_ErrorCodeSW_Base;
}
