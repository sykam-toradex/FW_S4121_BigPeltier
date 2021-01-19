/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_registers_funcs.h
*
*  \brief               Header file for \ref valve_act_registers_funcs.c
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2010-08-08     Initial version imported S6510_registers.h.
*
*
**//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef VALVE_ACT_REGISTERS_FUNCS__H
#define VALVE_ACT_REGISTERS_FUNCS__H


//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

ulong    ValveActRegistersFuncs_InitDeviceParams    (void);
ulong    ValveActRegistersFuncs_InitStatusData      (void);
ulong    ValveActRegistersFuncs_InitRegisterBank    (void);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActRegistersFuncs_InitRegisterBank (void)
*
*  \brief      Initializes the complete valve actuator register bank.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActRegistersFuncs_InitDeviceParams (void)
*
*  \brief      Initializes the valve actuator internal/external device params, i. e. motor params and valve type. 
*              Values are set to corresponding registers.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActRegistersFuncs_InitStatusData (void)
*
*  \brief      Initializes the valve actuator internal/external status data, e. g. device state, valve positions, 
*              calibration flags a. s. f. Values are set to corresponding registers.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
