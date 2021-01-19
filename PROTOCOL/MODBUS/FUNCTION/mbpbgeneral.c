//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBGENERAL.C
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-04-25 initial version
// 2013-05-01 fixed definition of MBPB_PDU_REQ_REGCNT_OFF
// 2013-05-01 fixed MBPBFuncReadGeneralReference
// 2013-05-02 fixed MBPBFuncWriteGeneralReference
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
#define MBPB_PDU_FUNC_HEADER_SIZE                  (8)
#define MBPB_PDU_FUNC_READ_SIZE                    (MBPB_PDU_FUNC_HEADER_SIZE)
#define MBPB_PDU_FUNC_WRITE_SIZE                   (MBPB_PDU_FUNC_HEADER_SIZE+ 2)

#define MBPB_PDU_REQ_READ_SIZE                     (MBPB_PDU_REQ_WRITE_SIZE+ 1)
#define MBPB_PDU_REQ_WRITE_SIZE                    (6)

#define MBPB_PDU_REQ_REF_TYPE                      (6)

#define MBPB_PDU_REQ_BYTE_COUNT_OFF                (MBPB_PDU_DATA_OFF)
#define MBPB_PDU_REQ_REF_TYPE_OFF                  (MBPB_PDU_DATA_OFF + 1)
#define MBPB_PDU_REQ_ADDR_OFF                      (MBPB_PDU_DATA_OFF + 2)
#define MBPB_PDU_REQ_REGCNT_OFF                    (MBPB_PDU_DATA_OFF + 6)
#define MBPB_PDU_REQ_WRITE_VALUES_OFF              (MBPB_PDU_DATA_OFF + 8)

//----------------------------------------------------------------------------
// EMBPBException MBPBFuncWriteGeneralReference
//----------------------------------------------------------------------------
EMBPBException MBPBFuncWriteGeneralReference(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
{
   struct _SMBPBTskPara *pSMBPBTskPara= (struct _SMBPBTskPara *) pvParam;
   
   U32               u32RegAddress;
   USHORT            usRegCount;
   USHORT            usRegCntMax;
   UCHAR             ucByteCount;
   UCHAR             ucRefType;
   
   EMBPBException    EStatus= MBPB_EX_NONE;
   EMBPBErrorCode    ERegStatus;
   EMBPBMode         EMode;
   
   // Safely access EMode update temporary local copy
   LOCKED(EMode= pSMBPBTskPara->pSMBPBPara->SPortRW.EMode);
   
   // Determine 'MBPB_PDU_FUNC_WRITE_REGCNT_MAX' for current case
   usRegCntMax= pSMBPBTskPara->pSMBPBApp->SDataRO.SMaxItems[EMode].SGeneral.usWrite;

   if (*usLen >= (MBPB_PDU_FUNC_WRITE_SIZE+ MBPB_PDU_SIZE_MIN))
   {
      ucByteCount= pucFrame[MBPB_PDU_REQ_BYTE_COUNT_OFF];
      
      ucRefType= pucFrame[MBPB_PDU_REQ_REF_TYPE_OFF];      
      if (ucRefType == MBPB_PDU_REQ_REF_TYPE)
      {
         if ((ucByteCount >= MBPB_PDU_REQ_READ_SIZE) && (ucByteCount <= (MBPB_PDU_FUNC_HEADER_SIZE+ (usRegCntMax << 1))))
         {
            u32RegAddress = (U32)(pucFrame[MBPB_PDU_REQ_ADDR_OFF   ] << 24);
            u32RegAddress|= (U32)(pucFrame[MBPB_PDU_REQ_ADDR_OFF+ 1] << 16);
            u32RegAddress|= (U32)(pucFrame[MBPB_PDU_REQ_ADDR_OFF+ 2] << 8 );
            u32RegAddress|= (U32)(pucFrame[MBPB_PDU_REQ_ADDR_OFF+ 3]      );
            
            usRegCount = (USHORT)(pucFrame[MBPB_PDU_REQ_REGCNT_OFF   ] << 8);
            usRegCount|= (USHORT)(pucFrame[MBPB_PDU_REQ_REGCNT_OFF+ 1]     );
            
            // Accept only single request (only one sub-request per request)
            if ((usRegCount >= 1) &&
               (usRegCount <= usRegCntMax) &&
               (ucByteCount == (UCHAR) ((usRegCount << 1)+ (MBPB_PDU_FUNC_HEADER_SIZE- 1))))
            {
               // Make callback to update the register values
               ERegStatus= MBPBRegGeneralReferenceCB(&pucFrame[MBPB_PDU_REQ_WRITE_VALUES_OFF], u32RegAddress, usRegCount, MBPB_REG_WRITE);
         
               // If an error occured convert it into a Modbus exception
               if (ERegStatus != MBPB_ENOERR)
               {
                   EStatus= MBPBError2Exception(ERegStatus);
               }
               else
               {
                   // The response is the full echo of the request.
                   // We reuse the old values in the buffer because they are still valid.
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
      }
      else
      {
         // Can't be a valid request because the reference type is invalid
         EStatus= MBPB_EX_ILLEGAL_DATA_ADDRESS;  
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
// EMBPBException MBPBFuncReadGeneralReference
//----------------------------------------------------------------------------
EMBPBException MBPBFuncReadGeneralReference(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
{
   struct _SMBPBTskPara *pSMBPBTskPara= (struct _SMBPBTskPara *) pvParam;
   
   U32               u32RegAddress;
   USHORT            usRegCount;
   USHORT            usRegCntMax;
   UCHAR             ucByteCount;
   UCHAR             ucRefType;
   UCHAR             *pucFrameCur;
   
   EMBPBException    EStatus= MBPB_EX_NONE;
   EMBPBErrorCode    ERegStatus;
   EMBPBMode         EMode;
   
   // Safely access EMode update temporary local copy
   LOCKED(EMode= pSMBPBTskPara->pSMBPBPara->SPortRW.EMode);
   
   // Determine 'MBPB_PDU_FUNC_READ_REGCNT_MAX' for current case
   usRegCntMax= pSMBPBTskPara->pSMBPBApp->SDataRO.SMaxItems[EMode].SGeneral.usRead;

   // Accept only single request (only one sub-request per request)
   if (*usLen == (MBPB_PDU_FUNC_READ_SIZE+ MBPB_PDU_SIZE_MIN))
   {
      ucByteCount= pucFrame[MBPB_PDU_REQ_BYTE_COUNT_OFF];

      ucRefType= pucFrame[MBPB_PDU_REQ_REF_TYPE_OFF];      
      if (ucRefType == MBPB_PDU_REQ_REF_TYPE)
      {
         if ((ucByteCount >= MBPB_PDU_REQ_READ_SIZE) && (ucByteCount <= (usRegCntMax << 1)))
         {
            u32RegAddress = (U32)(pucFrame[MBPB_PDU_REQ_ADDR_OFF   ] << 24);
            u32RegAddress|= (U32)(pucFrame[MBPB_PDU_REQ_ADDR_OFF+ 1] << 16);
            u32RegAddress|= (U32)(pucFrame[MBPB_PDU_REQ_ADDR_OFF+ 2] << 8 );
            u32RegAddress|= (U32)(pucFrame[MBPB_PDU_REQ_ADDR_OFF+ 3]      );
            
            usRegCount = (USHORT)(pucFrame[MBPB_PDU_REQ_REGCNT_OFF   ] << 8);
            usRegCount|= (USHORT)(pucFrame[MBPB_PDU_REQ_REGCNT_OFF+ 1]     );
            
            // Check if the number of registers to read is valid. If not return Modbus illegal data value exception. 
            if ((usRegCount >= 1) && (usRegCount <= usRegCntMax))
            {
               // Prepare reply buffer size
               
               // Set the current PDU data pointer to the beginning
               pucFrameCur= &pucFrame[MBPB_PDU_FUNC_OFF];
               *usLen= MBPB_PDU_FUNC_OFF;
               
               // First byte contains the function code.
               *pucFrameCur++= MBPB_CODE_READ_GENERAL_REFERENCE;
               *usLen += 1;
               
               // Second byte in the response contain the number of bytes of the whole request (sum of all sub-requests)
               *pucFrameCur++= (UCHAR) (usRegCount* 2)+ 2;
               *usLen += 1;
               
               // Third byte in the response contain the number of bytes of the sub-request only
               *pucFrameCur++= (UCHAR) (usRegCount* 2)+ 1;
               *usLen += 1;
         
               // Make callback to continue filling the buffer (overwrite the received request)
               ERegStatus= MBPBRegGeneralReferenceCB(pucFrameCur, u32RegAddress, usRegCount, MBPB_REG_READ);

               // If an error occured convert it into a Modbus exception
               if (ERegStatus != MBPB_ENOERR)
               {
                  EStatus= MBPBError2Exception(ERegStatus);
               }
               // Extend buffer for fitting reply data
               else
               {
                   *usLen += usRegCount* 2;
               }
            }
            else
            {
               // Can't be a valid request because the length is incorrect
               EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
            }
         }
         else
         {
            // Can't be a valid request because the length is incorrect
            EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
         }
      }
      else
      {
         // Can't be a valid request because the reference type is invalid
         EStatus= MBPB_EX_ILLEGAL_DATA_ADDRESS;      
      }
   }
   else
   {
      // Can't be a valid request because the length is incorrect
      EStatus= MBPB_EX_ILLEGAL_DATA_VALUE;
   }
   
   return (EStatus);
}

