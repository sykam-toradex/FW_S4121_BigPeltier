//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBINPUT.C
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-27 initial version
// 2009-08-27 added void pointer to code function calls
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2013-01-24 changed to use correct mode reg max for MBPBFuncReadCoils
// 2013-01-24 removed obsolete (and wrong for ASCII) define MBPB_PDU_FUNC_READ_COILCNT_MAX
// 2013-01-30 fixed determination of usRegCntMax in MBPBFuncReadCoils
// 2013-04-11 added function headers with return data type comment
// 2013-04-24 removed obsolete (and wrong for ASCII) define MBPB_PDU_FUNC_WRITE_MUL_COILCNT_MAX
// 2013-04-24 changed to use correct mode reg max for MBPBFuncWriteMultipleCoils
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
#define MBPB_PDU_FUNC_READ_ADDR_OFF           (MBPB_PDU_DATA_OFF)
#define MBPB_PDU_FUNC_READ_COILCNT_OFF        (MBPB_PDU_DATA_OFF+ 2)
#define MBPB_PDU_FUNC_READ_SIZE               (4)

#define MBPB_PDU_FUNC_WRITE_ADDR_OFF          (MBPB_PDU_DATA_OFF)
#define MBPB_PDU_FUNC_WRITE_VALUE_OFF         (MBPB_PDU_DATA_OFF+ 2)
#define MBPB_PDU_FUNC_WRITE_SIZE              (4)

#define MBPB_PDU_FUNC_WRITE_MUL_ADDR_OFF      (MBPB_PDU_DATA_OFF)
#define MBPB_PDU_FUNC_WRITE_MUL_COILCNT_OFF   (MBPB_PDU_DATA_OFF+ 2)
#define MBPB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF   (MBPB_PDU_DATA_OFF+ 4)
#define MBPB_PDU_FUNC_WRITE_MUL_VALUES_OFF    (MBPB_PDU_DATA_OFF+ 5)
#define MBPB_PDU_FUNC_WRITE_MUL_SIZE_MIN      (5)

//----------------------------------------------------------------------------
// EMBPBException MBPBFuncReadCoils
//----------------------------------------------------------------------------
EMBPBException MBPBFuncReadCoils(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
{
   struct _SMBPBTskPara *pSMBPBTskPara= (struct _SMBPBTskPara *) pvParam;
   
   USHORT            usRegAddress;
   USHORT            usCoilCount;
   UCHAR             ucNBytes;
   USHORT            usRegCntMax;
   UCHAR             *pucFrameCur;
   
   EMBPBException    EStatus= MBPB_EX_NONE;
   EMBPBErrorCode    ERegStatus;
   EMBPBMode         EMode;
   
   // Safely access EMode update temporary local copy
   LOCKED(EMode= pSMBPBTskPara->pSMBPBPara->SPortRW.EMode);
   
   // Determine 'MMBPB_PDU_FUNC_READ_COILCNT_MAX' for current case
   usRegCntMax= pSMBPBTskPara->pSMBPBApp->SDataRO.SMaxItems[EMode].SCoils.usRead;

   if (*usLen == (MBPB_PDU_FUNC_READ_SIZE+ MBPB_PDU_SIZE_MIN))
   {
     usRegAddress= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_ADDR_OFF] << 8);
     usRegAddress |= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_ADDR_OFF+ 1]);
     usRegAddress++;
   
     usCoilCount= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_COILCNT_OFF] << 8);
     usCoilCount |= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_COILCNT_OFF+ 1]);
   
     // Check if the number of registers to read is valid. If not return Modbus illegal data value exception
     if ((usCoilCount >= 1) && (usCoilCount <= usRegCntMax))
     {
         // Set the current PDU data pointer to the beginning
         pucFrameCur= &pucFrame[MBPB_PDU_FUNC_OFF];
         *usLen= MBPB_PDU_FUNC_OFF;
         
         // First byte contains the function code
         *pucFrameCur++= MBPB_CODE_READ_COILS;
         *usLen += 1;
         
         // Test if the quantity of coils is a multiple of 8. If not last byte is only partially field with unused coils set to zero
         if ((usCoilCount & 0x0007) != 0)
         {
            ucNBytes= (UCHAR)(usCoilCount/ 8+ 1);
         }
         else
         {
            ucNBytes= (UCHAR)(usCoilCount/ 8);
         }
         
         *pucFrameCur++= ucNBytes;
         *usLen += 1;
         
         ERegStatus= MBPBRegCoilsCB(pucFrameCur, usRegAddress, usCoilCount, MBPB_REG_READ);
         
         // If an error occured convert it into a Modbus exception
         if (ERegStatus != MBPB_ENOERR)
         {
             EStatus= MBPBError2Exception(ERegStatus);
         }
         else
         {
            // The response contains the function code, the starting address and the quantity of registers
            // We reuse the old values in the buffer because they are still valid
            *usLen += ucNBytes;;
         }
     }
     else
     {
         EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
     }
   }
   else
   {
      // Can't be a valid read coil register request because the length is incorrect
      EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
   }
   
   return (EStatus);
}

//----------------------------------------------------------------------------
// EMBPBException MBPBFuncWriteCoil
//----------------------------------------------------------------------------
EMBPBException MBPBFuncWriteCoil(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
{
   USHORT            usRegAddress;
   UCHAR             ucBuf[2];
   
   EMBPBException    EStatus= MBPB_EX_NONE;
   EMBPBErrorCode    ERegStatus;
   
   if (*usLen == (MBPB_PDU_FUNC_WRITE_SIZE+ MBPB_PDU_SIZE_MIN))
   {
      usRegAddress= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_ADDR_OFF] << 8);
      usRegAddress |= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_ADDR_OFF+ 1]);
      usRegAddress++;
      
      if ((pucFrame[MBPB_PDU_FUNC_WRITE_VALUE_OFF+ 1] == 0x00) &&
         ((pucFrame[MBPB_PDU_FUNC_WRITE_VALUE_OFF] == 0xFF) ||
           (pucFrame[MBPB_PDU_FUNC_WRITE_VALUE_OFF] == 0x00)))
      {
         ucBuf[1]= 0;
         if (pucFrame[MBPB_PDU_FUNC_WRITE_VALUE_OFF] == 0xFF)
         {
            ucBuf[0]= 1;
         }
         else
         {
            ucBuf[0]= 0;
         }
         
         ERegStatus= MBPBRegCoilsCB(&ucBuf[0], usRegAddress, 1, MBPB_REG_WRITE);
         
         // If an error occured convert it into a Modbus exception
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
   else
   {
      // Can't be a valid write coil register request because the length is incorrect
      EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
   }
   
   return (EStatus);
}

//----------------------------------------------------------------------------
// EMBPBException MBPBFuncWriteMultipleCoils
//----------------------------------------------------------------------------
EMBPBException MBPBFuncWriteMultipleCoils(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
{
   struct _SMBPBTskPara *pSMBPBTskPara= (struct _SMBPBTskPara *) pvParam;
   
   USHORT            usRegAddress;
   USHORT            usCoilCnt;
   USHORT            usCoilCntMax;
   UCHAR             ucByteCount;
   UCHAR             ucByteCountVerify;
   
   EMBPBException    EStatus= MBPB_EX_NONE;
   EMBPBErrorCode    ERegStatus;
   EMBPBMode         EMode;
   
   // Safely access EMode update temporary local copy
   LOCKED(EMode= pSMBPBTskPara->pSMBPBPara->SPortRW.EMode);
   
   // Determine 'MMBPB_PDU_FUNC_READ_COILCNT_MAX' for current case
   usCoilCntMax= pSMBPBTskPara->pSMBPBApp->SDataRO.SMaxItems[EMode].SCoils.usWrite;
   
   if (*usLen > (MBPB_PDU_FUNC_WRITE_SIZE+ MBPB_PDU_SIZE_MIN))
   {
      usRegAddress= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_MUL_ADDR_OFF] << 8);
      usRegAddress |= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_MUL_ADDR_OFF+ 1]);
      usRegAddress++;
      
      usCoilCnt= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_MUL_COILCNT_OFF] << 8);
      usCoilCnt |= (USHORT)(pucFrame[MBPB_PDU_FUNC_WRITE_MUL_COILCNT_OFF+ 1]);
      
      ucByteCount= pucFrame[MBPB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF];
      
      // Compute the number of expected bytes in the request
      if ((usCoilCnt & 0x0007) != 0)
      {
         ucByteCountVerify= (UCHAR)(usCoilCnt/ 8+ 1);
      }
      else
      {
         ucByteCountVerify= (UCHAR)(usCoilCnt/ 8);
      }
      
      if ((usCoilCnt >= 1) &&
         (usCoilCnt <= usCoilCntMax) &&
         (ucByteCountVerify == ucByteCount))
      {
         ERegStatus= MBPBRegCoilsCB(&pucFrame[MBPB_PDU_FUNC_WRITE_MUL_VALUES_OFF], usRegAddress, usCoilCnt, MBPB_REG_WRITE);
         
         // If an error occured convert it into a Modbus exception
         if (ERegStatus != MBPB_ENOERR)
         {
            EStatus= MBPBError2Exception(ERegStatus);
         }
         else
         {
            // The response contains the function code, the starting address and the quantity of registers
            // We reuse the old values in the buffer because they are still valid
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
      // Can't be a valid write coil register request because the length is incorrect
      EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
   }
   
   return (EStatus);
}

