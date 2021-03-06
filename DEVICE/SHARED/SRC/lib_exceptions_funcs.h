/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                lib_exceptions_funcs.h
*
*  \brief               This is the header file of \ref lib_exceptions_funcs.c.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-07-20     Initial version.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef LIB_EXCEPTIONS_FUNCS__H
#define LIB_EXCEPTIONS_FUNCS__H


//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

void     LibExceptionsFuncs_ExceptionSet (tpS_LibExceptions_Exceptions pS_Exceptions);


#endif


/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         void LibExceptionsFuncs_ExceptionSet (tpS_LibExceptions_Exceptions pS_Exceptions)
*
*  \brief      This function sets the software base and sub error code(s) to register bank starting
*              \@ \ref COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE1 and \@
*              \ref COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB1. \n
*              The function kills the task \ref TSK_UtilsTasks_Heartbeat if any errors occurs inside
*              this function itself, i.e. the alive board LED does not blink anymore. \n 
*              Future extension of this function will be the handling of application errors an warnings.
*
*
*  \param      pS_Exceptions        A pointer to an exception structure \ref tS_LibExceptions_Exceptions. It contains the
*                                   software base and sub error code as well as a warning code (which is not set into 
*                                   register bank.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
