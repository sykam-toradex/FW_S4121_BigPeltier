/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_support_funcs.h
*
*  \brief               Header file for \ref common_support_funcs.c
*
*  \author              Ruediger Radtke
*  \version             1.1
*
*  \date                2011-08-23     Initial version.
*  \date                2012-08-21     <em>File version 1.1</em>. Changed function argument for \ref CommonSupportFuncs_ErrorStringSW_FindAndSet
*                                      and extended function to search for strings of a sub error code, in case no string is found
*                                      for a base error code.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef COMMON_SUPPORT_FUNCS__H
#define COMMON_SUPPORT_FUNCS__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

#define  COMMON_SUPPORT_FUNCS__WRITE      (1)
#define  COMMON_SUPPORT_FUNCS__READ       (0)

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   OS_SEM   SEM_CommonSupportFuncs_DeviceRunTimeCounter;

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

ulong    CommonSupportFuncs_InitTasks                 (void);
ulong    CommonSupportFuncs_ErrorStringSW_FindAndSet  (tS_LibExceptions_Exceptions   *pS_Exceptions);
ulong    CommonSupportFuncs_ErrorStringSW_Clear       (void);
ulong    CommonSupportFuncs_ErrorCodeSW_ClearCurrPair (ushort *pu16_NumErrorsRemain);
//ulong    CommonSupportFuncs_DeviceStateRW             (ushort *pu16_DeviceState, bool b_RW);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonSupportFuncs_InitTasks (void)
*
*  \brief      This function initializes all tasks of file \ref common_support_tasks.c. Stacks, semaphore or mailbox names
*              can be adapted for tasks of named file inside this init function.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonSupportFuncs_ErrorStringSW_FindAndSet (tS_LibExceptions_Exceptions *pS_Exceptions)
*
*  \brief      This function finds a corresponding error string out of \ref _S_LibExceptions_ErrorStringsSW, firstly for
*              a string of base error code, if failed for sub error string and if there exist no corresponding string
*              the string <em>Unknown error</em> is set.
*
*  \param      *pS_Exceptions             A pointer to exception structure of \ref tS_LibExceptions_Exceptions.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonSupportFuncs_ErrorStringSW_Clear (void)
*
*  \brief      This function clears the software error string from common register bank.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonSupportFuncs_ErrorCodeSW_ClearCurrPair (ushort *pu16_NumErrorsRemain)
*
*  \brief      This function clears latest software base and sub error code from common register bank.
*
*  \param      *pu16_NumErrorsRemain      This value is set to number of remaining error in common register bank. It can be
*                                         used to execute this function inside a loop as long as all error are removed.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonSupportFuncs_DeviceStateRW (ushort *pu16_DeviceState, bool b_RW)
*
*  \brief      This function reads or writes device state \em pu16_DeviceState of enum \ref _E_CommonRegisters_DeviceStates into/from 
*              common register bank.
*
*  \param      *pu16_DeviceState          The device state from enum \ref _E_CommonRegisters_DeviceStates to read or write.
*
*  \param      b_RW                       This boolean value specifies if the function shall read (\ref COMMON_SUPPORT_FUNCS__READ)
*                                         or write (\ref COMMON_SUPPORT_FUNCS__WRITE) the common device state.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
