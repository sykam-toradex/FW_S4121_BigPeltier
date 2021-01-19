//!	hardware descriptor(s) of AT91SAM7X STPCLKDIR driver
/**
		\file		stpclkdirdev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIRDEV.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-15 initial version
// 2010-01-21 added u16Timeout
// 2010-01-27 added ucDac... SpiUnit, SpiChannel and Address to SStpClkDirRO
// 2010-01-27 added ucGpio... SpiUnit, SpiChannel and Address to SStpClkDirRO
// 2010-01-28 added ucMs2Ms1Value, ucMicrosteps
// 2010-01-28 removed ucGpio... SpiUnit, SpiChannel and added ucGpioUnit
// 2010-01-28 removed ucGpioAddress, Ms1 & Ms2 para
// 2010-01-28 added usDacValueHold, usDacValueRun
// 2010-02-11 changed usDacValueRun from 1023 to 512
// 2010-02-12 added ucDacValueDelayMS
// 2010-02-24 renamed u16GpioPinNRes to u16GpioPinResMask
// 2010-02-24 added u16GpioResHold & u16GpioResRun to _SStpClkDirRO
// 2010-02-24 added ucGpioPinResDelayMS to _SStpClkDirRO
// 2010-02-26 added ucFlags to _SStpClkDirRO
// 2010-03-03 removed ucMicrosteps of _SStpClkDirRO
// 2010-03-04 added ulClkStableHz to _SStpClkDirRO
// 2010-03-11 added ulClkModulo to _SStpClkDirRO
// 2010-05-06 added STPCLKDIR_DAC_HOLD, STPCLKDIR_DAC_RUN & STPCLKDIR_MODULO
// 2010-05-07 changed ucTskPriMsg to OS_P_HIGH
// 2010-07-03 renamed ulClkModulo to u32ClkModulo in data comments
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-07-29 changed ucTskPriMsg from OS_P_HIGH to OS_P_ABOVENORMAL
// 2010-09-16 removed fixed settings usDacValueHold and usDacValueRun
// 2010-10-13 fixed ulClkStableHz to match stepper drivers specification
// 2010-10-13 fixed u16Gpio... params for 2nd and 3rd stepper device
// 2010-11-05 changed flags setting for application preset
// 2010-11-16 added new mailbox MbxEncGapDiskOffset to queue edge offsets
// 2011-03-30 renamed ucGpioPinResDelayMS to ucResetDelayMS
// 2011-03-30 renamed ucDacValueDelayMS to ucDacDelayMS
// 2011-03-30 removed pPioBase & u32PioPinDir from _SStpClkDirRO
// 2011-03-30 removed all ...Gpio para from _SStpClkDirRO
// 2011-03-30 removed ucDacUnit & ucDacChannel from _SStpClkDirRO
// 2011-03-30 decreased ucResetDelayMS to zero (disabled)
// 2011-09-15 changed ucResetDelayMS &  ucResetDelayMS to 50 (ms)
// 2011-09-22 changed ucResetDelayMS &  ucResetDelayMS to 10 (ms)
// 2011-10-19 added public doxygen documentation
// 2011-11-17 added digraphs of data for public doxygen documentation
// 2013-08-22 changed delays/timeouts from milliseconds to ticks (_OS_T_MS for compatibility)
// 2014-10-15 added doxygen documentation for state AT91C_TC_CPAS
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRDEV_H
#define SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRDEV_H 1


// Declare arrays of mailboxes (size definitions are located inside stpclkdir.h)

//!   acceleration pwm value queue
/**
   data flow thru MbxStpClkDirAcc
   \dot
   digraph MbxStpClkDirAcc {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      StpClkDirMove [label="StpClkDirMove" URL="\ref StpClkDirMove"];
      StpClkDirPwmQueue [label="StpClkDirPwmQueue(AT91C_TC_CLKSTA)" URL="\ref StpClkDirPwmQueue"];
      MbxStpClkDirAcc [label="MbxStpClkDirAcc" style="filled"];
      StpClkDirPwmIrqHandler [label="StpClkDirPwmIrqHandler"];
      PwmChannel [label="PwmChannel" style="dashed" URL="\ref _SStpClkDirRO"];

      StpClkDirMove -> StpClkDirPwmQueue;
      StpClkDirPwmQueue -> MbxStpClkDirAcc;
      MbxStpClkDirAcc -> StpClkDirPwmIrqHandler;
      PwmChannel -> StpClkDirPwmIrqHandler [arrowhead="odot"];
   }
   \enddot
*/
os_mbx_declare_ex(MbxStpClkDirAcc, STPCLKDIR_MBXCNTACC, [STPCLKDIR_UNIT_MAX]);

//!   brake pwm value queue
/**
   data flow thru MbxStpClkDirBrk
   \dot
   digraph MbxStpClkDirBrk {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      StpClkDirMove [label="StpClkDirMove" URL="\ref StpClkDirMove"];
      StpClkDirPwmQueue [label="StpClkDirPwmQueue(AT91C_TC_CPBS)" URL="\ref StpClkDirPwmQueue"];
      MbxStpClkDirBrk [label="MbxStpClkDirBrk" style="filled"];
      StpClkDirPwmIrqHandler [label="StpClkDirPwmIrqHandler"];
      PwmChannel [label="PwmChannel" style="dashed" URL="\ref _SStpClkDirRO"];

      StpClkDirMove -> StpClkDirPwmQueue;
      StpClkDirPwmQueue -> MbxStpClkDirBrk;
      MbxStpClkDirBrk -> StpClkDirPwmIrqHandler;
      PwmChannel -> StpClkDirPwmIrqHandler [arrowhead="odot"];
   }
   \enddot
*/
os_mbx_declare_ex(MbxStpClkDirBrk, STPCLKDIR_MBXCNTBRK, [STPCLKDIR_UNIT_MAX]);

//!   pwm state control queue
/**
   data flow thru MbxStpClkDirState <br><br>#AT91C_TC_CLKSTA (AT91C_TC_CLK<b>STA</b>: <b>STA</b>rt move & <b>ST</b>art <b>A</b>cceleration / <b>ST</b>art speed ch<b>A</b>nge)<br>#AT91C_TC_CPAS (AT91C_TC_CP<b>AS</b>: <b>A</b>cceleration <b>S</b>uccessfully done / <b>A</b>fter <b>S</b>peed change done & st<b>A</b>rt <b>S</b>tandard move)<br>#AT91C_TC_CPBS (AT91C_TC_CP<b>BS</b>: <b>B</b>rake <b>S</b>tart)<br>#AT91C_TC_CPCS (AT91C_TC_CP<b>CS</b>: <b>C</b>omplete & <b>S</b>hutdown)<br>
   \dot
   digraph MbxStpClkDirState {
      node [shape="record" fontname="FreeSans" fontsize="10"];

      StpClkDirMove [label="StpClkDirMove" URL="\ref StpClkDirMove"];
      _StpClkDirStart [label="_StpClkDirStart" URL="\ref _StpClkDirStart"];
      StpClkDirSpeedChange [label="StpClkDirSpeedChange" URL="\ref StpClkDirSpeedChange"];
      StpClkDirBrk [label="StpClkDirBrk" URL="\ref StpClkDirBrk"];
      StpClkDirStateSetAcc [label="StpClkDirStateSet(AT91C_TC_CLKSTA)" style="dashed" URL="\ref StpClkDirStateSet"];
      StpClkDirStateSetBrk [label="StpClkDirStateSet(AT91C_TC_CPBS)" URL="\ref StpClkDirStateSet"];
      AT91C_TC_CLKSTA [label="State= AT91C_TC_CLKSTA" style="dashed"];
      AT91C_TC_CPBS [label="State= AT91C_TC_CPBS" style="dashed"];
      StpClkDirTccIrqHandler [label="StpClkDirTccIrqHandler"];
      MbxStpClkDirState [label="MbxStpClkDirState" style="filled"];
      StpClkDirPwmIrqHandler [label="StpClkDirPwmIrqHandler"];
      PwmChannel [label="PwmChannel" style="dashed" URL="\ref _SStpClkDirRO"];
           
      StpClkDirSpeedChange -> StpClkDirStateSetAcc;
      StpClkDirStateSetAcc -> MbxStpClkDirState;
      
      StpClkDirBrk -> StpClkDirStateSetBrk;
      StpClkDirStateSetBrk -> MbxStpClkDirState;
      
      StpClkDirMove -> _StpClkDirStart;
      _StpClkDirStart -> AT91C_TC_CLKSTA;
      AT91C_TC_CLKSTA -> MbxStpClkDirState;
           
      StpClkDirTccIrqHandler -> AT91C_TC_CPBS;
      AT91C_TC_CPBS -> MbxStpClkDirState;
      
      MbxStpClkDirState -> StpClkDirPwmIrqHandler;
      PwmChannel -> StpClkDirPwmIrqHandler [arrowhead="odot"];
   }
   \enddot
   \dot
   digraph State {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      State [label="{State | {AT91C_TC_CLKSTA\nSTArt move & STart Acceleration / STart speed chAnge| AT91C_TC_CPAS\n  Acceleration Successfully done / After Speed change done & stArt Standard move | AT91C_TC_CPBS\nBrake Start | AT91C_TC_CPCS\nComplete & Shutdown}}"];
   \enddot
*/
os_mbx_declare_ex(MbxStpClkDirState, STPCLKDIR_MBXCNTSTATE, [STPCLKDIR_UNIT_MAX]);

//!   position offset queue (filled by external module like encgapdiskext.h) to shift internal required position at move's end WITHOUT further movement
/**
   data flow thru MbxStpClkDirOffset
   \dot
   digraph MbxStpClkDirOffset {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      StpClkDirOffsetPush [label="StpClkDirOffsetPush" URL="\ref StpClkDirOffsetPush"];
      MbxStpClkDirOffset [label="MbxStpClkDirOffset" style="filled"];
      TskStpClkDirHold [label="TskStpClkDirHold" shape="ellipse"];
      StpClkDirClksAbsShiftSet [label="StpClkDirClksAbsShiftSet[Ex]" shape="invtrapezium" URL="\ref StpClkDirClksAbsShiftSet"];
      SemHold [label="SStpClkDirRW.SemHold" style="dashed" URL="\ref _SStpClkDirRW"];
      
      StpClkDirOffsetPush -> MbxStpClkDirOffset;
      MbxStpClkDirOffset -> TskStpClkDirHold;
      SemHold -> TskStpClkDirHold [arrowhead="odot"];
      TskStpClkDirHold -> StpClkDirClksAbsShiftSet;
   }
   \enddot
*/
os_mbx_declare_ex(MbxStpClkDirOffset, STPCLKDIR_MBXOFFSETCNTMSG, [STPCLKDIR_UNIT_MAX]);


// Customized stacks
OS_TSK_STACK_USER_EX(StkTskStpClkDirHold, STPCLKDIR_STACK_STKTSKSTPCLKDIRHOLD, [STPCLKDIR_UNIT_MAX]);


//! Hardware descriptors
struct _SStpClkDirRO SStpClkDirRO[STPCLKDIR_UNIT_MAX] =
{
   {
      /* pTcbBase            */ AT91C_BASE_TCB,
      /* pTccBase            */ AT91C_BASE_TC0,
      /* u32TcbPeriphId      */ AT91C_ID_TC0,
      /* u16TccCmrLsb        */ AT91C_TC_CLKS_XC0 | AT91C_TC_EEVT_XC0,
      /* u8TcbBmrLsb         */ AT91C_TCB_TC0XC0S_TCLK0,
      /* u8TcbBmrMask        */ AT91C_TCB_TC0XC0S,
      /* IsrTcc              */ IsrStpClkDirTcc0,
      /* pPwmBase			     */ AT91C_BASE_PWMC,
      /* ucPwmChannelId      */ AT91C_PWMC_CHID1,
      /* u32PwmPeriphId      */ AT91C_ID_PWMC,
      /* ucResetDelay        */ _OS_T_MS(10, OS_TICK_RATE),
      /* ucDacDelay          */ _OS_T_MS(10, OS_TICK_RATE),
      /* ulClkStableHz       */ 1000000,
      /* u32ClkModulo        */ STPCLKDIR0_MODULO,
      /* u16Timeout          */ _OS_T_MS(100, OS_TICK_RATE),
      /* ucTskPriMsg         */ OS_P_ABOVENORMAL,
      /* ucFlags             */ STPCLKDIR0_FLAGS /* STPCLKDIR_FLAGS_AUTOPOWEROFF *//* | STPCLKDIR_FLAGS_INVERSE */
   }
#if STPCLKDIR_UNIT_MAX > 1
   ,
   {
      /* pTcbBase            */ AT91C_BASE_TCB,
      /* pTccBase            */ AT91C_BASE_TC1,
      /* u32TcbPeriphId      */ AT91C_ID_TC1,
      /* u16TccCmrLsb        */ AT91C_TC_CLKS_XC1 | AT91C_TC_EEVT_XC1,
      /* u8TcbBmrLsb         */ AT91C_TCB_TC1XC1S_TCLK1,
      /* u8TcbBmrMask        */ AT91C_TCB_TC1XC1S,
      /* IsrTcc              */ IsrStpClkDirTcc1,
      /* pPwmBase			     */ AT91C_BASE_PWMC,
      /* ucPwmChannelId      */ AT91C_PWMC_CHID2,
      /* u32PwmPeriphId      */ AT91C_ID_PWMC,
      /* ucResetDelay        */ _OS_T_MS(10, OS_TICK_RATE),
      /* ucDacDelay          */ _OS_T_MS(10, OS_TICK_RATE),
      /* ulClkStableHz       */ 1000000,
      /* u32ClkModulo        */ 200* 16,
      /* u16Timeout          */ _OS_T_MS(100, OS_TICK_RATE),
      /* ucTskPriMsg         */ OS_P_ABOVENORMAL,
      /* ucFlags             */ STPCLKDIR1_FLAGS
   }
#endif
#if STPCLKDIR_UNIT_MAX > 2
   ,
   {
      /* pTcbBase            */ AT91C_BASE_TCB,
      /* pTccBase            */ AT91C_BASE_TC2,
      /* u32TcbPeriphId      */ AT91C_ID_TC2,
      /* u16TccCmrLsb        */ AT91C_TC_CLKS_XC2 | AT91C_TC_EEVT_XC2,
      /* u8TcbBmrLsb         */ AT91C_TCB_TC2XC2S_TCLK2,
      /* u8TcbBmrMask        */ AT91C_TCB_TC2XC2S,
      /* IsrTcc              */ IsrStpClkDirTcc2,
      /* pPwmBase			     */ AT91C_BASE_PWMC,
      /* ucPwmChannelId      */ AT91C_PWMC_CHID3,
      /* u32PwmPeriphId      */ AT91C_ID_PWMC,
      /* ucResetDelay        */ _OS_T_MS(10, OS_TICK_RATE),
      /* ucDacDelay          */ _OS_T_MS(10, OS_TICK_RATE),
      /* usDacValueHold      */ 410,
      /* usDacValueRun       */ 615,
      /* ulClkStableHz       */ 1000000,
      /* u32ClkModulo        */ 200* 16,
      /* u16Timeout          */ _OS_T_MS(100, OS_TICK_RATE),
      /* ucTskPriMsg         */ OS_P_ABOVENORMAL,
      /* ucFlags             */ STPCLKDIR2_FLAGS
   }
#endif
};

#endif
