//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/USART/USART.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-01-08 initial version
// 2009-01-08 removed sign on msg via direct buffer write, fixed tx IRQ init
// 2009-01-09 auto disabling of IRQs w/o data (AT91C_US_TXBUFE/AT91C_US_ENDTX)
// 2009-01-09 renamed Usart0 to Usart and added user define for usart base
// 2009-01-09 added defines for USARTPINSDATA and USARTPINSHWH
// 2009-05-29 changed task qualifier style for RL-ARM 3.50
// 2009-06-11 added timeout params for UsartRx, UsartTx and UsartMsg
// 2009-06-11 added defines for USARTID and USARTPIO
// 2009-06-16 generic function prototype macro for TSK and ISR
// 2009-06-16 US_RTOR is now configurable when calling UsartInit
// 2009-06-16 UsartInit param order reorganized / true result added
// 2009-06-16 added pin configuration support for rs485 tx enable
// 2009-06-17 changed prototype of UsartInit
// 2009-06-17 added support for external semaphore for receive timeout
// 2009-06-17 added support for user defined initial receiver timeout mode
// 2009-06-18 fixed UsartMsg to use specified timeout value
// 2009-06-18 renamed OIDSEMUSARTTORX to OIDSEMUSARTMSGNEW
// 2009-06-18 changed UsartMsg creation: IRQs with no more data flow now added
// 2009-06-18 changed TORXSem to MsgNewSem to allow notification of any msg
// 2009-06-18 MsgNewSem now given outside ISR in TskUsartMsg
// 2009-06-24 changed prototype of UsartInit
// 2009-06-24 added message mask for MsgNewSem
// 2009-06-25 added message USARTIRQRX to TskUsartRx
// 2009-06-25 filtering of messages moved from recievers to creators
// 2009-06-26 TskUsartRx sends USARTIRQRX for every single character received
// 2009-06-26 sending of USARTIRQRX moved into ISR to have timeout after data
// 2009-06-26 fixed ISR for AT91C_US_TIMEOUT to get always a timeout after rx
// 2009-06-26 USARTMBXMSG changed to use the combo count+value
// 2009-06-26 changed to use new MSG macros for count+message combo handling
// 2009-06-26 changed to use USARTMSGBITS constant
// 2009-07-29 added warning 550 ignore when DEBUG is set
// 2009-07-31 fixed code and changed prototype of UsartMsg
// 2009-07-31 SemMsgNew now only raised once per message type in TskUsartMsg
// 2009-09-03 changed UsartInit para ulTORXDiv to ulTORXMS
// 2009-09-03 changed UsartInit to calculate reload value suitable for ulTORX
// 2009-09-04 limited auto calculation of US_RTOR to 0xFFFF (max counter val)
// 2009-09-04 fixed auto calculation of US_RTOR (clearing upper word bits)
// 2009-09-04 changed UsartInit para ulTORXMS to ulTORXUS
// 2009-09-04 fixed max auto calculation value of US_RTOR (limiting to 0xFFFF)
// 2009-09-10 removed pio pin configuration (pio driver does the job now)
// 2009-09-24 reworking twice linked usart to single linked reentrant usart
// 2009-09-25 changed param order of os_mbx_declare_ex
// 2009-09-25 reworked twice linked usart to single linked reentrant usart
// 2009-09-25 code cleanup
// 2009-09-30 fixed TskUsartTx to have individual static storage for each unit
// 2009-09-30 fixed TskUsartRx to use only non-static vars yet
// 2009-10-01 fixed initialization of rtl-mbx objects by using new own macro
// 2009-10-09 changed type of ucResult in UsartInit
// 2009-10-09 added unit range check to UsartInit, UsartMsg, UsartRx, UsartTx
// 2009-10-15 changed TskUsartMsg to use RAISEMAX macro
// 2009-10-15 always using 'if (OS_SEM_TAKE_CHK(os_sem_take(...)))' now
// 2009-10-22 fixed IRQ source to u32IrqRx in TskUsartRx instead of define
// 2009-10-23 changed to support ushort block transfer
// 2009-11-06 changed UsartMsg and TskUsartMsg to support ushort msg counter
// 2009-11-11 changed TskUsartMsg to give news semaphore only once now
// 2009-12-18 UsartMsg now supports true counter decrement
// 2010-05-26 fixed new message notification by semaphore in TskUsartMsg
// 2010-06-03 fixed success check in UsartInit
// 2010-07-01 changed type of CountTx to USHORT in UsartIrqHandler
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-02 migrated from ATOMIC... to LOCKED macro
// 2010-09-23 removed obsolete include <c55x.h>
// 2010-10-22 added forced task switch in UsartMsg by RESCHEDULE(0)
// 2010-10-27 fixed new message notification in TskUsartMsg (hint by rr)
// 2010-10-27 added sem check before new message notification in TskUsartMsg
// 2010-10-27 fixed success determination for more than 255 tasks in UsartInit
// 2010-11-04 added special init of custom SCircular buffer
// 2010-11-04 removed obsolete code of UsartIrqHandler
// 2010-11-04 rewrote rx part of UsartIrqHandler and TskUsartRx
// 2010-11-23 removed obsolete data item ucChannel of _SUsartTsk
// 2010-11-23 added data comments
// 2011-04-06 added rx irq locking/unlocking to UsartIrqHandler/TskUsartRx
// 2011-06-01 fixed TskUsartMsg for full 16bit (USHORT) msg counter support
// 2011-06-01 moved news semaphore assignment before task start in UsartInit
// 2011-06-01 splitted old USART_MSGBITS to USART_MSG_MSGBITS & USART_DATA_MSGBITS
// 2011-06-01 replaced os_mbx_send's by os_mbx_send_safe in TskUsartRx
// 2011-06-01 changed TskUsartMsg to process all msgs at once (no RESCHEDULE(0))
// 2011-06-01 changed TskUsartMsg to set news sema always for valid messages
// 2011-06-30 added retriggerable tick cycle for TskUsartTx in UsartTx & UsartInit
// 2011-06-30 forever timeout of SemTickTx in TskUsartTx to avoid useless polling
// 2011-06-30 added retriggering of tick cycle for TskUsartTx in UsartIrqhandler
// 2011-07-07 fixed and optimized UsartMsg & TskUsartMsg (+ multiple bit feature)
// 2011-07-07 USART_MSGMAX is now a zero based setting
// 2011-07-13 added masking of (usually enabled) messages in TskUsartRx
// 2011-07-13 fixed masking of (usually enabled) message in TskUsartTx
// 2011-07-13 added RESCHEDULE(0) call after every os_mbx_send_safe call
// 2011-07-13 added 2nd masking of messages by consideration of USART_MSGMAX
// 2011-07-13 removed RESCHEDULE(0) call in TskUsartMsg
// 2011-07-13 added message count validation check in TskUsartMsg
// 2013-04-11 fixed function header's return data type comments
// 2013-04-11 renamed AT91B_MCK to AT91C_MCK ([B]it(mask), [C]onstant, [D]efinition, [F]unction!)
// 2013-08-21 changed delays from milliseconds to ticks
// 2013-08-21 changed RESCHEDULE (ms) to RESCHEDULE_TICKS (ticks)
// 2013-08-29 changed to _umax instrinsics suitable for used USHORTs
// 2016-03-02 removed obsolete pdc/dma stop check & force restart code from TskUsartRx
// 2016-03-02 data rx interrupt disabled only for full pdc/dma circular buffer yet
// 2016-03-03 added double bank pdc/dma support for latency free bank switching
// 2017-04-11 fixed AT91C_US_RXBUFF handler to support also UsartIrq Rx overrun
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

#include "usart.h"
#include "usartext.h"
#include "usartdata.h"
#include "usartdev.h"

//----------------------------------------------------------------------------
// void IsrUsartx __irq
//----------------------------------------------------------------------------
// Each supported usart unit requires a function definition here
#if USART_UNIT_MAX > 0
   USART_ISR_FUNC(0);
#endif

#if USART_UNIT_MAX > 1
   USART_ISR_FUNC(1);
#endif

//----------------------------------------------------------------------------
// UCHAR UsartInit
//----------------------------------------------------------------------------
UCHAR UsartInit(UCHAR ucUnit, UCHAR ucChannel, ULONG ulBaud, ULONG ulOption, ULONG ulTORXUS, ULONG ulTORXModeInit, OS_ID oiMsgNewSem, ULONG ulMsgMask, ULONG ulTimeGuard)
{
   UCHAR ucResult= 0;
   ULONG ulTORXCnt;

   if (ucUnit < USART_UNIT_MAX)
   {
      // Circular array init that cannot be done by the macro itsself
      ucUsartRx[ucUnit].Size= USART_BUFFERCNTRX;
      pucUsartRx[ucUnit]= &ucUsartRx[ucUnit];
      ucUsartTx[ucUnit].Size= USART_BUFFERCNTTX;
      pucUsartTx[ucUnit]= &ucUsartTx[ucUnit];
   
      circular_reset(ucUsartRx[ucUnit]);
      circular_reset(ucUsartTx[ucUnit]);
    
      // Circular buffer handling for rx pdc
      SUsartRW[ucUnit].SCircular.ucCntEntries= 0;
      SUsartRW[ucUnit].SCircular.ucIdxRead= 0;
      SUsartRW[ucUnit].SCircular.ucIdxWrite= 0;
      SUsartRW[ucUnit].SCircular.ucSize= USART_PDCDEPTHRX;
     
      OS_MBX_INIT_EX(MbxUsartRx[ucUnit]);
      OS_MBX_INIT_EX(MbxUsartMsg[ucUnit]);
       
      // Init USART
   
      // Clock
      AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, 1 << SUsartRO[ucUnit].u32PeriphId);
                
      // Params
      AT91F_US_Configure(SUsartRO[ucUnit].pBase, AT91C_MCK, AT91C_US_ASYNC_MODE | ulOption, ulBaud, ulTimeGuard);
      
      // Rx Peripheral DMA Controller (pdc)
      SUsartRO[ucUnit].pBase->US_PTCR= AT91C_PDC_RXTDIS;
      
      // Clear channel rx
      SUsartRO[ucUnit].pBase->US_RPR= 0L;
      SUsartRO[ucUnit].pBase->US_RCR= 0;
            
      // Set reload values for above channel rx
      SUsartRO[ucUnit].pBase->US_RNPR= 0L;
      SUsartRO[ucUnit].pBase->US_RNCR= 0;
      
      SUsartRO[ucUnit].pBase->US_PTCR= AT91C_PDC_RXTEN;
      
      // Tx Peripheral DMA Controller (pdc)
      SUsartRO[ucUnit].pBase->US_PTCR= AT91C_PDC_TXTDIS;
      
      // Clear channel tx
      SUsartRO[ucUnit].pBase->US_TPR= 0L;
      SUsartRO[ucUnit].pBase->US_TCR= 0;
      
      // Set reload values for above channel tx
      SUsartRO[ucUnit].pBase->US_TNPR= 0L;
      SUsartRO[ucUnit].pBase->US_TNCR= 0;
      
      SUsartRO[ucUnit].pBase->US_PTCR= AT91C_PDC_TXTEN;
       
      // Rx Timeout ms (3413ms @19200bps (52us* 0xFFFF); refer to manual at 30.6.3.8 "Receiver Time-out")
      ulTORXCnt= ((ULONG) ceil(ulTORXUS/ (1000000.0L/ ulBaud)));
      ulTORXCnt= (ulTORXCnt & 0xFFFF0000) ? (0x0000FFFF) : (ulTORXCnt);
      SUsartRO[ucUnit].pBase->US_RTOR= ulTORXCnt;
               
      // Semaphores
      os_sem_init(SUsartRW[ucUnit].SemMsg, 1);
      os_sem_init(SUsartRW[ucUnit].SemRx, 1);
      os_sem_init(SUsartRW[ucUnit].SemTx, 1);
      os_sem_init(SUsartRW[ucUnit].SemTickTx, 0);
      
      // Assign given semaphore and mask for new messages
      SUsartRW[ucUnit].oiSemMsgNew= oiMsgNewSem;
      SUsartRW[ucUnit].ulMsgMask= ulMsgMask;
      
      #ifdef DEBUG
         memset(&ucUsartRx[ucUnit].Data, 0x43, ucUsartRx[ucUnit].Size);
      #endif
      
      // Tasks
      SUsartTsk[ucUnit].ucUnit= ucUnit;
      
      SUsartRW[ucUnit].tidTskMsg= os_tsk_create_ex(TskUsartMsg, SUsartRO[ucUnit].u8TskPriMsg, &SUsartTsk[ucUnit]);
      SUsartRW[ucUnit].tidTskRx= os_tsk_create_ex(TskUsartRx, SUsartRO[ucUnit].u8TskPriRx, &SUsartTsk[ucUnit]);
      SUsartRW[ucUnit].tidTskTx= os_tsk_create_ex(TskUsartTx, SUsartRO[ucUnit].u8TskPriTx, &SUsartTsk[ucUnit]);
      
      // Interrupt USART
      // Clear pending IRQs
      AT91F_AIC_ClearIt(AT91C_BASE_AIC, SUsartRO[ucUnit].u32PeriphId);
      
      // Tx IRQ is enabled later by TskUsartTx
      AT91F_US_EnableIt(SUsartRO[ucUnit].pBase, (SUsartRO[ucUnit].u32IrqMsg & ~SUsartRO[ucUnit].u32IrqTx));
      AT91F_US_DisableIt(SUsartRO[ucUnit].pBase, ~((SUsartRO[ucUnit].u32IrqMsg)) | SUsartRO[ucUnit].u32IrqTx);
          
      // IRQ ctrl
      AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, SUsartRO[ucUnit].u32PeriphId, 0, 0, IRQ_CAST(SUsartRO[ucUnit].Isr));
      AT91F_AIC_EnableIt(AT91C_BASE_AIC, SUsartRO[ucUnit].u32PeriphId);
      
      // IRQ go (rx, tx and timeout timer)
      AT91F_US_EnableRx(SUsartRO[ucUnit].pBase);
      AT91F_US_EnableTx(SUsartRO[ucUnit].pBase);
      SUsartRO[ucUnit].pBase->US_CR= ulTORXModeInit;
       
      // Verify success
      if (SUsartRW[ucUnit].tidTskMsg && SUsartRW[ucUnit].tidTskRx && SUsartRW[ucUnit].tidTskTx)
      {
         ucResult= 1;
      }
   }
      
   return (ucResult);
}

//----------------------------------------------------------------------------
// USHORT UsartMsg
//----------------------------------------------------------------------------
USHORT UsartMsg(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   USHORT usCounter;
   UCHAR ucIndex;
      
   if (ucUnit < USART_UNIT_MAX)
   {
      RESCHEDULE_TICKS(0);
      
      // Lock counters
      if (OS_SEM_TAKE_CHK(os_sem_take(SUsartRW[ucUnit].SemMsg, u16Timeout)))
      {
         for (ucIndex= 0; ucIndex < USART_MSGMAX; ucIndex++)
         {
            if ((1 << ucIndex) & ulMsg)
            {
               usCounter= SUsartRW[ucUnit].xMsgCounter[ucIndex];
               if (usCounter)
               {
                  RAISEMAX(usResult, usCounter, USHRT_MAX);

                  if (usDecrement)
                  {
                     if (usDecrement < usCounter)
                     {
                        SUsartRW[ucUnit].xMsgCounter[ucIndex] -= usDecrement;
                     }
                     else
                     {
                        SUsartRW[ucUnit].xMsgCounter[ucIndex]= 0;
                     }
                  }
               }
            }
         }
   
         // Unlock counters
         os_sem_give(SUsartRW[ucUnit].SemMsg);   
      }
   }
         
   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT UsartRx
//----------------------------------------------------------------------------
USHORT UsartRx(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout)
{
   USHORT usResult= 0;

#ifdef DEBUG      
   USHORT usIndex, usDummy;
#endif

   if (ucUnit < USART_UNIT_MAX)
   {
      if (pvData)
      {
         // Lock buffer
         if (OS_SEM_TAKE_CHK(os_sem_take(SUsartRW[ucUnit].SemRx, u16Timeout)))
         {
            // Read buffer
            usResult= circular_read(ucUsartRx[ucUnit], (UCHAR *)pvData, usCount);
                  
#ifdef DEBUG         
            for (usIndex= 0; usIndex < usResult; usIndex++)
            {
               if (*((UCHAR *)pvData+ ucIndex) < 0x20)
               {
                  usDummy= 0;
               }
            }
#endif
      
            // Unlock buffer
            os_sem_give(SUsartRW[ucUnit].SemRx);
         }
      }
   }
      
   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT UsartTx
//----------------------------------------------------------------------------
USHORT UsartTx(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   if (ucUnit < USART_UNIT_MAX)
   {
      if (pvData)
      {   
         // Lock buffer
         if (OS_SEM_TAKE_CHK(os_sem_take(SUsartRW[ucUnit].SemTx, u16Timeout)))
         {
            // Write to buffer (only when all data fits yet)
            usResult= circular_write(ucUsartTx[ucUnit], (UCHAR *)pvData, usCount);
         
            // Unlock buffer
            os_sem_give(SUsartRW[ucUnit].SemTx);
            
            // Trigger TskUsartTx now
            if (!OS_SEM_TAKE_CHK(os_sem_take(SUsartRW[ucUnit].SemTickTx, OS_T_NONE)))
            {
               if (SUsartRW[ucUnit].SemTickTx) os_sem_give(SUsartRW[ucUnit].SemTickTx);
            }
         }
      }
   }
      
   return (usResult);
}

//----------------------------------------------------------------------------
// __task void TskUsartMsg
//----------------------------------------------------------------------------
__task void TskUsartMsg(void *pvParam)
{
   struct _SUsartTsk *pSUsartTsk= (struct _SUsartTsk *) pvParam;
   
   ULONG ulValue;
   USHORT usCount;
   UCHAR ucIndex;

#define DEBUG 1
#pragma diag_remark 550
   
#ifdef DEBUG
   UCHAR ucDummy;
#endif
   
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxUsartMsg[pSUsartTsk->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         usCount= MSG_GETCNT(USART_MSG_MSGBITS, ulValue);
         if (usCount)
         {
            // Lock counters
            if (OS_SEM_TAKE_CHK(os_sem_take(SUsartRW[pSUsartTsk->ucUnit].SemMsg, OS_T_FOREVER)))
            {         
               // Check message bits
               for (ucIndex= 0; ucIndex < USART_MSGMAX; ucIndex++)
               {
                  if (ulValue & (1 << ucIndex))
                  {
                     // Count until max-1 and set max to show overrun condition
                     RAISEMAX(SUsartRW[pSUsartTsk->ucUnit].xMsgCounter[ucIndex], usCount, USART_MSGCOUNTERMAX);
                  }
               }
            
               // Unlock counters
               os_sem_give(SUsartRW[pSUsartTsk->ucUnit].SemMsg);
            }
               
            // New message notification
            if (SUsartRW[pSUsartTsk->ucUnit].oiSemMsgNew)
            {
               if (!OS_SEM_TAKE_CHK(os_sem_check(SUsartRW[pSUsartTsk->ucUnit].oiSemMsgNew, FALSE)))
               {
                  os_sem_give(SUsartRW[pSUsartTsk->ucUnit].oiSemMsgNew);
               }
               UNLOCK;
            }
                    
#ifdef DEBUG
            if (usCount < 1 || usCount > 3)
            {
               ucDummy= 0;
            }
            
            if (MSG_GETMSG(USART_MSG_MSGBITS, ulValue) == AT91C_US_TIMEOUT)
            {
               ucDummy= 0;
            }
         }
   #endif
   #undef DEBUG         
      }
   }

   DETACH;
}

//----------------------------------------------------------------------------
// __task void TskUsartRx
//----------------------------------------------------------------------------
__task void TskUsartRx(void *pvParam)
{
   struct _SUsartTsk *pSUsartTsk= (struct _SUsartTsk *) pvParam;
   
   BOOL bFullUsartRx= FALSE;

   ULONG ulMsg;
   ULONG ulMsgMask;
   ULONG ulValue;
   ULONG ulData;
   USHORT usCount;
   USHORT usItemCnt;
   USHORT usResult;
   UCHAR ucIdxRead;
  
   FOREVER
   {
      // Lock buffer
      if (OS_SEM_TAKE_CHK(os_sem_take(SUsartRW[pSUsartTsk->ucUnit].SemRx, OS_T_FOREVER)))
      {
         bFullUsartRx= (circular_freeWrite(ucUsartRx[pSUsartTsk->ucUnit]) < SUsartRO[pSUsartTsk->ucUnit].u16PDCMaxRx);
      
         // Unlock buffer
         os_sem_give(SUsartRW[pSUsartTsk->ucUnit].SemRx);
      }
            
      if (!bFullUsartRx)
      {
         if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxUsartRx[pSUsartTsk->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
         {
            usCount= MSG_GETCNT(USART_DATA_MSGBITS, ulValue);
            ulData= MSG_GETMSG(USART_DATA_MSGBITS, ulValue);
            ucIdxRead= (ulData & BITS_ULONGHL) >> BITS_OF(USHORT);
            usItemCnt= ulData & BITS_USHORT;

            if (usCount && usItemCnt)
            {
               // Lock buffer
               if (OS_SEM_TAKE_CHK(os_sem_take(SUsartRW[pSUsartTsk->ucUnit].SemRx, OS_T_FOREVER)))
               {
                  // Retrieve message mask
                  LOCKED(ulMsgMask= SUsartRW[pSUsartTsk->ucUnit].ulMsgMask);
                  
                  // Casting ulValue to (UCHAR *) is very important here because the flexible circular buffer
                  // otherwise will calculate the positions using ULONG alignment but we have a byte stream here!                 
                  usResult= circular_write(ucUsartRx[pSUsartTsk->ucUnit], (UCHAR *)&SUsartRW[pSUsartTsk->ucUnit].ucPDCBufferRx[ucIdxRead][0], usItemCnt);
                  if (usResult >= usItemCnt)
                  {   
                     ulMsg= SUsartRO[pSUsartTsk->ucUnit].u32IrqRx;
                     
                     // Apply channel message filter
                     ulMsg&= ulMsgMask;
                     
                     // Apply unit max counter index available
                     ulMsg&= ((1 << (USART_MSGMAX+ 1))- 1);
                     
                     if (ulMsg)
                     {
                        os_mbx_send_safe(&MbxUsartMsg[pSUsartTsk->ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(USART_MSG_MSGBITS, ulMsg, usItemCnt)), OS_T_FOREVER);
                        RESCHEDULE_TICKS(0);
                     }
                  }
                  else
                  {
                     ulMsg= AT91C_US_OVRE;
                     
                     // Apply channel message filter
                     ulMsg&= ulMsgMask;

                     // Apply unit max counter index available
                     ulMsg&= ((1 << (USART_MSGMAX+ 1))- 1);
                     
                     if (ulMsg)
                     {
                        // Send msg AT91C_US_OVRES to this channel counter
                        os_mbx_send_safe(&MbxUsartMsg[pSUsartTsk->ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(USART_MSG_MSGBITS, ulMsg, usItemCnt)), OS_T_FOREVER);
                        RESCHEDULE_TICKS(0);
                     }
                  }
                                 
                  // Unlock buffer
                  os_sem_give(SUsartRW[pSUsartTsk->ucUnit].SemRx);
               }
            }
            
            // Enable the rx irq again
            AT91F_US_EnableIt(SUsartRO[pSUsartTsk->ucUnit].pBase, SUsartRO[pSUsartTsk->ucUnit].u32IrqRx);
         }
      }
      else
      {
         // Maximum check period
         RESCHEDULE_TICKS(SUsartRO[pSUsartTsk->ucUnit].u16DelayRx);
      }
   }
}

//----------------------------------------------------------------------------
// __task void TskUsartTx
//----------------------------------------------------------------------------
__task void TskUsartTx(void *pvParam)
{
   struct _SUsartTsk *pSUsartTsk= (struct _SUsartTsk *) pvParam;
   
   ULONG ulMsg;
   ULONG ulMsgMask;
   
   static BOOL bTXStart[USART_UNIT_MAX]= { FALSE };
   
   FOREVER
   {
      // Lock buffer
      if (OS_SEM_TAKE_CHK(os_sem_take(SUsartRW[pSUsartTsk->ucUnit].SemTx, OS_T_FOREVER)))
      {      
         if (circular_countEntries(ucUsartTx[pSUsartTsk->ucUnit]))
         {
            bTXStart[pSUsartTsk->ucUnit]= TRUE;
            
            // Enable Tx
            AT91F_US_EnableIt(SUsartRO[pSUsartTsk->ucUnit].pBase, (SUsartRO[pSUsartTsk->ucUnit].u32IrqTx));
            
            // Disable Tx and unlocking buffer (after specified number of characters) located inside ISR
         }
         else
         {
            if (bTXStart[pSUsartTsk->ucUnit])
            {
               bTXStart[pSUsartTsk->ucUnit]= FALSE;
               
               // Add irq without data action to messages
               LOCKED(ulMsgMask= SUsartRW[pSUsartTsk->ucUnit].ulMsgMask);
               
               ulMsg= SUsartRO[pSUsartTsk->ucUnit].u32IrqTx;
               
               // Apply channel message filter
               ulMsg&= ulMsgMask;

               // Apply unit max counter index available
               ulMsg&= ((1 << (USART_MSGMAX+ 1))- 1);
                     
               if (ulMsg)
               {
                  os_mbx_send_safe(&MbxUsartMsg[pSUsartTsk->ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(USART_MSG_MSGBITS, ulMsg, 1)), _umax(1, SUsartRO[pSUsartTsk->ucUnit].u16DelayTx));
                  RESCHEDULE_TICKS(0);
               }
            }
                  
            os_sem_give(SUsartRW[pSUsartTsk->ucUnit].SemTx);
         }
      }

      // Suspend task until new job trigger by UsartTx or post job trigger by UsartIrqHandler
      os_sem_take(SUsartRW[pSUsartTsk->ucUnit].SemTickTx, OS_T_FOREVER);
      
      // Limit cycle time
      if (SUsartRO[pSUsartTsk->ucUnit].u16DelayTx) RESCHEDULE_TICKS(SUsartRO[pSUsartTsk->ucUnit].u16DelayTx);
   }
   
   DETACH;
}

//----------------------------------------------------------------------------
// void UsartIrqHandler
//----------------------------------------------------------------------------
void UsartIrqHandler(UCHAR ucUnit)
{
   static USHORT usRxLeft[USART_UNIT_MAX]= { 0 };

   register ULONG ulUsartIrq;
   register ULONG ulUsartMsg;
   
   register USHORT usResult;

   register USHORT usRxLeftTemp;
   register USHORT usRxDone;
   
   register UCHAR ucChannel;
   
#ifdef DEBUG
   UCHAR ucDummy;
#endif

   // Copy and clear (US_IMR) the IRQ source to handle yet
   ulUsartIrq= SUsartRO[ucUnit].pBase->US_IMR & SUsartRO[ucUnit].pBase->US_CSR;
   
   // Clear all data IRQ sources (they may be set later when there is no more data action when they occur)
   ulUsartMsg= ((ulUsartIrq & ~(AT91C_US_RXRDY | AT91C_US_RXBUFF | AT91C_US_TXRDY | AT91C_US_ENDTX | AT91C_US_TXBUFE)));
   
   // Rx double bank pdc (supports RTS/CTS handshake) counters are zero after startup
   if (ulUsartIrq & AT91C_US_RXBUFF)
   {
      for (ucChannel= SUsartRW[ucUnit].SCircular.ucCntEntries, usResult= 0; (ucChannel < SUsartRW[ucUnit].SCircular.ucSize) && (usResult < 2); ucChannel++, usResult++)
      {
         // Queue pdx rx bank
         SUsartRO[ucUnit].pBase->US_RNPR= (UINT) &SUsartRW[ucUnit].ucPDCBufferRx[SUsartRW[ucUnit].SCircular.ucIdxWrite];
         SUsartRO[ucUnit].pBase->US_RNCR= SUsartRO[ucUnit].u16PDCMaxRx;
         
         // Increment write index
         SUsartRW[ucUnit].SCircular.ucIdxWrite= (SUsartRW[ucUnit].SCircular.ucIdxWrite+ 1) % SUsartRW[ucUnit].SCircular.ucSize;  
      }
      
      // usCntEntries is incremented in advance
      if (SUsartRW[ucUnit].SCircular.ucCntEntries < SUsartRW[ucUnit].SCircular.ucSize)
      {
         // Raise usCntEntries upto usSize+ 1
         RAISEMAX(SUsartRW[ucUnit].SCircular.ucCntEntries, 1, (SUsartRW[ucUnit].SCircular.ucSize+ 1));
      }
   }      
   // Rx single bank pdc (supports RTS/CTS handshake) counter is zero (just queue a new bank for next bank switch)
   else if (ulUsartIrq & AT91C_US_ENDRX)
   {
      // Check for a free buffer to set the rx pdc to write to (usCntEntries is incremented in advance)
      if (SUsartRW[ucUnit].SCircular.ucCntEntries <= SUsartRW[ucUnit].SCircular.ucSize)
      {   
         if (!SUsartRO[ucUnit].pBase->US_RNCR)
         {
            // Prepare pdc rx
            SUsartRO[ucUnit].pBase->US_RNPR= (UINT) &SUsartRW[ucUnit].ucPDCBufferRx[SUsartRW[ucUnit].SCircular.ucIdxWrite];
            SUsartRO[ucUnit].pBase->US_RNCR= SUsartRO[ucUnit].u16PDCMaxRx;
         
            if (SUsartRW[ucUnit].SCircular.ucCntEntries > 0)
            {
               // Determine item count
               usRxDone= (SUsartRO[ucUnit].u16PDCMaxRx- usRxLeft[ucUnit]) & BITS_ULONGLL;
               usRxLeft[ucUnit]= 0;

               // Send rx msg to task including fixed count of received items and current rx buffer index to read from
               if (isr_mbx_check(&MbxUsartRx[ucUnit]) > 0)
               {
                  // Trigger rx task to process received data with given count
                  isr_mbx_send(&MbxUsartRx[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(USART_DATA_MSGBITS, (((ULONG)SUsartRW[ucUnit].SCircular.ucIdxRead) << 16) | (ULONG)usRxDone, SUsartRW[ucUnit].SCircular.ucCntEntries)));

                  // Increment read index in advance
                  SUsartRW[ucUnit].SCircular.ucIdxRead= (SUsartRW[ucUnit].SCircular.ucIdxRead+ 1) % SUsartRW[ucUnit].SCircular.ucSize;
                  
                  // Decrement count in advance
                  if (SUsartRW[ucUnit].SCircular.ucCntEntries) SUsartRW[ucUnit].SCircular.ucCntEntries--;
               }
            }

            // usCntEntries and usIdxWrite are incremented in advance

            if (SUsartRW[ucUnit].SCircular.ucCntEntries < SUsartRW[ucUnit].SCircular.ucSize)
            {
               // Raise usCntEntries upto usSize+ 1
               RAISEMAX(SUsartRW[ucUnit].SCircular.ucCntEntries, 1, (SUsartRW[ucUnit].SCircular.ucSize+ 1));
                  
               // Increment write index
               SUsartRW[ucUnit].SCircular.ucIdxWrite= (SUsartRW[ucUnit].SCircular.ucIdxWrite+ 1) % SUsartRW[ucUnit].SCircular.ucSize;            
            }
            else
            {
               // Disable the rx irq yet - finished task will re-enable it again
               AT91F_US_DisableIt(SUsartRO[ucUnit].pBase, SUsartRO[ucUnit].u32IrqRx);
               
               // Debug
               usResult= usResult;
            }
         }
      }
   }
   
   // Tx using single bank pdc
   if ((ulUsartIrq & AT91C_US_ENDTX) || (ulUsartIrq & AT91C_US_TXBUFE))
   {
      usResult= circular_read(ucUsartTx[ucUnit], SUsartRW[ucUnit].ucPDCBufferTx, SUsartRO[ucUnit].u16PDCMaxTx);
      
      if (usResult)
      {
         SUsartRO[ucUnit].pBase->US_TNPR= (UINT) SUsartRW[ucUnit].ucPDCBufferTx;
         SUsartRO[ucUnit].pBase->US_TNCR= (USHORT) usResult;
      }

      AT91F_US_DisableIt(SUsartRO[ucUnit].pBase, (SUsartRO[ucUnit].u32IrqTx));
      isr_sem_give(SUsartRW[ucUnit].SemTx);
      
      // Trigger TskUsartTx for post processing
      if (!ISR_SEM_TAKE_CHK(isr_sem_check(SUsartRW[ucUnit].SemTickTx)))
      {
         isr_sem_give(SUsartRW[ucUnit].SemTickTx);
      }
   }
   
   // Rx timeout when less then pdc specified bytes received
   if (ulUsartIrq & AT91C_US_TIMEOUT)
   {           
      // Check pdc for still unfinished transfer
      if (SUsartRO[ucUnit].pBase->US_PTSR & AT91C_PDC_RXTEN)
      {     
         if (SUsartRO[ucUnit].pBase->US_RCR)
         {           
            if (SUsartRO[ucUnit].pBase->US_RCR != SUsartRO[ucUnit].u16PDCMaxRx)
            {
               // Some bytes done yet
               
               // Stop pdc
               SUsartRO[ucUnit].pBase->US_PTCR= AT91C_PDC_RXTDIS;
               
               // Determine number of bytes done so far
               usRxLeftTemp= SUsartRO[ucUnit].pBase->US_RCR;
               if (usRxLeftTemp > 0 && usRxLeftTemp < SUsartRO[ucUnit].u16PDCMaxRx)
               {
                  // pdc timeout (Byte count not a multiple of pdc block size / Not all by pdc exspected bytes received yet)
               
                  usRxLeft[ucUnit]= usRxLeftTemp;
                  SUsartRO[ucUnit].pBase->US_RCR= 0;
                  
                  // Clear timeout message
                  ulUsartMsg&= ~AT91C_US_TIMEOUT;                   
               }
   
               // Continue pdc
               SUsartRO[ucUnit].pBase->US_PTCR= AT91C_PDC_RXTEN;
            }
         }
      }
      
      // Re-arm the timer
      if (ulUsartMsg & AT91C_US_TIMEOUT)
      {
         // Trigger is now any incoming byte
         SUsartRO[ucUnit].pBase->US_CR= AT91C_US_STTTO;
      }
      else
      {
         // Not setting a new trigger condition keeps the IRQ active to get the timeout suffix after the received data
      }
         
#ifdef DEBUG
      ucDummy= 0;
#endif
   }
   
   // Record all other yet unhandled IRQ sources (includes communication errors)
   
   // Apply message filter
   ulUsartMsg&= SUsartRW[ucUnit].ulMsgMask;
   
   // Apply max counter index available
   ulUsartMsg&= ((1 << (USART_MSGMAX+ 1))- 1);
   
   if (ulUsartMsg)
   {
      if (isr_mbx_check(&MbxUsartMsg[ucUnit]) > 0)
      {
         isr_mbx_send(&MbxUsartMsg[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(USART_MSG_MSGBITS, ulUsartMsg, 1)));  
      }
   }
   
   // Reset PARE, FRAME, OVRE and RXBRK
   SUsartRO[ucUnit].pBase->US_CR= AT91C_US_RSTSTA;

   // ARM needs EXPLICIT acknowledge (qualifier __irq doesn't do this) for (E)nd (O)f (I)RQ!!
   EOI;
}
