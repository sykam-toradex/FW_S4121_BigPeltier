/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                utils_tasks.c
*
*  \brief               The file contains utility tasks.
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-25     Initial version imported from S4121_utils.c
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
#include <AT91SAM7X512.H>
#include <lib_AT91SAM7X256.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "utils_tasks.h"
//------------------------------------------------ tasks ---------------------------------------------------------------------------------------
// let the board LED blink
__task void TSK_UtilsTasks_Heartbeat (void)
{
   const uchar    cu8_ArrSize                   = 2;
   const ushort   cu16A_OnDelayMS[cu8_ArrSize]  = {300, 150};
   const ushort   cu16A_OffDelayMS[cu8_ArrSize] = {900, 300};
   uchar i;
   FOREVER
   {
      for( i = 0; i < cu8_ArrSize; i++)
      {
         // LED off for 300 ms / 150 ms
         AT91F_PIO_SetOutput( AT91D_BASE_PIO_LED, AT91B_LED_ALIVE ); // see AT91SAM7X.h for macros
         //os_dly_wait( OS_T_MS(cu16A_OnDelayMS[i]) );
         RESCHEDULE((cu16A_OnDelayMS[i]));
         // LED on for 900 ms / 300 ms
         AT91F_PIO_ClearOutput( AT91D_BASE_PIO_LED, AT91B_LED_ALIVE );
         //os_dly_wait( OS_T_MS(cu16A_OffDelayMS[i]) );
         RESCHEDULE((cu16A_OffDelayMS[i]));
      }
      RESCHEDULE(50);
   }
   DETACH;   
}
