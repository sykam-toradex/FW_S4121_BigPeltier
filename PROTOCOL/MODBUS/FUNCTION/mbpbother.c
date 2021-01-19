//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBOTHER.C
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-27 initial version
// 2009-08-27 added void pointer to code function calls
// 2010-04-30 changed MBPBFuncReportSlaveID to access SProduct at RW by sema
// 2010-06-30 added byte count after function code in MBPBFuncReportSlaveID
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-12-16 modified to support 32 bit device class mask and var desc length
// 2013-04-11 added function headers with return data type comment
// 2013-04-25 fixed MBPBFuncReportSlaveID for changed pvParam pointer
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include "..\..\..\rtl\support.h"

#include "..\mbpbcfg.h"
#include "..\mbpbframe.h"
#include "..\mbpbdata.h"

//----------------------------------------------------------------------------
// EMBPBException MBPBFuncReportSlaveID
//----------------------------------------------------------------------------
EMBPBException MBPBFuncReportSlaveID(UCHAR *pucFrame, USHORT *usLen, void *pvParam)
{
   struct _SMBPBTskPara *pSMBPBTskPara= (struct _SMBPBTskPara *) pvParam;
   
	UCHAR ucSrc;
	UCHAR ucDst;
   UCHAR ucShift;
   ULONG ulMask;
	
	ucDst= MBPB_PDU_DATA_OFF;
	
   if (SSEM_LOCK(SSemLock, pSMBPBTskPara->pSMBPBApp->SDataRW))
   {
      // Determine data length: standard class+ state+ description+ custom class
      *(pucFrame+ ucDst++)= (1+ 1+ pSMBPBTskPara->pSMBPBApp->SDataRW.SProduct.ucDeviceLen+ sizeof(pSMBPBTskPara->pSMBPBApp->SDataRW.SProduct.ulClass));
      
      // Set standard modbus class to length(!) of custom class appended after description
   	*(pucFrame+ ucDst++)= sizeof(pSMBPBTskPara->pSMBPBApp->SDataRW.SProduct.ulClass);
      
      // Determine the device state
      *(pucFrame+ ucDst++)= (UCHAR) pSMBPBTskPara->pSMBPBApp->SDataRW.ERunState;
     
      // Set the description (variable length!)   	
   	for (ucSrc= 0; ucSrc < pSMBPBTskPara->pSMBPBApp->SDataRW.SProduct.ucDeviceLen; ucSrc++)
   	{
   		*(pucFrame+ ucDst++)= pSMBPBTskPara->pSMBPBApp->SDataRW.SProduct.ucDevice[ucSrc];
   	}

      // Set the custom class mask
      for (ulMask= BITS_ULONGHH, ucShift= 24, ucSrc= 0; ucSrc < sizeof(pSMBPBTskPara->pSMBPBApp->SDataRW.SProduct.ulClass); ucSrc++, ulMask >>= BITS_OF(UCHAR), ucShift-= 8)
      {
         *(pucFrame+ ucDst++)= (UCHAR) ((pSMBPBTskPara->pSMBPBApp->SDataRW.SProduct.ulClass & ulMask) >> ucShift);
      }
     
      SSEM_UNLOCK(SSemLock, pSMBPBTskPara->pSMBPBApp->SDataRW);
	}
   
   // Determine telegram length: header+ count+ class+ state+ description+ custom class
	*usLen= (MBPB_PDU_DATA_OFF+ 1+ 1+ 1+ pSMBPBTskPara->pSMBPBApp->SDataRW.SProduct.ucDeviceLen+ sizeof(pSMBPBTskPara->pSMBPBApp->SDataRW.SProduct.ulClass));

	return (MBPB_EX_NONE);
}

