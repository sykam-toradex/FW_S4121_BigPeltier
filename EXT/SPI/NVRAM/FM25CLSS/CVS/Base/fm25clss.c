//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/NVRAM/FM25CLSS/FM25CLSS.C
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-04-07 initial version
// 2010-04-08 Fm25clssRDSR, Fm25clssWRSR, Fm25clssRead and Fm25clssRead
// 2010-04-09 added Fm25clssCtrl function
// 2010-04-09 added FM25CLSS_TAG_CTRL, FM25CLSS_TAG_SR and FM25CLSS_TAG_MEM
// 2010-04-09 splitted Fm25clssRead into Fm25clssRead and Fm25clssReadSafe
// 2010-04-09 changed Fm25clssWRSR style
// 2010-07-28 added code validation to Fm25clssCtrl (suggestion of rradtke)
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-09 optimized for speed by avoiding unnecessary semaphore timeouts
// 2010-09-09 changed Fm25clssReadSafe to use "no-source" feature for dummy tx
// 2010-10-06 added auto-split transfer to Fm25clssReadSafe & Fm25clssWrite
// 2010-10-20 fixed compare trigger of Fm25clssReadSafe (hint by rr)
// 2010-10-20 fixed compare function of Fm25clssReadSafe (hint by rr)
// 2010-10-22 optimized rx data processing in Fm25clssReadSafe/Write
// 2011-01-27 fixed possible deadlock of Fm25clssWrite in case of SpiTx failed
// 2011-01-27 fixed possible deadlock of Fm25clssReadSafe in case of SpiTx failed
// 2011-02-03 added semaphore SemStart to lock the busy device in all functions
// 2011-03-09 added reschedule call after os_sem_take of SemMsgNew in Fm25clssCtrl
// 2011-03-09 added reschedule call after os_sem_take of SemMsgNew in Fm25clssRDSR
// 2011-03-09 added reschedule call after os_sem_take of SemMsgNew in Fm25clssWRSR
// 2011-03-09 fixed pending for next incoming job data option in Fm25clssWRSR
// 2011-03-09 removed checking of AT91C_SPI_TXBUFE messages in Fm25clssWRSR
// 2011-03-09 added reschedule call after os_sem_take of SemMsgNew in Fm25clssReadSafe
// 2011-03-09 removed checking of AT91C_SPI_TXBUFE messages in Fm25clssReadSafe
// 2011-03-09 added reschedule call after os_sem_take of SemMsgNew in Fm25clssWrite
// 2011-03-09 removed checking of AT91C_SPI_TXBUFE messages in Fm25clssWrite
// 2011-04-07 added checking of usCount to Fm25clssReadSafe and Fm25clssWrite
// 2011-05-04 changed all functions here to use SpiChannelNext function
// 2013-04-11 fixed function header's return data type comments
// 2013-08-22 changed timeout from milliseconds to ticks
// 2013-08-22 changed RESCHEDULE (ms) to RESCHEDULE_TICKS (ticks)
// 2013-08-29 added checking of availability of required transfer block size
// 2013-08-29 changed to _umax instrinsics suitable for used USHORTs
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\..\..\cpu\at91\spi\spi.h"
#include "..\..\..\..\cpu\at91\spi\spiext.h"

#include "fm25clss.h"
#include "fm25clssdata.h"
#include "fm25clssdev.h"
#include "fm25clssext.h"

// Number of units located inside fm25clss.h
struct _SFm25clssRW SFm25clssRW[FM25CLSS_UNIT_MAX];

//----------------------------------------------------------------------------
// UCHAR Fm25clssInit
//----------------------------------------------------------------------------
UCHAR Fm25clssInit(UCHAR ucUnit, OS_ID oiMsgNewSem)
{
	UCHAR ucResult= 0;

	if (ucUnit < FM25CLSS_UNIT_MAX)
	{
      if (SPI_PDCMAX > _umax(FM25CLSS_SIZE_HEADERSR, (FM25CLSS_SIZE_HEADERMEM+ FM25CLSS_SPI_FIX)))
      {
         if (SFm25clssRO[ucUnit].u16Timeout)
         {
      		SFm25clssRW[ucUnit].oiSemMsgNew= oiMsgNewSem;
      
            // Prepare OS tools
            os_sem_init(SFm25clssRW[ucUnit].SemStart, 1);
      
      		ucResult= (SFm25clssRW[ucUnit].oiSemMsgNew != 0);
      	}
      }
   }
   	
	return (ucResult);
}

//----------------------------------------------------------------------------
// USHORT Fm25clssCtrl
//----------------------------------------------------------------------------
USHORT Fm25clssCtrl(UCHAR ucUnit, UCHAR ucCode, U16 u16Timeout)
{
	USHORT usResult= 0;
 
   USHORT usByteCnt;

	if (ucUnit < FM25CLSS_UNIT_MAX)
   {
		if ((ucCode == FM25CLSS_CODE_WRDI) || (ucCode == FM25CLSS_CODE_WREN))
		{
	      if (SFm25clssRW[ucUnit].oiSemMsgNew)
	      {
            // Lock device
            if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].SemStart, u16Timeout)))
            {
   	         usByteCnt= 1;
   	         usResult= SpiTx(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, &ucCode, usByteCnt, u16Timeout);
   	         if (usResult >= usByteCnt)
   	         {
   	            // Semaphore is given only once regardless of the number of messages
   	            if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout)))
   	            {
                     // Force scheduler to run other tasks yet
                     RESCHEDULE_TICKS(0);

                     // Wipe dummy data
                     usByteCnt= SpiRx(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, NULL, SPI_BUFFERCNTRX, SFm25clssRO[ucUnit].u16Timeout);
                     if (usByteCnt > 0)
                     {
                        // Decrement message counters
                        SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                        SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                     }
   	               
                     // Activate tasks pending for left messages
                     SpiMsgChannelNext(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ~AT91C_SPI_OVRES, SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout);
   	               
   	               usResult= usByteCnt;
   	            }
   	         }
               
               os_sem_give(SFm25clssRW[ucUnit].SemStart);
            }
	      }
      }
   }
   
	return (usResult);
}

//----------------------------------------------------------------------------
// USHORT Fm25clssRDSR
//----------------------------------------------------------------------------
USHORT Fm25clssRDSR(UCHAR ucUnit, void *pvData, U16 u16Timeout)
{
	USHORT usResult= 0;

   UCHAR ucDataIdx;
   UCHAR ucDataTx[FM25CLSS_SIZE_HEADERSR];
   UCHAR ucDataRx[sizeof(USHORT)];
   
   USHORT usByteCnt;
   USHORT usQueueCnt;
   UCHAR ucTagJobRx;

	if (ucUnit < FM25CLSS_UNIT_MAX)
   {
      if (SFm25clssRW[ucUnit].oiSemMsgNew)
      {
         if (pvData)
         {
            // Lock device
            if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].SemStart, u16Timeout)))
            {
               ucDataIdx= 0;
               ucDataTx[ucDataIdx++]= FM25CLSS_CODE_RDSR;
               ucDataTx[ucDataIdx++]= 0x00;
               usByteCnt= ucDataIdx;
               usQueueCnt= usByteCnt;
               usResult= SpiTxTag(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ucDataTx, usByteCnt, u16Timeout, FM25CLSS_TAG_SR);
               if (usResult >= usByteCnt)
               {
                  // Semaphore is given only once regardless of the number of messages
                  if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout)))
                  {
                     // Force scheduler to run other tasks yet
                     RESCHEDULE_TICKS(0);

                     // Retrieve FM25CLSS_TAG_SR tagged data
                     ucTagJobRx= FM25CLSS_TAG_SR;
                     usByteCnt= SpiRxTag(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ucDataRx, usQueueCnt, SFm25clssRO[ucUnit].u16Timeout, &ucTagJobRx);
                     if (usByteCnt > 0)
                     {
                        if (usByteCnt > 1)
                        {
                           *(UCHAR *)pvData= ucDataRx[1];
                        }
                        
                        // Decrement message counters
                        SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                        SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                     }

                     // Activate tasks pending for left messages
                     SpiMsgChannelNext(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ~AT91C_SPI_OVRES, SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout);
                     
                     usResult= 1;
                  }
               }
               
               os_sem_give(SFm25clssRW[ucUnit].SemStart);
            }
         }
      }
   }
   
	return (usResult);
}

//----------------------------------------------------------------------------
// USHORT Fm25clssWRSR
//----------------------------------------------------------------------------
USHORT Fm25clssWRSR(UCHAR ucUnit, UCHAR ucCode, U16 u16Timeout)
{
	USHORT usResult= 0;
   USHORT usPendCnt= 0;
   
   UCHAR ucDataIdx;
   UCHAR ucDataTx[FM25CLSS_SIZE_HEADERSR];
   
   ULONG ulMsgCnt;
   USHORT usByteCnt;
   
	if (ucUnit < FM25CLSS_UNIT_MAX)
   {
      if (SFm25clssRW[ucUnit].oiSemMsgNew)
      {
         // Lock device
         if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].SemStart, u16Timeout)))
         {
            // Remove write lock first (requires own /CS cycle!)
            ucDataIdx= 0;
            ucDataTx[ucDataIdx++]= FM25CLSS_CODE_WREN;
            usByteCnt= ucDataIdx;
            usResult= SpiTx(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ucDataTx, usByteCnt, u16Timeout);
            if (usResult >= usByteCnt)
            {
               // Write status register (sets write lock again as soon as complete) 
               ucDataIdx= 0;
               ucDataTx[ucDataIdx++]= FM25CLSS_CODE_WRSR;
               ucDataTx[ucDataIdx++]= ucCode;
               usByteCnt= ucDataIdx;
               usPendCnt += usResult;
               
               usResult= SpiTx(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ucDataTx, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
               if (usResult >= usByteCnt)
               {
                  usPendCnt += usResult;
                  
                  // Semaphore is given only once regardless of the number of messages
                  if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout)))
                  {
                     // Force scheduler to run other tasks yet
                     RESCHEDULE_TICKS(0);

                     do
                     {
                        // Wipe dummy data
                        usByteCnt= SpiRx(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, NULL, SPI_BUFFERCNTRX, SFm25clssRO[ucUnit].u16Timeout);
                        if (usByteCnt > 0)
                        {
                           usPendCnt -= usByteCnt;
                           
                           // Decrement message counters
                           SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                           SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                        }
                        
                        // Pend for next incoming job data when necessary
                        if (usPendCnt)
                        {
                           ulMsgCnt= SpiMsgChannelCheck(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SFm25clssRO[ucUnit].u16Timeout);
                           if (!ulMsgCnt)
                           {
                              if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout)))
                              {
                                 // Check for already retrieved next job data
                                 ulMsgCnt= SpiMsgChannelCheck(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SFm25clssRO[ucUnit].u16Timeout);
                              }
                           }
                        }
                        
                     } while (usPendCnt && ulMsgCnt);

                     // Activate tasks pending for left messages
                     SpiMsgChannelNext(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ~AT91C_SPI_OVRES, SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout);
                     
                     usResult= 1;
                  }
               }
            }
            
            os_sem_give(SFm25clssRW[ucUnit].SemStart);
         }         
      }
   }
   
	return (usResult);
}

//----------------------------------------------------------------------------
// USHORT Fm25clssRead
//----------------------------------------------------------------------------
USHORT Fm25clssRead(UCHAR ucUnit, void *pvData, USHORT usCount, USHORT usAddress, U16 u16Timeout)
{
   return (Fm25clssReadSafe(ucUnit, pvData, NULL, usCount, usAddress, u16Timeout));
}

//----------------------------------------------------------------------------
// USHORT Fm25clssReadSafe
//----------------------------------------------------------------------------
USHORT Fm25clssReadSafe(UCHAR ucUnit, void *pvData, void *pvTemp, USHORT usCount, USHORT usAddress, U16 u16Timeout)
{
	USHORT usResult= 0;

 	USHORT usResultCnt= 0;  
   USHORT usPendCnt= 0;
   ULONG ulDiffCnt= 0;
   
   UCHAR ucDataIdx;
   UCHAR ucDataTx[FM25CLSS_SIZE_HEADERMEM];

   ULONG ulMsgCnt;
   USHORT usByteCnt;
   USHORT usDataCnt;
   USHORT usQueueCnt;
   USHORT usToDoCnt;
   USHORT usBlockIdx;
   UCHAR ucTagJobRx;
   
   void *pvDest;
   ULONG ulPos;
   USHORT usBase;
   USHORT usOffset;
   UCHAR ucCycle;
   UCHAR ucCycleMax;
   
	if (ucUnit < FM25CLSS_UNIT_MAX)
   {
      if (SFm25clssRW[ucUnit].oiSemMsgNew)
      {
         if (SPI_PDCMAX > (FM25CLSS_SIZE_HEADERMEM+ FM25CLSS_SPI_FIX))
         {
            if (pvData && usCount)
            {
               // Lock device
               if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].SemStart, u16Timeout)))
               {
                  usOffset= SFm25clssRO[ucUnit].usSizeBytes/ SFm25clssRO[ucUnit].ucMultiStore;
                  if ((usAddress+ usCount) < usOffset)
                  {                 
                     ucCycleMax= (SFm25clssRO[ucUnit].ucMultiStore && (SFm25clssRO[ucUnit].bMultiTransfer || pvTemp)) ? SFm25clssRO[ucUnit].ucMultiStore : 1;
                     for (ucCycle= 0; ucCycle < ucCycleMax; ucCycle++)
                     {
                        usResultCnt= 0;
                        usToDoCnt= usCount;
                        
                        for (usBlockIdx= 0; usToDoCnt; usBlockIdx++)
                        {
                           // Set write lock first (requires own /CS cycle!)
                           ucDataIdx= 0;
                           ucDataTx[ucDataIdx++]= FM25CLSS_CODE_WRDI;
                           usByteCnt= ucDataIdx;
                           usResult= SpiTxTag(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ucDataTx, usByteCnt, u16Timeout, FM25CLSS_TAG_CTRL);
                           if (usResult >= usByteCnt)
                           {
                              usPendCnt += usResult;
                              
                              usDataCnt= _umin((SPI_PDCMAX- FM25CLSS_SIZE_HEADERMEM- FM25CLSS_SPI_FIX), usToDoCnt);
                              usQueueCnt= usDataCnt+ FM25CLSS_SIZE_HEADERMEM;
   
                              // Lock tx buffer (and check length support)
                              usResult= SpiTxLock(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, usQueueCnt, SFm25clssRO[ucUnit].u16Timeout);
                              if (usResult >= usQueueCnt)
                              {
                                 pvDest= (!ucCycle || SFm25clssRO[ucUnit].bMultiTransfer) ? (((UCHAR *)pvData)+ (usOffset* ucCycle)+ ((SPI_PDCMAX- FM25CLSS_SIZE_HEADERMEM- FM25CLSS_SPI_FIX)* usBlockIdx)) : (((UCHAR *)pvTemp)+ ((SPI_PDCMAX- FM25CLSS_SIZE_HEADERMEM- FM25CLSS_SPI_FIX)* usBlockIdx));
                                 usBase= usAddress+ (usOffset* ucCycle)+ ((SPI_PDCMAX- FM25CLSS_SIZE_HEADERMEM- FM25CLSS_SPI_FIX)* usBlockIdx);
                                 
                                 // Set up header
                                 ucDataIdx= 0;
                                 ucDataTx[ucDataIdx++]= FM25CLSS_CODE_READ;
                                 ucDataTx[ucDataIdx++]= (usBase & BITS_USHORTH) >> 8;
                                 ucDataTx[ucDataIdx++]= (usBase & BITS_USHORTL);
                                 usByteCnt= ucDataIdx;
                                 
                                 // Queue header
                                 usResult= SpiTxQueue(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ucDataTx, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                                 if (usResult >= usByteCnt)
                                 {
                                    // Queue DUMMY data by using the "no-source" feature of support.c (header already done above)
                                    usResult= SpiTxQueue(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, NULL, usDataCnt, SFm25clssRO[ucUnit].u16Timeout);
                                    if (usResult >= usDataCnt)
                                    {
                                       // Unlock, tag and send tx buffer (sets total job length!) - write data (sets write lock again as soon as complete)
                                       usResult= SpiTxTagUnlock(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, usQueueCnt, OS_T_NONE, FM25CLSS_TAG_MEM);
                                       if (usResult >= usQueueCnt)
                                       {
                                          usPendCnt += usResult;
                                          
                                          // Semaphore is given only once regardless of the number of messages
                                          if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout)))
                                          {
                                             // Force scheduler to run other tasks yet
                                             RESCHEDULE_TICKS(0);
                                             
                                             // Wipe FM25CLSS_TAG_MEM tagged dummy answer
                                             ucTagJobRx= FM25CLSS_TAG_CTRL;
                                             usResult= SpiRxTag(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, NULL, 1, SFm25clssRO[ucUnit].u16Timeout, &ucTagJobRx);
                                             if (usResult >= 1)
                                             {
                                                usPendCnt -= _umin(usPendCnt, usResult);
                                                
                                                // Decrement message counters
                                                SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usResult, SFm25clssRO[ucUnit].u16Timeout);
                                                SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usResult, SFm25clssRO[ucUnit].u16Timeout);
                                             }
               
                                             // Check for already retrieved next job data
                                             ulMsgCnt= SpiMsgChannelCheck(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SFm25clssRO[ucUnit].u16Timeout);
                                             
                                             // Pend for next incoming job data when necessary
                                             if (!ulMsgCnt) ulMsgCnt= OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout));
                                             
                                             if (ulMsgCnt)
                                             {                                                            
                                                // Wipe FM25CLSS_TAG_MEM tagged dummy answer bytes of header (part of queue tagged with FM25CLSS_TAG_MEM!)
                                                ucTagJobRx= FM25CLSS_TAG_MEM;
                                                usResult= SpiRxTag(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, NULL, FM25CLSS_SIZE_HEADERMEM, SFm25clssRO[ucUnit].u16Timeout, &ucTagJobRx);
                                                if (usResult >= FM25CLSS_SIZE_HEADERMEM)
                                                {
                                                   usPendCnt -= _umin(usPendCnt, usResult);
                                                   
                                                   // Decrement message counters
                                                   SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usResult, SFm25clssRO[ucUnit].u16Timeout);
                                                   SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usResult, SFm25clssRO[ucUnit].u16Timeout);
                                                }
         
                                                // Check for already retrieved next job data
                                                ulMsgCnt= SpiMsgChannelCheck(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SFm25clssRO[ucUnit].u16Timeout);
         
                                                // Pend for next incoming job data when necessary
                                                if (!ulMsgCnt) ulMsgCnt= OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout));
                     
                                                do
                                                {
                                                   // Receive FM25CLSS_TAG_MEM tagged data
                                                   ucTagJobRx= FM25CLSS_TAG_MEM;
                                                   usByteCnt= SpiRxTag(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, pvDest, usDataCnt, SFm25clssRO[ucUnit].u16Timeout, &ucTagJobRx);
                                                   if (usByteCnt >= usDataCnt)
                                                   {
                                                      usPendCnt -= _umin(usPendCnt, usByteCnt);
                                                      
                                                      // Decrement message counters
                                                      SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                                                      SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                                                   }
                                                   
                                                   // Pend for next incoming job data when necessary
                                                   if (usPendCnt)
                                                   {
                                                      ulMsgCnt= SpiMsgChannelCheck(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SFm25clssRO[ucUnit].u16Timeout);
                                                      if (!ulMsgCnt)
                                                      {
                                                         if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout)))
                                                         {
                                                            // Check for already retrieved next job data
                                                            ulMsgCnt= SpiMsgChannelCheck(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SFm25clssRO[ucUnit].u16Timeout);
                                                         }
                                                      }
                                                   }
         
                                                } while (usPendCnt && ulMsgCnt);

                                                // Activate tasks pending for left messages
                                                SpiMsgChannelNext(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ~AT91C_SPI_OVRES, SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout);

                                                usResultCnt += usDataCnt;
                                             }
                                          }
                                       }
                                    }
                                 }
                              }
                           
                              usToDoCnt -= _umin(usToDoCnt, usDataCnt);
                           }
                           
                           usToDoCnt= (usResult) ? (usToDoCnt) : (0);
                        }
                     }
                     
                     // Compare and count differences (for ucCyle > 0 && ucMultiStore > 1 && !SFm25clssRO[ucUnit].bMultiTransfer && pvTemp)
                     if (ucCycle)
                     {
                        if ((!SFm25clssRO[ucUnit].bMultiTransfer) && (SFm25clssRO[ucUnit].ucMultiStore > 1))
                        {
                           if (pvTemp)
                           {
                              if (usResultCnt >= usCount)
                              {
                                 for (ulPos= 0; ulPos < usCount; ulPos++)
                                 {
                                    ulDiffCnt += (*((UCHAR *)pvData+ ulPos) != *((UCHAR *)pvTemp+ ulPos));
                                 }
                              }
                              else
                              {
                                 ulDiffCnt += ABS(usResultCnt, usCount);
                              }
                           }
                        }
                     }
                  }
                  
                  os_sem_give(SFm25clssRW[ucUnit].SemStart);
               }
            }
         }
      }
   }
   
	return ((!ulDiffCnt) ? (usResultCnt) : (0));
}

//----------------------------------------------------------------------------
// USHORT Fm25clssWrite
//----------------------------------------------------------------------------
USHORT Fm25clssWrite(UCHAR ucUnit, void *pvData, USHORT usCount, USHORT usAddress, U16 u16Timeout)
{
	USHORT usResult= 0;
   
   USHORT usResultCnt= 0;
   USHORT usPendCnt= 0;
   
   UCHAR ucDataIdx;
   UCHAR ucDataTx[FM25CLSS_SIZE_HEADERMEM];

   ULONG ulMsgCnt;
   USHORT usByteCnt;
   USHORT usDataCnt;
   USHORT usQueueCnt;
   USHORT usToDoCnt;
   USHORT usBlockIdx;
   
   USHORT usBase;
   USHORT usOffset;
   UCHAR ucStoreIdx;
   void *pvDest;
   
	if (ucUnit < FM25CLSS_UNIT_MAX)
   {
      if (SFm25clssRW[ucUnit].oiSemMsgNew)
      {
         if (SPI_PDCMAX > (FM25CLSS_SIZE_HEADERMEM+ FM25CLSS_SPI_FIX))
         {
            if (pvData && usCount)
            {
               // Lock device
               if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].SemStart, u16Timeout)))
               {
                  // Calculate offset in FRAM for multistore
                  usOffset= SFm25clssRO[ucUnit].usSizeBytes/ SFm25clssRO[ucUnit].ucMultiStore;
                  if ((usAddress+ usCount) < usOffset)
                  {
                     for (ucStoreIdx= 0; ucStoreIdx < SFm25clssRO[ucUnit].ucMultiStore; ucStoreIdx++)
                     {
                        usResultCnt= 0;
                        usToDoCnt= usCount;
   
                        for (usBlockIdx= 0; usToDoCnt; usBlockIdx++)
                        {
                           // Remove write lock before every (!) write access (requires own /CS cycle!)
                           ucDataIdx= 0;
                           ucDataTx[ucDataIdx++]= FM25CLSS_CODE_WREN;
                           usByteCnt= ucDataIdx;
                           usResult= SpiTx(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ucDataTx, usByteCnt, u16Timeout);
                           if (usResult >= usByteCnt)
                           {
                              usPendCnt += usResult;
                              
                              usDataCnt= _umin((SPI_PDCMAX- FM25CLSS_SIZE_HEADERMEM- FM25CLSS_SPI_FIX), usToDoCnt);
                              usQueueCnt= usDataCnt+ FM25CLSS_SIZE_HEADERMEM;
   
                              // Lock tx buffer (and check length support)
                              usResult= SpiTxLock(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, usQueueCnt, SFm25clssRO[ucUnit].u16Timeout);
                              if (usResult >= usQueueCnt)
                              {
                                 pvDest= ((UCHAR *)pvData)+ ((SPI_PDCMAX- FM25CLSS_SIZE_HEADERMEM- FM25CLSS_SPI_FIX)* usBlockIdx);
                                 usBase= usAddress+ (usOffset* ucStoreIdx)+ ((SPI_PDCMAX- FM25CLSS_SIZE_HEADERMEM- FM25CLSS_SPI_FIX)* usBlockIdx);
   
                                 // Set up header
                                 ucDataIdx= 0;
                                 ucDataTx[ucDataIdx++]= FM25CLSS_CODE_WRITE;
                                 ucDataTx[ucDataIdx++]= (usBase & BITS_USHORTH) >> 8;
                                 ucDataTx[ucDataIdx++]= (usBase & BITS_USHORTL);
                                 usByteCnt= ucDataIdx;
                                 
                                 // Queue header
                                 usResult= SpiTxQueue(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ucDataTx, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                                 if (usResult >= usByteCnt)
                                 {
                                    // Queue data (header already done above)
                                    usResult= SpiTxQueue(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, pvDest, usDataCnt, SFm25clssRO[ucUnit].u16Timeout);
                                    if (usResult >= usDataCnt)
                                    {
                                       // Unlock and send tx buffer (sets total job length!) - write data (circuit sets write lock again as soon a write access is complete)
                                       usResult= SpiTxUnlock(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, usQueueCnt, OS_T_NONE);
                                       if (usResult >= usQueueCnt)
                                       {
                                          usPendCnt += usResult;
                                          
                                          // Semaphore is given only once regardless of the number of messages
                                          if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout)))
                                          {
                                             // Force scheduler to run other tasks yet
                                             RESCHEDULE_TICKS(0);
                                             
                                             // Wipe dummy answer of FM25CLSS_CODE_WREN
                                             usResult= SpiRx(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, NULL, 1, SFm25clssRO[ucUnit].u16Timeout);
                                             if (usResult >= 1)
                                             {
                                                usPendCnt -= _umin(usPendCnt, usResult);
                                                
                                                // Decrement message counters
                                                SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usResult, SFm25clssRO[ucUnit].u16Timeout);
                                                SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usResult, SFm25clssRO[ucUnit].u16Timeout);
                                             }
         
                                             // Check for already retrieved next job data
                                             ulMsgCnt= SpiMsgChannelCheck(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SFm25clssRO[ucUnit].u16Timeout);
         
                                             // Pend for next incoming job data when necessary
                                             if (!ulMsgCnt) ulMsgCnt= OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout));
         
                                             if (ulMsgCnt)
                                             {
                                                do
                                                {
                                                   // Wipe dummy data
                                                   usByteCnt= SpiRx(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, NULL, SPI_BUFFERCNTRX, SFm25clssRO[ucUnit].u16Timeout);
                                                   if (usByteCnt > 0)
                                                   {
                                                      usPendCnt -= _umin(usPendCnt, usByteCnt);
                                                      
                                                      // Decrement message counters
                                                      SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                                                      SpiMsgChannel(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usByteCnt, SFm25clssRO[ucUnit].u16Timeout);
                                                   }
                                                   
                                                   if (usPendCnt)
                                                   {
                                                      ulMsgCnt= SpiMsgChannelCheck(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SFm25clssRO[ucUnit].u16Timeout);
                                                      if (!ulMsgCnt)
                                                      {
                                                         if (OS_SEM_TAKE_CHK(os_sem_take(SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout)))
                                                         {   
                                                            ulMsgCnt= SpiMsgChannelCheck(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SFm25clssRO[ucUnit].u16Timeout);
                                                         }
                                                      }
                                                   }
                                                
                                                } while (usPendCnt && ulMsgCnt);

                                                // Activate tasks pending for left messages
                                                SpiMsgChannelNext(SFm25clssRO[ucUnit].ucUnit, SFm25clssRO[ucUnit].ucChannel, ~AT91C_SPI_OVRES, SFm25clssRW[ucUnit].oiSemMsgNew, SFm25clssRO[ucUnit].u16Timeout);

                                                // Return user data byte count
                                                usResultCnt += usDataCnt;
                                             }
                                          }
                                       }
                                    }
                                 }
                              }
                              
                              usToDoCnt -= _umin(usToDoCnt, usDataCnt);
                           }
                           
                           usToDoCnt= (usResult) ? (usToDoCnt) : (0);
                        }
                     }
                  }
                  
                  os_sem_give(SFm25clssRW[ucUnit].SemStart);
               }
            }
         }
      }
   }
   
   // Debug
   if (!usResultCnt)
   {
      usResultCnt= usResultCnt;
   }
   
	return (usResultCnt);
}

