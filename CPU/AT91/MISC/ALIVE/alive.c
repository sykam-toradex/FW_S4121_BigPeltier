//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/ALIVE/ALIVE.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-01-31 initial version
// 2013-02-06 added SLCK to AliveInit for task cycle by WDT clock approximation
// 2013-02-07 ALIVE_STACK_TSKALIVE is now an option
// 2013-02-07 renamed u8TskPriMsg as u8TskPriAlive
// 2013-02-07 renamed tidTskMsg as tidTskAlive
// 2013-02-13 confirm only active watchdog in TskAlive
// 2013-04-11 fixed function header's return data type comments
// 2013-08-22 changed cycles from milliseconds to ticks
// 2013-08-22 changed RESCHEDULE (ms) to RESCHEDULE_TICKS (ticks)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <math.h>
#include <limits.h>
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "alive.h"
#include "aliveext.h"
#include "alivedata.h"
#include "alivedev.h"

// Suppress warning for "#1293-D: assignment in condition"
#pragma diag_remark 1293

//----------------------------------------------------------------------------
// bool AliveInit
//----------------------------------------------------------------------------
bool AliveInit(UCHAR ucUnit, U32 u32SLCK)
{
   bool bResult= FALSE;
   
   U16 u16WdtCycle;
   U16 u16Factor;
   
   if (ucUnit < ALIVE_UNIT_MAX)
   {
      // Init
      SAliveRW[ucUnit].usAliveIdx= 0;
      SAliveRW[ucUnit].u8PinEventMask= 0;

      // Task interval
      if (SAliveRO[ucUnit].u16TskCycle)
      {
         if (u32SLCK)
         {
            // WDT counter clock (~32768Hz / 128 = ~256Hz) convertion to ms and ticks
            u16WdtCycle= OS_T_MS(((((256 << 1) << 2)/ (u32SLCK/ 128)) >> 1)); // Integer version of 1/x (for SLCK 1...65536Hz)
   
            if (u16WdtCycle)
            {
               // Approximation to specified task interval by WDT integer multiples or integer fractions
               if (SAliveRO[ucUnit].u16TskCycle > u16WdtCycle)
               {
                  u16Factor= SAliveRO[ucUnit].u16TskCycle / u16WdtCycle;
                  SAliveRW[ucUnit].u16TskCycle= u16WdtCycle* u16Factor;
               }
               else if (SAliveRO[ucUnit].u16TskCycle < u16WdtCycle)
               {
                  u16Factor= u16WdtCycle/ SAliveRO[ucUnit].u16TskCycle;
                  if (u16Factor)
                  {
                     SAliveRW[ucUnit].u16TskCycle= u16WdtCycle/ u16Factor;
                  }
               }
               else
               {
                  SAliveRW[ucUnit].u16TskCycle= u16WdtCycle;
               }
            }
         }
         else
         {
            // Task interval by specified cycle time
            SAliveRW[ucUnit].u16TskCycle= SAliveRO[ucUnit].u16TskCycle;
         }
         
         // Preload pending counters
         SAliveRW[ucUnit].usPinOffCnt= SAliveRO[ucUnit].SAliveFactor[SAliveRW[ucUnit].usAliveIdx].ucOff;
         SAliveRW[ucUnit].usPinOnCnt= SAliveRO[ucUnit].SAliveFactor[SAliveRW[ucUnit].usAliveIdx].ucOn;
         SAliveRW[ucUnit].usWdtCnt= SAliveRO[ucUnit].ucWdtFactor;
         
         // Semaphores
         os_sem_init(SAliveRW[ucUnit].SemRW, 1);
         
         // Task
         SAliveTsk[ucUnit].ucUnit= ucUnit;

#if ALIVE_STACK_TSKALIVE > 0
         SAliveRW[ucUnit].tidTskAlive= OS_TSK_CREATE_USER_EX(TskAlive, SAliveRO[ucUnit].u8TskPriAlive, StkTskAlive[ucUnit], &SAliveTsk[ucUnit]);
#else
         SAliveRW[ucUnit].tidTskAlive= os_tsk_create_ex(TskAlive, SAliveRO[ucUnit].u8TskPriAlive, &SAliveTsk[ucUnit]);
#endif

         // Verify success
         bResult= (SAliveRW[ucUnit].tidTskAlive != 0);
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// __task void TskAlive
//----------------------------------------------------------------------------
__task void TskAlive(void *pvParam)
{
   struct _SAliveTsk *pSAliveTsk= (struct _SAliveTsk *) pvParam;
   
   FOREVER
   {
      // Lock _SAliveRW (to verify rtos semaphore handling still works)
      if (OS_SEM_TAKE_CHK(os_sem_take(SAliveRW[pSAliveTsk->ucUnit].SemRW, OS_T_FOREVER)))
      {
         // Pin(s) triggers
         if (SAliveRO[pSAliveTsk->ucUnit].pPioBase && SAliveRO[pSAliveTsk->ucUnit].u32PinMask)
         {
            // Pin(s) off trigger
            if (!SAliveRW[pSAliveTsk->ucUnit].usPinOffCnt)
            {
               // Pin off
               AT91F_PIO_ClearOutput(SAliveRO[pSAliveTsk->ucUnit].pPioBase, SAliveRO[pSAliveTsk->ucUnit].u32PinMask);
               SAliveRW[pSAliveTsk->ucUnit].u8PinEventMask |= ALIVE_EVENT_PIN_OFF;
            }

            // Pin(s) on trigger
            if (!SAliveRW[pSAliveTsk->ucUnit].usPinOnCnt)
            {
               // Pin on
               AT91F_PIO_SetOutput(SAliveRO[pSAliveTsk->ucUnit].pPioBase, SAliveRO[pSAliveTsk->ucUnit].u32PinMask);
               SAliveRW[pSAliveTsk->ucUnit].u8PinEventMask |= ALIVE_EVENT_PIN_ON;
            }
         }
         
         // Watchdog confirm trigger
         if (SAliveRO[pSAliveTsk->ucUnit].pWdtBase)
         {
            // Confirm only active watchdog
            if (!(SAliveRO[pSAliveTsk->ucUnit].pWdtBase->WDTC_WDMR & AT91C_WDTC_WDDIS))
            {            
               if (SAliveRO[pSAliveTsk->ucUnit].ucWdtFactor)
               {
                  if (!SAliveRW[pSAliveTsk->ucUnit].usWdtCnt)
                  {
                     // Confirm watchdog
                     AT91F_WDTRestart(SAliveRO[pSAliveTsk->ucUnit].pWdtBase);
   
                     // Reload pending counter
                     SAliveRW[pSAliveTsk->ucUnit].usWdtCnt= SAliveRO[pSAliveTsk->ucUnit].ucWdtFactor;
                  }
                  else
                  {
                     // Decrement pending counter
                     if (SAliveRW[pSAliveTsk->ucUnit].usWdtCnt) SAliveRW[pSAliveTsk->ucUnit].usWdtCnt--;
                  }
               }
            }
         }

         // Check for 'pin on-off' sequence complete
         if ((SAliveRW[pSAliveTsk->ucUnit].u8PinEventMask & ALIVE_EVENT_PIN_MASK) == ALIVE_EVENT_PIN_MASK)
         {
            // Clear events
            SAliveRW[pSAliveTsk->ucUnit].u8PinEventMask= 0;
                       
            // Alternate pin on/off trigger factors
            
            // usAliveIdx 0...(ALIVE_HOLDCNT- 1)
            SAliveRW[pSAliveTsk->ucUnit].usAliveIdx++;
            if (SAliveRW[pSAliveTsk->ucUnit].usAliveIdx >= ALIVE_FACTORCNT)
            {
               SAliveRW[pSAliveTsk->ucUnit].usAliveIdx= 0;
            }
            
            // Reload pending counters
            SAliveRW[pSAliveTsk->ucUnit].usPinOffCnt= SAliveRO[pSAliveTsk->ucUnit].SAliveFactor[SAliveRW[pSAliveTsk->ucUnit].usAliveIdx].ucOff;
            SAliveRW[pSAliveTsk->ucUnit].usPinOnCnt= SAliveRO[pSAliveTsk->ucUnit].SAliveFactor[SAliveRW[pSAliveTsk->ucUnit].usAliveIdx].ucOn;
         }
         else
         {
            // Decrement pending counters
            if (SAliveRW[pSAliveTsk->ucUnit].usPinOffCnt) SAliveRW[pSAliveTsk->ucUnit].usPinOffCnt--;
            if (SAliveRW[pSAliveTsk->ucUnit].usPinOnCnt) SAliveRW[pSAliveTsk->ucUnit].usPinOnCnt--;
         }

         // Unlock _SAliveRW (to verify rtos semaphore handling still works)
         os_sem_give(SAliveRW[pSAliveTsk->ucUnit].SemRW);         
      }
      
      // Pend for determined task intervall
      RESCHEDULE_TICKS(SAliveRW[pSAliveTsk->ucUnit].u16TskCycle);
   }

   DETACH;
}
