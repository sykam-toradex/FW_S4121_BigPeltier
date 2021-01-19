//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/ADC/ADC.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-05-25 initial version
// 2011-06-01 splitted old ADC_UNIT_MSGBITS to ADC_UNIT_/ADC_DATA_MSG_MSGBITS
// 2011-06-30 moved data ptr check before semaphore access in AdcRx
// 2011-07-06 added old data argument to custom functions pAverage & pModify
// 2011-07-06 moved xData from _SAdcChannelRW to _SStorage and replace it with it
// 2011-07-07 fixed and optimized (Tsk)AdcMsgUnit/Channel (+ multiple bit feature)
// 2011-07-07 ADC_UNIT/CHANNEL_MSGMAX are now a zero based settings
// 2011-07-07 renamed oiSemMsgEnh to oiSemMsgNew in AdcInitUnit prototype
// 2011-07-12 added optional on change message filtering in TskAdcRx
// 2011-07-12 added 2nd masking of messages by consideration of ADC_..._MSGMAX
// 2011-10-12 fixed SemRdy release in AdcRx for timed out reads because of SemRx
// 2011-10-12 changed TskAdcRx to send all messages after whole unit is processed
// 2011-10-12 changed data value in AdcRx & TskAdcRx to ADC_MEMITEMTYPE
// 2013-04-11 fixed function header's return data type comments
// 2013-04-11 renamed AT91B_MCK to AT91C_MCK ([B]it(mask), [C]onstant, [D]efinition, [F]unction!)
// 2013-08-01 added retrieve support of (non-contiguous) row of channels to AdcRx
// 2013-08-01 added retrieve support of (non-contiguous) row of channels to AdcWait
// 2013-08-01 changed return type of AdcWait to USHORT
// 2013-08-07 added AdcContinue for manual triggering in one-shot software mode
// 2013-08-07 added support for one-shot software mode to AdcRx/TskAdcRx
// 2013-08-07 fixed AdcIrqHandler channel determination for pdc/dma
// 2013-08-07 added virtual channel EOC/OVRE messages in TskAdcRx
// 2013-08-14 fixed AdcIrqHandler channel message masking
// 2013-08-22 changed cycle from milliseconds to ticks
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

#include "adc.h"
#include "adcdata.h"
#include "adcext.h"
#include "adcdev.h"

//----------------------------------------------------------------------------
// void IsrAdc __irq
//----------------------------------------------------------------------------
// Each supported usart unit requires a function definition here
#if ADC_UNIT_MAX > 0
   ADC_ISR_FUNC(0);
#endif

#if ADC_UNIT_MAX > 1
   ADC_ISR_FUNC(1);
#endif

//----------------------------------------------------------------------------
// UCHAR AdcInitUnit
//----------------------------------------------------------------------------
UCHAR AdcInitUnit(UCHAR ucUnit, UCHAR ucChannel, OS_ID oiSemMsgNew, USHORT usChannelMask, ULONG ulMsgMask, ULONG ulMsgChangeMask)
{
   UCHAR ucResult= 0;
   
   ULONG ulPrescal;
   ULONG ulStartup;
   ULONG ulShtim;

   if (ucUnit < ADC_UNIT_MAX)
   {
      // Unit init
      
      // Circular buffer handling for rx pdc
      SAdcRW[ucUnit].SCircular.ucCntEntries= 0;
      SAdcRW[ucUnit].SCircular.ucIdxRead= 0;
      SAdcRW[ucUnit].SCircular.ucIdxWrite= 0;
      SAdcRW[ucUnit].SCircular.ucSize= ADC_PDCDEPTHRX;
      
      // Mailboxes
      OS_MBX_INIT_EX(MbxAdcRx[ucUnit]);
      OS_MBX_INIT_EX(MbxAdcMsg[ucUnit]);
      OS_MBX_INIT_EX(MbxAdcMsgChannel[ucUnit]);
      
      // Init ADC

      // Clock
      AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, 1 << SAdcRO[ucUnit].u32PeriphId);
      
      // Reset
      AT91F_ADC_SoftReset(SAdcRO[ucUnit].pBase);
      
      // Determine timings
      ulPrescal= (AT91C_MCK/ (2* SAdcRO[ucUnit].u32ClockHz))- 1;
      ulStartup= ((SAdcRO[ucUnit].u32ClockHz/ 1000000)* SAdcRO[ucUnit].u16StartupUS/ 8)- 1;
      ulShtim= (((SAdcRO[ucUnit].u32ClockHz/ 1000000)* SAdcRO[ucUnit].u16SampleHoldNS)/ 1000)- 1;
      
      // Set timings and custom options
      SAdcRO[ucUnit].pBase->ADC_MR= ((ulPrescal<< 8) & AT91C_ADC_PRESCAL) | ((ulStartup<< 16) & AT91C_ADC_STARTUP) | ((ulShtim<< 24) & AT91C_ADC_SHTIM) | (SAdcRO[ucUnit].u8MR & BITS_UCHAR);

      // Rx Peripheral DMA Controller (PDC)
      SAdcRO[ucUnit].pBase->ADC_PTCR= AT91C_PDC_RXTDIS;
      
      // Clear channel rx
      SAdcRO[ucUnit].pBase->ADC_RPR= 0L;
      SAdcRO[ucUnit].pBase->ADC_RCR= 0;
            
      // Set reload values for above channel rx
      SAdcRO[ucUnit].pBase->ADC_RNPR= 0L;
      SAdcRO[ucUnit].pBase->ADC_RNCR= 0;
      
      SAdcRO[ucUnit].pBase->ADC_PTCR= AT91C_PDC_RXTEN;

      // Semaphores
      os_sem_init(SAdcRW[ucUnit].SemMsg, 1);
      os_sem_init(SAdcRW[ucUnit].SemRx, 1);
      os_sem_init(SAdcRW[ucUnit].SemTickRx, 0);
      
      // Assign given semaphore and mask for new messages
      SAdcRW[ucUnit].oiSemMsgNew= oiSemMsgNew;
      SAdcRW[ucUnit].usChannelMask= usChannelMask;
      SAdcRW[ucUnit].ulMsgMask= ulMsgMask;
      SAdcRW[ucUnit].ulMsgChangeMask= ulMsgChangeMask;
      
      // Tasks
      SAdcRW[ucUnit].ucUnit= ucUnit;
      
      SAdcRW[ucUnit].tidTskMsg= os_tsk_create_ex(TskAdcMsg, SAdcRO[ucUnit].u8TskPriMsg, &SAdcRW[ucUnit]);
      SAdcRW[ucUnit].tidTskMsgChannel= os_tsk_create_ex(TskAdcMsgChannel, SAdcRO[ucUnit].u8TskPriMsg, &SAdcRW[ucUnit]);
      SAdcRW[ucUnit].tidTskRx= os_tsk_create_ex(TskAdcRx, SAdcRO[ucUnit].u8TskPriRx, &SAdcRW[ucUnit]);
      
      // Interrupt
      
      // Clear pending IRQs
      AT91F_AIC_ClearIt(AT91C_BASE_AIC, SAdcRO[ucUnit].u32PeriphId);
      
      // Enable IRQs
      AT91F_ADC_EnableIt(SAdcRO[ucUnit].pBase, (SAdcRO[ucUnit].u32IrqMsg));
      AT91F_ADC_DisableIt(SAdcRO[ucUnit].pBase, ~(SAdcRO[ucUnit].u32IrqMsg));
      
      // IRQ ctrl
      // General enabling done in AdcInitChannel
      
      // Verify unit success
      if (SAdcRW[ucUnit].tidTskMsg && SAdcRW[ucUnit].tidTskMsgChannel && SAdcRW[ucUnit].tidTskRx)
      {
         SAdcRW[ucUnit].ucFlags= ADC_UNIT_FLAGS_CONFIGDONE;
         ucResult= 1;
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR AdcInitChannel
//----------------------------------------------------------------------------
UCHAR AdcInitChannel(UCHAR ucUnit, UCHAR ucChannel, struct _SAdcCfgChannel *pSCfgChannel, UCHAR ucCfgChannelCnt)
{
   UCHAR ucResult= 0;
   
   U32 u32CHER= NULL;
   
   UCHAR ucChannelCurr;
   UCHAR ucFlagsCurr;
   UCHAR ucIdx;
   
   if (ucUnit < ADC_UNIT_MAX)
   {
      if (ucChannel < ADC_CHANNEL_MAX)
      {
         if (pSCfgChannel && ucCfgChannelCnt)
         {
            LOCKED(ucFlagsCurr= SAdcRW[ucUnit].ucFlags);
            if (ucFlagsCurr & ADC_UNIT_FLAGS_CONFIGDONE)
            {
               for (ucIdx= 0; ucIdx < ucCfgChannelCnt; ucIdx++)
               {
                  // Determine destination channel and use ucChannel para as offset here
                  ucChannelCurr= ucChannel+ (pSCfgChannel+ ucIdx)->ucChannel;
                  if (ucChannelCurr < ADC_CHANNEL_MAX)
                  {
                     // Semaphores
                     os_sem_init(SAdcChannelRW[ucUnit][ucChannelCurr].SemMsg, 1);
                     os_sem_init(SAdcChannelRW[ucUnit][ucChannelCurr].SemRx, 1);
                     os_sem_init(SAdcChannelRW[ucUnit][ucChannelCurr].SemRdy, 0);

                     // Params               
                     SAdcChannelRW[ucUnit][ucChannelCurr].ucUnit            = ucUnit;
                     SAdcChannelRW[ucUnit][ucChannelCurr].u16To             = (pSCfgChannel+ ucIdx)->u16To;

                     SAdcChannelRW[ucUnit][ucChannelCurr].oiSemMsgNew       = (pSCfgChannel+ ucIdx)->oiSemMsgNew;
                     SAdcChannelRW[ucUnit][ucChannelCurr].ulMsgMask         = (pSCfgChannel+ ucIdx)->ulMsgMask;
                     SAdcChannelRW[ucUnit][ucChannelCurr].ulMsgChangeMask   = (pSCfgChannel+ ucIdx)->ulMsgChangeMask;

                     SAdcChannelRW[ucUnit][ucChannelCurr].STrg.xMax         = (pSCfgChannel+ ucIdx)->STrg.xMax;
                     SAdcChannelRW[ucUnit][ucChannelCurr].STrg.xAbove       = (pSCfgChannel+ ucIdx)->STrg.xAbove;
                     SAdcChannelRW[ucUnit][ucChannelCurr].STrg.xBelow       = (pSCfgChannel+ ucIdx)->STrg.xBelow;
                     SAdcChannelRW[ucUnit][ucChannelCurr].STrg.xMin         = (pSCfgChannel+ ucIdx)->STrg.xMin;

                     SAdcChannelRW[ucUnit][ucChannelCurr].SCustom.pAverage  = (pSCfgChannel+ ucIdx)->SCustom.pAverage;
                     SAdcChannelRW[ucUnit][ucChannelCurr].SCustom.pModify   = (pSCfgChannel+ ucIdx)->SCustom.pModify;

                     SAdcChannelRW[ucUnit][ucChannelCurr].SStorage.bValid   = FALSE;
                     SAdcChannelRW[ucUnit][ucChannelCurr].ucFlags           = ADC_CHANNEL_FLAGS_CONFIGDONE;
                     
                     // Adjust PDC transfer count (in memory transfers - NOT in bytes, ref. to AT91SAM7X manual 22.3.1)
                     SAdcRW[ucUnit].u16PDCMaxRx++;
                     
                     // Store configured channel
                     u32CHER |= (1 << ucChannelCurr);
                     
                     ucResult++;
                  }
               }

               // Enable configured channels
               SAdcRO[ucUnit].pBase->ADC_CHER= u32CHER;

               // Trigger conversion (one shot but it's retriggered by driver's TskRx)
               AT91F_ADC_StartConversion(SAdcRO[ucUnit].pBase);
               
               // IRQ ctrl
               AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, SAdcRO[ucUnit].u32PeriphId, 0, 0, IRQ_CAST(SAdcRO[ucUnit].Isr));
               AT91F_AIC_EnableIt(AT91C_BASE_AIC, SAdcRO[ucUnit].u32PeriphId);
            }
         }
      }
   }

   return (ucResult);
}

//----------------------------------------------------------------------------
// USHORT AdcMsgUnit
//----------------------------------------------------------------------------
USHORT AdcMsgUnit(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   USHORT usCounter;
   UCHAR ucIndex;
      
   if (ucUnit < ADC_UNIT_MAX)
   {
      RESCHEDULE_TICKS(0);
      
      // Lock counters
      if (OS_SEM_TAKE_CHK(os_sem_take(SAdcRW[ucUnit].SemMsg, u16Timeout)))
      {
         for (ucIndex= 0; ucIndex < ADC_UNIT_MSGMAX; ucIndex++)
         {
            if ((1 << ucIndex) & ulMsg)
            {
               usCounter= SAdcRW[ucUnit].xMsgCounter[ucIndex];
               if (usCounter)
               {
                  RAISEMAX(usResult, usCounter, USHRT_MAX);
                  
                  if (usDecrement)
                  {
                     if (usDecrement < usCounter)
                     {
                        SAdcRW[ucUnit].xMsgCounter[ucIndex] -= usDecrement;
                     }
                     else
                     {
                        SAdcRW[ucUnit].xMsgCounter[ucIndex]= 0;
                     }
                  }
               }
            }
         }
   
         // Unlock counters
         os_sem_give(SAdcRW[ucUnit].SemMsg);   
      }
   }
         
   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT AdcMsgChannel
//----------------------------------------------------------------------------
USHORT AdcMsgChannel(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   USHORT usCounter;
   UCHAR ucIndex;
      
   if (ucUnit < ADC_UNIT_MAX)
   {
      if (ucChannel < ADC_CHANNEL_MAX)
      {
         RESCHEDULE_TICKS(0);
         
         // Lock counters
         if (OS_SEM_TAKE_CHK(os_sem_take(SAdcChannelRW[ucUnit][ucChannel].SemMsg, u16Timeout)))
         {
            for (ucIndex= 0; ucIndex < ADC_CHANNEL_MSGMAX; ucIndex++)
            {
               if ((1 << ucIndex) & ulMsg)
               {
                  usCounter= SAdcChannelRW[ucUnit][ucChannel].xMsgCounter[ucIndex];
                  if (usCounter)
                  {
                     RAISEMAX(usResult, usCounter, USHRT_MAX);
                     
                     if (usDecrement)
                     {
                        if (usDecrement < usCounter)
                        {
                           SAdcChannelRW[ucUnit][ucChannel].xMsgCounter[ucIndex] -= usDecrement;
                        }
                        else
                        {
                           SAdcChannelRW[ucUnit][ucChannel].xMsgCounter[ucIndex]= 0;
                        }
                     }
                  }
               }
            }

            // Unlock counters
            os_sem_give(SAdcChannelRW[ucUnit][ucChannel].SemMsg);
         }
      }
   }

   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT AdcWait
//----------------------------------------------------------------------------
USHORT AdcWait(UCHAR ucUnit, UCHAR ucChannel, USHORT usChannelCount, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   USHORT usConfig;   
   USHORT usIdx;
   UCHAR ucFlags;

   if (ucUnit < ADC_UNIT_MAX)
   {
      if (ucChannel < ADC_CHANNEL_MAX)
      {
         // Retrieve a (non-contiguous) row count (= usChannelCount) of channels
         for (usConfig= 0, usIdx= 0; (usConfig < usChannelCount) && ((ucChannel+ usIdx) < ADC_CHANNEL_MAX); usIdx++)
         {
            LOCKED(ucFlags= SAdcChannelRW[ucUnit][ucChannel+ usIdx].ucFlags);
            if (ucFlags & ADC_CHANNEL_FLAGS_CONFIGDONE)
            {
               // Config count
               usConfig++;

               // Pend for data ready rx
               if (OS_SEM_TAKE_CHK(os_sem_take(SAdcChannelRW[ucUnit][ucChannel+ usIdx].SemRdy, u16Timeout)))
               {
                  if (SAdcChannelRW[ucUnit][ucChannel+ usIdx].SStorage.bValid)
                  {
                     // Data rx is ready success count
                     usResult++;
                  }
                                 
                  // Unlock data ready rx
                  os_sem_give(SAdcChannelRW[ucUnit][ucChannel+ usIdx].SemRdy);
               }
            }
         }
      }
   }
      
   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT AdcRx
//----------------------------------------------------------------------------
USHORT AdcRx(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usChannelCount, U16 u16Timeout)
{
   USHORT usResult= 0;

   USHORT usConfig;   
   USHORT usIdx;
   UCHAR ucFlags;

   if (ucUnit < ADC_UNIT_MAX)
   {
      // Check for one-shot software mode
      if (!SAdcRO[ucUnit].u16CycleRx)
      {
         // Trigger TskAdcRx now
         if (!OS_SEM_TAKE_CHK(os_sem_check(SAdcRW[ucUnit].SemTickRx, FALSE)))
         {
            if (SAdcRW[ucUnit].SemTickRx) os_sem_give(SAdcRW[ucUnit].SemTickRx);
         }
      }
                           
      if (ucChannel < ADC_CHANNEL_MAX)
      {
         if (pvData)
         {
            // Retrieve a (non-contiguous) row count (= usChannelCount) of channels
            for (usConfig= 0, usIdx= 0; (usConfig < usChannelCount) && ((ucChannel+ usIdx) < ADC_CHANNEL_MAX); usIdx++)
            {
               LOCKED(ucFlags= SAdcChannelRW[ucUnit][ucChannel+ usIdx].ucFlags);
               if (ucFlags & ADC_CHANNEL_FLAGS_CONFIGDONE)
               {
                  // Config count
                  usConfig++;
                  
                  // Pend for data ready rx
                  if (OS_SEM_TAKE_CHK(os_sem_take(SAdcChannelRW[ucUnit][ucChannel+ usIdx].SemRdy, u16Timeout)))
                  {
                     // Lock channel rx
                     if (OS_SEM_TAKE_CHK(os_sem_take(SAdcChannelRW[ucUnit][ucChannel+ usIdx].SemRx, u16Timeout)))
                     {
                        if (SAdcChannelRW[ucUnit][ucChannel+ usIdx].SStorage.bValid)
                        {
                           // Retrieve data
                           *((ADC_MEMITEMTYPE *) pvData+ usResult)= SAdcChannelRW[ucUnit][ucChannel+ usIdx].SStorage.xData;
                           
                           // Retrieve success count
                           usResult++;
                        }
                                                
                        // Unlock channel rx
                        os_sem_give(SAdcChannelRW[ucUnit][ucChannel+ usIdx].SemRx);
                     }
                     else
                     {
                        // Unlock data ready rx ONLY for failed read attempt to show value is still available
                        os_sem_give(SAdcChannelRW[ucUnit][ucChannel+ usIdx].SemRdy);
                     }
   
                     // Do not unlock data ready rx to show value is already read
                  }
               }
            }
         }
      }
   }
      
   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT AdcContinue
//----------------------------------------------------------------------------
USHORT AdcContinue(UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout)
{
   USHORT usResult= 0;

   if (ucUnit < ADC_UNIT_MAX)
   {
      // Check for one-shot software mode
      if (!SAdcRO[ucUnit].u16CycleRx)
      {
         // Trigger TskAdcRx now
         if (!OS_SEM_TAKE_CHK(os_sem_check(SAdcRW[ucUnit].SemTickRx, FALSE)))
         {
            if (SAdcRW[ucUnit].SemTickRx) os_sem_give(SAdcRW[ucUnit].SemTickRx);
         }
      }
                           
      if (ucChannel < ADC_CHANNEL_MAX)
      {
         usResult= 1;
      }
   }
      
   return (usResult);
}

//----------------------------------------------------------------------------
// __task void TskAdcMsg
//----------------------------------------------------------------------------
__task void TskAdcMsg(void *pvParam)
{
   struct _SAdcRW *pSAdcRW= (struct _SAdcRW *)pvParam;
   
   ULONG ulValue;
   UCHAR ucCount;
   UCHAR ucIndex;
  
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxAdcMsg[pSAdcRW->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         ucCount= MSG_GETCNT(ADC_UNIT_MSG_MSGBITS, ulValue);
         if (ucCount)
         {
            // Lock counters
            if (OS_SEM_TAKE_CHK(os_sem_take(SAdcRW[pSAdcRW->ucUnit].SemMsg, OS_T_FOREVER)))
            {         
               // Check message bits
               for (ucIndex= 0; ucIndex < ADC_UNIT_MSGMAX; ucIndex++)
               {
                  if (ulValue & (1 << ucIndex))
                  {
                     // Count until max-1 and set max to show overrun condition
                     RAISEMAX(SAdcRW[pSAdcRW->ucUnit].xMsgCounter[ucIndex], ucCount, ADC_MSGCOUNTERMAX);
                  }
               }

               // Unlock counters
               os_sem_give(SAdcRW[pSAdcRW->ucUnit].SemMsg);

               // New message notification
               if (SAdcRW[pSAdcRW->ucUnit].oiSemMsgNew)
               {
                  if (!OS_SEM_TAKE_CHK(os_sem_check(SAdcRW[pSAdcRW->ucUnit].oiSemMsgNew, FALSE)))
                  {
                     os_sem_give(SAdcRW[pSAdcRW->ucUnit].oiSemMsgNew);
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
// __task void TskAdcChannelMsg
//----------------------------------------------------------------------------
__task void TskAdcMsgChannel(void *pvParam)
{
   struct _SAdcRW *pSAdcRW= (struct _SAdcRW *)pvParam;
   
   ULONG ulValue;
   UCHAR ucIndex;
   UCHAR ucChannel;
 
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxAdcMsgChannel[pSAdcRW->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         ucChannel= MSG_GETCNT(ADC_CHANNEL_MSG_MSGBITS, ulValue);
         if (ucChannel < ADC_CHANNEL_MAX)
         {
            // Lock channel counters
            if (OS_SEM_TAKE_CHK(os_sem_take(SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SemMsg, OS_T_FOREVER)))
            {
               // Check message bits
               for (ucIndex= 0; ucIndex < ADC_CHANNEL_MSGMAX; ucIndex++)
               {
                  if (ulValue & (1 << ucIndex))
                  {
                     // Count until max-1 and set max to show overrun condition
                     RAISEMAX(SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].xMsgCounter[ucIndex], 1, ADC_MSGCOUNTERMAX);
                  }
               }
               
               // Unlock channel counters
               os_sem_give(SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SemMsg);
            }

            // New message notification (but don't increment the semaphore above one)
            if (SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].oiSemMsgNew)
            {
               if (!OS_SEM_TAKE_CHK(os_sem_check(SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].oiSemMsgNew, FALSE)))
               {
                  os_sem_give(SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].oiSemMsgNew);
               }
               UNLOCK;
            }
         }
      }
   }

   DETACH;
}

//----------------------------------------------------------------------------
// __task void TskAdcRx
//----------------------------------------------------------------------------
__task void TskAdcRx(void *pvParam)
{
   struct _SAdcRW *pSAdcRW= (struct _SAdcRW *)pvParam;
   
   static ULONG ulMsgUnitCache[ADC_UNIT_MAX][ADC_CHANNEL_MAX]= { NULL };
   static ULONG ulMsgChannelCache[ADC_UNIT_MAX][ADC_CHANNEL_MAX]= { NULL };
   
   static ULONG ulMsgUnit[ADC_UNIT_MAX][ADC_CHANNEL_MAX];
   static ULONG ulMsgChannel[ADC_UNIT_MAX][ADC_CHANNEL_MAX];
   
   ULONG ulValue;
   ULONG ulData;
   ULONG ulMsgUnitChangeMask;
   ULONG ulMsgChannelChangeMask;

   USHORT usChannelMask;
   USHORT usCount;
   USHORT usItem;
   USHORT usItemMsk;
   USHORT usItemChk;
   
   ADC_MEMITEMTYPE xData;

   UCHAR ucChannel;
   UCHAR ucIdxRead;
   UCHAR ucFlags;
        
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxAdcRx[pSAdcRW->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         usCount= MSG_GETCNT(ADC_UNIT_DATA_MSGBITS, ulValue);
         ulData= MSG_GETMSG(ADC_UNIT_DATA_MSGBITS, ulValue);
         ucIdxRead= (ulData & BITS_ULONGHL) >> BITS_OF(USHORT);
         usItemMsk= ulData & BITS_USHORT;

         if (usCount && usItemMsk)
         {
            // Lock unit rx
            if (OS_SEM_TAKE_CHK(os_sem_take(SAdcRW[pSAdcRW->ucUnit].SemRx, OS_T_FOREVER)))
            {                           
               // SAdcRW[ucUnit].u16PDCMaxRx doesn't change after init anymore
               for (usItem= 0, ucChannel= 0, usItemChk=1; (usItem < SAdcRW[pSAdcRW->ucUnit].u16PDCMaxRx) && (usItemChk < (1 << ADC_CHANNEL_MAX)); usItemChk <<= 1, ucChannel++)
               {
                  // Process channels with active bit set
                  if (usItemMsk & usItemChk)
                  {                        
                     // Check ucChannel range for array access here
                     if (ucChannel < ADC_CHANNEL_MAX)
                     {
                        // Check channel's configuration
                        LOCKED(ucFlags= SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].ucFlags);
                        if (ucFlags & ADC_CHANNEL_FLAGS_CONFIGDONE)
                        {
                           // Lock channel rx
                           if (OS_SEM_TAKE_CHK(os_sem_take(SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SemRx, OS_T_FOREVER)))
                           {
                              // Retrieve data
                              xData= SAdcRW[pSAdcRW->ucUnit].xPDCBufferRx[ucIdxRead][usItem];
                              
                              // Optional data processing by custom functions
                              if (SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SCustom.pModify) xData= SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SCustom.pModify(pSAdcRW->ucUnit, ucChannel, xData, SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SStorage.xData, SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SStorage.bValid);
                              if (SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SCustom.pAverage) xData= SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SCustom.pAverage(pSAdcRW->ucUnit, ucChannel, xData, SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SStorage.xData, SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SStorage.bValid);
                              
                              // Store data
                              SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SStorage.xData= xData;
                              SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SStorage.bValid= TRUE;

                              // Set unit and channel data rx notification msg
                              ulMsgUnit[pSAdcRW->ucUnit][ucChannel]= SAdcRO[pSAdcRW->ucUnit].u32IrqRx;
                              ulMsgChannel[pSAdcRW->ucUnit][ucChannel]= ulMsgUnit[pSAdcRW->ucUnit][ucChannel];
                              
                              // Add virtual channel EOC
                              ulMsgUnit[pSAdcRW->ucUnit][ucChannel]|= (AT91C_ADC_EOC0 << ucChannel);
                              ulMsgChannel[pSAdcRW->ucUnit][ucChannel]|= (AT91C_ADC_EOC0 << ucChannel);
                              
                              // Add virtual channel OVRE
                              if (ulMsgUnit[pSAdcRW->ucUnit][ucChannel] & AT91C_ADC_GOVRE)
                              {
                                 ulMsgUnit[pSAdcRW->ucUnit][ucChannel]|= (AT91C_ADC_OVRE0 << ucChannel);
                                 ulMsgChannel[pSAdcRW->ucUnit][ucChannel]|= (AT91C_ADC_OVRE0 << ucChannel);
                              }
                              
                              // Retrieve masks
                              LOCK;
                              {
                                 ulMsgUnitChangeMask= SAdcRW[pSAdcRW->ucUnit].ulMsgChangeMask;
                                 ulMsgChannelChangeMask= SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].ulMsgChangeMask;
                                 usChannelMask= SAdcRW[pSAdcRW->ucUnit].usChannelMask;
                              }
                              UNLOCK;
                              
                              // Boundary check and notification (also forward to unit for enhanced notifications for masked channels)
                              
                              if (xData >= SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].STrg.xMax)
                              {
                                 // Add max notification msg to channel
                                 if ((!(ulMsgChannelChangeMask & ADC_TRG_MAX)) || (!(ulMsgChannelCache[pSAdcRW->ucUnit][ucChannel] & ADC_TRG_MAX)))
                                 {
                                    ulMsgChannel[pSAdcRW->ucUnit][ucChannel] |= ADC_TRG_MAX;
                                 }
                                 
                                 // Add max notification msg to unit for masked channel
                                 if (usChannelMask & (1 << ucChannel))
                                 {
                                    if ((!(ulMsgUnitChangeMask & ADC_TRG_MAX)) || (!(ulMsgUnitCache[pSAdcRW->ucUnit][ucChannel] & ADC_TRG_MAX)))
                                    {
                                       ulMsgUnit[pSAdcRW->ucUnit][ucChannel] |= ADC_TRG_MAX;
                                    }
                                 }
                              }
                              else if (xData >= SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].STrg.xAbove)
                              {
                                 // Add above notification msg to channel
                                 if ((!(ulMsgChannelChangeMask & ADC_TRG_ABOVE)) || (!(ulMsgChannelCache[pSAdcRW->ucUnit][ucChannel] & ADC_TRG_ABOVE)))
                                 {
                                    ulMsgChannel[pSAdcRW->ucUnit][ucChannel] |= ADC_TRG_ABOVE;
                                 }
                                                                  
                                 // Add above notification msg to unit for masked channel
                                 if (usChannelMask & (1 << ucChannel))
                                 {
                                    if ((!(ulMsgUnitChangeMask & ADC_TRG_ABOVE)) || (!(ulMsgUnitCache[pSAdcRW->ucUnit][ucChannel] & ADC_TRG_ABOVE)))
                                    {
                                       ulMsgUnit[pSAdcRW->ucUnit][ucChannel] |= ADC_TRG_ABOVE;
                                    }
                                 }
                              }                                                     
                              else if (xData <= SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].STrg.xMin)
                              {
                                 // Add min notification msg to channel
                                 if ((!(ulMsgChannelChangeMask & ADC_TRG_MIN)) || (!(ulMsgChannelCache[pSAdcRW->ucUnit][ucChannel] & ADC_TRG_MIN)))
                                 {
                                    ulMsgChannel[pSAdcRW->ucUnit][ucChannel] |= ADC_TRG_MIN;
                                 }
                                 
                                 // Add min notification msg to unit for masked channel
                                 if (usChannelMask & (1 << ucChannel))
                                 {
                                    if ((!(ulMsgUnitChangeMask & ADC_TRG_MIN)) || (!(ulMsgUnitCache[pSAdcRW->ucUnit][ucChannel] & ADC_TRG_MIN)))
                                    {
                                       ulMsgUnit[pSAdcRW->ucUnit][ucChannel] |= ADC_TRG_MIN;
                                    }
                                 }
                              }
                              else if (xData <= SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].STrg.xBelow)
                              {
                                 // Add below notification msg to channel
                                 if ((!(ulMsgChannelChangeMask & ADC_TRG_BELOW)) || (!(ulMsgChannelCache[pSAdcRW->ucUnit][ucChannel] & ADC_TRG_BELOW)))
                                 {
                                    ulMsgChannel[pSAdcRW->ucUnit][ucChannel] |= ADC_TRG_BELOW;
                                 }
                                 
                                 // Add below notification msg to unit for masked channel
                                 if (usChannelMask & (1 << ucChannel))
                                 {
                                    if ((!(ulMsgUnitChangeMask & ADC_TRG_BELOW)) || (!(ulMsgUnitCache[pSAdcRW->ucUnit][ucChannel] & ADC_TRG_BELOW)))
                                    {
                                       ulMsgUnit[pSAdcRW->ucUnit][ucChannel] |= ADC_TRG_BELOW;
                                    }
                                 }
                              }
                              else
                              {
                                 // Add normal notification msg to channel
                                 if ((!(ulMsgChannelChangeMask & ADC_TRG_NORMAL)) || (!(ulMsgChannelCache[pSAdcRW->ucUnit][ucChannel] & ADC_TRG_NORMAL)))
                                 {
                                    ulMsgChannel[pSAdcRW->ucUnit][ucChannel] |= ADC_TRG_NORMAL;
                                 }

                                 // Add normal notification msg to unit for masked channel
                                 if (usChannelMask & (1 << ucChannel))
                                 {
                                    if ((!(ulMsgUnitChangeMask & ADC_TRG_NORMAL)) || (!(ulMsgUnitCache[pSAdcRW->ucUnit][ucChannel] & ADC_TRG_NORMAL)))
                                    {
                                       ulMsgUnit[pSAdcRW->ucUnit][ucChannel] |= ADC_TRG_NORMAL;
                                    }
                                 }
                              }
   
                              // Apply unit message filter
                              ulMsgUnit[pSAdcRW->ucUnit][ucChannel] &= SAdcRW[pSAdcRW->ucUnit].ulMsgMask;
                              
                              // Apply unit max counter index available
                              ulMsgUnit[pSAdcRW->ucUnit][ucChannel] &= ((1 << (ADC_UNIT_MSGMAX+ 1))- 1);

                              // Update unit msg cache with filtered msg
                              if (ulMsgUnit[pSAdcRW->ucUnit][ucChannel])
                              {
                                 ulMsgUnitCache[pSAdcRW->ucUnit][ucChannel]= ulMsgUnit[pSAdcRW->ucUnit][ucChannel];
                              }

                              // Apply channel message filter
                              ulMsgChannel[pSAdcRW->ucUnit][ucChannel] &= SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].ulMsgMask;
                              
                              // Apply channel max counter index available
                              ulMsgChannel[pSAdcRW->ucUnit][ucChannel] &= ((1 << (ADC_CHANNEL_MSGMAX+ 1))- 1);

                              // Update channel msg cache with filtered msg
                              if (ulMsgChannel[pSAdcRW->ucUnit][ucChannel])
                              {
                                 ulMsgChannelCache[pSAdcRW->ucUnit][ucChannel]= ulMsgChannel[pSAdcRW->ucUnit][ucChannel];
                              }

                              // Unlock channel rx
                              os_sem_give(SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SemRx);
                           }
                        }
                     }
                     
                     // Increment processed item count
                     usItem++;
                  }
               }
               
               // Unlock unit rx
               os_sem_give(SAdcRW[pSAdcRW->ucUnit].SemRx);
            }
            
            // All channels of current unit are processed and unlocked now

            // SAdcRW[ucUnit].u16PDCMaxRx doesn't change after init anymore
            for (usItem= 0, ucChannel= 0, usItemChk=1; (usItem < SAdcRW[pSAdcRW->ucUnit].u16PDCMaxRx) && (usItemChk < (1 << ADC_CHANNEL_MAX)); usItemChk <<= 1, ucChannel++)
            {
               // Process channels with active bit set
               if (usItemMsk & usItemChk)
               {
                  // Send msg(s) to unit counter task
                  if (ulMsgUnit[pSAdcRW->ucUnit][ucChannel])
                  {
                     os_mbx_send_safe(&MbxAdcMsg[pSAdcRW->ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(ADC_UNIT_MSG_MSGBITS, ulMsgUnit[pSAdcRW->ucUnit][ucChannel], 1)), OS_T_FOREVER);
                     RESCHEDULE_TICKS(0);                                 
                  }

                  // Send msg(s) to channel counter task
                  if (ulMsgChannel[pSAdcRW->ucUnit][ucChannel])
                  {
                     // Send msg(s) to channel counter task
                     os_mbx_send_safe(&MbxAdcMsgChannel[pSAdcRW->ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(ADC_CHANNEL_MSG_MSGBITS, ulMsgChannel[pSAdcRW->ucUnit][ucChannel], ucChannel)), OS_T_FOREVER);
                     RESCHEDULE_TICKS(0);
                  }
                  
                  // Data ready notification (but don't increment the semaphore above one)
                  if (!OS_SEM_TAKE_CHK(os_sem_check(SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SemRdy, FALSE)))
                  {
                     os_sem_give(SAdcChannelRW[pSAdcRW->ucUnit][ucChannel].SemRdy);
                  }
                  UNLOCK;

                  // Increment processed item count
                  usItem++;
               }
            }                        
         }
         
         // Enable the rx irq again
         AT91F_ADC_EnableIt(SAdcRO[pSAdcRW->ucUnit].pBase, SAdcRO[pSAdcRW->ucUnit].u32IrqRx);
      }
      
      // Limit maximum check cycle speed
      if (SAdcRO[pSAdcRW->ucUnit].u16CycleRx)
      {
         RESCHEDULE_TICKS(SAdcRO[pSAdcRW->ucUnit].u16CycleRx);
      }
      // One-shot software mode
      else
      {
         // Suspend task until new job trigger by AdcRx or AdcContinue
         os_sem_take(SAdcRW[pSAdcRW->ucUnit].SemTickRx, OS_T_FOREVER);
      }     
      
      // Trigger next conversion (always one-shot hardware mode)
      AT91F_ADC_StartConversion(SAdcRO[pSAdcRW->ucUnit].pBase);
   }

   DETACH;
}

//----------------------------------------------------------------------------
// void AdcIrqHandler
//----------------------------------------------------------------------------
void AdcIrqHandler(UCHAR ucUnit)
{
   register ULONG ulAdcIrq;
   register ULONG ulAdcMsg;
   register ULONG ulAdcMsgChannel;
   
   register ULONG ulChannelMask;
   register UCHAR ucChannel;
   
   // Copy and clear (ADC_IMR) the IRQ source to handle yet
   ulAdcIrq= SAdcRO[ucUnit].pBase->ADC_IMR & SAdcRO[ucUnit].pBase->ADC_SR;
   
   // Clear all data IRQ sources (they may be set later when there is no more data action when they occur)
   ulAdcMsg= ((ulAdcIrq & ~(AT91C_ADC_RXBUFF | AT91C_ADC_ENDRX)));
   
   // Rx using preloaded PDC
   if ((ulAdcIrq & AT91C_ADC_RXBUFF) || (ulAdcIrq & AT91C_ADC_ENDRX))
   {
      // Check for a free buffer to set the rx pdc to write to (usCntEntries is incremented in advance)
      if (SAdcRW[ucUnit].SCircular.ucCntEntries <= SAdcRW[ucUnit].SCircular.ucSize)
      {   
         if (!SAdcRO[ucUnit].pBase->ADC_RNCR)
         {
            // Prepare pdc rx
            SAdcRO[ucUnit].pBase->ADC_RNPR= (UINT) &SAdcRW[ucUnit].xPDCBufferRx[SAdcRW[ucUnit].SCircular.ucIdxWrite];
            SAdcRO[ucUnit].pBase->ADC_RNCR= SAdcRW[ucUnit].u16PDCMaxRx;
         
            if (SAdcRW[ucUnit].SCircular.ucCntEntries > 0)
            {
               // Send rx msg to task including fixed count of received items and current rx buffer index to read from
               if (isr_mbx_check(&MbxAdcRx[ucUnit]) > 0)
               {
                  // Disable the rx irq yet - finished task will re-enable it again
                  AT91F_ADC_DisableIt(SAdcRO[ucUnit].pBase, SAdcRO[ucUnit].u32IrqRx);
               
                  // Trigger rx task to process received data with active channel mask (ADC_CHSR)
                  isr_mbx_send(&MbxAdcRx[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(ADC_UNIT_DATA_MSGBITS, (((ULONG)SAdcRW[ucUnit].SCircular.ucIdxRead) << BITS_OF(USHORT)) | (ULONG)SAdcRO[ucUnit].pBase->ADC_CHSR, SAdcRW[ucUnit].SCircular.ucCntEntries)));

                  // Increment read index
                  SAdcRW[ucUnit].SCircular.ucIdxRead= (SAdcRW[ucUnit].SCircular.ucIdxRead+ 1) % SAdcRW[ucUnit].SCircular.ucSize;
                  
                  // Decrement count
                  if (SAdcRW[ucUnit].SCircular.ucCntEntries) SAdcRW[ucUnit].SCircular.ucCntEntries--;
               }
            }

            // usCntEntries and usIdxWrite are incremented in advance

            if (SAdcRW[ucUnit].SCircular.ucCntEntries < SAdcRW[ucUnit].SCircular.ucSize)
            {
               // Raise usCntEntries upto usSize+ 1
               RAISEMAX(SAdcRW[ucUnit].SCircular.ucCntEntries, 1, (SAdcRW[ucUnit].SCircular.ucSize+ 1));
                  
               // Increment write index
               SAdcRW[ucUnit].SCircular.ucIdxWrite= (SAdcRW[ucUnit].SCircular.ucIdxWrite+ 1) % SAdcRW[ucUnit].SCircular.ucSize;
            }
            else
            {
               // Debug
               SAdcRW[ucUnit].SCircular.ucCntEntries= SAdcRW[ucUnit].SCircular.ucCntEntries;
            }
         }
      }
   }

   // Record all other yet unhandled IRQ sources

   // Assign full unit msg
   ulAdcMsgChannel= ulAdcMsg;
   
   // Clear any unit only msg bits for the channel
   ulAdcMsgChannel&= BITS_ULONGLX;

   // Determine active channel(s) for pdc/dma block retrieve
   for (ucChannel= 0; ucChannel < ADC_CHANNEL_MAX; ucChannel++)
   {
      ulChannelMask= ((AT91C_ADC_EOC0 | AT91C_ADC_OVRE0) << ucChannel);
      if (ulAdcMsgChannel & ulChannelMask)
      {        
         // Apply channel max counter index available
         ulAdcMsgChannel&= ((1 << (ADC_CHANNEL_MSGMAX+ 1))- 1);
         
         // Apply channel message filter
         ulAdcMsgChannel&= SAdcChannelRW[ucUnit][ucChannel].ulMsgMask;
         
         // Check for channel msg left
         if (ulAdcMsgChannel)
         {
            if (isr_mbx_check(&MbxAdcMsgChannel[ucUnit]) > 0)
            {
               // Send channel msg (for current channel only)
               isr_mbx_send(&MbxAdcMsgChannel[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(ADC_CHANNEL_MSG_MSGBITS, (ulAdcMsgChannel & ulChannelMask), ucChannel)));
            }
         }
      }
   }

   // Clear any channel msg bits for the unit
   ulAdcMsg&= BITS_ULONGHX;

   // Apply unit message filter
   ulAdcMsg&= SAdcRW[ucUnit].ulMsgMask;   
  
   // Apply unit max counter index available
   ulAdcMsg&= ((1 << (ADC_UNIT_MSGMAX+ 1))- 1);
   
   // Check for unit msg left
   if (ulAdcMsg)
   {
      if (isr_mbx_check(&MbxAdcMsg[ucUnit]) > 0)
      {
         // Send unit msg
         isr_mbx_send(&MbxAdcMsg[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(ADC_UNIT_MSG_MSGBITS, ulAdcMsg, 1)));
      }
   }

   // ARM needs EXPLICIT acknowledge (qualifier __irq doesn't do this) for (E)nd (O)f (I)RQ!!
   EOI;
}
