/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_cmds_reg.h
*
*  \brief               This is the header file of \ref valve_act_cmds_reg.c.
*                       It contains the \em register modbus command function prototypes.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-10     Initial version extracted from S6510_cmd_functions.h.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef VALVE_ACT_CMDS_REG__H
#define VALVE_ACT_CMDS_REG__H


//---------------------------------------------- typedefs --------------------------------------------------------------------------------------

typedef ushort (*tfp_ValveActCmdsReg_REGCMD) (ushort, ulong*);

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   tfp_ValveActCmdsReg_REGCMD   fpA_ValveActCmdsReg_REGCMDs_DeviceParamsExtern[];
extern   tfp_ValveActCmdsReg_REGCMD   fpA_ValveActCmdsReg_REGCMDs_DeviceParamsIntern[];

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

ushort   ValveActCmdsReg_Cal_UStepsInit    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_USPS_Init     (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_UStepsAcc       (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_UStepsBreak    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_USPS_Start     (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_USPS_Run       (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_Mode    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_UStepsFullCircle     (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_TrueGapNumber       (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_MinUStepsThreshold    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_EdgeTypeTriggeredOn     (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_Tol_UStepsSameGapsCal       (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_Cal_Tol_UStepsSameGapsRun    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_DAC_HoldMotorDriver    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_DAC_RunMotorDriver     (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsReg_CurrentValveType       (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);


#endif


// docu of register command functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort ValveActCmdsReg_DAC_HoldMotorDriver (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This function is called by owner or service technician via modbus (service tool) if writing a value to 
*              \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DAC_HOLD_MOTOR_DRIVER. It sets the DAC data input value, which 
*              output voltage is used as hold voltage (current) of step motor. The value shall be higher than 
*              \ref AT91SAM7X__STPCLKDIR0_DAC_MIN and lower than \ref AT91SAM7X__STPCLKDIR0_DAC_MAX. It shall be also 
*              \b lower or \b equal to value at register bank address \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DAC_RUN_MOTOR_DRIVER.
*
*
*  \param      u16_ModbusRxData           The DAC data input value for correponding \b hold voltage (current) of step motor.
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
*  \fn         ushort ValveActCmdsReg_DAC_RunMotorDriver (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This function is called by owner or service technician via modbus (service tool) if writing a value to 
*              \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DAC_RUN_MOTOR_DRIVER. It sets the DAC data input value, 
*              which output voltage is used as run voltage (current) of step motor. The value shall be higher than
*              \ref AT91SAM7X__STPCLKDIR0_DAC_MIN and lower than \ref AT91SAM7X__STPCLKDIR0_DAC_MAX. It shall be also 
*              \b higher or \b equal to value at register bank address \ref VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DAC_HOLD_MOTOR_DRIVER.
*
*
*  \param      u16_ModbusRxData           The DAC data input value for correponding \b run voltage (current) of step motor.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsReg_DAC_HoldMotorDriver.
*
*
*  \return     See \ref ValveActCmdsReg_DAC_HoldMotorDriver.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort ValveActCmdsReg_CurrentValveType (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This function is called whether by user via valve actuator display, if a new valve from menu is selected, \em or by 
*              owner/service technician via modbus (service tool). The function changes the current valve type and calibrates device
*              with default device parameters. Resulting calibration data and valve actuator device parameters are stored to 
*              permanent memory as well as to valve actuator register bank.
*
*
*  \param      u16_ModbusRxData           The desired valve type. Only valid if lower than \ref VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsReg_DAC_HoldMotorDriver.
*
*
*  \return     See \ref ValveActCmdsReg_DAC_HoldMotorDriver.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
