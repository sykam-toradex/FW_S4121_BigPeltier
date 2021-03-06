/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                utils_funcs.c
*
*  \brief               The file contains utility functions.
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-07-29     Initial version imported from S4121_utils.c
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
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <stdlib.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"

#include "..\..\..\ext\spi\nvram\fm25clss\fm25clss.h"
#include "..\..\..\ext\spi\nvram\fm25clss\fm25clssext.h"

#include "lib_exceptions.h"
#include "utils_funcs.h"
#include "utils_tasks.h"

#pragma import(__use_realtime_heap)             //< * this pragma causes the use of a faster memory allocation (internal) function */


//---------------------------------------------- global declarations/definitions ---------------------------------------------------------------

OS_TID   TID_UtilsFuncs_Heartbeat;

OS_TSK_STACK_USER(STK_TSK_UtilsFuncs_Heartbeat, 500);

//---------------------------------------------- externals defined in this file ----------------------------------------------------------------

const ushort   cu16_UtilsFuncs_TimeoutShort   =  AT91SAM7X__TICK_10;
const ushort   cu16_UtilsFuncs_TimeoutMedium  =  AT91SAM7X__TICK_20;
const ushort   cu16_UtilsFuncs_TimeoutLong    =  AT91SAM7X__TICK_30;

//---------------------------------------------- functions -------------------------------------------------------------------------------------

ulong UtilsFuncs_InitTasks (void)
{
   ulong u32_ErrorCodeSW_Sub = (ushort)LIB_EXCEPTIONS__ERROR_NONE;
   
   // let the control LED blink
   TID_UtilsFuncs_Heartbeat = os_tsk_create_user (TSK_UtilsTasks_Heartbeat, (uchar)OS_P_LOW, (void*)STK_TSK_UtilsFuncs_Heartbeat,
                                                  (ushort)(sizeof(STK_TSK_UtilsFuncs_Heartbeat)) );


   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong UtilsFuncs_SwapBytes (void *pv_Dest, ushort u16_NumWords)
{ 
   uchar    u8_Tmp; 
   uchar    *puc_Tmp;
   uchar    *puc_Dest            = (uchar *)pv_Dest;
   ushort   u16_Count;
   ulong    u32_ErrorCodeSW_Sub  = (ulong)LIB_EXCEPTIONS__ERROR_NONE;

   if ((pv_Dest) || (u16_NumWords == 0))
   {
      for( u16_Count = 1; u16_Count <= u16_NumWords; u16_Count++ )
      {  
         // swap the contents of src bytewise for u16_NumWords
         u8_Tmp         =  *puc_Dest;
         puc_Tmp        =  puc_Dest + 1;
         *puc_Dest++    =  *puc_Tmp; 
         *puc_Dest      =  u8_Tmp;
         *puc_Dest++;
      }
   }
   else
      u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__UTILS_FUNCS__SWAP_BYTES;


   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong UtilsFuncs_SwapWords (void *pv_Dest, ushort u16_NumWords)
{ 
   ushort   u16_Count, u16_Tmp;
   ushort   *pus_Tmp;
   ushort   *pus_Dest            = (ushort *)pv_Dest;
   ulong    u32_ErrorCodeSW_Sub  = (ulong)LIB_EXCEPTIONS__ERROR_NONE;


   if ((pv_Dest) || (u16_NumWords <= 1))
   {
      for( u16_Count = 1; u16_Count <= (u16_NumWords - 1); u16_Count++ )
      {  
         // swap the contents of src wordwise for u16_NumWords
         u16_Tmp        =  *pus_Dest;
         pus_Tmp        =  pus_Dest + 1;
         *pus_Dest++    =  *pus_Tmp; 
         *pus_Dest      =  u16_Tmp;
         *pus_Dest++;
      }
   }
   else
      u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__UTILS_FUNCS__SWAP_WORDS;


   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong UtilsFuncs_CalcMean (float *pf_Result, float *pf_Data, ushort u16_Count)
{
   ushort   u16_Iterations       =  0;
   ulong    u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;


   *pf_Result = 0.0;

   if( (pf_Result) && (pf_Data) )
   {
      for( u16_Iterations = 0; u16_Iterations < u16_Count; u16_Iterations++ )
      {
         *pf_Result += *pf_Data++;
      }
      *pf_Result /= u16_Count;
   }
   else
      u32_ErrorCodeSW_Sub = (ulong)LIB_EXCEPTIONS__ERROR__UTILS_FUNCS__CALC_MEAN;


   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong UtilsFuncs_FixedMemDataPackagesRW (void *pv_Data, ushort u16_NumBytes, ushort u16_DestAddr, bool b_RW)
{
   ulong    u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   uchar    u8_Unit              =  0;
   ushort   u16_Result;
   uchar    *pu8_CmpBuffer;
   
   switch (b_RW)
   {
      case UTILS_FUNCS__READ:

         // allocate temporary memory for a compare buffer
         pu8_CmpBuffer = (uchar *)calloc ((size_t)(u16_NumBytes * (FM25CLSS_NUM_MULTI_STORAGES - 1)), (size_t)sizeof(uchar));

         // if no memory left on heap return with an error
         if (!pu8_CmpBuffer)
         {
            u32_ErrorCodeSW_Sub = ((ulong)LIB_EXCEPTIONS__ERROR_SPECIAL__NO_SPACE_LEFT_ON_HEAP);
            break;
         }
         
         u16_Result = Fm25clssReadSafe (u8_Unit, pv_Data, (void *)(pu8_CmpBuffer), u16_NumBytes, u16_DestAddr, 
                                        cu16_UtilsFuncs_TimeoutLong);
         // free allocated memory
         free (pu8_CmpBuffer);

         if (u16_Result != u16_NumBytes)   // error case, return from function
            u32_ErrorCodeSW_Sub = ((ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__READ_SAFE);

         break; 
            
      case UTILS_FUNCS__WRITE:

         u16_Result = Fm25clssWrite (u8_Unit, pv_Data, u16_NumBytes, u16_DestAddr, cu16_UtilsFuncs_TimeoutLong);
         
         if (u16_Result != u16_NumBytes)   // error case, return from function
            u32_ErrorCodeSW_Sub = ((ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE);

         break;
            
      default:
         // error case, return from function if argument b_RW is wrong
         break;
   }
   if (u32_ErrorCodeSW_Sub)
   {
      u32_ErrorCodeSW_Sub = u32_ErrorCodeSW_Sub;
      u8_Unit=u8_Unit;
      pv_Data=pv_Data;
      u16_NumBytes=u16_NumBytes;
      u16_DestAddr=u16_DestAddr;
   }
   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

ulong UtilsFuncs_FixedMemErase (void)
{
   ulong    u32_ErrorCodeSW_Sub  =  (ulong)LIB_EXCEPTIONS__ERROR_NONE;
   uchar    u8_Unit              =  0;
   uchar    u8_Data              =  0;
   ushort   u16_NumBytes         =  1;
   ushort   u16_DestAddr         =  0;
   ushort   u16_Idx;
   ushort   u16_Result;

   // through tests determined max. FRAM address
   for (u16_Idx = 0; u16_Idx < (ushort)(FM25CLSS_BLOCKSIZEMAX/2) - 2; u16_Idx++)
   {
      u16_Result = Fm25clssWrite (u8_Unit, (void *)(&u8_Data), u16_NumBytes, u16_DestAddr, cu16_UtilsFuncs_TimeoutLong);
         
      if (u16_Result != u16_NumBytes)   // error case, return from function
      {
         u32_ErrorCodeSW_Sub = ((ulong)LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE);
         break;
      }
      
      u16_DestAddr++;
   }

   if (u32_ErrorCodeSW_Sub)
      u32_ErrorCodeSW_Sub = (u32_ErrorCodeSW_Sub << 16) | ((ulong)LIB_EXCEPTIONS__ERROR__UTILS_FUNCS__FIXED_MEM_ERASE);


   return u32_ErrorCodeSW_Sub;
}

//----------------------------------------------------------------------------------------------------------------------------------------------

// Dummy function for example to fill up function pointer array between existing functions. Execution is needed if someone changed  
// register bank entries for which no register command is defined. Zero will be set to function argument and as return value.
ushort UtilsFuncs_Dummy (ushort u16_Dummy, ulong *pu32_Dummy)
{
   *pu32_Dummy = 0;
   
   return (ushort)LIB_EXCEPTIONS__ERROR_NONE;
}
