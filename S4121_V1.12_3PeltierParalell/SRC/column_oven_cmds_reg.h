/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_cmds_reg.h
*
*  \brief               This is the header file of \ref column_oven_cmds_reg.c.
*                       It contains the \em register modbus command function prototypes as well
*                       as the possible active device interface names in \ref _E_ActiveInterfaceList.
*
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2010-06-23     Initial version.
*  \date                2011-02-03     Imported S6510_cmd_functions.h.
*  \date                2011-08-10     Renamed file from S4121_cmd_functions.h
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                ->>>> 2010-10-29     Tested all command and register command functions.
*
*  \todo                check "test" statement
*  \todo
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef COLUMN_OVEN_CMDS_REG__H
#define COLUMN_OVEN_CMDS_REG__H
//---------------------------------------------- defines -----------------------------------------------------------------------------
#define  COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_OFFSET_STORED                          (0x0001)
#define  COLUMN_OVEN_CMDS_REG__FLAG_TEMPERATURE_UNIT_STORED                            (0x0002)
#define  COLUMN_OVEN_CMDS_REG__FLAG_HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND_STORED    (0x0004)
#define  COLUMN_OVEN_CMDS_REG__MASK_DEVICE_PARAMS_STORED                               (0x0007)
#define  COLUMN_OVEN_CMDS_REG__CODE_DEVICE_PARAMS_STORED                               (0xA8E0)
//---------------------------------------------- typedefs --------------------------------------------------------------------------------------
typedef ushort (*tfp_ColumnOvenCmdsReg_REGCMD) ( ushort, ulong* );
//---------------------------------------------- externals defined in this module --------------------------------------------------------------
extern   tfp_ColumnOvenCmdsReg_REGCMD   fpA_ColumnOvenCmdsReg_REGCMDs_DeviceParamsExtern[];
extern   tfp_ColumnOvenCmdsReg_REGCMD   fpA_ColumnOvenCmdsReg_REGCMDs_DeviceParamsIntern[];
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
// register commands
ushort   ColumnOvenCmdsReg_ControlSectionParams                   (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsReg_GasSensorAlarmThresholdPPM             (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsReg_GasSensorCalibrationResistance         (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsReg_GasSensorAlarmEnabled                  (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsReg_TemperatureOffset                      (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsReg_TemperatureUnit                        (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsReg_HeatCoolProgramSetPointReachedBand     (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
#endif
// docu of register command functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         uhort ColumnOvenCmdsReg_ControlSectionParams (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This register command function stops a heat/cool program, ignoring current state, and set flag that control
*              sections parameters are modified.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. The value must fit into structure \ref _S_ColumnOvenRegisters_ControlSectionParams.
*
*  \param      *pu32_ErrorCodeSW_Sub   This software sub error code is set if any error occurred inside this command.
*
*
*  \return     Returns an ushort software base error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         uhort ColumnOvenCmdsReg_GasSensorAlarmThresholdPPM (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Stores the passed gas sensor alarm threshold ppm value to fixed memory. If remaining gas sensor parameters
*              were never set before, this function sets defaults into register bank and fixed memory.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It represents the desired gas sensor alarm threshold
*                                      value in ppm.
*
*  \param      *pu32_ErrorCodeSW_Sub   This software sub error code is set if any error occurred inside this command.
*
*
*  \return     Returns an ushort software base error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         uhort ColumnOvenCmdsReg_GasSensorCalibrationResistance (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function stores the gas sensor calibration resistance into fixed memory. Gas sensor calibration
*              resistance is determined at 20 °C with 300 ppm ethanol (== 3 ml ethanol). If remaining gas sensor
*              parameters were never set before, this function sets defaults into register bank and fixed memory.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. The calibration resistance can be read out from
*                                      register bank address \ref COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__GAS_SENSOR_CURRENT_RESISTANCE
*                                      when calibration is done.
*
*  \param      *pu32_ErrorCodeSW_Sub   This software sub error code is set if any error occurred inside this command.
*
*
*  \return     Returns an ushort software base error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         uhort ColumnOvenCmdsReg_GasSensorAlarmEnabled (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This function enables/disables the gas alarm. If gas sensor alarm is disabled the sensor resistance and
*              current ppm value is not refreshed anymore inside register bank until alarm enabling.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. Accepted values are \ref COLUMN_OVEN_CMDS_USER__CMD_ARG__GAS_SENSOR_ALARM_ENABLE
*                                      or \ref COLUMN_OVEN_CMDS_USER__CMD_ARG__GAS_SENSOR_ALARM_DISABLE.
*
*  \param      *pu32_ErrorCodeSW_Sub   This software sub error code is set if any error occurred inside this command.
*
*
*  \return     Returns an ushort software base error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         uhort ColumnOvenCmdsReg_TemperatureOffset (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function stores the passed temperature offset to fixed memory. It sets a flag that
*              temperature offset value was stored once.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be equal or less than
*                                      \ref AT91SAM7X__MAX_TEMPERATURE_OFFSET_MUL_TEN for execution. The offset value
*                                      itself is passed in [°C] multiplied by ten. I.e. \em u16_ModbusRxData = 15 equals
*                                      1.5 °C.
*
*  \param      *pu32_ErrorCodeSW_Sub   This software sub error code is set if any error occurred inside this command.
*
*
*  \return     Returns an ushort software base error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         uhort ColumnOvenCmdsReg_TemperatureUnit (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function stores the passed temperature unit to fixed memory. It sets a flag that
*              temperature unit value was stored once.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. The unit can be choosen between
*                                      \ref COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__CELSIUS and \ref COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__FAHRENHEIT.
*
*  \param      *pu32_ErrorCodeSW_Sub   This software sub error code is set if any error occurred inside this command.
*
*
*  \return     Returns an ushort software base error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         uhort ColumnOvenCmdsReg_HeatCoolProgramSetPointReachedBand (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function stores the new set point reached band value to fixed memory.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be greater than
*                                      \ref AT91SAM7X__MIN_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN and less than
*                                      \ref AT91SAM7X__MAX_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN. The value passed
*                                      is given in [°C] multiplied by ten. I.e. \em u16_ModbusRxData = 10 equals
*                                      1.0 °C.
*
*  \param      *pu32_ErrorCodeSW_Sub   This software sub error code is set if any error occurred inside this command.
*
*
*  \return     Returns an ushort software base error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
