//!	extended function support for RTL
/**
		\file		support.c
*/
//----------------------------------------------------------------------------
// R T L   K E R N E L   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/RTL/SUPPORT.C
// Purpose: RTL KERNEL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-01-08 initial version (imported from support.h)
// 2009-09-02 code cleanup for memcpyflex
// 2010-02-11 added isr_mbx_write (immediate update version of isr_mbx_send)
// 2010-02-11 added safety to isr_mbx_write by locking ALL irqs until complete
// 2010-02-12 removed "safety" to isr_mbx_write by locking ALL irqs until done
// 2010-04-23 added section constants; added ucBitMask[]
// 2010-08-12 added no-source feature to memcpyflex to just clear destination
// 2010-10-27 added os_sem_check (non-destructive version of os_sem_wait)
// 2011-02-24 enhanced os_sem_check to sem_check & added wrappers for os_/isr_
// 2011-09-15 isr_mbx_read/write always clear any attached task now
// 2011-09-16 added new functiona os_mbx_clear and os_mbx_read/write
// 2011-09-28 added new functions isr/os_mbx_count (ported from RTL ...check)
// 2011-09-28 updated struct OS_MCB to match renamed items of RL-ARM 4.13a
// 2011-09-28 speeded up os_mbx_clear by checking mbx first using os_mbx_count
// 2011-11-24 added public doxygen documentation
// 2015-10-22 added forwarding to memcpy & memset in memcpyflex for custom types
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#include <AT91SAM7X512.h>
#include "support.h"

// Constants

//! Bit mask array (8 bit)
UCHAR ucBitMask[8]= { 1, 2, 4, 8, 16, 32, 64, 128 };

// CAUTION when updating the RTL-ARM library!!!
// Definition taken from \Programme\Keil\ARM\RL\RTX\SRC\ARM\RT_TYPEDEF.H (isr_mbx_write)
typedef struct OS_MCB {
  U8     cb_type;                 /* Control Block Type                      */
  U8     isr_st;                  /* State flag variable for isr functions   */
  struct OS_TCB *p_lnk;           /* Chain of tasks waiting for message      */
  U16    first;                   /* Index of the message list begin         */
  U16    last;                    /* Index of the message list end           */
  U16    count;                   /* Actual number of stored messages        */
  U16    size;                    /* Maximum number of stored messages       */
  void   *msg[1];                 /* FIFO for Message pointers 1st element   */
} *P_MCB;

// Definition taken from \Programme\Keil\ARM\RL\RTX\SRC\ARM\RT_TYPEDEF.H (os_sem_wait)
typedef struct OS_SCB {
   U8     cb_type;                /* Control Block Type                      */
   U16    tokens;                 /* Semaphore tokens                        */
   struct OS_TCB *p_lnk;          /* Chain of tasks waiting for tokens       */
} *P_SCB;

// Utility

//----------------------------------------------------------------------------
// (os/isr_)sem_check: non-destructive version of os_sem_wait
//----------------------------------------------------------------------------
OS_RESULT os_sem_check (OS_ID semaphore, bool bUnlock)
{
   return (sem_check(semaphore, bUnlock, FALSE));
}

OS_RESULT isr_sem_check (OS_ID semaphore)
{
   return (sem_check(semaphore, FALSE, TRUE));
}

//----------------------------------------------------------------------------
// code taken from RT_SEMAPHORE.C os_sem_wait
//----------------------------------------------------------------------------
OS_RESULT sem_check (OS_ID semaphore, bool bUnlock, bool bIsr)
{
   OS_RESULT oiResult= OS_R_TMO;
   P_SCB p_SCB= semaphore;

   /* Check for available token */
   if (semaphore)
   {
      if (!bIsr) tsk_lock();
      if (p_SCB->tokens)
      {
         if (bUnlock && !bIsr)
         {
            tsk_unlock();
         }
         
         oiResult= OS_R_OK;
      }
   }
   
   return (oiResult);
}

//----------------------------------------------------------------------------
// os_mbx_write: immediate update version of os_mbx_send (w/o scheduler runs)
//----------------------------------------------------------------------------
// valid destination of data are only non-pending tasks and ISRs
//----------------------------------------------------------------------------
OS_RESULT os_mbx_write(OS_ID mailbox, void *p_msg, U16 u16Timeout)
{
   OS_RESULT orResult= OS_R_OK;
   
   if (mailbox)
   {
      LOCK;
      {
         orResult= (isr_mbx_check (mailbox) > 0) ? (OS_R_MBX) : (OS_R_OK);
         isr_mbx_write (mailbox, p_msg);
      }
      UNLOCK;
   }
   
   return (orResult);
}

//----------------------------------------------------------------------------
// os_mbx_read: immediate update version of os_mbx_send (w/o scheduler runs)
//----------------------------------------------------------------------------
// valid destination of data are only non-pending tasks and ISRs
//----------------------------------------------------------------------------
OS_RESULT os_mbx_read(OS_ID mailbox, void *p_msg, U16 u16Timeout)
{
   OS_RESULT orResult= OS_R_TMO;
   
   if (mailbox)
   {
      LOCK;
      {
         isr_mbx_read (mailbox, p_msg);
      }
      UNLOCK;
   }
   
   return (orResult);
}

//----------------------------------------------------------------------------
// isr_mbx_write: immediate update version of isr_mbx_send (w/o scheduler runs)
//----------------------------------------------------------------------------
// code taken from RT_MAILBOX.C os_mbx_psh (No task is waiting) WITHOUT _incw
//----------------------------------------------------------------------------
void isr_mbx_write (OS_ID mailbox, void *p_msg)
{
   P_MCB p_CB= mailbox;

   if (p_CB->isr_st)
   {
      // A task is waiting to send message, remove it from the waiting list
      p_CB->isr_st= 0;
   }
  
   // Store the message to the mailbox queue
   if (p_CB->count < p_CB->size)
   {
      p_CB->msg[p_CB->first]= p_msg;
      p_CB->count++;
      
      if (++p_CB->first == p_CB->size)
      {
         p_CB->first= 0;
      }
   }
}

//----------------------------------------------------------------------------
// isr_mbx_read: task free version of isr_mbx_receive (w/o scheduler runs)
//----------------------------------------------------------------------------
// code taken from RT_MAILBOX.C isr_mbx_receive WITHOUT task enqueue
//----------------------------------------------------------------------------
OS_RESULT isr_mbx_read (OS_ID mailbox, void **message)
{
   P_MCB p_MCB= mailbox;
   
   if (p_MCB->count)
   {
      // A message is available in the fifo buffer
      *message = p_MCB->msg[p_MCB->last];
      if (p_MCB->isr_st)
      {
         // A task is waiting to send message, remove it from the waiting list
         p_MCB->isr_st= 0;
      }
      
      p_MCB->count--;
      
      if (++p_MCB->last == p_MCB->size)
      {
         p_MCB->last= 0;
      }
      
      return (OS_R_MBX);
   }
      
   return (OS_R_OK);
}

//----------------------------------------------------------------------------
// os_mbx_send_safe: Priority safe version of os_mbx_send
//----------------------------------------------------------------------------
OS_RESULT os_mbx_send_safe (OS_ID oiMbx, void *pvMbx, U16 u16Timeout)
{
   OS_RESULT orResult;

   if (u16Timeout == OS_T_NONE)
   {
      orResult= os_mbx_send (oiMbx, pvMbx, u16Timeout);
   }   
   else
   {
      if (os_mbx_check (oiMbx) <= 0)
      {
         // This will solve same priority problem when creator and
         // consumer tasks have all the same priority
         os_tsk_pass();
      }
    
      // Critical section start
      tsk_lock();
   
      if (os_mbx_check (oiMbx) > 0)
      {
         orResult= os_mbx_send (oiMbx, pvMbx, u16Timeout);
      }
      else
      {
         orResult= OS_R_TMO;
      }
   
      // Critical section end
      tsk_unlock();
   }
       
   return (orResult);  
}

//----------------------------------------------------------------------------
// os_mbx_count: version of os_mbx_check returningh the item count
//----------------------------------------------------------------------------
// code taken from RT_MAILBOX.C os_mbx_check
//----------------------------------------------------------------------------
OS_RESULT os_mbx_count (OS_ID oiMbx)
{
   /* Check for free space in a mailbox. Returns the number of messages     */
   /* that can be stored to a mailbox. It returns 0 when mailbox is full.   */
   P_MCB p_MCB= oiMbx;
   
   return (p_MCB->count);
}

//----------------------------------------------------------------------------
// isr_mbx_count: version of os_mbx_check returning the item count
//----------------------------------------------------------------------------
// code taken from RT_MAILBOX.C & RTL.h isr_mbx_check
//----------------------------------------------------------------------------
OS_RESULT isr_mbx_count (OS_ID oiMbx)
{
   return (os_mbx_count(oiMbx));
}

//----------------------------------------------------------------------------
// UCHAR os_mbx_clear
//----------------------------------------------------------------------------
USHORT os_mbx_clear (OS_ID oiMbx, U16 u16Timeout)
{
   USHORT usResult= OS_R_OK; // == 0x00
   
   ULONG ulValue;
   
   if (oiMbx)
   {
      if (!os_mbx_count(oiMbx))
      {
         RESCHEDULE(0);
      }
      
      if (os_mbx_count(oiMbx))
      {
         // Wipe all data
         do
         {
            usResult= OS_MBX_VALUE_CHK(os_mbx_wait(oiMbx, OS_MBX_VALUE_GET(ulValue), u16Timeout));
         }
         while (usResult);
      }
   }
   
   // Success for OS_R_OK (== 0x00)
   return (!usResult);
}

//----------------------------------------------------------------------------
// memcpyflex: alternate to memcpy taking care about the element alignments
//----------------------------------------------------------------------------
void * memcpyflex (void *pvDestination, const void *cpvSource, size_t num, size_t size)
{
   void *pvResult;

   CHAR *pcDestination, *pcSource;
   SHORT *psDestination, *psSource;
   LONG *plDestination, *plSource;
   LONGLONG *pllDestination, *pllSource;
   
   size_t count;
   
   count= num;
   pvResult= pvDestination;

   switch (size)
   {
      case 1:
         pcDestination= (CHAR *) pvDestination;
         pcSource= (CHAR *) cpvSource;
         
         if (pcSource)
         {
            while (count--) { *pcDestination++ = *pcSource++; }
         }
         else
         {
            while (count--) { *pcDestination++ = (CHAR)0; }
         }
            
         break;
         
      case 2:
         psDestination= (SHORT *) pvDestination;
         psSource= (SHORT *) cpvSource;
         
         if (psSource)
         {
            while (count--) { *psDestination++ = *psSource++; }
         }
         else
         {
            while (count--) { *psDestination++ = (SHORT)0; }
         }
         
         break;
         
      case 4:
         plDestination= (LONG *) pvDestination;
         plSource= (LONG *) cpvSource;
         
         if (plSource)
         {
            while (count--) {*plDestination++ = *plSource++; }
         }
         else
         {
            while (count--) {*plDestination++ = (LONG)0; }
         }
         
         break;
               
      case 8:
         pllDestination= (LONGLONG *) pvDestination;
         pllSource= (LONGLONG *) cpvSource;
         
         if (pllSource)
         {
            while (count--) {*pllDestination++ = *pllSource++; }
         }
         else
         {
            while (count--) {*pllDestination++ = (LONGLONG)0; }
         }
         
         break;
         
      default:
         if (cpvSource)
         {
            pvResult= memcpy(pvDestination, cpvSource, (num* size));
         }
         else
         {
            pvResult= memset(pvDestination, 0, (num* size));
         }
         break;
   }
   
   return (pvResult);
}

