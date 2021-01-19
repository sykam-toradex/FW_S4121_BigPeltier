//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PMC/PCKO/PCKO.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-10-23 initial version
// 2013-10-30 initial revision
// 2013-10-31 second revision
// 2013-11-06 renamed PckoIdGetChannel to PckoChannelIdGet to fit overal naming style
// 2013-11-06 renamed PckoInactiveGetChannel to PckoInactiveGet to fit overal naming style
// 2013-11-06 renamed PckoConfigChannel to PckoConfig to fit overal naming style
// 2013-11-06 renamed PckoStartChannel to PckoStart to fit overal naming style
// 2013-11-06 renamed PckoStopChannel to PckoStop to fit overal naming style
// 2013-11-06 renamed PckoWaitChannel to PckoWait to fit overal naming style
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "limits.h"

#include "pcko.h"
#include "pckodata.h"
#include "pckodev.h"
#include "pckoext.h"

// Prototypes


//----------------------------------------------------------------------------
// void IsrPckox __irq
//----------------------------------------------------------------------------
// Each supported unit requires a function definition here
#if PCKO_UNIT_MAX > 0
   PCKO_ISR_FUNC(0);
#endif

#if PCKO_UNIT_MAX > 1
   PCKO_ISR_FUNC(1);
#endif

//----------------------------------------------------------------------------
// bool PckoInitUnit
//----------------------------------------------------------------------------
bool PckoInitUnit(UCHAR ucUnit)
{
   bool bResult= FALSE;
   
   UCHAR ucChannel;
     
   if (ucUnit < PCKO_UNIT_MAX)
   {
      // Init channel flags
      for (ucChannel= 0; ucChannel < PCKO_CHANNEL_MAX; ucChannel++)
      {
         SPckoChannelRW[ucUnit][ucChannel].bInit= FALSE;
         SPckoChannelRW[ucUnit][ucChannel].bRun= FALSE;
      }
      
      // Init complete
      SPckoRW[ucUnit].bInit= TRUE;
      
      // Disable irq via non-maskable sysirq yet (default)
      SPckoRW[ucUnit].bIrq= FALSE;
      
      // Return result
      bResult= TRUE;
   }
      
   return (bResult);
}

//----------------------------------------------------------------------------
// bool PckoInitChannel
//----------------------------------------------------------------------------
bool PckoInitChannel(UCHAR ucUnit, UCHAR ucChannel)
{
   bool bResult= FALSE;
   
   if (ucUnit < PCKO_UNIT_MAX)
   {
      if (SPckoRW[ucUnit].bInit)
      {
         if (ucChannel < PCKO_CHANNEL_MAX)
         {
            // Semaphore
            os_sem_init(SPckoChannelRW[ucUnit][ucChannel].SemRun, 0);
            
            // Always (!!!) clear (= disable) occurred IRQ to avoid endless IRQ signaling (sr does not clear the IRQs when read as known from other peripherals!)
            SPckoRO[ucUnit].pBase->PMC_IDR &= ~(AT91C_PMC_PCK0 << ucChannel);
            
            // No clock output ready yet (set by IRQ & PckoChannelWait later)
            SPckoChannelRW[ucUnit][ucChannel].bRun= FALSE;
            
            // Init complete
            SPckoChannelRW[ucUnit][ucChannel].bInit= TRUE;
            
            // Config channel (zero timeout because channel cannot have a busy/active or running state yet)
            if (PckoConfig(ucUnit, ucChannel, SPckoChannelRO[ucUnit][ucChannel].u8Pcr, OS_T_NONE))
            {
               // Handle autostart request
               if (SPckoChannelRO[ucUnit][ucChannel].bAutostart)
               {
                  // Start channel (still requires PckoChannelWait call to syncronize channel is running !!!) and enable IRQ
                  bResult= PckoStart(ucUnit, ucChannel);
               }
            }
            else
            {
               // Return result
               bResult= TRUE;
            }
         }
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// U16 PckoChannelIdGet
//----------------------------------------------------------------------------
U16 PckoChannelIdGet(UCHAR ucUnit, UCHAR ucChannel)
{
   U16 u16Result= 0;
   
   if (ucUnit < PCKO_UNIT_MAX)
   {
      if (ucChannel < PCKO_CHANNEL_MAX)
      {           
         u16Result= SPckoChannelRO[ucUnit][ucChannel].u16ChannelId;
      }
   }  

   return (u16Result);
}

//----------------------------------------------------------------------------
// bool PckoInactiveGet
//----------------------------------------------------------------------------
bool PckoInactiveGet(UCHAR ucUnit, UCHAR ucChannel)
{
   bool bResult= FALSE;
    
   if (ucUnit < PCKO_UNIT_MAX)
   {
      if (ucChannel < PCKO_CHANNEL_MAX)
      {
         bResult= ((SPckoRO[ucUnit].pBase->PMC_SCSR & SPckoChannelRO[ucUnit][ucChannel].u16ChannelId) == 0);
         
         if (bResult)
         {
            // Clear channel running flag (set by IRQ & PckoChannelWait later)
            SPckoChannelRW[ucUnit][ucChannel].bRun= FALSE;
         }
      }
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// bool PckoConfig
//----------------------------------------------------------------------------
bool PckoConfig(UCHAR ucUnit, UCHAR ucChannel, U8 u8Pcr, U16 u16Timeout)
{
   bool bResult= FALSE;
   bool bResult2= TRUE;
   
   if (ucUnit < PCKO_UNIT_MAX)
   {
      if (ucChannel < PCKO_CHANNEL_MAX)
      {
         // Check for channel busy/active
         if (!PckoInactiveGet(ucUnit, ucChannel))
         {
            // Stop the channel with given timeout
            bResult2= PckoStop(ucUnit, ucChannel, u16Timeout);
         }
         
         if (bResult2)
         {
            // Verify channel is ready/inactive
            if (PckoInactiveGet(ucUnit, ucChannel))
            {
               // Config channel
               SPckoRO[ucUnit].pBase->PMC_PCKR[PCKO_CHID2UNIT(SPckoChannelRO[ucUnit][ucChannel].u16ChannelId)]= (u8Pcr & (AT91C_PMC_PRES | AT91C_PMC_CSS));

               // Return result
               bResult= TRUE;
            }
         }
      }
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// bool PckoStart
//----------------------------------------------------------------------------
bool PckoStart(UCHAR ucUnit, UCHAR ucChannel)
{
   bool bResult= FALSE;
   
   if (ucUnit < PCKO_UNIT_MAX)
   {
      if (ucChannel < PCKO_CHANNEL_MAX)
      {
         // Check channel ready/inactive and clear channel running flag when ready/inactive (set by IRQ & PckoChannelWait later)
         if (PckoInactiveGet(ucUnit, ucChannel))
         {
            // Enable IRQ
            SPckoRW[ucUnit].bIrq= TRUE;

            // Enable pcko channel IRQ
            SPckoRO[ucUnit].pBase->PMC_IER |= SPckoChannelRO[ucUnit][ucChannel].u16ChannelId;
            
            // Enable pcko channel
            SPckoRO[ucUnit].pBase->PMC_SCER |= SPckoChannelRO[ucUnit][ucChannel].u16ChannelId;

            // Return result
            bResult= TRUE;
         }
      }
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// bool PckoStop
//----------------------------------------------------------------------------
bool PckoStop(UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   if (ucUnit < PCKO_UNIT_MAX)
   {
      if (ucChannel < PCKO_CHANNEL_MAX)
      {
         // Pend for channel running
         if (PckoWait(ucUnit, ucChannel, u16Timeout))
         {
            // Disable pcko channel
            SPckoRO[ucUnit].pBase->PMC_SCDR |= SPckoChannelRO[ucUnit][ucChannel].u16ChannelId;
            
            // Clear channel running flag (set by IRQ & PckoChannelWait later)
            SPckoChannelRW[ucUnit][ucChannel].bRun= FALSE;

            // Return result
            bResult= TRUE;
         }
      }
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// bool PckoWait
//----------------------------------------------------------------------------
bool PckoWait(UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout)
{
   bool bResult= FALSE;
    
   if (ucUnit < PCKO_UNIT_MAX)
   {
      if (ucChannel < PCKO_CHANNEL_MAX)
      {
         // Check for channel busy/active
         if (!PckoInactiveGet(ucUnit, ucChannel))
         {
            // Check for channel already checked and running
            if (SPckoChannelRW[ucUnit][ucChannel].bRun)
            {
               // Return success
               bResult= TRUE;
            }
            // Pend for channel running
            else if (OS_SEM_TAKE_CHK(os_sem_take(SPckoChannelRW[ucUnit][ucChannel].SemRun, u16Timeout)))
            {
               // Set channel running flag
               SPckoChannelRW[ucUnit][ucChannel].bRun= TRUE;
               
               // Return success
               bResult= TRUE;
            }
         }
      }
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// void PckoIrqHandler (sysirq!!!)
//----------------------------------------------------------------------------
void PckoIrqHandler(UCHAR ucUnit)
{
   register ULONG ulPckoMask;
   register ULONG ulPckoMsg;

   register U32   u32ChannelId;
   register UCHAR ucChannel;
     
   // Catch (PMC_ISR) the IRQ source to handle yet
   ulPckoMask= ((SPckoRO[ucUnit].pBase->PMC_SR & SPckoRO[ucUnit].pBase->PMC_IMR) & AT91C_PMC_PCKX);
   if (ulPckoMask)
   {
      // Always (!!!) clear (= disable) occurred PCKO IRQ to avoid endless IRQ signaling (sr does not clear the IRQs when read as known from other peripherals!)
      SPckoRO[ucUnit].pBase->PMC_IDR |= ulPckoMask;

      // Irq may triggered before pcko is initialized and cannot individually disabled
      if (SPckoRW[ucUnit].bIrq)
      {              
         // Record all other yet unhandled IRQ sources
         ulPckoMsg= ulPckoMask;
         
         // Check unit is initialized yet
         if (SPckoRW[ucUnit].bInit)
         {
            // Iterate thru channels
            for (ucChannel= 0; ucChannel < PCKO_CHANNEL_MAX; ucChannel++)
            {
               // Determine channel id of each channel container
               u32ChannelId= SPckoChannelRO[ucUnit][ucChannel].u16ChannelId;
               if (ulPckoMsg & u32ChannelId)
               {
                  // Check channel is initialized yet
                  if (SPckoChannelRW[ucUnit][ucChannel].bInit)
                  {
                     // Trigger pending pcko channel only
                     if (!ISR_SEM_TAKE_CHK(isr_sem_check(SPckoChannelRW[ucUnit][ucChannel].SemRun)))
                     {
                        isr_sem_give(SPckoChannelRW[ucUnit][ucChannel].SemRun);
                     }
                  }
               }
            }
         }
      }
   }
   
   // System IRQ!!! ---NO--- EOI!!!
}
