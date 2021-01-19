//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/RTT/RTT.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-10-13 initial version
// 2010-10-14 added RttMsg function and TskRttMsg task
// 2010-10-14 added auto calibration to RttRestart
// 2010-10-14 added RttAlmSet function
// 2010-10-15 until new alarm it set RttAlmSet disables all irqs now
// 2010-10-21 added RTT_IRQ irq option
// 2010-10-22 added forced task switch in RttMsg by RESCHEDULE(0)
// 2010-10-27 fixed new message notification in TskRttMsg (hint by rr)
// 2011-03-23 rewrote RttRestart with calibration now based on "RttSCKGet"
// 2011-03-23 added new function RttSCKGet
// 2011-03-24 removed determination of sDiff in RttRestart (obsolete)
// 2011-04-14 added support for u32SCKValid in RttInit, RttRestart & RttSCKGet
// 2011-06-30 changed prototype of RttRead for true success return code
// 2011-07-13 removed RESCHEDULE(0) call in TskRttMsg
// 2011-07-13 added message count validation check in TskRttMsg
// 2013-01-23 renamed RttSCKGet to RttSLCKGet (SLow ClocK)
// 2013-01-23 fixed and optimized RttMsg & TskRttMsg (+ multiple bit feature)
// 2013-01-30 renamed boolean u32SLCKValid to bSLCKValid in _SRttRW
// 2013-03-07 fixed comments for RttIrqHandler
// 2013-04-11 fixed function header's return data type comments
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

#include "rtt.h"
#include "rttext.h"
#include "rttdata.h"
#include "rttdev.h"

// Suppress warning for "#1293-D: assignment in condition"
#pragma diag_remark 1293

//----------------------------------------------------------------------------
// void IsrRttx __irq
//----------------------------------------------------------------------------
// Each supported unit requires a function definition here
#if RTT_IRQ > 0
   #if RTT_UNIT_MAX > 0
      RTT_ISR_FUNC(0);
   #endif

   #if RTT_UNIT_MAX > 1
      RTT_ISR_FUNC(1);
   #endif
#endif

//----------------------------------------------------------------------------
// bool RttInit
//----------------------------------------------------------------------------
bool RttInit(UCHAR ucUnit, OS_ID oiMsgNewSem, ULONG ulMsgMask)
{
   bool bResult= FALSE;
     
   if (ucUnit < RTT_UNIT_MAX)
   {
      // Init
      SRttRW[ucUnit].bIrq= FALSE;
      SRttRO[ucUnit].pBase->RTTC_RTSR= SRttRO[ucUnit].pBase->RTTC_RTSR;
      SRttRO[ucUnit].pBase->RTTC_RTMR= 0x00008000UL;
      
      SRttRW[ucUnit].ulOffset= 0UL;
      SRttRW[ucUnit].ulIncDiv= 1UL;

      SRttRW[ucUnit].u32SLCK= 0;
      SRttRW[ucUnit].bSLCKValid= FALSE;

      // Mailbox
#if RTT_IRQ > 0
      OS_MBX_INIT_EX(MbxRttMsg[ucUnit]);
#endif
      // Semaphores
      os_sem_init(SRttRW[ucUnit].SemRW, 1);
      os_sem_init(SRttRW[ucUnit].SemMsg, 1);

      // Task
      SRttTsk[ucUnit].ucUnit= ucUnit;
#if RTT_IRQ > 0
      SRttRW[ucUnit].tidTskMsg= os_tsk_create_ex(TskRttMsg, SRttRO[ucUnit].u8TskPriMsg, &SRttTsk[ucUnit]);

      // Assign given semaphore and mask for new messages
      SRttRW[ucUnit].oiSemMsgNew= oiMsgNewSem;
      SRttRW[ucUnit].ulMsgMask= ulMsgMask;

      // Verify success
      bResult= (SRttRW[ucUnit].tidTskMsg != 0);
#else
      bResult= TRUE;
#endif
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// bool RttStart
//----------------------------------------------------------------------------
bool RttRestart(UCHAR ucUnit, USHORT usSecDiv, U32 u32Almv, ULONG ulOffset, ULONG ulIncDiv, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   U32 u32Result;
   U32 u32Register;
   
   if (ucUnit < RTT_UNIT_MAX)
   {
      // Lock RW
      if (OS_SEM_TAKE_CHK(os_sem_take(SRttRW[ucUnit].SemRW, u16Timeout)))
      {
         SRttRW[ucUnit].bIrq= FALSE;
         
         SRttRO[ucUnit].pBase->RTTC_RTAR= u32Almv;
         
         SRttRW[ucUnit].ulOffset= ulOffset;
         SRttRW[ucUnit].ulIncDiv= ulIncDiv;
         
         // Set-up and restart (calibration)
         u32Register= ((AT91C_RTTC_RTTRST) & BITS_ULONGHX);
         LOCKED(SRttRO[ucUnit].pBase->RTTC_RTMR= (((USHORT)1 & BITS_ULONGLX) | u32Register));
            
         // Wait to sum up rtt ticks to determine sck (32768 are exspected per second)
         os_dly_wait(OS_T_MS((1000/ _umax(SRttRO[ucUnit].usAdjCycleDiv, 1))));
         LOCKED(u32Result= SRttRO[ucUnit].pBase->RTTC_RTVR);
   
         // Store results
         SRttRW[ucUnit].u32SLCK= (u32Result* _umax(SRttRO[ucUnit].usAdjCycleDiv, 1));
         SRttRW[ucUnit].bSLCKValid= (SRttRW[ucUnit].u32SLCK) ? (TRUE) : (FALSE);
                          
         // Set-up and re-start
         SRttRW[ucUnit].bIrq= TRUE;
         u32Register= ((AT91C_RTTC_RTTRST) & BITS_ULONGHX);
#if RTT_IRQ > 0
         u32Register |= ((SRttRO[ucUnit].u32IrqMask) & BITS_ULONGHX);
#endif
         SRttRO[ucUnit].pBase->RTTC_RTMR= (SRttRW[ucUnit].u32SLCK/ _umin(usSecDiv, 1) | u32Register);
                 
         // Unlock RW
         os_sem_give(SRttRW[ucUnit].SemRW);

#ifdef COMMENT
         // Debug
         u32Register= 1000* 60* 10;
         os_dly_wait(OS_T_MS(u32Register));
         LOCKED(u32Result= SRttRO[ucUnit].pBase->RTTC_RTVR);
         u32Result= u32Result;
#endif

         bResult= TRUE;
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// bool RttSLCKGet
//----------------------------------------------------------------------------
bool RttSLCKGet(UCHAR ucUnit, U32 *pu32SLCK, U16 u16Timeout)
{
   bool bResult= 0;
   
   if (ucUnit < RTT_UNIT_MAX)
   {
      if (pu32SLCK)
      {  
         // Lock RW
         if (OS_SEM_TAKE_CHK(os_sem_take(SRttRW[ucUnit].SemRW, u16Timeout)))
         {
            if (SRttRW[ucUnit].bSLCKValid)
            {
               *pu32SLCK= SRttRW[ucUnit].u32SLCK;
               bResult= TRUE;
            }
            // Unlock RW
            os_sem_give(SRttRW[ucUnit].SemRW);
         }
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// bool RttAlmSet
//----------------------------------------------------------------------------
bool RttAlmSet(UCHAR ucUnit, U32 u32Almv, bool bClear, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   if (ucUnit < RTT_UNIT_MAX)
   {
      SRttRW[ucUnit].bIrq= FALSE;
      
      if (bClear)
      {
#if RTT_IRQ > 0
         RttMsg(ucUnit, AT91C_RTTC_ALMS, RTT_MSGCOUNTERMAX, u16Timeout);
#endif
      }
      
      SRttRO[ucUnit].pBase->RTTC_RTAR= u32Almv;
      SRttRW[ucUnit].bIrq= TRUE;
      
      bResult= TRUE;
   }
            
   return (bResult);
}

//----------------------------------------------------------------------------
// bool RttRead
//----------------------------------------------------------------------------
bool RttRead(UCHAR ucUnit, U32 *pu32Ticks, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   register U32 u32Ticks;
   
   if (ucUnit < RTT_UNIT_MAX)
   {
      if (pu32Ticks)
      {
         // Lock RW
         if (OS_SEM_TAKE_CHK(os_sem_take(SRttRW[ucUnit].SemRW, u16Timeout)))
         {     
            // Retrieve rtt ticks
            u32Ticks= SRttRO[ucUnit].pBase->RTTC_RTVR;
            
            // Offset shift
            u32Ticks += SRttRW[ucUnit].ulOffset;
            
            // Return ticks
            *pu32Ticks= u32Ticks;
            
            // Unlock RW
            os_sem_give(SRttRW[ucUnit].SemRW);
            
            bResult= TRUE;
         }
      }
   }
   
   return (bResult);
}

#if RTT_IRQ > 0
//----------------------------------------------------------------------------
// USHORT RttMsg
//----------------------------------------------------------------------------
USHORT RttMsg(UCHAR ucUnit, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   USHORT usCounter;
   UCHAR ucIndex;
      
   if (ucUnit < RTT_UNIT_MAX)
   {
      RESCHEDULE_TICKS(0);
      
      // Lock counters
      if (OS_SEM_TAKE_CHK(os_sem_take(SRttRW[ucUnit].SemMsg, u16Timeout)))
      {
         for (ucIndex= 0; ucIndex < RTT_MSGMAX; ucIndex++)
         {
            if ((1 << ucIndex) & ulMsg)
            {
               usCounter= SRttRW[ucUnit].xMsgCounter[ucIndex];
               if (usCounter)
               {
                  RAISEMAX(usResult, usCounter, USHRT_MAX);
                  
                  if (usDecrement)
                  {
                     if (usDecrement < usCounter)
                     {
                        SRttRW[ucUnit].xMsgCounter[ucIndex] -= usDecrement;
                     }
                     else
                     {
                        SRttRW[ucUnit].xMsgCounter[ucIndex]= 0;
                     }
                  }
               }
            }
         }
         
         // Unlock counters
         os_sem_give(SRttRW[ucUnit].SemMsg);
      }
   }
   
   return (usResult);
}

//----------------------------------------------------------------------------
// __task void TskRttMsg
//----------------------------------------------------------------------------
__task void TskRttMsg(void *pvParam)
{
   struct _SRttTsk *pSRttTsk= (struct _SRttTsk *) pvParam;
   
   ULONG ulValue;
   UCHAR ucCount;
   UCHAR ucIndex;
 
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxRttMsg[pSRttTsk->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         ucCount= MSG_GETCNT(RTT_MSGBITS, ulValue);
         if (ucCount)
         {
            // Lock counters
            if (OS_SEM_TAKE_CHK(os_sem_take(SRttRW[pSRttTsk->ucUnit].SemMsg, OS_T_FOREVER)))
            {         
               // Check message bits
               for (ucIndex= 0; ucIndex < RTT_MSGMAX; ucIndex++)
               {
                  if (ulValue & (1 << ucIndex))
                  {
                     // Count until max-1 and set max to show overrun condition
                     RAISEMAX(SRttRW[pSRttTsk->ucUnit].xMsgCounter[ucIndex], ucCount, RTT_MSGCOUNTERMAX);
                  }
               }
               
               // Unlock counters
               os_sem_give(SRttRW[pSRttTsk->ucUnit].SemMsg);
               
               // New message notification
               if (SRttRW[pSRttTsk->ucUnit].oiSemMsgNew)
               {
                  if (!OS_SEM_TAKE_CHK(os_sem_check(SRttRW[pSRttTsk->ucUnit].oiSemMsgNew, FALSE)))
                  {
                     os_sem_give(SRttRW[pSRttTsk->ucUnit].oiSemMsgNew);
                  }
                  UNLOCK;
               }
            }
         }
      }
   }

   DETACH;
}

//----------------------------------------------------------------------------
// void RttIrqHandler (sysirq!!!)
//----------------------------------------------------------------------------
void RttIrqHandler(UCHAR ucUnit)
{
   static ULONG ulInc[RTT_UNIT_MAX]= { 0 };
   register U32 u32Sr= 0UL;
    
   register U32 u32Mask;
  
   // Always (!!!) catch & clear any pending IRQ (sysirq!!!)
   u32Sr= SRttRO[ucUnit].pBase->RTTC_RTSR;  

   // Irq may triggered before rtt is initialized and cannot individually disabled
   if (SRttRW[ucUnit].bIrq)
   {
      // RTT has '<< 16' shifted irq bits compared to sr!!
      // RTT doesn't have a imr and sr bits are set regardless of active irqs!!
      u32Mask= (u32Sr & (SRttRO[ucUnit].u32IrqMask >> 16));
      if (u32Mask)
      {
         if (u32Mask & AT91C_RTTC_ALMS)
         {
            u32Mask= u32Mask;
         }
   
         if (u32Mask & AT91C_RTTC_RTTINC)
         {
            ulInc[ucUnit]++;
            if (ulInc[ucUnit] >= SRttRW[ucUnit].ulIncDiv)
            {
               ulInc[ucUnit]= 0;
            }
            else
            {
               u32Mask &= ~AT91C_RTTC_RTTINC;
            }
         }
         
         if (u32Mask & SRttRW[ucUnit].ulMsgMask)
         {
            if (isr_mbx_check(&MbxRttMsg[ucUnit]) > 0)
            {
               isr_mbx_send(&MbxRttMsg[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(RTT_MSGBITS, u32Mask, 1)));
            }
         }
      }
   }
   
   // Catch multiple firings may caused by sclk (= asynchronously) driven peripheral
   while ((u32Sr= SRttRO[ucUnit].pBase->RTTC_RTSR));
   
   // System IRQ!!! ---NO--- EOI!!!
}
#endif
