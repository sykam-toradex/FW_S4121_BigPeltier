//!	defines and configuration of AT91SAM7X SPI driver
/**
		\file		spi.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/SPI/SPI.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-10-07 initial version
// 2009-10-08 added baudrate and delay macros, SPI_CHANNELMAX
// 2009-10-09 added SPI_CHANNEL_FLAGS_CSRUPDATE
// 2009-10-14 added SPI_CHANNEL_MSGMAX, SPI_CHANNEL_MSGBITS
// 2009-10-15 added SPI_PRIO macro and SPI_PRIO defines
// 2009-10-15 added SPI_PCS2CHANNEL, SPI_CHANNEL2PCS and SPI_CHANNEL2CSR macro
// 2009-10-22 added DATABITS and DATABYTES to clearify MSGBITS defines
// 2009-11-06 added MSGCOUNTERTYPE and MSGCOUNTERMAX
// 2009-12-17 increased SPI_PDCMAX to 16 and SPI_BUFFERCNTRX to 64
// 2009-12-21 added SPI_USERTAG_DEFAULT definition
// 2009-12-22 added board\AT91SAM7X.h for board specific configuration
// 2009-12-23 added SPI_STACK_TSKSPIRX and SPI_STACK_TSKSPITX
// 2010-01-21 fixed macros SPI_PRIO and SPI_PCS2CHANNEL
// 2010-06-16 added SPI_HWFIX_PDCRX & SPI_HWFIX_PDCTX to control hw bug fixes
// 2010-06-16 added define SPI_DEBUG_CONDITIONAL to control conditional debug
// 2010-06-17 added define SPI_HWFIX_PDCTX_REPLACE to replace byte by hw bug
// 2010-06-17 added define SPI_HWFIX_PDCTX_CLEAR to clear queued byte by pdc
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-08-05 SPI_BUFFERCNTRX & SPI_BUFFERCNTTX set relative to SPI_PDCMAX
// 2010-08-11 increased SPI_PDCMAX to 32 items (each contains 1-2 data bytes)
// 2010-08-26 fixed macro SPI_PCS2CHANNEL for slave mode peripheral
// 2010-08-26 removed defines SPI_HWFIX_PDCTX_REPLACE & SPI_HWFIX_PDCTX_CLEAR
// 2010-08-26 renamed define SPI_DEBUG_CONDITIONAL to SPI_DEBUG
// 2010-08-27 added define SPI_PDCDEPTH for pdc buffer count adjustment
// 2010-09-02 removed (hopefully for ever) obsolete define SPI_HWFIX_PDCRX
// 2010-09-23 added define SPI_CHANNEL_FLAGS_QUEUELOCKED
// 2010-09-23 changed to _u(l)min & _u(l)max intrinsics
// 2010-10-07 added define SPI_UNIT_CSR32COUNT
// 2010-12-09 re-introduced the define SPI_HWFIX_PDCRX
// 2011-01-19 added defines for states of tx irq (SPI_TXSTATE_...)
// 2011-04-26 added define SPI_BUFFERCNTADDRRX
// 2011-04-26 removed obsolete define SPI_MBXCNTRX
// 2011-05-26 removed opt defines SPI_CHANNEL_DATABYTES & SPI_CHANNEL_DATABITS
// 2011-06-01 decreased SPI_..._MSGBITS to 12 for 16bit (USHORT) msg channel+counter
// 2011-06-01 splitted old SPI_UNIT_MSGBITS to SPI_UNIT_/SPI_DATA_MSG_MSGBITS
// 2011-07-07 SPI_UNIT/CHANNEL_MSGMAX are now a zero based settings
// 2011-10-26 added public doxygen documentation
// 2011-11-30 added internal documentation
// 2014-01-09 added new configuration setting SPI_CHANNELID_MAX
// 2016-01-21 removed obsolete define SPI_TXSTATE_DO_RX
// 2016-02-11 added forced min and max limits for macro SPI_CSR_SCBR
//----------------------------------------------------------------------------
// Total buffer (without mailboxes and semaphores) comsumption in bytes:
// (SPI_PDCMAX* 4* 3)+ SPI_CHANNEL_MAX* (SPI_BUFFERCNTRX+ SPI_BUFFERCNTTX)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_SPI_SPI_H
#define SYKAM_CPU_AT91_SPI_SPI_H 1


// Macros

// Determine limited priority of specified priority p
#define SPI_PRIO(p) ((p) > 0) ? (1 << (_umin((p), 7))) : ((p))

// Function template IsrSpi
#define SPI_ISR_FUNC(unit) void IsrSpi##unit(void) __irq { SpiIrqHandler(unit); }

// Function prototype IsrSpi
#define SPI_ISR_PROTO(unit) void IsrSpi##unit(void) __irq

// Function name IsrSpi
#define SPI_ISR_NAME(unit) IsrSpi##unit


// Determine SCBR by specified MckHZ and SpckHZ
#define SPI_CSR_SCBR(MckHZ, SpckHZ) ((_ulmin((_ulmax(((MckHZ)/ (SpckHZ)), 1)), UCHAR_MAX) << 8) & AT91C_SPI_SCBR)

// Determine ***generic*** DLY (in NS) by specified MckHZ, DelayNS, Div, Shift and u32Mask
#define SPI_CSR_DLYNS(MckHZ, DelayNS, Div, Shift, u32Mask)  ((((((DelayNS)/ (Div)* ((MckHZ)/ 1000000))/ 1000)+ 1) << (Shift)) & (u32Mask))

// Determine DLYBS (in NS) by specified MckHZ and DelayNS
#define SPI_CSR_DLYBS(MckHZ, DelayNS)  SPI_CSR_DELAYNS(MckHZ, DelayNS, 1, 16, AT91C_SPI_DLYBS)

// Determine DLYBCT (in NS) by specified MckHZ and DelayNS
#define SPI_CSR_DLYBCT(MckHZ, DelayNS) SPI_CSR_DELAYNS(MckKZ, DelayNS, 32, 24, AT91C_SPI_DLYBCT)


// Determine channel by specified pcs for current peripheral base
#define SPI_PCS2CHANNEL(base, pcs) ((base->SPI_MR & AT91C_SPI_PCSDEC) || (!(base->SPI_MR & AT91C_SPI_MSTR))) ? ((pcs)) : (_umin((~(pcs) & 0x0F) >> 1, 0x03))

// Determine pcs by specified channel for current peripheral base
#define SPI_CHANNEL2PCS(base, channel) (base->SPI_MR & AT91C_SPI_PCSDEC) ? ((channel)) : (~(1 << (channel)))

// Determine index of CSR by specified channel for current peripheral base
#define SPI_CHANNEL2CSRIDX(base, channel) (base->SPI_MR & AT91C_SPI_PCSDEC) ? ((channel) >> 2) : ((channel))

// Determine index of CSR by specified pcs for current peripheral base
#define SPI_PCS2CSRIDX(base, pcs) SPI_CHANNEL2CSRIDX(base, SPI_PCS2CHANNEL(base, (pcs)))

// Determine bytes of pdc item for current configuration
#define SPI_PDCITEMBYTES(u32CSR) ((u32CSR & AT91C_SPI_BITS) ? (2) : (1))


// Defines

//! <b>Queue data priority (re-arrange order given by slave channel number 0...15):</b> maximum
#define SPI_P_MAX          (1 << 7)

//! <b>Queue data priority (re-arrange order given by slave channel number 0...15):</b> high
#define SPI_P_HIGH         (1 << 6) 

//! <b>Queue data priority (re-arrange order given by slave channel number 0...15):</b> above normal
#define SPI_P_ABOVENORMAL  (1 << 5)

//! <b>Queue data priority (re-arrange order given by slave channel number 0...15):</b> normal
#define SPI_P_NORMAL       (1 << 4)

//! <b>Queue data priority (re-arrange order given by slave channel number 0...15):</b> below normal
#define SPI_P_BELOWNORMAL  (1 << 3)

//! <b>Queue data priority (re-arrange order given by slave channel number 0...15):</b> low
#define SPI_P_LOW          (1 << 2)

//! <b>Queue data priority (re-arrange order given by slave channel number 0...15):</b> idle (process queue data after all other)
#define SPI_P_IDLE         (1 << 1)

//! <b>Queue data priority (re-arrange order given by slave channel number 0...15):</b> (process only when maximum SPECIFIED queue data age reached)
#define SPI_P_AGE          (0)

// Flags
#define SPI_UNIT_FLAGS_CONFIGDONE   (1 << 0)
#define SPI_CHANNEL_FLAGS_CONFIGDONE   (1 << 0)
#define SPI_CHANNEL_FLAGS_DATAQUEUEDTX (1 << 1)
#define SPI_CHANNEL_FLAGS_QUEUELOCKED  (1 << 2)

//! <b>Flag/option:</b> force immediate channel configuration update of hardware register (update done by use of channel later WITHOUT this flag/option)
#define SPI_CHANNEL_FLAGS_CSRUPDATE    (1 << 4)

// Isr states
#define SPI_TXSTATE_START_TX  (0)
#define SPI_TXSTATE_STOP_RX   (1)

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

// Conditional debug break points
#ifndef SPI_DEBUG
   #define SPI_DEBUG TRUE
#endif

//! <b>Overrideable configuration:</b> number of hardware CSRs
#ifndef SPI_UNIT_CSR32COUNT
   #define SPI_UNIT_CSR32COUNT   (4)
#endif

//! <b>Overrideable configuration:</b> hardware bug fix for pdc rx (pdc sets rx address one item to early to following channels address)
#ifndef SPI_HWFIX_PDCRX
   #define SPI_HWFIX_PDCRX TRUE
#endif

//! <b>Overrideable configuration:</b> hardware bug fix for pdc tx (SPI_RCR read to low by one because of hw-bug of pdc queues one unsent byte)
#ifndef SPI_HWFIX_PDCTX
   #define SPI_HWFIX_PDCTX TRUE
#endif

//! <b>Overrideable configuration:</b> default stack size for TskSpiRx
#ifndef SPI_STACK_TSKSPIRX
   #define SPI_STACK_TSKSPIRX 600
#endif

//! <b>Overrideable configuration:</b> default stack size for TskSpiTx
#ifndef SPI_STACK_TSKSPITX
   #define SPI_STACK_TSKSPITX 600
#endif

//! <b>Overrideable configuration:</b> default user tag data value
#ifndef SPI_USERTAG_DEFAULT
   #define SPI_USERTAG_DEFAULT 0xFE
#endif

//! <b>Overrideable configuration:</b> maximum items (each contains 8-16 data bits - depending on each individual slave's configuration) for each transfer
#ifndef SPI_PDCMAX
   #define SPI_PDCMAX (32)
#endif

//! <b>Overrideable configuration:</b> maximum depth of buffers for circular buffer style
#ifndef SPI_PDCDEPTH
   #define SPI_PDCDEPTH (2)
#endif

//! <b>Overrideable configuration:</b> maximum bits used for msg data in 'msg bit' msg (up to 16bit data/msg to keep 16bit left for counter field)
#ifndef SPI_UNIT_MSG_MSGBITS
   #define SPI_UNIT_MSG_MSGBITS 12
#endif

//! <b>Overrideable configuration:</b> maximum bits used for msg data in 'msg bit' msg (up to 12bit data/msg to keep 20bit left for channel (4bit) & counter (16bit) field)
#ifndef SPI_CHANNEL_MSG_MSGBITS
   #define SPI_CHANNEL_MSG_MSGBITS 12
#endif

//! <b>Overrideable configuration:</b> maximum bits used for msg data in 'data' msg (24bit data/msg to keep 8bit left for counter field)
#ifndef SPI_UNIT_DATA_MSGBITS
   #define SPI_UNIT_DATA_MSGBITS 24
#endif

//! <b>Overrideable configuration:</b> type of each message counter
#ifndef SPI_MSGCOUNTERTYPE
   #define SPI_MSGCOUNTERTYPE USHORT
#endif

//! <b>Overrideable configuration:</b> max value of each message counter (limits are defined in .\\Keil\\ARM\\RV31\\Inc\\limits.h)
#ifndef SPI_MSGCOUNTERMAX
   #define SPI_MSGCOUNTERMAX USHRT_MAX
#endif

//! <b>Overrideable configuration:</b> maximum msg counter index (9 for upto AT91C_SPI_TXEMPTY= 2^9, 24 max) <b>[messages with index value above are ignored]</b>
#ifndef SPI_UNIT_MSGMAX
   #define SPI_UNIT_MSGMAX 10
#endif

//! <b>Overrideable configuration:</b> maximum msg counter index (9 for upto AT91C_SPI_TXEMPTY= 2^9, 16(!) max) <b>[messages with index value above are ignored]</b>
#ifndef SPI_CHANNEL_MSGMAX
   #define SPI_CHANNEL_MSGMAX 10
#endif

//! <b>Overrideable configuration:</b> buffer holding task rx data (USHORT; should be multiples of SPI_PDCMAXRX for ideal rx performance)
#ifndef SPI_BUFFERCNTRX
   #define SPI_BUFFERCNTRX (SPI_PDCMAX* 4)
#endif

//! <b>Overrideable configuration:</b> buffer holding task tx data (USHORT; should be multiples of SPI_PDCMAXTX for ideal tx performance)
#ifndef SPI_BUFFERCNTTX
   #define SPI_BUFFERCNTTX (SPI_PDCMAX* 2)
#endif

#if SPI_HWFIX_PDCRX > 0
   //! <b>Overrideable configuration:</b> buffer holding task rx channel address (UCHAR; must match SPI_PDCMAX* SPI_PDCDEPTH)
   #ifndef SPI_BUFFERCNTADDRRX
      #define SPI_BUFFERCNTADDRRX (SPI_PDCMAX* SPI_PDCDEPTH)
   #endif
#endif

//! <b>Overrideable configuration:</b> number of messages buffered for IRQ to task
#ifndef SPI_MBXCNTMSG
   #define SPI_MBXCNTMSG (SPI_PDCMAX* 2)
#endif

//! <b>Overrideable configuration:</b> highest slave-id <b>+1</b> supported per unit
#ifndef SPI_CHANNELID_MAX
   #define SPI_CHANNELID_MAX 15
#endif

//! <b>Overrideable configuration:</b> number of slaves supported simultaneously per unit
#ifndef SPI_CHANNEL_MAX
   #define SPI_CHANNEL_MAX 15
#endif

//! <b>Overrideable configuration:</b> number of spi units supported simultaneously
#ifndef SPI_UNIT_MAX
   #define SPI_UNIT_MAX 2
#endif

// Prototypes
__task void TskSpiMsg(void *pvParam);
__task void TskSpiMsgChannel(void *pvParam);
__task void TskSpiRx(void *pvParam);
__task void TskSpiTx(void *pvParam);
void SpiIrqHandler(UCHAR ucUnit);

// Each supported spi unit requires a prototype definition here
#if SPI_UNIT_MAX > 0
   SPI_ISR_PROTO(0);
#endif

#if SPI_UNIT_MAX > 1
   SPI_ISR_PROTO(1);
#endif

#endif
