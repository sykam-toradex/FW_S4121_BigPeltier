//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/EFC/EFC.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-02-28 initial version
// 2013-03-07 fixed EfcBit change function
// 2013-03-07 irq clearing workarround (efc's sr does _not_ clear irqs!!!)
// 2013-03-13 added setup fws and fmcn to EfcBit change function
// 2013-03-21 added page and block determination functions
// 2013-03-28 extended EfcLocate to accept address or absolute page number
// 2013-03-28 added bitmap retrieve functions for GPNVM and locks
// 2013-03-28 fixed EfcInit to ommit non volatile bit configuration (unit 1)
// 2013-04-10 added close flag EFC_CLOSE_LOCKEND and feature to EfcClose
// 2013-04-10 added bInit checking, added *unit & *pagesize to EfcPageOpen...
// 2013-08-22 changed timeout from milliseconds to ticks
// 2013-08-22 changed RESCHEDULE (ms) to RESCHEDULE_TICKS (ticks)
// 2013-10-31 fixed flag checking in EfcLocate
// 2013-11-06 added factor style to param support of EfcBitChange for (un)locking
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

#include "..\mc\mc.h"
#include "efc.h"
#include "efcext.h"
#include "efcdata.h"
#include "efcdev.h"

// Suppress warning for "#1293-D: assignment in condition"
#pragma diag_remark 1293

// Remap "status": 18.4.1 page 99 (write only!!! status detection by (a) read from orgiginal & mirror address to determine SRAM/FLASH or (b) atomic compared write access!!!)

//----------------------------------------------------------------------------
// void IsrEfcx __irq
//----------------------------------------------------------------------------
// Each supported unit requires a function definition here
#if EFC_IRQ > 0
   #if EFC_UNIT_MAX > 0
      EFC_ISR_FUNC(0);
   #endif

   #if EFC_UNIT_MAX > 1
      EFC_ISR_FUNC(1);
   #endif
#endif

//----------------------------------------------------------------------------
// bool EfcInit
//----------------------------------------------------------------------------
bool EfcInit(UCHAR ucUnit)
{
   bool bResult= FALSE;
   bool bGPNVM= FALSE;
   
   if (ucUnit < EFC_UNIT_MAX)
   {
      // Init not complete yet (default)
      SEfcRW[ucUnit].bInit= FALSE;
      
      // Disable irq via non-maskable sysirq (default)
      SEfcRW[ucUnit].bIrq= FALSE;
      
      // Mailbox
      OS_MBX_INIT_EX(MbxEfcMsg[ucUnit]);
      
      // Semaphores
      os_sem_init(SEfcRW[ucUnit].SemNew, 0);
      os_sem_init(SEfcRW[ucUnit].SemRdy, 1);
      os_sem_init(SEfcRW[ucUnit].SemMsg, 1);
      os_sem_init(SEfcRW[ucUnit].SemPgBuff, 1);
      
      // Assign semaphore and mask for new messages
      SEfcRW[ucUnit].oiSemMsgNew= SEfcRW[ucUnit].SemNew;
      SEfcRW[ucUnit].ulMsgMask= EFC_IRQ_MASK;
      
      // Init EFC
      
      // Check unit is ready for operation
      if (EfcIsReady(ucUnit))
      {       
         // Task
         SEfcTsk[ucUnit].ucUnit= ucUnit;
         
         #if EFC_STACK_TSKMSG > 0
            SEfcRW[ucUnit].tidTskMsg= OS_TSK_CREATE_USER_EX(TskEfcMsg, SEfcRO[ucUnit].u8TskPriMsg, StkTskEfc[ucUnit], &SEfcTsk[ucUnit]);
         #else
            SEfcRW[ucUnit].tidTskMsg= os_tsk_create_ex(TskEfcMsg, SEfcRO[ucUnit].u8TskPriMsg, &SEfcTsk[ucUnit]);
         #endif
        
         // Initial setup (GPNVM timing) fws and fmcn (IRQs are enabled later on demand)
         SEfcRO[ucUnit].pEfcBase->EFC_FMR= ((((U32)SEfcRO[ucUnit].u16FWS) & AT91C_MC_FWS) | ((((U32)SEfcRO[ucUnit].u8FMCNGPNVM) << 16) & AT91C_MC_FMCN));
         
         // Default non-volatile configuration (supported only for unit 0, clear all set/clear flags to disable default configuration, e.g. for unit 1)
         bGPNVM= (SEfcRO[ucUnit].u16GPNVMCGPB & BITS_USHORTH) || (SEfcRO[ucUnit].u16GPNVMSGPB & BITS_USHORTH);
         
         // Init completed so far
         SEfcRW[ucUnit].bInit= (SEfcRW[ucUnit].tidTskMsg != 0);

         // Clear default General Purpose Non Volatile Memory bit(s)
         if (bGPNVM ? (EfcGPNVMClear(ucUnit, (SEfcRO[ucUnit].u16GPNVMCGPB & BITS_USHORTH), SEfcRO[ucUnit].u16Timeout)) : (TRUE))
         {
            // Set default General Purpose Non Volatile Memory bit(s)
            if (bGPNVM ? (EfcGPNVMSet(ucUnit, (SEfcRO[ucUnit].u16GPNVMSGPB & BITS_USHORTH), SEfcRO[ucUnit].u16Timeout)) : (TRUE))
            {
               // Clear default lock bit(s) for blocks (lock regions)
               if (EfcLockClear(ucUnit, (SEfcRO[ucUnit].u32LOCKCLB & BITS_ULONGHX), SEfcRO[ucUnit].u16Timeout))
               {
                  // Set default lock bit(s) for blocks (lock regions)
                  if (EfcLockSet(ucUnit, (SEfcRO[ucUnit].u32LOCKSLB & BITS_ULONGHX), SEfcRO[ucUnit].u16Timeout))
                  {
                     // Return result
                     bResult= SEfcRW[ucUnit].bInit;
                  }
               }
            }
         }
      }
   }
      
   return (bResult);
}

//----------------------------------------------------------------------------
// bool EfcIsReady
//----------------------------------------------------------------------------
bool EfcIsReady(UCHAR ucUnit)
{
   bool bResult= FALSE;
   
   if (ucUnit < EFC_UNIT_MAX)
   {
      bResult= ((SEfcRO[ucUnit].pEfcBase->EFC_FSR & AT91C_MC_FRDY) != 0);
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// UCHAR EfcIsError
//----------------------------------------------------------------------------
UCHAR EfcIsError(UCHAR ucUnit)
{
   UCHAR ucResult= (AT91C_MC_LOCKE | AT91C_MC_PROGE);
   
   if (ucUnit < EFC_UNIT_MAX)
   {
      ucResult= (SEfcRO[ucUnit].pEfcBase->EFC_FSR & (AT91C_MC_LOCKE | AT91C_MC_PROGE));
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// bool EfcMsgWait
//----------------------------------------------------------------------------
bool EfcMsgWait(UCHAR ucUnit, U16 u16Timeout)
{
   bool bResult= FALSE;

   if (ucUnit < EFC_UNIT_MAX)
   {
      // Check unit's init completed successfully
      if (SEfcRW[ucUnit].bInit)
      {
         // Enable irq via non-maskable sysirq
         SEfcRW[ucUnit].bIrq= TRUE;
   
         // Arm the IRQs (which trigger the semaphore we pend on below)
         SEfcRO[ucUnit].pEfcBase->EFC_FMR |= EFC_IRQ_MASK;
   
         if (OS_SEM_TAKE_CHK(os_sem_take(SEfcRW[ucUnit].SemNew, u16Timeout)))
         {
            // Disable irq via non-maskable sysirq
            SEfcRW[ucUnit].bIrq= FALSE;
   
            bResult= TRUE;
         }
      }
   }  
   
   return (bResult);
}

//----------------------------------------------------------------------------
// bool EfcGPNVMSet
//----------------------------------------------------------------------------
bool EfcGPNVMSet(UCHAR ucUnit, U16 u16GPNVMSGPB, U16 u16Timeout)
{
   return (EfcBitChange(ucUnit, u16GPNVMSGPB, AT91C_MC_FCMD_SET_GP_NVM, /* 0 ... */ 7 /* for AT91C_MC_GPNVM7 >> 8 */, u16Timeout));
}

//----------------------------------------------------------------------------
// bool EfcGPNVMClear
//----------------------------------------------------------------------------
bool EfcGPNVMClear(UCHAR ucUnit, U16 u16GPNVMCGPB, U16 u16Timeout)
{
   return (EfcBitChange(ucUnit, u16GPNVMCGPB, AT91C_MC_FCMD_CLR_GP_NVM, /* 0 ... */ 7 /* for AT91C_MC_GPNVM7 >> 8 */, u16Timeout));
}

//----------------------------------------------------------------------------
// bool EfcLockSet
//----------------------------------------------------------------------------
bool EfcLockSet(UCHAR ucUnit, U32 u32LOCKSLB, U16 u16Timeout)
{
   return (EfcBitChange(ucUnit, u32LOCKSLB, AT91C_MC_FCMD_LOCK, /* 0 ... */ 15 /* for AT91C_MC_LOCKS15 >> 16 */, u16Timeout));
}

//----------------------------------------------------------------------------
// bool EfcLockClear
//----------------------------------------------------------------------------
bool EfcLockClear(UCHAR ucUnit, U32 u32LOCKCLB, U16 u16Timeout)
{
   return (EfcBitChange(ucUnit, u32LOCKCLB, AT91C_MC_FCMD_UNLOCK, /* 0 ... */ 15 /* for AT91C_MC_LOCKS15 >> 16 */, u16Timeout));
}

//----------------------------------------------------------------------------
// bool EfcBitChange
//----------------------------------------------------------------------------
bool EfcBitChange(UCHAR ucUnit, U32 u32Mask, U8 u8Cmd, U32 u32Max, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   /* static */ U32 u32Cr;
   U32 u32Reg;
   U32 u32MaskAligned;
   U32 u32RegDiff;

   ULONG ulIndex;
   USHORT usReady;
   USHORT usError;  
   
   if (ucUnit < EFC_UNIT_MAX)
   {
      // Security bit is intentionally _not_ supported!
      if ((u8Cmd == AT91C_MC_FCMD_SET_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_LOCK) || (u8Cmd == AT91C_MC_FCMD_CLR_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_UNLOCK))
      {
         // Check unit's init completed successfully
         if (SEfcRW[ucUnit].bInit)
         {
            // Check unit is ready for operation
            if (EfcIsReady(ucUnit))
            {
               // Lock efc
               if (OS_SEM_TAKE_CHK(os_sem_take(SEfcRW[ucUnit].SemRdy, u16Timeout)))
               {
                  // Setup fws and fmcn (GPNVM timing)
                  SEfcRO[ucUnit].pEfcBase->EFC_FMR= ((((U32)SEfcRO[ucUnit].u16FWS) & AT91C_MC_FWS) | ((((U32)SEfcRO[ucUnit].u8FMCNGPNVM) << 16) & AT91C_MC_FMCN));
   
                  // General Purpose Non Volatile Memory bit(s)
                  if ((u8Cmd == AT91C_MC_FCMD_SET_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_CLR_GP_NVM))
                  {
                     // Only upper 8 bits are used
                     u32MaskAligned= ((u32Mask & BITS_USHORTH) >> 8);
                     
                     // Retrieve General Purpose Non Volatile Memory bit mask
                     u32Reg= ((SEfcRO[ucUnit].pEfcBase->EFC_FSR & BITS_USHORTH) >> 8);
                  }
                  // Lock region bit(s)
                  else if ((u8Cmd == AT91C_MC_FCMD_LOCK) || (u8Cmd == AT91C_MC_FCMD_UNLOCK))
                  {
                     // Only upper 16 bits are used
                     u32MaskAligned= ((u32Mask & BITS_ULONGHX) >> 16);
                     
                     // Retrieve non volatile lock bit mask
                     u32Reg= ((SEfcRO[ucUnit].pEfcBase->EFC_FSR & BITS_ULONGHX) >> 16);
                  }
                  
                  // Any Non Volatile Memory bits to change?
                  if (u32MaskAligned)
                  {
                     u32RegDiff= (u32Reg ^ u32MaskAligned);
                     
                     if ((u8Cmd == AT91C_MC_FCMD_SET_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_LOCK))
                     {
                        u32MaskAligned= (u32MaskAligned & u32RegDiff);
                     }
                     else if ((u8Cmd == AT91C_MC_FCMD_CLR_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_UNLOCK))
                     {
                        u32MaskAligned= (u32MaskAligned & ~u32RegDiff);
                     }

                     // Any change necessary?
                     if (u32MaskAligned)
                     {
                        // Set/clear (u8Cmd) the still different bits individually
                        for (usReady= 1, usError= 0, ulIndex= 0; (ulIndex <= u32Max) && (!usError) && (usReady); ulIndex++)
                        {
                           if (u32MaskAligned & (1 << ulIndex))
                           {
                              // Number param
                              if ((u8Cmd == AT91C_MC_FCMD_SET_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_CLR_GP_NVM))
                              {
                                 u32Cr= ((((ULONG)u8Cmd) & AT91C_MC_FCMD) | ((ulIndex << 8) & AT91C_MC_PAGEN) | (AT91C_MC_CORRECT_KEY & AT91C_MC_KEY));
                              }
                              // Number param with factor
                              else if ((u8Cmd == AT91C_MC_FCMD_LOCK) || (u8Cmd == AT91C_MC_FCMD_UNLOCK))
                              {
                                 // Convert bit mask into a page number for the command here!!!
                                 u32Cr= ((((ULONG)u8Cmd) & AT91C_MC_FCMD) | (((ulIndex* SEfcRO[ucUnit].ucPageCount) << 8) & AT91C_MC_PAGEN) | (AT91C_MC_CORRECT_KEY & AT91C_MC_KEY));
                              }

                              // Write access
                              SEfcRO[ucUnit].pEfcBase->EFC_FCR= u32Cr;
                                                
                              // Pend for flash message
                              if (EfcMsgWait(ucUnit, u16Timeout))
                              {
                                 // Retrieve messages
                                 usError= EfcMsg(ucUnit, (AT91C_MC_LOCKE | AT91C_MC_PROGE), 1, u16Timeout);
                                 usReady= EfcMsg(ucUnit, AT91C_MC_FRDY, 1, u16Timeout);
                              }
                           }
                        }
                                                            
                        if ((!usError) && (usReady))
                        {
                           // General Purpose Non Volatile Memory bit(s)
                           if ((u8Cmd == AT91C_MC_FCMD_SET_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_CLR_GP_NVM))
                           {
                              // Only upper 8 bits are used
                              u32Reg= ((SEfcRO[ucUnit].pEfcBase->EFC_FSR & BITS_USHORTH) >> 8);
                           }
                           // Lock region bit(s)
                           else if ((u8Cmd == AT91C_MC_FCMD_LOCK) || (u8Cmd == AT91C_MC_FCMD_UNLOCK))
                           {
                              // Only upper 16 bits are used
                              u32Reg= ((SEfcRO[ucUnit].pEfcBase->EFC_FSR & BITS_ULONGHX) >> 16);
                           }
                           
                           // Result is the verify result of the modified bit(s)
                           if ((u8Cmd == AT91C_MC_FCMD_SET_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_LOCK))
                           {
                              bResult= ((u32Reg & u32MaskAligned) == u32MaskAligned);
                           }
                           else if ((u8Cmd == AT91C_MC_FCMD_CLR_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_UNLOCK))
                           {
                              bResult= ((u32Reg & u32MaskAligned) == 0);
                           }
                        }
                     }
                     else
                     {
                        // Success also for already matching bit(s)
                        bResult= TRUE;
                     }
                  }
                  else
                  {
                     // Success also for no bit(s) to change
                     bResult= TRUE;
                  }
                  
                  // Unlock efc
                  os_sem_give(SEfcRW[ucUnit].SemRdy);
               }
            }
         }
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// bool EfcGPNVMGet
//----------------------------------------------------------------------------
bool EfcGPNVMMapGet(UCHAR ucUnit, U32 *pu32Map, U16 u16Timeout)
{
   return (EfcBitMapGet(ucUnit, AT91C_MC_FCMD_SET_GP_NVM, pu32Map, u16Timeout));
}

//----------------------------------------------------------------------------
// bool EfcLockGet
//----------------------------------------------------------------------------
bool EfcLockMapGet(UCHAR ucUnit, U32 *pu32Map, U16 u16Timeout)
{
   return (EfcBitMapGet(ucUnit, AT91C_MC_FCMD_LOCK, pu32Map, u16Timeout));
}

//----------------------------------------------------------------------------
// bool EfcBitMapGet
//----------------------------------------------------------------------------
bool EfcBitMapGet(UCHAR ucUnit, U8 u8Cmd, U32 *pu32Map, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   U32 u32Reg;
   
   if (ucUnit < EFC_UNIT_MAX)
   {
      if (pu32Map)
      {
         // Security bit is intentionally _not_ supported!
         if ((u8Cmd == AT91C_MC_FCMD_SET_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_LOCK) || (u8Cmd == AT91C_MC_FCMD_CLR_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_UNLOCK))
         {   
            // Check unit's init completed successfully
            if (SEfcRW[ucUnit].bInit)
            {
               // Check unit is ready for operation
               if (EfcIsReady(ucUnit))
               {
                  // Lock efc
                  if (OS_SEM_TAKE_CHK(os_sem_take(SEfcRW[ucUnit].SemRdy, u16Timeout)))
                  {
                     // General Purpose Non Volatile Memory bit(s)
                     if ((u8Cmd == AT91C_MC_FCMD_SET_GP_NVM) || (u8Cmd == AT91C_MC_FCMD_CLR_GP_NVM))
                     {
                        // Retrieve General Purpose Non Volatile Memory bit mask
                        u32Reg= ((SEfcRO[ucUnit].pEfcBase->EFC_FSR & BITS_USHORTH) /* >> 8 */);
                     }
                     // Lock region bit(s)
                     else if ((u8Cmd == AT91C_MC_FCMD_LOCK) || (u8Cmd == AT91C_MC_FCMD_UNLOCK))
                     {
                        // Retrieve non volatile lock bit mask
                        u32Reg= ((SEfcRO[ucUnit].pEfcBase->EFC_FSR & BITS_ULONGHX) /* >> 16 */);
                     }
                     
                     // Return result
                     *pu32Map= u32Reg;
                     bResult= TRUE;
                     
                     // Unlock efc
                     os_sem_give(SEfcRW[ucUnit].SemRdy);
                  }
               }
            }
         }
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// UCHAR EfcPageOpenByAddress
//----------------------------------------------------------------------------
UCHAR EfcPageOpenByAddress(bool bRCB, UCHAR ucFlags, U32 u32Arg, UCHAR *pucUnit, U32 **pu32Start, U32 **pu32End, USHORT *pusPageSize, U16 u16Timeout)
{
   return (EfcPageOpen(bRCB, (ucFlags & ~EFC_LOCATE_PAGEABS) | EFC_LOCATE_ADDRESS, u32Arg, pucUnit, pu32Start, pu32End, pusPageSize, u16Timeout));
}

//----------------------------------------------------------------------------
// UCHAR EfcPageOpenByPageAbs
//----------------------------------------------------------------------------
UCHAR EfcPageOpenByPageAbs(bool bRCB, UCHAR ucFlags, U32 u32Arg, UCHAR *pucUnit, U32 **pu32Start, U32 **pu32End, USHORT *pusPageSize, U16 u16Timeout)
{
   return (EfcPageOpen(bRCB, (ucFlags & ~EFC_LOCATE_ADDRESS) | EFC_LOCATE_PAGEABS, u32Arg, pucUnit, pu32Start, pu32End, pusPageSize, u16Timeout));
}

//----------------------------------------------------------------------------
// UCHAR EfcPageOpen
//----------------------------------------------------------------------------
UCHAR EfcPageOpen(bool bRCB, UCHAR ucFlags, U32 u32Arg, UCHAR *pucUnit, U32 **pu32Start, U32 **pu32End, USHORT *pusPageSize, U16 u16Timeout)
{
   /* static */ UCHAR ucResult= 0;
   
   /* static */ U32 u32BlockMap;
   /* static */ U32 u32PageSize;
   /* static */ U32 *pu32Access;
   /* static */ USHORT usIdx;
   /* static */ void *pvStart;
   /* static */ void *pvEnd;
   /* static */ UCHAR ucUnit;
   /* static */ UCHAR ucBlock;
   /* static */ bool bResult2;
   
   // Determine globals (unit, page start address) and block number (lock region) of the unit
   if (EfcLocate(bRCB, ucFlags, u32Arg, &ucUnit, &pvStart, &pvEnd, NULL, &ucBlock, NULL))
   {
      if (ucUnit < EFC_UNIT_MAX)
      {
         // Check unit's init completed successfully
         if (SEfcRW[ucUnit].bInit)
         {
            // Retrieve lock bits
            if (EfcLockMapGet(ucUnit, &u32BlockMap, u16Timeout))
            {
               // Test region is locked
               bResult2= TRUE;
               if (u32BlockMap & (/* 1 */ AT91C_MC_LOCKS0 << ucBlock))
               {
                  // Required unlock requested?
                  if (ucFlags & EFC_OPEN_UNLOCK)
                  {
                     // Unlock block (lock region)
                     if (!(EfcLockClear(ucUnit, AT91C_MC_LOCKS0 << ucBlock, u16Timeout)))
                     {
                        bResult2= FALSE;
                     }
                  }
               }
               
               // Continue when still successful
               if (bResult2)
               {
                  // Retrieve lock bits
                  if (EfcLockMapGet(ucUnit, &u32BlockMap, u16Timeout))
                  {
                     // Test region is unlocked now
                     if (!(u32BlockMap & (/* 1 */ AT91C_MC_LOCKS0 << ucBlock)))
                     {
                        // Lock hardware page buffer
                        if (OS_SEM_TAKE_CHK(os_sem_take(SEfcRW[ucUnit].SemPgBuff, u16Timeout)))
                        {
                           // Check for init request(s) of hardware page buffer
                           
                           u32PageSize= (SEfcRO[ucUnit].usPageSize >> 2);
                           pu32Access= pvStart;
      
                           // Copy page contents to hardware page buffer (32 bit write access only!!!)
                           if (ucFlags & EFC_OPEN_COPY)
                           {
                              for (usIdx= 0; usIdx < u32PageSize; usIdx++)
                              {
                                 // Read from efc page, write to hardware page buffer ram (both appear at same address and are selected by access type)
                                 *(pu32Access+ usIdx)= *(pu32Access+ usIdx);
                              }
                           }
                           // Clear hardware page buffer
                           else if (ucFlags & EFC_OPEN_CLEAR)
                           {
                              for (usIdx= 0; usIdx < u32PageSize; usIdx++)
                              {
                                 // Write to hardware page buffer ram only
                                 *(pu32Access+ usIdx)= 0xFFFFFFFF;
                              }
                           }
                           
                           // Return unit
                           if (pucUnit)
                           {
                              *pucUnit= ucUnit;
                              ucResult++;
                           }
                           
                           // Return start pointer
                           if (pu32Start)
                           {
                              *pu32Start= pvStart;
                              ucResult++;
                           }
                           
                           // Return end pointer
                           if (pu32End)
                           {
                              *pu32End= pvEnd;
                              ucResult++;
                           }
                           
                           // Return page size
                           if (pusPageSize)
                           {
                              *pusPageSize= SEfcRO[ucUnit].usPageSize;
                              ucResult++;
                           }
                           
                           // Keep hardware page buffer locked
                        }
                     }
                  }
               }
            }
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// bool EfcPageCloseByAddress
//----------------------------------------------------------------------------
bool EfcPageCloseByAddress(bool bRCB, UCHAR ucFlags, U32 u32Arg, U16 u16Timeout)
{
   return (EfcPageClose(bRCB, (ucFlags & ~EFC_LOCATE_PAGEABS) | EFC_LOCATE_ADDRESS, u32Arg, u16Timeout));
}

//----------------------------------------------------------------------------
// bool EfcPageCloseByPageAbs
//----------------------------------------------------------------------------
bool EfcPageCloseByPageAbs(bool bRCB, UCHAR ucFlags, U32 u32Arg, U16 u16Timeout)
{
   return (EfcPageClose(bRCB, (ucFlags & ~EFC_LOCATE_ADDRESS) | EFC_LOCATE_PAGEABS, u32Arg, u16Timeout));
}

//----------------------------------------------------------------------------
// bool EfcPageClose
//----------------------------------------------------------------------------
bool EfcPageClose(bool bRCB, UCHAR ucFlags, U32 u32Arg, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   U32 u32Cr;
   USHORT usError;
   USHORT usReady;
   USHORT usPage;
   USHORT usBlockPageCount;
   UCHAR ucBlockPage;
   UCHAR ucUnit;
   U8 u8Cmd;
   
   // Determine globals (unit), absolute page number of the unit and relative page number of the block
   if (EfcLocate(bRCB, ucFlags, u32Arg, &ucUnit, NULL, NULL, &usPage, NULL, &ucBlockPage))
   {
      if (ucUnit < EFC_UNIT_MAX)
      {
         // Check unit's init completed successfully
         if (SEfcRW[ucUnit].bInit)
         {
            if (EfcBlockPageCount(ucUnit, &usBlockPageCount))
            {
               // Test required lock of hardware page buffer
               if (!OS_SEM_TAKE_CHK(os_sem_take(SEfcRW[ucUnit].SemPgBuff, OS_T_NONE)))
               {
                  if (!(ucFlags & EFC_CLOSE_CANCEL))
                  {
                     // Check unit is ready for operation
                     if (EfcIsReady(ucUnit))
                     {
                        // Lock efc
                        if (OS_SEM_TAKE_CHK(os_sem_take(SEfcRW[ucUnit].SemRdy, u16Timeout)))
                        {
                           // Setup fws and fmcn (DATA timing) [NEBP support is useful only after erase-all by ram-functions and therefore ommited yet]
                           SEfcRO[ucUnit].pEfcBase->EFC_FMR= ((((U32)SEfcRO[ucUnit].u16FWS) & AT91C_MC_FWS) | ((((U32)SEfcRO[ucUnit].u8FMCNDATA) << 16) & AT91C_MC_FMCN));
         
                           // Check for normal write or write and lock
                           
                           // Lock block (write and lock region)
                           if (ucFlags & EFC_CLOSE_LOCK)
                           {
                              u8Cmd= AT91C_MC_FCMD_PROG_AND_LOCK;
                           }
                           else if (ucFlags & EFC_CLOSE_LOCKEND)
                           {
                              // Lock block (write and lock region) when page is the last in block's range
                              if ((ucBlockPage+ 1) >= usBlockPageCount)
                              {
                                 u8Cmd= AT91C_MC_FCMD_PROG_AND_LOCK;
                              }
                              // Normal write
                              else
                              {
                                 u8Cmd= AT91C_MC_FCMD_START_PROG;
                              }
                           }
                           // Normal write
                           else
                           {
                              u8Cmd= AT91C_MC_FCMD_START_PROG;
                           }
         
                           // Write prepared hardware page buffer to efc page
                           u32Cr= ((((ULONG)u8Cmd) & AT91C_MC_FCMD) | ((usPage << 8) & AT91C_MC_PAGEN) | (AT91C_MC_CORRECT_KEY & AT91C_MC_KEY));
                           SEfcRO[ucUnit].pEfcBase->EFC_FCR= u32Cr;
         
                           // Pend for flash message
                           if (EfcMsgWait(ucUnit, u16Timeout))
                           {
                              // Retrieve messages
                              usError= EfcMsg(ucUnit, (AT91C_MC_LOCKE | AT91C_MC_PROGE), 1, u16Timeout);
                              usReady= EfcMsg(ucUnit, AT91C_MC_FRDY, 1, u16Timeout);
                           }
         
                           // Check result(s)
                           if ((!usError) && (usReady))
                           {
                              bResult= TRUE;
                           }
                           
                           // Unlock efc
                           os_sem_give(SEfcRW[ucUnit].SemRdy);
                        }
                     }
                  }
                  // Discard any changes
                  else
                  {
                     bResult= TRUE;
                  }
                  
                  if (bResult)
                  {
                     // Unlock hardware page buffer
                     os_sem_give(SEfcRW[ucUnit].SemPgBuff);
                  }
               }
               else
               {
                  // Unlock hardware page buffer
                  os_sem_give(SEfcRW[ucUnit].SemPgBuff);
               }
            }
         }
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// UCHAR EfcLocateByAddress
//----------------------------------------------------------------------------
UCHAR EfcLocateByAddress(bool bRCB, U32 u32Arg, UCHAR *pucUnit, void **pvPageStart, void **pvPageEnd, USHORT *pusPage, UCHAR *pucBlock, UCHAR *pucBlockPage)
{
   return (EfcLocate(bRCB, EFC_LOCATE_ADDRESS, u32Arg, pucUnit, pvPageStart, pvPageEnd, pusPage, pucBlock, pucBlockPage));
}

//----------------------------------------------------------------------------
// UCHAR EfcLocateByPageAbs
//----------------------------------------------------------------------------
UCHAR EfcLocateByPageAbs(bool bRCB, U32 u32Arg, UCHAR *pucUnit, void **pvPageStart, void **pvPageEnd, USHORT *pusPage, UCHAR *pucBlock, UCHAR *pucBlockPage)
{
   return (EfcLocate(bRCB, EFC_LOCATE_PAGEABS, u32Arg, pucUnit, pvPageStart, pvPageEnd, pusPage, pucBlock, pucBlockPage));
}

//----------------------------------------------------------------------------
// UCHAR EfcLocate
//----------------------------------------------------------------------------
UCHAR EfcLocate(bool bRCB, UCHAR ucFlags, U32 u32Arg, UCHAR *pucUnit, void **pvPageStart, void **pvPageEnd, USHORT *pusPage, UCHAR *pucBlock, UCHAR *pucBlockPage)
{
   UCHAR ucResult= 0;
   
   /* static */ U32 u32Start;
   /* static */ U32 u32End;
   /* static */ U32 u32Addr;
   /* static */ U32 u32AddrOffs;
   /* static */ USHORT usPage;
   /* static */ UCHAR ucUnit;
   /* static */ UCHAR ucBlock;  
   /* static */ UCHAR ucBlockPage;
   
   if (ucFlags & EFC_LOCATE_ADDRESS)
   {
      u32Addr= u32Arg;
   }
   else if (ucFlags & EFC_LOCATE_PAGEABS)
   {
      u32Addr= MC_IFLASH_BASE_LOW(bRCB)+ (u32Arg* AT91C_IFLASH_PAGE_SIZE);
   }
   
   if ((ucFlags & EFC_LOCATE_ADDRESS) || (ucFlags & EFC_LOCATE_PAGEABS))
   {
      ucUnit= _min(UCHAR_MAX, u32Addr/ AT91C_IFLASH_UNIT_SIZE);
      if (ucUnit < EFC_UNIT_MAX)
      {
         // Unit number (required for error indication 'required unit not initialized')
         if (pucUnit)
         {
            *pucUnit= ucUnit;
         }
   
         // Check unit's init completed successfully
         if (SEfcRW[ucUnit].bInit)
         {
            if (SEfcRO[ucUnit].usBlockSize && SEfcRO[ucUnit].usPageSize && SEfcRO[ucUnit].ucPageCount)
            {
               // Address offset within the unit's range
               u32AddrOffs= u32Addr- (ucUnit* AT91C_IFLASH_UNIT_SIZE);
               
               // Block (lock region) number within the unit's range
               ucBlock= u32AddrOffs / SEfcRO[ucUnit].usBlockSize;
               
               // Page number within the unit's range (absolute page number)
               usPage= u32AddrOffs / SEfcRO[ucUnit].usPageSize;
               
               // Page number within the block's range (block relative page number)
               ucBlockPage= usPage % SEfcRO[ucUnit].ucPageCount;
               
               // Unit number (for success result count)
               if (pucUnit)
               {
                  ucResult++;
               }
               
               // Block number within the unit's range
               if (pucBlock)
               {
                  *pucBlock= ucBlock;
                  ucResult++;
               }
               
               // Page start address from absolute page number
               if (pvPageStart)
               {
                  u32Start= (MC_IFLASH_BASE_LOW(bRCB)+ (ucUnit* AT91C_IFLASH_UNIT_SIZE)+ (usPage* SEfcRO[ucUnit].usPageSize));
                  *pvPageStart= (void *)u32Start;
                  ucResult++;
               }
               
               // Page end address from absolute page number
               if (pvPageEnd)
               {
                  u32End= (MC_IFLASH_BASE_LOW(bRCB)+ (ucUnit* AT91C_IFLASH_UNIT_SIZE)+ (usPage* SEfcRO[ucUnit].usPageSize)+ (SEfcRO[ucUnit].usPageSize- 1));
                  *pvPageEnd= (void *)u32End;
                  ucResult++;
               }
               
               // Page number within the unit's range
               if (pusPage)
               {
                  *pusPage= usPage;
                  ucResult++;
               }
               
               // Page number within the block's range
               if (pucBlockPage)
               {
                  *pucBlockPage= ucBlockPage;
                  ucResult++;
               }   
            }
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EfcAddrRange
//----------------------------------------------------------------------------
UCHAR EfcAddrRange(UCHAR ucUnit, bool bRCB, U32 *pu32Start, U32 *pu32End)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < EFC_UNIT_MAX)
   {
      // Check unit's init completed successfully
      if (SEfcRW[ucUnit].bInit)
      {
         if (SEfcRO[ucUnit].ucBlockCount && SEfcRO[ucUnit].usBlockSize)
         {
            // Flash unit start address
            if (pu32Start)
            {
               *pu32Start= MC_IFLASH_BASE_LOW(bRCB)+ (ucUnit* AT91C_IFLASH_UNIT_SIZE);
               ucResult++;
            }
            
            // Flash unit end address
            if (pu32End)
            {
               *pu32End= MC_IFLASH_BASE_LOW(bRCB)+ (ucUnit* AT91C_IFLASH_UNIT_SIZE)+ ((SEfcRO[ucUnit].ucBlockCount* SEfcRO[ucUnit].usBlockSize)- 1);
               ucResult++;
            }
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// bool EfcBlockCount
//----------------------------------------------------------------------------
bool EfcBlockCount(UCHAR ucUnit, USHORT *pusBlockCount)
{
   return EfcRoGet(ucUnit, EFC_ROGET_BLOCKCOUNT, pusBlockCount);
}

//----------------------------------------------------------------------------
// bool EfcBlockPageCount
//----------------------------------------------------------------------------
bool EfcBlockPageCount(UCHAR ucUnit, USHORT *pusBlockPageCount)
{
   return EfcRoGet(ucUnit, EFC_ROGET_BLOCKPAGECOUNT, pusBlockPageCount);
}

//----------------------------------------------------------------------------
// bool EfcBlockSize
//----------------------------------------------------------------------------
bool EfcBlockSize(UCHAR ucUnit, USHORT *pusBlockSize)
{
   return EfcRoGet(ucUnit, EFC_ROGET_BLOCKSIZE, pusBlockSize);
}

//----------------------------------------------------------------------------
// bool EfcPageSize
//----------------------------------------------------------------------------
bool EfcPageSize(UCHAR ucUnit, USHORT *pusPageSize)
{
   return EfcRoGet(ucUnit, EFC_ROGET_PAGESIZE, pusPageSize);
}

//----------------------------------------------------------------------------
// bool EfcRoGet
//----------------------------------------------------------------------------
bool EfcRoGet(UCHAR ucUnit, EEFCRoGet EType, USHORT *pusValue)
{
   bool bResult= FALSE;
   
   if (ucUnit < EFC_UNIT_MAX)
   {
      // Check unit's init completed successfully
      if (SEfcRW[ucUnit].bInit)
      {
         if (pusValue)
         {
            bResult= TRUE;
            
            switch (EType)
            {
               case EFC_ROGET_BLOCKCOUNT:
                  *pusValue= SEfcRO[ucUnit].ucBlockCount;
                  break;
                  
               case EFC_ROGET_BLOCKPAGECOUNT:
                  *pusValue= SEfcRO[ucUnit].ucPageCount;
                  break;
                  
               case EFC_ROGET_BLOCKSIZE:
                  *pusValue= SEfcRO[ucUnit].usBlockSize;
                  break;
                  
               case EFC_ROGET_PAGESIZE:
                  *pusValue= SEfcRO[ucUnit].usPageSize;
                  break;
               
               default:
                  bResult= FALSE;
                  break;
            }
         }
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// USHORT EfcMsg
//----------------------------------------------------------------------------
USHORT EfcMsg(UCHAR ucUnit, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   USHORT usCounter;
   UCHAR ucIndex;
      
   if (ucUnit < EFC_UNIT_MAX)
   {
      // Check unit's init completed successfully
      if (SEfcRW[ucUnit].bInit)
      {
         RESCHEDULE_TICKS(0);
         
         // Lock counters
         if (OS_SEM_TAKE_CHK(os_sem_take(SEfcRW[ucUnit].SemMsg, u16Timeout)))
         {
            for (ucIndex= 0; ucIndex < EFC_MSGMAX; ucIndex++)
            {
               if ((1 << ucIndex) & ulMsg)
               {
                  usCounter= SEfcRW[ucUnit].xMsgCounter[ucIndex];
                  if (usCounter)
                  {
                     RAISEMAX(usResult, usCounter, USHRT_MAX);
   
                     if (usDecrement)
                     {
                        if (usDecrement < usCounter)
                        {
                           SEfcRW[ucUnit].xMsgCounter[ucIndex] -= usDecrement;
                        }
                        else
                        {
                           SEfcRW[ucUnit].xMsgCounter[ucIndex]= 0;
                        }
                     }
                  }
               }
            }
      
            // Unlock counters
            os_sem_give(SEfcRW[ucUnit].SemMsg);   
         }
      }
   }
         
   return (usResult);
}

//----------------------------------------------------------------------------
// __task void TskEfcMsg
//----------------------------------------------------------------------------
__task void TskEfcMsg(void *pvParam)
{
   struct _SEfcTsk *pSEfcTsk= (struct _SEfcTsk *) pvParam;
   
   ULONG ulValue;
   USHORT usCount;
   UCHAR ucIndex;
   
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxEfcMsg[pSEfcTsk->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         usCount= MSG_GETCNT(EFC_MSG_MSGBITS, ulValue);
         if (usCount)
         {
            // Lock counters
            if (OS_SEM_TAKE_CHK(os_sem_take(SEfcRW[pSEfcTsk->ucUnit].SemMsg, OS_T_FOREVER)))
            {         
               // Check message bits
               for (ucIndex= 0; ucIndex < EFC_MSGMAX; ucIndex++)
               {
                  if (ulValue & (1 << ucIndex))
                  {
                     // Count until max-1 and set max to show overrun condition
                     RAISEMAX(SEfcRW[pSEfcTsk->ucUnit].xMsgCounter[ucIndex], usCount, EFC_MSGCOUNTERMAX);
                  }
               }
            
               // Unlock counters
               os_sem_give(SEfcRW[pSEfcTsk->ucUnit].SemMsg);
            }
               
            // New message notification
            if (SEfcRW[pSEfcTsk->ucUnit].oiSemMsgNew)
            {
               if (!OS_SEM_TAKE_CHK(os_sem_check(SEfcRW[pSEfcTsk->ucUnit].oiSemMsgNew, FALSE)))
               {
                  os_sem_give(SEfcRW[pSEfcTsk->ucUnit].oiSemMsgNew);
               }
               UNLOCK;
            }
         }
      }
   }

   DETACH;
}

//----------------------------------------------------------------------------
// void EfcIrqHandler (sysirq!!!)
//----------------------------------------------------------------------------
void EfcIrqHandler(UCHAR ucUnit)
{
   register ULONG ulEfcMask;
   register ULONG ulEfcMsg;
     
   // Catch (EFC_FSR) the IRQ source to handle yet
   ulEfcMask= (SEfcRO[ucUnit].pEfcBase->EFC_FSR & EFC_IRQ_MASK);
   if (ulEfcMask)
   {
      // Always (!!!) clear (= disable for efc) occurred IRQ to avoid endless IRQ signaling (efc's sr does not clear the IRQs when read as known from other peripherals!)
      SEfcRO[ucUnit].pEfcBase->EFC_FMR &= ~ulEfcMask;

      // Irq may triggered before efc is initialized and cannot individually disabled
      if (SEfcRW[ucUnit].bIrq)
      {              
         // Record all other yet unhandled IRQ sources
         ulEfcMsg= ulEfcMask;
         
         // Apply message filter
         ulEfcMsg&= SEfcRW[ucUnit].ulMsgMask;
         
         // Apply max counter index available
         ulEfcMsg&= ((1 << (EFC_MSGMAX+ 1))- 1);
         
         if (ulEfcMsg)
         {
            if (isr_mbx_check(&MbxEfcMsg[ucUnit]) > 0)
            {
               isr_mbx_send(&MbxEfcMsg[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(EFC_MSG_MSGBITS, ulEfcMsg, 1)));  
            }
         }
      }
   }
   
   // System IRQ!!! ---NO--- EOI!!!
}
