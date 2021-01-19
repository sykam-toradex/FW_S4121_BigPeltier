//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIRAUX.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-03-23 initial version
// 2013-08-22 changed timeouts from milliseconds to ticks
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRAUX_H
#define SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRAUX_H 1


//----------------------------------------------------------------------------
// ULONG StpClkDirAuxDirAt91Pio
//----------------------------------------------------------------------------
ULONG StpClkDirAuxDirAt91Pio(UCHAR ucUnit, bool bClockwise)
{
   ULONG ulResult= 0;
   
   register U32 u32PinDir;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {  
      u32PinDir= (bClockwise) ? (SStpClkDirAuxDirAt91PioRO[ucUnit].bPinDirClockwise) : (!SStpClkDirAuxDirAt91PioRO[ucUnit].bPinDirClockwise);
      
      // Set direction
      if (u32PinDir)
      {
         AT91F_PIO_SetOutput(SStpClkDirAuxDirAt91PioRO[ucUnit].pBase, SStpClkDirAuxDirAt91PioRO[ucUnit].u32PinDirMask);
      }
      else
      {
         AT91F_PIO_ClearOutput(SStpClkDirAuxDirAt91PioRO[ucUnit].pBase, SStpClkDirAuxDirAt91PioRO[ucUnit].u32PinDirMask);
      }     

      ulResult= SStpClkDirAuxDirAt91PioRO[ucUnit].u32PinDirMask;
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirAuxPwrAt91Pio
//----------------------------------------------------------------------------
ULONG StpClkDirAuxPwrAt91Pio(UCHAR ucUnit, bool bEnable)
{
   ULONG ulResult= 0;
   
   register U32 u32PinPwr;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {  
      u32PinPwr= (bEnable) ? (SStpClkDirAuxPwrAt91PioRO[ucUnit].bPinPwrRun) : (!SStpClkDirAuxPwrAt91PioRO[ucUnit].bPinPwrRun);
      
      // Control power stage
      if (u32PinPwr)
      {
         AT91F_PIO_SetOutput(SStpClkDirAuxPwrAt91PioRO[ucUnit].pBase, SStpClkDirAuxPwrAt91PioRO[ucUnit].u32PinPwrMask);
      }
      else
      {
         AT91F_PIO_ClearOutput(SStpClkDirAuxPwrAt91PioRO[ucUnit].pBase, SStpClkDirAuxPwrAt91PioRO[ucUnit].u32PinPwrMask);
      }     

      ulResult= SStpClkDirAuxPwrAt91PioRO[ucUnit].u32PinPwrMask;
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirAuxPwrMcp2317
//----------------------------------------------------------------------------
ULONG StpClkDirAuxPwrMcp2317(UCHAR ucUnit, bool bEnable)
{
   ULONG ulResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      if (SStpClkDirAuxPwrMcp2317RO[ucUnit].u16PinPwrMask)
      {
         // Control power stage
         ulResult= Mcp2317SetOutput(SStpClkDirAuxPwrMcp2317RO[ucUnit].ucUnit, SStpClkDirAuxPwrMcp2317RO[ucUnit].u16PinPwrMask, (bEnable) ? (SStpClkDirAuxPwrMcp2317RO[ucUnit].u16PinPwrRun) : (!(SStpClkDirAuxPwrMcp2317RO[ucUnit].u16PinPwrRun & SStpClkDirAuxPwrMcp2317RO[ucUnit].u16PinPwrMask)), SStpClkDirAuxPwrMcp2317RO[ucUnit].u16Timeout);
      }
      else
      {
         // Simulation purpose
         ulResult= 1;
      }
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirAuxDacRrcs085
//----------------------------------------------------------------------------
ULONG StpClkDirAuxDacRrcs085(UCHAR ucUnit, bool bRun)
{
   ULONG ulResult= 0;

   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Set the current      
      ulResult= Dacrrcs085SetOutput(SStpClkDirAuxDacRrcs085RO[ucUnit].ucUnit, SStpClkDirAuxDacRrcs085RO[ucUnit].ucChannel, DACRRCS085_MODE_WRITEANDUPDATE, (bRun) ? (SStpClkDirRW[ucUnit].SPermanent.usDacValueRun) : (SStpClkDirRW[ucUnit].SPermanent.usDacValueHold), SStpClkDirAuxDacRrcs085RO[ucUnit].u16Timeout);
   }

   return (ulResult);
}

#endif
