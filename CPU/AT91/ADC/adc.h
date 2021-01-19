//!	defines and configuration of AT91SAM7X ADC peripheral driver
/**
		\file		adc.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/ADC/ADC.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-05-19 initial version
// 2011-06-01 splitted old ADC_UNIT_MSGBITS to ADC_UNIT_/ADC_DATA_MSG_MSGBITS
// 2011-07-07 added optional define ADC_MEMITEMTYPE
// 2011-07-07 ADC_UNIT/CHANNEL_MSGMAX are now a zero based settings
// 2011-07-12 changed trigger bit order to below-above, min-max and normal
// 2011-07-12 raised default values of ADC_UNIT/CHANNEL_MSGMAX to 25
// 2011-07-12 fixed comment of ADC_UNIT_MSG_MSGBITS and raised it to 28
// 2011-10-19 added public doxygen documentation
// 2013-04-11 added fallback configuration for AT91C_MCK
// 2013-08-07 moved AT91C_ADC_EOCX/OVREX defines to adc.h and added documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_ADC_ADC_H
#define SYKAM_CPU_AT91_ADC_ADC_H 1


// Macros

// Function template IsrAdc
#define ADC_ISR_FUNC(unit) void IsrAdc##unit(void) __irq { AdcIrqHandler(unit); }

// Function prototype IsrAdc
#define ADC_ISR_PROTO(unit) void IsrAdc##unit(void) __irq

// Function name IsrAdc
#define ADC_ISR_NAME(unit) IsrAdc##unit


// Defines
#define ADC_UNIT_FLAGS_CONFIGDONE      (1 << 0)

#define ADC_CHANNEL_FLAGS_CONFIGDONE   (1 << 0)


//! <b>Trigger message bit to send/give attached semaphore(s):</b> Summary of individual (E)nd (O)f (C)onversion flags
#define AT91C_ADC_EOCX (AT91C_ADC_EOC7 | AT91C_ADC_EOC6 | AT91C_ADC_EOC5 | AT91C_ADC_EOC4 | AT91C_ADC_EOC3 | AT91C_ADC_EOC2 | AT91C_ADC_EOC1 | AT91C_ADC_EOC0)

//! <b>Trigger message bit to send/give attached semaphore(s):</b> Summary of individual (OV)er-(R)un flags
#define AT91C_ADC_OVREX (AT91C_ADC_OVRE7 | AT91C_ADC_OVRE6 | AT91C_ADC_OVRE5 | AT91C_ADC_OVRE4 | AT91C_ADC_OVRE3 | AT91C_ADC_OVRE2 | AT91C_ADC_OVRE1 | AT91C_ADC_OVRE0)



//! <b>Trigger message bit to send/give attached semaphore(s):</b> Below range
#define ADC_TRG_BELOW                  (1 << 20)

//! <b>Trigger message bit to send/give attached semaphore(s):</b> Above range
#define ADC_TRG_ABOVE                  (1 << 21)

//! <b>Trigger message bit to send/give attached semaphore(s):</b> Min range
#define ADC_TRG_MIN                    (1 << 22)

//! <b>Trigger message bit to send/give attached semaphore(s):</b> Max range
#define ADC_TRG_MAX                    (1 << 23)

//! <b>Trigger message bit to send/give attached semaphore(s):</b> Normal range
#define ADC_TRG_NORMAL                 (1 << 24)

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

//! <b>Fallback configuration:</b> Define AT91C_MCK ([B]it, [C]onstant, [D]efinition, [F]unction) by obsolete AT91B_MCK
#ifndef AT91C_MCK
   #define AT91C_MCK AT91B_MCK
#endif

//! <b>Overrideable configuration:</b> maximum channel items for each pdc rx block
#ifndef ADC_CHANNEL_MAX
   #define ADC_CHANNEL_MAX 8
#endif

//! <b>Overrideable configuration:</b> maximum bits used for msg data in 'msg bit' msg (up to 28bit data/msg to keep 4bit left for counter field)
#ifndef ADC_UNIT_MSG_MSGBITS
   #define ADC_UNIT_MSG_MSGBITS 28
#endif

//! <b>Overrideable configuration:</b>  maximum bits used for msg data in 'msg bit' msg (up to 28bit data/msg to keep 4bit left for channel field)
#ifndef ADC_CHANNEL_MSG_MSGBITS
   #define ADC_CHANNEL_MSG_MSGBITS 28
#endif

//! <b>Overrideable configuration:</b>  maximum bits used for msg data in 'data' msg (24bit data/msg to keep 8bit left for counter field)
#ifndef ADC_UNIT_DATA_MSGBITS
   #define ADC_UNIT_DATA_MSGBITS 24
#endif

//! <b>Overrideable configuration:</b>  maximum count of pdc rx blocks
#ifndef ADC_PDCDEPTHRX
   #define ADC_PDCDEPTHRX 2
#endif

//! <b>Overrideable configuration:</b>  size of PDC item for a ADC value (USHORT for 10 bit, UCHAR for 8 bit mode)
#ifndef ADC_PDCITEMTYPE
   #define ADC_PDCITEMTYPE USHORT
#endif

//! <b>Overrideable configuration:</b>  size of MEMory item for ADC value (to fit also average() and modify() return values)
#ifndef ADC_MEMITEMTYPE
   #define ADC_MEMITEMTYPE ULONG
#endif

//! <b>Overrideable configuration:</b>  type of each message counter (maximum counter range)
#ifndef ADC_MSGCOUNTERTYPE
   #define ADC_MSGCOUNTERTYPE UCHAR
#endif

//! <b>Overrideable configuration:</b> max value of each message counter (limits are defined in .\\Keil\\ARM\\RV31\\Inc\\limits.h)
#ifndef ADC_MSGCOUNTERMAX
   #define ADC_MSGCOUNTERMAX UCHAR_MAX
#endif

//! <b>Overrideable configuration:</b>  maximum msg counter index (15 for upto AT91C_ADC_OVRE7= 2^15, 17 for upto AT91C_ADC_GOVRE= 2^17, 24 for upto ADC_TRG_ABOVE= 2^24, 25 for upto ADC_TRG_NORMAL= 2^25 (= MAX)) <b>[Messages with index value above are ignored]</b>
#ifndef ADC_UNIT_MSGMAX
   #define ADC_UNIT_MSGMAX 25
#endif

//! <b>Overrideable configuration:</b>  maximum msg counter index (15 for upto AT91C_ADC_OVRE7= 2^15, 17 for upto AT91C_ADC_GOVRE= 2^17, 24 for upto ADC_TRG_ABOVE= 2^24, 25 for upto ADC_TRG_NORMAL= 2^25 (= MAX)) <b>[Messages with index value above are ignored]</b>
#ifndef ADC_CHANNEL_MSGMAX
   #define ADC_CHANNEL_MSGMAX 25
#endif

//! <b>Overrideable configuration:</b>  number of rx messages buffered for IRQ to task
#ifndef ADC_MBXCNTRX
   #define ADC_MBXCNTRX 4
#endif

//! <b>Overrideable configuration:</b>  number of messages buffered for IRQ to task
#ifndef ADC_MBXCNTMSG
   #define ADC_MBXCNTMSG 16
#endif

//! <b>Overrideable configuration:</b> number of units supported simultaneously
#ifndef ADC_UNIT_MAX
   #define ADC_UNIT_MAX 1
#endif

// Prototypes
__task void TskAdcMsg(void *pvParam);
__task void TskAdcMsgChannel(void *pvParam);
__task void TskAdcRx(void *pvParam);
void AdcIrqHandler(UCHAR ucUnit);

// Each supported unit requires a prototype definition here
#if ADC_UNIT_MAX > 0
   ADC_ISR_PROTO(0);
#endif

#if ADC_UNIT_MAX > 1
   ADC_ISR_PROTO(1);
#endif

#endif
