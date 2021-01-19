/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_support_funcs.h
*
*  \brief               Header file for \ref valve_act_support_funcs.c
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-10     Initial version extracted from S6510_cmd_functions.h.
*  \date                2012-11-07     Added function prototype \ref ValveActSupportFuncs_TestAndCorrectInternDeviceParams.
*  \date                2012-11-12     Changed argument names of some functions.
*  \date                2012-11-12     Refreshed function documentation.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef VALVE_ACT_SUPPORT_FUNCS__H
#define VALVE_ACT_SUPPORT_FUNCS__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

#define  VALVE_ACT_SUPPORT_FUNCS__WRITE                        (1)
#define  VALVE_ACT_SUPPORT_FUNCS__READ                         (0)

#define  VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE                  (1)

#define  VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ                   (0)

#define  VALVE_ACT_SUPPORT_FUNCS__MASK_FLAG_LOADED             (1 << 0)
#define  VALVE_ACT_SUPPORT_FUNCS__MASK_FLAG_SAVED              (1 << 1)
#define  VALVE_ACT_SUPPORT_FUNCS__MASK_FLAG_FINISHED           (1 << 2)

#define  VALVE_ACT_SUPPORT_FUNCS__UNMASK_FLAG_LOADED(x)        (x >> 0)
#define  VALVE_ACT_SUPPORT_FUNCS__UNMASK_FLAG_SAVED(x)         (x >> 1)
#define  VALVE_ACT_SUPPORT_FUNCS__UNMASK_FLAG_FINISHED(x)      (x >> 2)

#define  VALVE_ACT_SUPPORT_FUNCS__SET_FLAG_LOADED              (1 << 0)
#define  VALVE_ACT_SUPPORT_FUNCS__SET_FLAG_SAVED               (1 << 1)
#define  VALVE_ACT_SUPPORT_FUNCS__SET_FLAG_FINISHED            (1 << 2)

#define  VALVE_ACT_SUPPORT_FUNCS__CLEAR_FLAG_LOADED            (0 << 0)
#define  VALVE_ACT_SUPPORT_FUNCS__CLEAR_FLAG_SAVED             (0 << 1)
#define  VALVE_ACT_SUPPORT_FUNCS__CLEAR_FLAG_FINISHED          (0 << 2)

#define  VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_LOADED             (1 << 0)
#define  VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_SAVED              (1 << 1)
#define  VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_FINISHED           (1 << 2)

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   OS_MBX   MBX_ValveActSupportFuncs_GenerateInjectSignal;

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

ulong    ValveActSupportFuncs_InitTasks                        (void);
ulong    ValveActSupportFuncs_PositionsValidGet                (ushort u16_ValveType,              ushort *pu16_Positions                                            );
ulong    ValveActSupportFuncs_PositionValidCheck               (ushort u16_ValveType,              ushort *pu16_EncoderEdge,        ushort   u16_InquiredPosition    );
ulong    ValveActSupportFuncs_PositionsRW                      (ushort *pu16_CurrentEncoderEdge,   ushort *pu16_CurrentPosition,    bool     b_RW                    );
ulong    ValveActSupportFuncs_FindMaxPosition                  (uchar  u8_ValveType,               ushort *pu16_CurrentPosition                                      );
ulong    ValveActSupportFuncs_StatusDataSet                    (ushort *pu16_CurrentEncoderEdge,   ushort *pu16_CurrentPosition                                      );
ulong    ValveActSupportFuncs_ExecuteMovement                  (ushort *pu16_CurrentEncoderEdge,   ushort *pu16_CurrentPosition,    ushort   u16_CurrentValveType    );
ulong    ValveActSupportFuncs_CalibrationFlagsRW               (uchar  u8_BitMask,                 bool   b_RW,                     uchar    *pu8_Result             );
ulong    ValveActSupportFuncs_DeviceParamFlagsRW               (uchar  u8_BitMask,                 bool   b_RW,                     uchar    *pu8_Result             );
ulong    ValveActSupportFuncs_ValveActStateRW                  (ushort *pu16_ValveActState,        bool   b_RW                                                       );
ulong    ValveActSupportFuncs_TestAndCorrectInternDeviceParams (void   *pv_DeviceParamsIntern                                                                        );

#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActSupportFuncs_InitTasks (void)
*
*  \brief      This function initializes support tasks of file \ref valve_act_support_tasks.c.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActSupportFuncs_PositionsValidGet (ushort u16_ValveType, ushort *pu16_Positions)
*
*  \brief      This function returns in \em pu16_Positions the amount of valid positions for given valve type \em u16_ValveType.
*
*
*  \param      u16_ValveType                    This is the valve type for which the amount of valid positions is determined. 
*                                               The valve type must be out of \ref _E_ValveActValveTypes_SykamValves.
*
*  \param      *pu16_Positions                  A pointer at which address the amount of valve positions is written to.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActSupportFuncs_PositionValidCheck (ushort u16_ValveType, ushort *pu16_EncoderEdge, ushort u16_InquiredPosition)
*
*  \brief      This function checks for given valve type \em u16_ValveType if the inquired valve position \em u16_InquiredPosition exists.
*              If existing it returns at address of \em pu16_EncoderEdge the corresponding encoder gap disk edge number of \ref _S_ValveActValveTypes_SykamValves.
*
*
*  \param      u16_ValveType                    This is the valve type for which the inquired position shall be checked. The valve 
*                                               type must be out of \ref _E_ValveActValveTypes_SykamValves.
*
*  \param      *pu16_EncoderEdge                This pointer is needed to return the <b>corresponding encoder gap disk edge number</b> in case 
*                                               the inquired position exists.
*
*  \param      u16_InquiredPosition             The inquired position to be checked if it is a valid valve index for \em u16_ValveType.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActSupportFuncs_PositionsRW (ushort *pu16_CurrentEncoderEdge, ushort *pu16_CurrentPosition, bool b_RW)
*
*  \brief      This function reads/writes the given position arguments to corresponding register bank entries <em> and 
*              permanent memory</em>.
*
*
*  \param      *pu16_CurrentEncoderEdge         The current <b>valve encoder edge</b> to read/write to register bank \@ 
*                                               \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_ENCODER_EDGE and to fixed memory.
*
*  \param      *pu16_CurrentPosition            The current <b>valve position index</b> to read/write to register bank \@
*                                               \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_POSITION and to fixed memory.
*
*  \param      b_RW                             The read/write flag to indicate desired operation. Possible flags: 
*                                               \ref VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ, \em VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActSupportFuncs_FindMaxPosition (uchar u8_ValveType, ushort *pu16_CurrentPosition)
*
*  \brief      Finds the maximum index in valve position array (\ref cSA_ValveActValveTypes_SykamValves). This is not 
*              allways equal to \ref VALVE_ACT_VALVE_TYPES__MAX_VALVE_POSITIONS - 1! It depends on \em u8_ValveType.
*
*
*  \param      u8_ValveType                     This is the valve type for which the maximum index of valve positions array 
*                                               shall be returned in \em *pu16_CurrentPosition. 
*                                               The valve type must be out of \ref _E_ValveActValveTypes_SykamValves.
*
*  \param      *pu16_CurrentPosition            The pointer at which address the <b>valve position array index</b> is written to.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActSupportFuncs_StatusDataSet (ushort *pu16_CurrentEncoderEdge, ushort *pu16_CurrentPosition)
*
*  \brief      This function sets some status data of valve actuator to register bank and permanent memory. The status data 
*              comprises the internal motor position \@ \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_POS_IN_USTEPS, 
*              the total amount of valve switches \@ \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__TOTAL_SWITCH_COUNTER as well as
*              given function arguments (valve position and position index).
*
*
*  \param      *pu16_CurrentEncoderEdge         The pointer \em contents, i.e. the current <b>valve encoder edge</b>, is written 
*                                               to register bank address \@ \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_ENCODER_EDGE
*                                               and permanent memory.
*
*  \param      *pu16_CurrentPosition            The pointer \em contents, i.e. the current <b>valve position</b>, is written
*                                               register bank address \@ \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_POSITION
*                                               and permanent memory.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActSupportFuncs_ExecuteMovement (ushort *pu16_CurrentEncoderEdge, ushort *pu16_CurrentPosition, ushort u16_CurrentValveType)
*
*  \brief      This function moves the valve to position \em pu16_CurrentEncoderEdge. It one after another calls \ref EncGapDiskEdgeGoto, 
*              \ref ValveActSupportFuncs_StatusDataSet, \ref ValveActSupportFuncs_ValveActStateRW and finally sends a message to 
*              task \ref TSK_ValveActSupportTasks_GenerateInjectSignal if the selected valve is an inject valve.
*
*
*  \param      *pu16_CurrentEncoderEdge         The pointer \em contents, i.e. the desired <b>valve encoder edge</b>.
*
*  \param      *pu16_CurrentPosition            The pointer \em contents, i.e. the desired <b>valve position</b>.
*
*  \param      u16_CurrentValveType             This is the current/selected valve type.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActSupportFuncs_DeviceParamFlagsRW (uchar u8_BitMask, bool b_RW, uchar *pu8_Result)
*
*  \brief      Reads or writes valve actuator device <em>parameter flags</em> into status register bank dependent on \em b_RW. 
*              The flags correspond to following status register bank entries: \em u16_DeviceParamsSaved\@ 
*              \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__DEVICE_PARAMS_SAVED and \em u16_DeviceParamsLoaded\@ 
*              \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__DEVICE_PARAMS_LOADED.
*
*
*  \param      u8_BitMask     This bit mask indicates which flags shall be read or written to register bank. E.g.
*                             to read all params flags \em u8_BitMask must be equal to <em>u8_BitMask = 
*                             \ref VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_LOADED | \ref VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_SAVED</em>. 
*                             To set/clear flags the bit mask is for example <em>u8_BitMask = 
*                             \ref VALVE_ACT_SUPPORT_FUNCS__SET_FLAG_LOADED | \ref VALVE_ACT_SUPPORT_FUNCS__CLEAR_FLAG_SAVED</em>.
*
*  \param      b_RW           The write/read flag. Possibilities: <em>b_RW = \ref VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE</em> \b or 
*                             <em>b_RW = \ref VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ</em>.
*                             Other flags are ignored an result in an error. If choosing write or read flag, also choose 
*                             appropriate flags for \em u8_BitMask.
*
*  \param      *pu8_Result    An pointer at which address the result is written if user has choosen read flags 
*                             <em>b_RW = \ref VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ</em>. 
*                             If at least one flag is set in register bank the content of pu8_Result is set to 0x01. 
*                             In case of <em>b_RW = \ref VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE</em>, the content of pu8_Result
*                             can be ignored.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActSupportFuncs_CalibrationFlagsRW (uchar u8_BitMask, bool b_RW, uchar *pu8_Result)
*
*  \brief      Reads or writes the valve actuator <em>calibration flags</em> into status register bank dependent on \em b_RW.
*              The flags correspond to following status register bank entries: \em u16_CalibrationFinished\@ 
*              \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CALIBRATION_FINISHED.
*
*
*  \param      u8_BitMask     This bit mask indicates which flags shall be read or written to register bank. E. g.
*                             to read calibration flag \em u8_BitMask must be equel to <em>u8_BitMask = 
*                             \ref VALVE_ACT_SUPPORT_FUNCS__READ_FLAG_FINISHED</em>. 
*                             To set/clear the flag, bit mask must be <em>u8_BitMask = 
*                             \ref VALVE_ACT_SUPPORT_FUNCS__CLEAR_FLAG_FINISHED</em>.
*
*  \param      b_RW           The write/read flag. Possibilities: <em>b_RW = \ref VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE</em> 
*                             \b or <em>b_RW = \ref VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ</em>. 
*                             Other flags are ignored an result in an error.
*
*  \param      *pu8_Result    An pointer at which address the result is written if user has choosen read flags <em>b_RW = 
*                             \ref VALVE_ACT_SUPPORT_FUNCS__FLAGS_READ</em>.
*                             If at least one flag is set in register bank the content of pu8_Result is set to 0x01. In case 
*                             of <em>b_RW = \ref VALVE_ACT_SUPPORT_FUNCS__FLAGS_WRITE</em> the content of pu8_Result can be 
*                             ignored.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActSupportFuncs_ValveActStateRW  (ushort *pu16_ValveActState,  bool b_RW)
*
*  \brief      This function reads/writes the valve actuator state of/from \em pu16_ValveActState dependent on \em b_RW
*
*
*  \param      *pu16_ValveActState              The pointer contents is the desired <b>valve actuator state</b> from enum
*                                               \ref _E_ValveActRegisters_ValveActStates. If it read or written depends on \em b_RW.
*
*  \param      b_RW                             This is the flag which indicates either to read, i.e. 
*                                               <em>b_RW = VALVE_ACT_SUPPORT_FUNCS__READ</em> or to write, i.e. 
*                                               <em>b_RW = VALVE_ACT_SUPPORT_FUNCS__WRITE</em> the content of \em pu16_ValveActState
*                                               into register bank.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
