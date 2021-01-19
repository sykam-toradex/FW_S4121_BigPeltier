//!	<b>INTERNAL</b> interface of AT91SAM7X STPCLKDIR driver
/**
		\file		stpclkdirint.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIRINT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-09-16 initial version (imported internal/private prototypes of ext.h)
// 2010-11-05 added _StpClkDirClksPosOffsGet
// 2010-11-19 imported IrqHandler support function from stpclkdir.h
// 2011-09-07 changed prototype of StpClkDirTccIrqHandlerClksUpdate
// 2011-09-15 added prototype of _StpClkDirMbxClear & StpClkDirPowerEx
// 2010-09-16 added prototype of _StpClkDirStart
// 2011-09-22 added prototype of StpClkDirPowerEx
// 2011-09-29 added prototype of StpClkDirClksAbsShiftSetEx
// 2011-11-10 added internal doxygen documentation
// 2011-12-01 added internal doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRINT_H
#define SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRINT_H 1


// Prototypes for internal interrupt / task handler functions

//!   \internal \brief <b>Internal interrupt handler function:</b> trigger brake of movement
void StpClkDirTccIrqHandlerBrake(UCHAR ucUnit);

//!   \internal \brief <b>Internal interrupt handler function:</b> prepare last 16 bit turn (of multi 16 bit turn)
void StpClkDirTccIrqHandlerPrepareLast(UCHAR ucUnit);

//!   \internal \brief <b>Internal interrupt (bIsr == #TRUE) or task (bIsr == #FALSE) handler function:</b> stop movement
/**
      \param	ucUnit					      number of the unit
      \param	bIsr				            interrupt (bIsr == #TRUE) or task (bIsr == #FALSE) mode
*/
void StpClkDirTccIrqHandlerStop(UCHAR ucUnit, bool bIsr);

//!   \internal \brief <b>Internal interrupt (bIsr == #TRUE) or task (bIsr == #FALSE) handler function:</b> calculate current position
/**
      \param	ucUnit					      number of the unit
      \param	*pu32Clk					      pointer to storage for the calculated <b>absolute position</b>
      \param	bIsr				            interrupt (bIsr == #TRUE) or task (bIsr == #FALSE) mode
      
      \return										> 0 for success, == 0 for failure
      
      \remarks
               - for task mode (stopped motor) <b>(bIsr == FALSE)</b> you usually specify the address of a <b>local var for *pu32Clk</b> (as done by #StpClkDirClksAbsGet)
               - for isr mode (running motor) <b>(bIsr == TRUE)</b> you usually specify the address <b>&SStpClkDirRW[ucUnit].SPermanent.u32ClkCntAbsolute</b> (as done by StpClkDirTccIrqHandler)
*/
bool StpClkDirTccIrqHandlerClksUpdate(UCHAR ucUnit, U32 *pu32Clk, bool bIsr);


// Prototypes for internal functions

//!   \internal \brief <b>Internal function:</b> determine OS_ID of mailbox to use for current state
/**
      \param	ucUnit					      number of the unit
      \param	ulState				         the state you need to queue data for (#AT91C_TC_CLKSTA [<b>STA</b>rt move / <b>ST</b>art speed ch<b>A</b>nge], #AT91C_TC_CPBS [<b>B</b>rake <b>S</b>tart])
      
      \return										OS_ID of mailbox responsible for specified state (> 0), == 0 for failure (unknown state)
*/
OS_ID StpClkDirPwmMbxId(UCHAR ucUnit, ULONG ulState);

//!   \internal \brief <b>Internal function:</b> clear mailbox used for current state
/**
      \param	ucUnit					      number of the unit
      \param	ulState				         the state you need to queue data for (#AT91C_TC_CLKSTA [<b>STA</b>rt move / <b>ST</b>art speed ch<b>A</b>nge], #AT91C_TC_CPBS [<b>B</b>rake <b>S</b>tart])
      
      \return										> 0 for success, == 0 for failure
*/
USHORT StpClkDirPwmQueueClear(UCHAR ucUnit, ULONG ulState);

//!   \internal \brief <b>Internal function:</b> retrieve number of free entries of mailbox used for specified state
/**
      \param	ucUnit					      number of the unit
      \param	ulState				         the state you need to queue data for (#AT91C_TC_CLKSTA [<b>STA</b>rt move / <b>ST</b>art speed ch<b>A</b>nge], #AT91C_TC_CPBS [<b>B</b>rake <b>S</b>tart])
      
      \return										number of free entries of mailbox used for specified state
*/
USHORT StpClkDirPwmQueueFree(UCHAR ucUnit, ULONG ulState);

//!   <b>Internal function:</b> en-queue data in mailbox used by current state
/**
      \param	ucUnit					      number of the unit
      \param	ulState				         the state you need to queue data for (#AT91C_TC_CLKSTA [<b>STA</b>rt move / <b>ST</b>art speed ch<b>A</b>nge], #AT91C_TC_CPBS [<b>B</b>rake <b>S</b>tart])
      \param   u16Cmr                     lower 16 bits to queue for the the Channel-Mode-Register (esp. the lower 4 bits for the Channel-PREscaler) <b>|</b> #AT91C_PWMC_CPD
      \param   u16PrdDty                  value to queue for the the Channel-PeRioD-Register
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
      
      \remarks
               - because the pwm seamless frequency modulation supports only period OR duty at a time the PWM driver is configured here to keep the duty constant/stable and modifies only the period (u16Cmr <b>|</b> #AT91C_PWMC_CPD)
*/
UCHAR StpClkDirPwmQueue(UCHAR ucUnit, ULONG ulState, U16 u16Cmr, U16 u16PrdDty, U16 u16Timeout);

//!   <b>Internal function:</b> set state of movement (for StpClkDirTccIrqHandler)
/**
      \param	ucUnit					      number of the unit
      \param	ulState				         the state to set (#AT91C_TC_CLKSTA [<b>STA</b>rt move / <b>ST</b>art speed ch<b>A</b>nge], #AT91C_TC_CPBS [<b>B</b>rake <b>S</b>tart], #AT91C_TC_CPCS [<b>C</b>omplete & <b>S</b>hutdown])
      \param	u16Timeout				      <b>unused</b>, just present for easy replacement of used <b>os_mbx_write</b>
      
      \return										number of free entries of mailbox used for specified state
*/
UCHAR StpClkDirStateSet(UCHAR ucUnit, ULONG ulState, U16 u16Timeout);


// Prototypes for internal INLINE functions

//!   \internal \brief <b>Internal inline function:</b> get position offset part <b>(NOT a absolute/composed position!) WITHOUT resouce locking</b>
ULONG _StpClkDirClksPosOffsGet(UCHAR ucUnit);

//!   \internal \brief <b>Internal inline function:</b> clear all mailboxes used for movement
/**
      \param	ucUnit					      number of the unit
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
*/
UCHAR _StpClkDirMbxClear(UCHAR ucUnit, U16 u16Timeout);

//!   <b>Internal inline function:</b> low level movement start (supports multi 16 bit movements [* usRepeat] for true 32 bit movements and endless [bContinous == TRUE] movements)
/**
      \param	ucUnit					      number of the unit
      \param	bDir					         direction of movement (#STPCLKDIR_CLOCKWISE, #STPCLKDIR_COUNTERCLOCKWISE of stpclkdir.h)
      \param   u16TccRa                   value to set in timer's "match a" register [#_AT91S_TC -> TC_RA] (end of acceleration [start of constant move speed] clock count compare match trigger)
      \param   u16TccRb                   value to set in timer's "match b" register [#_AT91S_TC -> TC_RB] (end of constant move speed [start of brake] clock count compare match trigger)
      \param   u16TccRc                   value to set in timer's "match c" register [#_AT91S_TC -> TC_RC] (end of movement [clock shut down] clock count compare match trigger)
      \param   usRepeat                   number of repeats/overruns of the 16 bit hardware clock counter
      \param   bContinous                 mode of movement (#FALSE == clocks count limited move, #TRUE == unlimited/endless move which needs additional stop)
      \param   u16PwmCmr                  initial lower 16 bits to set for the pwm's Channel-Mode-Register (esp. the lower 4 bits for the Channel-PREscaler) [#_AT91S_PWMC_CH -> PWMC_CMR]
      \param   u16PwmCprdr                initial value to set to for the pwm's Channel-PeRioD-Register [#_AT91S_PWMC_CH -> PWMC_CPRDR]
      \param   u16PwmCdtyr                initial value to set for pwm's Channel-DuTY-Register [#_AT91S_PWMC_CH -> PWMC_CDTYR]
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \remarks
               - to stop a continous move use #StpClkDirBrk (with brake clocks) or #StpClkDirQuit (immediate stop)
               - for minimum clocks required for acceleration, brake and the full movement refer to #StpClkDirMove and #StpClkDirMoveMax
               - for easy calculation of the pwm values use #PwmCalcCprePrdDty
*/
UCHAR _StpClkDirStart(UCHAR ucUnit, bool bDir, U16 u16TccRa, U16 u16TccRb, U16 u16TccRc, USHORT usRepeat, bool bContinous, U16 u16PwmCmr, U16 u16PwmCprdr, U16 u16PwmCdtyr, U16 u16Timeout);

//!   \internal \brief <b>Internal inline function:</b> extended motor power control <b>(supports already locked resource)</b>
/**
      \param	ucUnit					      number of the unit
      \param	bPower					      power state <b>(DRIVER)</b> to set (#TRUE == active, #FALSE == disabled)
      \param   bRun                       run state <b>(DAC)</b> to set (#TRUE == run, #FALSE == hold)
      \param   bLocked                    #TRUE == resources are assumed already locked <b>(and are NOT locked/unlocked)</b>, #FALSE == lock and unlock resources
      \param	u16Timeout				      <b>for bLocked == FALSE</b>: maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks), <b>for bLocked == #TRUE:</b> unused
      
      \return										> 0 for success, == 0 for failure

      \remarks
               - power may automatically disabled at end of movement using flag/option #STPCLKDIR_FLAGS_AUTOPOWEROFF (stpclkdir.h)
               - this function clears the internal <b>microstep after driver powered on</b> counter when turning the driver off (#_SStpClkDirRW -> u32ClkCntDriver)
*/
USHORT StpClkDirPowerEx(UCHAR ucUnit, bool bPower, bool bRun, bool bLocked, U16 u16Timeout);

//!   \internal \brief <b>Internal inline function:</b> extended shift <b>absolute/composed</b> (absolute + offset) position <b>(supports already locked resource)</b>
/**
      \param	ucUnit					      number of the unit
      \param   lOffset                    value to shift <b>absolute/composed</b> (absolute + offset) position
      \param   bLocked                    #TRUE == resources are assumed already locked <b>(and are NOT locked/unlocked)</b>, #FALSE == lock and unlock resources
      \param   u16Timeout                 <b>for bLocked == #FALSE</b>: maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks), <b>for bLocked == #TRUE:</b> unused
      
      \return										> 0 for success, == 0 for failure
*/
UCHAR  StpClkDirClksAbsShiftSetEx(UCHAR ucUnit, LONG lOffset, bool bLocked, U16 u16Timeout);

#endif
