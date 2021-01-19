//!	defines and configuration of MBPB (ModBusPortBus)
/**
		\file		mbpbcfg.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBCFG.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-07-30 initial version
// 2009-08-12 moved MBPB_PORTBUSID_RS232 to mbpbdev.h
// 2009-08-19 added address defines
// 2009-08-20 changed buffer logic from rx and tx to std and dbg
// 2009-08-27 added MBPB_PRODUCT_DEVICE_LENGTH
// 2009-09-03 added MBPB_TIMEOUTMSASC
// 2009-09-04 changed MBPB_TIMEOUTMSASC to MBPB_TIMEOUTUSASC
// 2009-09-25 divided MBPB_TIMEOUTUSASC by 2 for lower asc timeout
// 2009-09-30 divided MBPB_TIMEOUTUSASC by 3 for lower asc timeout
// 2010-04-14 divided MBPB_TIMEOUTUSASC by 12 for lower asc timeout (250ms)
// 2010-04-14 added MBPB_FILTERLIST_COUNT
// 2010-04-21 added MBPB_STACK_TSKMBPBDATA
// 2010-04-23 added MBPB_STACK_TSKMBPBTRGFILTER
// 2010-04-30 changed MBPB_PRODUCT_DEVICE_LENGTH to 5
// 2010-04-30 changed MBPB_ADDR_MAX macro
// 2010-05-21 divided MBPB_TIMEOUTUSASC by 120 for lower asc timeout (25ms)
// 2010-08-04 added MBPB_MBXCNT_EVENT and MBPB_MBXCNT_TRG
// 2011-07-21 added MBPB_PORTBUSID_MASKSHIFT and MBPB_TRANSLATE_MASKSHIFT
// 2011-07-21 MBPB_PORTBUSID_MASK now based on MBPB_PORTBUSID_MASKSHIFT
// 2011-07-21 MBPB_TRANSLATE_MASK now based on MBPB_TRANSLATE_MASKSHIFT
// 2011-08-04 added MBPB_ADDR_MASK define
// 2011-08-10 easier MBPB_TIMEOUTUSASC setting (ms* 1000) but same value (25ms)
// 2011-08-10 added MBPB_TIMEOUTMSFWD define with default of 150ms (6* 25ms)
// 2011-08-17 removed obsolete define MBPB_MBXEVENTMAX
// 2011-08-17 added optinal define MBPB_TIMER_BASE
// 2011-10-27 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBCFG_H
#define SYKAM_PROTOCOL_MODBUS_MBPBCFG_H 1


//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application to override them!
//----------------------------------------------------------------------------

// Defines

//! <b>Overrideable configuration:</b> idle time and time out for incomming asc telegrams
#ifndef MBPB_TIMEOUTUSASC
   #define MBPB_TIMEOUTUSASC     (25* 1000)
#endif

//! <b>Overrideable configuration:</b> response time out of forwarding port (for "routing" requests) 
#ifndef MBPB_TIMEOUTMSFWD
   #define MBPB_TIMEOUTMSFWD     150
#endif

//! <b>Overrideable configuration:</b> first system timer slot used for forwarding time out
#ifndef MBPB_TIMER_BASE
   #define MBPB_TIMER_BASE       TIMER0
#endif

//! <b>Overrideable configuration:</b> count of supported ports (also used to determine last timer slot for forwarding time out)
#ifndef MBPB_PORT_COUNT
   #define MBPB_PORT_COUNT       5
#endif

//! <b>Overrideable configuration:</b> count of supported telegram modes: 1= ASC only, 2= ASC and RTU (#EMBPBMode)
#ifndef MBPB_MODE_COUNT
   #define MBPB_MODE_COUNT       2
#endif

//! <b>Overrideable configuration:</b> count of buffers per port (default == required == 2) (#MBPB_BUFFER_RAW, #MBPB_BUFFER_COOKED)
#ifndef MBPB_BUFFER_COUNT
   #define MBPB_BUFFER_COUNT     2
#endif

//! <b>Overrideable configuration:</b> index of buffer for binary (raw) data
#ifndef MBPB_BUFFER_RAW
   #define MBPB_BUFFER_RAW       0
#endif

//! <b>Overrideable configuration:</b> index of buffer for prepared (asc/rtu) data
#ifndef MBPB_BUFFER_COOKED
   #define MBPB_BUFFER_COOKED    1
#endif

//! <b>Overrideable configuration:</b> shift count to push the 'port/bus' identifier bits into lowest bits
#ifndef MBPB_PORTBUSID_MASKSHIFT
   #define MBPB_PORTBUSID_MASKSHIFT 4
#endif

//! <b>Overrideable configuration:</b> bit mask covering the 'port/bus' identifier within the address byte
#ifndef MBPB_PORTBUSID_MASK
   #define MBPB_PORTBUSID_MASK   (0x07 << MBPB_PORTBUSID_MASKSHIFT)
#endif

//! <b>Overrideable configuration:</b> shift count to push the 'translation' flag bit into lowest bit
#ifndef MBPB_TRANSLATE_MASKSHIFT
   #define MBPB_TRANSLATE_MASKSHIFT 7
#endif

//! <b>Overrideable configuration:</b> bit mask covering the 'translation' flag within the address byte
#ifndef MBPB_TRANSLATE_MASK
   #define MBPB_TRANSLATE_MASK   (0x01 << MBPB_TRANSLATE_MASKSHIFT)
#endif

//! <b>Overrideable configuration:</b> address value reserved for broadcasts (multiple slave addressing)
#ifndef MBPB_ADDR_BROADCAST
   #define MBPB_ADDR_BROADCAST   0
#endif

//! <b>Overrideable configuration:</b> minimum address value for individual slave addressing
#ifndef MBPB_ADDR_MIN
   #define MBPB_ADDR_MIN         1
#endif

//! <b>Overrideable configuration:</b> maximum address value for individual slave addressing
#ifndef MBPB_ADDR_MAX
   #define MBPB_ADDR_MAX         (0xFF- (MBPB_TRANSLATE_MASK | MBPB_PORTBUSID_MASK))
#endif

//! <b>Overrideable configuration:</b> bit mask covering the 'plain address' (without 'port/bus and translation' bits) within the address byte
#ifndef MBPB_ADDR_MASK
   #define MBPB_ADDR_MASK        ((UCHAR) ~(MBPB_TRANSLATE_MASK | MBPB_PORTBUSID_MASK))
#endif

//! <b>Overrideable configuration:</b> count of supported command handlers
#ifndef MBPB_HANDLERS_MAX
   #define MBPB_HANDLERS_MAX     16
#endif

//! <b>Overrideable configuration:</b> length of device product string
#ifndef MBPB_PRODUCT_DEVICE_LENGTH
   #define MBPB_PRODUCT_DEVICE_LENGTH 5
#endif

//! <b>Overrideable configuration:</b> count of supported filters
#ifndef MBPB_FILTERLIST_COUNT
   #define MBPB_FILTERLIST_COUNT 16
#endif

//! <b>Overrideable configuration:</b> count of buffered telegram (frame) state change events (EMBPBEvent of mbpbdata.h)
#ifndef MBPB_MBXCNT_EVENT
   #define MBPB_MBXCNT_EVENT     16
#endif

//! <b>Overrideable configuration:</b> count of buffered register access messages (for filtering)
#ifndef MBPB_MBXCNT_TRG
   #define MBPB_MBXCNT_TRG       16
#endif

//! <b>Overrideable configuration:</b> stack size of data task
#ifndef MBPB_STACK_TSKMBPBDATA
   #define MBPB_STACK_TSKMBPBDATA 600
#endif

//! <b>Overrideable configuration:</b> stack size of filter task for register access
#ifndef MBPB_STACK_TSKMBPBTRGFILTER
   #define MBPB_STACK_TSKMBPBTRGFILTER 600
#endif

#endif
