//!	external interface of AT91SAM7X RTT driver
/**
		\file		rttext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/RTT/RTTEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-10-13 initial version
// 2010-10-14 added RttMsg
// 2011-03-23 added new function RttSCKGet
// 2011-03-23 changed prototype of RttRestart (2nd para; x= 1 for x/1 = 1 Hz)
// 2011-06-30 changed prototype of RttRead for true success return code
// 2011-10-20 added public doxygen documentation
// 2011-10-26 added public doxygen documentation
// 2013-01-23 renamed RttSCKGet to RttSLCKGet (SLow ClocK)
// 2013-01-23 renamed RttSLCKGet para pu32SCK to pu32SLCK (SLow ClocK)
// 2013-02-06 fixed public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_RTT_RTTEXT_H
#define SYKAM_CPU_AT91_RTT_RTTEXT_H 1


// Prototypes for functions

//!   init a unit
/**
      \param	ucUnit					      number of the unit (specifies descriptor array index <b>[not the hardware index]</b> of rttdev.h)
      \param	oiMsgNewSem					   (optional, set to NULL as default) OS_ID of <b>pre-initialized (to 0!)</b> semaphore to send/give for matching messages
      \param	ulMsgMask				      (optional, set to NULL as default) and-mask to include messages for semaphore triggering (refer to u32IrqMask of _SRttRO)

      \note
         - 1 task is spanned by this function
         - message bits are filtered hardware bits (refer to <a href="#RttInit">AT91SAM7X manual 14.4.4 page 78: RTT_SR</a>) by u32IrqMsg of _SRttRO

      \return										> 0 for success, == 0 for failure
      
      <b>Manual reference(s):</b><br>
      <a name="RttInit">
      <a href="../../../MANUAL/AT91/AT91SAM7X/RTT/AT91SAM7X_14.4.4_p78_RTT_SR.pdf" type="application/pdf">AT91SAM7X manual 14.4.4 page 78: RTT_SR</a><br>
*/
bool   RttInit(UCHAR ucUnit, OS_ID oiMsgNewSem, ULONG ulMsgMask);

//!   restart a unit
/**
      \param	ucUnit					      number of the unit (specifies descriptor array index <b>[not the hardware index]</b> of rttdev.h)
      \param	usSecDiv					      divider for clock's tick prescaler (use x= 1 for x/1 for 1 Hz, for faster clock ticks use usSecDiv/1 Hz; e.g. use 2 for 2/1 Hz)
      \param	u32Almv				         alarm value in clock ticks
      \param	ulOffset                   offset to start incrementing from (e.g. for a restarted applicated)
      \param   ulIncDiv                   increment divider to adjust increment message frequency (e.g. use 1 for every increment, use 10 for every 10th increment)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
*/
bool   RttRestart(UCHAR ucUnit, USHORT usSecDiv, U32 u32Almv, ULONG ulOffset, ULONG ulIncDiv, U16 u16Timeout);

//!   get determined value for SLow ClocK frequency in Hz
/**
      \param	ucUnit					      number of the unit (specifies descriptor array index <b>[not the hardware index]</b> of rttdev.h)
      \param	*pu32SLCK					   storage for determined value for SLow ClocK frequency in Hz
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \remarks
               - this function is useful to get the 'from chip to chip varying' and 'temperature dependent' SLow ClocK frequency in Hz (re-determined on every call of #RttRestart)
*/
bool   RttSLCKGet(UCHAR ucUnit, U32 *pu32SLCK, U16 u16Timeout);

//!   set alarm value
/**
      \param	ucUnit					      number of the unit (specifies descriptor array index <b>[not the hardware index]</b> of rttdev.h)
      \param	u32Almv					      alarm value in clock ticks (tick units are set by #RttRestart)
      \param	bClear					      #TRUE == clear all prevously counted alarm messages, #FALSE == keep all prevously counted alarm messages
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
      
      \remarks
               - this function is useful to set a new alarm value wich triggers and counts a message (and optional -for fitting mask- gives your "news semaphore")
*/
bool   RttAlmSet(UCHAR ucUnit, U32 u32Almv, bool bClear, U16 u16Timeout);

//!   retrieve message
/**
      \param	ucUnit					      number of the unit (specifies descriptor array index <b>[not the hardware index]</b> of rttdev.h)
      \param	ulMsg					         message bits to retrieve count for (refer to <a href="#RttMsg">AT91SAM7X manual 14.4.4 page 78: RTT_SR</a>)
      \param	usDecrement					   decrement count (0 for no decrement) for selected message bits <b>(var type's max value USHRT_MAX* just clears the counter(s))</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max
               - message bits are filtered hardware bits (refer to <a href="#RttMsg">AT91SAM7X manual 14.4.4 page 78: RTT_SR</a>) by u32IrqMsg of _SRttRO
      
      \remarks
               - this function is helpful to determine the reason for the sent/given "news semaphore" when more than one message is masked
      
      <b>Manual reference(s):</b><br>
      <a name="RttMsg">
      <a href="../../../MANUAL/AT91/AT91SAM7X/RTT/AT91SAM7X_14.4.4_p78_RTT_SR.pdf" type="application/pdf">AT91SAM7X manual 14.4.4 page 78: RTT_SR</a><br>

      <b>*)</b> refer to .\\Keil\\ARM\\RV31\\Inc\\limits.h for compiler type limit definitions
*/
USHORT RttMsg(UCHAR ucUnit, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout);

//!   get current Real-Time-Timer value
/**
      \param	ucUnit					      number of the unit (specifies descriptor array index <b>[not the hardware index]</b> of rttdev.h)
      \param	*pu32Ticks					   current clock ticks (tick units are set by #RttRestart)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
*/
bool   RttRead(UCHAR ucUnit, U32 *pu32Ticks, U16 u16Timeout);

#endif
