//!	defines and configuration of AT91SAM7X EFC driver
/**
		\file		efc.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/EFC/EFC.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-02-28 initial version
// 2013-03-05 added overideable define EFC_IRQ
// 2013-03-07 added define EFC_IRQ_MASK
// 2013-03-13 updated doxygen documentation
// 2013-03-28 added flags for open, close and locate functions
// 2013-04-03 changed flags values for locate functions
// 2013-04-10 added close flag EFC_CLOSE_LOCKEND
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_EFC_EFC_H
#define SYKAM_CPU_AT91_EFC_EFC_H 1


// Macros

// Function template IsrEfc
#define EFC_ISR_FUNC(unit) void IsrEfc##unit(void) { EfcIrqHandler(unit); }

// Function prototype IsrEfc
#define EFC_ISR_PROTO(unit) void IsrEfc##unit(void)

// Function name IsrEfc
#define EFC_ISR_NAME(unit) IsrEfc##unit

// Defines

//! \internal \brief <b>Internal:</b> filter mask for hardware bits
#define EFC_IRQ_MASK (AT91C_MC_FRDY | AT91C_MC_LOCKE | AT91C_MC_PROGE)

//! <b>Open option:</b> clear hardware page buffer with 0xFFFFFFFF (= empty flash value of hardware)
#define EFC_OPEN_CLEAR     (1 << 0)

//! <b>Open option:</b> copy current page contents to hardware page buffer (for easy patching/modifying)
#define EFC_OPEN_COPY      (1 << 1)

//! <b>Open option:</b> unlock block (lock) region of current page to enable write access
#define EFC_OPEN_UNLOCK    (1 << 2)


//! <b>Close option:</b> lock block (lock) region of current page after update to disable further write access
#define EFC_CLOSE_LOCK     (1 << 0)

//! <b>Close option:</b> lock block (lock) region of current page <b>(when it's the last page in the block's range)</b> after update to disable further write access
#define EFC_CLOSE_LOCKEND  (1 << 1)

//! <b>Close option:</b> cancel any changes to the current page and discard hardware page buffer
#define EFC_CLOSE_CANCEL   (1 << 2)


//! <b>Locate option:</b> argument type is absolute address
#define EFC_LOCATE_ADDRESS (1 << 6)

//! <b>Locate option:</b> argument type is absolute page number
#define EFC_LOCATE_PAGEABS (1 << 7)

// Enums

//! \internal \brief <b>Internal:</b> Value from the drivers read-only data
typedef enum
{
   EFC_ROGET_BLOCKCOUNT,            //!< SEfcRO[ucUnit].ucBlockCount
   EFC_ROGET_BLOCKPAGECOUNT,        //!< SEfcRO[ucUnit].ucPageCount
   EFC_ROGET_BLOCKSIZE,             //!< SEfcRO[ucUnit].usBlockSize
   EFC_ROGET_PAGESIZE               //!< SEfcRO[ucUnit].usPageSize
} EEFCRoGet;

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

//! \internal \brief <b>Internal configuration:</b> efc write enable key (refer to AT91SAM7X manual 19.3.2 page 117: MC_FCR)
#ifndef AT91C_MC_CORRECT_KEY
   #define AT91C_MC_CORRECT_KEY  ((ULONG) 0x5A << 24)
#endif

//! <b>Overrideable configuration:</b> use sysirq <b>[required (== 1) for efc status and error message generation]</b>
#ifndef EFC_IRQ
   #define EFC_IRQ 1
#endif

//! <b>Overrideable configuration:</b> maximum bits used for msg data in 'msg bit' msg (16bit data/msg to keep 16bit left for counter field)
#ifndef EFC_MSG_MSGBITS
   #define EFC_MSG_MSGBITS 16
#endif

//! <b>Overrideable configuration:</b> type of each message counter
#ifndef EFC_MSGCOUNTERTYPE
   #define EFC_MSGCOUNTERTYPE USHORT
#endif

//! <b>Overrideable configuration:</b> max value of each message counter (limits are defined in .\\Keil\\ARM\\RV31\\Inc\\limits.h)
#ifndef EFC_MSGCOUNTERMAX
   #define EFC_MSGCOUNTERMAX USHRT_MAX
#endif

//! <b>Overrideable configuration:</b> maximum msg counter index (0 for upto AT91C_MC_FRDY= 2^0, 3 for upto AT91C_MC_PROGE= 2^3) <b>[messages with index value above are ignored]</b>
#ifndef EFC_MSGMAX
   #define EFC_MSGMAX 3
#endif

//! <b>Overrideable configuration:</b> number of messages buffered for IRQ to task
#ifndef EFC_MBXCNTMSG
   #define EFC_MBXCNTMSG 16
#endif

//! <b>Overrideable configuration:</b> default stack size for TskEfcMsg (set to 0 to use default system stack and its default size, refer to RTX_Config.c)
#ifndef EFC_STACK_TSKMSG
   #define EFC_STACK_TSKMSG 300
#endif

//! <b>Overrideable configuration:</b> number of units supported simultaneously
#ifndef EFC_UNIT_MAX
   #define EFC_UNIT_MAX 2
#endif

// Prototypes

__task void TskEfcMsg(void *pvParam);
void EfcIrqHandler(UCHAR ucUnit);

// Each supported unit requires a prototype definition here
#if EFC_UNIT_MAX > 0
   EFC_ISR_PROTO(0);
#endif

#if EFC_UNIT_MAX > 1
   EFC_ISR_PROTO(1);
#endif

#endif
