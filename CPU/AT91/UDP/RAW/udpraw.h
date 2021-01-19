//!	defines and configuration of AT91SAM7X UDPRAW driver
/**
		\file		udpraw.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/UDP/RAW/UDPRAW.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2014-11-13 initial version
// 2014-12-04 added msgtx get/set macros
// 2015-01-14 added defines UDPRAW_STACK_TSKUDPRAW and ...MSGCHANNEL
// 2015-02-05 added user stacks for TskUdprawRx & TskUdprawTx
// 2015-05-07 added defines to split UDPRAW_TXRES in system and user bit ranges
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_UDP_RAW_UDPRAW_H
#define SYKAM_CPU_AT91_UDP_RAW_UDPRAW_H 1


// Macros

// RX

// MsgRx: new data rx by its UDP_CSR[x] (27 bit, reduced u8RxBkFirst (0= BK0/SETUP, 1= BK1) at unused 14 and 26...0) and channel id (4 bit, 4...1) plus count (1 bit, at 0) (5 bit as msg count)

// Get udp rxbytecnt
#define UDPRAW_GET_RXBYTECNT(csr) (((csr) & AT91C_UDP_RXBYTECNT) >> 16)

// TX

// MsgTx: new data tx by size in bytes (11 bit, 26...16 like UDP_CSR), timeout (16 bit, 15...0) and new dir state (1 bit, 27) to set (in UDP_CSR, 7) after completion (tx only for IN types; forces dir IN for control type before data tx) and channel id (4 bit as msg count) plus count (1 bit, at 0) (5 bit as msg count)

// Get bytes count from msgtx
#define UDPRAW_GETMSGTX_BYTES(bits, msg) (((((ULONG)(msg)) & MSG_MASKMSG((bits))) & 0x07FF0000) >> 16)

// Set msgtx to byte count
#define UDPRAW_SETMSGTX_BYTES(bits, msg, bytes) (MSG_GETMSG((bits), (msg)) | ((((ULONG)(bytes)) & 0x000007FF) << 16))

// Get timeout value from msgtx
#define UDPRAW_GETMSGTX_TIMEOUT(bits, msg) ((((ULONG)(msg)) & MSG_MASKMSG((bits))) & BITS_ULONGLX)

// Set msgtx to timeout value
#define UDPRAW_SETMSGTX_TIMEOUT(bits, msg, timeout) (MSG_GETMSG((bits), (msg)) | ((((ULONG)(timeout)) & BITS_ULONGLX)))

//-Get msgtx next direction value
//-#define UDPRAW_GETMSGTX_NEXTDIR(bits, msg) (((((ULONG)(msg)) & MSG_MASKMSG((bits))) & 0x08000000) >> 27)

// Set msgtx next direction value
//-#define UDPRAW_SETMSGTX_NEXTDIR(bits, msg, nextdir) (MSG_GETMSG((bits), (msg)) | ((((ULONG)(nextdir)) & 0x00000001) << 27))

// COMMON

// Get channel from cnt (endpoint)
#define UDPRAW_GETMSGCNT_CHANNEL(cnt) (((cnt) & 0x0000001E) >> 1)

// Set cnt to channel (endpoint)
#define UDPRAW_SETMSGCNT_CHANNEL(channel) (((channel) & 0x0000000F) << 1)

// Set/get cnt from/to count
#define UDPRAW_MSGCNT_COUNT(cnt) ((cnt) & 0x01)

// Determine (d)ual (p)ort (r)am feature of specified channel (endpoint) is active
#define UDPRAW_IS_DPR(unit, channel) ((!(channel)) || ((channel) == 3) ? (FALSE) : (TRUE))

// Determine maximum byte count of a channel (endpoint)
#define UDPRAW_MAX_BYTE(unit, channel) ((channel) < sizeof(UdprawSize)) ? (((channel) >= 4) ? (256) : ((!(channel)) ? (8) : (64))) : (0)

// Channel (endpoint) data direction rx/host-out
#define UDPRAW_IS_EPTYPE_OUT(eptype) (!(UDPRAW_IS_EPTYPE_IN((eptype))))
 
// Channel (endpoint) data direction tx/host-in
#define UDPRAW_IS_EPTYPE_IN(eptype) ((eptype) & 2^10)

// Channel (endpoint) data direction rx/host-out and tx/host-in (control)
#define UDPRAW_IS_EPTYPE_BOTH(eptype) ((!(UDPRAW_IS_EPTYPE_IN((eptype)))) && (!((eptype) & 2^8)))

// Function template IsrUdpraw
#define UDPRAW_ISR_FUNC(unit) void IsrUdpraw##unit(void) __irq { UdprawIrqHandler(unit); }

// Function prototype IsrUdpraw
#define UDPRAW_ISR_PROTO(unit) void IsrUdpraw##unit(void) __irq

// Function name IsrUdpraw
#define UDPRAW_ISR_NAME(unit) IsrUdpraw##unit

// Defines

// All data channels (endpoints)
#define AT91C_UDP_EPINTX (AT91C_UDP_EPINT0 | AT91C_UDP_EPINT1 | AT91C_UDP_EPINT2 | AT91C_UDP_EPINT3 | AT91C_UDP_EPINT4 | AT91C_UDP_EPINT5)

// All data rx flags (interrupts)
#define AT91C_UDP_RX_DATA_BKX (AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_RX_DATA_BK1)

// All valid channel (endpoint) interrupts (there is NO separate mask register and the UDP_CSR itself is a mixed register!)
#define AT91C_UDP_CSR_IRQS (AT91C_UDP_RX_DATA_BK1 | AT91C_UDP_STALLSENT | AT91C_UDP_RXSETUP | AT91C_UDP_RX_DATA_BK0 | AT91C_UDP_TXCOMP)

// Reduced u8RxBkFirst (0= NONE/RX_DATA_BK0/SETUP, 1= RX_DATA_BK1) at unused bit of CSR (0= BK0/SETUP, 1= BK1) for optional dual software buffer support
#define UDPRAW_IMR_RX_DATA_BKX (1 << 14)

// Data tx system start bit in msg data (27 bit, 26...0)
#define UDPRAW_TXRES_SHIFT_SYSTEM (UDPRAW_CHANNEL_DATA_MSGBITS- 3)

// Data tx user bit mask for msg data
#define UDPRAW_TXRES_MASK_USER ((1 << UDPRAW_TXRES_SHIFT_SYSTEM) -1)

// Data tx system bit mask for msg data
#define UDPRAW_TXRES_MASK_SYSTEM (~UDPRAW_TXRES_MASK_USER)

// Data tx flag system in msg data (at 24)
#define UDPRAW_TXRES_FLAG_SYSTEM (1 << (UDPRAW_TXRES_SHIFT_SYSTEM+ 0))

// Data tx result ok (at 25)
#define UDPRAW_TXRES_OK (UDPRAW_TXRES_FLAG_SYSTEM | (1 << (UDPRAW_TXRES_SHIFT_SYSTEM+ 1)))

// Data tx result err (at 25)
#define UDPRAW_TXRES_ERR (UDPRAW_TXRES_FLAG_SYSTEM | (0 << (UDPRAW_TXRES_SHIFT_SYSTEM+ 1)))

// Flags
#define UDPRAW_UNIT_FLAGS_CONFIGDONE   (1 << 0)
#define UDPRAW_UNIT_FLAGS_CONFIGDONE   (1 << 0)

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

//! <b>Fallback configuration:</b> Define AT91C_MCK ([B]it, [C]onstant, [D]efinition, [F]unction) by obsolete AT91B_MCK
#ifndef AT91C_MCK
   #define AT91C_MCK AT91B_MCK
#endif

/*TBD*///! <b>Overrideable configuration:</b> maximum bits used for msg data in 'msg bit' msg (16bit data/msg to keep 16bit left for counter field)
#ifndef UDPRAW_UNIT_MSG_MSGBITS
   #define UDPRAW_UNIT_MSG_MSGBITS 16
#endif

/*TBD*///! <b>Overrideable configuration:</b> maximum bits used for msg data in 'data' msg (24bit data/msg to keep 8bit left for counter field)
#ifndef UDPRAW_UNIT_DATA_MSGBITS
   #define UDPRAW_UNIT_DATA_MSGBITS 24
#endif

/*TBD*///! <b>Overrideable configuration:</b> maximum bits used for msg data in 'msg bit' msg (16bit data/msg to keep 16bit left for counter field)
#ifndef UDPRAW_CHANNEL_MSG_MSGBITS
   #define UDPRAW_CHANNEL_MSG_MSGBITS 16
#endif

//! <b>Overrideable configuration:</b> maximum bits used for msg data in 'data' msg (27bit data/msg to keep 5bit left for counter field)
#ifndef UDPRAW_CHANNEL_DATA_MSGBITS
   #define UDPRAW_CHANNEL_DATA_MSGBITS 27
#endif

//! <b>Overrideable configuration:</b> type of each message counter
#ifndef UDPRAW_MSGCOUNTERTYPE
   #define UDPRAW_MSGCOUNTERTYPE USHORT
#endif

//! <b>Overrideable configuration:</b> max value of each message counter (limits are defined in .\\Keil\\ARM\\RV31\\Inc\\limits.h)
#ifndef UDPRAW_MSGCOUNTERMAX
   #define UDPRAW_MSGCOUNTERMAX USHRT_MAX
#endif

//! <b>Overrideable configuration:</b> maximum unit msg counter index (13 for upto AT91C_UDP_WAKEUP= 2^13) <b>[messages with index value above are ignored]</b>
#ifndef UDPRAW_UNIT_MSGMAX
   #define UDPRAW_UNIT_MSGMAX 13
#endif

//! <b>Overrideable configuration:</b> maximum channel msg counter index (6 for upto AT91C_UDP_RX_DATA_BK1= 2^6) <b>[messages with index value above are ignored]</b>
#ifndef UDPRAW_CHANNEL_MSGMAX
   #define UDPRAW_CHANNEL_MSGMAX 6
#endif

//! <b>Overrideable configuration:</b> number of tx messages from consumer/client task to tx task
#ifndef UDPRAW_MBXCNTTX
   #define UDPRAW_MBXCNTTX 4
#endif

//! <b>Overrideable configuration:</b> number of messages buffered for IRQ to task
#ifndef UDPRAW_MBXCNTMSG
   #define UDPRAW_MBXCNTMSG 16
#endif

//! <b>Overrideable configuration:</b> default stack size for TskUdprawMsg
#ifndef UDPRAW_STACK_TSKUDPRAWMSG
   #define UDPRAW_STACK_TSKUDPRAWMSG 300
#endif

//! <b>Overrideable configuration:</b> default stack size for TskUdprawMsgChannel
#ifndef UDPRAW_STACK_TSKUDPRAWMSGCHANNEL
   #define UDPRAW_STACK_TSKUDPRAWMSGCHANNEL 300
#endif

//! <b>Overrideable configuration:</b> default stack size for TskUdprawRx
#ifndef UDPRAW_STACK_TSKUDPRAWRXCHANNEL
   #define UDPRAW_STACK_TSKUDPRAWRXCHANNEL 300
#endif

//! <b>Overrideable configuration:</b> default stack size for TskUdprawRx
#ifndef UDPRAW_STACK_TSKUDPRAWTXCHANNEL
   #define UDPRAW_STACK_TSKUDPRAWTXCHANNEL 300
#endif

//! <b>Overrideable configuration:</b> highest channels-id <b>+1</b> supported per unit
#ifndef UDPRAW_CHANNELID_MAX
   #define UDPRAW_CHANNELID_MAX 6
#endif

//! <b>Overrideable configuration:</b> number of channels supported simultaneously per unit
#ifndef UDPRAW_CHANNEL_MAX
   #define UDPRAW_CHANNEL_MAX 6
#endif

//! <b>Overrideable configuration:</b> number of udpraw units supported simultaneously
#ifndef UDPRAW_UNIT_MAX
   #define UDPRAW_UNIT_MAX 1
#endif

// Prototypes
__task void TskUdprawMsg(void *pvParam);
__task void TskUdprawMsgChannel(void *pvParam);
__task void TskUdprawRxChannel(void *pvParam);
__task void TskUdprawTxChannel(void *pvParam);
void UdprawIrqHandler(UCHAR ucUnit);
void UdprawIrqHandlerChannel(UCHAR ucUnit, UCHAR ucChannels);

// Each supported udpraw unit requires a prototype definition here
#if UDPRAW_UNIT_MAX > 0
   UDPRAW_ISR_PROTO(0);
#endif

#if UDPRAW_UNIT_MAX > 1
   UDPRAW_ISR_PROTO(1);
#endif

#endif
