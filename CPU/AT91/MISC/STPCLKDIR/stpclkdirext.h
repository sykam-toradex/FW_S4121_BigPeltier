//!	external interface of AT91SAM7X STPCLKDIR driver
/**
		\file		stpclkdirext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIREXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-15 initial version
// 2010-01-20 added StpClkDirStart
// 2010-01-29 added StpClkDirPwmQueue
// 2010-02-04 added StpClkDirPwmMbxId
// 2010-02-05 added StpClkDirQuit, StpClkDirBrkNow, StpClkDirStateSet
// 2010-02-10 renamed StpClkDirBrkNow to StpClkDirBrk
// 2010-02-24 added StpClkDirMove
// 2010-02-25 added StpClkDirClksGet, StpClkDirHzGet and StpClkDirSpeedChange
// 2010-02-26 added StpClkDirJobClksGet and StpClkDirPwmQueueFree
// 2010-02-26 added StpClkDirPower
// 2010-03-04 added ulClkStableHz to _SStpClkDirRO
// 2010-03-04 added StpClkDirWait
// 2010-03-11 added StpClkDirClksAbsGet & StpClkDirClksPosOffsGet/Set
// 2010-03-12 added StpClkDirClksPosOffsZeroGet
// 2010-03-12 changed StpClkDirClksPosOffs... to ULONG offset
// 2010-07-03 added function StpClkDirModuloClksGet
// 2010-07-08 added function StpClkDirPwmQueueClear
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-07-29 changed param names of StpClkDirSpeedChange
// 2010-09-16 changed prototype of StpClkDirInit (added initial dac values)
// 2010-09-16 added prototypes for StpClkDirDacGet & StpClkDirDacSet
// 2010-09-16 added prototypes for StpClkDirClksAbsRawSet/Get
// 2010-09-16 StpClkDirClksPosOffsSet now locks/unlocks the resource
// 2010-09-16 added prototype for StpClkDirClksPosOffsGet
// 2010-09-16 added prototypes for StpClkDirSnapShot, -SizeGet & -AddrGet
// 2010-11-03 changed prototype of StpClkDirStart and StpClkDirMove
// 2010-11-16 added new special purpose function StpClkDirOffsetPush
// 2010-11-18 added new common function StpClkDirClksAbsSet
// 2010-11-19 added prototype for StpClkDirModuloMaxClksGet
// 2010-12-17 added new function StpClkDirProfileAdjust
// 2011-02-23 changed prototype of StpClkDirBrk
// 2011-02-24 added new function StpClkDirSpeedChangeWait
// 2011-03-02 changed prototype of StpClkDirBrk (undo last changes)
// 2011-03-17 added new function prototype StpClkDirMoveMax
// 2011-03-24 added StpClkDirInitEx prototype
// 2011-09-07 added u16Timeout para to StpClkDirClksPosOffsVal
// 2011-09-07 added u16Timeout para to StpClkDirClksPosOffsMakePlus
// 2011-09-16 moved prototype of StpClkDirStart as _StpClkDirStart to ...int.h
// 2011-09-28 added new function prototype for StpClkDirClksAbsShiftGet
// 2011-09-29 added new function prototype for StpClkDirClksAbsShiftSet
// 2011-09-29 added new function prototype for StpClkDirMoveResultGet
// 2011-10-20 added public doxygen documentation
// 2011-10-26 added public doxygen documentation
// 2013-08-21 updated public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIREXT_H
#define SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIREXT_H 1


// Prototypes for functions

//!	initialize a unit (for default hardware)
/**
      \param	ucUnit					      number of the unit
      \param	usDacValueHold					dac value for hold current setting
      \param	usDacValueRun				   dac value for run current setting
      
      \return										> 0 for success, == 0 for failure

      \remarks
               - dac values may be changed later using #StpClkDirDacSet and are used for the <b>next movement's run/hold</b>
*/
UCHAR StpClkDirInit(UCHAR ucUnit, USHORT usDacValueHold, USHORT usDacValueRun);

//!	initialize a unit with custom (auxiliary) functions for custom hardware
/**
      \param	ucUnit					      number of the unit
      \param	usDacValueHold					dac value for hold current setting
      \param	usDacValueRun				   dac value for run current setting
      \param   *pAuxDir                   (optional, set NULL as default) pointer to custom direction control function (e.g. stpclkdirauxext.h)
      \param   *pAuxPwr                   (optional, set NULL as default) pointer to custom power control function (e.g. stpclkdirauxext.h)
      \param   *pAuxDac                   (optional, set NULL as default) pointer to custom dac/current control function (e.g. stpclkdirauxext.h)
      
      \return										> 0 for success, == 0 for failure

      \remarks
               - dac values may be changed later using #StpClkDirDacSet and are used for the <b>next movement's run/hold</b>
*/
UCHAR StpClkDirInitEx(UCHAR ucUnit, USHORT usDacValueHold, USHORT usDacValueRun, ULONG (*pAuxDir)(UCHAR ucUnit, bool bClockwise), ULONG (*pAuxPwr)(UCHAR ucUnit, bool bEnable), ULONG (*pAuxDac)(UCHAR ucUnit, bool bRun));

//!	control the power of the driver
/**
      \param	ucUnit					      number of the unit
      \param	bPower					      power state <b>(DAC/DRIVER)</b> to set (#TRUE == run/active, #FALSE == hold/disabled)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \remarks
               - power may automatically disabled at end of movement using flag/option #STPCLKDIR_FLAGS_AUTOPOWEROFF (stpclkdir.h)
               - this function clears the internal <b>microstep after driver powered on</b> counter when turning the driver off (#_SStpClkDirRW -> u32ClkCntDriver)
*/
USHORT StpClkDirPower(UCHAR ucUnit, bool bPower, U16 u16Timeout);

//!   shrinks given acceleration-run-brake profile length to fit for lower clocks move count <b>(run/high clocks are changed to keep edge steepness and torque untouched)</b>
/**
      \param	ucUnit					      number of the unit
      \param	ulClks					      count of clocks for movement
      \param	*pulClksAcc				      pointer to count of acceleration clocks (will be modified)
      \param	*pulClksBrk				      pointer to count of brake clocks (will be modified)
      \param	ulClkLowHz				      start/low clocks frequency
      \param	*pulClkHighHz				   pointer to run/high clocks frequency (will be modified to reflect the max available frequency for this shrinked profile)
      
      \return										> 0 for success, == 0 for failure

      \remarks
               - assign the current profile values to *pulClksAcc, *pulClksBrk and *pulClkHighHz which are just replaced here by the adjusted values for successful return
*/
bool  StpClkDirProfileAdjust(UCHAR ucUnit, ULONG ulClks, ULONG *pulClksAcc, ULONG *pulClksBrk, ULONG ulClkLowHz, ULONG *pulClkHighHz);

//!   generate movement clocks with direction control with almost zero cpu-load <b>(for movements without further speed up change)</b>
/**
      \param	ucUnit					      number of the unit
      \param	bDir					         direction of movement (#STPCLKDIR_CLOCKWISE, #STPCLKDIR_COUNTERCLOCKWISE of stpclkdir.h)
      \param	ulClks				         lenght of the complete movement in clocks (for min clocks refert to #STPCLKDIR_CLKSRUN_MIN of stpclkdir.h)
      \param	usClksAcc				      lenght of the acceleration part of movement in clocks (for min clocks refert to #STPCLKDIR_CLKSACC_MIN of stpclkdir.h)
      \param	usClksBrk				      lenght of the brake part of movement in clocks (for min clocks refert to #STPCLKDIR_CLKSBRK_MIN of stpclkdir.h)
      \param	ulClkLowHz				      clock frequency [Hz] of start and end (low) of movement
      \param	ulClkHighHz				      run (high) clocks frequency [Hz] movement is accellerating to / braking from
      \param   usMicroSteps               microsteps per full-step <b>information, NOT the hardware setting</b> (which is usually done in "at91sam7x.c") for the #STPCLKDIR_FULLSTEPSHIFT calculation option (stpclkdir.h)
      \param   bContinous                 mode of movement (#FALSE == clocks count limited move, #TRUE == unlimited/endless move which needs additional stop)
      \param   *pucError                  pointer to detail error code storage (stpclkdirerr.h)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is already a running movement)

      \note
         - the enabled (default) #STPCLKDIR_FULLSTEPSHIFT option shortens or prolongs your movements to always end at a (power off/on safe) full-step positon
         - to adjust the <b>current position</b> to the <b>required position</b> after the movement the option #STPCLKDIR_OFFSETAUTOSHIFT is required (default)
         - the #STPCLKDIR_OFFSETAUTOSHIFT option requires an encoder module (e.g. EncGapDisk) which determines offsets it stores using #StpClkDirOffsetPush
         - to stop a continous move use #StpClkDirBrk (with brake clocks) or #StpClkDirQuit (immediate stop)
         
      \remarks
               - The return code above reflects only the result of the initiation of a movement which is executed at almost zero-cpu load asyncronously. Therefore use #StpClkDirMoveResultGet to get the final return code of the completed movement.
*/
UCHAR StpClkDirMove(UCHAR ucUnit, bool bDir, ULONG ulClks, USHORT usClksAcc, USHORT usClksBrk, ULONG ulClkLowHz, ULONG ulClkHighHz, USHORT usMicroSteps, bool bContinous, UCHAR *pucError, U16 u16Timeout);

//!   generate movement clocks with direction control with almost zero cpu-load <b>(with speed up change support up to specified max clocks frequency)</b>
/**
      \param	ucUnit					      number of the unit
      \param	bDir					         direction of movement (#STPCLKDIR_CLOCKWISE, #STPCLKDIR_COUNTERCLOCKWISE of stpclkdir.h)
      \param	ulClks				         lenght of the complete movement in clocks (for min clocks refert to #STPCLKDIR_CLKSRUN_MIN of stpclkdir.h)
      \param	usClksAcc				      lenght of the acceleration part of movement in clocks (for min clocks refert to #STPCLKDIR_CLKSACC_MIN of stpclkdir.h)
      \param	usClksBrk				      lenght of the brake part of movement in clocks (for min clocks refert to #STPCLKDIR_CLKSBRK_MIN of stpclkdir.h)
      \param	ulClkLowHz				      start/low clocks frequency
      \param	ulClkHighHz				      run/high clocks frequency
      \param   ulClkMaxHz                 max clocks frequency available for later speed up changes using #StpClkDirSpeedChange
      \param   usMicroSteps               microsteps per full-step <b>information, NOT the hardware setting</b> (which is usually done in "at91sam7x.c") for the #STPCLKDIR_FULLSTEPSHIFT calculation option (stpclkdir.h)
      \param   bContinous                 mode of movement (#FALSE == clocks count limited move, #TRUE == unlimited/endless move which needs additional stop)
      \param   *pucError                  pointer to detail error code storage (stpclkdirerr.h)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is already a running movement)

      \note
               - the enabled (default) #STPCLKDIR_FULLSTEPSHIFT option shortens or prolongs your movements to always end at a (power off/on safe) full-step positon
               - to adjust the <b>current position</b> to the <b>required position</b> after the movement the option #STPCLKDIR_OFFSETAUTOSHIFT is required (default)
               - the #STPCLKDIR_OFFSETAUTOSHIFT option requires an encoder module (e.g. EncGapDisk) which determines offsets it stores using #StpClkDirOffsetPush
               - to stop a continous move use #StpClkDirBrk (with brake clocks) or #StpClkDirQuit (immediate stop)
               - <b>ulClkMaxHz must be the highest frequency you're speeding up ever, otherwise the timing for the hardware driver gets to fast and movement will fail WITHOUT ANY error code</b>

      \remarks
               - The return code above reflects only the result of the initiation of a movement which is executed at almost zero-cpu load asyncronously. Therefore use #StpClkDirMoveResultGet to get the final return code of the completed movement.
*/
UCHAR StpClkDirMoveMax(UCHAR ucUnit, bool bDir, ULONG ulClks, USHORT usClksAcc, USHORT usClksBrk, ULONG ulClkLowHz, ULONG ulClkHighHz, ULONG ulClkMaxHz, USHORT usMicroSteps, bool bContinous, UCHAR *pucError, U16 u16Timeout);

//!   change the speed (up/down) of a running movement
/**
      \param	ucUnit					      number of the unit
      \param	usClksAcc					   lenght of the acceleration/brake part of movement in clocks (for min clocks refert to stpclkdir.h)
      \param	ulClkFromHz				      current movement speed from which to change to (use StpClkDirHzGet to get the current frequency when not already known)
      \param	ulClkToHz				      movement speed to change to
      \param	*pucError				      pointer to detail error code storage (stpclkdirerr.h)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is no running movement)

      \note
               - <b>do not try to speed up above ulClkHighHz (#StpClkDirMove) or ulClkMaxHz (#StpClkDirMoveMax)</b><br>otherwise the timing for the hardware driver gets to fast and <b>movement will fail WITHOUT ANY error code</b>
               - <b>for movements started by #StpClkDirMove you should only speed down, for movements started by #StpClkDirMoveMax you may also speed up upto ulClkMaxHz</b>
               - speed changes are possible as long the movement is in run state, but during acceleration/brake this function will return with failure (stpclkdir.h)
               - to get to know when acceleration is complete use #StpClkDirSpeedChangeWait to be triggered when the current speed change is complete and the driver is ready for your change
               - when calling #StpClkDirBrk the original (when the movement was started) brake ramp is used which may result in a unexspected speed up when you've changed below the original move frequency
               - to avoid unexspected speed up before brake you may do another speed change before #StpClkDirBrk to set original run frequency again before brake or just stop immediately using #StpClkDirQuit

      \remarks
               - for consecutive speed changes use #StpClkDirSpeedChangeWait to be triggered when the current speed change is complete and the driver is ready for the next one

      <b>Example code:</b><br>
      \include StpClkDirSpeedChange.c
*/
UCHAR StpClkDirSpeedChange(UCHAR ucUnit, USHORT usClksAcc, ULONG ulClkFromHz, ULONG ulClkToHz, UCHAR *pucError, U16 u16Timeout);

//!   brake a running movement
/**
      \param	ucUnit					      number of the unit
      \param	*pucError				      pointer to detail error code storage (stpclkdirerr.h)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is no running movement)

      \note
         - when calling #StpClkDirBrk the original (when the movement was started) brake ramp is used which may result in a unexspected speed up when you've changed below the original move frequency using #StpClkDirSpeedChange
         - to avoid unexspected speed up before brake you may do another speed change before #StpClkDirBrk to set original run frequency again before brake or just stop immediately using #StpClkDirQuit
*/
UCHAR StpClkDirBrk(UCHAR ucUnit, UCHAR *pucError, U16 u16Timeout);

//!   cancel a running movement immediately (without any brakeing)
/**
      \param	ucUnit					      number of the unit
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is no running movement)

      \remarks
               - use #StpClkDirQuit instead of #StpClkDirBrk to avoid unexspected speed up before brake when you've changed below the original move frequency using #StpClkDirSpeedChange before
*/
UCHAR StpClkDirQuit(UCHAR ucUnit, U16 u16Timeout);

//!   pend for completion of a previous StpClkDirSpeedChange
/**
      \param	ucUnit					      number of the unit
      \param	*pucError				      pointer to detail error code storage (stpclkdirerr.h)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is no running movement)

      \note
               - this function clears the used trigger semaphore (which would prevents multiple successful calls in a row) but sets a flag for fast re-checking call needs
      
      \remarks
               - for consecutive speed changes use #StpClkDirSpeedChangeWait to be triggered when the current speed change is complete and the driver is ready for the next one
*/
UCHAR StpClkDirSpeedChangeWait(UCHAR ucUnit, UCHAR *pucError, U16 u16Timeout);

//!   pend for completion of movement
/**
      \param	ucUnit					      number of the unit
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is no running movement)

      \note
               - #StpClkDirWait may be used to trigger application when movement (executed almost without cpu-load asyncronously) is complete and the driver is ready for the next one
               - the final result of a completed movement is returned by #StpClkDirMoveResultGet
         
      \remarks
               - to successfully pend (wait without cpu load) you need a u16Timeout value longer than the duration of the movement
*/
UCHAR StpClkDirWait(UCHAR ucUnit, U16 u16Timeout);


// Prototypes for functions to support permanent storage

//!   set position offset part to specified value <b>(NOT a absolute/composed position!)</b>
/**
      \param	ucUnit					      number of the unit
      \param   ulClkPosZeroOffset         value to set position offset part to <b>(NOT to a absolute/composed position!)</b>
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is a running movement)
*/
UCHAR StpClkDirClksPosOffsSet(UCHAR ucUnit, ULONG ulClkPosZeroOffset, U16 u16Timeout);

//!   get position offset part <b>(NOT a absolute/composed position!)</b>
/**
      \param	ucUnit					      number of the unit
      \param   *pulClkPosZeroOffset       storage for position offset part value <b>(NOT a absolute/composed position!)</b>
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is a running movement)
*/
UCHAR StpClkDirClksPosOffsGet(UCHAR ucUnit, ULONG *pulClkPosZeroOffset, U16 u16Timeout);

//!   set position absolute part to specified value <b>(NOT a absolute/composed position!)</b>
/**
      \param	ucUnit					      number of the unit
      \param   u32ClkCntAbsolute          value to set position absolute part to <b>(NOT to a absolute/composed position!)</b>
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is a running movement)
*/
UCHAR StpClkDirClksAbsRawSet(UCHAR ucUnit, ULONG u32ClkCntAbsolute, U16 u16Timeout);

//!   get position absolute part <b>(NOT a absolute/composed position!)</b>
/**
      \param	ucUnit					      number of the unit
      \param   *pu32ClkCntAbsolute        storage for position absolute part <b>(NOT a absolute/composed position!)</b>
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
*/
UCHAR StpClkDirClksAbsRawGet(UCHAR ucUnit, ULONG *pu32ClkCntAbsolute, U16 u16Timeout);

//!   set <b>absolute/composed</b> (absolute + offset) position
/**
      \param	ucUnit					      number of the unit
      \param   ulClksAbs                  value to set <b>absolute/composed</b> (absolute + offset) position to
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure (e.g. when there is a running movement)
*/
UCHAR StpClkDirClksAbsSet(UCHAR ucUnit, ULONG ulClksAbs, U16 u16Timeout);

//!   shift <b>absolute/composed</b> (absolute + offset) position
/**
      \param	ucUnit					      number of the unit
      \param   lOffset                    value to shift <b>absolute/composed</b> (absolute + offset) position
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
*/
UCHAR StpClkDirClksAbsShiftSet(UCHAR ucUnit, LONG lOffset, U16 u16Timeout);

//!   get last determined shift offset for <b>absolute/composed</b> (absolute + offset) position
/**
      \param	ucUnit					      number of the unit
      \param   *plOffset                  last determined value to shift <b>absolute/composed</b> (absolute + offset) position
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										count of offsets received from encoder module to calculate the offset (0 == no offsets received)

      \note
               - the last determined shift offset is not the last shift offseting done by #StpClkDirClksAbsShiftSet but the offset determined by the driver+encoder the absolute/composed (absolute + offset) position should be shifted
               - <b>the return code 0 is not strictly an error but you may interpret 0 received offsets as a warning or error in your application</b>
*/
ULONG StpClkDirClksAbsShiftGet(UCHAR ucUnit, LONG *plOffset, U16 u16Timeout);

//!   get result of movement
/**
      \param	ucUnit					      number of the unit
      \param   *pulResult                 the result of the last completed movement (> 0 for success, == 0 for failure)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
      
      \remarks
               - to successfully pend (wait without cpu load) you need a u16Timeout value longer than the duration of the movement
*/
UCHAR StpClkDirMoveResultGet(UCHAR ucUnit, ULONG *pulResult, U16 u16Timeout);

//!   set hold/run values for power current control dac
/**
      \param	ucUnit					      number of the unit
      \param	usDacValueHold					dac value for hold current setting
      \param	usDacValueRun				   dac value for run current setting
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
      
      \note
               - dac values changed are used for the <b>next movement's run/hold</b> (NOT immediately)
*/
UCHAR StpClkDirDacSet(UCHAR ucUnit, USHORT usDacValueHold, USHORT usDacValueRun, U16 u16Timeout);

//!   get hold/run values of power current control dac
/**
      \param	ucUnit					      number of the unit
      \param	*pusDacValueHold				storage for dac value of hold current setting
      \param	*pusDacValueRun				storage for dac value of run current setting
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
*/
UCHAR StpClkDirDacGet(UCHAR ucUnit, USHORT *pusDacValueHold, USHORT *pusDacValueRun, U16 u16Timeout);


// Prototypes for data snaphot

//!   get required size in bytes for snapshot of permanent data
/**    
      \return										required size in bytes for snapshot of permanent data (_SStpClkDirPermanent)
*/
size_t StpClkDirSnapShotSizeGet(void);

//!   get address of snapshot of permanent data
/**    
      \param	ucUnit					      number of the unit
      \return										address of snapshot of permanent data (_SStpClkDirPermanent)
*/
void * StpClkDirSnapShotAddrGet(UCHAR ucUnit);

//!   save/restore snapshot of permanent data
/**
      \param	ucUnit					      number of the unit
      \param	bRestore				         #TRUE == restore, #FALSE == save the permanent data (_SStpClkDirPermanent)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
*/
UCHAR StpClkDirSnapShot(UCHAR ucUnit, bool bRestore, U16 u16Timeout);


// Prototypes for special purpose functions (unsecured and therefore isr safe)

//!   set absolute/composed (absolute + offset) position to specified absolute/composed position by modifying offset part
/**
      \param	ucUnit					      number of the unit
      \param	ulVal				            value to set absolute/composed (absolute + offset) position to by modifying offset part
      \param	bSet				            #TRUE == set the calculated offset part, #FALSE == just return (and NOT set) the calculated offset part
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
      
      \remarks
               - the absolute/composed position is calculated by 'absolute + offset' which is used here to change the absolute/composed position by just setting another offset part (using integer overflow when lower absolute/composed position is needed)
*/
ULONG StpClkDirClksPosOffsVal(UCHAR ucUnit, ULONG ulVal, bool bSet, U16 u16Timeout);

//!   inverse sign of <b>absolute/composed</b> (absolute + offset) position
/**
      \param	ucUnit					      number of the unit
      \param	bSet				            #TRUE == set the calculated offset part, #FALSE == just return (and NOT set) the calculated offset part
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
      
      \remarks
               - the absolute/composed position is calculated by 'absolute + offset' which is used here to change the absolute/composed position by just setting another offset part (using integer overflow when lower absolute/composed position is needed)
*/
ULONG StpClkDirClksPosOffsMakePlus(UCHAR ucUnit, bool bSet, U16 u16Timeout);

//!   get current clock frequency of active movement
/**    
      \param	ucUnit					      number of the unit
      \return										last frequency move currently changed to
*/
ULONG StpClkDirHzGet(UCHAR ucUnit);

//!   get count of clocks of active movement <b>(NOT a absolute/composed position!)</b>
/**    
      \param	ucUnit					      number of the unit
      \return										count of clocks of active movement <b>(NOT a absolute/composed position!)</b>
*/
ULONG StpClkDirClksGet(UCHAR ucUnit);

//!   get absolute/composed position (supports active AND inactive movement)
/**    
      \param	ucUnit					      number of the unit
      \return										absolute/composed position
*/
ULONG StpClkDirClksAbsGet(UCHAR ucUnit);

//!   get total count of clocks of active movement (NOT a position)
/**    
      \param	ucUnit					      number of the unit
      \return										total count of clocks of active movement
*/
ULONG StpClkDirJobClksGet(UCHAR ucUnit);

//!   get count of modulo clocks (0= modulo disabled)
/**    
      \param	ucUnit					      number of the unit
      \return										count of modulo clocks (0= modulo disabled)
*/
ULONG StpClkDirModuloClksGet(UCHAR ucUnit);

//!   get max absolute/composed clocks position for current modulo value
/**    
      \param	ucUnit					      number of the unit
      \return										max absolute/composed clocks position for current modulo value
*/
ULONG StpClkDirModuloMaxClksGet(UCHAR ucUnit);

//!   write into offset queue to correct internal absolute/composed position data at end of movement
/**
      \param	ucUnit					      number of the unit
      \param	lOffset				         offset value to push/store for optional #STPCLKDIR_OFFSETAUTOSHIFT feature
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
      
      \remarks
               - This function is usually called by a encoder module (e.g. EncGapDisk) to push/store the determined offsets for the optional #STPCLKDIR_OFFSETAUTOSHIFT feature which adjusts the current to the required position after the movement
*/
OS_RESULT StpClkDirOffsetPush(UCHAR ucUnit, LONG lOffset, U16 u16Timeout);

#endif
