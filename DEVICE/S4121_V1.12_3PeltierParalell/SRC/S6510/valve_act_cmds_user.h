/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_cmds_user.h
*
*  \brief               This is the header file of \ref valve_act_cmds_user.c.
*                       It contains the \em user modbus command function prototypes.
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

#ifndef VALVE_ACT_CMDS_USER__H
#define VALVE_ACT_CMDS_USER__H


//---------------------------------------------- defines -----------------------------------------------------------------------------

//#define  VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_HOME          (0x0001)
//#define  VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_NEXT          (0x0001)
//#define  VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_PREVIOUS      (0x0001)
#define  VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_HOME_POS          (0x0001)
#define  VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_NEXT_POS          (0x0001)
#define  VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_PREVIOUS_POS      (0x0001)


//---------------------------------------------- typedefs --------------------------------------------------------------------------------------

typedef ushort (*tfp_ValveActCmdsUser_CMD) (ushort, ulong*);

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   tfp_ValveActCmdsUser_CMD   fpA_ValveActCmdsUser_CMDs[];

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

// user commands
ushort   ValveActCmdsUser_GoToHomePos        (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsUser_GoToNextPos        (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsUser_GoToPreviousPos    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   ValveActCmdsUser_GoToInquiredPos    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);


#endif


// docu of user command functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort ValveActCmdsUser_GoToHomePos (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This function moves the valve to \b home position (middle gap of three equally spaced gaps) and sets position 
*              into valve actuator status register bank.
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_HOME_POS to execute this command.
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
*  \fn         ushort ValveActCmdsUser_GoToNextPos (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This function moves the valve to \b next position according to current valve type and sets position into 
*              valve actuator status register bank.
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_NEXT_POS to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsUser_GoToHomePos.
*
*
*  \return     See \ref ValveActCmdsUser_GoToHomePos.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort ValveActCmdsUser_GoToPreviousPos (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This function moves the valve to \b previous position according to current valve type and sets position 
*              into valve actuator status register bank.
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref VALVE_ACT_CMDS_USER__CMD_ARG__GO_TO_PREVIOUS to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsUser_GoToHomePos.
*
*
*  \return     See \ref ValveActCmdsUser_GoToHomePos.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort ValveActCmdsUser_GoToInquiredPos (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This function moves the valve to \b inquired position according to current valve type and sets position into
*              valve actuator status register bank. Not available but inquired positions lead to no valve movement. Inquired 
*              positions out of bounds, e. g. zero (0) and values greater than \ref VALVE_ACT_VALVE_TYPES__MAX_VALVE_POSITIONS 
*              lead to an modbus exception code via modbus prefilter.
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to to an valid valve position inside
*                                         bounds 1...\ref VALVE_ACT_VALVE_TYPES__MAX_VALVE_POSITIONS to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref ValveActCmdsUser_GoToHomePos.
*
*
*  \return     See \ref ValveActCmdsUser_GoToHomePos.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
