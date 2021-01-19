//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PIO/PIO.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-09-10 initial version
// 2009-09-16 new AT91F_PIO_CfgPinAttr function for bitwise access & defaults
// 2009-09-16 renamed AT91C_PIOATTR_DEFAULT to AT91C_PIOATTR_RESET
// 2009-09-16 changed AT91C_PIOATTR_RESET to a true action flag
// 2009-09-16 changed AT91F_PIO_CfgPinAttr to set true user attr after reset
// 2009-11-11 completed isr and trigger support
// 2009-11-11 renamed functions
// 2009-11-11 pin list index doesn't need to match port index anymore
// 2009-11-12 added args u32PDSR, u32Pin and bLevel to user isr functions
// 2009-12-22 fixed PioIrqHandler upper ucBitPos limit
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-30 added piobase arg to call of isr handlers in PioIrqHandler
// 2010-10-21 changed irq mode of PioInitIrq to INT_POSITIVE_EDGE
// 2010-10-27 changed irq mode of PioInitIrq to INT_HIGH_LEVEL (default)
// 2013-04-11 added function headers with return data type comment
// 2016-03-02 added support for pin init handler for full ext interrupt control
// 2016-06-08 enabled interrupt support in PioInit for all pin configurations
// 2016-06-10 enabled init without isr for passing params only in PioInit
// 2017-09-07 added check of pSPioList's range after seek in PioIrqHandler
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\rtl\support.h"

#include "pio.h"
#include "piodata.h"
#include "piodev.h"

// Prototypes
void PioCfgPinAttr(AT91PS_PIO pPio, struct _SPioItem *pSPioItem);
void PioInitIrq(UCHAR ucUnit);
void PioIrqHandler(UCHAR ucUnit);

// Each supported unit requires a function definition here
#if PIO_UNIT_MAX > 0
   PIO_ISR_FUNC(0);
#endif

#if PIO_UNIT_MAX > 1
   PIO_ISR_FUNC(1);
#endif

//----------------------------------------------------------------------------
// bool PioInit
//----------------------------------------------------------------------------
bool PioInit(UCHAR ucUnit, struct _SPioItem *pSPioList, UCHAR ucItemCnt)
{
   AT91PS_PIO pPio;
   struct _SPioItem *pSPioItem;
   U32 u32IrqMask;
   UCHAR ucIndex;
   
   bool bPeriphClockReq= 0;
   bool bIsrReq= 0;
   
   pPio= SPioRO[ucUnit].pBase;
   SPioRO[ucUnit].pSPioList= pSPioList;
   SPioRO[ucUnit].ucPioListSize= ucItemCnt;
   
   // Clock required for inputs with or without isr callback function
   for (ucIndex= 0; (ucIndex < ucItemCnt) && (!bPeriphClockReq); ucIndex++)
   {   
      pSPioItem= pSPioList+ ucIndex;
      bPeriphClockReq= (pSPioItem->EType == AT91C_PIOTYPE_INPUT);
   }
   
   AT91F_PIO_InterruptDisable(pPio, 0xFFFFFFFF);
   if (bPeriphClockReq) AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, 1 << SPioRO[ucUnit].u32PeriphId);
   
   // Assign pin config for every pin 
   for (u32IrqMask= 0, ucIndex= 0; ucIndex < ucItemCnt; ucIndex++)
   {
      pSPioItem= pSPioList+ ucIndex;
      PioCfgPinAttr(pPio, pSPioItem);
      
      switch (pSPioItem->EType)
      {
         case AT91C_PIOTYPE_PERIPH:
            AT91F_PIO_CfgPeriph(pPio, pSPioItem->uiMask, 0);           
            break;
            
         case AT91C_PIOTYPE_PERIPH_ALT:
            AT91F_PIO_CfgPeriph(pPio, 0, pSPioItem->uiMask);
            break;
            
         case AT91C_PIOTYPE_INPUT:
            AT91F_PIO_Enable(pPio, pSPioItem->uiMask);
            AT91F_PIO_CfgInput(pPio, pSPioItem->uiMask);
            break;
            
         case AT91C_PIOTYPE_OUTPUT_SET0:
            AT91F_PIO_Enable(pPio, pSPioItem->uiMask);
            AT91F_PIO_ClearOutput(pPio, pSPioItem->uiMask);
            AT91F_PIO_OutputEnable(pPio, pSPioItem->uiMask);
            break;
            
         case AT91C_PIOTYPE_OUTPUT_SET1:
            AT91F_PIO_Enable(pPio, pSPioItem->uiMask);
            AT91F_PIO_SetOutput(pPio, pSPioItem->uiMask);
            AT91F_PIO_OutputEnable(pPio, pSPioItem->uiMask);
            break;
      }
      
      // Check for valid isr handler
      if (pSPioItem->Isr)
      {
         bPeriphClockReq= 1;
         bIsrReq= 1;
         
         // Check for valid init handler
         if (pSPioItem->Init)
         {
            // Call init handler to pass control parameters and to determine initial interrupt state
            if (pSPioItem->Init(pSPioItem->uiMask, SPioRO[ucUnit].pBase))
            {
               u32IrqMask |= pSPioItem->uiMask;
            }
         }
         // Enable interrupt for non-existing init handler by default (for compatibility)
         else
         {
            u32IrqMask |= pSPioItem->uiMask;           
         }
      }
      else
      {
         // Check for valid init handler (without isr handler)
         if (pSPioItem->Init)
         {
            // Call init handler to pass control parameters only
            pSPioItem->Init(pSPioItem->uiMask, SPioRO[ucUnit].pBase);
         }
      }
   }
         
   if (bIsrReq)
   {
      AT91F_PIO_InterruptEnable(pPio, u32IrqMask);   
      PioInitIrq(ucUnit);
   }
   
   return (bPeriphClockReq);
}

//----------------------------------------------------------------------------
// void PioCfgPinAttr
//----------------------------------------------------------------------------
// Calls to the remarked AT91F-functions are not bitwise, therefore registers are directly accessed here
void PioCfgPinAttr(AT91PS_PIO pPio, struct _SPioItem *pSPioItem)
{  
   // Handle default (like after reset)
   if (pSPioItem->EAttr & AT91C_PIOATTR_RESET)
   {
      if (pPio->PIO_MDSR & pSPioItem->uiMask) pPio->PIO_MDDR= pSPioItem->uiMask;
      if (pPio->PIO_PPUSR & pSPioItem->uiMask) pPio->PIO_PPUER= pSPioItem->uiMask;
      if (pPio->PIO_OWSR & pSPioItem->uiMask) pPio->PIO_OWDR= pSPioItem->uiMask;
      if (pPio->PIO_IFSR & pSPioItem->uiMask) pPio->PIO_IFDR= pSPioItem->uiMask;
   }

   // AT91F_PIO_CfgOpendrain(pPio, pSPioItem->uiMask)
   if (pSPioItem->EAttr & AT91C_PIOATTR_OPENDRAIN_ON) pPio->PIO_MDER= pSPioItem->uiMask;
   if (pSPioItem->EAttr & AT91C_PIOATTR_OPENDRAIN_OFF) pPio->PIO_MDDR= pSPioItem->uiMask;

   // AT91F_PIO_CfgPullup(pPio, pSPioItem->uiMask)
   if (pSPioItem->EAttr & AT91C_PIOATTR_PULLUP_ON) pPio->PIO_PPUER= pSPioItem->uiMask;
   if (pSPioItem->EAttr & AT91C_PIOATTR_PULLUP_OFF) pPio->PIO_PPUDR= pSPioItem->uiMask;
   
   // AT91F_PIO_CfgDirectDrive(pPio, pSPioItem->uiMask)
   if (pSPioItem->EAttr & AT91C_PIOATTR_DIRECTDRIVE_ON) pPio->PIO_OWER= pSPioItem->uiMask;
   if (pSPioItem->EAttr & AT91C_PIOATTR_DIRECTDRIVE_OFF) pPio->PIO_OWDR= pSPioItem->uiMask;
   
   // AT91F_PIO_CfgInputFilter(pPio, pSPioItem->uiMask)
   if (pSPioItem->EAttr & AT91C_PIOATTR_DEGLITCH_ON) pPio->PIO_IFER= pSPioItem->uiMask;
   if (pSPioItem->EAttr & AT91C_PIOATTR_DEGLITCH_OFF) pPio->PIO_IFDR= pSPioItem->uiMask;
}

//----------------------------------------------------------------------------
// void PioInitIrq
//----------------------------------------------------------------------------
void PioInitIrq(UCHAR ucUnit)
{
   // Clear pending IRQs
   AT91F_PIO_GetInterruptStatus(SPioRO[ucUnit].pBase);
   
   // IRQ ctrl
   AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, SPioRO[ucUnit].u32PeriphId, 0, 0, IRQ_CAST(SPioRO[ucUnit].Isr));
   AT91F_AIC_EnableIt(AT91C_BASE_AIC, SPioRO[ucUnit].u32PeriphId);
}     

//----------------------------------------------------------------------------
// void PioIrqHandler
//----------------------------------------------------------------------------
void PioIrqHandler(UCHAR ucUnit)
{
   register U32 u32ISR;
   register U32 u32IMR;
   register U32 u32PDSR;
   register U32 u32Pin;
   register UCHAR ucBitPos;
   register UCHAR ucListIdx;
   register bool bLevel;

   u32IMR= SPioRO[ucUnit].pBase->PIO_IMR;
   u32ISR= SPioRO[ucUnit].pBase->PIO_ISR & u32IMR;
   u32PDSR= SPioRO[ucUnit].pBase->PIO_PDSR & u32IMR;
   
   for (ucBitPos= 0; ucBitPos < 32; ucBitPos++)
   {
      u32Pin= (1 << ucBitPos);
      if (u32ISR & u32Pin)
      {
         // Retrieve current pin level
         bLevel= (u32PDSR & (1 << ucBitPos)) != 0; ///>> ucBitPos;
         
         if (u32Pin != SPioRO[ucUnit].pSPioList[ucBitPos].uiMask)
         {
            // Seek to pin inside list
            for (ucListIdx= 0; (ucListIdx < SPioRO[ucUnit].ucPioListSize) && (u32Pin != SPioRO[ucUnit].pSPioList[ucListIdx].uiMask); ucListIdx++);
         }
         else
         {
            // Already at right position
            ucListIdx= ucBitPos;
         }
         
         // Handle found pin
         if (ucListIdx < SPioRO[ucUnit].ucPioListSize)
         {
            if (u32Pin == SPioRO[ucUnit].pSPioList[ucListIdx].uiMask)
            {
               // Filter irq by specified trigger condition
               if ((SPioRO[ucUnit].pSPioList[ucListIdx].ETrigger == AT91C_PIOTRG_ANY) ||
                  ((SPioRO[ucUnit].pSPioList[ucListIdx].ETrigger == AT91C_PIOTRG_FALL) && !bLevel) ||
                  ((SPioRO[ucUnit].pSPioList[ucListIdx].ETrigger == AT91C_PIOTRG_RAISE) && bLevel))
               {      
                  // Call user isr function with args PIO_ISR and PIO_PDSR
                  if (SPioRO[ucUnit].pSPioList[ucListIdx].Isr) SPioRO[ucUnit].pSPioList[ucListIdx].Isr(u32ISR, u32PDSR, u32Pin, bLevel, SPioRO[ucUnit].pBase);
               }
            }
         }
      }
   }
           
   // ARM needs EXPLICIT acknowledge (qualifier __irq doesn't do this) for (E)nd (O)f (I)RQ!!
   EOI;
}
