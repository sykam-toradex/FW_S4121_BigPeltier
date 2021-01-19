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
// 2013-01-24 changed to use correct mode reg max for MBPBFuncReadInputRegister
// 2013-01-24 removed obsolete (and wrong for ASCII) define MBPB_PDU_FUNC_READ_REGCNT_MAX
// 2013-04-11 added function headers with return data type comment
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
#define MBPB_PDU_FUNC_READ_REGCNT_OFF         (MBPB_PDU_DATA_OFF+ 2)
#define MBPB_PDU_FUNC_READ_SIZE               (4)

#define MBPB_PDU_FUNC_READ_RSP_BYTECNT_OFF    (MBPB_PDU_DATA_OFF)

//----------------------------------------------------------------------------
// EMBPBException MBPBFuncReadInputRegister
//----------------------------------------------------------------------------
EMBPBException MBPBFuncReadInputRegister(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
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
   usRegCntMax= pSMBPBTskPara->pSMBPBApp->SDataRO.SMaxItems[EMode].SInput.usRead;
   
   if (*usLen == (MBPB_PDU_FUNC_READ_SIZE+ MBPB_PDU_SIZE_MIN))
   {
      usRegAddress= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_ADDR_OFF] << 8);
      usRegAddress |= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_ADDR_OFF+ 1]);
      usRegAddress++;
      
      usRegCount= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_REGCNT_OFF] << 8);
      usRegCount |= (USHORT)(pucFrame[MBPB_PDU_FUNC_READ_REGCNT_OFF+ 1]);
      
      // Check if the number of registers to read is valid. If not return Modbus illegal data value exception
      if ((usRegCount >= 1) && (usRegCount <= usRegCntMax))
      {
         // Set the current PDU data pointer to the beginning
         pucFrameCur= &pucFrame[MBPB_PDU_FUNC_OFF];
         *usLen= MBPB_PDU_FUNC_OFF;
         
         // First byte contains the function code
         *pucFrameCur++= MBPB_CODE_READ_INPUT_REGISTER;
         *usLen += 1;
         
         // Second byte in the response contain the number of bytes
         *pucFrameCur++= (UCHAR)(usRegCount* 2);
         *usLen += 1;
         
         ERegStatus= MBPBRegInputCB(pucFrameCur, usRegAddress, usRegCount);
         
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
      // Can't be a valid read input register request because the length is incorrect
      EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
   }
   
   return (EStatus);
}

