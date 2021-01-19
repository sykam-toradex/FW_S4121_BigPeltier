//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/DAC/DAC_GENERIC/DAC_GENERIC.C
// Purpose: PERIPHERAL SUPPORT
// Author:  rradtke, hschoettner
//----------------------------------------------------------------------------
// 2012-01-16 rr: initial version imported from "dacrrcs085.c"
// 2013-07-21 renamed DACxxxS085_SETOUTPUT to DACRRCS085_SETOUTPUT
// 2013-08-22 changed RESCHEDULE (ms) to RESCHEDULE_TICKS (ticks)
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "dac_generic.h"
#include "dac_generic_data.h"
#include "dac_generic_dev.h"
#include "dac_generic_ext.h"

#include "..\..\..\..\cpu\at91\spi\spi.h"
#include "..\..\..\..\cpu\at91\spi\spiext.h"


// init driver structs
struct _SDacGenericRW SDacGenericRW;

// hs: should be moved to AT91SAM7X.c (and only referenced here in the framework)!
const struct _SDacGeneric_GenericDacType cSDacGeneric_DacTypes[DAC_TYPE_COUNT] =
{
   // CAUTION: the vertical order of this struct must be the same as in the enum "_EDacGeneric_DacTypes"
   
   //                   ucNumDataBits, ucNumFrameBytes,  ucNumChannels, ucSpiUnit,                          ucSpiChannel
   /* DAC102S085     */ {10,           2,                2,             AT91SAM7X__DAC102S085_SPI_UNIT,     AT91SAM7X__DAC102S085_SPI_CHANNEL   },
   /* DAC104S085     */ {10,           2,                4,             AT91SAM7X__DAC104S085_SPI_UNIT,     AT91SAM7X__DAC104S085_SPI_CHANNEL   },
   /* DAC8552        */ {16,           3,                2,             AT91SAM7X__DAC8552_SPI_UNIT,        AT91SAM7X__DAC8552_SPI_CHANNEL      }
};

const USHORT usDriverTimeout = 1;  // timeout value in system ticks

//----------------------------------------------------------------------------
// DacGenericInit
//----------------------------------------------------------------------------
UCHAR DacGenericInit(OS_ID SemSpiNewMsgDac)
{
	UCHAR ucResult = 0;
   UCHAR ucIdx;


   if (SemSpiNewMsgDac)
   {
   	SDacGenericRW.SemSpiNewMsgDac = SemSpiNewMsgDac;
   
      // Prepare cache
      for (ucIdx = 0; ucIdx < (UCHAR)DAC_GENERIC__SPI_CHANNELS_MAX; ucIdx++)
      {
         SDacGenericRW.SDataCache.ulValue[ucIdx] =  0;
         SDacGenericRW.SDataCache.bValid[ucIdx]  =  FALSE;
      }
      
      // Prepare OS tools
      os_sem_init(SDacGenericRW.SemStart, 1);

      ucResult = 1;
   }

	return (ucResult);
}

//----------------------------------------------------------------------------
// DacGenericSetOutput
//----------------------------------------------------------------------------
USHORT DacGenericSetOutput(UCHAR ucDacType, UCHAR ucChannel, UCHAR ucMode, ULONG ulData, U16 u16Timeout)
{
   UCHAR    ucDataTx[DAC_GENERIC__FRAME_BYTES_MAX];

	USHORT   usResult    = 0;
	USHORT   usPendCnt   = 0;
   USHORT   usByteCnt;
   
 	ULONG    ulMsgCnt;
   ULONG    ulDataTx;


   // check for correct type
   if (ucDacType < (UCHAR)DAC_TYPE_COUNT)
   {
      // check for exceeded number of channels, frame bytes and data bits
	   if (     (ucChannel                                            <  cSDacGeneric_DacTypes[ucDacType].ucNumChannels)
            && (cSDacGeneric_DacTypes[ucDacType].ucNumChannels       <= (UCHAR)DAC_GENERIC__CHANNELS_MAX)
            && (cSDacGeneric_DacTypes[ucDacType].ucNumFrameBytes     <= (UCHAR)DAC_GENERIC__FRAME_BYTES_MAX)
            && (cSDacGeneric_DacTypes[ucDacType].ucNumDataBits       <= (UCHAR)DAC_GENERIC__DATA_BITS_MAX)
         )
	   {
         // check for SPI missing configuration
         if (     (cSDacGeneric_DacTypes[ucDacType].ucSpiUnit        <  (UCHAR)DAC_GENERIC__SPI_UNITS_MAX) 
               && (cSDacGeneric_DacTypes[ucDacType].ucSpiChannel     <  (UCHAR)DAC_GENERIC__SPI_CHANNELS_MAX)
            )
         {
            // check for missing driver init
            if (SDacGenericRW.SemSpiNewMsgDac)
            {
               // Lock device
               if (OS_SEM_TAKE_CHK(os_sem_take(SDacGenericRW.SemStart, u16Timeout)))
               {
                  if ((SDacGenericRW.SDataCache.ulValue[ucChannel] != ulData) || (!SDacGenericRW.SDataCache.bValid[ucChannel]))
                  {
                     // start dac type specific data assembly here
                     switch (ucDacType)
                     {
                        case (((UCHAR)DAC102S085) || ((UCHAR)DAC104S085)):
                        {
                  			ulDataTx    =  DACRRCS085_SETOUTPUT(ucChannel, ucMode, ulData, cSDacGeneric_DacTypes[ucDacType].ucNumDataBits);
                           ucDataTx[0] =  (((USHORT)ulDataTx & BITS_USHORTH) >> 8);
                           ucDataTx[1] =  ((USHORT)ulDataTx & BITS_USHORTL);
                           usByteCnt   =  (USHORT)cSDacGeneric_DacTypes[ucDacType].ucNumFrameBytes;
                           break;
                        }
      
                        case (((UCHAR)DAC8552)):
                        {
                           ucDataTx[0] =  ucMode | (ucChannel << 2);
                           ucDataTx[1] =  (UCHAR)((ulData & BITS_ULONGLH) >> 8);
                           ucDataTx[2] =  (UCHAR)(ulData & BITS_ULONGLL);
                           usByteCnt   =  (USHORT)cSDacGeneric_DacTypes[ucDacType].ucNumFrameBytes;
                           break;
                        }
                        
                        default: // dac type not supported at moment
                        {
                           break;
                        }
                     }
      
                     // type switch ends here
            			usResult = SpiTx(cSDacGeneric_DacTypes[ucDacType].ucSpiUnit, cSDacGeneric_DacTypes[ucDacType].ucSpiChannel, ucDataTx, usByteCnt, usDriverTimeout);
            			  
                     // Retrieve/wipe reply for successful tx only
                     if (usResult >= usByteCnt)
                     {
                        usPendCnt += usResult;
                        
               			// Semaphore is given only once regardless of the number of messages
               			if (OS_SEM_TAKE_CHK(os_sem_take(SDacGenericRW.SemSpiNewMsgDac, usDriverTimeout)))
               			{
                           // Force scheduler to run other tasks yet
                           RESCHEDULE_TICKS(0);
                           
                           do
                           {
                  				usByteCnt = SpiRx(cSDacGeneric_DacTypes[ucDacType].ucSpiUnit, cSDacGeneric_DacTypes[ucDacType].ucSpiChannel, NULL, SPI_BUFFERCNTRX, usDriverTimeout);
                  				if (usByteCnt > 0)
                  				{
                                 usPendCnt -= _umin(usPendCnt, usByteCnt);
                                 
                  					// Decrement message counters
                  					SpiMsgChannel(cSDacGeneric_DacTypes[ucDacType].ucSpiUnit, cSDacGeneric_DacTypes[ucDacType].ucSpiChannel, AT91C_SPI_RXBUFF, usByteCnt, usDriverTimeout);
                  					SpiMsgChannel(cSDacGeneric_DacTypes[ucDacType].ucSpiUnit, cSDacGeneric_DacTypes[ucDacType].ucSpiChannel, AT91C_SPI_TXBUFE, usByteCnt, usDriverTimeout);
                  				}
                              
                              // Pend for next incoming job data when necessary
                              if (usPendCnt)
                              {
                                 ulMsgCnt= SpiMsgChannelCheck(cSDacGeneric_DacTypes[ucDacType].ucSpiUnit, cSDacGeneric_DacTypes[ucDacType].ucSpiChannel, AT91C_SPI_RXBUFF, usDriverTimeout);
                                 if (!ulMsgCnt)
                                 {
                                    if (OS_SEM_TAKE_CHK(os_sem_take(SDacGenericRW.SemSpiNewMsgDac, usDriverTimeout)))
                                    {
                                       ulMsgCnt = SpiMsgChannelCheck(cSDacGeneric_DacTypes[ucDacType].ucSpiUnit, cSDacGeneric_DacTypes[ucDacType].ucSpiChannel, AT91C_SPI_RXBUFF, usDriverTimeout);
                                    }
                                 }
                              }
                              
                           } while (usPendCnt && ulMsgCnt);
      
                           // Update last output cache
                           SDacGenericRW.SDataCache.ulValue[ucChannel]  =  ulData;
                           SDacGenericRW.SDataCache.bValid[ucChannel]   =  TRUE;
      
                           // Activate tasks pending for left messages
                           SpiMsgChannelNext(cSDacGeneric_DacTypes[ucDacType].ucSpiUnit, cSDacGeneric_DacTypes[ucDacType].ucSpiChannel, ~AT91C_SPI_OVRES, SDacGenericRW.SemSpiNewMsgDac, usDriverTimeout);
      
                           usResult = usByteCnt;
                        }
                     }
                  }
                  else
                  {
                     usResult = (USHORT)DAC_GENERIC__FRAME_BYTES_MAX;   // Fake success for unchanged DAC value
                  }
                  
                  os_sem_give(SDacGenericRW.SemStart);               
               }
            }
	      }
      }
   }
   
	return (usResult);
}

//----------------------------------------------------------------------------
// DacGenericGetOutput
//----------------------------------------------------------------------------
USHORT DacGenericGetOutput(UCHAR ucDacType, UCHAR ucChannel, ULONG *pulDataLast, U16 u16Timeout)
{
	USHORT usResult = 0;


   // check for correct type
   if (ucDacType < (UCHAR)DAC_TYPE_COUNT)
   {
      // check for exceeded number of channels, frame bytes and data bits
	   if (     (ucChannel                                            <  cSDacGeneric_DacTypes[ucDacType].ucNumChannels)
            && (cSDacGeneric_DacTypes[ucDacType].ucNumChannels       <= (UCHAR)DAC_GENERIC__CHANNELS_MAX)
            && (cSDacGeneric_DacTypes[ucDacType].ucNumFrameBytes     <= (UCHAR)DAC_GENERIC__FRAME_BYTES_MAX)
            && (cSDacGeneric_DacTypes[ucDacType].ucNumDataBits       <= (UCHAR)DAC_GENERIC__DATA_BITS_MAX)
         )
	   {
         // check for SPI missing configuration
         if (     (cSDacGeneric_DacTypes[ucDacType].ucSpiUnit        <  (UCHAR)DAC_GENERIC__SPI_UNITS_MAX) 
               && (cSDacGeneric_DacTypes[ucDacType].ucSpiChannel     <  (UCHAR)DAC_GENERIC__SPI_CHANNELS_MAX)
            )
         {
            // check for missing driver init
            if (SDacGenericRW.SemSpiNewMsgDac)
            {
               if (pulDataLast)
         	   {
                  // Lock device
               	if (OS_SEM_TAKE_CHK(os_sem_take(SDacGenericRW.SemStart, u16Timeout)))               
                  {
                     if (SDacGenericRW.SDataCache.bValid[ucChannel])
                     {
                        switch (ucDacType)
                        {
                           case (UCHAR)DAC8552:
                           {
                              *pulDataLast   =  SDacGenericRW.SDataCache.ulValue[ucChannel];
                              usResult       =  (USHORT)DAC_GENERIC__FRAME_BYTES_MAX;
                              break;
                           }
                           
                           default: // dac type not supported at moment
                           {
                              break;
                           }
                        }
                     }
                     
                     os_sem_give(SDacGenericRW.SemStart);
                  }
               }
            }
         }
      }
   }
   
   return (usResult);
}
