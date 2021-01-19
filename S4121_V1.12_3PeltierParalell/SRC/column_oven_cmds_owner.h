/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_cmds_owner.h
*
*  \brief               This is the header file of \ref column_oven_cmds_owner.c.
*                       It contains the \em owner (manufacturer) modbus command function prototypes as well
*                       as the possible active device interface names in \ref _E_ActiveInterfaceList.
*
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2011-02-03     Initial version. Imported S6510_cmd_functions.h.
*  \date                2011-08-10     Renamed file from \em S4121_cmd_functions.h to \em column_oven_cmds_owner.h.
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
#ifndef COLUMN_OVEN_CMDS_OWNER__H
#define COLUMN_OVEN_CMDS_OWNER__H
//---------------------------------------------- defines ---------------------------------------------------------------------------------------
#define  COLUMN_OVEN_CMDS_OWNER__CMD_ARG__CONTROL_SECTION_PARAMS_SAVE   (0x0001)
#define  COLUMN_OVEN_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD            (0x0001)
#define  COLUMN_OVEN_CMDS_OWNER__CMD_ARG__STATUS_DATA_LOAD              (0x0001)
#define  COLUMN_OVEN_CMDS_OWNER__CMD_ARG__HEAT_COOL_PROGRAMS_RESET      (0x0001)
//---------------------------------------------- typedefs --------------------------------------------------------------------------------------
typedef ushort (*tfp_ColumnOvenCmdsOwner_CMD) (ushort, ulong*);
//---------------------------------------------- externals defined in this module --------------------------------------------------------------
extern   tfp_ColumnOvenCmdsOwner_CMD   fpA_ColumnOvenCmdsOwner_CMDs[];
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
// owner (manufacturer) commands
ushort   ColumnOvenCmdsOwner_ControlParamsSave           (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsOwner_DeviceParamsLoad            (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsOwner_StatusDataLoad              (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ColumnOvenCmdsOwner_HeatCoolProgramsReset       (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
#endif
// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         uhort ColumnOvenCmdsOwner_ControlParamsSave (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function saves the modified control section params to permanent memory for currently
*              only one device. During device initialization device option is checked by function calling of \ref
*              ColumnOvenCmdsOwner_DeviceParamsLoad. Therefore the stored control section parameters can used by
*              choosen device type.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be equal to
*                                      \ref COLUMN_OVEN_CMDS_OWNER__CMD_ARG__CONTROL_SECTION_PARAMS_SAVE for execution.
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
*  \fn         uhort ColumnOvenCmdsOwner_DeviceParamsLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function loads device parameters from permanent memory to register bank. It set defaults
*              for temperature unit/offset or temperature set point reached band if never stored on permanent memory.
*              It checks internal device params for errors and sets defaults if necessary. Finally some status flags
*              are set (\ref COLUMN_OVEN_REGISTERS__DEVICE_PARAMS_LOADED and \ref COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__LOADED.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be equal to
*                                      \ref COLUMN_OVEN_CMDS_OWNER__CMD_ARG__DEVICE_PARAMS_LOAD for execution.
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
*  \fn         uhort ColumnOvenCmdsOwner_StatusDataLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function loads external status data without flags. This is mainly the heat/cool program
*              which was used at last.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be equal to
*                                      \ref COLUMN_OVEN_CMDS_OWNER__CMD_ARG__STATUS_DATA_LOAD for execution.
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
*  \fn         uhort ColumnOvenCmdsOwner_HeatCoolProgramsReset (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command function resets all heat/cool program to default values. Default means only a hold temperature
*              of \ref AT91SAM7X__DEFAULT_HEAT_COOL_PROGRAM_SET_POINT_IN_DEGREE_MUL_TEN.
*
*
*  \param      u16_ModbusRxData        This is the modbus PDU data. It must be equal to
*                                      \ref COLUMN_OVEN_CMDS_OWNER__CMD_ARG__HEAT_COOL_PROGRAMS_RESET for execution.
*
*  \param      *pu32_ErrorCodeSW_Sub   This software sub error code is set if any error occurred inside this command.
*
*
*  \return     Returns an ushort software base error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
