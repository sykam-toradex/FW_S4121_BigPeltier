//!	data definitions of AT91SAM7X STPCLKDIR driver
/**
		\file		stpclkdirdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIRDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-15 initial version
// 2010-01-21 added SemStart to _SStpClkDirRW
// 2010-01-27 added ucDac... SpiUnit, SpiChannel and Address to _SStpClkDirRO
// 2010-01-27 added ucGpio... SpiUnit, SpiChannel and Address to _SStpClkDirRO
// 2010-10-27 added u16GpioPinMs1, u16GpioPinMs2 and u16GpioPinRes
// 2010-01-28 added ucMs21Value, ucMicrosteps
// 2010-01-28 removed ucGpio... SpiUnit, SpiChannel and added ucGpioUnit
// 2010-01-28 removed ucGpioAddress, Ms1 & Ms2 para
// 2010-01-28 added usDacValueHold, usDacValueRun
// 2010-02-10 renamed u16Timeout to u16TimeoutMS (still unused yet)
// 2010-02-12 added ucDacValueDelayMS
// 2010-02-24 added u32ClkCntOffset to _SStpClkDirRW
// 2010-02-24 renamed u16GpioPinNRes to u16GpioPinResMask
// 2010-02-24 added u16GpioResHold & u16GpioResRun to _SStpClkDirRO
// 2010-02-24 added u32ClkCntOffset to _SStpClkDirRW
// 2010-02-26 added ucFlags to _SStpClkDirRO
// 2010-03-03 removed ucMicrosteps of _SStpClkDirRO
// 2010-03-04 added SemDone to _SStpClkDirRW
// 2010-03-04 added ulClkStableHz to _SStpClkDirRO
// 2010-03-11 added u32ClkModulo, u32ClkPosAbsolute & lClkPosOffset
// 2010-03-12 added ulClkIdPrevious & ulClkId
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-16 moved usDacValueHold & usDacValueRun from ...RO to ...RW
// 2010-09-16 added structure _SStpClkDirPermanent
// 2010-09-16 reorganized data structures (added permanent tree to rw data)
// 2010-11-17 added new var u32ClkCntDriver to _SStpClkDirRW
// 2010-11-23 removed absolete data item u32ClkCntPrevious
// 2010-11-23 added data comments
// 2011-02-24 added SemRun to _SStpClkDirRW
// 2011-03-30 renamed ucGpioPinResDelayMS to ucResetDelayMS
// 2011-03-30 renamed ucDacValueDelayMS to ucDacDelayMS
// 2011-03-30 removed pPioBase & u32PioPinDir from _SStpClkDirRO
// 2011-03-30 removed all ...Gpio para from _SStpClkDirRO
// 2011-03-30 removed ucDacUnit & ucDacChannel from _SStpClkDirRO
// 2011-09-07 fixed comments for ulClkPosZeroOffset, u32ClkCntAbsolute & u32ClkCntOffset
// 2011-09-15 renamed semaphore SemStop to SemHold (triggers TskStpClkDirHold)
// 2011-09-28 added ulOffsetMsgCnt & ulTskStpClkDirHold to structure _SStpClkDirRW
// 2011-10-19 added public doxygen documentation
// 2011-11-24 added public doxygen documentation
// 2011-11-24 added internal doxygen documentation
// 2013-08-21 added bChanged to _SStpClkDirRW
// 2013-08-22 changed delays/timeouts from milliseconds to ticks
// 2014-10-22 moved local bStop from StpClkDirTccIrqHandler to _SStpClkDirRW
// 2014-10-22 moved local usCycle from StpClkDirTccIrqHandler to _SStpClkDirRW
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRDATA_H
#define SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRDATA_H 1


//! \internal \brief <b>Internal data:</b> Task parameter data
struct _SStpClkDirTsk
{
   UCHAR ucUnit;                                         //!< Unit task is assigned to
};

//! Hardware descriptor data (read only) for each unit
/**           
      <b>Example code: the following array is REQUIRED in "at91sam7x.c" to re-direct the pwm interrupt to stpclkdir's common pwm irq handler</b><br>
      \include    _SPwmItem.h
*/
struct _SStpClkDirRO
{
	AT91PS_TCB				pTcbBase;                        //!< Timer-counter block base to handle
	AT91PS_TC				pTccBase;                        //!< Timer-counter channel base to handle
	U32                  u32TcbPeriphId;                  //!< Bit-Id (!) of TCB to handle
	U16						u16TccCmrLsb;                    //!< Timer-counter channel CMR lower significant bank (user configurable)
	U8							u8TcbBmrLsb;                     //!< Timer-counter block BMW lower significant bank (user configurable)
   U8                   u8TcbBmrMask;                    //!< Timer-counter block BMW clear bit mask before OR-ing above data
   void (*IsrTcc) (void)__irq;                           //!< Isr handler to call
	AT91PS_PWMC				pPwmBase;                        //!< Pwm channel base to handle
	UCHAR						ucPwmChannelId;                  //!< Bit-Id (!) of pwm channel to handle
	U32                  u32PwmPeriphId;                  //!< Bit-Id (!) of pwm peripheral to handle
   UCHAR                ucResetDelay;                    //!< Delay after changing reset control <b>(in ticks)</b>
   UCHAR                ucDacDelay;                      //!< Delay after changing dac value <b>(in ticks)</b>
   ULONG                ulClkStableHz;                   //!< Constant width of clock's (not modulated) duty part
   U32                  u32ClkModulo;                    //!< Modulo value to enable translated (circular) positions
   U16                  u16Timeout;                      //!< Maximum cpu-load free resource pending time before any resource claim is canceled by the rtos <b>(in ticks)</b>
   UCHAR                ucTskPriMsg;                     //!< Priority of msg (hold) task
   UCHAR                ucFlags;                         //!< Flags/Options (see stpclkdir.h)
};

//! <b>Internal data:</b> Permanent runtime data (read/write) for each unit
struct _SStpClkDirPermanent
{
   ULONG                         ulClkPosZeroOffset;     //!< Value to add to u32ClkCntAbsolute to get 0 position (timer-counter <b>in</b>dependent)
   U32                           u32ClkCntAbsolute;      //!< Raw clock position counter (timer-counter IN_dependent) which is added to ulClkPosZeroOffset <b> to get the absolute/composed</b> (absolute + offset) position
   USHORT                        usDacValueHold;         //!< Dac value to set for hold mode
   USHORT                        usDacValueRun;          //!< Dac value to set for run mode
};

//! <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SStpClkDirRW
{
   U32                           u32PwmCmrCupdr;         //!< Mirror of written value of pwm's CMR (16bit MSB) and CUPD (16bit LSB)
   U32                           u32ClkCntOffset;        //!< Offset to get true 32bit _JOB's__RELATIVE_ value by adding the 16bit timer-counter (written by StpClkDirTccIrqHandler only)
   U32                           u32ClkCntDriver;        //!< Count of clocks sent to motor driver (for full step determination)
   ULONG                         ulClkId;                //!< Sequential number of current move
   ULONG                         ulClkIdPrevious;        //!< Sequential number of previous move
   U32                           u32PwmState;            //!< State for pwm clock generation data source (AT91C_TC_CLKST(A)/AT91C_TC_CP(B)S/AT91C_TC_CP(C)S)
   U32                           u32TccImr;              //!< Software IMR of timer-counter (because hardware does not monitor its IMR)
   LONG                          lOffsetMean;            //!< Mean value of post move correction offset
   ULONG                         ulOffsetMsgCnt;         //!< Count of edge messages received for last move lOffsetMean calculation based on
   struct _SStpClkDirPermanent   SPermanent;             //!< Permanent data (see above)
	U16						         u16TccRa;               //!< Copy of start value of timer-counter compare A (TCC_R(A))
	U16						         u16TccRb;               //!< Copy of start value of timer-counter compare B (TCC_R(B))
	U16						         u16TccRc;               //!< Copy of start value of timer-counter compare C (TCC_R(C))
	USHORT					         usRepeat;               //!< Count of required repeats for 16bit timer counter
   ULONG                         ulTskStpClkDirHold;     //!< Result of storage for TskStpClkDirHold
   USHORT                        usCycle;                //!< Current cycle counter of 16bit timer counter
   bool                          bRun;                   //!< Flag for run state
   bool                          bStop;                  //!< Flag for stop state
	bool						         bContinous;             //!< Flag for continuous mode (true) or single shot mode (false)
   bool                          bDirection;             //!< Flag for active direction (see stpclkdir.h)
   bool                          bDirectionPrevious;     //!< Flag of previous direction
   bool                          bChanged;               //!< Flag of previous speed change action done
   OS_SEM                        SemStart;               //!< Semaphore to lock started motor
   OS_SEM                        SemDone;                //!< Semaphore to lock motor move completion
   OS_SEM                        SemHold;                //!< Semaphore to trigger actions waiting on completed move
   OS_SEM                        SemRun;                 //!< Semaphore to trigger speed changes for acc completed and run started
   OS_TID                        tidTskStpClkDirHold;    //!< OS_TID of msg (hold) task
   ULONG (*pAuxDir)(UCHAR ucUnit, bool bClockwise);      //!< Function pointer for auxilary function to set direction of move
   ULONG (*pAuxPwr)(UCHAR ucUnit, bool bEnable);         //!< Function pointer for auxilary function to enable/disable power stage
   ULONG (*pAuxDac)(UCHAR ucUnit, bool bRun);            //!< Function pointer for auxilary function to set run/hold current
};

// Number of units located inside stpclkdir.h
struct _SStpClkDirRW SStpClkDirRW[STPCLKDIR_UNIT_MAX];
struct _SStpClkDirTsk SStpClkDirTsk[STPCLKDIR_UNIT_MAX];
struct _SStpClkDirPermanent SStpClkDirSnapShot[STPCLKDIR_UNIT_MAX];

#endif
