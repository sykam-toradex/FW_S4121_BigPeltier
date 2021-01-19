//!	message defines of MBPB (ModBusPortBus)
/**
		\file		mbpbmsg.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBMSG.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-04-21 initial version
// 2010-04-22 added MBPB_TRGMSG_RWMASK_... defines
// 2010-04-23 added MBPB_TRGMSG_RWMASK_...BITS defines
// 2011-08-17 added MBPB_TRGMSG_RWMASK_RANGE_ADDR & MBPB_TRGMSG_RWMASK_RANGE_VAL
// 2011-10-27 added public doxygen documentation
// 2011-11-24 added public doxygen documentation
// 2011-11-30 added internal doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBMSG_H
#define SYKAM_PROTOCOL_MODBUS_MBPBMSG_H 1


//!   <b>Trigger/filter option:</b> trigger for any write access
#define MBPB_TRGMSG_RWMASK_WRITE    (1 << 0)

//!   <b>Trigger/filter option:</b> trigger for write access and changing the value
#define MBPB_TRGMSG_RWMASK_CHANGE   (1 << 1)

//!   <b>Trigger/filter option:</b> trigger for any read access
#define MBPB_TRGMSG_RWMASK_READ     (1 << 2)


// Trigger message bit masks (for message check macros)

//! \internal \brief <b>Internal define:</b> cumulated write bit mask
#define MBPB_TRGMSG_RWMASK_WBITS    (MBPB_TRGMSG_RWMASK_WRITE | MBPB_TRGMSG_RWMASK_CHANGE)

//! \internal \brief <b>Internal define:</b> cumulated read bit mask
#define MBPB_TRGMSG_RWMASK_RBITS    (MBPB_TRGMSG_RWMASK_READ)

//! \internal \brief <b>Internal define:</b> cumulated read/write bit mask
#define MBPB_TRGMSG_RWMASK_RWBITS   (MBPB_TRGMSG_RWMASK_WBITS | MBPB_TRGMSG_RWMASK_RBITS)


//!   <b>Trigger/filter option:</b> trigger for write access for changing value to zero
#define MBPB_TRGMSG_RWMASK_VALZERO  (1 << 4)


//!   <b>Trigger/filter option:</b> trigger for specified address range
#define MBPB_TRGMSG_RWMASK_RANGE_ADDR (1 << 6)

//!   <b>Trigger/filter option:</b> trigger for specified value range
#define MBPB_TRGMSG_RWMASK_RANGE_VAL  (1 << 7)


// Trigger message check macros (return lsb shifted value)

//! \internal \brief <b>Internal macro:</b> return (lsb shifted value) matching #MBPB_TRGMSG_RWMASK_WBITS
#define MBPB_TRGMSG_WBITS(x)        ((x) & MBPB_TRGMSG_RWMASK_WBITS)

//! \internal \brief <b>Internal macro:</b> return (lsb shifted value) matching #MBPB_TRGMSG_RWMASK_CHANGE
#define MBPB_TRGMSG_CHANGEBITS(x)   (((x) & MBPB_TRGMSG_RWMASK_CHANGE) >> 1)

//! \internal \brief <b>Internal macro:</b> return (lsb shifted value) matching #MBPB_TRGMSG_RWMASK_RBITS
#define MBPB_TRGMSG_RBITS(x)        (((x) & MBPB_TRGMSG_RWMASK_RBITS) >> 2)


// Trigger message access type

//! \internal \brief <b>Internal define:</b> write access type of trigger message
#define MBPB_TRGMSG_RW_WRITE        (0)

//! \internal \brief <b>Internal define:</b> read access type of trigger message
#define MBPB_TRGMSG_RW_READ         (1)


// Trigger message types

//! \internal \brief <b>Internal define:</b> 16 bit large value & 8 bit small address type trigger message <b>(absolute value & address)</b>
#define MBPB_TRGMSG_ID_ABS_ADDR8    (1)

//! \internal \brief <b>Internal define:</b> base 16 bit large address type trigger message <b>(absolute address)</b>
#define MBPB_TRGMSG_ID_BASE_ADDR16  (2)

//! \internal \brief <b>Internal define:</b> 8 bit small address type trigger message after #MBPB_TRGMSG_ID_BASE_ADDR16 <b>(relative address)</b>
#define MBPB_TRGMSG_ID_REL_ADDR8    (3)

//! \internal \brief <b>Internal define:</b> 8 bit small value & 16 bit large address type trigger message <b>(absolute value & address)</b>
#define MBPB_TRGMSG_ID_ABS_ADDR16   (4)


//! \internal \brief <b>Internal macro:</b> trigger message macro to pack message type/id, register type, access type, small item and large item into a single 32 bit message
/**
      \param   id3                        trigger message type/id (3 == 3 bit)
      \param   type4                      register type (#EMBPBRegType) (4 == 4 bit)
      \param   rw1                        access type (#EMBPBRegRW) (1 == 1 bit)
      \param   val8                       small item (value <b>or</b> address, depends on trigger message type) (8 == 8 bit)
      \param   val16                      large item (value <b>or</b> address, depends on trigger message type) (16 == 16 bit)
      
      \return										packed 32 bit message
      
      \note
               - <b>the param suffix numbers are the bit width</b> (see appended comments above)
*/
#define MBPB_TRGMSG_VAL8VAL16(id3, type4, rw1, val8, val16)     ((((ULONG)id3 & 0x07) << (32- 3)) | ((type4 & 0x0F) << (32- 3- 4)) | ((rw1 & 0x01) << (32- 3- 4- 1)) | ((val8 & 0xFF) << (32- 3- 4- 1- 8)) | (val16 & 0xFFFF))


// Trigger message data retreive macros

//! \internal \brief <b>Internal macro:</b> return trigger message type (e.g. #MBPB_TRGMSG_ID_ABS_ADDR8 ff.)
#define MBPB_TRGMSG_GETID(msg32)    ((msg32 >> (32- 3)) & 0x07)

//! \internal \brief <b>Internal macro:</b> return register type (#EMBPBRegType) of trigger message 
#define MBPB_TRGMSG_GETTYPE(msg32)  ((msg32 >> (32- 3- 4)) & 0x0F)

//! \internal \brief <b>Internal macro:</b> return register access mode (#EMBPBRegRW) of trigger message
#define MBPB_TRGMSG_GETRW(msg32)    ((msg32 >> (32- 3- 4- 1)) & 0x01)

//! \internal \brief <b>Internal macro:</b> return 8 bit small item (value <b>or</b> address, depends on trigger message type) of trigger message
#define MBPB_TRGMSG_GETVAL8(msg32)  ((msg32 >> (32- 3- 4- 1- 8)) & 0xFF)

//! \internal \brief <b>Internal macro:</b> return 16 bit large item (value <b>or</b> address, depends on trigger message type) of trigger message
#define MBPB_TRGMSG_GETVAL16(msg32) ((msg32 & 0xFFFF))

#endif
