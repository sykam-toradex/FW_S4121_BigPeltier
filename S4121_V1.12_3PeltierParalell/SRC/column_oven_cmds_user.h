/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_cmds_user.h
*
*  \brief               This is the header file of \ref column_oven_cmds_user.c.
*                       It contains the \em user modbus command function prototypes.
*
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2011-02-03     Initial version, imported from S6510_cmd_functions.h.
*  \date                2011-08-10     Renamed file from \em S4121_cmd_functions.h to \em column_oven_cmds_user.h.
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
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef COLUMN_OVEN_CMDS_USER__H
#define COLUMN_OVEN_CMDS_USER__H
//---------------------------------------------- defines -----------------------------------------------------------------------------
#define  COLUMN_OVEN_CMDS_USER__CMD_ARG__HEAT_COOL_PROGRAM_STOP                              (0x0001)
#define  COLUMN_OVEN_CMDS_USER__CMD_ARG__HEAT_COOL_CALIBRATION_START                         (0x0001)
#define  COLUMN_OVEN_CMDS_USER__CMD_ARG__HEAT_COOL_PERFORMANCE_TEST_START                    (0x0001)
#define  COLUMN_OVEN_CMDS_USER__CMD_ARG__GAS_SENSOR_ALARM_ENABLE                             (0x0001)
#define  COLUMN_OVEN_CMDS_USER__CMD_ARG__GAS_SENSOR_ALARM_DISABLE                            (0x0002)
#define  COLUMN_OVEN_CMDS_USER__CODE_HEAT_COOL_PROGRAM_STORED                                (0x8000)
#define  COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED                                (0xF000)
#define  COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_STORED_ID                             (0x03FF)
#define  COLUMN_OVEN_CMDS_USER__MASK_HEAT_COOL_PROGRAM_START_DIRECTLY                        (0x8000)
#define  COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_CURRENT_BUT_INACTIVE    (0x0001)
#define  COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_CURRENT_AND_ACTIVE      (0x0002)
#define  COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_CURRENT_AND_IN_HOLD     (0x0003)
#define  COLUMN_OVEN_CMDS_USER__FLAG_DESIRED_HEAT_COOL_PROGRAM_ID_IS_NOT_CURRENT             (0x0004)
#define  COLUMN_OVEN_CMDS_USER__DEFAULT_HEAT_COOL_PROGRAM_ID                                 (0x0001)
//---------------------------------------------- typedefs --------------------------------------------------------------------------------------
typedef ushort (*tfp_ColumnOvenCmdsUser_CMD) (ushort, ulong*);
//---------------------------------------------- externals defined in this module --------------------------------------------------------------
extern   tfp_ColumnOvenCmdsUser_CMD   fpA_ColumnOvenCmdsUser_CMDs[];
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
// user commands
ushort   ColumnOvenCmdsUser_HeatCoolProgramStart            (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsUser_HeatCoolProgramStop             (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsUser_HeatCoolProgramDelete           (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsUser_HeatCoolProgramSave             (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsUser_HeatCoolProgramSelectToModify   (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsUser_HeatCoolCalibrationStart        (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
#endif
// docu of user command functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         uhort ColumnOvenCmdsUser_HeatCoolProgramStart (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function starts a heat/cool program with passed identifier \b or controls the column oven
*              temperature to one set point. Before a program is started by user it is possible to modify a program by
*              calling command \ref ColumnOvenCmdsUser_HeatCoolProgramSelectToModify. Finally for usage the program must
*              be stored by calling command \ref ColumnOvenCmdsUser_HeatCoolProgramSave.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be equal to a valid heat/cool program identifier
*                                      \b if device option \ref COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM and
*                                      \ref COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL is set. In other cases the
*                                      passed identifier is equal to an temperature set point in [°C] times ten. For example
*                                      \em u16_ModbusRxData = 302 is equal to 30.2 °C.
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
*  \fn         uhort ColumnOvenCmdsUser_HeatCoolProgramStop (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function stops the heat/cool program, which is in hold/run mode, with passed identifier. \n
*              \b Note: The function is only available if device option is equal to \ref COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM
*              and \ref COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be greater than zero and less than
*                                      \ref AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID for execution.
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
*  \fn         uhort ColumnOvenCmdsUser_HeatCoolProgramDelete (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function resets a heat/cool program with passed identifier to default values. Default means
*              only the hold temperature (\ref AT91SAM7X__DEFAULT_HEAT_COOL_PROGRAM_SET_POINT_IN_DEGREE_MUL_TEN) for section 0. \n
*              \b Note: The function is only available if device option is equal to \ref COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM
*              and \ref COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be greater than zero and less than
*                                      \ref AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID for execution.
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
*  \fn         uhort ColumnOvenCmdsUser_HeatCoolProgramSave (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function saves a heat/cool program with passed identifier to permanent memory. \n
*              \b Note: The function is only available if device option is equal to \ref COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM
*              and \ref COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be greater than zero and less than
*                                      \ref AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID for execution.
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
*  \fn         uhort ColumnOvenCmdsUser_HeatCoolProgramSelectToModify (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function selects a heat/cool program with passed identifier for modification. This means the entire
*              program content is copied (or loaded from fixed memory is not active) to register bank place
*              \ref COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__HEAT_COOL_PROGRAM_TO_MODIFY. There it can be modified and finally
*              it must be save by calling command \ref ColumnOvenCmdsUser_HeatCoolProgramSave. \n
*              \b Note: The function is only available if device option is equal to \ref COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM
*              and \ref COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be greater than zero and less than
*                                      \ref AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID for execution.
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
*  \fn         uhort ColumnOvenCmdsUser_HeatCoolCalibrationStart (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function is not implemented at moment.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. Though this function is not implemented the passed
*                                      command argument has no meaning.
*
*  \param      *pu32_ErrorCodeSW_Sub   This software sub error code is set if any error occurred inside this command.
*
*
*  \return     Returns an ushort software base error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
