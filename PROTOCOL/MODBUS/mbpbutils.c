//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBUTILS.C
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-20 initial version
// 2009-09-10 changed MBPBChar2Bin to accept also lower case hex digits
// 2010-04-14 changed MBPBUtilSetBits to return the written byte value
// 2010-04-23 MBPBUtilSetBits now returns the written byte without target mem
// 2010-04-30 replaced BITCOUNT_UCHAR by global macro BITS_OF()
// 2010-04-30 removed assertions of MBPBUtilSetBits by conditional branch
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-07-12 added MBPB_EINVAL case to MBPBError2Exception
// 2013-04-11 added function headers with return data type comment
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Include
#include <stdlib.h>
#include <string.h>

#include "..\..\..\rtl\support.h"

#include "mbpbcfg.h"
#include "mbpbframe.h"
#include "mbpbdata.h"

//----------------------------------------------------------------------------
// EMBPBException MBPBError2Exception
//----------------------------------------------------------------------------
EMBPBException MBPBError2Exception(EMBPBErrorCode EErrorCode)
{
    EMBPBException    EStatus;

    switch (EErrorCode)
    {
        case MBPB_ENOERR:
            EStatus= MBPB_EX_NONE;
            break;

        case MBPB_ENOREG:
            EStatus= MBPB_EX_ILLEGAL_DATA_ADDRESS;
            break;
            
        case MBPB_EINVAL:
            EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
            break;

        case MBPB_ETIMEDOUT:
            EStatus= MBPB_EX_SLAVE_BUSY;
            break;

        default:
            EStatus= MBPB_EX_SLAVE_DEVICE_FAILURE;
            break;
    }

    return (EStatus);
}

//----------------------------------------------------------------------------
// UCHAR MBPBUtilSetBits
//----------------------------------------------------------------------------
UCHAR MBPBUtilSetBits(UCHAR *ucByteBuf, USHORT usBitOffset, UCHAR ucNBits, UCHAR ucValue)
{
   USHORT          usWordBuf= 0;
   
   USHORT          usMask;
   USHORT          usByteOffset;
   USHORT          usNPreBits;
   USHORT          usValue= ucValue;
   
   if (ucNBits <= 8)
   {     
      // Calculate byte offset for first byte containing the bit values starting at usBitOffset
      usByteOffset= (USHORT)((usBitOffset) / BITS_OF(UCHAR));
      
      // How many bits precede our bits to set
      usNPreBits= (USHORT)(usBitOffset - usByteOffset* BITS_OF(UCHAR));
      
      // Move bit field into position over bits to set
      usValue <<= usNPreBits;
      
      // Prepare a mask for setting the new bits
      usMask= (USHORT)((1 << (USHORT) ucNBits) - 1);
      usMask <<= usBitOffset - usByteOffset* BITS_OF(UCHAR);
      
      // copy bits into temporary storage
      if (ucByteBuf)
      {
         usWordBuf= ucByteBuf[usByteOffset];
         usWordBuf |= ucByteBuf[usByteOffset + 1] << BITS_OF(UCHAR);
      }
      else
      {
         usWordBuf= NULL;
      }
      
      // Zero out bit field bits and then or-value bits into them
      usWordBuf= (USHORT)((usWordBuf & (~usMask)) | usValue);
      
      // move bits back into storage
      if (ucByteBuf)
      {
         ucByteBuf[usByteOffset]= (UCHAR)(usWordBuf & BITS_UCHAR);
         ucByteBuf[usByteOffset + 1]= (UCHAR)(usWordBuf >> BITS_OF(UCHAR));
      }
   }
         
   return ((UCHAR) usWordBuf);
}

//----------------------------------------------------------------------------
// UCHAR MBPBUtilGetBits
//----------------------------------------------------------------------------
UCHAR MBPBUtilGetBits(UCHAR *ucByteBuf, USHORT usBitOffset, UCHAR ucNBits)
{
   USHORT          usWordBuf= 0;
   
   USHORT          usMask;
   USHORT          usByteOffset;
   USHORT          usNPreBits;
   
   if (ucNBits <= 8)
   {
      // Calculate byte offset for first byte containing the bit values starting at usBitOffset
      usByteOffset= (USHORT)((usBitOffset) / BITS_OF(UCHAR));
      
      // How many bits precede our bits to set
      usNPreBits= (USHORT)(usBitOffset - usByteOffset* BITS_OF(UCHAR));
      
      // Prepare a mask for setting the new bits
      usMask= (USHORT)((1 << (USHORT) ucNBits) - 1);
      
      // copy bits into temporary storage
      usWordBuf= ucByteBuf[usByteOffset];
      usWordBuf |= ucByteBuf[usByteOffset + 1] << BITS_OF(UCHAR);
      
      // throw away unneeded bits
      usWordBuf >>= usNPreBits;
      
      // mask away bits above the requested bitfield
      usWordBuf &= usMask;
   }
      
   return ((UCHAR) usWordBuf);
}

//----------------------------------------------------------------------------
// UCHAR MBPBChar2Bin
//----------------------------------------------------------------------------
UCHAR MBPBChar2Bin(UCHAR ucCharacter)
{
   UCHAR ucResult;
   
   if (( ucCharacter >= '0') && (ucCharacter <= '9'))
   {
      ucResult= (UCHAR)(ucCharacter- '0');
   }
   else if (( ucCharacter >= 'A') && (ucCharacter <= 'F'))
   {
      ucResult= (UCHAR)(ucCharacter- 'A' + 0x0A);
   }
   else if (( ucCharacter >= 'a') && (ucCharacter <= 'f'))
   {
      ucResult= (UCHAR)(ucCharacter- 'a' + 0x0A);
   }
   else
   {
      ucResult= BITS_UCHAR;
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR MBPBBin2Char
//----------------------------------------------------------------------------
UCHAR MBPBBin2Char(UCHAR ucByte)
{
   UCHAR ucResult;
   
   if (ucByte <= 0x09)
   {
      ucResult= (UCHAR)('0' + ucByte);
   }
   else if ((ucByte >= 0x0A) && (ucByte <= 0x0F))
   {
      ucResult= (UCHAR)(ucByte - 0x0A + 'A');
   }
   else
   {
      ucResult= 0;
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR MBPBLRC
//----------------------------------------------------------------------------
UCHAR MBPBLRC(UCHAR *pucFrame, USHORT usLen)
{
   USHORT usPos;
   
   UCHAR ucLRC= 0;
   
   for (usPos= 0; usPos < usLen; usPos++)
   {
      // Add buffer byte without carry
      ucLRC+= *pucFrame++;
   }
   
   // Return twos complement
   ucLRC= (UCHAR) (-((CHAR) ucLRC));
   
   return (ucLRC);
}

//----------------------------------------------------------------------------
// const UCHAR ucCRCHiAr[]
//----------------------------------------------------------------------------
const UCHAR ucCRCHiAr[]=
{
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
   0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
   0x00, 0xC1, 0x81, 0x40
};

//----------------------------------------------------------------------------
// const UCHAR ucCRCLoAr[]
//----------------------------------------------------------------------------
const UCHAR ucCRCLoAr[]=
{
   0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
   0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
   0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
   0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
   0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
   0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
   0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
   0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
   0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
   0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
   0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
   0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
   0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
   0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
   0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
   0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
   0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
   0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
   0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
   0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
   0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
   0x41, 0x81, 0x80, 0x40
};

//----------------------------------------------------------------------------
// USHORT MBPBCRC16
//----------------------------------------------------------------------------
USHORT MBPBCRC16(UCHAR *pucFrame, USHORT usLen)
{
   UCHAR   ucCRCHi= 0xFF;
   UCHAR   ucCRCLo= 0xFF;
   USHORT  usPos;
   USHORT  usIndex;
   
   for (usPos= 0; usPos < usLen; usPos++)
   {
      usIndex= ucCRCLo^ *( pucFrame++ );
      ucCRCLo= (UCHAR)(ucCRCHi ^ ucCRCHiAr[usIndex]);
      ucCRCHi= ucCRCLoAr[usIndex];
   }
   
   return ((USHORT)(ucCRCHi << 8 | ucCRCLo));
}
