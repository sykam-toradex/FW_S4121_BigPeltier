//!	defines and configuration of AT91SAM7X USART driver
/**
		\file		usart.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/USART/USART.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-01-08 initial version
// 2009-01-08 added defines for USART_DELAYRX and USART_DELAYTX
// 2009-01-09 added define USART_BASE, made IRQ defines external overrideable
// 2009-01-09 renamed Usart0 to Usart and added user define for usart base
// 2009-01-09 added defines for USART_PINSDATA and USART_PINSHWH
// 2009-05-29 changed task qualifier style for RL-ARM 3.50
// 2009-06-11 added defines for USART_ID and USART_PIO
// 2009-06-11 moved all global data to usartgbl.h
// 2009-06-16 generic function prototyp macro for TSK and ISR
// 2009-06-18 increased USART_MSGMAX to 14 to cover messages upto AT91C_US_NACK
// 2009-06-25 increased USART_MBXCNTMSG to 16 to handle 4x more sent messages
// 2009-06-26 added USART_MSGBITS constant
// 2009-09-25 changed to fit single linked reentrant usart driver model
// 2009-10-06 added USART_MSGCOUNTERTYPE and USART_MSGCOUNTERMAX
// 2009-12-23 added board\AT91SAM7X.h for board specific configuration
// 2010-05-26 multiplied USART_BUFFERCNTTX by 4 to 128
// 2010-06-18 increased USART_BUFFERCNTTX to 384
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-11-04 added define USART_PDCDEPTHRX
// 2010-11-05 changed value of USART_PDCMAXRX and of USART_BUFFERCNTRX
// 2011-05-19 change USART_UNIT_MAX to optional define type
// 2011-06-01 decreased USART_MSGBITS to 16 for full 16bit (USHORT) msg counter
// 2011-06-01 splitted old USART_MSGBITS to USART_MSG_MSGBITS & USART_DATA_MSGBITS
// 2011-07-07 USART_MSGMAX is now a zero based setting
// 2011-10-20 added public doxygen documentation
// 2011-10-26 added public doxygen documentation
// 2011-11-30 added internal documentation
// 2013-04-11 added fallback configuration for AT91C_MCK
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_USART_USART_H
#define SYKAM_CPU_AT91_USART_USART_H 1


// Macros

// Function template IsrUsart
#define USART_ISR_FUNC(unit) void IsrUsart##unit(void) __irq { UsartIrqHandler(unit); }

// Function prototype IsrUsart
#define USART_ISR_PROTO(unit) void IsrUsart##unit(void) __irq

// Function name IsrUsart
#define USART_ISR_NAME(unit) IsrUsart##unit

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

//! <b>Fallback configuration:</b> Define AT91C_MCK ([B]it, [C]onstant, [D]efinition, [F]unction) by obsolete AT91B_MCK
#ifndef AT91C_MCK
   #define AT91C_MCK AT91B_MCK
#endif

//! <b>Overrideable configuration:</b> maximum bits used for msg data in 'msg bit' msg (16bit data/msg to keep 16bit left for counter field)
#ifndef USART_MSG_MSGBITS
   #define USART_MSG_MSGBITS 16
#endif

//! <b>Overrideable configuration:</b> maximum bits used for msg data in 'data' msg (24bit data/msg to keep 8bit left for counter field)
#ifndef USART_DATA_MSGBITS
   #define USART_DATA_MSGBITS 24
#endif

//! <b>Overrideable configuration:</b> maximum count of pdc rx blocks
#ifndef USART_PDCDEPTHRX
   #define USART_PDCDEPTHRX 2
#endif

//! <b>Overrideable configuration:</b> maximum chars for each pdc rx block
#ifndef USART_PDCMAXRX
   #define USART_PDCMAXRX 64
#endif

//! <b>Overrideable configuration:</b> maximum chars for each pdc tx transfer
#ifndef USART_PDCMAXTX
   #define USART_PDCMAXTX 8
#endif

//! <b>Overrideable configuration:</b> type of each message counter
#ifndef USART_MSGCOUNTERTYPE
   #define USART_MSGCOUNTERTYPE USHORT
#endif

//! <b>Overrideable configuration:</b> max value of each message counter (limits are defined in .\\Keil\\ARM\\RV31\\Inc\\limits.h)
#ifndef USART_MSGCOUNTERMAX
   #define USART_MSGCOUNTERMAX USHRT_MAX
#endif

//! <b>Overrideable configuration:</b> maximum msg counter index (7 for upto AT91C_US_PARE= 2^7, 13 for upto AT91C_US_NACK= 2^13) <b>[messages with index value above are ignored]</b>
#ifndef USART_MSGMAX
   #define USART_MSGMAX 13
#endif

//! <b>Overrideable configuration:</b> buffer holding task rx data (should be multiples of USART_PDCMAXRX for ideal rx performance)
#ifndef USART_BUFFERCNTRX
   #define USART_BUFFERCNTRX (USART_PDCMAXRX* 2)
#endif

//! <b>Overrideable configuration:</b> buffer holding task tx data (should be multiples of USART_PDCMAXTX for ideal tx performance)
#ifndef USART_BUFFERCNTTX
   #define USART_BUFFERCNTTX 384
#endif

//! <b>Overrideable configuration:</b> number of rx messages for IRQ to task
#ifndef USART_MBXCNTRX
   #define USART_MBXCNTRX 4
#endif

//! <b>Overrideable configuration:</b> number of messages buffered for IRQ to task
#ifndef USART_MBXCNTMSG
   #define USART_MBXCNTMSG 16
#endif

//! <b>Overrideable configuration:</b> number of usart units supported simultaneously
#ifndef USART_UNIT_MAX
   #define USART_UNIT_MAX 2
#endif

// Prototypes
__task void TskUsartMsg(void *pvParam);
__task void TskUsartRx(void *pvParam);
__task void TskUsartTx(void *pvParam);
void UsartIrqHandler(UCHAR ucUnit);

// Each supported usart unit requires a prototype definition here
#if USART_UNIT_MAX > 0
   USART_ISR_PROTO(0);
#endif

#if USART_UNIT_MAX > 1
   USART_ISR_PROTO(1);
#endif

#endif
