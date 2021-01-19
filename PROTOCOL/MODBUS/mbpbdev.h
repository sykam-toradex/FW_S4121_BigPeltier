//!	defines and configuration of MBPB ports (ModBusPortBus)
/**
		\file		mbpbdev.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBDEV.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-12 initial version
// 2010-05-07 added defines for task priorities
// 2010-08-04 added defines MBPB_PORTBUSID_SPI and MBPB_INDEX_SPI
// 2010-08-04 added defines MBPB_PORTBUSID_USB and MBPB_INDEX_USB
// 2010-08-04 added defines MBPB_PORTBUSID_RS485 and MBPB_INDEX_RS485
// 2010-08-04 added defines MBPB_PORTBUSID_LAN and MBPB_INDEX_LAN
// 2011-08-04 renumbered MBPB_INDEX_... to reflect (MBPB_PORTBUSID_... - 1)
// 2011-08-04 removed obsolete MBPB_PORTBUSID_... defines
// 2011-10-27 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBDEV_H
#define SYKAM_PROTOCOL_MODBUS_MBPBDEV_H 1


//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application to override them!
//----------------------------------------------------------------------------

// Defines

//! <b>Overrideable port configuration:</b> index/port-id of SPI
#ifndef MBPB_INDEX_SPI
   #define MBPB_INDEX_SPI        (0)
#endif

//! <b>Overrideable port configuration:</b> index/port-id of RS232
#ifndef MBPB_INDEX_RS232
   #define MBPB_INDEX_RS232      (1)
#endif

//! <b>Overrideable port configuration:</b> index/port-id of USB
#ifndef MBPB_INDEX_USB
   #define MBPB_INDEX_USB        (2)
#endif

//! <b>Overrideable port configuration:</b> index/port-id of RS485
#ifndef MBPB_INDEX_RS485
   #define MBPB_INDEX_RS485      (3)
#endif

//! <b>Overrideable port configuration:</b> index/port-id of LAN
#ifndef MBPB_INDEX_LAN
   #define MBPB_INDEX_LAN        (4)
#endif

//! <b>Overrideable port configuration:</b> priority of trigger filtering task
#ifndef MBPB_TSKTRGFILTERPRI
   #define MBPB_TSKTRGFILTERPRI (OS_P_HIGH)
#endif

//! <b>Overrideable port configuration:</b> priority of data task
#ifndef MBPB_TSKDATAPRI
   #define MBPB_TSKDATAPRI (OS_P_ABOVENORMAL)
#endif

//! <b>Overrideable port configuration:</b> priority of message task
#ifndef MBPB_TSKMSGPRI
   #define MBPB_TSKMSGPRI (OS_P_ABOVENORMAL)
#endif

#endif
