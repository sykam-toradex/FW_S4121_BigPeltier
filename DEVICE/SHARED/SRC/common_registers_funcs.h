/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_registers_funcs.h
*
*  \brief               The header file of \ref common_registers_funcs.h.
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*
*  \date                2011-08-08     Initial version.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright (c) 2008-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef COMMON_REGISTERS_FUNCS__H
#define COMMON_REGISTERS_FUNCS__H


//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern const   ushort   cu16_DefaultActiveInterface;
extern         ushort   u16_CommonRegistersFuncs_SykamDeviceDataWriteEnableCode;

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

ulong CommonRegistersFuncs_InitRegisterBank                 (void);
ulong CommonRegistersFuncs_InitDeviceParams                 (void);
ulong CommonRegistersFuncs_InitDeviceStatusData             (void);
ulong CommonRegistersFuncs_InitSykamDeviceData              (void);

ulong CommonRegistersFuncs_SykamDeviceDataConsistencyCheck  (tpS_CommonRegisters_SykamDeviceData pS_CommonRegisters_SykamDeviceData);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonRegistersFuncs_InitRegisterBank (void)
*
*  \brief      Initializes complete common register bank.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonRegistersFuncs_InitSykamDeviceData (void)
*
*  \brief      Initializes Sykam device data, e.g. OEM id, device class etc. and sets data into corresponding registers.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonRegistersFuncs_InitDeviceParams (void)
*
*  \brief      Initializes common device parameter and set data into corresponding registers.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonRegistersFuncs_InitDeviceStatusData (void)
*
*  \brief      Initializes common device status data and sets data into corresponding registers.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonRegistersFuncs_SykamDeviceDataConsistencyCheck  (tpS_CommonRegisters_SykamDeviceData pS_CommonRegisters_SykamDeviceData)
*
*  \brief      The consistency check function is needed for commands \ref CommonCmdsOwner_SykamDeviceDataLoad and 
*              \ref CommonCmdsOwner_SykamDeviceDataLoad. Check inside first command is an \em additional check for consistency of 
*              loaded data from permanent memory. For the last stated command the check inside is compulsory if no modbus prefilter
*              is implemented! This is because the Sykam device data can be set by owner/service technician via RS232 but it is 
*              required by Sykam that only the none constant parts of \ref sykam_cfg.c shall be changed.
*              Therfore right before data is saved inside \ref CommonCmdsOwner_SykamDeviceDataSave the constant parts of Sykam 
*              device data will be compared to internal hard coded values.
*
*
*  \param      pS_CommonRegisters_SykamDeviceData     This is a structure pointer to \em structure of type \ref tS_CommonRegisters_SykamDeviceData
*                                                     inside which the register bank contents of Sykam device data is stored.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

