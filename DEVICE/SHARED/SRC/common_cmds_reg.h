/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_cmds_reg.h
*
*  \brief               This is the header file of \ref common_cmds_reg.c.
*                       It contains the \em register modbus command function prototypes.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-07-29     Initial version.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef COMMON_CMDS_REG__H
#define COMMON_CMDS_REG__H


//---------------------------------------------- typedefs --------------------------------------------------------------------------------------

typedef ushort (*tfp_CommonCmdsReg_REGCMD) (ushort, ulong*);

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   tfp_CommonCmdsReg_REGCMD   fpA_CommonCmdsReg_REGCMDs_DeviceParams[]; 

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

// register commands
ushort CommonCmdsReg_DeviceActiveInterface   (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ushort CommonCmdsReg_DeviceActiveInterface (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This function selects another active interface from \ref _E_CommonRegisters_DeviceActiveInterfaceList
*              and blocks execution of other device interfaces. It is called when user selects another device interface
*              via display or if owner, service technician or user selects another device interface with writing to 
*              appropriate holding register. \n
*              Exception: User commands are excutable via RS232 though 'display' is selected. Software intern it can 
*              not be distinguished between modbus commands sends from SPI (display) or RS232. Owner functions are 
*              generally executable independent of active device interface. \n
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to a valid active device interface
*                                         of \ref _E_CommonRegisters_DeviceActiveInterfaceList to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref CommonCmdsOwner_SykamDeviceDataLoad.
*
*
*  \return     Returns an ulong software base error code which is greater than zero if any error occurred. See \ref tU_LibExceptions_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
