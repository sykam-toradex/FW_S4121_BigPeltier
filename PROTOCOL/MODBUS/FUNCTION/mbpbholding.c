//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBHOLDING.C
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-20 initial version
// 2009-08-27 added void pointer to code function calls
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2013-01-24 use defines for req limit chk of MBPBFuncReadWriteMultipleHoldingRegister
// 2013-01-24 changed to use correct mode reg max for MBPBFuncReadHoldingRegister
// 2013-01-24 changed to use correct mode reg max for MBPBFuncReadWriteMultipleHoldingRegister
// 2013-01-24 removed obsolete (and wrong for ASCII) define MBPB_PDU_FUNC_READ_REGCNT_MAX
// 2013-04-11 added function headers with return data type comment
// 2013-04-24 removed obsolete (and wrong for ASCII) define MBPB_PDU_FUNC_WRITE_MUL_REGCNT_MAX
// 2013-04-24 changed to use correct mode reg max for MBPBFuncWriteMultipleHoldingRegister
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include "..\..\..\rtl\support.h"

#include "..\mbpbcfg.h"
#include "..\mbpbframe.h"
#include "..\mbpbdata.h"
#include "..\mbpbcode.h"
#include "..\mbpbregcb.h"
#include "..\mbpbutils.h"

// Defines
#define MBPB_PDU_FUNC_READ_ADDR_OFF                (MBPB_PDU_DATA_OFF + 0)
#define MBPB_PDU_FUNC_READ_REGCNT_OFF              (MBPB_PDU_DATA_OFF + 2)
#define MBPB_PDU_FUNC_READ_SIZE                    (4)

#define MBPB_PDU_FUNC_WRITE_ADDR_OFF               (MBPB_PDU_DATA_OFF + 0)
#define MBPB_PDU_FUNC_WRITE_VALUE_OFF              (MBPB_PDU_DATA_OFF + 2)
#define MBPB_PDU_FUNC_WRITE_SIZE                   (4)

#define MBPB_PDU_FUNC_WRITE_MUL_ADDR_OFF           (MBPB_PDU_DATA_OFF + 0)
#define MBPB_PDU_FUNC_WRITE_MUL_REGCNT_OFF         (MBPB_PDU_DATA_OFF + 2)
#define MBPB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF        (MBPB_PDU_DATA_OFF + 4)
#define MBPB_PDU_FUNC_WRITE_MUL_VALUES_OFF         (MBPB_PDU_DATA_OFF + 5)
#define MBPB_PDU_FUNC_WRITE_MUL_SIZE_MIN           (5)

#define MBPB_PDU_FUNC_READWRITE_READ_ADDR_OFF      (MBPB_PDU_DATA_OFF + 0)
#define MBPB_PDU_FUNC_READWRITE_READ_REGCNT_OFF    (MBPB_PDU_DATA_OFF + 2)
#define MBPB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF     (MBPB_PDU_DATA_OFF + 4)
#define MBPB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF   (MBPB_PDU_DATA_OFF + 6)
#define MBPB_PDU_FUNC_READWRITE_BYTECNT_OFF        (MBPB_PDU_DATA_OFF + 8)
#define MBPB_PDU_FUNC_READWRITE_WRITE_VALUES_OFF   (MBPB_PDU_DATA_OFF + 9)
#define MBPB_PDU_FUNC_READWRITE_SIZE_MIN           (9)

//----------------------------------------------------------------------------
// EMBPBException MBPBFuncWriteHoldingRegister
//----------------------------------------------------------------------------
EMBPBException MBPBFuncWriteHoldingRegister(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
{ 
   USHORT            usRegAddress;
   EMBPBException    EStatus= MBPB_EX_NONE;
   EMBPBErrorCode    ERegStatus;

   if (*usLen == (MBPB_PDU_FUNC_WRITE_SIZE + MBPB_PDU_SIZE_MIN))
   {
      usRegAddress= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_ADDR_OFF] << 8);
      usRegAddress |= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_ADDR_OFF + 1]);
      usRegAddress++;
      
      // Make callback to update the value
      ERegStatus= MBPBRegHoldingCB(&pucFrame[MBPB_PDU_FUNC_WRITE_VALUE_OFF], usRegAddress, 1, MBPB_REG_WRITE);
      
      // If an error occured convert it into a Modbus exception
      if (ERegStatus != MBPB_ENOERR)
      {
         EStatus= MBPBError2Exception(ERegStatus);
      }
   }
   else
   {
      // Can't be a valid request because the length is incorrect
      EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
   }
   
   return (EStatus);
}

//----------------------------------------------------------------------------
// EMBPBException MBPBFuncWriteMultipleHoldingRegister
//----------------------------------------------------------------------------
EMBPBException MBPBFuncWriteMultipleHoldingRegister(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
{
   struct _SMBPBTskPara *pSMBPBTskPara= (struct _SMBPBTskPara *) pvParam;
   
   USHORT            usRegAddress;
   USHORT            usRegCount;
   USHORT            usRegCntMax;
   UCHAR             ucRegByteCount;
   
   EMBPBException    EStatus= MBPB_EX_NONE;
   EMBPBErrorCode    ERegStatus;
   EMBPBMode         EMode;
   
   // Safely access EMode update temporary local copy
   LOCKED(EMode= pSMBPBTskPara->pSMBPBPara->SPortRW.EMode);
   
   // Determine 'MBPB_PDU_FUNC_WRITE_MUL_REGCNT_MAX' for current case
   usRegCntMax= pSMBPBTskPara->pSMBPBApp->SDataRO.SMaxItems[EMode].SHolding.usWrite;

   if (*usLen >= (MBPB_PDU_FUNC_WRITE_MUL_SIZE_MIN + MBPB_PDU_SIZE_MIN))
   {
      usRegAddress= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_MUL_ADDR_OFF] << 8);
      usRegAddress |= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_MUL_ADDR_OFF + 1]);
      usRegAddress++;
      
      usRegCount= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_MUL_REGCNT_OFF] << 8);
      usRegCount |= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_MUL_REGCNT_OFF + 1]);
      
      ucRegByteCount= pucFrame[MBPB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF];
      
      if ((usRegCount >= 1) &&
         (usRegCount <= usRegCntMax) &&
         (ucRegByteCount == (UCHAR) (2* usRegCount)))
      {
         // Make callback to update the register values
         ERegStatus= MBPBRegHoldingCB(&pucFrame[MBPB_PDU_FUNC_WRITE_MUL_VALUES_OFF], usRegAddress, usRegCount, MBPB_REG_WRITE);
         
         // If an error occured convert it into a Modbus exception
         if (ERegStatus != MBPB_ENOERR)
         {
             EStatus= MBPBError2Exception(ERegStatus);
         }
         else
         {
             // The response contains the function code, the starting address and the quantity of registers.
             // We reuse the old values in the buffer because they are still valid.
             *usLen= MBPB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
         }
      }
      else
      {
         EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
      }
   }
   else
   {
      // Can't be a valid request because the length is incorrect
      EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
   }

   return (EStatus);
}

//----------------------------------------------------------------------------
// EMBPBException MBPBFuncReadHoldingRegister
//----------------------------------------------------------------------------
EMBPBException MBPBFuncReadHoldingRegister(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
{
   struct _SMBPBTskPara *pSMBPBTskPara= (struct _SMBPBTskPara *) pvParam;
   
   USHORT            usRegAddress;
   USHORT            usRegCount;
   USHORT            usRegCntMax;
   UCHAR             *pucFrameCur;
   
   EMBPBException    EStatus= MBPB_EX_NONE;
   EMBPBErrorCode    ERegStatus; 
   EMBPBMode         EMode;
   
   // Safely access EMode update temporary local copy
   LOCKED(EMode= pSMBPBTskPara->pSMBPBPara->SPortRW.EMode);
   
   // Determine 'MBPB_PDU_FUNC_READ_REGCNT_MAX' for current case
   usRegCntMax= pSMBPBTskPara->pSMBPBApp->SDataRO.SMaxItems[EMode].SHolding.usRead;
   
   if (*usLen == (MBPB_PDU_FUNC_READ_SIZE + MBPB_PDU_SIZE_MIN))
   {
      usRegAddress= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_ADDR_OFF] << 8);
      usRegAddress |= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_ADDR_OFF + 1]);
      usRegAddress++;
      
      usRegCount= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_REGCNT_OFF] << 8);
      usRegCount= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_REGCNT_OFF + 1]);
      
      // Check if the number of registers to read is valid. If not return Modbus illegal data value exception. 
      if ((usRegCount >= 1) && (usRegCount <= usRegCntMax))
      {
         // Set the current PDU data pointer to the beginning
         pucFrameCur= &pucFrame[MBPB_PDU_FUNC_OFF];
         *usLen= MBPB_PDU_FUNC_OFF;
         
         // First byte contains the function code.
         *pucFrameCur++= MBPB_CODE_READ_HOLDING_REGISTER;
         *usLen += 1;
         
         // Second byte in the response contain the number of bytes
         *pucFrameCur++= (UCHAR) (usRegCount* 2);
         *usLen += 1;
         
         // Make callback to fill the buffer
         ERegStatus= MBPBRegHoldingCB(pucFrameCur, usRegAddress, usRegCount, MBPB_REG_READ);
         
         // If an error occured convert it into a Modbus exception
         if (ERegStatus != MBPB_ENOERR)
         {
             EStatus= MBPBError2Exception(ERegStatus);
         }
         else
         {
             *usLen += usRegCount* 2;
         }
      }
      else
      {
         EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
      }
   }
   else
   {
      // Can't be a valid request because the length is incorrect
      EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
   }

   return (EStatus);
}

//----------------------------------------------------------------------------
// EMBPBException MBPBFuncReadWriteMultipleHoldingRegister
//----------------------------------------------------------------------------
EMBPBException MBPBFuncReadWriteMultipleHoldingRegister(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
{
   struct _SMBPBTskPara *pSMBPBTskPara= (struct _SMBPBTskPara *) pvParam;
   
   USHORT            usRegReadAddress;
   USHORT            usRegReadCount;
   USHORT            usRegWriteAddress;
   USHORT            usRegWriteCount;
   USHORT            usRegCntMaxRead;
   USHORT            usRegCntMaxWrite;
   UCHAR             ucRegWriteByteCount;
   UCHAR             *pucFrameCur;
   
   EMBPBException    EStatus= MBPB_EX_NONE;
   EMBPBErrorCode    ERegStatus;
   EMBPBMode         EMode;
   
   // Safely access EMode update temporary local copy
   LOCKED(EMode= pSMBPBTskPara->pSMBPBPara->SPortRW.EMode);
   
   // Determine 'MBPB_PDU_FUNC_READ/WRITE_REGCNT_MAX' for current case
   usRegCntMaxRead= pSMBPBTskPara->pSMBPBApp->SDataRO.SMaxItems[EMode].SHolding.usReadCombo;
   usRegCntMaxWrite= pSMBPBTskPara->pSMBPBApp->SDataRO.SMaxItems[EMode].SHolding.usWriteCombo;
   
   if (*usLen >= (MBPB_PDU_FUNC_READWRITE_SIZE_MIN + MBPB_PDU_SIZE_MIN))
   {
      usRegReadAddress= (USHORT)(pucFrame[MBPB_PDU_FUNC_READWRITE_READ_ADDR_OFF] << 8U);
      usRegReadAddress |= (USHORT)(pucFrame[MBPB_PDU_FUNC_READWRITE_READ_ADDR_OFF + 1]);
      usRegReadAddress++;
      
      usRegReadCount= (USHORT)(pucFrame[MBPB_PDU_FUNC_READWRITE_READ_REGCNT_OFF] << 8U);
      usRegReadCount |= (USHORT)(pucFrame[MBPB_PDU_FUNC_READWRITE_READ_REGCNT_OFF + 1]);
      
      usRegWriteAddress= (USHORT)(pucFrame[MBPB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF] << 8U);
      usRegWriteAddress |= (USHORT)(pucFrame[MBPB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF + 1]);
      usRegWriteAddress++;
      
      usRegWriteCount= (USHORT)(pucFrame[MBPB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF] << 8U);
      usRegWriteCount |= (USHORT)(pucFrame[MBPB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF + 1]);
      
      ucRegWriteByteCount= pucFrame[MBPB_PDU_FUNC_READWRITE_BYTECNT_OFF];
      
      if ((usRegReadCount >= 1) && (usRegReadCount <= usRegCntMaxRead) &&
         (usRegWriteCount >= 1) && (usRegWriteCount <= usRegCntMaxWrite) &&
         ((2* usRegWriteCount) == ucRegWriteByteCount))
      {
         // Make callback to update the register values
         ERegStatus= MBPBRegHoldingCB(&pucFrame[MBPB_PDU_FUNC_READWRITE_WRITE_VALUES_OFF], usRegWriteAddress, usRegWriteCount, MBPB_REG_WRITE);
         
         if (ERegStatus == MBPB_ENOERR)
         {
            // Set the current PDU data pointer to the beginning
            pucFrameCur= &pucFrame[MBPB_PDU_FUNC_OFF];
            *usLen= MBPB_PDU_FUNC_OFF;
            
            // First byte contains the function code
            *pucFrameCur++= MBPB_CODE_READWRITE_MULTIPLE_REGISTERS;
            *usLen += 1;
            
            // Second byte in the response contain the number of bytes
            *pucFrameCur++= (UCHAR) (usRegReadCount* 2);
            *usLen += 1;
            
            // Make the read callback
            ERegStatus= MBPBRegHoldingCB(pucFrameCur, usRegReadAddress, usRegReadCount, MBPB_REG_READ);
            
            if (ERegStatus == MBPB_ENOERR)
            {
               *usLen += 2* usRegReadCount;
            }
         }
         
         if (ERegStatus != MBPB_ENOERR)
         {
            EStatus= MBPBError2Exception(ERegStatus);
         }
      }
      else
      {
         EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
      }
   }
   
   return (EStatus);
}

