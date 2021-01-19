//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIR.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-15 initial version
// 2010-01-21 workarround for tcc IMR bug for running tcc
// 2010-01-27 fixed PWMC_CMR setting in StpClkDirPwmIrqHandler
// 2010-01-29 added Dacrrcs085SetOutput to StpClkDirInit
// 2010-02-04 removed sending of AT91C_TC_CPAS to MbxStpClkDirState[ucUnit]
// 2010-02-05 added StpClkDirQuit
// 2010-02-10 changed prototype of StpClkDirTccIrqHandlerStop
// 2010-02-10 fixed irq handler for AT91C_TC_ETRGS
// 2010-02-10 using now OS_MBX_INIT_EX(...) in StpClkDirQuit to clear mbx
// 2010-02-10 added busy condition check to StpClkDirBrkNow
// 2010-02-10 renamed StpClkDirBrkNow to StpClkDirBrk
// 2010-02-10 fixed return of StpClkDirStateSet, StpClkDirBrk, StpClkDirInit
// 2010-02-11 fixed bDir use in StpClkDirStart
// 2010-02-11 fixed memory type of ulState in StpClkDirTccIrqHandler for mbx
// 2010-02-11 using now isr_mbx_write in StpClkDirTccIrqHandler for (...send)
// 2010-02-12 fixed sporadic crash of StpClkDirPwmIrqHandler -(PWMC_CCNTR < 1)
// 2010-02-12 using now own isr_mbx_read in StpClkDirTccIrqHandler for mbx
// 2010-02-12 added ucDacValueDelayMS in StpClkDirStart
// 2010-02-18 added param u16Timeout to Mcp2317SetOutput
// 2010-02-24 added StpClkDirMove
// 2010-02-24 Mcp2317SetOutput uses now u16GpioPinResHold & u16GpioPinResRun
// 2010-02-24 added delays before ucDacValueDelayMS and after ucDacValueRunMS
// 2010-02-24 added delay ucGpioPinResDelayMS after u16GpioPinResRun
// 2010-02-24 added u32ClkCntOffset to get current clkpos by this + TC_CV reg
// 2010-02-25 added added StpClkDirClksGet, StpClkDirHzGet, StpClkDirSpeedSet
// 2010-02-25 added global constant ulUSHRT_CNT
// 2010-02-26 added StpClkDirJobClksGet and StpClkDirPwmQueueFree
// 2010-02-26 safer handling of SemStart and safer busy checks therefore
// 2010-02-26 workarround for tcc ignoring zero compares in StpClkDirStart
// 2010-02-26 IrqStpClkDirTccHandler now supports brk in next to last cycle
// 2010-02-26 added STPCLKDIR_FLAGS_AUTOPOWEROFF feature to TskStpClkDirHold
// 2010-03-04 added ulClkStableHz to _SStpClkDirRO
// 2010-03-04 fixed STPCLKDIR_FLAGS_AUTOPOWEROFF for StpClkDirStart
// 2010-03-11 added StpClkDirClksPosGet and integrated into TskStpClkDirHold
// 2010-03-12 added ulClkIdPrevious & ulClkId
// 2010-03-12 fixed StpClkDirClksAbsGet
// 2010-03-12 changed StpClkDirClksPosOffs... to ULONG offset
// 2010-03-12 added turn inversion support to StpClkDirStart
// 2010-03-12 added creep mode (allows less than 3 clks) to StpClkDirMove
// 2010-03-26 optimized StpClkDirPwmIrqHandler
// 2010-05-05 fixed missing init of bQueueState in StpClkDirMove
// 2010-05-05 fixed missing give in StpClkDirStart of SemDone & SemStart
// 2010-05-05 fixed conditional give in StpClkDirWait of SemDone
// 2010-05-07 fixed StpClkDirClksAbsGet to always calculate modulo when set
// 2010-07-01 renamed STPCLKDIR_CHID2UNIT/UNIT2CHID to PWM_CHID2UNIT/UNIT2CHID
// 2010-07-03 added function StpClkDirModuloClksGet
// 2010-07-08 added function StpClkDirPwmQueueClear
// 2010-07-08 TskStpClkDirHold now clears all pwm queues after movement done
// 2010-07-08 removed obsolete dac write & driver reset ctrl of StpClkDirInit
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-07-29 changed param names of StpClkDirSpeedChange
// 2010-07-29 fixed StpClkDirTccIrqHandler to stop continous runs correctly
// 2010-07-29 fixed give of SemStart in StpClkDirStart for SemDone unavailable
// 2010-07-29 removed obsolete addtional give of SemStart in StpClkDirQuit
// 2010-09-16 reorganized data structures (added permanent tree to rw data) 
// 2010-09-16 changed prototype of StpClkDirInit (added initial dac values)
// 2010-09-16 added functions StpClkDirDacGet & StpClkDirDacSet
// 2010-09-16 added functions StpClkDirClksAbsRawSet & StpClkDirClksAbsRawGet
// 2010-09-16 added functions StpClkDirSnapShot, -SizeGet & -AddrGet
// 2010-09-16 modified StpClkDirClksPosOffsSet for user safety (no isr use!)
// 2010-09-16 modified StpClkDirClksPosOffsGet for user safety (no isr use!)
// 2010-09-23 removed obsolete include <c55x.h>
// 2010-10-07 fixed calculation of ulClkDeltaHz in StpClkDirMove (hint by rr)
// 2010-10-14 always power stepper for dac ok in StpClkDirStart (hint by rr)
// 2010-10-15 added forced task switch to StpClkDirWait using RESCHEDULE(0)
// 2010-10-20 added STPCLKDIR_PWM_UPDATE_FIX to StpClkDirPwmIrqHandler
// 2010-11-03 added sClkAdjustOffset and usMicroStepCnt
// 2010-11-03 changed prototype of StpClkDirMove
// 2010-11-03 removed local const ulUSHRT_CNT of StpClkDirMove
// 2010-11-03 added forceable lower error full step shift in StpClkDirMove
// 2010-11-05 added function _StpClkDirClksPosOffsGet for StpClkDirMove
// 2010-11-10 fixed compare matches in StpClkDirStart (TC-TIOA delayed drop)
// 2010-11-10 fixed inverse option in StpClkDirInit & StpClkDirClksAbsGet
// 2010-11-11 removed compare match fix in StpClkDirStart (it's no TIO delay!)
// 2010-11-16 made full step shift in StpClkDirMove an optional feature
// 2010-11-16 added new mailbox MbxStpClkDirOffset to queue edge offsets
// 2010-11-16 added new function StpClkDirOffsetPush for offset correction
// 2010-11-17 fixed clks determination for full step shift in StpClkDirMove
// 2010-11-17 rewrote StpClkDirClksAbsGet to support offset correction feature
// 2010-11-17 added clear off driver's microstep count for each power off
// 2010-11-17 added post move offset correction to TskStpClkDirHold
// 2010-11-19 rewritten handling of clks in StpClkDirTccIrqHandlerClksUpdate
// 2010-11-19 added direction independent calculation in StpClkDirAbsGet
// 2010-11-19 rewrote low overflow handling in StpClkDirClksAbsGet
// 2010-11-19 rewrote low overflow handling in StpClkDirTccIrqHandlerClksUpdate
// 2010-11-19 optional offset correction feature being direction dependent now
// 2010-11-19 fixed StpClkDirClksAbsGet for clock range greather than 16bit
// 2010-11-19 fixed StpClkDirTccIrqHandlerClksUpdate for clock range > 16bit
// 2010-11-19 added high overflow handling in StpClkDirClksAbsGet
// 2010-11-19 added high overflow handling in StpClkDirTccIrqHandlerClksUpdate
// 2010-11-19 fixed calc of maximum clock by modulo in StpClkDirClksAbsGet
// 2010-11-19 fixed calc of max clock by modulo in StpClkDirTccIrqHandlerClksUpdate
// 2010-11-19 added new function StpClkDirModuloMaxClksGet
// 2010-11-24 optional offset correction feature ignores single offsets yet
// 2010-12-09 fixed IrqStpClkDirTccHandler to handle stop of continuous properly
// 2010-12-15 fixed IrqStpClkDirTccHandler to clear stop trigger after stop
// 2010-12-15 fixed IrqStpClkDirTccHandler to clear cycle count after stop
// 2010-12-17 added new function StpClkDirProfileAdjust
// 2011-02-03 added ulTimeout to all Dacrrcs085SetOutput(..) calls
// 2011-02-16 added error handling for Mcp2317SetOutput() in TskStpClkDirHold
// 2011-02-16 added error handling for Mcp2317SetOutput() in StpClkDirStart
// 2011-02-16 removed misplaced u32ClkCntDriver clear in StpClkDirStart
// 2011-02-17 peripherals are disabled now before new setup in StpClkDirStart
// 2011-02-17 fixed giving of SemStart in StpClkDirMove and StpClkDirStart
// 2011-02-17 fixed SemStart check in StpClkDirSpeedChange, StpClkDirQuit, StpClkDirBrk
// 2011-02-17 added acc to busy checking in StpClkDirSpeedChange
// 2011-02-23 enhanced StpClkDirBrk to use also on demand calculated brk ramps
// 2011-02-24 added SemRun to StpClkDirInit
// 2011-02-24 fixed error code for unit out of range in StpClkDirBrk
// 2011-02-24 fixed error code for unit out of range in StpClkDirSpeedChange
// 2011-02-24 fixed error code for unit out of range in StpClkDirSpeedChangeWait
// 2011-02-24 fixed u32PwmState check in StpClkDirBrk & StpClkDirSpeedChange(Wait)
// 2011-02-24 locked scheduler during clearing MbxStpClkDirBrk in StpClkDirBrk
// 2011-03-02 undo changes (on demand calculated brk ramps) in StpClkDirBrk
// 2011-03-02 added state forward change and update to StpClkDirPwmIrqHandler
// 2011-03-02 fixed StpClkDirSpeedChangeWait to allow pending for acc too
// 2011-03-02 added RESCHEDULE call before successfull return in StpClkDirStart
// 2011-03-17 renamed function StpClkDirMove to StpClkDirMoveMax with new para
// 2011-03-17 added wrapper function StpClkDirMove for compatibility
// 2011-03-17 added verification of ClkMax >= ulClkLow/High in StpClkDirMoveMax
// 2011-03-30 changed StpClkDirStart to use auxiliary function for direction set
// 2011-03-30 changed StpClkDirStart to use auxiliary function for power set
// 2011-03-30 changed StpClkDirStart to use auxiliary function for dac set
// 2011-03-30 added missing pre-delay for new dac setting in StpClkDirStart
// 2011-03-30 changed TskStpClkDirHold to use auxiliary function for power set
// 2011-03-30 changed TskStpClkDirHold to use auxiliary function for dac set
// 2011-03-30 fixed microstep counter clearing in StpClkDirPower for power on
// 2011-03-30 added missing pre-delay for new dac setting in StpClkDirPower
// 2011-03-30 changed StpClkDirPower to use auxiliary function for dac set
// 2011-03-30 renamed ucDacValueDelayMS to ucDacDelayMS
// 2011-03-30 added RESCHEDULE call to task mode of StpClkDirTccIrqHandlerStop
// 2011-03-30 added device available check in StpClkDirWait
// 2011-03-30 changed StpClkDir to give SemDone via TskStpClkDirHold by giving SemStop
// 2011-03-30 added RESCHEDULE call to TskStpClkDirHold
// 2011-03-30 removed RESCHEDULE call of TStpClkDirWait
// 2011-09-07 more condition checking to determine ready state
// 2011-09-07 StpClkDirTccIrqHandlerClksUpdate rewritten and enhanced for isr & task
// 2011-09-07 StpClkDirClksAbsGet rewritten to use StpClkDirTccIrqHandlerClksUpdate
// 2011-09-07 StpClkDirTccIrqHandler modified for new StpClkDirTccIrqHandlerClksUpdate
// 2011-09-08 secured resource access of StpClkDirClksPosOffsVal
// 2011-09-08 secured resource access of StpClkDirClksPosOffsMakePlus
// 2011-09-08 added dest check to trigger offset clear in StpClkDirTccIrqHandlerClksUpdate
// 2011-09-15 added delay ucResetDelayMS to TskStpClkDirHold
// 2011-09-15 skipping ucDacDelayMS for non-existing (null) pAuxDac in TskStpClkDirHold
// 2011-09-15 renamed StpClkDirPower to StpClkDirPowerEx and added wrapper
// 2011-09-15 changed StpClkDirPowerEx to support calls from StpClkDirStart
// 2011-09-15 added delay ucResetDelayMS to StpClkDirPowerEx
// 2011-09-15 skipping ucDacDelayMS for non-existing (null) pAuxDac in StpClkDirPowerEx
// 2011-09-15 added optional hardware driver restart in StpClkDirPowerEx for power enable
// 2011-09-15 changed StpClkDirStart to use StpClkDirPowerEx (dropped own redundant code)
// 2011-09-15 changed giving of SemStop in StpClkDirStart to non-conditional (always)
// 2011-09-15 locked u32ClkCntAbsolute/ulClkPosZeroOffset pair update in StpClkDirClksAbsSet
// 2011-09-15 locked position retrival in standby/ready state in StpClkDirClksAbsGet
// 2011-09-15 locked position retrival in StpClkDirClksGet
// 2011-09-15 locked position retrival in StpClkDirJobClksGet
// 2011-09-15 changed u32ClkTc init source depending on bIsr in StpClkDirTccIrqHandlerClksUpdate
// 2011-09-16 changed all os_mbx_send(_safe) calls to os_mbx_write calls
// 2011-09-21 changed TskStpClkDirHold to use StpClkDirPowerEx (dropped own redundant code)
// 2011-09-21 reworked semaphore control logic and comments in StpClkDirMoveMax
// 2011-09-21 reworked semaphore control logic and comments in _StpClkDirStart
// 2011-09-21 reworked move init and set-up in _StpClkDirStart for better IRQ safety
// 2011-09-21 moved giving of SemDone from StpClkDirTccIrqHandlerStop to TskStpClkDirHold
// 2011-09-21 added second chance write access to PWMC_CUPDR in StpClkDirPwmIrqHandler
// 2011-09-28 now using define STPCLKDIR_PWM_2NDCHANCEUPDATE_FIX in StpClkDirPwmIrqHandler
// 2011-09-28 added ulOffsetMsgCnt to StpClkDirInitEx
// 2011-09-28 rewrote offset calculation of TskStpClkDirHold after move
// 2011-09-28 added function StpClkDirClksAbsShiftGet to get diff of real/req pos
// 2011-09-29 added new function StpClkDirClksAbsShiftSet to shift required pos
// 2011-09-29 changed TskStpClkDirHold to use StpClkDirClksAbsShiftSet (dropped own code)
// 2011-09-29 renamed StpClkDirClksAbsShiftSet to StpClkDirClksAbsShiftSetEx
// 2011-09-29 added wrapper function StpClkDirClksAbsShiftSet
// 2011-11-16 StpClkDirTccIrqHandlerStop now clears bRun itsself for StpClkDirQuit
// 2011-11-16 added lost return code assigment in StpClkDirQuit for success return
// 2013-04-11 fixed function header's return data type comments
// 2013-04-24 fixed lost result bits handling in StpClkDirPowerEx
// 2013-08-21 fixed STPCLKDIR_PWM_2NDCHANCEUPDATE_FIX
// 2013-08-21 own and working version for STPCLKDIR_PWM_UPDATE_FIX
// 2013-08-21 check for true speed change on pwm period basis in StpClkDirSpeedChange
// 2013-08-21 fake success for STPCLKDIR_ERR_INVALID_CLKDIFF in StpClkDirSpeedChange for pwm period basis
// 2013-08-21 added quick check for speed change already done in StpClkDirSpeedChangeWait (allows multiple calls now)
// 2013-08-21 locked access to SStpClkDirRW[ucUnit].u32PwmCmrCupdr in StpClkDirHzGet
// 2013-08-21 added bChanged to _SStpClkDirRW
// 2013-08-22 changed delays/timeouts from milliseconds to ticks
// 2013-08-22 changed RESCHEDULE (ms) to RESCHEDULE_TICKS (ticks)
// 2013-08-21 fixed check for true speed change on pwm period basis in StpClkDirSpeedChange for both new-to/new-from
// 2014-10-16 fixed state indicator & irq control in StpClkDirPwmIrqHandler & StpClkDirTccIrqHandler
// 2014-10-16 added clearing of still available semaphore in StpClkDirSpeedChangeWait
// 2014-10-16 removed partially redundant brake code of StpClkDirBrk
// 2014-10-22 moved local bStop from StpClkDirTccIrqHandler to _SStpClkDirRW
// 2014-10-22 moved local usCycle from StpClkDirTccIrqHandler to _SStpClkDirRW
// 2014-10-22 added clearing of SStpClkDirRW[ucUnit].bStop in _StpClkDirStart
// 2014-10-22 added clearing of SStpClkDirRW[ucUnit].usCycle in _StpClkDirStart
// 2014-10-22 removed redundant reset of cycle counter in StpClkDirTccIrqHandler
// 2015-04-29 added comment details in TskStpClkDirHold for StpClkDirPowerEx call
// 2015-04-29 changed OS_MBX_INIT_EX to os_mbx_clear calls in _StpClkDirMbxClear
// 2015-04-30 added clear of last written pwm value copy in _StpClkDirStart
// 2015-04-30 added check for a complete pwm cycle/irq in StpClkDirQuit before hard stop
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <math.h>
#include <limits.h>
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\..\..\cpu\at91\pwm\pwm.h"
#include "..\..\..\..\cpu\at91\pwm\pwmext.h"
#include "..\..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"
#include "..\..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085.h"
#include "..\..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085ext.h"
#include "..\..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085data.h"

#include "stpclkdir.h"
#include "stpclkdirext.h"
#include "stpclkdirint.h"
#include "stpclkdirerr.h"
#include "stpclkdirdata.h"
#include "stpclkdirdev.h"

#include ".\auxiliary\stpclkdirauxdata.h"
#include ".\auxiliary\stpclkdirauxdev.h"
#include ".\auxiliary\stpclkdiraux.h"

// Consts
const ULONG ulUSHRT_CNT= (USHRT_MAX+ 1);

//----------------------------------------------------------------------------
// void IsrStpClkDirTcc/Pwm __irq
//----------------------------------------------------------------------------
// Each supported unit requires a function definition here
#if STPCLKDIR_UNIT_MAX > 0
   STPCLKDIR_ISRTCC_FUNC(0);
   STPCLKDIR_ISRPWM_FUNC;
#endif

#if STPCLKDIR_UNIT_MAX > 1
   STPCLKDIR_ISRTCC_FUNC(1);
#endif

#if STPCLKDIR_UNIT_MAX > 2
   STPCLKDIR_ISRTCC_FUNC(2);
#endif

//----------------------------------------------------------------------------
// UCHAR StpClkDirInit
//----------------------------------------------------------------------------
UCHAR StpClkDirInit(UCHAR ucUnit, USHORT usDacValueHold, USHORT usDacValueRun)
{
   return (StpClkDirInitEx(ucUnit, usDacValueHold, usDacValueRun, &StpClkDirAuxDirAt91Pio, &StpClkDirAuxPwrMcp2317, &StpClkDirAuxDacRrcs085));
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirInitEx
//----------------------------------------------------------------------------
UCHAR StpClkDirInitEx(UCHAR ucUnit, USHORT usDacValueHold, USHORT usDacValueRun, ULONG (*pAuxDir)(UCHAR ucUnit, bool bClockwise), ULONG (*pAuxPwr)(UCHAR ucUnit, bool bEnable), ULONG (*pAuxDac)(UCHAR ucUnit, bool bRun))
{
   UCHAR ucResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      if (SStpClkDirRO[ucUnit].u16Timeout)
      {
         // Prepare timer channel (TCLKx: input clock (inverted PWMx to avoid glitch of anded clock and TIOA))
      
         // Peripheral clock
         AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, 1 << SStpClkDirRO[ucUnit].u32TcbPeriphId);
         
         // Timer clock source
         SStpClkDirRO[ucUnit].pTcbBase->TCB_BMR &= ~((U32)SStpClkDirRO[ucUnit].u8TcbBmrMask);
         SStpClkDirRO[ucUnit].pTcbBase->TCB_BMR |= ((U32)SStpClkDirRO[ucUnit].u8TcbBmrLsb & (U32)SStpClkDirRO[ucUnit].u8TcbBmrMask);
         
         // Mode
         // Caution! RB is disabled by default because set as external trigger input!
         // Change external trigger to anything else than AT91C_TC_EEVT_TIOB to use RB with IRQs! (refer to AT91SAM7X manual 32.5.12 page 404 and page 412 note 1)
          
         // Enable hardware control for TIOA as clock switch based on SWTRG and RC (counter max) (for single run including drop of TIOA)
         SStpClkDirRO[ucUnit].pTccBase->TC_CMR= (  /* ext clock source pin and */ ((U32)SStpClkDirRO[ucUnit].u16TccCmrLsb & (AT91C_TC_CLKS | AT91C_TC_EEVT)) | \
                                                   /* ext clock as ext trg     */ AT91C_TC_EEVTEDG_RISING | \
                                                   /* timer mode               */ AT91C_TC_WAVESEL_UP_AUTO | AT91C_TC_WAVE | \
                                                   /* tioa coupling            */ AT91C_TC_ACPA_NONE | AT91C_TC_ACPC_CLEAR | AT91C_TC_AEEVT_NONE | AT91C_TC_ASWTRG_SET | \
                                                   /* tiob coupling            */ AT91C_TC_BCPB_SET | AT91C_TC_BCPC_CLEAR | AT91C_TC_BEEVT_NONE | AT91C_TC_BSWTRG_NONE | \
                                                   /* stop after single cycle  */ AT91C_TC_CPCSTOP
                                                );
   
               
         // Clear compare regs
         SStpClkDirRO[ucUnit].pTccBase->TC_RA= 0;
         SStpClkDirRO[ucUnit].pTccBase->TC_RB= 0;
         SStpClkDirRO[ucUnit].pTccBase->TC_RC= 0;
         
         // Init auxiliaries
         SStpClkDirRW[ucUnit].pAuxDir= pAuxDir;
         SStpClkDirRW[ucUnit].pAuxPwr= pAuxPwr;
         SStpClkDirRW[ucUnit].pAuxDac= pAuxDac;
         
         // Init position
         SStpClkDirRW[ucUnit].ulClkId= 0;
         SStpClkDirRW[ucUnit].ulClkIdPrevious= 0;
         SStpClkDirRW[ucUnit].lOffsetMean= 0;
         SStpClkDirRW[ucUnit].u32ClkCntDriver= 0;
         SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute= 0;
         SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset= 0;
         SStpClkDirRW[ucUnit].bRun= FALSE;
         SStpClkDirRW[ucUnit].bDirection= ((SStpClkDirRO[ucUnit].ucFlags & STPCLKDIR_FLAGS_INVERSE)) ? (STPCLKDIR_COUNTERCLOCKWISE) : (STPCLKDIR_CLOCKWISE);
         SStpClkDirRW[ucUnit].bChanged= FALSE;
         SStpClkDirRW[ucUnit].ulOffsetMsgCnt= 0;
         SStpClkDirRW[ucUnit].ulTskStpClkDirHold= 0;
   
         // Init DAC values
         SStpClkDirRW[ucUnit].SPermanent.usDacValueHold= usDacValueHold;
         SStpClkDirRW[ucUnit].SPermanent.usDacValueRun= usDacValueRun;
         
         // Initial timer IRQ mask
         SStpClkDirRW[ucUnit].u32TccImr= (AT91C_TC_CPAS | AT91C_TC_CPBS | AT91C_TC_CPCS | AT91C_TC_ETRGS);
         AT91F_TC_InterruptDisable(SStpClkDirRO[ucUnit].pTccBase, SStpClkDirRW[ucUnit].u32TccImr);
         
         // IRQ ctrl
         AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, SStpClkDirRO[ucUnit].u32TcbPeriphId, 0, 0, IRQ_CAST(SStpClkDirRO[ucUnit].IsrTcc));
         AT91F_AIC_EnableIt(AT91C_BASE_AIC, SStpClkDirRO[ucUnit].u32TcbPeriphId);
         
         // Enable (but not arm!) timer
         SStpClkDirRO[ucUnit].pTccBase->TC_CCR |= AT91C_TC_CLKEN;          
         
         
         // Prepare pwm channel
         // Setup done by PwmInit (pwm.c) with channel IRQs kept disabled (enable them using AT91F_PWMC_InterruptEnable()) and channel mode/period/duty kept clear
         
         // Prepare OS tools
         os_sem_init(SStpClkDirRW[ucUnit].SemStart, 1);
         os_sem_init(SStpClkDirRW[ucUnit].SemDone, 1);
         os_sem_init(SStpClkDirRW[ucUnit].SemHold, 0);
         os_sem_init(SStpClkDirRW[ucUnit].SemRun, 0);
         
         OS_MBX_INIT_EX(MbxStpClkDirOffset[ucUnit]);
         OS_MBX_INIT_EX(MbxStpClkDirState[ucUnit]);
         OS_MBX_INIT_EX(MbxStpClkDirAcc[ucUnit]);
         OS_MBX_INIT_EX(MbxStpClkDirBrk[ucUnit]);
         
         // Tasks
         SStpClkDirTsk[ucUnit].ucUnit= ucUnit;
         SStpClkDirRW[ucUnit].tidTskStpClkDirHold= OS_TSK_CREATE_USER_EX(TskStpClkDirHold, SStpClkDirRO[ucUnit].ucTskPriMsg, StkTskStpClkDirHold[ucUnit], &SStpClkDirTsk[ucUnit]);
         
         ucResult= (SStpClkDirRW[ucUnit].tidTskStpClkDirHold != 0) ? (1) : (0);
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// TskStpClkDirHold
//----------------------------------------------------------------------------
__task void TskStpClkDirHold(void *pvParam)
{
   struct _SStpClkDirTsk *pSStpClkDirTsk= (struct _SStpClkDirTsk *)pvParam;
     
   /* static */ LONG lOffset;
   /* static */ LONG lValue;
   
   FOREVER
   {
      // Pend for movement stop
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[pSStpClkDirTsk->ucUnit].SemHold, OS_T_FOREVER)))
      {
         // Set hold current and optional power off (bLocked= TRUE)
         SStpClkDirRW[pSStpClkDirTsk->ucUnit].ulTskStpClkDirHold= (StpClkDirPowerEx(pSStpClkDirTsk->ucUnit, !(SStpClkDirRO[pSStpClkDirTsk->ucUnit].ucFlags & STPCLKDIR_FLAGS_AUTOPOWEROFF), FALSE, TRUE, OS_T_NONE) != 0);

         // Determine mean offset of all collected offsets (already limited by EncGapDiskIrqHandler) during movement
         
         // Init
         lOffset= 0;
         SStpClkDirRW[pSStpClkDirTsk->ucUnit].ulOffsetMsgCnt= 0;

         // Minimum pending on offset mailbox
         if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxStpClkDirOffset[pSStpClkDirTsk->ucUnit], OS_MBX_VALUE_GET(lValue), 1)))
         {
            SStpClkDirRW[pSStpClkDirTsk->ucUnit].ulOffsetMsgCnt++;
            
#if STPCLKDIR_OFFSETBYDIRECTION > 0
            lOffset= ((SStpClkDirRO[pSStpClkDirTsk->ucUnit].ucFlags & STPCLKDIR_FLAGS_INVERSE)) ? (-lValue) : (lValue);
#else
            lOffset= lValue;
#endif
            // Minimum pending on offset mailbox
            if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxStpClkDirOffset[pSStpClkDirTsk->ucUnit], OS_MBX_VALUE_GET(lValue), 1)))
            {
               do
               {
#if STPCLKDIR_OFFSETBYDIRECTION > 0
                  lOffset= (lOffset+ (((SStpClkDirRO[pSStpClkDirTsk->ucUnit].ucFlags & STPCLKDIR_FLAGS_INVERSE)) ? (-lValue) : (lValue))) >> 1;
#else
                  lOffset= (lOffset+ lValue) >> 1;
#endif
                  SStpClkDirRW[pSStpClkDirTsk->ucUnit].ulOffsetMsgCnt++;
                  
               // Minimum pending on offset mailbox
               } while (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxStpClkDirOffset[pSStpClkDirTsk->ucUnit], OS_MBX_VALUE_GET(lValue), 1)));

               // Store average offset for later retrival
               SStpClkDirRW[pSStpClkDirTsk->ucUnit].lOffsetMean= lOffset;

               // Determine current position from required position and offset(s) (refer to StpClkDirTccIrqHandlerClksUpdate)
               if (lOffset)
               {
#if STPCLKDIR_OFFSETAUTOSHIFT > 0
                  // Shift current internal position
                  SStpClkDirRW[pSStpClkDirTsk->ucUnit].ulTskStpClkDirHold= SStpClkDirRW[pSStpClkDirTsk->ucUnit].ulTskStpClkDirHold && StpClkDirClksAbsShiftSetEx(pSStpClkDirTsk->ucUnit, lOffset, TRUE, SStpClkDirRO[pSStpClkDirTsk->ucUnit].u16Timeout);
#endif
               }
            }
         }

         // Signal hold stage complete
         os_sem_give(SStpClkDirRW[pSStpClkDirTsk->ucUnit].SemDone);
         
         // Signal start stage available again
         if (!OS_SEM_TAKE_CHK(os_sem_check(SStpClkDirRW[pSStpClkDirTsk->ucUnit].SemStart, FALSE)))
         {
            os_sem_give(SStpClkDirRW[pSStpClkDirTsk->ucUnit].SemStart);
         }
         UNLOCK;
         
         RESCHEDULE_TICKS(0);
      }
   }
   
   DETACH;
}

//----------------------------------------------------------------------------
// USHORT StpClkDirPower
//----------------------------------------------------------------------------
USHORT StpClkDirPower(UCHAR ucUnit, bool bPower, U16 u16Timeout)
{
   return (StpClkDirPowerEx(ucUnit, bPower, bPower, FALSE, u16Timeout));
}

//----------------------------------------------------------------------------
// USHORT StpClkDirPowerEx
//----------------------------------------------------------------------------
__inline USHORT StpClkDirPowerEx(UCHAR ucUnit, bool bPower, bool bRun, bool bLocked, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   UCHAR ucDelay;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Lock data (start stage)
      if ((!bLocked) ? (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout))) : (TRUE))
      {
         // Lock data (finish stage)
         if ((!bLocked) ? (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout))) : (TRUE))
         {
            // Check ready condition        
            if ((!bLocked) ? (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun) : (TRUE))
            {                       
               // Prepare ref voltage hold
               usResult= (SStpClkDirRW[ucUnit].pAuxDac) ? (SStpClkDirRW[ucUnit].pAuxDac(ucUnit, bRun) != 0) : (1);
               if (usResult || !bPower)
               {
                  // Delay AFTER new ref voltage / BEFORE new power state
                  if (SStpClkDirRW[ucUnit].pAuxDac || SStpClkDirRW[ucUnit].pAuxPwr)
                  {
                     ucDelay= _umax(SStpClkDirRO[ucUnit].ucDacDelay, SStpClkDirRO[ucUnit].ucResetDelay);
                     if (ucDelay)
                     {
                        os_dly_wait(ucDelay);
                     }
                  }
                  
                  // Lock/power off stepper driver
   
#if STPCLKDIR_POWERONRESET > 0               
                  if (bPower)
                  {
                     // Force hardware driver reset by powering off and on again
                     usResult= (SStpClkDirRW[ucUnit].pAuxPwr) ? (SStpClkDirRW[ucUnit].pAuxPwr(ucUnit, FALSE) != 0) : (1);
                     
                     // Delay AFTER new power state
                     if (SStpClkDirRO[ucUnit].ucResetDelay)
                     {
                        os_dly_wait(SStpClkDirRO[ucUnit].ucResetDelay);
                     }
                     
                     if (usResult)
                     {            
                        // Powering off driver clears its internal microstep count
                        SStpClkDirRW[ucUnit].u32ClkCntDriver= 0;
                     }
                  }

                  if (usResult || !bPower)
#else
                  if (usResult)
#endif
                  {
                     usResult= (SStpClkDirRW[ucUnit].pAuxPwr) ? (SStpClkDirRW[ucUnit].pAuxPwr(ucUnit, bPower) != 0) : (1);
                     if (usResult)
                     {
                        // Delay AFTER new power state
                        if (SStpClkDirRO[ucUnit].ucResetDelay)
                        {
                           os_dly_wait(SStpClkDirRO[ucUnit].ucResetDelay);
                        }
                        
                        if (usResult && !bPower)
                        {            
                           // Powering off driver clears its internal microstep count
                           SStpClkDirRW[ucUnit].u32ClkCntDriver= 0;
                        }
                     }
                  }
               }
            }
            
            if (!bLocked) os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
                  
         if (!bLocked) os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }
   
   return (usResult);
}

//----------------------------------------------------------------------------
// bool StpClkDirProfileAdjust
//----------------------------------------------------------------------------
bool StpClkDirProfileAdjust(UCHAR ucUnit, ULONG ulClks, ULONG *pulClksAcc, ULONG *pulClksBrk, ULONG ulClkLowHz, ULONG *pulClkHighHz)
{
   bool bResult= FALSE;
   
   register ULONG ulClksAcc= 0;
   register ULONG ulClksBrk= 0;
   register ULONG ulHelp= 0;
   register ULONG ulClkDeltaHz= 0;
   
   register ULONG ulClksCmp;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      if (pulClksAcc && pulClksBrk && pulClkHighHz)
      {
         // Adjust profile when ramp sum doesn't fit into distance
         if (*pulClksAcc+ *pulClksBrk > ulClks)
         {
            // Build ramps requires minimum distance
            if (ulClks >= STPCLKDIR_CLKSRUN_MIN)
            {
               ulClkDeltaHz= ABS(ulClkLowHz, *pulClkHighHz)/ *pulClksAcc;
               ulClksCmp= ulClks- 1;
               
               while ((ulClksAcc+ ulClksBrk) < ulClksCmp)
               {
                  if (ulHelp >= *pulClksAcc)
                  {
                     ulClksBrk++;
                     ulHelp -= *pulClksAcc;
                  }
                  else
                  {
                     ulClksAcc++;
                     ulHelp += *pulClksBrk;
                  }
               }
               
               // Have the new ramp ratio as close as possible to the original ratio
               if (*pulClksAcc <= *pulClksBrk)
               {
                  ulClksAcc++;
               }
               else
               {
                  ulClksBrk++;
               }
               
               // Ramp minimum check and fix attempt (fall back to creep move when fix cannot applied)
               if (ulClksBrk < STPCLKDIR_CLKSBRK_MIN)
               {
                  ulHelp= STPCLKDIR_CLKSBRK_MIN- ulClksBrk;
                  if (ulClksAcc >= (STPCLKDIR_CLKSACC_MIN+ ulHelp))
                  {
                     ulClksAcc -= ulHelp;
                     ulClksBrk= STPCLKDIR_CLKSBRK_MIN;
                  }
                  else
                  {
                     ulClksAcc= 0;
                     ulClksBrk= 0;
                  }
               }

               // Ramp minimum check and fix attempt (fall back to creep move when fix cannot applied)
               if (ulClksAcc < STPCLKDIR_CLKSACC_MIN)
               {
                  ulHelp= STPCLKDIR_CLKSACC_MIN- ulClksAcc;
                  if (ulClksBrk >= (STPCLKDIR_CLKSBRK_MIN+ ulHelp))
                  {
                     ulClksBrk -= ulHelp;
                     ulClksAcc= STPCLKDIR_CLKSACC_MIN;
                  }
                  else
                  {
                     ulClksAcc= 0;
                     ulClksBrk= 0;
                  }
               }
            }
            else
            {
               // Set up creep move (ulClksAcc= 0, ulClksBrk= 0, ulClkHighHz= ulClkLowHz) via param init
            }
               
            if ((ulClksAcc+ ulClksBrk) <= ulClks)
            {
               *pulClkHighHz= ulClkLowHz+ ((*pulClksAcc)* ulClkDeltaHz);
               *pulClksAcc= ulClksAcc;
               *pulClksBrk= ulClksBrk;
               
               bResult= TRUE;
            }
         }
         else
         {
            bResult= TRUE;
         }
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirMove
//----------------------------------------------------------------------------
UCHAR StpClkDirMove(UCHAR ucUnit, bool bDir, ULONG ulClks, USHORT usClksAcc, USHORT usClksBrk, ULONG ulClkLowHz, ULONG ulClkHighHz, USHORT usMicroSteps, bool bContinous, UCHAR *pucError, U16 u16Timeout)
{
   return (StpClkDirMoveMax(ucUnit, bDir, ulClks, usClksAcc, usClksBrk, ulClkLowHz, ulClkHighHz, ulClkHighHz, usMicroSteps, bContinous, pucError, u16Timeout));
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirMoveMax
//----------------------------------------------------------------------------
UCHAR StpClkDirMoveMax(UCHAR ucUnit, bool bDir, ULONG ulClks, USHORT usClksAcc, USHORT usClksBrk, ULONG ulClkLowHz, ULONG ulClkHighHz, ULONG ulClkMaxHz, USHORT usMicroSteps, bool bContinous, UCHAR *pucError, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   UCHAR ucError= 0;
   
   bool bQueueState;

   UCHAR ucCpreHigh;
   U16 u16PrdHigh;
   U16 u16DtyMax;
   UCHAR ucCpreStart;
   U16 u16PrdStart;
   UCHAR ucCpre;
   U16 u16Prd;

   ULONG ulClkDeltaHz;
   ULONG ulClkHz;

   ULONG ulClksMove;
   USHORT usRepeat;
   USHORT usIndex;
#if STPCLKDIR_FULLSTEPSHIFT > 0
   ULONG ulClksRun;
   ULONG ulClkDriver;
   
   USHORT usClkMicro;
   USHORT usClkHalfMicro;
   USHORT usHalfMicroSteps;
   SHORT sClkAdjustOffset;
#endif
     
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {  
      // Lock data (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Lock data (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               if (_StpClkDirMbxClear(ucUnit, u16Timeout))
               {               
                  // Check clks max is max of clks
                  if ((ulClkMaxHz >= ulClkLowHz) && (ulClkMaxHz >= ulClkHighHz))
                  {
                     // Check acc clks (= frequency) limits
                     if ((ulClkLowHz == ulClkHighHz) || ((ulClkLowHz != ulClkHighHz) && (usClksAcc >= STPCLKDIR_CLKSACC_MIN) && (usClksAcc <= STPCLKDIR_MBXCNTACC)))
                     {
                        // Check brk clks limits
                        if ((ulClkLowHz == ulClkHighHz) || ((ulClkLowHz != ulClkHighHz) && (usClksBrk >= STPCLKDIR_CLKSBRK_MIN) && (usClksBrk <= STPCLKDIR_MBXCNTBRK)))
                        {
                           // Check clks summary
                           if (ulClks >= (usClksAcc+ usClksBrk))
                           {
                              // Check space of acc queue
                              if (StpClkDirPwmQueueFree(ucUnit, AT91C_TC_CLKSTA) >= usClksAcc)
                              {
                                 // Check space of brk queue
                                 if (StpClkDirPwmQueueFree(ucUnit, AT91C_TC_CPBS) >= usClksBrk)
                                 {
#if STPCLKDIR_FULLSTEPSHIFT > 0
                                    // Check for clks adjustment option request
                                    if (usMicroSteps > 1)
                                    {
                                       // Precalculate often used value
                                       usHalfMicroSteps= (usMicroSteps >> 1);
                                       ulClksRun= ulClks- (usClksAcc+ usClksBrk);
         
                                       // Determine total clock count of driver plus these new clocks after powering on
                                       ulClkDriver= (SStpClkDirRW[ucUnit].u32ClkCntDriver+ ulClks) % StpClkDirModuloClksGet(ucUnit);
                                       
                                       // Determine new shift delta for new position
                                       usClkMicro= ulClkDriver% usMicroSteps;
                                       usClkHalfMicro= ulClkDriver% usHalfMicroSteps;
                                       sClkAdjustOffset= 0;
                                       
                                       // Example for 1/16              L  O  W  E  R            U  P  P  E  R
                                       // Clock % usMicroSteps    : 00 01 02 03 04 05 06 07  08 09 10 11 12 13 14 15
                                       // Clock % usMicroSteps/ 2 : 00 01 02 03 04 05 06 07  00 01 02 03 04 05 06 07
                                       // Standard case for 1/16  : 00 -1 -2 -3 -4 -5 -6 -7  +8 +7 +6 +5 +4 +3 +2 +1
                                       // Lower dec to inc change : 16 15 14 13 12 11 10 09  +8 +7 +6 +5 +4 +3 +2 +1
                                       // Upper inc to dec change : 00 -1 -2 -3 -4 -5 -6 -7  -8 -9 -A -B -C -D -E -F
                                       
                                       if (usClkMicro < usHalfMicroSteps)
                                       {
                                          // Decrease to shift to previous full step position
                                          sClkAdjustOffset= -usClkHalfMicro;
                                          
                                          // Only the run clocks are changed here!
                                          if (ulClksRun < -sClkAdjustOffset)
                                          {
                                             // Change to increase to next full step position
                                             sClkAdjustOffset= usMicroSteps- usClkHalfMicro;
                                          }
                                       }
                                       else
                                       {
                                          // Increase to shift to next full step position
                                          sClkAdjustOffset= usHalfMicroSteps- usClkHalfMicro;
                                          
                                          // The total clocks must fit in unsigned 32bit!
                                          if (ULONG_MAX- ulClks < sClkAdjustOffset)
                                          {
                                             // Change to decrease to previous full step position
                                             sClkAdjustOffset= -(usHalfMicroSteps+ usClkHalfMicro);
                                          }
                                       }
         
                                       // Apply the shift offset
                                       
                                       if (sClkAdjustOffset != 0)
                                       {
                                          if (sClkAdjustOffset < 0)
                                          {
                                             if (ulClks > -sClkAdjustOffset)
                                             {
                                                // Decrease
                                                ulClks += sClkAdjustOffset;
                                             }
                                          }
                                          
                                          if (sClkAdjustOffset > 0)
                                          {
                                             if (ULONG_MAX- ulClks >= sClkAdjustOffset)
                                             {
                                                // Increase
                                                ulClks += sClkAdjustOffset;
                                             }
                                          }
                                       }
                                    }
#endif
         
                                    // Determine repeats (rollovers) of the 16 bit hardware counter
                                    if (ulClks <= USHRT_MAX)
                                    {
                                       ulClksMove= (USHORT)ulClks;
                                       usRepeat= 0;
                                    }
                                    else
                                    {
                                       // Step# 0x0000 to step# 0xFFFF plus clear clk= 0x10000 (USHRT_MAX+ 1) steps per rollover
                                       for (usRepeat= 0, ulClksMove= ulClks; ulClksMove >= ulUSHRT_CNT; usRepeat++, ulClksMove -= ulUSHRT_CNT);
                                    }
                                 
                                    // Determine cpre, prd and dty for max and run clock and check hardware support for it
                                    if (PwmCalcCprePrdDty(ulClkHighHz, SStpClkDirRO[ucUnit].ulClkStableHz, &ucCpreHigh, &u16PrdHigh, NULL) &&
                                        PwmCalcCprePrdDty(ulClkMaxHz, SStpClkDirRO[ucUnit].ulClkStableHz, NULL, NULL, &u16DtyMax))
                                    {
                                       // Determine cpre and prd for start and check hardware support for it
                                       if (PwmCalcCprePrdDty(ulClkLowHz, SStpClkDirRO[ucUnit].ulClkStableHz, &ucCpreStart, &u16PrdStart, NULL))
                                       {
                                          // 1st value set for acc is start=stop speed (already calculated by hardware support check)
                                          //-1st value for acc is set directly to pwm - no need to queue it again!!
                                          //-bQueueState= StpClkDirPwmQueue(ucUnit, AT91C_TC_CLKSTA, (ucCpreStart | AT91C_PWMC_CPD), u16PrdStart, SStpClkDirRO[ucUnit].u16Timeout);
                                          bQueueState= TRUE;                                 
                                          if (bQueueState)
                                          {       
                                             if (usClksAcc > 0)
                                             {
                                                ulClkDeltaHz= (ulClkHighHz- ulClkLowHz)/ (usClksAcc- 0);
                                                
                                                // Generate cpre and prd values for acc ramp (2nd and up)
                                                for (ulClkHz= ulClkLowHz- ulClkDeltaHz, usIndex= 0; (usIndex < (usClksAcc- 1)) && (bQueueState); usIndex++, ulClkHz += ulClkDeltaHz)
                                                {
                                                   // Determine cpre and prd for given clock and check hardware support for it (just skip unsupported clocks)
                                                   if (PwmCalcCprePrdDty(ulClkHz, SStpClkDirRO[ucUnit].ulClkStableHz, &ucCpre, &u16Prd, NULL))
                                                   {
                                                      bQueueState= StpClkDirPwmQueue(ucUnit, AT91C_TC_CLKSTA, (ucCpre | AT91C_PWMC_CPD), u16Prd, SStpClkDirRO[ucUnit].u16Timeout);
                                                   }
                                                }
                                             }
                                             
                                             if (bQueueState)
                                             {
                                                if (usClksBrk > 0)
                                                {
                                                   // Last value set for acc is run speed (prevent any rounding errors)
                                                   bQueueState= StpClkDirPwmQueue(ucUnit, AT91C_TC_CLKSTA, (ucCpreHigh | AT91C_PWMC_CPD), u16PrdHigh, SStpClkDirRO[ucUnit].u16Timeout);
                                                }
                                                
                                                if (bQueueState)
                                                {
                                                   if (usClksBrk > 0)
                                                   {
                                                      ulClkDeltaHz= (ABS(ulClkHighHz, ulClkLowHz))/ (usClksBrk- 1);
                                                      
                                                      // Generate cpre and prd values for brk ramp
                                                      for (ulClkHz= ulClkHighHz- ulClkDeltaHz, usIndex= 0; (usIndex < (usClksBrk- 1)) && (bQueueState); usIndex++, ulClkHz -= ulClkDeltaHz)
                                                      {
                                                         // Determine cpre and prd for given clock and check hardware support for it (just skip unsupported clocks)
                                                         if (PwmCalcCprePrdDty(ulClkHz, SStpClkDirRO[ucUnit].ulClkStableHz, &ucCpre, &u16Prd, NULL))
                                                         {
                                                            bQueueState= StpClkDirPwmQueue(ucUnit, AT91C_TC_CPBS, (ucCpre | AT91C_PWMC_CPD), u16Prd, SStpClkDirRO[ucUnit].u16Timeout);
                                                         }
                                                      }
                                                   }
                                                   
                                                   if (bQueueState)
                                                   {
                                                      if (usClksBrk > 0)
                                                      {
                                                         // Last value set for brk is start=stop speed (prevent any rounding errors)
                                                         bQueueState= StpClkDirPwmQueue(ucUnit, AT91C_TC_CPBS, (ucCpreStart | AT91C_PWMC_CPD /*| AT91C_PWMC_CPOL*/), u16PrdStart, SStpClkDirRO[ucUnit].u16Timeout);
                                                      }
                                                      
                                                      if (bQueueState)
                                                      {
                                                         // Start the movement
                                                         ucResult= _StpClkDirStart(ucUnit, bDir, usClksAcc, ulClksMove- usClksBrk, ulClksMove, usRepeat, bContinous, ucCpreStart, u16PrdStart, u16DtyMax, u16Timeout);
                                                         if (!ucResult)
                                                         {
                                                            // start failed
                                                            ucError= STPCLKDIR_ERR_START;
                                                         }
                                                      }
                                                      else
                                                      {
                                                         // brk queue error (last value)
                                                         ucError= STPCLKDIR_ERR_SPACE_QBRKLAST;
                                                      }
                                                   }
                                                   else // brk queue error (2nd and up value)
                                                   {
                                                      ucError= STPCLKDIR_ERR_SPACE_QBRK2NDFF;
                                                   }
                                                }
                                                else // acc queue error (last value)
                                                {
                                                   ucError= STPCLKDIR_ERR_SPACE_QACCLAST;
                                                }
                                             }
                                             else // acc queue error (2nd and up value)
                                             {
                                                ucError= STPCLKDIR_ERR_SPACE_QACC2NDFF;
                                             }
                                          }
                                          else // acc queue error (1st value)
                                          {
                                             ucError= STPCLKDIR_ERR_SPACE_QACC1ST;
                                          }        
                                       }
                                       else // start=stop speed unsupported
                                       {
                                          ucError= STPCLKDIR_ERR_INVALID_CLKLOW;
                                       }
                                    }
                                    else // run speed unsupported
                                    {
                                       ucError= STPCLKDIR_ERR_INVALID_CLKHIGH;
                                    }
                                 }
                                 else // not enough space in brk queue
                                 {
                                    ucError= STPCLKDIR_ERR_SPACE_QBRK;
                                 }
                              }
                              else // not enough space in acc queue
                              {
                                 ucError= STPCLKDIR_ERR_SPACE_QACC;
                              }
                           }
                           else // less clks than sum of acc+brk
                           {
                              ucError= STPCLKDIR_ERR_RANGE_CLKS;
                           }
                        }
                        else // clksbrk out of range
                        {
                           ucError= STPCLKDIR_ERR_RANGE_CLKSBRK;
                        }
                     }
                     else // clksacc out of range
                     {
                        ucError= STPCLKDIR_ERR_RANGE_CLKSACC;
                     }
                  }
                  else // clksmax less than clkslow or clkshigh
                  {
                     ucError= STPCLKDIR_ERR_RANGE_CLKS;
                  }
               }
               else // mbx clear failed
               {
                  ucError= STPCLKDIR_ERR_MBX_CLEAR;
               }
            }
            else // unit not ready (hw check)
            {
               ucError= STPCLKDIR_ERR_STATE_UNITHW;
            }

            if (!ucError)
            {
               // Unlock SemStart & SemDone by TskStpClkDirHold (triggered by SemHold)
            }
            else
            {
               // Signal hold stage complete
               os_sem_give(SStpClkDirRW[ucUnit].SemDone);
            }
         }
         else // unit not ready (SemDone check) - finish stage
         {
            ucError= STPCLKDIR_ERR_STATE_UNITOS;
         }
         
         if (!ucError)
         {
            // Unlock SemStart & SemDone by TskStpClkDirHold (triggered by SemHold)
         }
         else
         {
            // Signal start stage available again
            os_sem_give(SStpClkDirRW[ucUnit].SemStart);
         }         
      }
      else // unit not ready (SemStart check) - start stage
      {
         ucError= STPCLKDIR_ERR_STATE_UNITOS;
      }
   }
   else // unit out of range
   {
      ucError= STPCLKDIR_ERR_RANGE_UNIT;
   }

   if (pucError) *pucError= ucError;

   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirSpeedChange
//----------------------------------------------------------------------------
UCHAR StpClkDirSpeedChange(UCHAR ucUnit, USHORT usClksAcc, ULONG ulClkFromHz, ULONG ulClkToHz, UCHAR *pucError, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   UCHAR ucError= 0;
   
   bool bQueueState;
   
   UCHAR ucCpreMax;
   U16 u16PrdMax;
   U16 u16DtyMax;
   UCHAR ucCpreStart;
   U16 u16PrdStart;
   UCHAR ucCpre;
   U16 u16Prd;
   U32 u32PwmCmrCupdr;
   
   ULONG ulClkDeltaHz;
   ULONG ulClkHz;
   
   USHORT usIndex;
   
   ULONG ulValue;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check for current movement (lock data of start stage should fail)
      if (!OS_SEM_TAKE_CHK(os_sem_check(SStpClkDirRW[ucUnit].SemStart, TRUE)))
      {         
         // Check busy condition
         if (SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId)
         {
            // Check for active run (acc done yet, no brk invoked)
            if (SStpClkDirRW[ucUnit].u32PwmState & AT91C_TC_CPAS)
            {  
               // Check acc limits
               if ((usClksAcc >= STPCLKDIR_CLKSACC_MIN) && (usClksAcc <= STPCLKDIR_MBXCNTACC))
               {
                  // Check for true speed change (from/to)
                  if (ulClkFromHz != ulClkToHz)
                  {
                     // Check space of queue acc
                     if (StpClkDirPwmQueueFree(ucUnit, AT91C_TC_CLKSTA) >= usClksAcc)
                     {                  
                        // Determine cpre, prd and dty for max=run clock and check hardware support for it
                        if (PwmCalcCprePrdDty(ulClkToHz, SStpClkDirRO[ucUnit].ulClkStableHz, &ucCpreMax, &u16PrdMax, &u16DtyMax))
                        {
                           // Determine cpre and prd for start and check hardware support for it
                           if (PwmCalcCprePrdDty(ulClkFromHz, SStpClkDirRO[ucUnit].ulClkStableHz, &ucCpreStart, &u16PrdStart, NULL))
                           {
                              // Determine current (last set) speed on pwm period basis
                              LOCKED(u32PwmCmrCupdr= SStpClkDirRW[ucUnit].u32PwmCmrCupdr);
                              ucCpre= (UCHAR)((u32PwmCmrCupdr & BITS_ULONGHX) >> 16);
                              u16Prd= (U16)(u32PwmCmrCupdr & BITS_ULONGLX);
                           
                              // Check for true speed change (current/new-to, current/new-from) on pwm period basis
                              if (((ucCpreMax != ucCpre) || (u16PrdMax != u16Prd)) || ((ucCpreStart != ucCpre) || (u16PrdStart != u16Prd)))
                              {                              
                                 // 1st value set for acc is start=stop speed (already calculated by hardware support check)
                                 bQueueState= StpClkDirPwmQueue(ucUnit, AT91C_TC_CLKSTA, (ucCpreStart | AT91C_PWMC_CPD), u16PrdStart, SStpClkDirRO[ucUnit].u16Timeout);
                                 if (bQueueState)
                                 {
                                    ulClkDeltaHz= (ulClkToHz > ulClkFromHz) ? (ulClkToHz- ulClkFromHz) : (ulClkFromHz- ulClkToHz);
                                    ulClkDeltaHz /= (usClksAcc- 1);
               
                                    // Generate cpre and prd values for acc ramp (2nd and up)
                                    for (ulClkHz= ulClkFromHz- ulClkDeltaHz, usIndex= 0; (usIndex < (usClksAcc- 2)) && (bQueueState); usIndex++, ulClkHz += ulClkDeltaHz)
                                    {
                                       // Determine cpre and prd for given clock and check hardware support for it (just skip unsupported clocks)
                                       if (PwmCalcCprePrdDty(ulClkHz, SStpClkDirRO[ucUnit].ulClkStableHz, &ucCpre, &u16Prd, NULL))
                                       {
                                          bQueueState= StpClkDirPwmQueue(ucUnit, AT91C_TC_CLKSTA, (ucCpre | AT91C_PWMC_CPD), u16Prd, SStpClkDirRO[ucUnit].u16Timeout);
                                       }
                                    }
                                    
                                    if (bQueueState)
                                    {
                                       // Last value set for acc is run speed (prevent any rounding errors)
                                       bQueueState= StpClkDirPwmQueue(ucUnit, AT91C_TC_CLKSTA, (ucCpreMax | AT91C_PWMC_CPD), u16PrdMax, SStpClkDirRO[ucUnit].u16Timeout);
                                       if (bQueueState)
                                       {
                                          // Clear speed changed flag
                                          SStpClkDirRW[ucUnit].bChanged= FALSE;
                                          
                                          // Set start=acc state
                                          ulValue= AT91C_TC_CLKSTA;
                                          if(OS_MBX_VALUE_CHK(os_mbx_write(&MbxStpClkDirState[ucUnit], OS_MBX_VALUE_PUT(ulValue), SStpClkDirRO[ucUnit].u16Timeout)))
                                          {
                                             // Enable pwm channel IRQ
                                             AT91F_PWMC_InterruptEnable(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);
                                             
                                             ucResult= 1;
                                          }
                                       }
                                       else // acc queue error (last value)
                                       {
                                          ucError= STPCLKDIR_ERR_SPACE_QACCLAST;
                                       }
                                    }
                                    else // acc queue error (2nd and up value)
                                    {
                                       ucError= STPCLKDIR_ERR_SPACE_QACC2NDFF;
                                    }
                                 }
                                 else // acc queue error (1st value)
                                 {
                                    ucError= STPCLKDIR_ERR_SPACE_QACC1ST;
                                 }
                              }
                              else // clks current/new are equal on pwm period basis
                              {
                                 ucError= STPCLKDIR_ERR_INVALID_CLKDIFF;
                                 
                                 // Fake success
                                 ucResult= 1;
                              }
                           }
                           else // low speed unsupported
                           {
                              ucError= STPCLKDIR_ERR_INVALID_CLKLOW;
                           }
                        }
                        else // high speed unsupported
                        {
                           ucError= STPCLKDIR_ERR_INVALID_CLKHIGH;
                        }
                     }
                     else // not enough space in acc queue
                     {
                        ucError= STPCLKDIR_ERR_SPACE_QACC;
                     }
                  }
                  else // clks from/to are equal
                  {
                     ucError= STPCLKDIR_ERR_INVALID_CLKDIFF;
                  }
               }
               else // clksacc out of range
               {
                  ucError= STPCLKDIR_ERR_RANGE_CLKSACC;
               }
            }
            else // brk/acc already active
            {
               ucError= STPCLKDIR_ERR_STATE_BRKACC;
            }
         }
         else // unit not active (hw check)
         {
            ucError= STPCLKDIR_ERR_STATE_UNITHW;
         }
      }
      else // unit not active (sem check)
      {
         ucError= STPCLKDIR_ERR_STATE_UNITOS;
      }
   }
   else // unit out of range
   {
      ucError= STPCLKDIR_ERR_RANGE_UNIT;
   }
   
   if (pucError) *pucError= ucError;
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirBrk
//----------------------------------------------------------------------------
UCHAR StpClkDirBrk(UCHAR ucUnit, UCHAR *pucError, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   UCHAR ucError= 0;

   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check for current movement (lock data of start stage should fail)
      if (!OS_SEM_TAKE_CHK(os_sem_check(SStpClkDirRW[ucUnit].SemStart, TRUE)))
      {
         // Check busy condition
         if (SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId)
         {      
            // Check for active run (acc done yet, no brk invoked)
            if (SStpClkDirRW[ucUnit].u32PwmState & AT91C_TC_CPAS)
            {                  
               // Enable external event to restart counter (also prepares timer for last cycle and stop)
               SStpClkDirRO[ucUnit].pTccBase->TC_CMR |= AT91C_TC_ENETRG;
               
               ucResult= 1;
            }
            else // brk already active
            {
               ucError= STPCLKDIR_ERR_STATE_BRKACC;
            }
         }
         else // unit not active (hw check)
         {
            ucError= STPCLKDIR_ERR_STATE_UNITHW;
         }
      }
      else // unit not active (sem check)
      {
         ucError= STPCLKDIR_ERR_STATE_UNITOS;
      }      
   }
   else // unit out of range
   {
      ucError= STPCLKDIR_ERR_RANGE_UNIT;
   }     

   if (pucError) *pucError= ucError;
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR _StpClkDirStart
//----------------------------------------------------------------------------
__inline UCHAR _StpClkDirStart(UCHAR ucUnit, bool bDir, U16 u16TccRa, U16 u16TccRb, U16 u16TccRc, USHORT usRepeat, bool bContinous, U16 u16PwmCmr, U16 u16PwmCprdr, U16 u16PwmCdtyr, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   ULONG ulValue;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check lock data (start stage)
      if (!OS_SEM_TAKE_CHK(os_sem_check(SStpClkDirRW[ucUnit].SemStart, TRUE)))
      {
         // Check data lock (finish stage)
         if (!OS_SEM_TAKE_CHK(os_sem_check(SStpClkDirRW[ucUnit].SemDone, TRUE)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               // Clear last written pwm value copy
               SStpClkDirRW[ucUnit].u32PwmCmrCupdr= 0;
               
               // Clear speed changed flag
               SStpClkDirRW[ucUnit].bChanged= FALSE;
               
               // Stop pwm and timer
               AT91F_PWMC_StopChannel(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);
               SStpClkDirRO[ucUnit].pTccBase->TC_CCR |= AT91C_TC_CLKDIS;

               // Disable pwm and timer interrupts
               AT91F_PWMC_InterruptDisable(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);
               AT91F_TC_InterruptDisable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPAS | AT91C_TC_CPBS | AT91C_TC_CPCS | AT91C_TC_ETRGS));

               // Set direction
               SStpClkDirRW[ucUnit].bDirection= ((SStpClkDirRO[ucUnit].ucFlags & STPCLKDIR_FLAGS_INVERSE)) ? (!bDir) : (bDir);
               if ((SStpClkDirRW[ucUnit].pAuxDir) ? (SStpClkDirRW[ucUnit].pAuxDir(ucUnit, SStpClkDirRW[ucUnit].bDirection)) : (1))
               {
                  // Prepare motor dac and driver
                  if (StpClkDirPowerEx(ucUnit, TRUE, TRUE, TRUE, u16Timeout))
                  {
                     // Update data structure
                     
                     // CAVEAT!!! Zero compare values do *not* generate IRQs!!!)
                     SStpClkDirRW[ucUnit].u16TccRa= (u16TccRa) ? (u16TccRa) : (0x0001);
                     SStpClkDirRW[ucUnit].u16TccRb= (u16TccRb) ? (u16TccRb) : (0xFFFF);
                     
                     // Rc cannot be a zero value by design
                     SStpClkDirRW[ucUnit].u16TccRc= u16TccRc;
            
                     //SStpClkDirRW[ucUnit].u16TccRa= (u16TccRa) ? (u16TccRa) : (0x0001);
                     //SStpClkDirRW[ucUnit].u16TccRb= (u16TccRb) ? ((u16TccRb > 1) ? (u16TccRb- 1) : (u16TccRb)) : (0xFFFF);
                     //SStpClkDirRW[ucUnit].u16TccRc= (u16TccRc) ? ((u16TccRc > 1) ? (u16TccRc- 1) : (u16TccRc)) : (0x0001);
                                            
                     SStpClkDirRW[ucUnit].usRepeat= usRepeat;
                     SStpClkDirRW[ucUnit].bContinous= bContinous;
                     
                     // Change Id
                     SStpClkDirRW[ucUnit].ulClkId++;

                     // Set start=acc state
                     ulValue= AT91C_TC_CLKSTA;
                     if (OS_MBX_VALUE_CHK(os_mbx_write(&MbxStpClkDirState[ucUnit], OS_MBX_VALUE_PUT(ulValue), SStpClkDirRO[ucUnit].u16Timeout)))
                     {
                        // State indicator update (safe here because pwm channel IRQ still disabled and not running yet)
                        SStpClkDirRW[ucUnit].u32PwmState= ulValue;
                        
                        // Reset u32ClkCntOffset (safe here because pwm channel IRQ still disabled and not running yet)
                        SStpClkDirRW[ucUnit].u32ClkCntOffset= 0;
   
                        // Clear cycle
                        SStpClkDirRW[ucUnit].usCycle= 0;
                        
                        // Clear stop state
                        SStpClkDirRW[ucUnit].bStop= FALSE;
                        
                        // Set run state (required for time critical isr safe functions)
                        SStpClkDirRW[ucUnit].bRun= TRUE;

                        LOCK;
                        {
                           // CFG
                           
                           // Cfg initial pwm clock (mode, period and duty)
                           AT91F_PWMC_CfgChannel(SStpClkDirRO[ucUnit].pPwmBase, PWM_CHID2UNIT(SStpClkDirRO[ucUnit].ucPwmChannelId), u16PwmCmr, u16PwmCprdr, u16PwmCdtyr);

                           // Cfg timer compare regs and single/multi mode
                           SStpClkDirRO[ucUnit].pTccBase->TC_RA= SStpClkDirRW[ucUnit].u16TccRa;
                           SStpClkDirRO[ucUnit].pTccBase->TC_RB= SStpClkDirRW[ucUnit].u16TccRb;
                           
                           if (!SStpClkDirRW[ucUnit].usRepeat && !SStpClkDirRW[ucUnit].bContinous)
                           {
                              // Single cycle drops TIOA at RC counter match
                              SStpClkDirRO[ucUnit].pTccBase->TC_RC= SStpClkDirRW[ucUnit].u16TccRc;
                              SStpClkDirRO[ucUnit].pTccBase->TC_CMR |= (AT91C_TC_CPCSTOP | AT91C_TC_ACPC_CLEAR);
                           }
                           else
                           {
                              // Multi cycle keeps TIOA unchanged and RC at max until its last cycle is reached
                              SStpClkDirRO[ucUnit].pTccBase->TC_RC= 0xFFFF;
                              SStpClkDirRO[ucUnit].pTccBase->TC_CMR &= ~(AT91C_TC_CPCSTOP | AT91C_TC_ACPC);
                           }

                           // IRQ

                           // Enable pwm channel IRQ
                           AT91F_PWMC_InterruptEnable(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);

                           // Set timer IRQ masks (virtual hardware register)
                           SStpClkDirRW[ucUnit].u32TccImr= (AT91C_TC_CPAS | AT91C_TC_CPBS | AT91C_TC_CPCS | AT91C_TC_ETRGS);
                           //-AT91F_TC_InterruptDisable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPBS | AT91C_TC_CPCS));
                           //-AT91F_TC_InterruptEnable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPAS | AT91C_TC_ETRGS));
                           // Because changes are ignored by true hardware register after timer is started we need to enable them all here
                           AT91F_TC_InterruptEnable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPAS | AT91C_TC_CPBS | AT91C_TC_CPCS | AT91C_TC_ETRGS));

                           // ARM

                           // Arm timer, enable timer clock input and pass clock thru to stepper driver
                           SStpClkDirRO[ucUnit].pTccBase->TC_CCR |= (AT91C_TC_SWTRG | AT91C_TC_CLKEN);
                        }
                        UNLOCK;

                        // START

                        // Start pwm channel (used as external clock source for the already armed timer)
                        AT91F_PWMC_StartChannel(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);

                        // Force scheduler to run other tasks
                        RESCHEDULE_TICKS(0);
                        
                        ucResult= 1;
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
// UCHAR _StpClkDirMbxClear
//----------------------------------------------------------------------------
__inline UCHAR _StpClkDirMbxClear(UCHAR ucUnit, U16 u16Timeout)
{
   // Clear (wipe) mbx state
   os_mbx_clear(&MbxStpClkDirState[ucUnit], u16Timeout);
   
   // Clear (wipe) mbx acc
   os_mbx_clear(&MbxStpClkDirAcc[ucUnit], u16Timeout);
   
   // Clear (wipe) mbx brk
   os_mbx_clear(&MbxStpClkDirBrk[ucUnit], u16Timeout);
   
   // Clear (wipe) mbx offset
   os_mbx_clear(&MbxStpClkDirOffset[ucUnit], u16Timeout);
   
   return (1);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirQuit
//----------------------------------------------------------------------------
UCHAR StpClkDirQuit(UCHAR ucUnit, U16 u16Timeout)
{
   UCHAR ucResult= 0;

   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check for current movement (lock data start stage should fail)
      if (!OS_SEM_TAKE_CHK(os_sem_check(SStpClkDirRW[ucUnit].SemStart, TRUE)))
      {
         // Check busy condition
         if (SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId)
         {
            // Only hard stop pwm after one complete pwm cycle/irq because of hardware bug (refer to AT91SAM7X manual 41.4.6.4 page 651)
            
            // Check last written pwm value copy for a complete pwm cycle/irq (cleared by _StpClkDirStart, updated by StpClkDirPwmIrqHandler)
            if (!SStpClkDirRW[ucUnit].u32PwmCmrCupdr)
            {
               os_dly_wait(u16Timeout);
            }
            
            if (SStpClkDirRW[ucUnit].u32PwmCmrCupdr)
            {
               // Hard stop
               // Trigger TskStpClkDirHold by StpClkDirTccIrqHandlerStop()
               StpClkDirTccIrqHandlerStop(ucUnit, FALSE);
               ucResult= 1;
            }
         }
      }
   }
   
   return (ucResult);
} 

//----------------------------------------------------------------------------
// UCHAR StpClkDirSpeedChangeWait
//----------------------------------------------------------------------------
UCHAR StpClkDirSpeedChangeWait(UCHAR ucUnit, UCHAR *pucError, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   UCHAR ucError= 0;
    
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check for current movement (lock data start stage should fail)
      if (!OS_SEM_TAKE_CHK(os_sem_check(SStpClkDirRW[ucUnit].SemStart, TRUE)))      
      {
         // Check busy condition
         if (SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId)
         {
            // Check for active acc or run (no brk invoked, no stop)
            if ((SStpClkDirRW[ucUnit].u32PwmState & AT91C_TC_CLKSTA) || (SStpClkDirRW[ucUnit].u32PwmState & AT91C_TC_CPAS))
            {  
               // Check for ramp already done (cleared by StpClkDirSpeedChange)
               if (SStpClkDirRW[ucUnit].bChanged)
               {
                  // Clear still available semaphore without pending
                  OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemRun, OS_T_NONE));
                  
                  ucResult= 1;
               }
               // Pend for ramp done
               else if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemRun, u16Timeout)))
               {
                  // Set speed changed flag
                  SStpClkDirRW[ucUnit].bChanged= TRUE;

                  ucResult= 1;
               }
               else // brk/acc already active
               {
                  ucError= STPCLKDIR_ERR_STATE_BRKACC;
               }
            }
            else // brk/acc already active
            {
               ucError= STPCLKDIR_ERR_STATE_BRKACC;
            }            
         }
         else // unit not active (hw check)
         {
            ucError= STPCLKDIR_ERR_STATE_UNITHW;
         }
      }
      else // unit not active (sem check)
      {
         ucError= STPCLKDIR_ERR_STATE_UNITOS;
      }      
   }
   else // unit out of range
   {
      ucError= STPCLKDIR_ERR_RANGE_UNIT;
   }   
   
   if (pucError) *pucError= ucError;
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirWait
//----------------------------------------------------------------------------
UCHAR StpClkDirWait(UCHAR ucUnit, U16 u16Timeout)
{
   UCHAR ucResult= 0;
    
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               ucResult= 1;
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
         
         RESCHEDULE_TICKS(0);
      }
   }

   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirClksPosOffsSet
//----------------------------------------------------------------------------
UCHAR StpClkDirClksPosOffsSet(UCHAR ucUnit, ULONG ulClkPosZeroOffset, U16 u16Timeout)
{
   UCHAR ucResult= 0;
     
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset= ulClkPosZeroOffset;
               ucResult= 1;
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }

   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirClksPosOffsGet
//----------------------------------------------------------------------------
UCHAR StpClkDirClksPosOffsGet(UCHAR ucUnit, ULONG *pulClkPosZeroOffset, U16 u16Timeout)
{
   UCHAR ucResult= 0;
     
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               if (pulClkPosZeroOffset)
               {
                  *pulClkPosZeroOffset= SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset;
                  ucResult= 1;
               }
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }

   return (ucResult);
}

//----------------------------------------------------------------------------
// ULONG _StpClkDirClksPosOffsGet
//----------------------------------------------------------------------------
__inline ULONG _StpClkDirClksPosOffsGet(UCHAR ucUnit)
{
   register ULONG ulResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      ulResult= SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset;
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirClksPosOffsVal
//----------------------------------------------------------------------------
ULONG StpClkDirClksPosOffsVal(UCHAR ucUnit, ULONG ulVal, bool bSet, U16 u16Timeout)
{
   register ULONG ulResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {    
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               ulResult= ((ULONG_MAX- SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute)+ 1)+ ulVal;
   
               if (bSet)
               {
                  SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset= ulResult;
               }
            }
   
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }

   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirClksPosOffsMakePlus
//----------------------------------------------------------------------------
ULONG StpClkDirClksPosOffsMakePlus(UCHAR ucUnit, bool bSet, U16 u16Timeout)
{
   register ULONG ulResult= 0;
     
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               ulResult= ((ULONG_MAX- SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute)+ 1) << 1;
   
               if (bSet)
               {
                  SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset= ulResult;
               }
            }
   
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }

   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirHzGet
//----------------------------------------------------------------------------
ULONG StpClkDirHzGet(UCHAR ucUnit)
{
   register ULONG ulResult= 0;
   
   register U32 u32PwmCmrCupdr;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      LOCKED(u32PwmCmrCupdr= SStpClkDirRW[ucUnit].u32PwmCmrCupdr);
      ulResult= PwmCalcHz((UCHAR)((u32PwmCmrCupdr & BITS_ULONGHX) >> 16), (U16)(u32PwmCmrCupdr & BITS_ULONGLX));
   }

   return (ulResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirClksAbsRawSet
//----------------------------------------------------------------------------
UCHAR StpClkDirClksAbsRawSet(UCHAR ucUnit, ULONG u32ClkCntAbsolute, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute= u32ClkCntAbsolute;
               ucResult= 1;
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirClksAbsRawGet
//----------------------------------------------------------------------------
UCHAR StpClkDirClksAbsRawGet(UCHAR ucUnit, ULONG *pu32ClkCntAbsolute, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               if (pu32ClkCntAbsolute)
               {
                  *pu32ClkCntAbsolute= SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute;
                  ucResult= 1;
               }
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirClksAbsSet
//----------------------------------------------------------------------------
UCHAR StpClkDirClksAbsSet(UCHAR ucUnit, ULONG ulClksAbs, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               LOCK;
               {
                  SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute= ulClksAbs;
                  SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset= 0;
               }
               UNLOCK;
   
               ucResult= 1;
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirClksAbsShiftSet
//----------------------------------------------------------------------------
UCHAR StpClkDirClksAbsShiftSet(UCHAR ucUnit, LONG lOffset, U16 u16Timeout)
{
   UCHAR ucResult= 0;

   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      if (lOffset)
      {
         // Check device available (start stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
         {
            // Check for movement done (finish stage)
            if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
            {
               // Check ready condition
               if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
               {
                  ucResult= StpClkDirClksAbsShiftSetEx(ucUnit, lOffset, TRUE, u16Timeout);
               }
               
               os_sem_give(SStpClkDirRW[ucUnit].SemDone);
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemStart);
         }
      }
   }
   
   return (ucResult);
}


//----------------------------------------------------------------------------
// UCHAR StpClkDirClksAbsShiftSetEx
//----------------------------------------------------------------------------
__inline UCHAR StpClkDirClksAbsShiftSetEx(UCHAR ucUnit, LONG lOffset, bool bLocked, U16 u16Timeout)
{
   UCHAR ucResult= 0;

   // Determine modulo value for overrun and underrun handling
   ULONG ulClkModulo= (SStpClkDirRO[ucUnit].u32ClkModulo) ? (SStpClkDirRO[ucUnit].u32ClkModulo) : (ULONG_MAX);

   // Determine true absolute position (offset'ed !!!)
   ULONG ulClkAbs= (SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute+ SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset) % ulClkModulo;
   
   ULONG ulDiff;
   ULONG ulClkCurr;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      if (lOffset)
      {
         // Check device available (start stage)
         if ((!bLocked) ? (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout))) : (TRUE))
         {
            // Check for movement done (finish stage)
            if ((!bLocked) ? (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout))) : (TRUE))
            {
               // Check ready condition
               if ((!bLocked) ? (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun) : (TRUE))
               {
                  LOCK;
                  {
                     if (lOffset >= 0)
                     {
                        // Increment of position
                        
                        // Determine head-room of u32ClkCntAbsolute for incrementing
                        ulDiff= ULONG_MAX- SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute;
   
                        if (ulDiff >= lOffset)
                        {
                           ulClkCurr= (SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute+ SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset+ lOffset) % ulClkModulo;
                        }
                        else
                        {
                           // Overrun handling         
                           ulClkCurr= lOffset- (ulClkModulo- ulClkAbs);
                        }
                     }
                     else
                     {
                        // Decrement of position
                        
                        // ulClkAbs is the head-room of u32ClkCntAbsolute for decrementing
                        
                        if (ulClkAbs >= (-lOffset))
                        {
                           ulClkCurr= ulClkAbs- (-lOffset);
                        }
                        else
                        {
                           // Underrun handling         
                           ulClkCurr= ulClkModulo- ((-lOffset)- ulClkAbs);
                        }
                     }
                     
                     SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute= ulClkCurr;
                     SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset= 0;
                     
                     ucResult= 1;
                  }
                  UNLOCK;
               }
               
               if (!bLocked) os_sem_give(SStpClkDirRW[ucUnit].SemDone);
            }
            
            if (!bLocked) os_sem_give(SStpClkDirRW[ucUnit].SemStart);
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirClksAbsShiftGet
//----------------------------------------------------------------------------
ULONG StpClkDirClksAbsShiftGet(UCHAR ucUnit, LONG *plOffset, U16 u16Timeout)
{
   ULONG ulResult= 0;
    
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               if (plOffset) *plOffset= SStpClkDirRW[ucUnit].lOffsetMean;
               ulResult= SStpClkDirRW[ucUnit].ulOffsetMsgCnt;
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }

   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirClksAbsGet (ISR safe)
//----------------------------------------------------------------------------
ULONG StpClkDirClksAbsGet(UCHAR ucUnit)
{  
   ULONG ulResult= 0;

   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check state (time critical here - both hardware and software must indicate completion)
      if ((SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) || SStpClkDirRW[ucUnit].bRun)
      {
         // Running state (movement active)

         // Determine current position (modulo'd and offset'ed)
         StpClkDirTccIrqHandlerClksUpdate(ucUnit, (U32 *)&ulResult, FALSE);
      }
      else
      {
         // Standby (no move active yet)

         // Determine current position (offset'ed but not modulo'd)
         LOCKED(ulResult= SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute+ SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset);
         
         // Modulo mode (limits are given ulClkModulo sides)
         if (SStpClkDirRO[ucUnit].u32ClkModulo)
         {
            if (ulResult >= SStpClkDirRO[ucUnit].u32ClkModulo)
            {
               ulResult %= SStpClkDirRO[ucUnit].u32ClkModulo;
            }
         }

         // Update previous
         SStpClkDirRW[ucUnit].bDirectionPrevious= (((SStpClkDirRO[ucUnit].ucFlags & STPCLKDIR_FLAGS_INVERSE)) ? (!SStpClkDirRW[ucUnit].bDirection) : (SStpClkDirRW[ucUnit].bDirection));
         SStpClkDirRW[ucUnit].ulClkIdPrevious= SStpClkDirRW[ucUnit].ulClkId;
      } 
   }
         
   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirClksGet (ISR safe)
//----------------------------------------------------------------------------
ULONG StpClkDirClksGet(UCHAR ucUnit)
{
   register ULONG ulResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      LOCKED(ulResult= SStpClkDirRW[ucUnit].u32ClkCntOffset+ SStpClkDirRO[ucUnit].pTccBase->TC_CV);
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirJobClksGet (ISR safe)
//----------------------------------------------------------------------------
ULONG StpClkDirJobClksGet(UCHAR ucUnit)
{
   register ULONG ulResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      LOCKED(ulResult= (SStpClkDirRW[ucUnit].usRepeat* ulUSHRT_CNT)+ SStpClkDirRW[ucUnit].u16TccRc);
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirModuloMaxClksGet (ISR safe)
//----------------------------------------------------------------------------
ULONG StpClkDirModuloMaxClksGet(UCHAR ucUnit)
{
   register ULONG ulResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      if (SStpClkDirRO[ucUnit].u32ClkModulo)
      {
         ulResult= (SStpClkDirRO[ucUnit].u32ClkModulo- 1);
      }
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG StpClkDirModuloClksGet (ISR safe)
//----------------------------------------------------------------------------
ULONG StpClkDirModuloClksGet(UCHAR ucUnit)
{
   register ULONG ulResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      ulResult= SStpClkDirRO[ucUnit].u32ClkModulo;
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirMoveResultGet
//----------------------------------------------------------------------------
UCHAR StpClkDirMoveResultGet(UCHAR ucUnit, ULONG *pulResult, U16 u16Timeout)
{
   UCHAR ucResult= 0;
    
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               if (pulResult) *pulResult= SStpClkDirRW[ucUnit].ulTskStpClkDirHold;
               ucResult= 1;
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }

   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirDacSet
//----------------------------------------------------------------------------
UCHAR StpClkDirDacSet(UCHAR ucUnit, USHORT usDacValueHold, USHORT usDacValueRun, U16 u16Timeout)
{
   UCHAR ucResult= 0;
    
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               if (usDacValueHold)
               {
                  SStpClkDirRW[ucUnit].SPermanent.usDacValueHold= usDacValueHold;
                  ucResult++;
               }
               
               if (usDacValueRun)
               {
                  SStpClkDirRW[ucUnit].SPermanent.usDacValueRun= usDacValueRun;
                  ucResult++;
               }            
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }

   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirDacGet
//----------------------------------------------------------------------------
UCHAR StpClkDirDacGet(UCHAR ucUnit, USHORT *pusDacValueHold, USHORT *pusDacValueRun, U16 u16Timeout)
{
   UCHAR ucResult= 0;
    
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               if (pusDacValueHold)
               {
                  *pusDacValueHold= SStpClkDirRW[ucUnit].SPermanent.usDacValueHold;
                  ucResult++;
               }
               
               if (pusDacValueRun)
               {
                  *pusDacValueRun= SStpClkDirRW[ucUnit].SPermanent.usDacValueRun;
                  ucResult++;
               }            
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }

   return (ucResult);
}

//----------------------------------------------------------------------------
// size_t StpClkDirSnapShotSizeGet
//----------------------------------------------------------------------------
size_t StpClkDirSnapShotSizeGet(void)
{
   return (sizeof(struct _SStpClkDirPermanent));
}

//----------------------------------------------------------------------------
// void * StpClkDirSnapShotAddrGet
//----------------------------------------------------------------------------
void * StpClkDirSnapShotAddrGet(UCHAR ucUnit)
{
   void *pvResult= NULL;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      pvResult= (void *) &SStpClkDirSnapShot[ucUnit];
   }
   
   return (pvResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirSnapShot
//----------------------------------------------------------------------------
UCHAR StpClkDirSnapShot(UCHAR ucUnit, bool bRestore, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check device available (start stage)
      if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check for movement done (finish stage)
         if (OS_SEM_TAKE_CHK(os_sem_take(SStpClkDirRW[ucUnit].SemDone, u16Timeout)))
         {
            // Check ready condition
            if (!(SStpClkDirRO[ucUnit].pPwmBase->PWMC_SR & SStpClkDirRO[ucUnit].ucPwmChannelId) && !SStpClkDirRW[ucUnit].bRun)
            {
               if (!bRestore)
               {
                  // Create snapshot
                  
                  SStpClkDirSnapShot[ucUnit].ulClkPosZeroOffset      = SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset;
                  SStpClkDirSnapShot[ucUnit].u32ClkCntAbsolute       = SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute;
                  SStpClkDirSnapShot[ucUnit].usDacValueHold          = SStpClkDirRW[ucUnit].SPermanent.usDacValueHold;
                  SStpClkDirSnapShot[ucUnit].usDacValueRun           = SStpClkDirRW[ucUnit].SPermanent.usDacValueRun;
               }
               else
               {
                  // Restore snapshot
                  
                  SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset = SStpClkDirSnapShot[ucUnit].ulClkPosZeroOffset;
                  SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute  = SStpClkDirSnapShot[ucUnit].u32ClkCntAbsolute;
                  SStpClkDirRW[ucUnit].SPermanent.usDacValueHold     = SStpClkDirSnapShot[ucUnit].usDacValueHold;
                  SStpClkDirRW[ucUnit].SPermanent.usDacValueRun      = SStpClkDirSnapShot[ucUnit].usDacValueRun;
               }
   
               ucResult= 1;
            }
            
            os_sem_give(SStpClkDirRW[ucUnit].SemDone);
         }
         
         os_sem_give(SStpClkDirRW[ucUnit].SemStart);
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// OS_RESULT StpClkDirOffsetPush
//----------------------------------------------------------------------------
OS_RESULT StpClkDirOffsetPush(UCHAR ucUnit, LONG lOffset, U16 u16Timeout)
{
   OS_RESULT oResult= OS_R_TMO;
   
   if (os_mbx_check(&MbxStpClkDirOffset[ucUnit]) > 0)
   {
      oResult= os_mbx_send(&MbxStpClkDirOffset[ucUnit], OS_MBX_VALUE_PUT(lOffset), u16Timeout);
   }
   
   return (oResult);
}

//----------------------------------------------------------------------------
// void StpClkDirTccIrqHandlerBrake
//----------------------------------------------------------------------------
__inline void StpClkDirTccIrqHandlerBrake(UCHAR ucUnit)
{
   // Enable pwm channel IRQ
   AT91F_PWMC_InterruptEnable(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);
}

//----------------------------------------------------------------------------
// void StpClkDirTccIrqHandlerPrepareLast
//----------------------------------------------------------------------------
__inline void StpClkDirTccIrqHandlerPrepareLast(UCHAR ucUnit)
{
   // Set tcc IRQ mask (AT91C_TC_CPBS | AT91C_TC_CPCS | AT91C_TC_ETRGS)
   // Changes are ignored by true hardware register after timer is started!!!
   //-AT91F_TC_InterruptDisable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPAS));
   //-AT91F_TC_InterruptEnable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPBS | AT91C_TC_CPCS | AT91C_TC_ETRGS));
   
   // Set RC value
   SStpClkDirRO[ucUnit].pTccBase->TC_RC= SStpClkDirRW[ucUnit].u16TccRc;
   
   // Stop and drop TIOA at next RC counter match
   SStpClkDirRO[ucUnit].pTccBase->TC_CMR |= (AT91C_TC_CPCSTOP | AT91C_TC_ACPC_CLEAR); 
}

//----------------------------------------------------------------------------
// bool StpClkDirTccIrqHandlerClksUpdate
//----------------------------------------------------------------------------
__inline bool StpClkDirTccIrqHandlerClksUpdate(UCHAR ucUnit, U32 *pu32Clk, bool bIsr)
{   
	// Catch 16bit timer-counter immediately and determine true 32bit _JOB's__RELATIVE_counter clks (assume (end of) running state)
   register U32 u32ClkTc= (bIsr) ? (SStpClkDirRW[ucUnit].u16TccRc+ SStpClkDirRW[ucUnit].u32ClkCntOffset) : (SStpClkDirRO[ucUnit].pTccBase->TC_CV+ SStpClkDirRW[ucUnit].u32ClkCntOffset);

   // Determine modulo value for overrun and underrun handling
   register ULONG ulClkModulo= (SStpClkDirRO[ucUnit].u32ClkModulo) ? (SStpClkDirRO[ucUnit].u32ClkModulo) : (ULONG_MAX);

   // Determine true absolute position (offset'ed !!!)
   register ULONG ulClkAbs= (SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute+ SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset) % ulClkModulo;

   bool bResult= FALSE;

   register ULONG ulDiff;

   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      // Check required storage pointer
      if (pu32Clk)
      {
         if (bIsr)
         {
            // Force run state (required for time critical isr safe functions)
            SStpClkDirRW[ucUnit].bRun= TRUE;
         }
      
         // Update u32ClkCntAbsolute only
      
         // Determine increment (clockwise) or decrement (counter-clockwise) of current move
         if ((((SStpClkDirRO[ucUnit].ucFlags & STPCLKDIR_FLAGS_INVERSE)) ? (!SStpClkDirRW[ucUnit].bDirection) : (SStpClkDirRW[ucUnit].bDirection)) == STPCLKDIR_CLOCKWISE)
         {
            // Increment of position
      
            // Determine head-room of u32ClkCntAbsolute for incrementing
            ulDiff= ULONG_MAX- SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute;
      
            if (ulDiff >= u32ClkTc)
            {
               *pu32Clk= (SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute+ SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset+ u32ClkTc) % ulClkModulo;
            }
            else
            {
               // Overrun handling         
               *pu32Clk= u32ClkTc- (ulClkModulo- ulClkAbs);
            }
         }
         else
         {
            // Decrement of position
            
            // ulClkAbs is the head-room of u32ClkCntAbsolute for decrementing
            if (ulClkAbs >= u32ClkTc)
            {
               *pu32Clk= ulClkAbs- u32ClkTc;
            }
            else
            {     
               // Underrun handling         
               *pu32Clk= ulClkModulo- (u32ClkTc- ulClkAbs);
            }
         }

         if (pu32Clk == &SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute)
         {
            // Clear ulClkPosZeroOffset after move completed TO AVOID DOUBLE ADDING OF THIS OFFSET BY STPCLKDIRCLKSABSGET
            // BECAUSE IT IS ALREADY ADDED TO THE DESTINATION POSITION VALUE ABOVE!!!
            SStpClkDirRW[ucUnit].SPermanent.ulClkPosZeroOffset= 0;
         }

         if (bIsr)
         {
            // Clear run state (required for time critical isr safe functions)
            SStpClkDirRW[ucUnit].bRun= FALSE;
         }
         
         bResult= TRUE;
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// void StpClkDirTccIrqHandlerStop
//----------------------------------------------------------------------------
__inline void StpClkDirTccIrqHandlerStop(UCHAR ucUnit, bool bIsr)
{
   // Disable pwm channel IRQ
   AT91F_PWMC_InterruptDisable(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);
     
   // Stop pwm channel
   AT91F_PWMC_StopChannel(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);   
     
   // Pwm state indicator update (safe here because pwm channel irq already disabled and stopped)
   SStpClkDirRW[ucUnit].u32PwmState= AT91C_TC_CPCS;

   // Set tcc IRQ mask (AT91C_TC_CPAS | AT91C_TC_ETRGS)
   // Changes are ignored by true hardware register after timer is started!!!
   //-AT91F_TC_InterruptDisable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPBS | AT91C_TC_CPCS));
   //-AT91F_TC_InterruptEnable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPAS | AT91C_TC_ETRGS));
   
   if (bIsr)
   {
      // Trigger stop/hold setup, which unlocks start stage too
      if (!ISR_SEM_TAKE_CHK(isr_sem_check(SStpClkDirRW[ucUnit].SemHold)))
      {
         isr_sem_give(SStpClkDirRW[ucUnit].SemHold);
      }
   }
   else
   {
      // Trigger stop/hold setup, which unlocks start stage too
      if (!OS_SEM_TAKE_CHK(os_sem_check(SStpClkDirRW[ucUnit].SemHold, FALSE)))
      {
         os_sem_give(SStpClkDirRW[ucUnit].SemHold);
      }
      UNLOCK;
      
      RESCHEDULE_TICKS(0);
   }
   
   // Clear run state (required because StpClkDirHandlerClksUpdate [which clears this flag before StpClkDirTccIrqHandlerStop is called] won't run for StpClkDirQuit!!)
   SStpClkDirRW[ucUnit].bRun= FALSE;
}

//----------------------------------------------------------------------------
// void StpClkDirTccIrqHandler
//----------------------------------------------------------------------------
void StpClkDirTccIrqHandler(UCHAR ucUnit)
{
   ULONG ulState= 0;
   
   register U32 u32TccIrq;
   register U32 u32Ra, u32Rb, u32Rc, u32Cnt;
   
   u32Ra= SStpClkDirRO[ucUnit].pTccBase->TC_RA;
   u32Rb= SStpClkDirRO[ucUnit].pTccBase->TC_RB;
   u32Rc= SStpClkDirRO[ucUnit].pTccBase->TC_RC;
   u32Cnt= SStpClkDirRO[ucUnit].pTccBase->TC_CV;
   
   // Avoid compiler warnings
   u32Ra= u32Ra;
   u32Rb= u32Rb;
   u32Rc= u32Rc;
   u32Cnt= u32Cnt;
   
   // Copy and clear the IRQ source to handle yet (IRQs may blocked by SStpClkDirRW[ucUnit].u32TccImr)
   u32TccIrq= (SStpClkDirRO[ucUnit].pTccBase->TC_IMR & SStpClkDirRO[ucUnit].pTccBase->TC_SR) & SStpClkDirRW[ucUnit].u32TccImr;

   // ETRGS (counter reset *without* touching TIOA caused by early brake request)
   if (u32TccIrq & AT91C_TC_ETRGS)
   {
      // Disable further external events
      SStpClkDirRO[ucUnit].pTccBase->TC_CMR &= ~AT91C_TC_ENETRG;
      
      // Prepare for forced last cycle

      // Disable timer clock
      SStpClkDirRO[ucUnit].pTccBase->TC_CCR &= ~AT91C_TC_CLKEN;
      {
         // Set new RC value (= original RC- original RB)
         SStpClkDirRO[ucUnit].pTccBase->TC_RC= (SStpClkDirRW[ucUnit].u16TccRc- SStpClkDirRW[ucUnit].u16TccRb);
   
         // Stop and drop TIOA at next RC counter match
         SStpClkDirRO[ucUnit].pTccBase->TC_CMR |= (AT91C_TC_CPCSTOP | AT91C_TC_ACPC_CLEAR); 
      }
      // Re-enable timer clock
      SStpClkDirRO[ucUnit].pTccBase->TC_CCR |= AT91C_TC_CLKEN;
      
      // Set tcc IRQ mask (AT91C_TC_CPCS | AT91C_TC_ETRGS)
      // Changes are ignored by true hardware register after timer is started!!!
      //-AT91F_TC_InterruptEnable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPCS | AT91C_TC_ETRGS));
      //-AT91F_TC_InterruptDisable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPAS | AT91C_TC_CPBS));

      // Send state (respond to AT91C_TC_ETRGS like to AT91C_TC_CPBS)
      ulState= AT91C_TC_CPBS;
      isr_mbx_write(&MbxStpClkDirState[ucUnit], ISR_MBX_VALUE_PUT(ulState));
      
      // Pwm state indicator update (safe here because pwm irq cannot maintain this state)
      SStpClkDirRW[ucUnit].u32PwmState= ulState; 
                  
      StpClkDirTccIrqHandlerBrake(ucUnit);

      // Set tcc IRQ mask (AT91C_TC_CPCS only!!) 
      SStpClkDirRW[ucUnit].u32TccImr= (AT91C_TC_CPCS);

      // Clear any originally triggered RA, RB and RC IRQ
      u32TccIrq &= ~(AT91C_TC_CPAS | AT91C_TC_CPBS | AT91C_TC_CPCS);
            
      // Fake last cycle (for 32bit multicycles= (2^16)* repeat)
      SStpClkDirRW[ucUnit].usCycle= SStpClkDirRW[ucUnit].usRepeat;
          
      // Set stop condition (overrides continous option)
      SStpClkDirRW[ucUnit].bStop= TRUE;
   }

   if (!SStpClkDirRW[ucUnit].usRepeat && !SStpClkDirRW[ucUnit].bContinous)
   {
      // Basic 16bit single cycle movement
      
      // RA (stop acceleration & start run)
      if (u32TccIrq & AT91C_TC_CPAS)
      {
         // Disable pwm channel IRQ (pwm channel irq handles this itsself for this state)
         //-AT91F_PWMC_InterruptDisable(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);
                 
         // Set state (don't send/change yet because pwm channel irq handles this itsself for this state)
         ulState= AT91C_TC_CPAS;
         
         // Set tcc IRQ mask (AT91C_TC_CPAS | AT91C_TC_CPBS | AT91C_TC_CPCS | AT91C_TC_ETRGS)
         // Changes are ignored by true hardware register after timer is started!!!
         //-AT91F_TC_InterruptEnable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPAS | AT91C_TC_CPBS | AT91C_TC_CPCS | AT91C_TC_ETRGS));
      }
      
      // RB (stop run & start deceleration)
      if (u32TccIrq & AT91C_TC_CPBS)
      {
         // Send state
         ulState= AT91C_TC_CPBS;
         isr_mbx_write(&MbxStpClkDirState[ucUnit], ISR_MBX_VALUE_PUT(ulState));
         
         // Pwm state indicator update (safe here because pwm irq cannot maintain this state)
         SStpClkDirRW[ucUnit].u32PwmState= ulState; 

         
         StpClkDirTccIrqHandlerBrake(ucUnit);
      }
      
      // RC (stop deceleration & stop movement)
      if (u32TccIrq & AT91C_TC_CPCS)
      {
         StpClkDirTccIrqHandlerClksUpdate(ucUnit, &SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute, TRUE);
         StpClkDirTccIrqHandlerStop(ucUnit, TRUE);
   
         // Clear stop
         SStpClkDirRW[ucUnit].bStop= FALSE;
         
         // Reset cycle counter
         SStpClkDirRW[ucUnit].usCycle= 0;

         // Set state
         ulState= AT91C_TC_CPCS;
         
         // Pwm state indicator update (already done by StpClkDirTccIrqHandlerStop)
         //-SStpClkDirRW[ucUnit].u32PwmState= ulState;
      }
   }
   else
   {
      if ((SStpClkDirRW[ucUnit].usCycle == 0) && !SStpClkDirRW[ucUnit].bStop)
      {
         // First cycle of a multi cycle movement (without pending stop)
      
         // RA (stop acceleration & start run)
         if (u32TccIrq & AT91C_TC_CPAS)
         {
            // Disable pwm channel IRQ (pwm channel irq handles this itsself for this state)
            //-AT91F_PWMC_InterruptDisable(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);

            // Set state (don't send/change yet because pwm channel irq handles this itsself for this state)
            ulState= AT91C_TC_CPAS;

            // Set tcc IRQ mask (AT91C_TC_CPCS | AT91C_TC_ETRGS)
            // Changes are ignored by true hardware register after timer is started!!!
            //-AT91F_TC_InterruptDisable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPAS | AT91C_TC_CPBS));
            //-AT91F_TC_InterruptEnable(SStpClkDirRO[ucUnit].pTccBase, (AT91C_TC_CPCS | AT91C_TC_ETRGS));
         }
         
         // RB (ignore)
         if (u32TccIrq & AT91C_TC_CPBS)
         {
            
         }
         
         // RC (count cycles / prepare for last cycle)
         if (u32TccIrq & AT91C_TC_CPCS)
         {
            // Count cycles
            SStpClkDirRW[ucUnit].usCycle++;
            SStpClkDirRW[ucUnit].u32ClkCntOffset += ulUSHRT_CNT;
         
            if (!SStpClkDirRW[ucUnit].bContinous || SStpClkDirRW[ucUnit].bStop)
            {
               if (SStpClkDirRW[ucUnit].usCycle == (SStpClkDirRW[ucUnit].usRepeat))
               {
                  // Prepare for last cycle
                  
                  // Set state
                  ulState= AT91C_TC_CPCS;
                  
                  // Pwm state indicator update (safe here because pwm irq cannot maintain this state)
                  SStpClkDirRW[ucUnit].u32PwmState= ulState;
                  
                  StpClkDirTccIrqHandlerPrepareLast(ucUnit);
               }
            }
         }      
      }
      else
      {
         if (SStpClkDirRW[ucUnit].usCycle > 0 && ((SStpClkDirRW[ucUnit].usCycle < SStpClkDirRW[ucUnit].usRepeat) || (SStpClkDirRW[ucUnit].bContinous && !SStpClkDirRW[ucUnit].bStop)))
         {
            // Inner cycle(s) of a multi cycle movement
            
            // RA (ignore)
            if (u32TccIrq & AT91C_TC_CPAS)
            {
         
            }
            
            // RB (ignore - when brk pos is in last cycle)
            if (u32TccIrq & AT91C_TC_CPBS)
            {
               // Check for next to last cycle
               if ((SStpClkDirRW[ucUnit].usCycle+ 1) == (SStpClkDirRW[ucUnit].usRepeat))
               {
                  // Check for brk triggering already required in next to last cycle
                  if (SStpClkDirRW[ucUnit].u16TccRb > SStpClkDirRW[ucUnit].u16TccRc)
                  {
                     // Send state
                     ulState= AT91C_TC_CPBS;
                     isr_mbx_write(&MbxStpClkDirState[ucUnit], ISR_MBX_VALUE_PUT(ulState));
                     
                     // Pwm state indicator update (safe here because pwm irq cannot maintain this state)
                     SStpClkDirRW[ucUnit].u32PwmState= ulState;
                     
                     StpClkDirTccIrqHandlerBrake(ucUnit);                
                  }
               }
            }
            
            // RC (count cycles / prepare for last cycle)
            if (u32TccIrq & AT91C_TC_CPCS)
            {
               // Count cycles
               SStpClkDirRW[ucUnit].usCycle++;
               SStpClkDirRW[ucUnit].u32ClkCntOffset += ulUSHRT_CNT;
            
               if (!(SStpClkDirRW[ucUnit].bContinous && !SStpClkDirRW[ucUnit].bStop))
               {
                  if (SStpClkDirRW[ucUnit].usCycle == (SStpClkDirRW[ucUnit].usRepeat))
                  {
                     // Prepare for last cycle
                     
                     // Set state
                     ulState= AT91C_TC_CPCS;
                     
                     // Pwm state indicator update (safe here because pwm irq cannot maintain this state)
                     SStpClkDirRW[ucUnit].u32PwmState= ulState;
                     
                     StpClkDirTccIrqHandlerPrepareLast(ucUnit);
                  }
               }         
            }            
         }
         else
         {
            if ((SStpClkDirRW[ucUnit].usCycle >= SStpClkDirRW[ucUnit].usRepeat) || !SStpClkDirRW[ucUnit].bStop)
            {
               // Last cycle of a multi cycle movement (may have pending stop)
               
               // RA (ignore)
               if (u32TccIrq & AT91C_TC_CPAS)
               {
            
               }
               
               // RB (stop run & start deceleration - when not already triggered by previous cycle)
               if (u32TccIrq & AT91C_TC_CPBS)
               {
                  if (SStpClkDirRW[ucUnit].u16TccRb <= SStpClkDirRW[ucUnit].u16TccRc)
                  {
                     // Send state
                     ulState= AT91C_TC_CPBS;
                     isr_mbx_write(&MbxStpClkDirState[ucUnit], ISR_MBX_VALUE_PUT(ulState));
                     
                     // Pwm state indicator update (safe here because pwm irq cannot maintain this state)
                     SStpClkDirRW[ucUnit].u32PwmState= ulState;
                     
                     StpClkDirTccIrqHandlerBrake(ucUnit);
                  }
               }
               
               // RC (stop deceleration & stop movement)
               if (u32TccIrq & AT91C_TC_CPCS)
               {
                  StpClkDirTccIrqHandlerClksUpdate(ucUnit, &SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute, TRUE);
                  StpClkDirTccIrqHandlerStop(ucUnit, TRUE);
                  
                  // Clear stop
                  SStpClkDirRW[ucUnit].bStop= FALSE;
                  
                  // Reset cycle counter
                  SStpClkDirRW[ucUnit].usCycle= 0;

                  // Set state
                  ulState= AT91C_TC_CPCS;
                  
                  // Pwm state indicator update (already done by StpClkDirTccIrqHandlerStop)
                  //-SStpClkDirRW[ucUnit].u32PwmState= ulState;
               }               
            }
         }
      }
   }

   // ARM needs EXPLICIT acknowledge (qualifier __irq doesn't do this) for (E)nd (O)f (I)RQ!!
   EOI;   
}

//----------------------------------------------------------------------------
// OS_ID StpClkDirPwmMbxId
//----------------------------------------------------------------------------
__inline OS_ID StpClkDirPwmMbxId(UCHAR ucUnit, ULONG ulState)
{
   OS_ID oiMbx= 0;
   
   if (ulState & AT91C_TC_CLKSTA)
   {
      // Acceleration mbx
      oiMbx= &MbxStpClkDirAcc[ucUnit];
   }
   else if (ulState & AT91C_TC_CPBS)
   {
      // Brake mbx
      oiMbx= &MbxStpClkDirBrk[ucUnit];
   }   

   return (oiMbx);
}

//----------------------------------------------------------------------------
// USHORT StpClkDirPwmQueueClear
//----------------------------------------------------------------------------
USHORT StpClkDirPwmQueueClear(UCHAR ucUnit, ULONG ulState)
{
   USHORT usResult= 0;
   OS_ID oiMbx= 0;
   
   ULONG ulValue;
   
   // Determine mbx for new data
   oiMbx= StpClkDirPwmMbxId(ucUnit, ulState);  
   if (oiMbx)
   {
      // Wipe all data
      do
      {
         usResult= OS_MBX_VALUE_CHK(os_mbx_wait(oiMbx, OS_MBX_VALUE_GET(ulValue), OS_T_NONE));
      }
      while (usResult);
   }
   
   // Success when read until error
   return (!usResult);
}

//----------------------------------------------------------------------------
// USHORT StpClkDirPwmQueueFree
//----------------------------------------------------------------------------
USHORT StpClkDirPwmQueueFree(UCHAR ucUnit, ULONG ulState)
{
   USHORT usResult= 0;
   OS_ID oiMbx= 0;
   
   // Determine mbx for new data
   oiMbx= StpClkDirPwmMbxId(ucUnit, ulState);  
   if (oiMbx)
   {
      usResult= os_mbx_check(oiMbx);
   }
   
   return (usResult);
}   

//----------------------------------------------------------------------------
// UCHAR StpClkDirPwmQueue
//----------------------------------------------------------------------------
UCHAR StpClkDirPwmQueue(UCHAR ucUnit, ULONG ulState, U16 u16Cmr, U16 u16PrdDty, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   OS_ID oiMbx= 0;
      
   ULONG ulValue;
   
   // Determine mbx for new data
   oiMbx= StpClkDirPwmMbxId(ucUnit, ulState);  
   if (oiMbx)
   {
      ulValue= (u16Cmr << 16) | (u16PrdDty);
   
      if (OS_MBX_VALUE_CHK(os_mbx_write(oiMbx, OS_MBX_VALUE_PUT(ulValue), u16Timeout)))
      {
         ucResult= 1;
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR StpClkDirStateSet
//----------------------------------------------------------------------------
UCHAR StpClkDirStateSet(UCHAR ucUnit, ULONG ulState, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   OS_ID oiMbx= 0;
   
   if (ucUnit < STPCLKDIR_UNIT_MAX)
   {
      oiMbx= StpClkDirPwmMbxId(ucUnit, ulState);
      if (oiMbx)
      {     
         if (OS_MBX_CHECK(os_mbx_write(&MbxStpClkDirState[ucUnit], OS_MBX_VALUE_PUT(ulState), u16Timeout)))
         {
            // Enable pwm channel IRQ (which retrieves the above msg and does the processing)
            AT91F_PWMC_InterruptEnable(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);
            
            ucResult= 1;
         }
      }
   }
      
   return (ucResult);
}

//----------------------------------------------------------------------------
// void IrqStpClkDirPwmHandler
//----------------------------------------------------------------------------
void StpClkDirPwmIrqHandler(UCHAR ucChid)
{
   static ULONG ulState= 0;
   OS_ID oiMbx= 0;

   register bool bPwmUpdate;

   register UCHAR ucUnit;
   register UCHAR ucPwmc;
   register UCHAR ucIndex;
   
#if STPCLKDIR_PWM_UPDATE_FIX > 0
   register USHORT usRetry;
   register U32 u32PwmCcntr;
#endif

   register U32 u32PwmCmr;
   register U32 u32PwmCupdr;
      
   ULONG ulValue;
     
   // Determine unit of channel id (chid)
   for (ucIndex= 0; ((ucIndex < STPCLKDIR_UNIT_MAX) && (SStpClkDirRO[ucIndex].ucPwmChannelId != ucChid)); ucIndex++);     
   if (SStpClkDirRO[ucIndex].ucPwmChannelId == ucChid)
   {
      ucUnit= ucIndex;

      // Check for new state (only states known by StpClkDirPwmMbxId work!!)
      if (ISR_MBX_VALUE_CHK(isr_mbx_read(&MbxStpClkDirState[ucUnit], ISR_MBX_VALUE_GET(ulValue))))
      {
         // State change
         ulState= ulValue;
         
         // State indicator update handled by StpClkDirTccIrqHandler (except AT91C_TC_CLKSTA -> AT91C_TC_CPAS)
      }

      // Determine mbx of new data to set
      oiMbx= StpClkDirPwmMbxId(ucUnit, ulState);
      if (oiMbx)
      {
         bPwmUpdate= FALSE;
         if (ISR_MBX_VALUE_CHK(isr_mbx_read(oiMbx, ISR_MBX_VALUE_GET(ulValue))))
         {
            // Prepare update of PWM hardware registers
            
            u32PwmCmr= ((ulValue & BITS_ULONGHX) >> 16);
            u32PwmCupdr= (ulValue & BITS_ULONGLX);
            
            // Only non-zero values are allowed by hardware
            if (u32PwmCupdr)
            {
               ucPwmc= PWM_CHID2UNIT(SStpClkDirRO[ucUnit].ucPwmChannelId);

#if STPCLKDIR_PWM_UPDATE_FIX > 0
               // Set new values only for pwm_ccnt > 1 (Errata rev. a 41.3.6 p.653)
               
               for (usRetry= 0, u32PwmCcntr= SStpClkDirRO[ucUnit].pPwmBase->PWMC_CH[ucPwmc].PWMC_CCNTR; (SStpClkDirRO[ucUnit].pPwmBase->PWMC_IMR & ucChid) && (u32PwmCcntr < 2) && (usRetry < STPCLKDIR_PWM_UPDATE_FIX); u32PwmCcntr= SStpClkDirRO[ucUnit].pPwmBase->PWMC_CH[ucPwmc].PWMC_CCNTR)
               {
                  // Avoid deadlock
                  if (u32PwmCcntr == SStpClkDirRO[ucUnit].pPwmBase->PWMC_CH[ucPwmc].PWMC_CCNTR)
                  {
                     usRetry++;
                  }
               }
               
               // The workarround below from ATMEL will end in a deadlock for the 2nd move
               /*
               while ((SStpClkDirRO[ucUnit].pPwmBase->PWMC_IMR & ucChid) && \
                      (SStpClkDirRO[ucUnit].pPwmBase->PWMC_CH[ucPwmc].PWMC_CCNTR < 2));
               */
#endif

               SStpClkDirRO[ucUnit].pPwmBase->PWMC_CH[ucPwmc].PWMC_CMR= u32PwmCmr;
               SStpClkDirRO[ucUnit].pPwmBase->PWMC_CH[ucPwmc].PWMC_CUPDR= u32PwmCupdr;

#if STPCLKDIR_PWM_2NDCHANCEUPDATE_FIX > 0
               // Second chance
               SStpClkDirRO[ucUnit].pPwmBase->PWMC_CH[ucPwmc].PWMC_CMR= u32PwmCmr;
               SStpClkDirRO[ucUnit].pPwmBase->PWMC_CH[ucPwmc].PWMC_CUPDR= u32PwmCupdr;
#endif

               // Update RW value u32PwmCmrCupdr
               SStpClkDirRW[ucUnit].u32PwmCmrCupdr= ulValue;
            
               bPwmUpdate= TRUE;
            }
            else
            {
               // Debug
               ulValue= ulValue;
            }
         }
         
         if (!bPwmUpdate)
         {
            // State indicator forward change after completed acc/brk plus one clock
            switch (ulState)
            {
               case AT91C_TC_CLKSTA:
                  // Acc complete - run active now
                  ulState= AT91C_TC_CPAS;

                  // State indicator update
                  SStpClkDirRW[ucUnit].u32PwmState= ulState;
            
                  // Trigger pending speed change
                  if (!ISR_SEM_TAKE_CHK(isr_sem_check(SStpClkDirRW[ucUnit].SemRun)))
                  {
                     isr_sem_give(SStpClkDirRW[ucUnit].SemRun);
                  }
   
                  break;
                  
               case AT91C_TC_CPBS:
                  // Brk complete - move complete yet (AT91C_TC_CPCS)
                  // Handled by StpClkDirTccIrqHandler (no additional clock after brk possible)
                  break;
            }

            // Disable pwm channel IRQ
            AT91F_PWMC_InterruptDisable(SStpClkDirRO[ucUnit].pPwmBase, SStpClkDirRO[ucUnit].ucPwmChannelId);
         }
      }
      else
      {
         // Debug
         oiMbx= oiMbx;
      }
   }
   
   // ARM needs EXPLICIT acknowledge (qualifier __irq doesn't do this) for (E)nd (O)f (I)RQ!!
   EOI;   
}

