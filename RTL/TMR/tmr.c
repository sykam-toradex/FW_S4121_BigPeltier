//----------------------------------------------------------------------------
// R T L   K E R N E L   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/RTL/TMR/TMR.C
// Purpose: RTL KERNEL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-08-04 initial version
// 2011-08-10 moved defines into tmrdata.h as enum
// 2011-08-17 renamed u16Info/2 to u16Val/2 in _STmrItem
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "tmr.h"
#include "tmrdata.h"

// Externals (located inside AT91SAM7X...c)
extern struct _STmrItem STmrItemList[];

#if AT91F_TMRLIST_SIZE > 0

//----------------------------------------------------------------------------
// os_tmr_call
//----------------------------------------------------------------------------
void os_tmr_call (U16 u16Info)
{
   register UINT uiIndex;
    
   for (uiIndex= 0; uiIndex < AT91F_TMRLIST_SIZE; uiIndex++)
   {
      if (STmrItemList[uiIndex].Isr)
      {
         if (((STmrItemList[uiIndex].EOption == TMR_MATCH) && (u16Info == STmrItemList[uiIndex].u16Val)) || \
             ((STmrItemList[uiIndex].EOption == TMR_RANGE) && ((u16Info >= STmrItemList[uiIndex].u16Val) && (u16Info <= STmrItemList[uiIndex].u16Val2))) || \
             ((STmrItemList[uiIndex].EOption == TMR_RANGE_OUT) && ((u16Info < STmrItemList[uiIndex].u16Val) || (u16Info > STmrItemList[uiIndex].u16Val2))) || \
             ((STmrItemList[uiIndex].EOption == TMR_MASK_AND) && (u16Info == (STmrItemList[uiIndex].u16Val & u16Info))) || \
             ((STmrItemList[uiIndex].EOption == TMR_MASK_AND_ANY) && (u16Info & STmrItemList[uiIndex].u16Val)) \
             )
         {
            STmrItemList[uiIndex].Isr(u16Info);
         }
      }
   }
}

#endif
