//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/DAC/DACRRCS085/DACRRCS085.C
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-28 initial version
// 2010-01-29 added checking for valid (external initialized) oiSemMsgNew
// 2010-02-18 completed checking of SpiTx result
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-09 optimized for speed by avoiding unnecessary semaphore timeouts
// 2010-10-22 secured rx data processing in Dacrrcs085SetOutput
// 2011-02-03 added semaphore SemStart to lock the busy device in all functions
// 2011-03-09 added reschedule call after os_sem_take of SemMsgNew in Dacrrcs085SetOutput
// 2011-03-09 removed checking of AT91C_SPI_TXBUFE messages in Dacrrcs085SetOutput
// 2011-03-29 rr: added function Dacrrcs085GetOutput using new out cache
// 2011-03-29 rr: added checking in Dacrrcs085SetOutput to ignore unchanged value
// 2011-03-29 rr: added additional init in Dacrrcs085Init for cache
// 2011-04-14 fixed additional init in Dacrrcs085Init for cache and its flags
// 2011-04-14 fixed cache checking in Dacrrcs085SetOutput & added cache valid support
// 2011-04-14 fixed Dacrrcs085GetOutput to verify ptr, cache valid & use proper semaphore
// 2011-05-04 changed Dacrrcs085SetOutput to use SpiChannelNext function
// 2011-09-22 minor changes to data telegram tx set-up in Dacrrcs085SetOutput
// 2013-04-11 fixed function header's return data type comments
// 2013-08-22 changed timeout from milliseconds to ticks
// 2013-08-22 changed RESCHEDULE (ms) to RESCHEDULE_TICKS (ticks)
// 2013-08-29 added checking of availability of required transfer block size
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "dacrrcs085.h"
#include "dacrrcs085data.h"
#include "dacrrcs085dev.h"
#include "dacrrcs085ext.h"

#include "..\..\..\..\cpu\at91\spi\spi.h"
#include "..\..\..\..\cpu\at91\spi\spiext.h"

// Number of units located inside dacrrcs085.h
struct _SDacrrcs085RW SDacrrcs085RW[DACRRCS085_UNIT_MAX];

//----------------------------------------------------------------------------
// UCHAR Dacrrcs085Init
//----------------------------------------------------------------------------
UCHAR Dacrrcs085Init(UCHAR ucUnit, UCHAR ucChannel, OS_ID oiMsgNewSem)
{
	UCHAR ucResult= 0;

   UCHAR ucIdx;

	if (ucUnit < DACRRCS085_UNIT_MAX)
	{
      if (SPI_PDCMAX > DACRRCS085_BLOCKSIZEMAX)
      {
         if (SDacrrcs085RO[ucUnit].u16Timeout)
         {
      		SDacrrcs085RW[ucUnit].oiSemMsgNew= oiMsgNewSem;
      
            // Prepare cache
            for (ucIdx= 0; ucIdx < DACRRCS085_NUM_CHANNELS; ucIdx++)
            {
               SDacrrcs085RW[ucUnit].SCacheOut.usValue[ucIdx]= 0;
               SDacrrcs085RW[ucUnit].SCacheOut.bValid[ucIdx]= FALSE;
            }
            
            // Prepare OS tools
            os_sem_init(SDacrrcs085RW[ucUnit].SemStart, 1);
      
      		ucResult= (SDacrrcs085RW[ucUnit].oiSemMsgNew != 0);
         }
      }
	}
	
	return (ucResult);
}

//----------------------------------------------------------------------------
// USHORT Dacrrcs085SetOutput
//----------------------------------------------------------------------------
USHORT Dacrrcs085SetOutput(UCHAR ucUnit, UCHAR ucChannel, UCHAR ucMode, USHORT usOut, U16 u16Timeout)
{
	USHORT usResult= 0;
	USHORT usPendCnt= 0;
   
	ULONG ulMsgCnt;
   USHORT usByteCnt;
   
   USHORT usDataTx;
   UCHAR ucDataTx[DACRRCS085_BLOCKSIZEMAX];

	if (ucUnit < DACRRCS085_UNIT_MAX)
	{
		if (ucChannel < SDacrrcs085RO[ucUnit].ucChannels)
		{
         if (SDacrrcs085RW[ucUnit].oiSemMsgNew)
         {
            // Lock device
            if (OS_SEM_TAKE_CHK(os_sem_take(SDacrrcs085RW[ucUnit].SemStart, u16Timeout)))
            {
               if ((SDacrrcs085RW[ucUnit].SCacheOut.usValue[ucChannel] != usOut) || (!SDacrrcs085RW[ucUnit].SCacheOut.bValid[ucChannel]))
               {
         			usDataTx= DACRRCS085_SETOUTPUT(ucChannel, ucMode, usOut, SDacrrcs085RO[ucUnit].ucBits);
                  ucDataTx[0]= ((usDataTx & BITS_USHORTH) >> 8);
                  ucDataTx[1]= (usDataTx & BITS_USHORTL);
                  usByteCnt= sizeof(USHORT);
         			usResult= SpiTx(SDacrrcs085RO[ucUnit].ucUnit, SDacrrcs085RO[ucUnit].ucChannel, ucDataTx, usByteCnt, SDacrrcs085RO[ucUnit].u16Timeout);
         			  
                  // Retrieve/wipe reply for successful tx only
                  if (usResult >= usByteCnt)
                  {
                     usPendCnt += usResult;
                     
            			// Semaphore is given only once regardless of the number of messages
            			if (OS_SEM_TAKE_CHK(os_sem_take(SDacrrcs085RW[ucUnit].oiSemMsgNew, SDacrrcs085RO[ucUnit].u16Timeout)))
            			{
                        // Force scheduler to run other tasks yet
                        RESCHEDULE_TICKS(0);
                        
                        do
                        {
               				usByteCnt= SpiRx(SDacrrcs085RO[ucUnit].ucUnit, SDacrrcs085RO[ucUnit].ucChannel, NULL, SPI_BUFFERCNTRX, SDacrrcs085RO[ucUnit].u16Timeout);
               				if (usByteCnt > 0)
               				{
                              usPendCnt -= _umin(usPendCnt, usByteCnt);
                              
               					// Decrement message counters
               					SpiMsgChannel(SDacrrcs085RO[ucUnit].ucUnit, SDacrrcs085RO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usByteCnt, SDacrrcs085RO[ucUnit].u16Timeout);
               					SpiMsgChannel(SDacrrcs085RO[ucUnit].ucUnit, SDacrrcs085RO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usByteCnt, SDacrrcs085RO[ucUnit].u16Timeout);
               				}
                           
                           // Pend for next incoming job data when necessary
                           if (usPendCnt)
                           {
                              ulMsgCnt= SpiMsgChannelCheck(SDacrrcs085RO[ucUnit].ucUnit, SDacrrcs085RO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SDacrrcs085RO[ucUnit].u16Timeout);
                              if (!ulMsgCnt)
                              {
                                 if (OS_SEM_TAKE_CHK(os_sem_take(SDacrrcs085RW[ucUnit].oiSemMsgNew, SDacrrcs085RO[ucUnit].u16Timeout)))
                                 {
                                    ulMsgCnt= SpiMsgChannelCheck(SDacrrcs085RO[ucUnit].ucUnit, SDacrrcs085RO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SDacrrcs085RO[ucUnit].u16Timeout);
                                 }
                              }
                           }
                           
                        } while (usPendCnt && ulMsgCnt);

                        // Update last output cache
                        SDacrrcs085RW[ucUnit].SCacheOut.usValue[ucChannel]= usOut;
                        SDacrrcs085RW[ucUnit].SCacheOut.bValid[ucChannel]= TRUE;

                        // Activate tasks pending for left messages
                        SpiMsgChannelNext(SDacrrcs085RO[ucUnit].ucUnit, SDacrrcs085RO[ucUnit].ucChannel, ~AT91C_SPI_OVRES, SDacrrcs085RW[ucUnit].oiSemMsgNew, SDacrrcs085RO[ucUnit].u16Timeout);

                        usResult= usByteCnt;
                     }
                  }
               }
               else
               {
                  usResult= DACRRCS085_BLOCKSIZEMAX;   // Fake success for unchanged DAC value
               }
               
               os_sem_give(SDacrrcs085RW[ucUnit].SemStart);               
            }
         }
		}
	}
	
	return (usResult);
}

//----------------------------------------------------------------------------
// USHORT Dacrrcs085GetOutput
//----------------------------------------------------------------------------
USHORT Dacrrcs085GetOutput(UCHAR ucUnit, UCHAR ucChannel, USHORT *pusOutLast, U16 u16Timeout)
{
	USHORT usResult= 0;

	if (ucUnit < DACRRCS085_UNIT_MAX)
	{
		if (ucChannel < SDacrrcs085RO[ucUnit].ucChannels)
		{
         if (SDacrrcs085RW[ucUnit].oiSemMsgNew)
         {
            if (pusOutLast)
      	   {
               // Lock device
            	if (OS_SEM_TAKE_CHK(os_sem_take(SDacrrcs085RW[ucUnit].SemStart, u16Timeout)))               
               {
                  if (SDacrrcs085RW[ucUnit].SCacheOut.bValid[ucChannel])
                  {
                     *pusOutLast= SDacrrcs085RW[ucUnit].SCacheOut.usValue[ucChannel];
                     usResult= DACRRCS085_BLOCKSIZEMAX;
                  }
                  
                  os_sem_give(SDacrrcs085RW[ucUnit].SemStart);
               }
            }
         }
      }
   }
   
   return (usResult);
}
