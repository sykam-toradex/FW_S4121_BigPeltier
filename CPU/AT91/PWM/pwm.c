//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PWM/PWM.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-15 initial version (dispatcher for shared irq of all channels)
// 2010-01-20 removed auto call to AT91F_PWMC_InterruptEnable()
// 2010-01-28 added PwmCalcModnPrdDty
// 2010-01-29 PwmCalcModnPrdDty returns now pwm_cmr compatible modn value
// 2010-02-04 renamed PwmCalcModnPrdDty to PwmCalcCprePrdDty
// 2010-02-05 fixed modn calculation of PwmCalcCprePrdDty
// 2010-02-25 added PwmCalcHz
// 2010-07-01 added PwmInitChannel, PwmChannelTx and TskPwmChannelTx
// 2010-07-01 added PwmChannelIrqHandler
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-23 changed to _u(l)min & _u(l)max intrinsics
// 2010-10-15 added optional define PWM_UPDATE_FIX
// 2010-10-20 fixed pwm update fix in PwmChannelIrqHandler
// 2010-10-21 changed irq mode of PwmInitIrq to INT_POSITIVE_EDGE (more stable)
// 2010-12-16 added new function PwmChannelIdGet and PwmChannelInactiveGet
// 2010-12-16 added new function PwmStartChannel and PwmStopChannel
// 2011-01-20 added AT91F_PWMC_InterruptEnable in PwmStartChannel
// 2011-01-26 raised irq priority to AT91C_AIC_PRIOR_HIGHEST
// 2011-02-16 changed irq mode of PwmInitIrq back to default level mode
// 2011-02-14 fixed channel arg for AT91F_PWMC_CfgChannel(...) in PwmStartChannel
// 2011-03-10 changed PwmCalcCprePrdDty to divide at least by 2^1 to avoid PWM hw-bug
// 2011-05-11 fixed storage for circular_read in PwmChannelIrqHandler
// 2011-05-11 moved set of SemDone to busy from TskPwmChannelTx to PwmChannelTx
// 2011-09-28 now using define PWM_2NDCHANCEUPDATE_FIX in PwmChannelIrqHandler
// 2013-04-11 fixed function header's return data type comments
// 2013-04-11 renamed AT91B_MCK to AT91C_MCK ([B]it(mask), [C]onstant, [D]efinition, [F]unction!)
// 2013-08-22 fixed PWM_2NDCHANCEUPDATE_FIX
// 2013-08-22 own and working version for PWM_UPDATE_FIX
// 2013-08-22 changed RESCHEDULE (ms) to RESCHEDULE_TICKS (ticks)
// 2013-10-23 fixed header comment for PwmChannelIdGet
// 2013-10-31 renamed PwmChannelTxValue to PwmTxValueChannel to fit overal naming style
// 2013-10-31 renamed PwmChannelTx to PwmTxChannel to fit overal naming style
// 2013-10-31 renamed PwmChannelInactiveGet to PwmInactiveGetChannel to fit overal naming style
// 2013-10-31 renamed PwmChannelWait to PwmWaitChannel to fit overal naming style
// 2013-11-06 renamed PwmStartChannel to PwmStart to fit overal naming style
// 2013-11-06 renamed PwmStopChannel to PwmStop to fit overal naming style
// 2013-11-06 renamed PwmTxValueChannel to PwmTxValue to fit overal naming style
// 2013-11-06 renamed PwmTxChannel to PwmTx to fit overal naming style
// 2013-11-06 renamed PwmInactiveGetChannel to PwmInactiveGet to fit overal naming style
// 2013-11-06 renamed PwmWaitChannel to PwmWait to fit overal naming style
// 2015-04-29 added more comment details in PwmCalcCprePrdDty
// 2015-04-29 added masking of ulCpre in PwmCalcCprePrdDty
// 2017-09-07 added check of pSPwmList's range after seek in PwmIrqHandler
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "limits.h"

#include "pwm.h"
#include "pwmdata.h"
#include "pwmdev.h"
#include "pwmext.h"

// Prototypes
void PwmInitIrq(UCHAR ucUnit);
void PwmIrqHandler(UCHAR ucUnit);

//----------------------------------------------------------------------------
// void IsrPwmx __irq
//----------------------------------------------------------------------------
// Each supported unit requires a function definition here
#if PWM_UNIT_MAX > 0
   PWM_ISR_FUNC(0);
#if PWM_CHANNEL_MAX > 0
   PWM_CHANNEL_ISR_FUNC;
#endif
#endif


#if PWM_UNIT_MAX > 1
   PWM_ISR_FUNC(1);
#endif

//----------------------------------------------------------------------------
// bool PwmInitUnit
//----------------------------------------------------------------------------
bool PwmInitUnit(UCHAR ucUnit, struct _SPwmItem *pSPwmList, UCHAR ucItemCnt)
{
   AT91PS_PWMC pPwm;
   struct _SPwmItem *pSPwmItem;
   U32 u32IrqMask;
   UCHAR ucIndex;
   
   bool bPeriphClockReq= 0;
   bool bIsrReq= 0;
   
   if (ucUnit < PWM_UNIT_MAX)
   {
      pPwm= SPwmRO[ucUnit].pBase;
      SPwmRO[ucUnit].pSPwmList= pSPwmList;
      SPwmRO[ucUnit].ucPwmListSize= ucItemCnt;
        
      AT91F_PWMC_InterruptDisable(pPwm, 0xFFFFFFFF);
      
      for (u32IrqMask= 0, ucIndex= 0; ucIndex < ucItemCnt; ucIndex++)
      {
         pSPwmItem= pSPwmList+ ucIndex;
              
         if (pSPwmItem->Isr)
         {
            bPeriphClockReq= 1;
            bIsrReq= 1;
            
            u32IrqMask |= pSPwmItem->uiMask;
         }      
      }
            
      if (bPeriphClockReq) AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, 1 << SPwmRO[ucUnit].u32PeriphId);
      
      if (bIsrReq)
      {
         //-AT91F_PWMC_InterruptEnable(pPwm, u32IrqMask);   
         PwmInitIrq(ucUnit);
      }
   }
      
   return (bPeriphClockReq);
}

//----------------------------------------------------------------------------
// void PwmInitIrq
//----------------------------------------------------------------------------
void PwmInitIrq(UCHAR ucUnit)
{
   U32 u32ISR;
   
   if (ucUnit < PWM_UNIT_MAX)
   {
      // Clear pending IRQs
      u32ISR= SPwmRO[ucUnit].pBase->PWMC_ISR;
      u32ISR= u32ISR;
      
      // IRQ ctrl
      AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, SPwmRO[ucUnit].u32PeriphId, 0 /*AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE*/, AT91C_AIC_PRIOR_HIGHEST, IRQ_CAST(SPwmRO[ucUnit].Isr));
      AT91F_AIC_EnableIt(AT91C_BASE_AIC, SPwmRO[ucUnit].u32PeriphId);
   }
}     

//----------------------------------------------------------------------------
// void PwmIrqHandler
//----------------------------------------------------------------------------
void PwmIrqHandler(UCHAR ucUnit)
{
   register U32 u32ISR;
   register U32 u32IMR;
   register U32 u32Pin;
   register UCHAR ucBitPos;
   register UCHAR ucListIdx;

   if (ucUnit < PWM_UNIT_MAX)
   {
      u32IMR= SPwmRO[ucUnit].pBase->PWMC_IMR;
      u32ISR= SPwmRO[ucUnit].pBase->PWMC_ISR & u32IMR;
      
      for (ucBitPos= 0; ucBitPos < 32; ucBitPos++)
      {
         u32Pin= (1 << ucBitPos);
         if (u32ISR & u32Pin)
         {        
            if (u32Pin != SPwmRO[ucUnit].pSPwmList[ucBitPos].uiMask)
            {
               // Seek to pin inside list
               for (ucListIdx= 0; (ucListIdx < SPwmRO[ucUnit].ucPwmListSize) && (u32Pin != SPwmRO[ucUnit].pSPwmList[ucListIdx].uiMask); ucListIdx++);
            }
            else
            {
               // Already at right position
               ucListIdx= ucBitPos;
            }
            
            // Handle found pin
            if (ucListIdx < SPwmRO[ucUnit].ucPwmListSize)
            {
               if (u32Pin == SPwmRO[ucUnit].pSPwmList[ucListIdx].uiMask)
               {
                  // Call user isr function with args PWM_ISR and PWM_CHID
                  if (SPwmRO[ucUnit].pSPwmList[ucListIdx].Isr) SPwmRO[ucUnit].pSPwmList[ucListIdx].Isr(u32ISR, u32Pin);
               }
            }
         }
      }
   }
              
   // ARM needs EXPLICIT acknowledge (qualifier __irq doesn't do this) for (E)nd (O)f (I)RQ!!
   EOI;
}

#if PWM_CHANNEL_MAX > 0

//----------------------------------------------------------------------------
// UCHAR PwmChannelIdGet
//----------------------------------------------------------------------------
UCHAR PwmChannelIdGet(UCHAR ucUnit, UCHAR ucChannel)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < PWM_UNIT_MAX)
   {
      if (ucChannel < PWM_CHANNEL_MAX)
      {
         ucResult= SPwmChannelRO[ucUnit][ucChannel].ucChannelId;
      }
   }  

   return (ucResult);
}

//----------------------------------------------------------------------------
// bool PwmInitChannel
//----------------------------------------------------------------------------
bool PwmInitChannel(UCHAR ucUnit, UCHAR ucChannel)
{
   bool bResult= FALSE;
   
   if (ucUnit < PWM_UNIT_MAX)
   {
      if (ucChannel < PWM_CHANNEL_MAX)
      {
         if (SPwmRO[ucUnit].pSPwmList && SPwmRO[ucUnit].ucPwmListSize)
         {
            // Circular array init that cannot be done by the macro itsself
            ulPwmChannelTx[ucUnit][ucChannel].Size= PWM_CHANNEL_BUFFERCNTTX;
            pulPwmChannelTx[ucUnit][ucChannel]= &ulPwmChannelTx[ucUnit][ucChannel];
            
            circular_reset(ulPwmChannelTx[ucUnit][ucChannel]);
         
            // Semaphore
            os_sem_init(SPwmChannelRW[ucUnit][ucChannel].SemTx, 1);
            os_sem_init(SPwmChannelRW[ucUnit][ucChannel].SemDone, 1);
            
            // Task
            SPwmChannelTsk[ucUnit][ucChannel].ucUnit= ucUnit;
            SPwmChannelTsk[ucUnit][ucChannel].ucChannel= ucChannel;
            
            SPwmChannelRW[ucUnit][ucChannel].tidTskTx= os_tsk_create_ex(TskPwmChannelTx, SPwmChannelRO[ucUnit][ucChannel].u8TskPriTx, &SPwmChannelTsk[ucUnit][ucChannel]);
            
            bResult= (SPwmChannelRW[ucUnit][ucChannel].tidTskTx != 0);
         }
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// bool PwmStart
//----------------------------------------------------------------------------
bool PwmStart(UCHAR ucUnit, UCHAR ucChannel, U16 u16PwmCmr, U16 u16PwmCprdr, U16 u16PwmCdtyr)
{
   bool bResult= FALSE;
   
   if (ucUnit < PWM_UNIT_MAX)
   {
      if (ucChannel < PWM_CHANNEL_MAX)
      {
         if (PwmInactiveGet(ucUnit, ucChannel, OS_T_NONE))
         {
            AT91F_PWMC_CfgChannel(SPwmRO[ucUnit].pBase, PWM_CHID2UNIT(SPwmChannelRO[ucUnit][ucChannel].ucChannelId), u16PwmCmr, u16PwmCprdr, u16PwmCdtyr);
            AT91F_PWMC_InterruptEnable(SPwmRO[ucUnit].pBase, SPwmChannelRO[ucUnit][ucChannel].ucChannelId);
            AT91F_PWMC_StartChannel(SPwmRO[ucUnit].pBase, SPwmChannelRO[ucUnit][ucChannel].ucChannelId);
            
            bResult= TRUE;
         }
      }
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// bool PwmStop
//----------------------------------------------------------------------------
bool PwmStop(UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   if (ucUnit < PWM_UNIT_MAX)
   {
      if (ucChannel < PWM_CHANNEL_MAX)
      {
         if (PwmWait(ucUnit, ucChannel, u16Timeout))
         {
            AT91F_PWMC_InterruptDisable(SPwmRO[ucUnit].pBase, SPwmChannelRO[ucUnit][ucChannel].ucChannelId);
            AT91F_PWMC_StopChannel(SPwmRO[ucUnit].pBase, SPwmChannelRO[ucUnit][ucChannel].ucChannelId);
            bResult= TRUE;
         }
      }
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// USHORT PwmTxValue (ulData= PWM32_CMR16_PRDDTY16(cmr, prddty))
//----------------------------------------------------------------------------
USHORT PwmTxValue(UCHAR ucUnit, UCHAR ucChannel, ULONG ulData, U16 u16Timeout)
{
   return (PwmTx(ucUnit, ucChannel, &ulData, 1, u16Timeout));
}

//----------------------------------------------------------------------------
// USHORT PwmTx (ulData= PWM32_CMR16_PRDDTY16(cmr, prddty))
//----------------------------------------------------------------------------
USHORT PwmTx(UCHAR ucUnit, UCHAR ucChannel, ULONG *pulData, USHORT usCount, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   if (ucUnit < PWM_UNIT_MAX)
   {
      if (ucChannel < PWM_CHANNEL_MAX)
      {
         if (pulData)
         {   
            // Lock buffer
            if (OS_SEM_TAKE_CHK(os_sem_take(SPwmChannelRW[ucUnit][ucChannel].SemTx, u16Timeout)))
            {
               // Set SemDone to busy
               if (OS_SEM_TAKE_CHK(os_sem_take(SPwmChannelRW[ucUnit][ucChannel].SemDone, u16Timeout)))
               {
                  // Write to buffer (only when all data fits yet)
                  usResult= circular_write(ulPwmChannelTx[ucUnit][ucChannel], (ULONG *)pulData, usCount);
                  
                  // Set SemDone to ready is located inside ISR (which runs for filled queue only!) 
                  if (!usResult)
                  {
                     os_sem_give(SPwmChannelRW[ucUnit][ucChannel].SemDone);
                  }
                  
                  // Unlock buffer
                  os_sem_give(SPwmChannelRW[ucUnit][ucChannel].SemTx);
               }
            }
         }
      }
   }
      
   return (usResult);
}

//----------------------------------------------------------------------------
// bool PwmInactiveGet
//----------------------------------------------------------------------------
bool PwmInactiveGet(UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout)
{
   bool bResult= FALSE;
    
   if (ucUnit < PWM_UNIT_MAX)
   {
      if (ucChannel < PWM_CHANNEL_MAX)
      {
         // Check for data queue done
         if (OS_SEM_TAKE_CHK(os_sem_take(SPwmChannelRW[ucUnit][ucChannel].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SPwmRO[ucUnit].pBase->PWMC_SR & SPwmChannelRO[ucUnit][ucChannel].ucChannelId))
            {
               bResult= TRUE;
            }
            
            os_sem_give(SPwmChannelRW[ucUnit][ucChannel].SemDone);
         }
      }
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// bool PwmWait
//----------------------------------------------------------------------------
bool PwmWait(UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout)
{
   bool bResult= FALSE;
    
   if (ucUnit < PWM_UNIT_MAX)
   {
      if (ucChannel < PWM_CHANNEL_MAX)
      {
         // Check for data queue done
         if (OS_SEM_TAKE_CHK(os_sem_take(SPwmChannelRW[ucUnit][ucChannel].SemDone, u16Timeout)))
         {
            bResult= TRUE;
            
            os_sem_give(SPwmChannelRW[ucUnit][ucChannel].SemDone);
         }
      }
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// __task void TskPwmChannelTx
//----------------------------------------------------------------------------
__task void TskPwmChannelTx(void *pvParam)
{
   struct _SPwmChannelTsk *pSPwmChannelTsk= (struct _SPwmChannelTsk *) pvParam;
     
   static BOOL bTXStart[PWM_UNIT_MAX][PWM_CHANNEL_MAX]= { FALSE };
   
   FOREVER
   {
      // Lock buffer
      if (OS_SEM_TAKE_CHK(os_sem_take(SPwmChannelRW[pSPwmChannelTsk->ucUnit][pSPwmChannelTsk->ucChannel].SemTx, OS_T_FOREVER)))
      {      
         if (circular_countEntries(ulPwmChannelTx[pSPwmChannelTsk->ucUnit][pSPwmChannelTsk->ucChannel]))
         {
            bTXStart[pSPwmChannelTsk->ucUnit][pSPwmChannelTsk->ucChannel]= TRUE;
         
            // Enable Tx
            AT91F_PWMC_InterruptEnable(SPwmRO[pSPwmChannelTsk->ucUnit].pBase, SPwmChannelRO[pSPwmChannelTsk->ucUnit][pSPwmChannelTsk->ucChannel].ucChannelId);
         
            // Disable Tx, unlocking buffer and done semaphore located inside ISR
         }
         else
         {
            if (bTXStart[pSPwmChannelTsk->ucUnit][pSPwmChannelTsk->ucChannel])
            {
               bTXStart[pSPwmChannelTsk->ucUnit][pSPwmChannelTsk->ucChannel]= FALSE;               
            }
                  
            os_sem_give(SPwmChannelRW[pSPwmChannelTsk->ucUnit][pSPwmChannelTsk->ucChannel].SemTx);
         }
      }
      
      RESCHEDULE_TICKS(SPwmChannelRO[pSPwmChannelTsk->ucUnit][pSPwmChannelTsk->ucChannel].u16DelayTx);
   }
   
   DETACH;
}

//----------------------------------------------------------------------------
// void PwmChannelIrqHandler
//----------------------------------------------------------------------------
void PwmChannelIrqHandler(UCHAR ucChid)
{
   static USHORT usCountTx[PWM_UNIT_MAX][PWM_CHANNEL_MAX]= { 0 };

   register bool bPwmUpdate= FALSE;

   register UCHAR ucUnit;
   register UCHAR ucChannel;
   register UCHAR ucPwmc;
   register USHORT usResult;
   
#if PWM_UPDATE_FIX > 0
   register USHORT usRetry;
   register U32 u32PwmCcntr;
#endif

   register U32 u32PwmCmr;
   register U32 u32PwmCupdr;
      
   ULONG ulValue;
     
   // Determine unit of channel id (chid)
   for (ucUnit= 0, ucChannel= 0; (ucUnit < PWM_UNIT_MAX) && (SPwmChannelRO[ucUnit][ucChannel].ucChannelId != ucChid); ucUnit++)
   {
      for (ucChannel= 0; (ucChannel < PWM_CHANNEL_MAX) && (SPwmChannelRO[ucUnit][ucChannel].ucChannelId != ucChid); ucChannel++);
   }
   
   if (SPwmChannelRO[ucUnit][ucChannel].ucChannelId == ucChid)
   {   
      // Limit Tx buffer access to avoid unnecessary blocked tasks trying to write into the buffer
      if (circular_countEntries(ulPwmChannelTx[ucUnit][ucChannel]) && (usCountTx[ucUnit][ucChannel] <= SPwmChannelRO[ucUnit][ucChannel].u16MaxTx))
      {
         usResult= circular_read(ulPwmChannelTx[ucUnit][ucChannel], &ulValue, 1);
         if (usResult)
         {
            usCountTx[ucUnit][ucChannel]++;
            
            // Update PWM hardware registers
            
            u32PwmCmr= PWM32_GET_CMR16(ulValue);
            u32PwmCupdr= PWM32_GET_PRDDTY16(ulValue);
            
            if (u32PwmCupdr)
            {
               ucPwmc= PWM_CHID2UNIT(SPwmChannelRO[ucUnit][ucChannel].ucChannelId);

#if PWM_UPDATE_FIX > 0
               // Set new values only for pwm_ccnt > 1 (Errata rev. a 41.3.6 p.653)
               
               for (usRetry= 0, u32PwmCcntr= SPwmRO[ucUnit].pBase->PWMC_CH[ucPwmc].PWMC_CCNTR; (SPwmRO[ucUnit].pBase->PWMC_IMR & ucChid) && (u32PwmCcntr < 2) && (usRetry < PWM_UPDATE_FIX); u32PwmCcntr= SPwmRO[ucUnit].pBase->PWMC_CH[ucPwmc].PWMC_CCNTR)
               {
                  // Avoid deadlock
                  if (u32PwmCcntr == SPwmRO[ucUnit].pBase->PWMC_CH[ucPwmc].PWMC_CCNTR)
                  {
                     usRetry++;
                  }
               }
               
               // The workarround below from ATMEL will end in a deadlock for the 2nd move
               /*
               while ((SPwmRO[ucUnit].pBase->PWMC_IMR & ucChid) && \
                      (SPwmRO[ucUnit].pBase->PWMC_CH[ucPwmc].PWMC_CCNTR < 2));
               */
#endif
                             
               SPwmRO[ucUnit].pBase->PWMC_CH[ucPwmc].PWMC_CMR= u32PwmCmr;
               SPwmRO[ucUnit].pBase->PWMC_CH[ucPwmc].PWMC_CUPDR= u32PwmCupdr;

#if PWM_2NDCHANCEUPDATE_FIX > 0
               // Second chance
               SPwmRO[ucUnit].pBase->PWMC_CH[ucPwmc].PWMC_CMR= u32PwmCmr;
               SPwmRO[ucUnit].pBase->PWMC_CH[ucPwmc].PWMC_CUPDR= u32PwmCupdr;
#endif

               bPwmUpdate= TRUE;
            }
         }
      }
         
      if (!bPwmUpdate)
      {
         // Disable pwm channel IRQ
         AT91F_PWMC_InterruptDisable(SPwmRO[ucUnit].pBase, SPwmChannelRO[ucUnit][ucChannel].ucChannelId);
         isr_sem_give(SPwmChannelRW[ucUnit][ucChannel].SemTx);
         isr_sem_give(SPwmChannelRW[ucUnit][ucChannel].SemDone);
         usCountTx[ucUnit][ucChannel]= 0;
      }
   }
   
   // ARM needs EXPLICIT acknowledge (qualifier __irq doesn't do this) for (E)nd (O)f (I)RQ!!
   EOI;   
}
#endif // #if PWM_CHANNEL_MAX > 0

//----------------------------------------------------------------------------
// bool PwmCalcCprePrdDty
//----------------------------------------------------------------------------
///TBD!!! In left aligned mode, the first start value of the counter is 0. For the other periods, the counter starts at 1. (Errata rev. a 41.3.6.3)
bool PwmCalcCprePrdDty(ULONG ulClkHz, ULONG ulStableHz, UCHAR *pucCpre, U16 *pu16Prd, U16 *pu16Dty)
{
   ULONG ulModnPrd;
   ULONG ulModn;
   ULONG ulDiv;
   ULONG ulPrd;
   ULONG ulDty;
   ULONG ulHelp;
   ULONG ulCpre;
   
   bool bResult= FALSE;

   if (ulClkHz && ulStableHz)
   {
      // Calc product Modn* Prd   
      ulModnPrd= AT91C_MCK/ (ulClkHz);
      
      // Calc Mck linear divider to fit product into U16 prd register
      for (ulDiv= 1, ulHelp= ulModnPrd; ulHelp > USHRT_MAX; ulHelp -= USHRT_MAX, ulDiv++);
      
#if PWM_CPRE_FIX > 0
      // Caution: because of a (supposed) PWM hardware bug we need to divide at least by 2^1 to keep the PWM running
      ulModn= 2;
#else
      ulModn= 1;
#endif
      // Calc Modn 2^x divider fitting linear divider
      for (; ulDiv > ulModn; ulModn <<= 1);
           
      // Verify hardware support (33.6.9 PWM Channel Mode Register)
      if (ulModn <= 1024)
      {
         // Adjust Prd
         ulPrd= ulModnPrd/ ulModn;

         // Verify hardware support
         if (ulPrd <= USHRT_MAX)
         {
            // Determine counter value for required stable duration of signal
            ulDty= _ulmax((AT91C_MCK/ (ulStableHz* ulModn)), 1);
            
            // Verify required low and high duration (period must be at least twice the length of duty to ensure always required stable level length)
            if (ulPrd >= (ulDty << 1))
            {
               // Convert modn value to binary value required by CPRE field of PWM_CMR
               for (ulCpre= 0; ulModn > 1; ulModn >>= 1, ulCpre++);
               
               // Update values by their given pointers
               if (pu16Dty) *pu16Dty= (U16)(ulDty & BITS_USHORT);
               if (pu16Prd) *pu16Prd= (U16)(ulPrd & BITS_USHORT);
               if (pucCpre) *pucCpre= (U8)(ulCpre & BITS_UCHAR);
      
               bResult= TRUE;
            }
         }
      }
   }
      
   return (bResult);
}

//----------------------------------------------------------------------------
// ULONG PwmCalcHz
//----------------------------------------------------------------------------
ULONG PwmCalcHz(UCHAR ucCpre, U16 u16Prd)
{
   register ULONG ulResult= 0;
   
   register ULONG ulModn;
   register ULONG ulCpre;

   if (u16Prd > 0)
   {
      // Keep only Cpre bits (33.6.9 PWM Channel Mode Register)
      ulCpre= (ULONG)ucCpre & 0x0000000F;
      
      // Verify hardware support (33.6.9 PWM Channel Mode Register)
      if (ulCpre <= 10)
      {
         ulModn= 1 << ulCpre;
         ulResult= AT91C_MCK/ (ulModn* (ULONG)u16Prd);
      }
   }
      
   return (ulResult);
}

