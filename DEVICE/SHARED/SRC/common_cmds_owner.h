/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_cmds_owner.h
*
*  \brief               This is the header file of \ref common_cmds_owner.c.
*                       It contains the \em owner (manufacturer) command function prototypes.
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

#ifndef COMMON_CMDS_OWNER__H
#define COMMON_CMDS_OWNER__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

#define  COMMON_CMDS_OWNER__WRITE                                    (1)
#define  COMMON_CMDS_OWNER__READ                                     (0)
#define  COMMON_CMDS_OWNER__CMD_ARG__SYKAM_DEVICE_DATA_LOAD          (0x0001)
#define  COMMON_CMDS_OWNER__CMD_ARG__SYKAM_DEVICE_DATA_SAVE          (0x0001)
#define  COMMON_CMDS_OWNER__CMD_ARG__DEVICE_RUN_TIME_COUNTER_RESET   (0x0001)
#define  COMMON_CMDS_OWNER__CMD_ARG__ERROR_CODE_SW_CLEAR_CURR        (0x0001)
#define  COMMON_CMDS_OWNER__CMD_ARG__ERROR_CODE_SW_CLEAR_ALL         (0x0001)

//---------------------------------------------- typedefs --------------------------------------------------------------------------------------

typedef ushort (*tfp_CommonCmdsOwner_CMD) (ushort, ulong*);

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   tfp_CommonCmdsOwner_CMD   fpA_CommonCmdsOwner_CMDs[];

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

// owner (manufacturer) commands
ushort   CommonCmdsOwner_SykamDeviceDataLoad          (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   CommonCmdsOwner_SykamDeviceDataSave          (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   CommonCmdsOwner_ModbusSetSlaveAddress        (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   CommonCmdsOwner_DeviceRunTimeCounterReset    (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   CommonCmdsOwner_ErrorCodeSW_ClearCurr        (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   CommonCmdsOwner_ErrorCodeSW_ClearAll         (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);
ushort   CommonCmdsOwner_SykamDeviceDataWriteEnable   (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonCmdsOwner_SykamDeviceDataLoad (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Loads Sykam device data from permanent memory into register bank. Functions shall be called once at every start-up.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to \ref COMMON_CMDS_OWNER__CMD_ARG__SYKAM_DEVICE_DATA_LOAD
*                                         to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      This software sub error code is set into common device status data of register
*                                         bank if any error occurs inside this command.
*
*
*  \return     Returns an ulong software base error code which is greater than zero if any error occurred. See \ref tU_LibExceptions_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonCmdsOwner_SykamDeviceDataSave (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Stores Sykam device data from register bank to permanent memory. Notice that \b only some entries of sykam device 
*              data can be changed. It exists firstly a modbus prefilter and secondly a consistency check inside this command 
*              function which checks changed data. \n
*              If wrong parameters are changed no storage is possible and an error occurs. Look for <em>non constant</em> parts 
*              in \ref sykam_cfg.c to get know which data parts can be changed. Sykam device data stored once are loaded at 
*              next device startup.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to \ref COMMON_CMDS_OWNER__CMD_ARG__SYKAM_DEVICE_DATA_SAVE
*                                         to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref CommonCmdsOwner_SykamDeviceDataLoad.
*
*
*  \return     Returns an ulong software base error code which is greater than zero if any error occurred. See \ref tU_LibExceptions_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         CommonCmdsOwner_ModbusSetSlaveAddress (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Sets modbus slave ID to desired one. Slave ID is stored on permanent memory and loaded at next startup
*              as default modbus slave ID. That means every modbus request must address the new slave ID.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to a valid modbus slave ID
*                                         to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref CommonCmdsOwner_SykamDeviceDataLoad.
*
*
*  \return     Returns an ulong software base error code which is greater than zero if any error occurred. See \ref tU_LibExceptions_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         CommonCmdsOwner_DeviceRunTimeCounterReset (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      Command resets the common device run time counter. The new time (zero) is also stored on permanent
*              memory. The counter itself is than steadily incremented as usual.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to 
*                                         \ref COMMON_CMDS_OWNER__CMD_ARG__DEVICE_RUN_TIME_COUNTER_RESET to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref CommonCmdsOwner_SykamDeviceDataLoad.
*
*
*  \return     Returns an ulong software base error code which is greater than zero if any error occurred. See \ref tU_LibExceptions_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         CommonCmdsOwner_ErrorCodeSW_ClearCurr (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command clears the latest software error pair (base and sub error) from common register bank.
*              If two sub errors exist for one base error, two pairs (four 16 bit values) are deleted from common
*              register bank arcording underlying idea of software error code handling. Furthermore the corresponding
*              error string is deleted from common register bank and if there remain errors, a new one is set.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to \ref COMMON_CMDS_OWNER__CMD_ARG__ERROR_CODE_SW_CLEAR_CURR
*                                         to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref CommonCmdsOwner_SykamDeviceDataLoad.
*
*
*  \return     Returns an ulong software base error code which is greater than zero if any error occurred. See \ref tU_LibExceptions_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         CommonCmdsOwner_ErrorCodeSW_ClearAll (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command clears all software errors pairs from common register bank as well as the software 
*              error string.
*
*
*  \param      u16_ModbusRxData           This is the modbus PDU data. It must be equal to \ref COMMON_CMDS_OWNER__CMD_ARG__ERROR_CODE_SW_CLEAR_ALL
*                                         to execute this command.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref CommonCmdsOwner_SykamDeviceDataLoad.
*
*
*  \return     Returns an ulong software base error code which is greater than zero if any error occurred. See \ref tU_LibExceptions_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         CommonCmdsOwner_SykamDeviceDataWriteEnable (ushort u16_ModbusRxData, ulong *pu32_ErrorCodeSW_Sub)
*
*  \brief      This command enables write (disable write protection) for Sykam device data.
*
*
*  \param      u16_ModbusRxData           This is the write enable code. If correct write protection is switched off until saving Sykam device data.
*
*  \param      *pu32_ErrorCodeSW_Sub      See \ref CommonCmdsOwner_SykamDeviceDataLoad.
*
*
*  \return     Returns an ulong software base error code which is \b never greater than zero because an error cannot occur here. 
*              See \ref tU_LibExceptions_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
