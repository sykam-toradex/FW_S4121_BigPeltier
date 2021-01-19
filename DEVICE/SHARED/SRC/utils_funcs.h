/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                utils_funcs.h
*
*  \brief               The header file for \ref utils_funcs.c
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-07-29     Initial version imported from S4121_utils.c
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef UTILS_FUNCS__H
#define UTILS_FUNCS__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

#define  UTILS_FUNCS__WRITE   (1)
#define  UTILS_FUNCS__READ    (0)

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   OS_TID   TID_UtilsFuncs_Heartbeat;

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

ulong    UtilsFuncs_InitTasks                (void);
ulong    UtilsFuncs_SwapBytes                (void    *pv_Dest,   ushort   u16_NumWords);
ulong    UtilsFuncs_SwapWords                (void    *pv_Dest,   ushort   u16_NumWords);
ulong    UtilsFuncs_CalcMean                 (float   *pf_Result, float    *pf_Data,      ushort u16_Count);
ulong    UtilsFuncs_FixedMemDataPackagesRW   (void    *pv_Data,   ushort   u16_NumBytes,  ushort u16_DestAddr, bool b_RW);
ulong    UtilsFuncs_FixedMemErase            (void);
ushort   UtilsFuncs_Dummy                    (ushort  u16_Dummy,  ulong    *pu32_Dummy);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong UtilsFuncs_InitTasks (void)
*
*  \brief      This function initializes all tasks of file \ref utils_tasks.c. Stacks, semaphore or mailbox names
*              can be adapted for tasks of named file inside this init function.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong UtilsFuncs_SwapBytes (void *pv_Dest, ushort u16_NumWords)
*
*  \brief      This function swaps (\em u16_NumWords * 2) bytes from start address \em *pv_Dest. Though this microcontroller 
*              works with little endian the swap of bytes or words is useful to show internal values to a host e.g. via RS232.
*
*
*  \param      *pv_Dest                   The destination address at which the swap function shall start.
*
*  \param      u16_NumWords               The number of words in which a byte swap is done. E.g. to swap the bytes of a long
*                                         0xAABBCCDD \em u16_NumWords shall be equal to 0x0002. The result will be 0xBBAADDCC.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong UtilsFuncs_SwapWords (void *pv_Dest, ushort u16_NumWords)
*
*  \brief      This function swaps \em u16_NumWords of words from start address \em *pv_Dest.
*
*
*  \param      *pv_Dest                   The destination address at which the swap function shall start.
*
*  \param      u16_NumWords               The number of words to swaps. E.g. to swap the words of a long value 0xAABBCCDD 
*                                         \em u16_NumWords shall be equal to 0x0002. The result will be 0xCCDDAABB.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong UtilsFuncs_CalcMean (float *pf_Result, float *pf_Data, ushort u16_Count)
*
*  \brief      This function calculates the mean of data located at address \em pf_Data for \em u16_Count and stores the 
*              result at \em pf_Result.
*
*
*  \param      *pf_Result                 At this address the result of mean calculation is stored.
*
*  \param      *pf_Data                   The data start address for calculation of mean.
*
*  \param      u16_Count                  This is the number of floats to include to calculation of mean starting from \em pf_Data.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong UtilsFuncs_FixedMemDataPackagesRW (void *pv_Data, ushort u16_NumBytes, ushort u16_DestAddr, bool b_RW)
*
*  \brief      This function writes/reads data blocks of size \em u16_DataSize to/from FRAM. It simplifies access to FRAM
*              and reduces code in application because of two different FRAM driver functions for read and write accesses.
*              It evaluates the return argument of driver function and reduces comparision overhead in application.
*
*
*  \param      *pv_Data                This is an void pointer at which data is written to or read from FRAM dependent on \em b_RW.
*
*  \param      u16_NumBytes            The number of bytes to write or read from FRAM.
*
*  \param      u16_DestAddr            The destination address to which the data is written or read from.
*
*  \param      b_RW                    The write/read flag. Possibilities: \em "b_RW = \ref UTILS_FUNCS__WRITE" \b or 
*                                      \em "b_RW = UTILS_FUNCS__READ". Other flags are ignored and result in an error.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
