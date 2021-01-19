/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_registers_funcs.h
*
*  \brief               Header file for \ref column_oven_registers_funcs.c
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*
*  \date                2011-02-03     Initial version imported S6510_registers.h.
*  \date                2011-12-05     Extended initialization functions.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test
*
*  \todo                FILE OK
*
*
**//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef COLUMN_OVEN_REGISTERS_FUNCS__H
#define COLUMN_OVEN_REGISTERS_FUNCS__H
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
ulong    ColumnOvenRegistersFuncs_InitDeviceParams    (void);
ulong    ColumnOvenRegistersFuncs_InitStatusData      (void);
ulong    ColumnOvenRegistersFuncs_InitRegisterBank    (void);
#endif
// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ColumnOvenRegistersFuncs_InitDeviceParams (void)
*
*  \brief      This function initializes internal and external device paramerters of column oven. It sets defaults to register bank
*              entries where necessary.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ColumnOvenRegistersFuncs_InitStatusData (void)
*
*  \brief      This function initializes internal and external status data of column oven. It sets defaults to register bank
*              entries where necessary.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ColumnOvenRegistersFuncs_InitRegisterBank (void)
*
*  \brief      This function initializes complete register bank of column oven. It uses functions \ref ColumnOvenRegistersFuncs_InitDeviceParams
*              and \ref ColumnOvenRegistersFuncs_InitStatusData.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
