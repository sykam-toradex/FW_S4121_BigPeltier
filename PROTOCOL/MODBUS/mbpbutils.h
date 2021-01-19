//!	\internal \brief <b>INTERNAL</b> interface of MBPB utilities (ModBusPortBus)
/**
      \internal
		\file		mbpbutils.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBUTILS.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-20 initial version
// 2010-04-14 changed MBPBUtilSetBits to return the written byte value
// 2011-11-30 added internal doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBUTILS_H
#define SYKAM_PROTOCOL_MODBUS_MBPBUTILS_H 1


// Prototypes

//!	\internal \brief <b>Internal function:</b> return exeption code for specified error code eErrorCode
EMBPBException MBPBError2Exception(EMBPBErrorCode eErrorCode);

//!	\internal \brief <b>Internal function:</b> set and return ucNBits bits in data ucByteBuf at bit offset usBitOffset to value ucValue
/**   
      \param	*ucByteBuf					   pointer to data to retrieve bits from
      \param   usBitOffset                offset of first bit to retrieve
      \param   ucNBits                    number of bits to retrieve
      \param   ucValue                    up to 8 bits to set in data

      \return                             updated bits (ucNBits) in data ucByteBuf
*/
UCHAR MBPBUtilSetBits(UCHAR *ucByteBuf, USHORT usBitOffset, UCHAR ucNBits, UCHAR ucValue);

//!	\internal \brief <b>Internal function:</b> return ucNBits bits of data ucByteBuf at bit offset usBitOffset
/**   
      \param	*ucByteBuf					   pointer to data to retrieve bits from
      \param   usBitOffset                offset of first bit to retrieve
      \param   ucNBits                    number of bits to retrieve

      \return                             current bits (ucNBits) in data ucByteBuf
*/
UCHAR MBPBUtilGetBits(UCHAR *ucByteBuf, USHORT usBitOffset, UCHAR ucNBits);

//!	\internal \brief <b>Internal function:</b> return binary value of asc ucCharacter 
UCHAR MBPBChar2Bin(UCHAR ucCharacter);

//!	\internal \brief <b>Internal function:</b>return asc value of binary ucByte
UCHAR MBPBBin2Char(UCHAR ucByte);

//!	\internal \brief <b>Internal function:</b>calculate and return Longitudinal Redundancy Checksum for data pucFrame with length usLen
/**   
      \param	*pucFrame					   pointer to internal raw (binary) telegram data
      \param   usLen                      size of internal raw (binary) telegram data <b>in bytes</b>

      \return                             Longitudinal Redundancy Checksum for data pucFrame with length usLen
*/
UCHAR MBPBLRC(UCHAR *pucFrame, USHORT usLen);

//!	\internal \brief <b>Internal function:</b>calculate and return Cyclic Redundancy Checksum for data pucFrame with length usLen
/**   
      \param	*pucFrame					   pointer to internal raw (binary) telegram data
      \param   usLen                      size of internal raw (binary) telegram data <b>in bytes</b>

      \return                             Cyclic Redundancy Checksum for data pucFrame with length usLen
*/
USHORT MBPBCRC16(UCHAR *pucFrame, USHORT usLen);

#endif
