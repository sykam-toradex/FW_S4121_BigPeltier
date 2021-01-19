/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_cmds_owner.h
*
*  \brief               This is the header file of \ref valve_act_cmds_owner.c.
*                       It contains the \em owner (manufacturer) modbus command function prototypes.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-10     Initial version extracted from S6510_cmd_functions.h
*  \date                2012-11-07     Removed command function prototypes and argument defines of "ValveActCmdsOwner_CalDataLoad"  
*                                      and "ValveActCmdsOwner_CalDataLoad". Adapted/corrected function documentation.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef VALVE_ACT_CMDS_OWNER__H
#define VALVE_ACT_CMDS_OWNER__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

#define  VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD              (0x0001)
#define  VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD_DEFAULT      (0x0001)
#define  VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_SAVE              (0x0001)
#define  VALVE_ACT_CMDS_OWNER__CMD_ARG__CAL_WITH_DEVICE_PARAMS          (0x0001)
#define  VALVE_ACT_CMDS_OWNER__CMD_ARG__CAL_WITH_DEFAULT_PARAMS         (0x0001)
#define  VALVE_ACT_CMDS_OWNER__CMD_ARG__RESET_TOTAL_SWITCH_COUNTER      (0x0001)
#define  VALVE_ACT_CMDS_OWNER__CMD_ARG__STATUS_DATA_LOAD                (0x0001)

// FRAM codes
#define  VALVE_ACT_CMDS_OWNER__FRAM_CODE__DEVICE_PARAMS_SAVED           (0xCBCB)

//---------------------------------------------- typedefs --------------------------------------------------------------------------------------

typedef ushort (*tfp_ValveActCmdsOwner_CMD) (ushort, ulong*);

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   const ushort   cu16_ValveActCmdsOwner__DelayAfterMove;

extern                  tfp_ValveActCmdsOwner_CMD   fpA_ValveActCmdsOwner_CMDs[];

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

// owner (manufacturer) commands
ushort   ValveActCmdsOwner_DeviceParamsLoad           (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsOwner_DeviceParamsLoadDefault    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsOwner_DeviceParamsSave           (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsOwner_StatusDataLoad             (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsOwner_CalWithDeviceParams        (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsOwner_CalWithDefaultParams       (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsOwner_ResetTotalSwitchCounter    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort ValveActCmdsOwner_DeviceParamsLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Loads external and internal device parameters from fixed memory.
*              The function is used at startup to get and set the motor parameters which are used for the calibration
*              of valve actuator and later on to move the step motor.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      This software sub error code is set into valve actuator device status data of register
*                                         bank if any error occurs inside this command.
*
*
*  \return     Returns an ulong software \em base error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort ValveActCmdsOwner_DeviceParamsLoadDefault (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Loads (sets) default external and internal device parameters.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD_DEFAULT to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*
*  \return     See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ValveActCmdsOwner_DeviceParamsSave (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Saves external and internal device parameters to fixed memory.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref VALVE_ACT_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_SAVE to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*
*  \return     See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ValveActCmdsOwner_StatusDataLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Loads external and internal device status data from fixed memory. Flags of valve actuator 
*              are not set from fixed memory, i. e. they are not overwritten by this function. These flags 
*              are set at run-time and never stored to permanent memory.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref VALVE_ACT_CMDS_OWNER__CMD_ARG__STATUS_DATA_LOAD to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*
*  \return     See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort ValveActCmdsOwner_CalWithDeviceParams (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Calibrates valve actuator with current device parameters. If not changed by user the default 
*              calibration (motor) parameters are taken. The calibration routine finishes at \b home position.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref VALVE_ACT_CMDS_OWNER__CMD_ARG__CAL_WITH_DEVICE_PARAMS to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*
*  \return     See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort ValveActCmdsOwner_CalWithDefaultParams (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Calibrates valve actuator with default device parameters which can be found in \ref AT91SAM7X.h and 
*              in \ref valve_act_registers_funcs.c. The calibration routine finishes at \b home position.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref VALVE_ACT_CMDS_OWNER__CMD_ARG__CAL_WITH_DEFAULT_PARAMS to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*
*  \return     See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort ValveActCmdsOwner_ResetTotalSwitchCounter (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This function simply resets the valve actuator 32-bit switching counter of external status register bank.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref VALVE_ACT_CMDS_OWNER__CMD_ARG__RESET_TOTAL_SWITCH_COUNTER to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*
*  \return     See \ref ValveActCmdsOwner_DeviceParamsLoad.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
