//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPB.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-07-22 initial version
// 2009-07-23 added own struct for semaphores, adjusted (un)lock macro
// 2009-07-29 added portbusID and translate masks
// 2009-07-30 cleaned up and moved other contents to separate header files
// 2010-04-15 added TskMBPBTrgFilter
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPB_H
#define SYKAM_PROTOCOL_MODBUS_MBPB_H 1


// Prototypes

// Prototypes for tasks
__task void TskMBPBMsg(void *pvParam);
__task void TskMBPBData(void *pvParam);
__task void TskMBPBTrgFilter(void);

#endif
