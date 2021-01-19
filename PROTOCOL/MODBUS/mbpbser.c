//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBSER.C
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-26 moved here from mbpb.c
// 2009-08-26 changed notifyto to ignore tx done caused by driver block size
// 2009-09-02 fixed code changes for asc mode also always exexcutes after TO
// 2009-09-02 added fake checksum feature replace for terminal user asc input
// 2009-09-02 changed forced OS_T_FOREVER only for new telegram data input
// 2009-09-03 added MBPBTimeOutCalc function
// 2009-09-04 changed MBPBTimeOutCalc to MBPBSerTimeOutCalc and its params
// 2009-09-04 changed MBPBTimeOutCalc to return us instead of ms
// 2009-09-09 added MBPBSerAddrSet and MBPBSerAddrFix
// 2009-09-25 changed to fit single linked reentrant usart driver model
// 2009-10-01 removed local static data from MBPBSerNotifyRx
// 2010-05-05 moved params of SDriver to SDriver.SConfig
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-08-12 added function MBPBSerSyncPrepareRx
// 2010-08-27 introduced new state MBPB_STATE_RX_COMPLETE
// 2010-09-02 improved asc mode received frame processing by forced timeout
// 2010-09-02 migrated from ATOMIC... to LOCKED macro and built locked blocks
// 2010-09-08 added missing return code in MBPBSerParserCook for MBPB_MODE_RTU
// 2010-09-08 added function MBPBSerEncode for simple master interface
// 2010-09-15 added function MBPBSerDecode for simple master interface
// 2010-09-08 added function MBPBSerReplyBytes for simple master interface
// 2011-08-04 replaced SPortRW.ucAddrMaskTx in MBPBSerAddrFix with MBPB_ADDR_MASK
// 2011-12-06 changed MBPBSerCookedReplyBytes for MBPBFuncReportSlaveID (32 bit class)
// 2012-07-04 fixed and clarified comments for MBPBCRC16 and CRC16 calls
// 2013-01-23 changed data block loop type of MBPBSerNotifyRx/To to USHORT
// 2013-01-23 changed data block loop type of MBPBSerDecode to ULONG
// 2013-01-24 imported MBPBSerParserMaximum from VCPP implementation
// 2013-01-24 changed Count of MBPBSerNotifyRx to USHORT, Repeat index to ULONG
// 2013-01-24 changed Count of MBPBSerNotifyTxDone to USHORT
// 2013-01-24 changed Count of MBPBSerNotifyBitTo to USHORT, Repeat index to ULONG
// 2013-02-13 added debug cases in MBPBSerNotifyRx for further state checking
// 2013-02-13 renamed MBPBSerNotifyTo to MBPBSerNotifyExTo and added param
// 2013-02-13 added MBPBSerNotifyTo wrapper for MBPBSerNotifyExTo
// 2013-04-11 added function headers with return data type comment
// 2013-04-24 added support for general reference type to MBPBSerParserMaximum
// 2013-05-01 fixed support for general reference write in MBPBSerParserMaximum
// 2013-05-02 added support for general reference type to MBPBSerCookedReplyBytes
// 2016-05-11 added rx stuck byte detection to MBPBSerSyncPrepareRx
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#include <math.h>

#include "..\..\..\rtl\support.h"

#include "mbpb.h"
#include "mbpbcfg.h"
#include "mbpbframe.h"
#include "mbpbcode.h"
#include "mbpbdata.h"
#include "mbpbext.h"
#include "mbpbutils.h"
#include "mbpbser.h"

// Functions for syncronous serial line transmission

//----------------------------------------------------------------------------
// USHORT MBPBSerSyncPrepareRx
//----------------------------------------------------------------------------
USHORT MBPBSerSyncPrepareRx(void *pvParam)
{
   struct _SMBPBPara *pSMBPBPara= pvParam;
   USHORT usResult= 0;
   USHORT usCount= 0;

   // For syncronous drivers we always queue reply data to enable them to receive again
   
   // Determine and release any rx stuck bytes
   if (pSMBPBPara->SPortRO.SDriver.Msg)
   {
      do
      {
         usCount= pSMBPBPara->SPortRO.SDriver.Msg(pSMBPBPara->SPortRO.SDriver.SConfig.ucUnit, pSMBPBPara->SPortRO.SDriver.SConfig.ucChannel, pSMBPBPara->SPortRO.ulMsgBitRx, 0, pSMBPBPara->SPortRO.SDriver.SConfig.u16To);
         if (usCount)
         {
            RESCHEDULE(0);
         }
         
      } while (usCount);
   }
      
   // Queue data using no-source feature of memcpyflex used by SpiTx
   if (pSMBPBPara->SPortRO.SDriver.Tx) usResult= pSMBPBPara->SPortRO.SDriver.Tx(pSMBPBPara->SPortRO.SDriver.SConfig.ucUnit, pSMBPBPara->SPortRO.SDriver.SConfig.ucChannel, NULL, pSMBPBPara->SPortRO.SDriver.SConfig.usMaxTx, pSMBPBPara->SModeRW[pSMBPBPara->SPortRW.EMode].ulTimeOutMS);
   
   return (usResult);
}

// Functions for classical serial line transmission

//----------------------------------------------------------------------------
// ULONG MBPBSerTimeOutCalc
//----------------------------------------------------------------------------
ULONG MBPBSerTimeOutCalc(EMBPBMode EMode, ULONG ulBaud)
{
   ULONG ulTimeOutUS= 0;
   
   switch (EMode)
   {
      case MBPB_MODE_ASC:
         ulTimeOutUS= MBPB_TIMEOUTUSASC;  
         break;
      
      case MBPB_MODE_RTU:
         // If baudrate > 19200 then we should use the fixed timer values t35= 1750us
         // Otherwise t35 must be 3.5 times the character time
         if (ulBaud > 19200)
         {
            // 1800us
            ulTimeOutUS= 1800;       
         }
         else
         {
            // 3.5 times the character time
            ulTimeOutUS= ceil((1000000.0L/ ulBaud)* 3.5);
         }
         break;      
   }

   return (ulTimeOutUS);
}
//----------------------------------------------------------------------------
// UCHAR MBPBSerAddrGet
//----------------------------------------------------------------------------
UCHAR MBPBSerAddrGet(struct _SMBPBPara *pSMBPBPara)
{
   UCHAR ucAddr;
   
   LOCKED(ucAddr= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[MBPB_SER_PDU_ADDR_OFF]);
   
   return (ucAddr);
}

//----------------------------------------------------------------------------
// UCHAR MBPBSerAddrSet
//----------------------------------------------------------------------------
UCHAR MBPBSerAddrSet(struct _SMBPBPara *pSMBPBPara, UCHAR ucAddr)
{  
   LOCKED(pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[MBPB_SER_PDU_ADDR_OFF]= ucAddr);
   
   return (ucAddr);
}

//----------------------------------------------------------------------------
// UCHAR MBPBSerAddrFix
//----------------------------------------------------------------------------
UCHAR MBPBSerAddrFix(struct _SMBPBPara *pSMBPBPara)
{
   UCHAR ucAddrMaskTx= MBPB_ADDR_MASK;
   UCHAR ucPBAddr;

   if (ucAddrMaskTx)
   {      
      ucPBAddr= MBPBSerAddrGet(pSMBPBPara) & ucAddrMaskTx;
   }
   else
   {
      ucPBAddr= MBPBSerAddrGet(pSMBPBPara);
   }
   
   return (ucPBAddr);
}

//----------------------------------------------------------------------------
// USHORT MBPBSerLenGet
//----------------------------------------------------------------------------
USHORT MBPBSerLenGet(struct _SMBPBPara *pSMBPBPara)
{
   USHORT usLen;
   USHORT usCnt;
   EMBPBMode EMode;

   LOCK;
   {
      // Safely access EMode and update temporary local copy
      EMode= pSMBPBPara->SPortRW.EMode;
      
      // Safely access usCnt and update temporary local copy
      usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt;
   }
   UNLOCK;
    
   switch (EMode)
   {
      case MBPB_MODE_ASC:
         usLen= (USHORT)(usCnt- MBPB_SER_PDU_PDU_OFF- MBPB_ASC_PDU_SIZE_LRC);
         break;
         
      case MBPB_MODE_RTU:
         usLen= (USHORT)(usCnt- MBPB_SER_PDU_PDU_OFF- MBPB_RTU_PDU_SIZE_CRC);
         break;
   }
   
   return (usLen);
}

//----------------------------------------------------------------------------
// USHORT MBPBSerLenFix
//----------------------------------------------------------------------------
USHORT MBPBSerLenFix(struct _SMBPBPara *pSMBPBPara)
{
   USHORT usCntFrame;
   USHORT usCnt;
   EMBPBMode EMode;

   LOCK;
   {
      // Safely access EMode and update temporary local copy
      EMode= pSMBPBPara->SPortRW.EMode;
      
      // Safely access usCnt and update temporary local copy
      usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt;
   }
   UNLOCK;
          
   switch (EMode)
   {
      case MBPB_MODE_ASC:
         usCntFrame= (USHORT)(usCnt+ MBPB_SER_PDU_PDU_OFF+ MBPB_ASC_PDU_SIZE_LRC);
         break;
         
      case MBPB_MODE_RTU:
         usCntFrame= (USHORT)(usCnt+ MBPB_SER_PDU_PDU_OFF+ MBPB_RTU_PDU_SIZE_CRC);
         break;
   }
   
   return (usCntFrame);
}

//----------------------------------------------------------------------------
// UCHAR * MBPBSerDataGet
//----------------------------------------------------------------------------
UCHAR * MBPBSerDataGet(struct _SMBPBPara *pSMBPBPara)
{
   return ((UCHAR *) &pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[MBPB_SER_PDU_PDU_OFF]);
}

//----------------------------------------------------------------------------
// void MBPBSerNotifyRx
//----------------------------------------------------------------------------
void MBPBSerNotifyRx(struct _SMBPBPara *pSMBPBPara, USHORT usCount, UCHAR *pucPrevAr, UCHAR *pucPrevIdx)
{
   ULONG ulRepeat;
   UCHAR ucByteCnt;
   UCHAR ucByte;
   UCHAR ucResult;
   UCHAR usCnt;
   UCHAR ucCR;
   UCHAR ucLF;
   UCHAR ucChk;
   
   bool bForceTo= FALSE;
   
   EMBPBMode EMode;
   EMBPBStateRx EStateRx;
   EMBPBBytePos EBytePos;
      
   // Safely access EMode and update temporary local copy
   LOCKED(EMode= pSMBPBPara->SPortRW.EMode);      

   // Lock - Safely access and modify EStateRx
   if (SSEM_LOCK(SSemLock, pSMBPBPara->SModeRW[EMode]))
   {                 
      for (ulRepeat= 0; ulRepeat < usCount; ulRepeat++)
      {
         ucByteCnt= pSMBPBPara->SPortRO.SDriver.Rx(pSMBPBPara->SPortRO.SDriver.SConfig.ucUnit, pSMBPBPara->SPortRO.SDriver.SConfig.ucChannel, &ucByte, 1, pSMBPBPara->SPortRO.SDriver.SConfig.u16To);            
         
         if (ucByteCnt)
         {
            LOCK;
            {
               usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt;
               EStateRx= pSMBPBPara->SModeRW[EMode].EStateRx;
            }
            UNLOCK;
                        
            if (EMode == MBPB_MODE_ASC)
            {
               // ASC
               
               LOCK;
               {
                  ucCR= pSMBPBPara->SModeRW[EMode].ucCR;
                  ucLF= pSMBPBPara->SModeRW[EMode].ucLF;
               }
               UNLOCK;
               
               switch (EStateRx)
               {
                  case MBPB_STATE_RX_RCV:
                     if (ucByte == ':')
                     {
                        *pucPrevIdx= 0;
                        
                        LOCK;
                        {
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos= 0;
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= 0;                  
                           pSMBPBPara->SModeRW[EMode].EBytePos= MBPB_BYTE_HIGH_NIBBLE;
                        }
                        UNLOCK;
                     }
                     else if(ucByte == ucCR)
                     {
                        // Replace faked (= ??) checksum
                        if (pucPrevAr[0] == '?' && pucPrevAr[1] == '?')
                        {
                           // Replace invalid byte value caused by invalid hex characters (= ??)
                           LOCKED(pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[usCnt- 1]= 0);
                           
                           // Recalculate checksum
                           ucChk= MBPBLRC(pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData, usCnt);
                           
                           // Replace checksum
                           LOCKED(pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[usCnt- 1]= ucChk);
                        }
                        
                        LOCKED(pSMBPBPara->SModeRW[EMode].EStateRx= MBPB_STATE_RX_WAIT_EOF);
                     }
                     else
                     {
                        ucResult= MBPBChar2Bin(ucByte);
                        
                        LOCKED(EBytePos= pSMBPBPara->SModeRW[EMode].EBytePos);
                        switch (EBytePos)
                        {
                           case MBPB_BYTE_HIGH_NIBBLE:
                              if (usCnt <= MBPB_SER_PDU_SIZE_MAX)
                              {
                                 LOCK;
                                 {
                                    pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos]= (UCHAR)(ucResult << 4);
                                    pSMBPBPara->SModeRW[EMode].EBytePos= MBPB_BYTE_LOW_NIBBLE;
                                 }
                                 UNLOCK;
                              }
                              else
                              {
                                 // Not handled in Modbus specification but seems a resonable implementation
                                 LOCKED(pSMBPBPara->SModeRW[EMode].EStateRx= MBPB_STATE_RX_IDLE);
                              }
                              break;
                              
                           case MBPB_BYTE_LOW_NIBBLE:
                              LOCK;
                              {
                                 pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos] |= ucResult;
                                 pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos+ 1;
                                 pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt+ 1;
                                 pSMBPBPara->SModeRW[EMode].EBytePos= MBPB_BYTE_HIGH_NIBBLE;
                              }
                              UNLOCK;
                              break;
                        }
                     }
                     break;
                     
                  case MBPB_STATE_RX_WAIT_EOF:
                     if (ucByte == ucLF)
                     {
                        LOCKED(pSMBPBPara->SModeRW[EMode].EStateRx= MBPB_STATE_RX_COMPLETE);
                        
                        // Send receive msg immediately
                        bForceTo= TRUE;
                     }
                     else if (ucByte == ':')
                     {
                        *pucPrevIdx= 0;
                        LOCK;
                        {
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos= 0;
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= 0;                                       
                           pSMBPBPara->SModeRW[EMode].EBytePos= MBPB_BYTE_HIGH_NIBBLE;
                           
                           pSMBPBPara->SModeRW[EMode].EStateRx= MBPB_STATE_RX_RCV;
                        }
                        UNLOCK;
                     }                     
                     else
                     {
                        //- LOCKED(pSMBPBPara->SModeRW[EMode].EStateRx= MBPB_STATE_RX_IDLE);
                     }
                     break;
                     
                  case MBPB_STATE_RX_IDLE:
                     // Lock SBufferRW[MBPB_BUFFER_RAW] (pend infinite until unlocked)
                     if (SSEM_LOCKUSR(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW], OS_T_FOREVER))
                     {
                        if (ucByte == ':')
                        {
                           *pucPrevIdx= 0;
                           LOCK;
                           {
                              pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos= 0;
                              pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= 0;                                       
                              pSMBPBPara->SModeRW[EMode].EBytePos= MBPB_BYTE_HIGH_NIBBLE;
                              
                              pSMBPBPara->SModeRW[EMode].EStateRx= MBPB_STATE_RX_RCV;
                           }
                           UNLOCK;
                        }
                        
                        SSEM_UNLOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]);
                     }
                     break;
                     
                  default:
                     // Debug
                     EStateRx= EStateRx;
                     break;
               }
               
               // Store last two (cooked) bytes
               pucPrevAr[*pucPrevIdx]= ucByte; 
               *pucPrevIdx= (*pucPrevIdx+ 1) % 2;
            }
            else if (EMode == MBPB_MODE_RTU)
            {
               // RTU
               
               switch (EStateRx)
               {
                  case MBPB_STATE_RX_INIT:
                     break;
                     
                  case MBPB_STATE_RX_ERROR:
                     break;
                     
                  case MBPB_STATE_RX_IDLE:
                     // Lock SBufferRW[MBPB_BUFFER_RAW] (pend infinite until unlocked)
                     if (SSEM_LOCKUSR(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW], OS_T_FOREVER))
                     {
                        LOCK;
                        {
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos= 0;
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= 0;
                                                                 
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos]= ucByte;
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos+ 1;
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt+ 1;
                           
                           pSMBPBPara->SModeRW[EMode].EStateRx= MBPB_STATE_RX_RCV;
                        }
                        UNLOCK;
                        
                        SSEM_UNLOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]);
                     }
                     break;
                     
                  case MBPB_STATE_RX_RCV:
                     if (usCnt <= MBPB_SER_PDU_SIZE_MAX)
                     {
                        LOCK;
                        {
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos]= ucByte;
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos+ 1;
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt+ 1;
                        }
                        UNLOCK;
                     }
                     else
                     {
                        LOCKED(pSMBPBPara->SModeRW[EMode].EStateRx= MBPB_STATE_RX_ERROR);
                     }            
                     break;
                     
                  default:
                     // Debug
                     EStateRx= EStateRx;
                     break;
               }
            }           
         }
      }
            
      SSEM_UNLOCK(SSemLock, pSMBPBPara->SModeRW[EMode]);         
   }
   
   // Send receive msg immediately
   if (bForceTo)
   {
      MBPBSerNotifyTo(pSMBPBPara, 1);
   }
}

//----------------------------------------------------------------------------
// void MBPBSerNotifyTxDone
//----------------------------------------------------------------------------
void MBPBSerNotifyTxDone(struct _SMBPBPara *pSMBPBPara, USHORT usCount)
{
   USHORT usCnt;
   USHORT usPos;
   EMBPBMode EMode;
   EMBPBStateTx EStateTx;

   LOCK;
   {
      // Safely access EMode and update temporary local copy
      EMode= pSMBPBPara->SPortRW.EMode;
      
      // Safely access EStateTx and update temporary local copy
      EStateTx= pSMBPBPara->SModeRW[EMode].EStateTx;
   }
   UNLOCK;
      
   if (EStateTx == MBPB_STATE_TX_XMIT)
   {
      LOCK;
      {
         // Safely access usPos and update temporary local copy
         usPos= pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usPos;
         
         // Safely access usCnt and update temporary local copy
         usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usCnt;
      }
      UNLOCK;
         
      // Notify only full frame tx done message (ignore partial tx done for driver block with too slow refill)
      // Both of usPos and usCnt are set to zero when driver tx was successful
      if (usPos >= usCnt)
      {
         LOCKED(pSMBPBPara->SModeRW[EMode].EStateTx= MBPB_STATE_TX_IDLE);
         SMBX_SEND(SMbxEvent, pSMBPBPara->SModeRO[EMode], MBPB_EVT_FRAME_SENT);
      }
   }
}

//----------------------------------------------------------------------------
// void MBPBSerNotifyTo
//----------------------------------------------------------------------------
void MBPBSerNotifyTo(struct _SMBPBPara *pSMBPBPara, USHORT usCount)
{
   MBPBSerNotifyExTo(pSMBPBPara, usCount, TRUE);
}

//----------------------------------------------------------------------------
// void MBPBSerNotifyExTo
//----------------------------------------------------------------------------
void MBPBSerNotifyExTo(struct _SMBPBPara *pSMBPBPara, USHORT usCount, bool bStateChange)
{
   ULONG ulRepeat;
   EMBPBMode EMode;
   
   OS_RESULT oResult= OS_R_TMO;

   for (ulRepeat= 0; ulRepeat < usCount; ulRepeat++)
   {
      // Safely access EMode and update temporary local copy
      LOCKED(EMode= pSMBPBPara->SPortRW.EMode);

      // Lock - Safely access and modify EStateRx
      if (SSEM_LOCK(SSemLock, pSMBPBPara->SModeRW[EMode]))
      {
         if (EMode == MBPB_MODE_ASC)
         {
            // ASC
            
            switch (pSMBPBPara->SModeRW[EMode].EStateRx)
            {
               case MBPB_STATE_RX_INIT:
                  // Timer t1s expired: startup phase is finished  
                  oResult= SMBX_SEND(SMbxEvent, pSMBPBPara->SModeRO[EMode], MBPB_EVT_READY);
                  break;
                  
               case MBPB_STATE_RX_RCV:
                  // Go back to the idle state and wait for the next frame
                  oResult= SMBX_SEND(SMbxEvent, pSMBPBPara->SModeRO[EMode], MBPB_EVT_READY);
                  //-oResult= OS_R_OK;
                  break;
                  
               case MBPB_STATE_RX_WAIT_EOF:
                  // Go back to the idle state and wait for the next frame
                  oResult= SMBX_SEND(SMbxEvent, pSMBPBPara->SModeRO[EMode], MBPB_EVT_READY);
                  //-oResult= OS_R_OK;                 
                  break;
                  
               case MBPB_STATE_RX_COMPLETE:
                  // A frame was completely received and t35 expired. Notify the listener that a new frame was received
                  
                  // Lock SBufferRW[MBPB_BUFFER_RAW]
                  if (SSEM_LOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]))
                  {
                     oResult= SMBX_SEND(SMbxEvent, pSMBPBPara->SModeRO[EMode], MBPB_EVT_FRAME_RECEIVED);
                  }                  
                  break;
               
               default:
                  break;
            }               
         }
         else if (EMode == MBPB_MODE_RTU)
         {
            // RTU
            
            switch (pSMBPBPara->SModeRW[EMode].EStateRx)
            {
               case MBPB_STATE_RX_INIT:
                  // Timer t35 expired: startup phase is finished (required for rtu by spec)
                  oResult= SMBX_SEND(SMbxEvent, pSMBPBPara->SModeRO[EMode], MBPB_EVT_READY);
                  break;
                  
               case MBPB_STATE_RX_RCV:
                  // A frame was received and t35 expired. Notify the listener that a new frame was received
                  // Lock SBufferRW[MBPB_BUFFER_RAW]
                  if (SSEM_LOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]))
                  {
                     oResult= SMBX_SEND(SMbxEvent, pSMBPBPara->SModeRO[EMode], MBPB_EVT_FRAME_RECEIVED);
                  }
                  break;
                  
               case MBPB_STATE_RX_ERROR:
                  // An error occured while receiving the frame
                  break;
                  
               default:
                  // Function called in an illegal state
                  break;
            }
         }
         
         if (bStateChange && (oResult == OS_R_OK))
         {
            pSMBPBPara->SModeRW[EMode].EStateRx= MBPB_STATE_RX_IDLE;
         }
         
         SSEM_UNLOCK(SSemLock, pSMBPBPara->SModeRW[EMode]);
      }
   }
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBSerParserVerify
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBSerParserVerify(struct _SMBPBPara *pSMBPBPara)
{
   USHORT usCnt;
   EMBPBMode EMode;
   EMBPBStateRx EStateRx;
   
   EMBPBErrorCode EErrorCode= MBPB_EILLSTATE;
  
   LOCK;
   {
      // Safely access EMode and update temporary local copy
      EMode= pSMBPBPara->SPortRW.EMode;
       
      // Safely access EStateRx and update temporary local copy
      EStateRx= pSMBPBPara->SModeRW[EMode].EStateRx;
   }
   UNLOCK;
   
   if (EStateRx == MBPB_STATE_RX_IDLE)
   {
      // Safely access usCnt and update temporary local copy
      LOCKED(usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt);
      
      switch (EMode)
      {
         case MBPB_MODE_ASC:
            if (usCnt >= MBPB_ASC_PDU_SIZE_MIN)
            {
               if (MBPBLRC(pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData, usCnt) == 0)
               {
                  EErrorCode= MBPB_ENOERR;
               }
            }
            break;
            
         case MBPB_MODE_RTU:
            if (usCnt >= MBPB_RTU_PDU_SIZE_MIN)
            {
               if (MBPBCRC16(pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData, usCnt) == 0)
               {
                  EErrorCode= MBPB_ENOERR;
               }
            }
            
            break;
      }
   }
   
   return (EErrorCode);
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBSerParserCook
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBSerParserCook(struct _SMBPBPara *pSMBPBPara)
{
   UCHAR *pucRaw;
   UCHAR *pucCooked;
   
   UCHAR ucLRC;
   USHORT usCRC;
   USHORT usRawCnt;
   USHORT usRawIdx, usCookedIdx;
   
   EMBPBMode EMode;
   EMBPBStateRx EStateRx;
   EMBPBStateTx EStateTx;
   EMBPBBytePos EBytePos;
   
   UCHAR ucRxCnt= 0;
   EMBPBErrorCode EErrorCode= MBPB_EILLSTATE;
  
   LOCK;
   {
      // Safely access EMode and update temporary local copy
      EMode= pSMBPBPara->SPortRW.EMode;
       
      // Safely access EStateRx and update temporary local copy
      EStateRx= pSMBPBPara->SModeRW[EMode].EStateRx;
   
      // Safely access EStateRx and update temporary local copy
      EStateTx= pSMBPBPara->SModeRW[EMode].EStateTx;
   }
   UNLOCK;
        
   // Rx Overrun check - read and keep counter unchanged for rx message bit
   if (pSMBPBPara->SPortRO.SDriver.Msg) ucRxCnt= pSMBPBPara->SPortRO.SDriver.Msg(pSMBPBPara->SPortRO.SDriver.SConfig.ucUnit, pSMBPBPara->SPortRO.SDriver.SConfig.ucChannel, pSMBPBPara->SPortRO.ulMsgBitRx, FALSE, pSMBPBPara->SPortRO.SDriver.SConfig.u16To);
   
   if ((EStateRx == MBPB_STATE_RX_IDLE) && (EStateTx == MBPB_STATE_TX_IDLE) && (!ucRxCnt))
   {      
      if (SSEM_LOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED]))
      {        
         pucRaw= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData;
         pucCooked= pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].pucData;

         // Finalize raw data and calculate checksum for raw data (common)
                  
         LOCKED(usRawCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt);

         switch (EMode)
         {
            case MBPB_MODE_ASC:
               // Finalize raw data and calculate checksum for raw data
                                             
               // Exclude the checksum from checksum calculation (usCnt includes it already with MBPB_ASC_PDU_SIZE_LRC!)
               ucLRC= MBPBLRC(pucRaw, usRawCnt- MBPB_ASC_PDU_SIZE_LRC);
               
               // Store the calculated checksum
               *(pucRaw+ usRawCnt-1)= ucLRC;
               
               // Cook the finalized raw data into the cooked buffer space
               
               pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usCnt= 0;
               pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usPos= 0;
                              
               // Header
               *pucCooked= ':';
               
               // Data and checksum
               EBytePos= MBPB_BYTE_HIGH_NIBBLE;
               
               for (usCookedIdx= MBPB_SER_PDU_PDU_OFF, usRawIdx= 0; (usRawIdx < usRawCnt) && (usCookedIdx < MBPB_SER_PDU_SIZE_MAX); )
               {
                  switch (EBytePos)
                  {
                     case MBPB_BYTE_HIGH_NIBBLE:
                        *(pucCooked+ usCookedIdx++)= MBPBBin2Char(*(pucRaw+ usRawIdx) >> 4);
                        
                        EBytePos= MBPB_BYTE_LOW_NIBBLE;
                        break;
                        
                     case MBPB_BYTE_LOW_NIBBLE:
                        *(pucCooked+ usCookedIdx++)= MBPBBin2Char(*(pucRaw+ usRawIdx++) & 0x0F);
                        
                        EBytePos= MBPB_BYTE_HIGH_NIBBLE;
                        break;
                  }
               }

               LOCK;
               {
                  *(pucCooked+ usCookedIdx++)= pSMBPBPara->SModeRW[EMode].ucCR;
                  *(pucCooked+ usCookedIdx++)= pSMBPBPara->SModeRW[EMode].ucLF;
               }
               UNLOCK;
                  
               pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usCnt= usCookedIdx;
               pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usPos= usCookedIdx;
               
               EErrorCode= MBPB_ENOERR;
               
               break;
               
            case MBPB_MODE_RTU:
               // Finalize raw data and calculate checksum for raw data
               
               // Copy raw to cooked buffer
               memcpy(pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].pucData, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData, usRawCnt);
               pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usPos= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos;
               pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt;
               
               // Exclude the checksum from checksum calculation (usCnt includes it already with MBPB_RTU_PDU_SIZE_CRC!)
               usCRC= MBPBCRC16(pucCooked, usRawCnt- MBPB_RTU_PDU_SIZE_CRC);
               *(pucCooked+ (usRawCnt- 2))= usCRC & 0xFF;
               *(pucCooked+ (usRawCnt- 1))= usCRC >> 8;
               
               EErrorCode= MBPB_ENOERR;
             
               break;
         }
      }
   }
   
   return (EErrorCode);
}

//----------------------------------------------------------------------------
// USHORT MBPBSerEncode
//----------------------------------------------------------------------------
USHORT MBPBSerEncode(EMBPBMode EMode, CHAR ucCR, UCHAR ucLF, UCHAR *pucRaw, USHORT usRawCnt, UCHAR *pucCooked, USHORT usCookedLen)
{
   USHORT usCookedCnt= 0;
   
   USHORT usRawIdx, usCookedIdx;
   USHORT usCRC;
   UCHAR ucLRC;
   
   EMBPBBytePos EBytePos;
   
   if (ucCR && ucLF && pucRaw && usRawCnt && pucCooked && usCookedLen)
   {
      // Finalize raw data and calculate checksum for raw data (common)
               
      switch (EMode)
      {
         case MBPB_MODE_ASC:
            // Finalize raw data and calculate checksum for raw data
                                          
            // Exclude the checksum from checksum calculation (usCnt includes it already with MBPB_ASC_PDU_SIZE_LRC!)
            ucLRC= MBPBLRC(pucRaw, usRawCnt- MBPB_ASC_PDU_SIZE_LRC);
            
            // Store the calculated checksum
            *(pucRaw+ usRawCnt- 1)= ucLRC;
            
            // Cook the finalized raw data into the cooked buffer space
                                     
            // Header
            *pucCooked= ':';
            
            // Data and checksum
            EBytePos= MBPB_BYTE_HIGH_NIBBLE;
            
            for (usCookedIdx= MBPB_SER_PDU_PDU_OFF, usRawIdx= 0; (usRawIdx < usRawCnt) && (usCookedIdx < usCookedLen); )
            {
               switch (EBytePos)
               {
                  case MBPB_BYTE_HIGH_NIBBLE:
                     *(pucCooked+ usCookedIdx++)= MBPBBin2Char(*(pucRaw+ usRawIdx) >> 4);
                     
                     EBytePos= MBPB_BYTE_LOW_NIBBLE;
                     break;
                     
                  case MBPB_BYTE_LOW_NIBBLE:
                     *(pucCooked+ usCookedIdx++)= MBPBBin2Char(*(pucRaw+ usRawIdx++) & 0x0F);
                     
                     EBytePos= MBPB_BYTE_HIGH_NIBBLE;
                     break;
               }
            }
   
            LOCK;
            {
               *(pucCooked+ usCookedIdx++)= ucCR;
               *(pucCooked+ usCookedIdx++)= ucLF;
            }
            UNLOCK;
               
            usCookedCnt= usCookedIdx;
            
            break;
            
         case MBPB_MODE_RTU:
            // Finalize raw data and calculate checksum for raw data
            
            // Copy raw to cooked buffer
            memcpy(pucCooked, pucRaw, usRawCnt);
            usCookedCnt= usRawCnt;
            
            // Exclude the checksum from checksum calculation (usCnt includes it already with MBPB_RTU_PDU_SIZE_CRC!)
            usCRC= MBPBCRC16(pucCooked, usRawCnt- MBPB_RTU_PDU_SIZE_CRC);
            *(pucCooked+ (usRawCnt- 2))= usCRC & 0xFF;
            *(pucCooked+ (usRawCnt- 1))= usCRC >> 8;
                      
            break;
      }
   }
      
   return (usCookedCnt);
}

//----------------------------------------------------------------------------
// USHORT MBPBSerDecode
//----------------------------------------------------------------------------
USHORT MBPBSerDecode(EMBPBMode EMode, CHAR ucCR, UCHAR ucLF, UCHAR *pucCooked, USHORT usCookedCnt, UCHAR *pucRaw, USHORT usRawLen)
{
   USHORT usResult= 0;
   USHORT usCnt= 0;
   EMBPBStateRx EStateRx= MBPB_STATE_RX_RCV;
   
   ULONG ulRepeat;
   UCHAR ucResult;
   UCHAR ucByte;
   
   EMBPBBytePos EBytePos;
   
   if (ucCR && ucLF && pucRaw && usRawLen && pucCooked && usCookedCnt)
   {                       
      for (ulRepeat= 0; ulRepeat < usCookedCnt; ulRepeat++)
      {
         ucByte= *(pucCooked+ ulRepeat);
                             
         if (EMode == MBPB_MODE_ASC)
         {
            // ASC
                       
            switch (EStateRx)
            {
               case MBPB_STATE_RX_RCV:
                  if (ucByte == ':')
                  {
                     usCnt= 0;
                     EBytePos= MBPB_BYTE_HIGH_NIBBLE;
                  }
                  else if(ucByte == ucCR)
                  {                     
                     EStateRx= MBPB_STATE_RX_WAIT_EOF;
                  }
                  else
                  {
                     ucResult= MBPBChar2Bin(ucByte);
                     
                     switch (EBytePos)
                     {
                        case MBPB_BYTE_HIGH_NIBBLE:
                           if (usCnt <= MBPB_SER_PDU_SIZE_MAX)
                           {
                              *(pucRaw+ usCnt)= (UCHAR)(ucResult << 4);
                              EBytePos= MBPB_BYTE_LOW_NIBBLE;
                           }
                           else
                           {
                              // Not handled in Modbus specification but seems a resonable implementation
                              EStateRx= MBPB_STATE_RX_IDLE;
                           }
                           break;
                           
                        case MBPB_BYTE_LOW_NIBBLE:
                           *(pucRaw+ usCnt) |= ucResult;
                           usCnt++;
                           EBytePos= MBPB_BYTE_HIGH_NIBBLE;
                           break;
                     }
                  }
                  break;
                  
               case MBPB_STATE_RX_WAIT_EOF:
                  if (ucByte == ucLF)
                  {
                     EStateRx= MBPB_STATE_RX_COMPLETE;
                  }
                  else if (ucByte == ':')
                  {
                     usCnt= 0;
                     EBytePos= MBPB_BYTE_HIGH_NIBBLE;
                     EStateRx= MBPB_STATE_RX_RCV;
                  }                     
                  else
                  {
                     //- EStateRx= MBPB_STATE_RX_IDLE;
                  }
                  break;
                  
               case MBPB_STATE_RX_IDLE:
                  if (ucByte == ':')
                  {
                     usCnt= 0;
                     EBytePos= MBPB_BYTE_HIGH_NIBBLE;
                     EStateRx= MBPB_STATE_RX_RCV;
                  }
                  break;
            }
         }
         else if (EMode == MBPB_MODE_RTU)
         {
            // RTU
            
            switch (EStateRx)
            {
               case MBPB_STATE_RX_INIT:
                  break;
                  
               case MBPB_STATE_RX_ERROR:
                  break;
                  
               case MBPB_STATE_RX_IDLE:
                  usCnt= 0;
                  
                  *(pucRaw+ usCnt)= ucByte;
                  usCnt++;
                  EStateRx= MBPB_STATE_RX_RCV;
                  
                  break;
                  
               case MBPB_STATE_RX_RCV:
                  if (usCnt <= MBPB_SER_PDU_SIZE_MAX)
                  {
                     *(pucRaw+ usCnt)= ucByte;
                     usCnt++;
                  }
                  else
                  {
                     EStateRx= MBPB_STATE_RX_ERROR;
                  }            
                  break;
            }
         }           
      }
   }
   
   // Verify checksum 
   if (EMode == MBPB_MODE_ASC)
   {     
      if (EStateRx == MBPB_STATE_RX_COMPLETE)
      {
         usResult= (MBPBLRC(pucRaw, usCnt) == 0) ? (usCnt) : (0);
      }
   }
   else if (EMode == MBPB_MODE_RTU)
   {
      usResult= (EStateRx == MBPB_STATE_RX_RCV) ? (usCnt) : (0);
      
      if (EStateRx == MBPB_STATE_RX_RCV)
      {
         usResult= (MBPBCRC16(pucRaw, usCnt) == 0) ? (usCnt) : (0);
      }
   }
   
   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT MBPBSerCookedReplyBytes
//----------------------------------------------------------------------------
USHORT MBPBSerCookedReplyBytes(EMBPBMode EMode, UCHAR *pucRaw, USHORT usRawLen)
{
   USHORT usResult= 0;
   
   USHORT usCount;
   USHORT usSize;
   UCHAR ucCode;
   
   if (pucRaw && usRawLen)
   {
      ucCode= *(pucRaw+ MBPB_SER_PDU_PDU_OFF);
      
      if (ucCode != MBPB_CODE_NONE)
      {
         if (EMode == MBPB_MODE_ASC)
         {
            usSize= 2; // ASC requires single transfer byte per data nibble (':', CR and LF require only one transmission byte each)
            usResult= 1+ (MBPB_ASC_PDU_SIZE_LRC* usSize)+ 1+ 1; // ':'(1)+ LRC(2)+ CR(1)+ LF(1)
         }
         else if (EMode == MBPB_MODE_RTU)
         {
            usSize= 1; // RTU requires single transfer byte per data byte
            usResult= MBPB_RTU_PDU_SIZE_CRC; // CRC(2)
         }
         
         if (usSize)
         {
            switch (ucCode)
            {
               case MBPB_CODE_READ_COILS:
               case MBPB_CODE_READ_DISCRETE_INPUTS:
                  // Count (big endian!) after SADR, FUNC (offset= 2)
                  usCount= (USHORT) (*((pucRaw+ MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 2)) << 8);
                  usCount|= (USHORT) (*((pucRaw+ MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 2+ 1)));
                  
                  // 8 coils covered by a single data byte
                  usResult+= (usCount% 8) ? (((usCount/ 8)+ 1)* usSize) : ((usCount/ 8)* usSize);
                  
                  // + [SADR+ FUNC]+ CNT
                  usResult+= ((MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 1)* usSize);
                  
                  break;
                  
               case MBPB_CODE_READ_HOLDING_REGISTER:
               case MBPB_CODE_READ_INPUT_REGISTER:
                  // Count (big endian!) after SADR, FUNC (offset= 2)
                  usCount= (USHORT) (*((pucRaw+ MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 2)) << 8);
                  usCount|= (USHORT) (*((pucRaw+ MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 2+ 1)));
                  
                  // Single register requires two data bytes
                  usResult+= (usCount* 2* usSize);
                  
                  // + [SADR+ FUNC]+ CNT
                  usResult+= ((MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 1)* usSize);
                  
                  break;
                  
               case MBPB_CODE_READ_GENERAL_REFERENCE:
                  // Count (big endian!) after SADR, FUNC, CNT, REF, ADR32(4) (offset= 8)
                  usCount= (USHORT) (*((pucRaw+ MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 8)) << 8);
                  usCount|= (USHORT) (*((pucRaw+ MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 8+ 1)));
                  
                  // Single register requires two data bytes
                  usResult+= (usCount* 2* usSize);
                  
                  // + [SADR+ FUNC]+ CNT+ SUBCNT+ REF
                  usResult+= ((MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 1+ 1+ 1)* usSize);
               
                  break;
                              
               case MBPB_CODE_WRITE_SINGLE_COIL:
               case MBPB_CODE_WRITE_REGISTER:
               case MBPB_CODE_WRITE_MULTIPLE_COILS:
               case MBPB_CODE_WRITE_MULTIPLE_REGISTERS:
                  // + ADR16(2)+ DATA(2)/CNT(2)
                  usResult+= (4* usSize);
                  
                  // + [SADR+ FUNC]
                  usResult+= ((MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF)* usSize);
                  
                  break;
                  
               case MBPB_CODE_WRITE_GENERAL_REFERENCE:
                  // Count (big endian!) after SADR, FUNC, CNT, REF, ADR32(4) (offset= 8)
                  usCount= (USHORT) (*((pucRaw+ MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 8)) << 8);
                  usCount|= (USHORT) (*((pucRaw+ MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 8+ 1)));
                  
                  // Single register requires two data bytes
                  usResult+= (usCount* 2* usSize);
                  
                  // + [SADR+ FUNC]+ CNT+ REF+ ADR32(4)+ REGCNT(2)
                  usResult+= ((MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 1+ 1+ 4+ 2)* usSize);
               
                  break;
                  
               case MBPB_CODE_OTHER_REPORT_SLAVEID:
                  // + [SADR+ FUNC+ CNT+ SLAVEID+ STATUS]+ LENGTH(x)+ CLASS(4)
                  usResult+= ((5+ MBPB_PRODUCT_DEVICE_LENGTH+ sizeof(ULONG))* usSize);
                  
                  break;
                  
               case MBPB_CODE_READWRITE_MULTIPLE_REGISTERS:
                  // Count (big endian!) after SADR, FUNC (offset= 2)
                  usCount= (USHORT) (*((pucRaw+ MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 2)) << 8);
                  usCount|= (USHORT) (*((pucRaw+ MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 2+ 1)));
                  
                  // Single register requires two data bytes
                  usResult+= (usCount* 2* usSize);
                  
                  // + [SADR+ FUNC]+ CNT
                  usResult+= ((MBPB_SER_PDU_PDU_OFF+ MBPB_PDU_DATA_OFF+ 1)* usSize);
                  
                  break;
            }
         }
      }
   }
   
   return (usResult);
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBSerParserEnable
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBSerParserEnable(struct _SMBPBPara *pSMBPBPara)
{
   return (MBPBSerParserChange(pSMBPBPara, TRUE));
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBSerParserDisable
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBSerParserDisable(struct _SMBPBPara *pSMBPBPara)
{
   return (MBPBSerParserChange(pSMBPBPara, FALSE));
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBSerParserChange
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBSerParserChange(struct _SMBPBPara *pSMBPBPara, BOOL bEnable)
{ 
   EMBPBMode EMode;
   
   EMBPBErrorCode EErrorCode= MBPB_ENOERR;

   // Safely access EMode and update temporary local copy
   LOCKED(EMode= pSMBPBPara->SPortRW.EMode);

   // Lock - Safely access and modify EState
   if (SSEM_LOCK(SSemLock, pSMBPBPara->SModeRW[EMode]))
   {  
      if (pSMBPBPara->SModeRW[EMode].EState == (bEnable) ? (MBPB_STATE_DISABLED) : (MBPB_STATE_ENABLED))
      {
         if ((bEnable) ? (pSMBPBPara->SModeRO[EMode].SParser.Start) : (pSMBPBPara->SModeRO[EMode].SParser.Stop))
         {
            EErrorCode= (bEnable) ? (pSMBPBPara->SModeRO[EMode].SParser.Start(pSMBPBPara)) : (pSMBPBPara->SModeRO[EMode].SParser.Stop(pSMBPBPara));
            if (EErrorCode == MBPB_ENOERR)
            {
               pSMBPBPara->SModeRW[EMode].EState= (bEnable) ? (MBPB_STATE_ENABLED) : (MBPB_STATE_DISABLED);
            }
         }
         else
         {
            EErrorCode= MBPB_ENORES;
         }
      }
      else
      {
         EErrorCode= MBPB_EILLSTATE;
      }
      
      SSEM_UNLOCK(SSemLock, pSMBPBPara->SModeRW[EMode]);
   }
   else
   {
      EErrorCode= MBPB_ENORES;
   }      
      
   return (EErrorCode);
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBSerParserStart
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBSerParserStart(struct _SMBPBPara *pSMBPBPara)
{  
   //return (MBPBMsgCtrl(pSMBPBPara, TRUE, FALSE));
   return (MBPBMsgCtrl(pSMBPBPara, TRUE, TRUE));
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBSerParserStop
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBSerParserStop(struct _SMBPBPara *pSMBPBPara)
{
   return (MBPBMsgCtrl(pSMBPBPara, FALSE, FALSE));
}

//----------------------------------------------------------------------------
// bool MBPBSerParserMaximum
//----------------------------------------------------------------------------
bool MBPBSerParserMaximum(EMBPBMode EMode, EMBPBRegType ERegType, USHORT usCountRegType, USHORT usCountRegType2, EMBPBRegRW ERW, USHORT usRequestBytesMax, USHORT usReplyBytesMax, USHORT *pusCountRegTypeMax, USHORT *pusCountRegTypeMax2)
{
	bool bResult= FALSE;

	USHORT *pusCountRegTypeMaxWrite= NULL;

	//-USHORT usCountRegTypeRead;
	//-USHORT usCountRegTypeWrite;
	USHORT usCountRegTypeReadMax;
	USHORT usCountRegTypeWriteMax;

	UCHAR ucLenStart;
	UCHAR ucLenAddress;
	UCHAR ucLenFunction;
	UCHAR ucLenCheck;
	UCHAR ucLenEnd;
	UCHAR ucLenDataByte;
	USHORT usLenDataRequest;
	USHORT usLenDataReply;
	USHORT usLenFrame;

	if ((EMode < MBPB_MODE_COUNT) && (ERW < MBPB_REGRW_COUNT))
	{
		// Determine frame components length
		switch (EMode)
		{
			case MBPB_MODE_ASC:	// two ascii character per binary byte (except start byte)
				ucLenStart= 1;
				ucLenAddress= 2;
				ucLenFunction= 2;
				ucLenCheck= 2;
				ucLenEnd= 2;
				ucLenDataByte= 2;
				break;

			case MBPB_MODE_RTU:
				ucLenStart= 0;		// indication using time-out
				ucLenAddress= 1;
				ucLenFunction= 1;
				ucLenCheck= 2;		// 16 bit crc
				ucLenEnd= 0;		// indication using time-out
				ucLenDataByte= 1;
				break;

			default:
				ucLenStart= 0;
				ucLenAddress= 0;
				ucLenFunction= 0;
				ucLenCheck= 0;
				ucLenEnd= 0;
				ucLenDataByte= 0;
				break;
		}

		usLenFrame= ucLenStart+ ucLenAddress+ ucLenFunction+ ucLenCheck+ ucLenEnd;

		if ((ERegType < MBPB_REGTYPE_COUNT) && (usRequestBytesMax > (usLenFrame+ usLenDataRequest)) && (usReplyBytesMax > (usLenFrame+ usLenDataReply)))
		{
         // Determine minimum required buffer space for request and reply

         // Special register type general reference
         if (ERegType == MBPB_REGTYPE_GENERAL)
         {
            if (ERW == MBPB_REGRW_READ)
            {
               usLenDataRequest= 8; // byte count total, reference type, address hihi, address hilo, address lohi, address lolo, item count hi, item count lo
               usLenDataReply= 3; // byte count total, byte count sub-request, reference type
            }
            else if (ERW == MBPB_REGRW_WRITE)
            {
               usLenDataRequest= 8; // byte count total, reference type, address hihi, address hilo, address lohi, address lolo, item count hi, item count lo
               usLenDataReply= 8; // byte count total, reference type, address hihi, address hilo, address lohi, address lolo, item count hi, item count lo
            }
         }
         // All common basic register types
         else
         {
            if ((ERW == MBPB_REGRW_READ) || (ERW == MBPB_REGRW_READWRITE))
            {
               usLenDataRequest= 4; // address hi, address lo, item count hi, item count lo
               usLenDataReply= 3; // byte count, data hi, data lo
            }
            else if (ERW == MBPB_REGRW_WRITE)
            {
               usLenDataRequest= 4+ (usCountRegType != 0); // address hi, address lo, item count hi, item count lo, byte count (for multiple write)
               usLenDataReply= 4; // address hi, address lo, data hi / item count hi, data lo / item count lo
            }

            if (ERW == MBPB_REGRW_READWRITE)
            {
               usLenDataRequest += 4+ (usCountRegType != 0); // address hi, address lo, item count hi, item count lo, byte count (for multiple write)
               usLenDataReply= 3; // byte count, data hi, data lo
            }
         }
      
			// Standard reads require one CountType, combined read/writes require two CountTypes
			if ((((ERW == MBPB_REGRW_READ) || (ERW == MBPB_REGRW_WRITE)) && (pusCountRegTypeMax)) || ((ERW == MBPB_REGRW_READWRITE) && (pusCountRegTypeMax) && (pusCountRegTypeMax2)))
			{
				if ((ERW == MBPB_REGRW_READ) || (ERW == MBPB_REGRW_READWRITE))
				{
					bResult= TRUE;
					usLenDataReply= 1; // byte count

					// Determine usCountRegTypeReadMax for allowed frame size of REPLY (count dependend size)
					switch (ERegType)
					{
						case MBPB_REGTYPE_INPUT:
						case MBPB_REGTYPE_HOLDING:
							usCountRegTypeReadMax= ((usReplyBytesMax- (usLenFrame+ usLenDataReply))/ ucLenDataByte)/ sizeof(USHORT);
							break;

						case MBPB_REGTYPE_COILS:
						case MBPB_REGTYPE_DISCRETE:
							usCountRegTypeReadMax= ((usReplyBytesMax- (usLenFrame+ usLenDataReply))/ ucLenDataByte)* BITS_OF(UCHAR);
							break;
                     
                  case MBPB_REGTYPE_GENERAL:
                     usCountRegTypeReadMax= ((usReplyBytesMax- (usLenFrame+ usLenDataReply))/ ucLenDataByte)/ sizeof(USHORT);
                     break;
/*
						case MBPB_REGTYPE_SLAVEID:
							usCountRegTypeReadMax= 1;
							break;
*/
						default:
							bResult= FALSE;
							break;
					}

					//-usCountRegTypeRead= _min(usCountRegTypeReadMax, usCountRegType);

					if (pusCountRegTypeMax)
					{
						*pusCountRegTypeMax= usCountRegTypeReadMax;
					}
					else
					{
						bResult= FALSE;
					}
				}
				
				if ((ERW == MBPB_REGRW_WRITE) || (ERW == MBPB_REGRW_READWRITE))
				{
					if (ERW == MBPB_REGRW_WRITE)
					{
						// Normal write (use buffer from top)

						//-usCountRegTypeWrite= usCountRegType;
						pusCountRegTypeMaxWrite= pusCountRegTypeMax;
					}
					else
					{
						// Combined write after read data (use available buffer past read data)

						//-usCountRegTypeWrite= usCountRegType2;
						pusCountRegTypeMaxWrite= pusCountRegTypeMax2;
					}

					bResult= TRUE;
					
					// Determine usCountRegTypeWriteMax for allowed frame size of REQUEST (count dependend size)
					switch (ERegType)
					{
						case MBPB_REGTYPE_HOLDING:
							usCountRegTypeWriteMax= ((usRequestBytesMax- (usLenFrame+ usLenDataRequest))/ ucLenDataByte)/ sizeof(USHORT);
							break;

						case MBPB_REGTYPE_COILS:
							usCountRegTypeWriteMax= ((usRequestBytesMax- (usLenFrame+ usLenDataRequest))/ ucLenDataByte)* BITS_OF(UCHAR);
							break;
                     
                  case MBPB_REGTYPE_GENERAL:
                     usCountRegTypeWriteMax= ((usRequestBytesMax- (usLenFrame+ usLenDataRequest))/ ucLenDataByte)/ sizeof(USHORT);
                     break;

						default:
							bResult= FALSE;
							break;
					}

					if (pusCountRegTypeMaxWrite)
					{
						*pusCountRegTypeMaxWrite= usCountRegTypeWriteMax;
					}
					else
					{
						bResult= FALSE;
					}
				}
			}
		}
	}

	return (bResult);
}
