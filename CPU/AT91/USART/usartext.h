//!	external interface of AT91SAM7X USART driver
/**
		\file		usartext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/USART/USARTEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-01-08 initial version (imported from usart.h)
// 2009-01-08 added UsartInit(...)
// 2009-01-09 renamed Usart0 to Usart
// 2009-06-11 changed prototypes
// 2009-06-16 US_RTOR is now configurable when calling UsartInit
// 2009-06-17 changed prototype of UsartInit
// 2009-06-18 renamed oiTORXSem to oiMsgNewSem
// 2009-06-24 changed prototype of UsartInit
// 2009-07-31 changed prototype of UsartMsg
// 2009-09-03 changed UsartInit para ulTORXDiv to ulTORXMS
// 2009-09-04 changed UsartInit para ulTORXMS to ulTORXUS
// 2009-09-24 reworking twice linked usart to single linked reentrant usart
// 2009-09-25 reworked twice linked usart to single linked reentrant usart
// 2009-10-23 changed style of UsartTx and UsartRx for support of ushort blocks
// 2009-11-06 changed return type of UsartMsg to USHORT to support ushort msg
// 2009-12-18 UsartMsg now supports true counter decrement
// 2011-10-20 added public doxygen documentation
// 2011-10-26 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_USART_USARTEXT_H
#define SYKAM_CPU_AT91_USART_USARTEXT_H 1


// Prototypes for functions

//!   init a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of usartdev.h)
      \param	ucChannel					   (unused, set 0) number of the sub-channel for units tree(s)
      \param   ulBaud                     baud rate for communication
      \param   ulOption                   use AT91C_US_... defines of AT91SAM7X512.h for communication options like handshake, character width and parity (refer to <a href="#UsartInit">AT91SAM7X manual 30.7.2 page 335: US_MR</a>)
      \param   ulTORXUS                   TimeOut of Receiver in US (to release data from pending PDC when data count is smaller than PCD's block size)
      \param   ulTORXModeInit             (set to #AT91C_US_RETTO as default) initial mode for TimeOut of Receiver (refer to <a href="#UsartInit">AT91SAM7X manual 30.7.1 page 333: US_CR</a>)
      \param	oiMsgNewSem				      (optional, set to NULL as default) OS_ID of <b>pre-initialized (to 0!)</b> semaphore to send/give for matching messages
      \param	ulMsgMask						(optional, set to NULL as default) and-mask to include triggered messages for semaphore triggering
      \param	ulTimeGuard				      (optional, set to 0 as default) transmitter time guard (refer to <a href="#UsartInit">AT91SAM7X manual 30.6.3.8 page 318: US_RTOR</a>)

      \return										> 0 for success, == 0 for failure

      \note
               - 3 tasks are spanned by this function<br>
               - message bits are composed of filtered hardware bits (refer to <a href="#UsartInit">AT91SAM7X manual 30.7.6 page 341: US_CSR</a>) by u32IrqMsg of SUsartRO<br>
               
      <b>Example:</b>
               - for usual <b>RS232 mode</b> (8, N, 1) with hardware handshake via RTS/CTS<br>set <b>ulOption=</b> (#AT91C_US_USMODE_HWHSH | #AT91C_US_CHRL_8_BITS | #AT91C_US_PAR_NONE | #AT91C_US_NBSTOP_1_BIT)<br>


      <b>Example:</b>
               - for usual <b>RS422 mode</b> (8, N, 1)<br>set <b>ulOption=</b> (#AT91C_US_USMODE_RS485 | #AT91C_US_CHRL_8_BITS | #AT91C_US_PAR_NONE | #AT91C_US_NBSTOP_1_BIT)
      
      <b>Manual reference(s):</b><br>
      <a name="UsartInit">
      <a href="../../../MANUAL/AT91/AT91SAM7X/USART/AT91SAM7X_30.7.2_p335-337_US_MR.pdf" type="application/pdf">AT91SAM7X manual 30.7.2 page 335: US_MR</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/USART/AT91SAM7X_30.7.1_p333-335_US_CR.pdf" type="application/pdf">AT91SAM7X manual 30.7.1 page 333: US_CR</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/USART/AT91SAM7X_30.6.3.8_p318-320_US_RTOR.pdf" type="application/pdf">AT91SAM7X manual 30.6.3.8 page 318: US_RTOR</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/USART/AT91SAM7X_30.7.6_p341-343_US_CSR.pdf" type="application/pdf">AT91SAM7X manual 30.7.6 page 341: US_CSR</a><br>
*/
UCHAR UsartInit(UCHAR ucUnit, UCHAR ucChannel, ULONG ulBaud, ULONG ulOption, ULONG ulTORXUS, ULONG ulTORXModeInit, OS_ID oiMsgNewSem, ULONG ulMsgMask, ULONG ulTimeGuard);

//!   retrieve message counters of a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of usartdev.h)
      \param	ucChannel					   (unused, set 0) number of the sub-channel for units tree(s)
      \param	ulMsg				            message bits to retrieve (refer to <a href="#UsartMsg">AT91SAM7X manual 30.7.6 page 341: US_CSR</a>)
      \param	usDecrement				      decrement count (0 for no decrement) for selected message bits <b>(var type's max value USHRT_MAX* just clears the counter(s))</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										summarized count of all selected message bits <b>before decrementing</b>

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max<br>
               - message bits are composed of filtered hardware bits (refer to <a href="#UsartMsg">AT91SAM7X manual 30.7.6 page 341: US_CSR</a>) by u32IrqMsg of SUsartRO<br>
               
      \remarks
               <b>this function is helpful to</b><br>
               - determine the reason for the sent/given "news semaphore" when more than one message is masked <b>before</b> any data retrival using #UsartRx
               - track count of messages just <b>after</b> a normal data access using #UsartRx / #UsartTx for easier debugging and verification
               
      <br><b>*)</b> refer to .\\Keil\\ARM\\RV31\\Inc\\limits.h for compiler type limit definitions
      
      <b>Manual reference(s):</b><br>
      <a name="UsartMsg">
      <a href="../../../MANUAL/AT91/AT91SAM7X/USART/AT91SAM7X_30.7.6_p341-343_US_CSR.pdf" type="application/pdf">AT91SAM7X manual 30.7.6 page 341: US_CSR</a><br>
*/
USHORT UsartMsg(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout);

//!   retrieve received data
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of usartdev.h)
      \param	ucChannel					   (unused, set 0) number of the sub-channel for units tree(s)
      \param	*pvData					      storage for data
      \param   usCount                    count of data to retrieve
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet)
*/
USHORT UsartRx(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout);

//!   queue data to send
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of usartdev.h)
      \param	ucChannel					   (unused, set 0) number of the sub-channel for units tree(s)
      \param	*pvData					      pointer to data to send
      \param   usCount                    count of data to send
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet)
*/
USHORT UsartTx(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout);

#endif
