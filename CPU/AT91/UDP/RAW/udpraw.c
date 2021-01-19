//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/UDP/RAW/UDPRAW.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2014-11-13 initial version
// 2015-02-05 added user stacks for TskUdprawRx & TskUdprawTx
// 2015-02-05 fixed data access in UdprawIrqHandlerChannel
// 2015-02-25 moved init of masks UDP_IMR & UDP_CSR to AT91C_UDP_ENDBUSRES irq
// 2015-02-26 fixed access by SUdprawChannelMapRW (reserved for SUdprawChannelRW)
// 2015-03-05 fixed interface of UdprawInitChannel
// 2015-03-11 fixed message data for setup/data rx in UdprawIrqHandlerChannel
// 2015-03-18 added reset of u8RxBkFirst in TskUdprawRxChannel
// 2015-04-09 fixed client buffer and zero length support in TskUdprawTxChannel
// 2015-04-09 fixed zero length packet support in UdprawIrqHandlerChannel
// 2015-04-16 removed obsolete para u8FAdd in UdprawInitUnit
// 2015-05-21 fixed TskUdprawTxChannel to check both channel and count of msg cnt
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

#include "udpraw.h"
#include "udprawint.h"
#include "udprawext.h"
#include "udprawdata.h"
#include "udprawdev.h"

// Customized stacks
OS_TSK_STACK_USER_EX(StkTskUdprawMsg, UDPRAW_STACK_TSKUDPRAWMSG, [UDPRAW_UNIT_MAX]);
OS_TSK_STACK_USER_EX(StkTskUdprawMsgChannel, UDPRAW_STACK_TSKUDPRAWMSGCHANNEL, [UDPRAW_UNIT_MAX]);
OS_TSK_STACK_USER_EX(StkTskUdprawRxChannel, UDPRAW_STACK_TSKUDPRAWRXCHANNEL, [UDPRAW_UNIT_MAX][UDPRAW_CHANNEL_MAX]);
OS_TSK_STACK_USER_EX(StkTskUdprawTxChannel, UDPRAW_STACK_TSKUDPRAWTXCHANNEL, [UDPRAW_UNIT_MAX][UDPRAW_CHANNEL_MAX]);

//----------------------------------------------------------------------------
// void IsrUdpraw __irq
//----------------------------------------------------------------------------
// Each supported udpraw unit requires a function definition here
#if UDPRAW_UNIT_MAX > 0
   UDPRAW_ISR_FUNC(0);
#endif

#if UDPRAW_UNIT_MAX > 1
   UDPRAW_ISR_FUNC(1);
#endif

//----------------------------------------------------------------------------
// UCHAR UdprawInit
//----------------------------------------------------------------------------
UCHAR UdprawInitUnit(UCHAR ucUnit, UCHAR ucChannel, OS_ID oiSemMsgNew, ULONG ulMsgMask)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < UDPRAW_UNIT_MAX)
   {
      if (ucChannel < UDPRAW_CHANNELID_MAX)
      {
         // Identification
         SUdprawRW[ucUnit].ucUnit= ucUnit;
         
         // Channel count reset
         SUdprawRW[ucUnit].ucChannelIdx= 0;
         
         // Mailbox(es)
         OS_MBX_INIT_EX(MbxUdprawMsg[ucUnit]);
         OS_MBX_INIT_EX(MbxUdprawMsgChannel[ucUnit]);
         
         // Init UDP
         
         // Clock(s)
         
         // Peripheral
         AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, 1 << SUdprawRO[ucUnit].u32PeriphId);
         
         // USB
         AT91F_PMC_CfgSysClkEnableReg(AT91C_BASE_PMC, AT91C_PMC_UDP);
            
         // Semaphore(s)
         os_sem_init(SUdprawRW[ucUnit].SemMsg, 1);
         
         // Assign given semaphore and mask for new messages
         SUdprawRW[ucUnit].oiSemMsgNew= oiSemMsgNew;
         SUdprawRW[ucUnit].ulMsgMask= ulMsgMask;
         
         // Task(s)
         SUdprawTsk[ucUnit].ucUnit= ucUnit;
         
         SUdprawRW[ucUnit].tidTskMsg= OS_TSK_CREATE_USER_EX(TskUdprawMsg, SUdprawRO[ucUnit].u8TskPriMsg, StkTskUdprawMsg[ucUnit], &SUdprawTsk[ucUnit]);
         SUdprawRW[ucUnit].tidTskMsgChannel= OS_TSK_CREATE_USER_EX(TskUdprawMsgChannel, SUdprawRO[ucUnit].u8TskPriMsg, StkTskUdprawMsgChannel[ucUnit], &SUdprawTsk[ucUnit]);
         
         // Interrupt UDP
         // Clear pending IRQs
         AT91F_AIC_ClearIt(AT91C_BASE_AIC, SUdprawRO[ucUnit].u32PeriphId);
         
         // IRQ ctrl
         AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, SUdprawRO[ucUnit].u32PeriphId, 0, 0, IRQ_CAST(SUdprawRO[ucUnit].Isr));
         AT91F_AIC_EnableIt(AT91C_BASE_AIC, SUdprawRO[ucUnit].u32PeriphId);
         
         // Clear (after reset) undefined IRQ flags first (refer to AT91SAM7X manual 34.6.7 page 475)
         AT91F_UDP_InterruptClearRegister(SUdprawRO[ucUnit].pBase, AT91C_UDP_WAKEUP);

         //-IRQ enable for required unit messages (UDP_IER, 13...8) with inactive data irqs yet (activated by channels later)
         //-AT91F_UDP_Enable(SUdprawRO[ucUnit].pBase, SUdprawRO[ucUnit].u32Irq);
         //          
         // IRQ enable only base irq AT91C_UDP_ENDBUSRES which will enable required unit messages (UDP_IER, 13...8) and data irqs (UDP_CSR) later in UdprawHandler
         AT91F_UDP_EnableIt(SUdprawRO[ucUnit].pBase, AT91C_UDP_ENDBUSRES);
         
         // Verify success
         if (SUdprawRW[ucUnit].tidTskMsg && SUdprawRW[ucUnit].tidTskMsgChannel)
         {
            ucResult= 1;
         }
         
         // Update unit flags
         SUdprawRW[ucUnit].ucFlags= (ucResult) ? (UDPRAW_UNIT_FLAGS_CONFIGDONE) : (0);
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR UdprawInitChannel
//----------------------------------------------------------------------------
UCHAR UdprawInitChannel(UCHAR ucUnit, UCHAR ucChannel, USHORT usType, UCHAR *pucRx, OS_ID oiMbxRxQ, OS_ID *poiSemRx, UCHAR *pucTx, OS_ID *poiMbxTxQ, OS_ID oiMbxTxQ, OS_ID oiSemMsgNew, ULONG ulMsgMask)
{
   UCHAR ucResult= 0;
   UCHAR ucValid= FALSE;
   
   if (ucUnit < UDPRAW_UNIT_MAX)
   {
      if (SUdprawRW[ucUnit].ucFlags & UDPRAW_UNIT_FLAGS_CONFIGDONE)
      {
         if ((ucChannel < UDPRAW_CHANNELID_MAX) && (SUdprawRW[ucUnit].ucChannelIdx < UDPRAW_CHANNEL_MAX))
         {
            // Validate arguments
            if (!(usType & (~AT91C_UDP_EPTYPE)))
            {
               // Para valid state may be invalidated later below
               ucValid= TRUE;
               
               // Type rx/host-out or rx/host-out & tx/host-in (control)
               if (UDPRAW_IS_EPTYPE_OUT(usType) || UDPRAW_IS_EPTYPE_BOTH(usType))
               {
                  if (!(pucRx && oiMbxRxQ && poiSemRx))
                  {
                     ucValid= FALSE;
                  }
               }
               // Type tx/host-in or rx/host-out & tx/host-in (control)
               else if (UDPRAW_IS_EPTYPE_IN(usType) || UDPRAW_IS_EPTYPE_BOTH(usType))
               {
                  if (!(pucTx && poiMbxTxQ && oiMbxTxQ))
                  {
                     ucValid= FALSE;
                  }
               }
            }

            // Continue with valid arguments only
            if (ucValid)
            {
               // Storage mapping
               SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx= SUdprawRW[ucUnit].ucChannelIdx++;
               
               // Identification
               SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucUnit= ucUnit;
               
               // Type of channel (endpoint)
               SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].usType= usType;
               
               // Para rx/host-out (null'd or set by para)
               SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].pucRx= pucRx;
               SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].oiMbxRxQ= oiMbxRxQ;
               
               // Para tx/host-in (null'd or storage, always set by para)
               SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].pucTx= pucTx;
               SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].oiMbxTxQ= oiMbxTxQ;
               
               // Clear channel (endpoint) Tx (always includes Rx too!)
               UdprawCancelTx(ucUnit, ucChannel);
               
               // Mailbox(es)
               OS_MBX_INIT_EX(MbxUdprawTxChannel[ucUnit][ucChannel]);
               
               // Semaphore(s)
               os_sem_init(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemRx, 0);
               os_sem_init(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTxComp, 0);
               os_sem_init(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemMsg, 1);

               // Assign given semaphore and mask for new messages
               SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].oiSemMsgNew= oiSemMsgNew;
               SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ulMsgMask= ulMsgMask;
            
               // Task(s)
               SUdprawTskChannel[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucUnit= ucUnit;
               SUdprawTskChannel[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucChannel= ucChannel;

               // Grant access to internal objects to consumer/client by OS_ID
               // Spawn tasks

               // Type rx/host-out or rx/host-out & tx/host-in (control)
               if (UDPRAW_IS_EPTYPE_OUT(usType) || UDPRAW_IS_EPTYPE_BOTH(usType))
               {
                  *poiSemRx= (&(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemRx));
                  SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].tidTskRx= OS_TSK_CREATE_USER_EX(TskUdprawRxChannel, SUdprawChannelRO[ucUnit][ucChannel].u8TskPriRx, StkTskUdprawRxChannel[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx], &SUdprawTskChannel[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx]);
               }
               // Type tx/host-in or rx/host-out & tx/host-in (control)
               else if (UDPRAW_IS_EPTYPE_IN(usType) || UDPRAW_IS_EPTYPE_BOTH(usType))
               {
                  *poiMbxTxQ= (&(MbxUdprawTxChannel[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx]));
                  SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].tidTskTx= OS_TSK_CREATE_USER_EX(TskUdprawTxChannel, SUdprawChannelRO[ucUnit][ucChannel].u8TskPriTx, StkTskUdprawTxChannel[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx], &SUdprawTskChannel[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx]);
               }
               
               //-IRQ enable for specified channel UDP_CSR[0...5]) and (UDP_IER, 5...0)
               //-UdprawSafeWriteCSR(ucUnit, ucIndex, SUdprawChannelRO[ucUnit][ucIndex].u16CSR, TRUE);
               //-AT91F_UDP_Enable(SSpiRO[ucUnit].pBase, (1 << ucChannel));
               //
               // IRQ enable mask setting triggered by irq AT91C_UDP_ENDBUSRES in UdprawHandler
               
               // Verify success
               if (SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].tidTskRx && SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].tidTskTx)
               {
                  ucResult= 1;
               }

               // Update channel flags
               SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags= (ucResult) ? (UDPRAW_UNIT_FLAGS_CONFIGDONE) : (0);
            }
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR UdprawCancelRx
//----------------------------------------------------------------------------
UCHAR UdprawCancelRx(UCHAR ucUnit, UCHAR ucChannel)
{
   UCHAR ucResult= 0;
   
   USHORT usMask;
   
   if (ucUnit < UDPRAW_UNIT_MAX)
   {
      if (ucChannel < UDPRAW_CHANNELID_MAX)
      {
         usMask= (1 << ucChannel);
         
         // Initial state must be zero
         if (SUdprawRO[ucUnit].pBase->UDP_RSTEP & usMask)
         {
            // Force zero
            SUdprawRO[ucUnit].pBase->UDP_RSTEP &= ~usMask;
         }
         
         // Initial state must be zero
         if (!(SUdprawRO[ucUnit].pBase->UDP_RSTEP & usMask))
         {
            // First action is one
            SUdprawRO[ucUnit].pBase->UDP_RSTEP |= usMask;

            // Second state must be one
            if (SUdprawRO[ucUnit].pBase->UDP_RSTEP & usMask)
            {
               // Final action is zero again
               SUdprawRO[ucUnit].pBase->UDP_RSTEP &= ~usMask;
               
               // Final state must be zero again
               if (!(SUdprawRO[ucUnit].pBase->UDP_RSTEP & usMask))
               {
                  SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u8RxBkFirst= 0;
                  ucResult= 1;
               }
            }
         }
      }
   }

   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR UdprawCancelTx
//----------------------------------------------------------------------------
UCHAR UdprawCancelTx(UCHAR ucUnit, UCHAR ucChannel)
{
   UCHAR ucResult= 0;
   
   UCHAR ucDualBank;

   if (ucUnit < UDPRAW_UNIT_MAX)
   {
      if (ucChannel < UDPRAW_CHANNELID_MAX)
      {
         // AT91C_UDP_TXPKTRDY is set
         if (SUdprawRO[ucUnit].pBase->UDP_CSR[ucChannel] & AT91C_UDP_TXPKTRDY)
         {
            // Clear AT91C_UDP_TXPKTRDY manually yet
            UdprawSafeWriteCSR(ucUnit, ucChannel, AT91C_UDP_TXPKTRDY, FALSE);

            // Determine dual bank mode
            ucDualBank= UDPRAW_IS_DPR(ucUnit, ucChannel);
            
            // Dual-bank support
            if (ucDualBank)
            {
               // Set AT91C_UDP_TXPKTRDY manually yet
               UdprawSafeWriteCSR(ucUnit, ucChannel, AT91C_UDP_TXPKTRDY, TRUE);
               
               // Clear AT91C_UDP_TXPKTRDY manually yet
               UdprawSafeWriteCSR(ucUnit, ucChannel, AT91C_UDP_TXPKTRDY, FALSE);
            }
         }
         
         // Reset the channels (endpoints) fifo
         ucResult= UdprawCancelRx(ucUnit, ucChannel);
      }
   }

   return (ucResult);
}

//----------------------------------------------------------------------------
// USHORT UdprawMsg
//----------------------------------------------------------------------------
USHORT UdprawMsg(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   USHORT usCounter;
   UCHAR ucIndex;
      
   if (ucUnit < UDPRAW_UNIT_MAX)
   {
      RESCHEDULE_TICKS(0);
      
      // Lock counters
      if (OS_SEM_TAKE_CHK(os_sem_take(SUdprawRW[ucUnit].SemMsg, u16Timeout)))
      {    
         for (ucIndex= 0; ucIndex < UDPRAW_UNIT_MSGMAX; ucIndex++)
         {
            if ((1 << ucIndex) & ulMsg)
            {
               usCounter= SUdprawRW[ucUnit].xMsgCounter[ucIndex];
               if (usCounter)
               {
                  RAISEMAX(usResult, usCounter, USHRT_MAX);
   
                  if (usDecrement) 
                  {
                     if (usDecrement < usCounter)
                     {
                        SUdprawRW[ucUnit].xMsgCounter[ucIndex] -= usDecrement;
                     }
                     else
                     {   
                        SUdprawRW[ucUnit].xMsgCounter[ucIndex]= 0;
                     }
                  }
               }
            }
         }

         // Unlock counters
         os_sem_give(SUdprawRW[ucUnit].SemMsg);   
      }
   }
         
   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT UdprawMsgChannel
//----------------------------------------------------------------------------
USHORT UdprawMsgChannel(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   USHORT usCounter;
   UCHAR ucIndex;
      
   if (ucUnit < UDPRAW_UNIT_MAX)
   {
      if (ucChannel < UDPRAW_CHANNELID_MAX)
      {
         // Lock counters
         if (OS_SEM_TAKE_CHK(os_sem_take(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemMsg, u16Timeout)))
         {    
            for (ucIndex= 0; ucIndex < UDPRAW_CHANNEL_MSGMAX; ucIndex++)
            {
               if ((1 << ucIndex) & ulMsg)
               {
                  usCounter= SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].xMsgCounter[ucIndex];
                  if (usCounter)
                  {
                     RAISEMAX(usResult, usCounter, USHRT_MAX);
   
                     if (usDecrement)
                     {
                        if (usDecrement < usCounter)
                        {
                           SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].xMsgCounter[ucIndex] -= usDecrement;
                        }
                        else
                        {
                           SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].xMsgCounter[ucIndex]= 0;
                        }
                     }
                  }
               }
            }

            // Unlock counters
            os_sem_give(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemMsg);
         }
      }
   }

   return (usResult);
}

//----------------------------------------------------------------------------
// __task void TskUdprawMsg
//----------------------------------------------------------------------------
__task void TskUdprawMsg(void *pvParam)
{
   struct _SUdprawTsk *pSUdprawTsk= (struct _SUdprawTsk *)pvParam;
   
   ULONG ulValue;
   USHORT usCount;
   UCHAR ucIndex;
 
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxUdprawMsg[pSUdprawTsk->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         usCount= MSG_GETCNT(UDPRAW_UNIT_MSG_MSGBITS, ulValue);
         if (usCount)
         {
            // Lock unit counters
            if (OS_SEM_TAKE_CHK(os_sem_take(SUdprawRW[pSUdprawTsk->ucUnit].SemMsg, OS_T_FOREVER)))
            {
               // Check message bits
               for (ucIndex= 0; ucIndex < UDPRAW_UNIT_MSGMAX; ucIndex++)
               {
                  if (ulValue & (1 << ucIndex))
                  {
                     // Count until max-1 and set max to show overrun condition
                     RAISEMAX(SUdprawRW[pSUdprawTsk->ucUnit].xMsgCounter[ucIndex], usCount, UDPRAW_MSGCOUNTERMAX);
                  }
               }
            
               // Unlock unit counters
               os_sem_give(SUdprawRW[pSUdprawTsk->ucUnit].SemMsg);
            }

            // New message notification
            if (SUdprawRW[pSUdprawTsk->ucUnit].oiSemMsgNew)
            {
               if (!OS_SEM_TAKE_CHK(os_sem_check(SUdprawRW[pSUdprawTsk->ucUnit].oiSemMsgNew, FALSE)))
               {
                  os_sem_give(SUdprawRW[pSUdprawTsk->ucUnit].oiSemMsgNew);
               }
               UNLOCK;
            }
         }
      }
   }

   DETACH;
}

//----------------------------------------------------------------------------
// __task void TskUdprawMsgChannel
//----------------------------------------------------------------------------
__task void TskUdprawMsgChannel(void *pvParam)
{
   struct _SUdprawTsk *pSUdprawTsk= (struct _SUdprawTsk *) pvParam;
   
   ULONG ulValue;
   ULONG ulCntCh;
   USHORT usCount;
   
   UCHAR ucIndex;
   UCHAR ucChannel;
 
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxUdprawMsgChannel[pSUdprawTsk->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         ulCntCh= MSG_GETCNT(UDPRAW_CHANNEL_MSG_MSGBITS, ulValue);
         ucChannel= UDPRAW_GETMSGCNT_CHANNEL(ulCntCh);
         usCount= UDPRAW_MSGCNT_COUNT(ulCntCh);
                 
         if (usCount)
         {
            if (ucChannel < UDPRAW_CHANNELID_MAX)
            {
               // Lock channel counters
               if (OS_SEM_TAKE_CHK(os_sem_take(SUdprawChannelRW[pSUdprawTsk->ucUnit][SUdprawChannelMapRW[pSUdprawTsk->ucUnit][ucChannel].ucStorageIdx].SemMsg, OS_T_FOREVER)))
               {
                  // Check message bits
                  for (ucIndex= 0; ucIndex < UDPRAW_CHANNEL_MSGMAX; ucIndex++)
                  {
                     if (ulValue & (1 << ucIndex))
                     {
                        // Count until max-1 and set max to show overrun condition
                        RAISEMAX(SUdprawChannelRW[pSUdprawTsk->ucUnit][SUdprawChannelMapRW[pSUdprawTsk->ucUnit][ucChannel].ucStorageIdx].xMsgCounter[ucIndex], usCount, UDPRAW_MSGCOUNTERMAX);
                     }
                  }
                  
                  // Unlock channel counters
                  os_sem_give(SUdprawChannelRW[pSUdprawTsk->ucUnit][SUdprawChannelMapRW[pSUdprawTsk->ucUnit][ucChannel].ucStorageIdx].SemMsg);
               }
                           
               // New message notification (but don't increment the semaphore above one)
               if (SUdprawChannelRW[pSUdprawTsk->ucUnit][SUdprawChannelMapRW[pSUdprawTsk->ucUnit][ucChannel].ucStorageIdx].oiSemMsgNew)
               {
                  if (!OS_SEM_TAKE_CHK(os_sem_check(SUdprawChannelRW[pSUdprawTsk->ucUnit][SUdprawChannelMapRW[pSUdprawTsk->ucUnit][ucChannel].ucStorageIdx].oiSemMsgNew, FALSE)))
                  {
                     os_sem_give(SUdprawChannelRW[pSUdprawTsk->ucUnit][SUdprawChannelMapRW[pSUdprawTsk->ucUnit][ucChannel].ucStorageIdx].oiSemMsgNew);
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
// __task void TskUdprawRxChannel
//----------------------------------------------------------------------------
__task void TskUdprawRxChannel(void *pvParam)
{
   struct _SUdprawTskChannel *pSUdprawTskChannel= (struct _SUdprawTskChannel *) pvParam;
   
   U8 u8RxBkAck;
   
   FOREVER
   {
      // Pend for completion of consumer/client to allow re-use of the single software buffer
      if (OS_SEM_TAKE_CHK(os_sem_take(SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].SemRx, OS_T_FOREVER)))
      {         
         // Debug (when this happens, something is totally wrong and dual software buffer support is required then too)
         if ((SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].u8RxBkFirst & AT91C_UDP_RX_DATA_BKX) == AT91C_UDP_RX_DATA_BKX)
         {
            // Dummy
            SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].u8RxBkFirst= SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].u8RxBkFirst;
         }

         // Acknowledge handled/transferred bank/fifo to re-arm one rx hardware buffer (which will be copied after rx to the unlocked software buffer)
         // For DualPortRAM channels (endpoints) the 2nd hardware buffer may still receive while the 1st hardware buffer isn't re-armed (but can raise IRQ after re-arm only!?)
         
         // Safe update of local copy
         LOCKED(u8RxBkAck= SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].u8RxBkFirst);
         
         // Acknowledge specified dual port ram bank
         UdprawSafeWriteCSR(pSUdprawTskChannel->ucUnit, pSUdprawTskChannel->ucChannel, u8RxBkAck, FALSE);
         
         // Safe update of global var
         LOCKED(SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].u8RxBkAck= u8RxBkAck);
      }
   }
   
   DETACH;
}   

//----------------------------------------------------------------------------
// __task void TskUdprawTxChannel
//----------------------------------------------------------------------------
__task void TskUdprawTxChannel(void *pvParam)
{
   struct _SUdprawTskChannel *pSUdprawTskChannel= (struct _SUdprawTskChannel *) pvParam;
   
   ULONG ulValue;
   ULONG ulCntCh;
   ULONG ulMsg;
   USHORT usCount;
   USHORT usBytes;
   USHORT usTimeout;
   UCHAR ucChannel;
   UCHAR ucDualBank;
   
   UCHAR ucResult= 0;
   
   FOREVER
   {
      // Pend for new data to send from consumer/client
      if (OS_MBX_VALUE_CHK(os_mbx_wait(MbxUdprawTxChannel[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         // Verify channel & message
         ulCntCh= MSG_GETCNT(UDPRAW_CHANNEL_MSG_MSGBITS, ulValue);
         ucChannel= UDPRAW_GETMSGCNT_CHANNEL(ulCntCh);
         usCount= UDPRAW_MSGCNT_COUNT(ulCntCh);

         if (usCount)
         {
            if (ucChannel == (pSUdprawTskChannel->ucChannel))
            {
               // Verify valid consumer/client
               if ((SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].pucTx) && (SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].oiMbxTxQ))
               {
                  // Unpack the message data
                  usBytes= UDPRAW_GETMSGTX_BYTES(UDPRAW_CHANNEL_DATA_MSGBITS, ulValue);
                  usTimeout= UDPRAW_GETMSGTX_TIMEOUT(UDPRAW_CHANNEL_DATA_MSGBITS, ulValue);
                  //-bNextDir= UDPRAW_GETMSGTX_NEXTDIR(UDPRAW_CHANNEL_DATA_MSGBITS, ulValue);
                  
                  // Determine dual bank mode
                  ucDualBank= UDPRAW_IS_DPR(pSUdprawTskChannel->ucUnit, pSUdprawTskChannel->ucChannel);
                
                  // AT91C_UDP_TXPKTRDY must be cleared to allow access of 1st bank only!
                  // After the 1st bank is filled the 2nd bank can be accessed immediately (for dual bank channels/endpoints only)
                  // When both banks are filled we've to pend until the 1st bank is free again
                  ucResult= 1;
                  if ((!ucDualBank) || (!SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].ucTxBankQueue) || \
                     ((ucDualBank) && (SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].ucTxBankQueue) >= 2))
                  {
                     // Check for pending bank
                     if (SUdprawRO[pSUdprawTskChannel->ucUnit].pBase->UDP_CSR[pSUdprawTskChannel->ucChannel] & AT91C_UDP_TXPKTRDY)
                     {
                        // Pend until 1st bank is available again (SemTxComp is available after all pending banks are sent)
                        ucResult= OS_SEM_TAKE_CHK(os_sem_take(SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].SemTxComp, usTimeout));
                        if (ucResult)
                        {
                           // Check for bank done successfully
                           ucResult= ((SUdprawRO[pSUdprawTskChannel->ucUnit].pBase->UDP_CSR[pSUdprawTskChannel->ucChannel] & AT91C_UDP_TXPKTRDY) != 0);
                        }
                     }
                  }

                  // Access bank/fifo
                  if (ucResult)
                  {
                     // Transfer data for non-zero length packet only
                     if (usBytes)
                     {
                        // Transfer user memory to bank/fifo
                        for (usCount= 0; usCount < usBytes; usCount++)
                        {
                           SUdprawRO[pSUdprawTskChannel->ucUnit].pBase->UDP_FDR[pSUdprawTskChannel->ucChannel]= *(SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].pucTx+ usCount);
                        }
                     }
                        
                     // Increase bank queue counter
                     RAISEMAX(SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].ucTxBankQueue, 1, UCHAR_MAX);
                     //-SUdprawRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].ucTxBankQueue++;
                     
                     // Arm data tx (host-in) IMMEDIATELY for the 1st filled bank only
                     // The 2nd bank must be armed by interrupt TXCOMP only for the 1st bank completion
                     if ((!ucDualBank) || (!SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].ucTxBankQueue))
                     {
                        // Arm data tx (host-in)
                        UdprawSafeWriteCSR(pSUdprawTskChannel->ucUnit, pSUdprawTskChannel->ucChannel, AT91C_UDP_TXPKTRDY, TRUE);
                     }
                     
                     // Success message is sent by UdprawIrqHandlerChannel for AT91C_UDP_TXCOMP
                  }
                  else
                  {
                     // Timeout message
                     ulMsg= UDPRAW_TXRES_ERR;
                     os_mbx_send_safe(SUdprawChannelRW[pSUdprawTskChannel->ucUnit][SUdprawChannelMapRW[pSUdprawTskChannel->ucUnit][pSUdprawTskChannel->ucChannel].ucStorageIdx].oiMbxTxQ, OS_MBX_VALUE_PUT(MSG_ADDCNT(UDPRAW_CHANNEL_DATA_MSGBITS, ulMsg, UDPRAW_SETMSGCNT_CHANNEL(pSUdprawTskChannel->ucChannel) | UDPRAW_MSGCNT_COUNT(1))), OS_T_FOREVER);
                     RESCHEDULE_TICKS(0);
                  }
               }
            }
         }
      }
   }
   
   DETACH;
}

//----------------------------------------------------------------------------
// bool UdprawSafeWriteCSR
//----------------------------------------------------------------------------
bool UdprawSafeWriteCSR(UCHAR ucUnit, UCHAR ucChannel, U32 u32Mask, bool bSet)
{
   bool bResult= FALSE;
   
   if (ucUnit < UDPRAW_UNIT_MAX)
   {
      if (ucChannel < UDPRAW_CHANNEL_MAX)
      {
         if (bSet)
         {
            LOCK;
            {
               SUdprawRO[ucUnit].pBase->UDP_CSR[ucChannel] |= u32Mask;
               while((SUdprawRO[ucUnit].pBase->UDP_CSR[ucChannel] & u32Mask) != u32Mask);
            }
            UNLOCK;
         }
         else
         {
            LOCK;
            {
               SUdprawRO[ucUnit].pBase->UDP_CSR[ucChannel] &= ~u32Mask;
               while((SUdprawRO[ucUnit].pBase->UDP_CSR[ucChannel] & u32Mask) == u32Mask);
            }
            UNLOCK;
         }
         
         bResult= TRUE;
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// void UdprawIrqHandler
//----------------------------------------------------------------------------
void UdprawIrqHandler(UCHAR ucUnit)
{
   register ULONG ulUdprawIrq;
   register ULONG ulUdprawMsg;
   register UCHAR ucChannels;
   register UCHAR ucImrChannels;
   register UCHAR ucIndex;
   
   // Copy the IRQ source to handle yet (AT91C_UDP_ENDBUSRES is always enabled and cannot be masked)
   ulUdprawIrq= ((SUdprawRO[ucUnit].pBase->UDP_IMR | AT91C_UDP_ENDBUSRES) & SUdprawRO[ucUnit].pBase->UDP_ISR);
     
   // Handle unit IRQ sources
   
   // Special AT91C_UDP_RXSUSP (suspend)
   // No USB activity for 3ms, assume host disconnection/power-down (critical when pull-downs are missing on DP and DM!)
   if (ulUdprawIrq & AT91C_UDP_RXSUSP)
   {
      // Disable transmitter
      SUdprawRO[ucUnit].pBase->UDP_TXVC |= AT91C_UDP_TXVDIS;
   }
   
   // Special AT91C_UDP_RXRSM (resume), AT91C_UDP_ENDBUSRES and AT91C_UDP_WAKEUP
   if ((ulUdprawIrq & AT91C_UDP_RXRSM) || \
      (ulUdprawIrq & AT91C_UDP_ENDBUSRES) || \
      (ulUdprawIrq & AT91C_UDP_WAKEUP))
   {
      // Enable transmitter
      SUdprawRO[ucUnit].pBase->UDP_TXVC &= ~AT91C_UDP_TXVDIS;
   }
   
   // Special AT91C_UDP_ENDBUSRES
   if (ulUdprawIrq & AT91C_UDP_ENDBUSRES)
   {
      // Warning: Each time the (non maskable) AT91C_UDP_ENDBUSRES interrupt is triggered, the Interrupt Mask Registers UDP_IMR and UDP_CSR registers have been reset!
      // refer to AT91SAM7X manual 34.5.3.3 page 464 (from Powered State to Default State)
      // THIS is the ideal trigger for safe initialization of the Interrupt Mask Registers UDP_IMR and UDP_CSR registers!
      // USB state definition here is DEFAULT (and was POWERED before)
     
      for (ucImrChannels= 0, ucIndex= 0; ucIndex < UDPRAW_CHANNEL_MAX; ucIndex++)
      {
         if (SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucIndex].ucStorageIdx].ucFlags & UDPRAW_UNIT_FLAGS_CONFIGDONE)
         {
            // Build channel mask for IMR
            ucImrChannels |= (1 << ucIndex);
            
            // Optional!? Cancel channel (endpoint) Tx (always includes the required - refer to AT91SAM7X manual 34.6.9 page 477 - Rx too!)
            UdprawCancelTx(ucUnit, ucIndex);
            
            // Retrieve each configured UDP_CSR[x] from its virtual software register(s) and activate it by setting AT91C_UDP_EPEDS
            UdprawSafeWriteCSR(ucUnit, ucIndex, (AT91C_UDP_EPEDS | SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucIndex].ucStorageIdx].usType | SUdprawChannelRO[ucUnit][ucIndex].u16CSR), TRUE);
         }
      }

      // Retrieve UDP_IMR from virtual software register but activate irq for configured channels (endpoints) only
      SUdprawRO[ucUnit].pBase->UDP_IMR= (SUdprawRO[ucUnit].u32Irq | ucImrChannels);
   }

   // Call channel (endpoint) IRQ handler for any pending channel (endpoint) IRQ
   ucChannels= (ulUdprawIrq & AT91C_UDP_EPINTX);
   if (ucChannels)
   {
      UdprawIrqHandlerChannel(ucUnit, ucChannels);
   }

   // Record all other yet unhandled IRQ sources and add SOFINT details FRM_OK/FRM_ERR of UDP_FRM_NUM at unused 15...14
   ulUdprawMsg= ulUdprawIrq | ((SUdprawRO[ucUnit].pBase->UDP_NUM & (AT91C_UDP_FRM_OK | AT91C_UDP_FRM_ERR)) << 2);
   
   // Apply message filter
   ulUdprawMsg&= SUdprawRW[ucUnit].ulMsgMask;
   
   // Apply max counter index available
   ulUdprawMsg&= ((1 << (UDPRAW_UNIT_MSGMAX+ 1))- 1);
   
   if (ulUdprawMsg)
   {
      if (isr_mbx_check(&MbxUdprawMsg[ucUnit]) > 0)
      {
         isr_mbx_send(&MbxUdprawMsg[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(UDPRAW_UNIT_MSG_MSGBITS, ucUnit, 1)));
      }
   }
   
   // UDP needs EXPLICIT clearing of unit interrupts
   SUdprawRO[ucUnit].pBase->UDP_ICR= ulUdprawMsg;

   // ARM needs EXPLICIT acknowledge (qualifier __irq doesn't do this) for (E)nd (O)f (I)RQ!!
   EOI;
}

//----------------------------------------------------------------------------
// void UdprawIrqHandlerChannel
//----------------------------------------------------------------------------
void UdprawIrqHandlerChannel(UCHAR ucUnit, UCHAR ucChannels)
{
   register ULONG ulUdprawCsr;
   register ULONG ulUdprawIrq;
   register ULONG ulUdprawIrq2;
   register ULONG ulUdprawMsg;
   
   register USHORT usCount;
   register USHORT usByteCount;
   
   register UCHAR ucBit;
   register UCHAR ucChannel;
   register UCHAR ucDualBank;
   
   ULONG ulMsg;

   // Query active channel(s)
   for (ucChannel= 0; ucChannel < UDPRAW_CHANNEL_MAX ; ucChannel++)
   {
      ucBit= (1 << ucChannel);
      if (ucChannels & ucBit)
      {
         // Get CSR contents
         ulUdprawCsr= (SUdprawRO[ucUnit].pBase->UDP_CSR[ucChannel]);
         
         // Mask out all non-interrupt bits (UDP_CSR is a mixed register!)
         ulUdprawIrq= (ulUdprawCsr & AT91C_UDP_CSR_IRQS);
         
         // Determine dual bank mode
         ucDualBank= UDPRAW_IS_DPR(ucUnit, ucChannel);
            
         // Handle channel IRQ sources

         // Mask only rx data bank/fifo interrupt bits
         ulUdprawIrq2= (ulUdprawIrq & AT91C_UDP_RX_DATA_BKX);

         // RX (host-out) data (RX_DATA_BK0, RX_DATA_BK1) acknowledge queuing (done by TskUdprawRxChannel)
         if (ulUdprawIrq2)
         {           
            // Debug (when this happens, something is totally wrong and dual software buffer support is required then too)
            if ((ulUdprawIrq2 & AT91C_UDP_RX_DATA_BKX) == AT91C_UDP_RX_DATA_BKX)
            {
               // Dummy
               ulUdprawIrq2= ulUdprawIrq2;
            }

            // Clear already acknowledged first bank
            SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u8RxBkFirst &= ~SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u8RxBkAck;
            
            // Set bank to acknowledge first for already acknowledged bank (with support of single & dual bank mode)
            if (!SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u8RxBkFirst)
            {
               if ((!ucDualBank) || (!SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u8RxBkQueue))
               {
                  SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u8RxBkFirst= ulUdprawIrq2;
               }
               else
               {
                  SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u8RxBkFirst= SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u8RxBkQueue;
                  SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u8RxBkQueue= ulUdprawIrq2;
               }
            }
         }        
         
         // RX (host-out) data (and special) transfer
         if ((ulUdprawIrq2) || (ulUdprawIrq & AT91C_UDP_RXSETUP))
         {
            // Transfer 1st pending bank/fifo to user memory
            usByteCount= UDPRAW_GET_RXBYTECNT(ulUdprawCsr);

            // Verify valid consumer/client
            if ((SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].pucRx) && (SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].oiMbxRxQ))
            {
               // Transfer data for non-zero length packet only
               if (usByteCount)
               {
                  // Transfer bank/fifo to user memory
                  for (usCount= 0; usCount < usByteCount; usCount++)
                  {
                     *(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].pucRx+ usCount)= SUdprawRO[ucUnit].pBase->UDP_FDR[ucChannel];
                  }
               }

               // Add reduced u8RxBkFirst (0= NONE, RX_DATA_BK0, RX_DATA_BK1) to ulUdprawCsr at unused bit 27 (0= BK0/SETUP, 1= BK1) for optional dual software buffer support
               ulUdprawCsr |= (SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u8RxBkFirst == AT91C_UDP_RX_DATA_BK1) ? (UDPRAW_IMR_RX_DATA_BKX) : (0);
               
               // Notify consumer/client by sending UDP_CSR[ucChannel] (contains also byte count and ucChannel) to attached mailbox
               isr_mbx_send(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].oiMbxRxQ, OS_MBX_VALUE_PUT(MSG_ADDCNT(UDPRAW_CHANNEL_DATA_MSGBITS, ulUdprawCsr, UDPRAW_SETMSGCNT_CHANNEL(ucChannel) | UDPRAW_MSGCNT_COUNT(1))));
            }
         }
                  
         // Special RX (host-out) control for AT91C_UDP_RXSETUP
         if (ulUdprawIrq & AT91C_UDP_RXSETUP)
         {
            // Set AT91C_UDP_DIR manually yet before clear of IRQs at the of this function (refer to AT91SAM7X manual 34.6.10 page 481)
            UdprawSafeWriteCSR(ucUnit, ucChannel, AT91C_UDP_DIR, TRUE);
         }
         
         // TX (host-in) data (AT91C_UDP_TXCOMP)
         if (ulUdprawIrq & AT91C_UDP_TXCOMP)
         {
            // Clear AT91C_UDP_TXCOMP manually yet
            UdprawSafeWriteCSR(ucUnit, ucChannel, AT91C_UDP_TXCOMP, FALSE);

            // Decrease bank queue counter
            //-LOWERMIN(SUdprawRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucTxBankQueue, 1, 0);
            if (SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucTxBankQueue)
            {
               SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucTxBankQueue--;
            }            

            // Dual bank handling
            if (ucDualBank)
            {
               // Raise AT91C_UDP_TXPKTRDY for already loaded 2nd bank to re-arm tx immediately
               if (SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucTxBankQueue)
               {
                  UdprawSafeWriteCSR(ucUnit, ucChannel, AT91C_UDP_TXPKTRDY, TRUE);
               }
            }
            
            // Trigger pending tx (for dual bank support after BOTH banks done!!)
            if (!(ucDualBank) || !(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucTxBankQueue))
            {
               if (!ISR_SEM_TAKE_CHK(isr_sem_check(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTxComp)))
               {
                  isr_sem_give(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTxComp);
               }
            }
            
            // Success message to consumer/client      
            ulMsg= UDPRAW_TXRES_OK;
            if (isr_mbx_check(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].oiMbxTxQ) > 0)
            {
               isr_mbx_send(SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].oiMbxTxQ, OS_MBX_VALUE_PUT(MSG_ADDCNT(UDPRAW_CHANNEL_DATA_MSGBITS, ulMsg, UDPRAW_SETMSGCNT_CHANNEL(ucChannel) | UDPRAW_MSGCNT_COUNT(1))));
            }
         }
         
         // Special TX (host-in) special (AT91C_UDP_STALLSENT/AT91C_UDP_ISOERROR)
         // Clear AT91C_UDP_STALLSENT manually
         // Done by final clear channel interrupts at end of this function
                 
         // Record all other yet unhandled IRQ sources (includes communication errors)
         ulUdprawMsg= ulUdprawIrq;
         
         // Apply message filter
         ulUdprawMsg&= SUdprawChannelRW[ucUnit][SUdprawChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ulMsgMask;
         
         // Apply max counter index available
         ulUdprawMsg&= ((1 << (UDPRAW_CHANNEL_MSGMAX+ 1))- 1);
         
         if (ulUdprawMsg)
         {
            if (isr_mbx_check(&MbxUdprawMsgChannel[ucUnit]) > 0)
            {
               isr_mbx_send(&MbxUdprawMsgChannel[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(UDPRAW_CHANNEL_MSG_MSGBITS, ulUdprawMsg, UDPRAW_SETMSGCNT_CHANNEL(ucChannel) | UDPRAW_MSGCNT_COUNT(1))));
            }
         }
         
         // Clear channel interrupts (except individually handled RX_DATA_BK0 and RX_DATA_BK1)
         UdprawSafeWriteCSR(ucUnit, ucChannel, (ulUdprawMsg & ~AT91C_UDP_RX_DATA_BKX), FALSE);
      }
   }
}
