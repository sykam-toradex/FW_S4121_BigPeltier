//!	register definitions and functions of MBPB (ModBusPortBus)
/**
		\file		mbpbregcb.c
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBREGCB.C
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-20 initial version
// 2009-08-26 added fix to suppress warning for zero based registers
// 2009-08-27 added fix to suppress warning for zero based inputs
// 2009-09-02 added initial version of MBPBRegCoilsCB
// 2009-09-02 added initial version of MBPBRegDiscreteCB
// 2009-09-02 fixed MBPBRegCoilsCB & MBPBRegDiscreteCB to use byte from bit 0
// 2009-09-03 code and comments cleanup
// 2010-04-14 declared usRegHoldingBuf static (private) again
// 2010-04-14 added external reference to SMBPBApp
// 2010-04-15 fixed MBPBRegInputCB & MBPBRegHoldingCB to use byte start at [0]
// 2010-04-15 added address mask trigger check to all functions
// 2010-04-21 added MBPBRegMsg used by all register functions to send trg msgs
// 2010-04-22 added write change filter to MBPBRegHoldingCB and MBPBRegCoilsCB
// 2010-04-22 moved trg msg params into STrgMsg structure
// 2010-04-22 added semaphore to lock STrgMsg structure during update
// 2010-04-23 fixed optional value change filter for write access
// 2010-04-23 enhanced address mask match filtering
// 2010-04-23 added off-memory write "access" messages for masked addresses
// 2010-04-23 moved ucBitMask[] to support.c
// 2010-04-23 added initial version of MBPBRegAccess
// 2010-04-29 completed & fixed MBPBRegAccess MBPB_TRGTYPE_INPUT_READ
// 2010-04-29 completed & fixed MBPBRegAccess MBPB_TRGTYPE_HOLDING_READ/WRITE
// 2010-04-30 completed & fixed MBPBRegAccess MBPB_TRGTYPE_COILS_READ/WRITE
// 2010-04-30 completed & fixed MBPBRegAccess MBPB_TRGTYPE_DISCRETE_READ
// 2010-04-30 changed locking of trigger data & added reg memory data locking
// 2010-04-30 replaced BITCOUNT_UCHAR by global macro BITS_OF()
// 2010-04-30 added configurable 'zerobased' define switch (0/1) for each type
// 2010-04-30 added u16Timeout to MBPBRegAccess
// 2010-06-16 changed MBPBRegAccess to use own dedicated defines for type & rw
// 2010-06-17 made all defines overrideable by configuration header file
// 2010-06-18 changed param names of MBPBRegAccess to avoid further confusion
// 2010-06-18 added wrapper MBPBRegAccessStruct to MBPBRegAccess
// 2010-06-30 fixed index of ucBitMask[] by +usBitOffset in MBPBRegCoilsCB
// 2010-06-30 fixed index of ucBitMask[] by +usBitOffset in MBPBRegDiscreteCB
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-23 changed to _u(l)min & _u(l)max intrinsics
// 2011-07-14 added user validation function integration to MBPBRegHoldingCB
// 2011-10-27 changed param type of bRW of MBPBRegAccess from BOOL to bool
// 2011-11-30 added public doxygen documentation
// 2013-04-11 added function headers with return data type comment
// 2013-04-25 added new callback function MBPBRegGeneralReferenceCB
// 2013-05-02 fixed function MBPBRegGeneralReferenceCB for write access
// 2015-04-22 added combined 32 bit write access in MBPBRegGeneralReferenceCB
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "mbpbcfg.h"
#include "mbpbmsg.h"
#include "mbpbframe.h"
#include "mbpbdata.h"
#include "mbpbutils.h"


// Defines

//! <b>Overrideable configuration:</b> start address of holding (read/write 16 bit) registers
#ifndef MBPB_REG_HOLDING_START
   #define MBPB_REG_HOLDING_START           (0x0000)
#endif

//! <b>Overrideable configuration:</b> number of holding (read/write 16 bit) registers
#ifndef MBPB_REG_HOLDING_NREGS
   #define MBPB_REG_HOLDING_NREGS           (130)
#endif

//! <b>Overrideable configuration:</b> zero base memory option of holding (read/write 16 bit) registers (=1 to avoid unused zero memory item, 0 == disable)
#ifndef MBPB_REG_HOLDING_ZEROBASED
   #define MBPB_REG_HOLDING_ZEROBASED       (1)
#endif


//! <b>Overrideable configuration:</b> start address of input (read only 16 bit) registers
#ifndef MBPB_REG_INPUT_START
   #define MBPB_REG_INPUT_START             (0x0000)
#endif

//! <b>Overrideable configuration:</b> number of input (read only 16 bit) registers   
#ifndef MBPB_REG_INPUT_NREGS
   #define MBPB_REG_INPUT_NREGS             (4)
#endif

//! <b>Overrideable configuration:</b> zero base memory option of input (read only 16 bit) registers (=1 to avoid unused zero memory item, 0 == disable)
#ifndef MBPB_REG_INPUT_ZEROBASED
   #define MBPB_REG_INPUT_ZEROBASED         (1)
#endif


//! <b>Overrideable configuration:</b> start address of coils (read/write single bit) registers
#ifndef MBPB_REG_COILS_START
   #define MBPB_REG_COILS_START             (0)
#endif

//! <b>Overrideable configuration:</b> number of coils (read/write single bit) registers
#ifndef MBPB_REG_COILS_SIZE
   #define MBPB_REG_COILS_SIZE              (16)
#endif

//! <b>Overrideable configuration:</b> zero base memory option of coils (read/write single bit) registers (=1 to avoid unused zero memory item, 0 == disable)
#ifndef MBPB_REG_COILS_ZEROBASED
   #define MBPB_REG_COILS_ZEROBASED         (1)
#endif


//! <b>Overrideable configuration:</b> start address of discrete (read only single bit) registers
#ifndef MBPB_REG_DISCRETE_START
   #define MBPB_REG_DISCRETE_START          (0)
#endif

//! <b>Overrideable configuration:</b> number of discrete (read only single bit) registers
#ifndef MBPB_REG_DISCRETE_SIZE
   #define MBPB_REG_DISCRETE_SIZE           (16)
#endif

//! <b>Overrideable configuration:</b> zero base memory option of discrete (read only single bit) registers (=1 to avoid unused zero memory item, 0 == disable)
#ifndef MBPB_REG_DISCRETE_ZEROBASED
   #define MBPB_REG_DISCRETE_ZEROBASED      (1)
#endif


// Externals
extern UCHAR ucBitMask[];
extern struct _SMBPBApp SMBPBApp;

// Storage
static USHORT   usRegHoldingBuf[MBPB_REG_HOLDING_NREGS];

static USHORT   usRegInputBuf[MBPB_REG_INPUT_NREGS];

static UCHAR    ucRegCoilsBuf[MBPB_REG_COILS_SIZE/ 8];

static UCHAR    ucRegDiscBuf[MBPB_REG_DISCRETE_SIZE/ 8];

// Fix for unnecessary warnings that may occur by define settings
#if MBPB_REG_HOLDING_START == 0 || MBPB_REG_INPUT_START == 0
   // Suppress warning for "#186-D: pointless comparision of unsigned integer with zero"
   #pragma diag_remark 186
#endif

//----------------------------------------------------------------------------
// bool MBPBRegMsg
//----------------------------------------------------------------------------
bool MBPBRegMsg(bool *bAddrBase, bool bVal8, UCHAR ucType, bool bRW, USHORT usAddr, USHORT usAddrIdx, USHORT usVal)
{
   bool bResult= 0;
   
   bool bAddr8;
   ULONG ulValue;
   
   if (bAddrBase)
   {
      bAddr8= (usAddr <= 255) ? (TRUE) : (FALSE);
      
      if (SMBX_CHECK(SMbxTrg, SMBPBApp.SDataRW.STrgMsg) >= ((bAddr8 || bVal8 || *bAddrBase) ? (1) : (2)))
      {
         if (bVal8)
         {
            // Addr16/val8 msg
            ulValue= MBPB_TRGMSG_VAL8VAL16(MBPB_TRGMSG_ID_ABS_ADDR16, ucType, bRW, usVal, usAddr);
            SMBX_SEND(SMbxTrg, SMBPBApp.SDataRW.STrgMsg, ulValue);
         }
         else
         {
            if (bAddr8)
            {
               // Addr8/val16 msg
               ulValue= MBPB_TRGMSG_VAL8VAL16(MBPB_TRGMSG_ID_ABS_ADDR8, ucType, bRW, usAddr, usVal);
               SMBX_SEND(SMbxTrg, SMBPBApp.SDataRW.STrgMsg, ulValue);
            }
            else
            {
               if (!*bAddrBase || !(usAddrIdx % 256))
               {                    
                  // Leading base addr16 msg
                  ulValue= MBPB_TRGMSG_VAL8VAL16(MBPB_TRGMSG_ID_BASE_ADDR16, ucType, bRW, 0, usAddr);
                  SMBX_SEND(SMbxTrg, SMBPBApp.SDataRW.STrgMsg, ulValue);
      
                  *bAddrBase= TRUE;
               }
               
               // Relative addr8/val16 msg
               ulValue= MBPB_TRGMSG_VAL8VAL16(MBPB_TRGMSG_ID_REL_ADDR8, ucType, bRW, (usAddrIdx % 256), usVal);
               SMBX_SEND(SMbxTrg, SMBPBApp.SDataRW.STrgMsg, ulValue);
            }
         }
                  
         bResult= 1;
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// bool MBPBRegAccess
//----------------------------------------------------------------------------
bool MBPBRegAccess(UCHAR ucType, bool bRW, USHORT usAddress, USHORT usCount, void *pvData, void **pvIntern, U16 u16Timeout)
{
   bool bResult= FALSE;
   USHORT usBits= usCount;
   
   INT iRegIndex;
   USHORT usAddrIdx;
   USHORT usByteIdx;
   USHORT usByteOffset;
   USHORT usBitOffset;
  
   if (usAddress)
   {
      switch (ucType)
      {
         case MBPB_REGTYPE_INPUT:       
            if ((usAddress >= MBPB_REG_INPUT_START) && (usAddress+ usCount <= (MBPB_REG_INPUT_START+ MBPB_REG_INPUT_NREGS+ MBPB_REG_INPUT_ZEROBASED)))
            {
               // Lock register memory
               if (SSEM_LOCKUSR(SSemLockInput, SMBPBApp.SDataRW.SReg, u16Timeout))
               {
                  bResult= TRUE;
                  iRegIndex= (INT)(usAddress - MBPB_REG_INPUT_START- MBPB_REG_INPUT_ZEROBASED);
                  
                  if (pvIntern) *pvIntern= &usRegInputBuf[iRegIndex];
                  if (pvData)
                  {
                     switch (bRW)
                     {
                        case MBPB_REGRW_READ:
                           for (usAddrIdx= 0; usAddrIdx < usCount; usAddrIdx++, iRegIndex++)
                           {  
                              *((USHORT *)pvData+ usAddrIdx)= usRegInputBuf[iRegIndex];
                           }
                           break;
                           
                        case MBPB_REGRW_WRITE:
                           for (usAddrIdx= 0; usAddrIdx < usCount; usAddrIdx++, iRegIndex++)
                           {  
                              usRegInputBuf[iRegIndex]= *((USHORT *)pvData+ usAddrIdx);
                           }
                           break;
                     }
                  }
                  
                  SSEM_UNLOCK(SSemLockInput, SMBPBApp.SDataRW.SReg);
               }
            }
            break;
            
         case MBPB_REGTYPE_HOLDING:
            if ((usAddress >= MBPB_REG_HOLDING_START) && (usAddress+ usCount <= (MBPB_REG_HOLDING_START+ MBPB_REG_HOLDING_NREGS+ MBPB_REG_HOLDING_ZEROBASED)))
            {
               // Lock register memory
               if (SSEM_LOCKUSR(SSemLockHolding, SMBPBApp.SDataRW.SReg, u16Timeout))
               {            
                  bResult= TRUE;
                  iRegIndex= (INT)(usAddress- MBPB_REG_HOLDING_START- MBPB_REG_HOLDING_ZEROBASED);
      
                  if (pvIntern) *pvIntern= &usRegHoldingBuf[iRegIndex];            
                  if (pvData)
                  {
                     switch (bRW)
                     {
                        case MBPB_REGRW_READ:
                           for (usAddrIdx= 0; usAddrIdx < usCount; usAddrIdx++, iRegIndex++)
                           {
                              *((USHORT *)pvData+ usAddrIdx)= usRegHoldingBuf[iRegIndex];
                           }
                           break;
                        
                        case MBPB_REGRW_WRITE:
                           for (usAddrIdx= 0; usAddrIdx < usCount; usAddrIdx++, iRegIndex++)
                           {
                              usRegHoldingBuf[iRegIndex]= *((USHORT *)pvData+ usAddrIdx);
                           }
                           break;
                     }                  
                  }
                  
                  SSEM_UNLOCK(SSemLockHolding, SMBPBApp.SDataRW.SReg);
               }
            }
            break;
            
         case MBPB_REGTYPE_COILS:
            if ((usAddress >= MBPB_REG_COILS_START) && (usAddress+ usCount <= (MBPB_REG_COILS_START+ MBPB_REG_COILS_SIZE+ MBPB_REG_COILS_ZEROBASED)))
            {
               // Lock register memory
               if (SSEM_LOCKUSR(SSemLockCoils, SMBPBApp.SDataRW.SReg, u16Timeout))
               {            
                  bResult= TRUE;
                  usBitOffset= (unsigned short)(usAddress- MBPB_REG_COILS_START- MBPB_REG_COILS_ZEROBASED);
                  usByteOffset = (USHORT)((usBitOffset) / BITS_OF(UCHAR));
                  
                  if (pvIntern) *pvIntern= &ucRegCoilsBuf[usByteOffset];
                  if (pvData)
                  {
                     switch (bRW)
                     {
                        case MBPB_REGRW_READ:
                           for (usByteIdx= 0; usByteIdx < usCount; usByteIdx += BITS_OF(UCHAR), usBitOffset += BITS_OF(UCHAR), usBits -= _umin(usBits, BITS_OF(UCHAR)))
                           {
                              *((UCHAR *)pvData+ usByteIdx)= MBPBUtilGetBits(ucRegCoilsBuf, usBitOffset, (UCHAR) _umin(usBits, 8));
                           }
                           break;
                           
                        case MBPB_REGRW_WRITE:
                           for (usByteIdx= 0; usByteIdx < usCount; usByteIdx += BITS_OF(UCHAR), usBitOffset += BITS_OF(UCHAR), usBits -= _umin(usBits, BITS_OF(UCHAR)))
                           {
                              MBPBUtilSetBits(ucRegCoilsBuf, usBitOffset, (UCHAR) _umin(usBits, BITS_OF(UCHAR)), *((UCHAR *)pvData+ usByteIdx));
                           }
                           break;
                     }
                  }
                  
                  SSEM_UNLOCK(SSemLockCoils, SMBPBApp.SDataRW.SReg);
               }
            }
            break;
            
         case MBPB_REGTYPE_DISCRETE:
            if ((usAddress >= MBPB_REG_DISCRETE_START) && (usAddress+ usCount <= (MBPB_REG_DISCRETE_START+ MBPB_REG_DISCRETE_SIZE+ MBPB_REG_DISCRETE_ZEROBASED)))
            {
               // Lock register memory
               if (SSEM_LOCKUSR(SSemLockDiscrete, SMBPBApp.SDataRW.SReg, u16Timeout))
               {
                  bResult= TRUE;
                  usBitOffset= (unsigned short)(usAddress- MBPB_REG_DISCRETE_START- MBPB_REG_DISCRETE_ZEROBASED);
                  usByteOffset = (USHORT)((usBitOffset) / BITS_OF(UCHAR));
                  
                  if (pvIntern) *pvIntern= &ucRegDiscBuf[usByteOffset];
                  if (pvData)
                  {
                     switch (bRW)
                     {
                        case MBPB_REGRW_READ:
                           for (usByteIdx= 0; usByteIdx < usCount; usByteIdx += BITS_OF(UCHAR), usBitOffset += BITS_OF(UCHAR), usBits -= _umin(usBits, BITS_OF(UCHAR)))
                           {
                              *((UCHAR *)pvData+ usByteIdx)= MBPBUtilGetBits(ucRegDiscBuf, usBitOffset, (UCHAR) _umin(usBits, BITS_OF(UCHAR)));
                           }
                           break;
                           
                        case MBPB_REGRW_WRITE:
                           for (usByteIdx= 0; usByteIdx < usCount; usByteIdx += BITS_OF(UCHAR), usBitOffset += BITS_OF(UCHAR), usBits -= _umin(usBits, BITS_OF(UCHAR)))
                           {
                              MBPBUtilSetBits(ucRegDiscBuf, usBitOffset, (UCHAR) _umin(usBits, BITS_OF(UCHAR)), *((UCHAR *)pvData+ usByteIdx));
                           }
                           break;
                     }
                  }
                  
                  SSEM_UNLOCK(SSemLockDiscrete, SMBPBApp.SDataRW.SReg);
               }
            }
            break;
      }
   }
      
   return (bResult);
}

//----------------------------------------------------------------------------
// bool MBPBRegAccessStruct
//----------------------------------------------------------------------------
bool MBPBRegAccessStruct(struct _SMBPBRegAccess *pSMBPBRegAccess)
{
   return (MBPBRegAccess(pSMBPBRegAccess->ucType, pSMBPBRegAccess->bRW, (pSMBPBRegAccess->usAddr+ pSMBPBRegAccess->usAddrOffs), pSMBPBRegAccess->usCount, pSMBPBRegAccess->pvData, pSMBPBRegAccess->pvIntern, pSMBPBRegAccess->u16To));
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBRegInputCB
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBRegInputCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
   EMBPBErrorCode EStatus= MBPB_ENOERR;
   bool bAddrBase= FALSE;
   
   INT iRegIndex;
   USHORT usAddrIdx;
   USHORT usAddr;
   
   if ((usAddress >= MBPB_REG_INPUT_START) && (usAddress+ usNRegs <= (MBPB_REG_INPUT_START+ MBPB_REG_INPUT_NREGS+ MBPB_REG_INPUT_ZEROBASED)))
   {
      iRegIndex= (INT)(usAddress - MBPB_REG_INPUT_START- MBPB_REG_INPUT_ZEROBASED);

      // Lock register memory
      if (SSEM_LOCK(SSemLockInput, SMBPBApp.SDataRW.SReg))
      {
         for (usAddrIdx= 0; usAddrIdx < usNRegs; usAddrIdx++, iRegIndex++)
         {
            *pucRegBuffer++= (unsigned char)(usRegInputBuf[iRegIndex] >> BITS_OF(UCHAR));
            *pucRegBuffer++= (unsigned char)(usRegInputBuf[iRegIndex] & 0xFF);
            
            // Lock STrgMsg
            if (SSEM_LOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg))
            {            
               // Check access match
               if (SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_INPUT_READ].ucR)
               {
                  // Check address/value mask match
                  usAddr= usAddress+ usAddrIdx;
                  if (usAddr == (usAddr & SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_INPUT_READ].usAddrMask))
                  {
                     MBPBRegMsg(&bAddrBase, FALSE, MBPB_TRGTYPE_INPUT_READ, MBPB_TRGMSG_RW_READ, usAddr, usAddrIdx, usRegInputBuf[iRegIndex]);
                  }
               }
               
               SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
            }
         }
         
         SSEM_UNLOCK(SSemLockInput, SMBPBApp.SDataRW.SReg);
      }
   }
   else
   {
      EStatus= MBPB_ENOREG;
   }
   
   return (EStatus);
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBRegHoldingCB
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBRegHoldingCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNRegs, EMBPBRegisterMode EMode)
{
   EMBPBErrorCode EStatus= MBPB_ENOERR;
   bool bAddrBase= FALSE;
   
   INT iRegIndex;
   USHORT usRegPrev;
   USHORT usAddrIdx;
   USHORT usAddr;
   USHORT usRegCurr;
   bool bMemory;
   bool bResult;

   // Check for existing registers mapped at this block
   bMemory= ((usAddress >= MBPB_REG_HOLDING_START) && (usAddress + usNRegs <= (MBPB_REG_HOLDING_START+ MBPB_REG_HOLDING_NREGS+ MBPB_REG_HOLDING_ZEROBASED)));
   
   if (bMemory || (EMode == MBPB_REG_WRITE))
   {
      iRegIndex= (INT)(usAddress- MBPB_REG_HOLDING_START- MBPB_REG_HOLDING_ZEROBASED);
      
      // Lock register memory
      if (SSEM_LOCK(SSemLockHolding, SMBPBApp.SDataRW.SReg))
      {
         switch (EMode)
         {
            // Pass current register values to the protocol stack
            case MBPB_REG_READ:
               for (usAddrIdx= 0; usAddrIdx < usNRegs; usAddrIdx++, iRegIndex++)
               {
                  *pucRegBuffer++= (unsigned char)(usRegHoldingBuf[iRegIndex] >> BITS_OF(UCHAR));
                  *pucRegBuffer++= (unsigned char)(usRegHoldingBuf[iRegIndex] & 0xFF);

                  // Lock STrgMsg
                  if (SSEM_LOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg))
                  {                  
                     // Check access match
                     if (SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_INPUT_READ].ucR)
                     {
                        // Check address/value mask match
                        usAddr= usAddress+ usAddrIdx;
                        if (usAddr == (usAddr & SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_HOLDING_READ].usAddrMask))
                        {
                           MBPBRegMsg(&bAddrBase, FALSE, MBPB_TRGTYPE_HOLDING_READ, MBPB_TRGMSG_RW_READ, usAddr, usAddrIdx, usRegHoldingBuf[iRegIndex]);
                        }
                     }
                     else
                     {
                        if (!bMemory)
                        {
                           EStatus= MBPB_ENOREG;
                        }
                     }
                     
                     SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
                  }
               }
               break;

               // Update current register values with new values from the protocol stack
            case MBPB_REG_WRITE:
               for (bResult= TRUE, usAddrIdx= 0; (usAddrIdx < usNRegs) && (bResult); usAddrIdx++, iRegIndex++)
               {
                  usAddr= usAddress+ usAddrIdx;
                  
                  if (bMemory)
                  {
                     // Backup previous register content
                     usRegPrev= usRegHoldingBuf[iRegIndex];
                     
                     // Temporary new register content
                     usRegCurr= *pucRegBuffer++ << BITS_OF(UCHAR);
                     usRegCurr|= *pucRegBuffer++;

                     // Call user validate function to block and/or change the temporary new register content
                     if (SMBPBApp.SDataRO.Validate) bResult=SMBPBApp.SDataRO.Validate(MBPB_REGTYPE_HOLDING, usAddr, &usRegCurr);
                     
                     // Set the new register content
                     if (bResult) usRegHoldingBuf[iRegIndex]= usRegCurr;
                  }
                  else
                  {
                     usRegCurr= *pucRegBuffer++ << BITS_OF(UCHAR);
                     usRegCurr |= *pucRegBuffer++;
                     usRegPrev= usRegCurr;
                  }

                  // Process valid new register content
                  if (bResult)
                  {
                     // Lock STrgMsg
                     if (SSEM_LOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg))
                     {                  
                        // Check access match
                        if (SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_HOLDING_WRITE].ucW)
                        {
                           // Check optional value change
                           if (!MBPB_TRGMSG_CHANGEBITS(SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_HOLDING_WRITE].ucW) || (MBPB_TRGMSG_CHANGEBITS(SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_HOLDING_WRITE].ucW) && ((usRegPrev != usRegCurr) || !bMemory)))
                           {
                              // Check address/value mask match
                              //-usAddr= usAddress+ usAddrIdx;
                              if (usAddr == (usAddr & SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_HOLDING_WRITE].usAddrMask))
                              {
                                 MBPBRegMsg(&bAddrBase, FALSE, MBPB_TRGTYPE_HOLDING_WRITE, MBPB_TRGMSG_RW_WRITE, usAddr, usAddrIdx, usRegCurr);
                              }
                              else
                              {
                                 if (!bMemory)
                                 {
                                    EStatus= MBPB_ENOREG;
                                 }
                              }
                           }
                        }
                        
                        SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
                     }
                  }
                  else
                  {
                     // Deny user rated invalid values
                     EStatus= MBPB_EINVAL;
                  }
               }
               break;
         }
         
         SSEM_UNLOCK(SSemLockHolding, SMBPBApp.SDataRW.SReg);
      }
   }      
   else
   {
      EStatus= MBPB_ENOREG;
   }
       
   return (EStatus);
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBRegCoilsCB
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBRegCoilsCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNCoils, EMBPBRegisterMode EMode)
{
   EMBPBErrorCode EStatus= MBPB_ENOERR;
   USHORT usCoils= usNCoils;
   bool bAddrBase= FALSE;
   
   USHORT usAddrIdx;
   USHORT usByteIdx;
   USHORT usBitIdx;
   USHORT usBitOffset;
   USHORT usAddr;
   UCHAR ucBytePrev;
   UCHAR ucByte;
   UCHAR ucBitCnt;
   bool bBitPrev;
   bool bBit;
   
   bool bMemory;   
   
   // Check for existing registers mapped at this block
   bMemory= ((usAddress >= MBPB_REG_COILS_START) && (usAddress+ usNCoils <= (MBPB_REG_COILS_START+ MBPB_REG_COILS_SIZE+ MBPB_REG_COILS_ZEROBASED)));
   
   if (bMemory || (EMode == MBPB_REG_WRITE))
   {
      usBitOffset= (unsigned short)(usAddress- MBPB_REG_COILS_START- MBPB_REG_COILS_ZEROBASED);
      
      // Lock register memory
      if (SSEM_LOCK(SSemLockCoils, SMBPBApp.SDataRW.SReg))
      {
         switch (EMode)
         {
            // Read current values and pass to protocol stack
            case MBPB_REG_READ:
               for (usByteIdx= 0; usByteIdx < usNCoils; usByteIdx += BITS_OF(UCHAR), usBitOffset += BITS_OF(UCHAR), usCoils -= _umin(usCoils, BITS_OF(UCHAR)))
               {
                  ucBitCnt= (UCHAR) _umin(usCoils, BITS_OF(UCHAR));
                  ucByte= MBPBUtilGetBits(ucRegCoilsBuf, usBitOffset, ucBitCnt);
                  *pucRegBuffer++ = ucByte;
   
                  for (usBitIdx= 0; usBitIdx < ucBitCnt; usBitIdx++)
                  {
                     // Lock STrgMsg
                     if (SSEM_LOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg))
                     {                                       
                        // Check access match
                        if (SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_COILS_READ].ucR)
                        {
                           // Check address/value mask match
                           usAddrIdx= usByteIdx+ usBitIdx;
                           usAddr= usAddress+ usAddrIdx;
                           if (usAddr == (usAddr & SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_COILS_READ].usAddrMask))
                           {
                              MBPBRegMsg(&bAddrBase, TRUE, MBPB_TRGTYPE_COILS_READ, MBPB_TRGMSG_RW_READ, usAddr, usAddrIdx, ((ucByte & ucBitMask[usBitIdx]) != 0));
                           }
                        }
                        else
                        {
                           if (!bMemory)
                           {
                              EStatus= MBPB_ENOREG;
                           }
                        }
                        
                        SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
                     }
                  }
               }
               break;
      
            // Update current register values
            case MBPB_REG_WRITE:
               for (usByteIdx= 0; usByteIdx < usNCoils; usByteIdx += BITS_OF(UCHAR), usBitOffset += BITS_OF(UCHAR), usCoils -= _umin(usCoils, BITS_OF(UCHAR)))
               {
                  ucBitCnt= (UCHAR) _umin(usCoils, BITS_OF(UCHAR));
                  
                  if (bMemory)
                  {
                     ucBytePrev= MBPBUtilGetBits(ucRegCoilsBuf, usBitOffset, ucBitCnt);
                     ucByte= MBPBUtilSetBits(ucRegCoilsBuf, usBitOffset, ucBitCnt, *pucRegBuffer++);
                  }
                  else
                  {
                     ucByte= MBPBUtilSetBits(NULL, usBitOffset, ucBitCnt, *pucRegBuffer++);
                     ucBytePrev= ucByte;
                  }
                  
                  for (usBitIdx= 0; usBitIdx < ucBitCnt; usBitIdx++)
                  {
                     // Lock STrgMsg
                     if (SSEM_LOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg))
                     {
                        // Check access match
                        if (SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_COILS_WRITE].ucW)
                        {
                           bBitPrev= (ucBytePrev & ucBitMask[usBitIdx+ usBitOffset]) != 0;
                           bBit= (ucByte & ucBitMask[usBitIdx+ usBitOffset]) != 0;
                                                                                      
                           // Check optional value change
                           if (!MBPB_TRGMSG_CHANGEBITS(SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_COILS_WRITE].ucW) || (MBPB_TRGMSG_CHANGEBITS(SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_COILS_WRITE].ucW) && ((bBitPrev != bBit) || !bMemory)))
                           {
                              // Check address/value mask match
                              usAddrIdx= usByteIdx+ usBitIdx;
                              usAddr= usAddress+ usAddrIdx;
                              if (usAddr == (usAddr & SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_COILS_WRITE].usAddrMask))
                              {
                                 MBPBRegMsg(&bAddrBase, TRUE, MBPB_TRGTYPE_COILS_WRITE, MBPB_TRGMSG_RW_WRITE, usAddr, usAddrIdx, bBit);
                              }
                              else
                              {
                                 if (!bMemory)
                                 {
                                    EStatus= MBPB_ENOREG;
                                 }
                              }
                           }
                        }
                        
                        SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
                     }
                  }
               }
               break;
         }
                     
         SSEM_UNLOCK(SSemLockCoils, SMBPBApp.SDataRW.SReg);
      }
   }
   else
   {
      EStatus= MBPB_ENOREG;
   }
   
   return (EStatus);
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBRegDiscreteCB
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBRegDiscreteCB(UCHAR *pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
   EMBPBErrorCode EStatus= MBPB_ENOERR;
   SHORT usDiscrete= usNDiscrete;
   bool bAddrBase= FALSE;
   
   USHORT usAddrIdx;
   USHORT usByteIdx;
   USHORT usBitIdx;
   USHORT usBitOffset;
   USHORT usAddr;
   UCHAR ucByte;
   UCHAR ucBitCnt;
   
   // Check for existing registers mapped at this block
   if ((usAddress >= MBPB_REG_DISCRETE_START) && (usAddress+ usNDiscrete <= (MBPB_REG_DISCRETE_START+ MBPB_REG_DISCRETE_SIZE+ MBPB_REG_DISCRETE_ZEROBASED)))
   {
      usBitOffset= (unsigned short)(usAddress- MBPB_REG_DISCRETE_START- MBPB_REG_DISCRETE_ZEROBASED);
      
      // Lock register memory
      if (SSEM_LOCK(SSemLockDiscrete, SMBPBApp.SDataRW.SReg))
      {
         for (usByteIdx= 0; usByteIdx < usNDiscrete; usByteIdx += BITS_OF(UCHAR), usBitOffset += BITS_OF(UCHAR), usDiscrete -= _umin(usDiscrete, BITS_OF(UCHAR)))
         {
            ucBitCnt= (UCHAR) _umin(usDiscrete, BITS_OF(UCHAR));
            ucByte= MBPBUtilGetBits(ucRegDiscBuf, usBitOffset, ucBitCnt);
            *pucRegBuffer++ = ucByte;
   
            for (usBitIdx= 0; usBitIdx < ucBitCnt; usBitIdx++)
            {
               // Lock STrgMsg
               if (SSEM_LOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg))
               {            
                  // Check access match
                  if (SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_DISCRETE_READ].ucR)
                  {
                     // Check address/value mask match
                     usAddrIdx= usByteIdx+ usBitIdx;
                     usAddr= usAddress+ usAddrIdx;
                     if (usAddr == (usAddr & SMBPBApp.SDataRW.STrgMsg.STrg[MBPB_TRGTYPE_DISCRETE_READ].usAddrMask))
                     {
                        MBPBRegMsg(&bAddrBase, TRUE, MBPB_TRGTYPE_DISCRETE_READ, MBPB_TRGMSG_RW_READ, usAddr, usAddrIdx, ((ucByte & ucBitMask[usBitIdx+ usBitOffset]) != 0));
                     }
                  }
                  
                  SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
               }
            }
         }
         
         SSEM_UNLOCK(SSemLockDiscrete, SMBPBApp.SDataRW.SReg);
      }
   }
   else
   {
      EStatus= MBPB_ENOREG;
   }
   
   return (EStatus);
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBRegGeneralReferenceCB
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBRegGeneralReferenceCB(UCHAR *pucRegBuffer, U32 u32Address, USHORT usNRegs, EMBPBRegisterMode EMode)
{
   EMBPBErrorCode EStatus= MBPB_ENOERR;
   ULONG *pulReference= NULL;
   USHORT *pusReference= NULL;

   ULONG ulRegCurr;
   USHORT usRegCurr;
   INT iRegIndex;
   CHAR cWordEven;
   bool bResult;

   // Pointer to system memory space
   pusReference= (USHORT *)u32Address;
   pulReference= (ULONG *)u32Address;
   
   switch (EMode)
   {
      // Pass current reference values to the protocol stack
      case MBPB_REG_READ:
         for (iRegIndex= 0; iRegIndex < usNRegs; iRegIndex++)
         {
            *pucRegBuffer++= (unsigned char)(pusReference[iRegIndex] >> BITS_OF(UCHAR));
            *pucRegBuffer++= (unsigned char)(pusReference[iRegIndex] & 0xFF);
         }
         break;
      
      // Update current reference values with new values from the protocol stack
      case MBPB_REG_WRITE:
         for (bResult= TRUE, iRegIndex= 0, cWordEven= 0; (iRegIndex < usNRegs) && (bResult); cWordEven= (cWordEven+ 1) % 2)
         {
            // Temporary new register content (16 bit)
            usRegCurr= *pucRegBuffer++ << BITS_OF(UCHAR);
            usRegCurr|= *pucRegBuffer++;
            
            // Temporary new register content (32 bit)
            ulRegCurr= (!cWordEven) ? (NULL) : (ulRegCurr);
            ulRegCurr|= ((ULONG)usRegCurr) << (BITS_OF(USHORT) * cWordEven);
            
            // Call user validate function to block and/or change the temporary new register content
            if (SMBPBApp.SDataRO.Validate)
            {
               bResult=SMBPBApp.SDataRO.Validate(MBPB_REGTYPE_GENERAL, u32Address, &usRegCurr);
            }
            else
            {
               // Decline all write request without a existing user validate function
               bResult= FALSE;
            }

            if (bResult)
            {
               // Set the new register content(s)

               // Even word enables combined 32 bit memory access (required for efc hardware buffer)
               if (cWordEven)
               {
                  pulReference[(iRegIndex >> 1)]= ulRegCurr;
                  iRegIndex+= 2; // Counter for 16 bit register(s)
               }
               else
               {
                  // Check for last word
                  if (iRegIndex+ 1 >= usNRegs)
                  {
                     // Odd word forces 16 bit memory access
                     pusReference[iRegIndex++]= usRegCurr;
                  }
               }
            }
            else
            {
               // Deny user rated invalid values
               EStatus= MBPB_EINVAL;
            }
         }
         break;
   }
       
   return (EStatus);
}

