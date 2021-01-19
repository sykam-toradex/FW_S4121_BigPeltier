/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_support_funcs.c
*
*  \brief               The file contains several support functions. They are mostly used inside command functions. For example
*                       this file contains a function with executes a valve movement and stores new status data to register bank.
*                       Furthermore it initializes the tasks of \ref valve_act_support_tasks.c. Documentation can be found in
*                       \ref valve_act_support_funcs.h.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-10     Initial version extracted from S6510_cmd_functions_utils.c.
*  \date                2011-11-24     Removed code intern word swaps for 32-bit register bank values (NOW they are shown swaped externally).
*  \date                2012-11-07     Changed function code from \ref ValveActSupportFuncs_CalibrationFlagsRW according to removed
*                                      owner commands "calibration data load/save". Changed some variable names inside other functions.
*                                      Added function \ref ValveActSupportFuncs_TestAndCorrectInternDeviceParams.
*  \date                2012-11-12     Changed some functions to use valve position values beginning from one (1).
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
#include "..\..\..\cpu\at91\misc\stpclkdir\stpclkdirext.h"
#include "..\..\..\ext\spi\nvram\fm25clss\fm25clssext.h"
#include "..\..\..\ext\misc\encgapdisk\encgapdiskext.h"
#include "..\..\..\ext\misc\encgapdisk\encgapdisk.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\shared\src\lib_exceptions.h"
#include "..\..\shared\src\common_registers.h"
#include "..\..\shared\src\utils_funcs.h"
#include "valve_act_registers.h"
#include "valve_act_cmds_owner.h"
#include "valve_act_cmds_reg.h"
#include "valve_act_support_funcs.h"
#include "valve_act_support_tasks.h"
#include "valve_act_valvetypes.h"
//---------------------------------------------- globals definitions ---------------------------------------------------------------------------
OS_TID   TID_ValveActSupportFuncs_GenerateInjectSignal;
os_mbx_declare( MBX_ValveActSupportFuncs_GenerateInjectSignal, 100 );          /**< \em define mailbox for \ref TSK_ValveActSupportTasks_GenerateInjectSignal */
OS_TSK_STACK_USER( STK_TSK_ValveActSupportFuncs_GenerateInjectSignal, 1000 );
//---------------------------------------------- externals used in this file -------------------------------------------------------------------
extern const   ushort   cu16_TimeoutVeryShort;
extern const   ushort   cu16_TimeoutShort;
extern const   ushort   cu16_TimeoutLong;
//------------------------------------------------ functions -----------------------------------------------------------------------------------
ulong ValveActSupportFuncs_InitTasks (void)
{
   ulong u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // initialize mailbox "MBX_ValveActSupportFuncs_GenerateInjectSignal"
   os_mbx_init ((OS_ID)MBX_ValveActSupportFuncs_GenerateInjectSignal, (ushort)(sizeof(MBX_ValveActSupportFuncs_GenerateInjectSignal)));
   // messages are generated in "GoTo..." commands and send to following task
   TID_ValveActSupportFuncs_GenerateInjectSignal = os_tsk_create_user (TSK_ValveActSupportTasks_GenerateInjectSignal, (uchar)OS_P_NORMAL,
   (void *)STK_TSK_ValveActSupportFuncs_GenerateInjectSignal,
   (ushort)(sizeof(STK_TSK_ValveActSupportFuncs_GenerateInjectSignal)));
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
// return the number of valid positions for given valve type
ulong ValveActSupportFuncs_PositionsValidGet (ushort u16_ValveType, ushort *pu16_Positions)
{
   uchar u8_Idx               =  1;
   ulong u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   *pu16_Positions            =  0;
   // check if valve type exists
   if (u16_ValveType < (ushort)VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER)
   {
      while (u8_Idx < (uchar)VALVE_ACT_VALVE_TYPES__MAX_VALVE_POSITIONS)
      {
         if (cSA_ValveActValveTypes_SykamValves[u16_ValveType].u8A_Positions[u8_Idx] > 0)
         {
            (*pu16_Positions)++;
         }
         u8_Idx++;
      }
      *pu16_Positions = *pu16_Positions + 1; // add one to include home position
   }
   else
   u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__POSITIONS_VALID_GET;
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ValveActSupportFuncs_PositionValidCheck (ushort u16_ValveType, ushort *pu16_EncoderEdge, ushort u16_InquiredPosition)
{
   static   ushort   u16_ValidPositions;
   ulong u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // read amount of valid positions from input register
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ,
               (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__VALID_POSITIONS, (ushort)AT91SAM7X__NUM_REGS_1,
               (void *)(&u16_ValidPositions), NULL, cu16_TimeoutShort),
            u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // check for exceeded valve type and inquired position
      if ((u16_ValveType < (ushort)VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER) && (u16_InquiredPosition >= 1))
      {
         // check for exceeded valve type and inquired position
         if ((u16_InquiredPosition <= u16_ValidPositions) && (u16_InquiredPosition >= 1))
         {
            // find encoder edge number for inquired position
            if (u16_InquiredPosition == 1) // home pos
            {
               *pu16_EncoderEdge = 0;
            }
            else if (cSA_ValveActValveTypes_SykamValves[u16_ValveType].u8A_Positions[u16_InquiredPosition - 1] != 0)
            {
               *pu16_EncoderEdge = cSA_ValveActValveTypes_SykamValves[u16_ValveType].u8A_Positions[u16_InquiredPosition - 1];
            }
            else // set invalid pos
            *pu16_EncoderEdge = (ushort)ENCGAPDISK0_TRUE_GAP_NUMBER + 1;
         }
      }
   }  // check if inquired valve position is valid
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
// finds the maximum from zero different index in constant structure cSA_ValveActValveTypes_SykamValves, depended on current valve type
ulong ValveActSupportFuncs_FindMaxPosition (uchar u8_ValveType, ushort *pu16_CurrentPosition)
{
   uchar u8_Idx               =  (uchar)VALVE_ACT_VALVE_TYPES__MAX_VALVE_POSITIONS - 1;
   ulong u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // check if valve type exists
   if (u8_ValveType < (uchar)VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER)
   {
      // grep highest non-zero value in given structure, if highest index is allready reached, skip while loop
      while (cSA_ValveActValveTypes_SykamValves[u8_ValveType].u8A_Positions[u8_Idx] == 0)
      {
         u8_Idx--;
      }
      *pu16_CurrentPosition = u8_Idx + 1;
   }
   else
   u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__FIND_MAX_POS_IDX;
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ValveActSupportFuncs_PositionsRW (ushort *pu16_CurrentEncoderEdge, ushort *pu16_CurrentPosition, bool b_RW)
{
   bool     b_RW_Int;
   ushort   u16_FixedMemNumBytesRw  =  (ushort)AT91SAM7X__NUM_BYTES_2;
   ulong    u32_ErrorCodeSW_Sub     =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // prepare write position
   if (b_RW == (bool)VALVE_ACT_SUPPORT_FUNCS__WRITE)
   b_RW_Int = (bool)MBPB_REGRW_WRITE;
   // prepare read position
   else if (b_RW == (bool)VALVE_ACT_SUPPORT_FUNCS__READ)
   b_RW_Int = (bool)MBPB_REGRW_READ;
   else
   return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__POSITIONS_RW;
   // write/read current position to/from input register
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, b_RW_Int, (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_ENCODER_EDGE,
               (ushort)AT91SAM7X__NUM_REGS_1, (void *)(pu16_CurrentEncoderEdge), NULL, cu16_TimeoutShort),
            u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // write/read current position to/from input register
      if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, b_RW_Int, (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_POSITION,
                  (ushort)AT91SAM7X__NUM_REGS_1, (void *)(pu16_CurrentPosition), NULL, cu16_TimeoutShort),
               u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         if (b_RW != (bool)VALVE_ACT_SUPPORT_FUNCS__READ)
         {
            // write/read current encode edge to/from fixed memory (better: do this in a function which is called when brownout is detected)
            if (SUCCESS_APP_FUNC (UtilsFuncs_FixedMemDataPackagesRW ((void *)(pu16_CurrentEncoderEdge), u16_FixedMemNumBytesRw, (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CURRENT_ENCODER_EDGE), b_RW),
                     u32_ErrorCodeSW_Sub))
            {
               // write/read current position index to/from fixed memory (better: do this in a function which is called when brownout is detected)
               TRY_AND_CATCH_APP_FUNC
               (
               UtilsFuncs_FixedMemDataPackagesRW ((void *)(pu16_CurrentPosition), u16_FixedMemNumBytesRw, (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CURRENT_POSITION), b_RW),
               u32_ErrorCodeSW_Sub
               );
            }
         }
      }
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__POSITIONS_RW);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ValveActSupportFuncs_StatusDataSet (ushort *pu16_CurrentEncoderEdge, ushort *pu16_CurrentPosition)
{
   uchar    u8_Unit                    =  0;
   ushort   u16_FixedMemNumBytesRw     =  (ushort)AT91SAM7X__NUM_BYTES_4;
   ushort   u16_Result;
   ulong    u32_ErrorCodeSW_Sub        =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   ulong    u32_CurrentPosInUSteps;
   ulong    u32_TotalSwitchCounter;
   if (SUCCESS (StpClkDirWait (u8_Unit, OS_T_MS(AT91SAM7X__STPCLKDIRWAIT_TIMEOUT_MS)), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__WAIT))
   {
      u32_CurrentPosInUSteps = StpClkDirClksAbsGet (u8_Unit);
      // set internal position to status register
      if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE,
                  (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_POS_IN_USTEPS, (ushort)AT91SAM7X__NUM_REGS_2,
                  (void *)(&u32_CurrentPosInUSteps), NULL, cu16_TimeoutShort),
               u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         // set internal position to fixed memory
         u16_Result  =  Fm25clssWrite (u8_Unit, (void *)(&u32_CurrentPosInUSteps), u16_FixedMemNumBytesRw,
         (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__CURRENT_POS_IN_USTEPS), cu16_TimeoutLong);
         if (u16_Result == u16_FixedMemNumBytesRw)
         {
            // read number of valve switching operations
            if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_READ,
                        (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__TOTAL_SWITCH_COUNTER, (ushort)AT91SAM7X__NUM_REGS_2,
                        (void *)(&u32_TotalSwitchCounter), NULL, cu16_TimeoutShort),
                     u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               // increment valve switch counter
               u32_TotalSwitchCounter = u32_TotalSwitchCounter + 1;
               // write total switching operations to register bank
               if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE,
                           (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__TOTAL_SWITCH_COUNTER, (ushort)AT91SAM7X__NUM_REGS_2,
                           (void *)(&u32_TotalSwitchCounter), NULL, cu16_TimeoutShort),
                        u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
               {
                  // store number total switching operations to fixed memory
                  u16_Result = Fm25clssWrite (u8_Unit, (void *)(&u32_TotalSwitchCounter), u16_FixedMemNumBytesRw,
                  (ushort)(VALVE_ACT_REGISTERS__FRAM_ADDR__TOTAL_SWITCH_COUNTER), cu16_TimeoutLong);
                  if (u16_Result == u16_FixedMemNumBytesRw)
                  {
                     // write current position and position index to registers and fixed memory
                     TRY_AND_CATCH_APP_FUNC
                     (
                     ValveActSupportFuncs_PositionsRW (pu16_CurrentEncoderEdge, pu16_CurrentPosition, (bool)(VALVE_ACT_SUPPORT_FUNCS__WRITE)),
                     u32_ErrorCodeSW_Sub
                     );
                  }
                  else
                  return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
               }
            }
         }
         else
         return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
      }
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__STATUS_DATA_SET);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ValveActSupportFuncs_ExecuteMovement (ushort *pu16_CurrentEncoderEdge, ushort *pu16_CurrentPosition, ushort u16_CurrentValveType)
{
   uchar    u8_Unit              =  0;
   ushort   u16_ValveActState;
   ushort   u16_CurrEdgeType;
   ushort   u16_MsgToSend;
   ulong    u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   u16_ValveActState =  (ushort)VALVE_ACT_REGISTERS__VALVE_ACT_STATE__RUN;
   // set device state
   if (SUCCESS_APP_FUNC (ValveActSupportFuncs_ValveActStateRW ((ushort *)(&u16_ValveActState), (bool)VALVE_ACT_SUPPORT_FUNCS__WRITE), u32_ErrorCodeSW_Sub))
   {
      // read current edge type from holding register
      if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_HOLDING, (bool)MBPB_REGRW_READ,
                  (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_EDGE_TYPE_TRIGGERED_ON,
                  (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_CurrEdgeType), NULL, cu16_TimeoutLong),
               u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         // wait for finishing former movements + time for task execution
         if (SUCCESS (StpClkDirWait (u8_Unit, OS_T_MS(AT91SAM7X__STPCLKDIRWAIT_TIMEOUT_MS)), u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__WAIT))
         {
            if (SUCCESS (EncGapDiskEdgeGoto  (u8_Unit, (uchar)*pu16_CurrentEncoderEdge, (uchar)u16_CurrEdgeType,
                        (ushort)STPCLKDIR0_CLKSMICRO, cu16_TimeoutLong),
                     u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_GOTO))
            {
               // switch to other tasks and return immediately
               os_dly_wait ((ushort)AT91SAM7X__TICK_100);
               // write current valve position/position index as well as motor position to registers and fixed memory
               if (SUCCESS_APP_FUNC (ValveActSupportFuncs_StatusDataSet (pu16_CurrentEncoderEdge, pu16_CurrentPosition),
                        u32_ErrorCodeSW_Sub))
               {
                  u16_ValveActState = (ushort)VALVE_ACT_REGISTERS__VALVE_ACT_STATE__IDLE;
                  // set device state
                  if (SUCCESS_APP_FUNC (ValveActSupportFuncs_ValveActStateRW ((ushort *)(&u16_ValveActState), (bool)VALVE_ACT_SUPPORT_FUNCS__WRITE), u32_ErrorCodeSW_Sub))
                  {
                     // send current position index to taks "TSK_InjectSignal"
                     if ((u16_CurrentValveType == (ushort)VALVE_ACT_VALVE_TYPES__S6X10_ANAL_INJECT_VALVE_3x2) ||
                           (u16_CurrentValveType == (ushort)VALVE_ACT_VALVE_TYPES__S6X11_PREP_INJECT_VALVE_3x2))
                     {
                        u16_MsgToSend = *pu16_CurrentPosition;
                        os_mbx_send ((OS_ID)MBX_ValveActSupportFuncs_GenerateInjectSignal, (void *)(u16_MsgToSend), cu16_TimeoutLong);
                        // wait for msg to be send
                        os_dly_wait (cu16_TimeoutVeryShort);
                     }
                  }
               }
            }
         }
      }
   }  // read current edge type
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__EXECUTE_MOVEMENT);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
// function returns zero if at least one flag is set for read mode or if flag(s) were successfully set in write mode
ulong ValveActSupportFuncs_CalibrationFlagsRW (uchar u8_BitMask, bool b_RW, uchar *pu8_Result)
{
   bool        b_RW_Int;
   ushort      u16_FlagCalibrationFinished;
   ulong       u32_ErrorCodeSW_Sub  = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // prepare set/clear flags
   if (b_RW == (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE)
   {
      b_RW_Int                      =  (bool)MBPB_REGRW_WRITE;
      u16_FlagCalibrationFinished   =  (ushort)(u8_BitMask & (uchar)VALVE_ACT_SUPPORT_FUNCS__MASK_FLAG_FINISHED);  // set/clear flag calibration finished
      u16_FlagCalibrationFinished   =  VALVE_ACT_SUPPORT_FUNCS__UNMASK_FLAG_FINISHED (u16_FlagCalibrationFinished);
   }
   // prepare read flags
   else if ((b_RW == (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ))
   {
      b_RW_Int = (bool)MBPB_REGRW_READ;
   }
   else
   return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__CALIBRATION_FLAGS_RW;
   // set/clear flag calibration finished
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, b_RW_Int, (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CALIBRATION_FINISHED,
               (ushort)AT91SAM7X__NUM_REGS_1, (void *)(&u16_FlagCalibrationFinished), NULL, cu16_TimeoutShort),
            u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      if (b_RW == (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ)
      {
         switch (u8_BitMask)
         {
         case (VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_FINISHED):
            *pu8_Result = (uchar)(u16_FlagCalibrationFinished);
            break;
         default:
            // error case: wrong input flag(s) in u8_BitMask
            return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__CALIBRATION_FLAGS_RW;
         } // end of switch
      }
   } // set/clear flag calibration finished
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__CALIBRATION_FLAGS_RW);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
// function returns zero if at least one flag is set for read mode or if flag(s) were successfully set in write mode
ulong ValveActSupportFuncs_DeviceParamFlagsRW (uchar u8_BitMask, bool b_RW, uchar *pu8_Result)
{
   bool     b_RW_Int;
   ushort   u16_ParamsSaved;
   ushort   u16_ParamsLoaded;
   ulong    u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   // prepare set/clear flags
   if (b_RW == (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE)
   {
      b_RW_Int             = (bool)MBPB_REGRW_WRITE;
      u16_ParamsLoaded     = (ushort)(u8_BitMask & (uchar)VALVE_ACT_SUPPORT_FUNCS__MASK_FLAG_LOADED);   // set/clear flag param data loaded
      u16_ParamsLoaded     = VALVE_ACT_SUPPORT_FUNCS__UNMASK_FLAG_LOADED (u16_ParamsLoaded);
      u16_ParamsSaved      = (ushort)(u8_BitMask & (uchar)VALVE_ACT_SUPPORT_FUNCS__MASK_FLAG_SAVED);    // set/clear flag param data saved
      u16_ParamsSaved      = VALVE_ACT_SUPPORT_FUNCS__UNMASK_FLAG_SAVED (u16_ParamsSaved);
   }
   // prepare read flags
   else if ((b_RW == (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ) && (u8_BitMask == 0x00))
   {
      b_RW_Int = (bool)MBPB_REGRW_READ;
   }
   else
   return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__DEVICE_PARAM_FLAGS_RW;
   // set/clear flag param data loaded
   if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, b_RW_Int, (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__DEVICE_PARAMS_LOADED, (ushort)AT91SAM7X__NUM_REGS_1,
               (void *)(&u16_ParamsLoaded), NULL, cu16_TimeoutShort),
            u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
   {
      // set/clear flag param data saved
      if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, b_RW_Int, (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__DEVICE_PARAMS_SAVED, (ushort)AT91SAM7X__NUM_REGS_1,
                  (void *)(&u16_ParamsSaved), NULL, cu16_TimeoutShort),
               u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
      {
         if ((b_RW == (bool)VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ) && (u8_BitMask == 0x00))
         *pu8_Result = (uchar)(u16_ParamsLoaded | u16_ParamsSaved);
      }
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__DEVICE_PARAM_FLAGS_RW);
   return u32_ErrorCodeSW_Sub;
}
//----------------------------------------------------------------------------------------------------------------------------------------------
ulong ValveActSupportFuncs_ValveActStateRW (ushort *pu16_ValveActState, bool b_RW)
{
   uchar    u8_Unit                 =  0;
   ushort   u16_FixedMemNumBytesRw  =  (ushort)AT91SAM7X__NUM_BYTES_2;
   ulong    u32_ErrorCodeSW_Sub     =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   ushort   u16_Result;
   ushort   u16_CmpBuffer[FM25CLSS_NUM_MULTI_STORAGES - 1];
   switch (b_RW)
   {
   case VALVE_ACT_SUPPORT_FUNCS__READ:
      {
         if (pu16_ValveActState)   // for read accesses pointer must be available
         {
            u16_Result = Fm25clssReadSafe (u8_Unit, (void *)(pu16_ValveActState), (void *)(&u16_CmpBuffer), u16_FixedMemNumBytesRw,
            (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__VALVE_ACT_STATE, cu16_TimeoutLong);
            if (u16_Result != u16_FixedMemNumBytesRw)
            u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__READ_SAFE;
         }
         else
         return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__STATE_RW;
         break;
      }
   case VALVE_ACT_SUPPORT_FUNCS__WRITE:
      {
         if (*pu16_ValveActState < (uchar)VALVE_ACT_REGISTERS__VALVE_ACT_STATES_COUNTER)
         {
            // set valve actuator state
            if (SUCCESS (MBPBRegAccess ((uchar)MBPB_REGTYPE_INPUT, (bool)MBPB_REGRW_WRITE, (ushort)VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__VALVE_ACT_STATE,
                        (ushort)AT91SAM7X__NUM_REGS_1, (void *)(pu16_ValveActState), NULL, cu16_TimeoutShort),
                     u32_ErrorCodeSW_Sub, (ulong)LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS))
            {
               u16_Result = Fm25clssWrite (u8_Unit, (void *)(pu16_ValveActState), u16_FixedMemNumBytesRw, (ushort)VALVE_ACT_REGISTERS__FRAM_ADDR__VALVE_ACT_STATE,
               cu16_TimeoutLong);
               if (u16_Result != u16_FixedMemNumBytesRw)
               u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE;
            }
         }
         else
         return u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__STATE_RW;
         break;
      }
   }
   if (u32_ErrorCodeSW_Sub)
   u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__STATE_RW);
   return u32_ErrorCodeSW_Sub;
}
//------------------------------------------------------------------------------------------------------------------------------------
ulong ValveActSupportFuncs_TestAndCorrectInternDeviceParams (void *pv_DeviceParamsIntern)
{
   ulong u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   tS_ValveActRegisters_DeviceParamsIntern *pS_DeviceParamsIntern;
   //struct   _S_ValveActRegisters_DeviceParamsExtern   S_DeviceParamsExtern;
   //struct   _S_ValveActRegisters_DeviceParamsIntern   S_DeviceParamsIntern;
   pS_DeviceParamsIntern = pv_DeviceParamsIntern;
   // Check device params
   if (pS_DeviceParamsIntern->u16_Cal_Mode > (ushort)ENCGAPDISK0_CONTINUOUS)
   {
      pS_DeviceParamsIntern->u16_Cal_Mode = (ushort)ENCGAPDISK0_CONTINUOUS;
      //S_DeviceParamsIntern.u16_Cal_Mode = (ushort)ENCGAPDISK0_CONTINUOUS;
   }
   if (pS_DeviceParamsIntern->u16_Cal_EdgeTypeTriggeredOn > ((ushort)ENCGAPDISK_EDGETYPE_MAX - 1))
   {
      pS_DeviceParamsIntern->u16_Cal_EdgeTypeTriggeredOn = (ushort)ENCGAPDISK0_DEFAULT_EDGE_TYPE;
      //S_DeviceParamsIntern.u16_Cal_EdgeTypeTriggeredOn = (ushort)ENCGAPDISK0_DEFAULT_EDGE_TYPE;
   }
   if (     (pS_DeviceParamsIntern->u16_DAC_HoldMotorDriver > (ushort)AT91SAM7X__STPCLKDIR0_DAC_MAX)
         || (pS_DeviceParamsIntern->u16_DAC_HoldMotorDriver < (ushort)AT91SAM7X__STPCLKDIR0_DAC_MIN)
         )
   {
      pS_DeviceParamsIntern->u16_DAC_HoldMotorDriver = (ushort)STPCLKDIR0_DAC_HOLD;
      //S_DeviceParamsIntern.u16_DAC_HoldMotorDriver = 	 (ushort)STPCLKDIR0_DAC_HOLD;
   }
   if (     (pS_DeviceParamsIntern->u16_DAC_RunMotorDriver > (ushort)AT91SAM7X__STPCLKDIR0_DAC_MAX)
         || (pS_DeviceParamsIntern->u16_DAC_RunMotorDriver < (ushort)AT91SAM7X__STPCLKDIR0_DAC_MIN)
         )
   {
      pS_DeviceParamsIntern->u16_DAC_RunMotorDriver = (ushort)STPCLKDIR0_DAC_RUN;
      //S_DeviceParamsIntern.u16_DAC_RunMotorDriver = (ushort)STPCLKDIR0_DAC_RUN;
   }
   return u32_ErrorCodeSW_Sub;
}
