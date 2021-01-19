//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/GPIO/MCP2317/MCP2317.C
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-12-18 initial version
// 2009-12-21 Mcp2317Init now using on mcp2317dev.h only
// 2009-12-22 added MCP2317_UNIT_MAX checks
// 2009-12-23 using easy data ignore feature of SpiRxTag now
// 2009-12-23 Mcp2317Init less stack consumption by data ignore feature
// 2009-12-23 Mcp2317SetOutput less stack consumption by data ignore feature
// 2009-12-23 TskNotifyMcp2317 less stack consumption by lowering tx buffer
// 2009-12-23 optimized message loops to avoid non successful calls of SpiRx
// 2010-01-29 fixed ucResult of Mcp2317Init
// 2010-01-29 added checking for valid (external initialized) oiSemMsgNew
// 2010-02-05 fixed safety checking of oiMsgNewSem in Mcp2317Init
// 2010-02-17 Mcp2317Init fixed zeroing of GPIOA/GPIOB by zeroing OLATA/OLATB
// 2010-02-18 added param u16Timeout to Mcp2317SetOutput
// 2010-02-18 added update of SemMcp2317Reg by reading of TskNotifyMcp2317
// 2010-02-18 completed checking of SpiTx(Tag) result
// 2010-02-26 Mcp2317SetOutput now returns ((usResult != 0)* ResultPin)
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-09 changed type of DataRxLen to USHORT in TskNotifyMcp2317
// 2010-09-09 fixed giving of SemMcp2317Reg[] in Mcp2317Init
// 2010-09-10 fixed again giving of SemMcp2317Reg[] in Mcp2317Init
// 2010-09-30 added piobase para to Mcp2317IrqHandler
// 2010-10-22 optimized rx data processing in Mcp2317Init/SetOutput
// 2010-10-22 optimized rx data processing in TskNotifyMcp2317
// 2010-12-02 added bIrq flag to ...Init, ...IrqHandler and TskNotifyMcp2317
// 2010-12-09 fixed data rx processing in Mcp2317SetOutput
// 2011-02-03 fixed possible deadlock for rx data in Mcp2317SetOutput
// 2011-02-16 added semaphore SemStart to lock the busy device in all functions
// 2011-02-16 changed TskNotifyMcp2317 to support calls to mcp2317 from notifies
// 2011-02-17 changed Mcp2317SetOutput to ignore unchanged output
// 2011-03-04 added RESCHEDULE(0) after taking oiSemMsgNew in TskNotifyMcp2317
// 2011-03-09 added reschedule call after os_sem_take of SemMsgNew in Mcp2317Init
// 2011-03-09 removed checking of AT91C_SPI_TXBUFE messages in Mcp2317Init
// 2011-03-09 changed data rx condition to verify data tag also in Mcp2317Init
// 2011-03-09 added reschedule call after os_sem_take of SemMsgNew in Mcp2317SetOutput
// 2011-03-09 changed data rx condition to verify data tag also in Mcp2317SetOutput
// 2011-03-09 added pending for next incoming job data option io Mcp2317SetOutput
// 2011-03-09 removed checking of AT91C_SPI_TXBUFE messages in TskNotifyMcp2317
// 2011-03-09 added re-giving of SemMsgNew in TskNotifyMcp2317
// 2011-03-17 added new task TskNotifyHelperMcp2317 for safer user func calls
// 2011-03-30 added RESCHEDULE in TskNotifyHelperMcp2317 before & after user calls
// 2011-03-30 added RESCHEDULE in TskNotifyMcp2317 to avoid cont spi bursts
// 2011-04-14 added adjustable delay for irq query service in TskNotifyMcp2317
// 2011-04-14 added cache valid flag support to Mcp2317Init and Mcp2317SetOutput
// 2011-05-04 changed Init, SetOutput & Notify to use SpiChannelNext function
// 2011-06-01 moved notify task starts after complete init in Mcp2317Init
// 2013-04-11 fixed function header's return data type comments
// 2013-08-22 changed delays/timeouts from milliseconds to ticks
// 2013-08-22 changed RESCHEDULE (ms) to RESCHEDULE_TICKS (ticks)
// 2013-08-29 added checking of availability of required transfer block size
// 2013-08-29 fixed broken minimum delay in TskNotifyMcp2317
// 2013-08-29 changed to _umax instrinsics suitable for used USHORTs
// 2015-01-29 fixed bit check range in TskNotifyHelperMcp2317
// 2016-03-02 added macros and function InitMcp2317Handler
// 2016-03-02 added wrapper & check function Mcp2317IrqCtrl (controls hw & sw irq)
// 2016-03-02 renamed Mcp2317IrqHandler to IsrMcp2317IrqHandler (new name style)
// 2017-09-07 added check of pSMcp2317List's range after seek in Mcp2317SetOutput
// 2017-09-07 added check of pSMcp2317List's range after seek in TskNotifyHelperMcp2317
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

#include "mcp2317.h"
#include "mcp2317data.h"
#include "mcp2317dev.h"
#include "mcp2317ext.h"
#include "mcp2317int.h"

// Number of units located inside mcp2317.h
struct _SMcp2317RW SMcp2317RW[MCP2317_UNIT_MAX];
struct _SMcp2317Tsk SMcp2317Tsk[MCP2317_UNIT_MAX];

// Customized stacks
OS_TSK_STACK_USER_EX(StkTskNotifyMcp2317, MCP2317_STACK_TSKNOTIFYMCP2317, [MCP2317_UNIT_MAX]);
OS_TSK_STACK_USER_EX(StkTskNotifyHelperMcp2317, MCP2317_STACK_TSKNOTIFYHELPERMCP2317, [MCP2317_UNIT_MAX]);

//----------------------------------------------------------------------------
// void IsrMcp2317_x __irq
//----------------------------------------------------------------------------
// Each supported mcp2317 unit requires a function definition here
#if MCP2317_UNIT_MAX > 0
   MCP2317_ISR_FUNC(0);
   MCP2317_INIT_FUNC(0);
#endif

#if MCP2317_UNIT_MAX > 1
   MCP2317_ISR_FUNC(1);
   MCP2317_INIT_FUNC(1);
#endif

//----------------------------------------------------------------------------
// USHORT Mcp2317Init
//----------------------------------------------------------------------------
USHORT Mcp2317Init(UCHAR ucUnit, struct _SMcp2317Item *pSMcp2317List, UCHAR ucItemCnt, OS_ID oiMsgNewSem)
{
   USHORT usResult= 0;
   USHORT usPendCnt= 0;
     
   const UCHAR ucDataTxLen= 12;
   UCHAR ucDataTx[ucDataTxLen];
     
   UCHAR ucDataIdx; 
   UCHAR ucTagJobRx;
   
   ULONG ulMsgCnt;
   USHORT usByteCnt;
   
   struct _SMcp2317Item *pSMcp2317Item;
   UCHAR ucIndex;
     
   if (ucUnit < MCP2317_UNIT_MAX)
   {
      if (SPI_PDCMAX > ucDataTxLen)
      {
         if (SMcp2317RO[ucUnit].u16Timeout)
         {
            if (oiMsgNewSem)
            {
               Mcp2317IrqCtrl(ucUnit, FALSE);
      
               // Prepare OS tools
               os_sem_init(SMcp2317RW[ucUnit].SemStart, 0);
               os_sem_init(SemNotifyMcp2317[ucUnit], 0);
               os_sem_init(SemMcp2317Reg[ucUnit], 0);
               
               OS_MBX_INIT_EX(MbxNotifyHelperMcp2317[ucUnit]);
               
               // SSMcp2317RW[ucUnit].SemStart is initialized as already taken
               // SemMcp2317Reg[ucUnit] is initialized as already taken
               {         
                  SMcp2317Tsk[ucUnit].ucUnit= ucUnit;
                  SMcp2317RW[ucUnit].oiSemMsgNew= oiMsgNewSem;   
        
                  // Clear local cache
                  SMcp2317RW[ucUnit].bMcp2317RegValid= FALSE;
                  for (ucDataIdx= 0; ucDataIdx < (MCP2317_REG_COUNT/ 2); ucDataIdx++)
                  {
                     SMcp2317RW[ucUnit].u16Mcp2317Reg[ucDataIdx]= 0;
                  }
                  
                  SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IODIRA(MCP2317_REG_NOBANKSPLIT)/ 2]     = 0xFFFF;   // set input direction for all ports by default
                  SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IPOLA(MCP2317_REG_NOBANKSPLIT)/ 2]      = 0;
                  SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPINTENA(MCP2317_REG_NOBANKSPLIT)/ 2]   = 0;
                  SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_DEFVALA(MCP2317_REG_NOBANKSPLIT)/ 2]    = 0;
                  SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_INTCONA(MCP2317_REG_NOBANKSPLIT)/ 2]    = 0;
                  SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IOCONA(MCP2317_REG_NOBANKSPLIT)/ 2]     = SMcp2317RO[ucUnit].u8IOCON & ~(MCP2317_IOCON_BANKSPLIT | MCP2317_IOCON_NSEQOP);
                  SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPPUA(MCP2317_REG_NOBANKSPLIT)/ 2]      = 0;
                  SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPIOA(MCP2317_REG_NOBANKSPLIT)/ 2]      = 0;
                  SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_OLATA(MCP2317_REG_NOBANKSPLIT)/ 2]      = 0;
                  
                  SMcp2317RO[ucUnit].pSMcp2317List= pSMcp2317List;
                  SMcp2317RO[ucUnit].ucMcp2317ListSize= ucItemCnt; 
               
                  for (ucIndex= 0; ucIndex < ucItemCnt; ucIndex++)
                  {
                     pSMcp2317Item= pSMcp2317List+ ucIndex;
               
                     switch (pSMcp2317Item->EType)
                     {
                        case MCP2317_TYPE_INPUT:
                           SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IODIRA(MCP2317_REG_NOBANKSPLIT)/ 2] |= pSMcp2317Item->usMask;  // 1= input, 0= output (default)
                           SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPIOA(MCP2317_REG_NOBANKSPLIT)/ 2] &= ~pSMcp2317Item->usMask;  // (default)
                           break;
                           
                        case MCP2317_TYPE_OUTPUT_SET0:
                           SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IODIRA(MCP2317_REG_NOBANKSPLIT)/ 2] &= ~pSMcp2317Item->usMask; // 1= input, 0= output
                           SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPIOA(MCP2317_REG_NOBANKSPLIT)/ 2] &= ~pSMcp2317Item->usMask;  // (default)
                           break;
                           
                        case MCP2317_TYPE_OUTPUT_SET1:
                           SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IODIRA(MCP2317_REG_NOBANKSPLIT)/ 2] &= ~pSMcp2317Item->usMask; // 1= input, 0= output 
                           SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPIOA(MCP2317_REG_NOBANKSPLIT)/ 2] |= pSMcp2317Item->usMask;
                           break;
                     }
               
                     if ((pSMcp2317Item->EType == MCP2317_TYPE_INPUT) && (pSMcp2317Item->Notify))
                     {        
                        SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPINTENA(MCP2317_REG_NOBANKSPLIT)/ 2] |= pSMcp2317Item->usMask;
                     }
                     
                     if (pSMcp2317Item->EAttr & MCP2317_ATTR_RESET)
                     {
                        // default
                     }
                     
                     if (pSMcp2317Item->EAttr & MCP2317_ATTR_PULLUP_ON)    SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPPUA(MCP2317_REG_NOBANKSPLIT)/ 2] |= pSMcp2317Item->usMask;     
                     if (pSMcp2317Item->EAttr & MCP2317_ATTR_PULLUP_OFF)   SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPPUA(MCP2317_REG_NOBANKSPLIT)/ 2] &= ~pSMcp2317Item->usMask;
                     
                     if (pSMcp2317Item->EAttr & MCP2317_ATTR_INVERTER_ON)
                     {
                        if (pSMcp2317Item->EType == MCP2317_TYPE_INPUT)    SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IPOLA(MCP2317_REG_NOBANKSPLIT)/ 2] |= pSMcp2317Item->usMask;
                        ///else                                               SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_OLATA(MCP2317_REG_NOBANKSPLIT)/ 2] |= pSMcp2317Item->usMask; 
                     }
                        
                     if (pSMcp2317Item->EAttr & MCP2317_ATTR_INVERTER_OFF)
                     {
                        if (pSMcp2317Item->EType == MCP2317_TYPE_INPUT)    SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IPOLA(MCP2317_REG_NOBANKSPLIT)/ 2] &= ~pSMcp2317Item->usMask;
                        ///else                                               SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_OLATA(MCP2317_REG_NOBANKSPLIT)/ 2] &= ~pSMcp2317Item->usMask;
                     }
                  }
               
                  // Setup access mode
                  ucDataIdx= 0;
                  ucDataTx[ucDataIdx++]= MCP2317_ACCESS(SMcp2317RO[ucUnit].u8HAID, MCP2317_ACCESS_WRITE);
                  ucDataTx[ucDataIdx++]= MCP2317_REG_IOCONA(MCP2317_REG_NOBANKSPLIT);
                  // MCP2317_REG_IOCONA
                  ucDataTx[ucDataIdx++]= SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IOCONA(MCP2317_REG_NOBANKSPLIT)/ 2];
                  usByteCnt= ucDataIdx;
      
                  usResult= SpiTxTag(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, ucDataTx, usByteCnt, SMcp2317RO[ucUnit].u16Timeout, MCP2317_TAG_INIT);            
                  if (usResult >= usByteCnt)
                  {
                     // Setup part one
                     ucDataIdx= 0;
                     ucDataTx[ucDataIdx++]= MCP2317_ACCESS(SMcp2317RO[ucUnit].u8HAID, MCP2317_ACCESS_WRITE);
                     ucDataTx[ucDataIdx++]= MCP2317_REG_IPOLA(MCP2317_REG_NOBANKSPLIT);
                     // MCP2317_IPOLA...MCP2317_INTCONB
                     memcpy(&ucDataTx[ucDataIdx], &SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IPOLA(MCP2317_REG_NOBANKSPLIT)/ 2], 8);
                     usByteCnt= ucDataIdx+ 8;
                     usPendCnt += usResult;
      
                     usResult= SpiTxTag(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, ucDataTx, usByteCnt, SMcp2317RO[ucUnit].u16Timeout, MCP2317_TAG_INIT);               
                     if (usResult >= usByteCnt)
                     {
                        // Setup part two
                        ucDataIdx= 0;
                        ucDataTx[ucDataIdx++]= MCP2317_ACCESS(SMcp2317RO[ucUnit].u8HAID, MCP2317_ACCESS_WRITE);
                        ucDataTx[ucDataIdx++]= MCP2317_REG_GPPUA(MCP2317_REG_NOBANKSPLIT);
                        // MCP2317_GPPUA...MCP2317_GPIOB (writing zero to MCP2317_OLATA...MCP2317_OLATB clears MCP2317_GPIOA...MCP2317_GPIOB again!!)
                        memcpy(&ucDataTx[ucDataIdx], &SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPPUA(MCP2317_REG_NOBANKSPLIT)/ 2], 8);
                        usByteCnt= ucDataIdx+ 8;
                        usPendCnt += usResult;
      
                        usResult= SpiTxTag(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, ucDataTx, usByteCnt, SMcp2317RO[ucUnit].u16Timeout, MCP2317_TAG_INIT);                  
                        if (usResult >= usByteCnt)
                        {
                           // Setup output drivers
                           ucDataIdx= 0;
                           ucDataTx[ucDataIdx++]= MCP2317_ACCESS(SMcp2317RO[ucUnit].u8HAID, MCP2317_ACCESS_WRITE);
                           ucDataTx[ucDataIdx++]= MCP2317_REG_IODIRA(MCP2317_REG_NOBANKSPLIT);
                           // MCP2317_IODIRA...MCP2317_IODIRB
                           memcpy(&ucDataTx[ucDataIdx], &SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_IODIRA(MCP2317_REG_NOBANKSPLIT)/ 2], 2);      
                           usByteCnt= ucDataIdx+ 2;
                           usPendCnt += usResult;
      
                           usResult= SpiTxTag(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, ucDataTx, usByteCnt, SMcp2317RO[ucUnit].u16Timeout, MCP2317_TAG_INIT);                     
                           if (usResult >= usByteCnt)
                           {
                              usPendCnt += usResult;
                              
                              // Give semaphore to unlock u16Mcp2317Reg *here* because it is initialized as already taken
                              os_sem_give(SemMcp2317Reg[ucUnit]);
                           }
                        }
                     }
                  }
               }
               
               // Retrieve/wipe reply for successful tx only
               if (usPendCnt)
               {
                  // Semaphore is given only once regardless of the number of messages
                  if (OS_SEM_TAKE_CHK(os_sem_take(SMcp2317RW[ucUnit].oiSemMsgNew, SMcp2317RO[ucUnit].u16Timeout)))
                  {
                     // Force scheduler to run other tasks yet
                     RESCHEDULE_TICKS(0);
                     
                     do
                     {
                        // Retrieve MCP2317_TAG_INIT tagged jobs only
                        ucTagJobRx= MCP2317_TAG_INIT;
                        usByteCnt= SpiRxTag(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, NULL, SPI_BUFFERCNTRX, SMcp2317RO[ucUnit].u16Timeout, &ucTagJobRx);
                        if (usByteCnt > 0)
                        {
                           usPendCnt -= _umin(usPendCnt, usByteCnt);
                           
                           // Decrement message counters
                           SpiMsgChannel(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usByteCnt, SMcp2317RO[ucUnit].u16Timeout);
                           SpiMsgChannel(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usByteCnt, SMcp2317RO[ucUnit].u16Timeout);
                        }
                        
                        // Pend for next incoming job data when necessary
                        if (usPendCnt)
                        {
                           ulMsgCnt= SpiMsgChannelCheck(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SMcp2317RO[ucUnit].u16Timeout);
                           if (!ulMsgCnt)
                           {
                              if (OS_SEM_TAKE_CHK(os_sem_take(SMcp2317RW[ucUnit].oiSemMsgNew, SMcp2317RO[ucUnit].u16Timeout)))
                              {
                                 // Check for already retrieved next job data
                                 ulMsgCnt= SpiMsgChannelCheck(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SMcp2317RO[ucUnit].u16Timeout);
                              }
                           }
                        }
      
                     } while (usPendCnt && (ucTagJobRx == MCP2317_TAG_INIT));
                       
                     // Activate tasks pending for left messages
                     SpiMsgChannelNext(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, ~AT91C_SPI_OVRES, SMcp2317RW[ucUnit].oiSemMsgNew, SMcp2317RO[ucUnit].u16Timeout);
                  }   
               
                  // Tasks            
                  SMcp2317RW[ucUnit].tidTskNotifyMcp2317= OS_TSK_CREATE_USER_EX(TskNotifyMcp2317, SMcp2317RO[ucUnit].u8TskNotifyMcp2317, StkTskNotifyMcp2317[ucUnit], &SMcp2317Tsk[ucUnit]);
                  SMcp2317RW[ucUnit].tidTskNotifyHelperMcp2317= OS_TSK_CREATE_USER_EX(TskNotifyHelperMcp2317, SMcp2317RO[ucUnit].u8TskNotifyHelperMcp2317, StkTskNotifyHelperMcp2317[ucUnit], &SMcp2317Tsk[ucUnit]);
      
                  if (SMcp2317RW[ucUnit].tidTskNotifyMcp2317 != 0 && SMcp2317RW[ucUnit].tidTskNotifyHelperMcp2317 != 0)
                  {
                     // Give semaphore to unlock device yet
                     os_sem_give(SMcp2317RW[ucUnit].SemStart);
                     
                     // Enable Irq
                     Mcp2317IrqCtrl(ucUnit, TRUE);
                     
                     // Give semaphore to clear any pending irq state of the mcp2317 yet
                     os_sem_give(SemNotifyMcp2317[ucUnit]);
      
                     usResult= TRUE;
                  }
               }
            }
         }
      }
   }
   
   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT Mcp2317SetOutput
//----------------------------------------------------------------------------
USHORT Mcp2317SetOutput(UCHAR ucUnit, USHORT usMask, USHORT usLevel, U16 u16Timeout)
{ 
   USHORT usResult= 0;
   USHORT usPendCnt= 0;
   USHORT usResultPin= 0;
    
   const UCHAR ucDataTxLen= 4;
   UCHAR ucDataTx[ucDataTxLen];

   UCHAR ucDataIdx;  
   UCHAR ucListIdx;
   UCHAR ucBitPos;
   UCHAR ucTagJobRx;
   
   ULONG ulMsgCnt;
   USHORT usByteCnt;
   
   U16 u16Pin;
   U16 u16Gpio;
   
   if (ucUnit < MCP2317_UNIT_MAX)
   {
      // Check assigment of news semaphore
      if (SMcp2317RW[ucUnit].oiSemMsgNew)
      {
         // Lock device
         if (OS_SEM_TAKE_CHK(os_sem_take(SMcp2317RW[ucUnit].SemStart, u16Timeout)))
         {
            // Take semaphore to lock u16Mcp2317Reg
            if (OS_SEM_TAKE_CHK(os_sem_take(SemMcp2317Reg[ucUnit], u16Timeout)))
            {
               // Get last written value for GPIOA...GPIOB
               u16Gpio= SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPIOA(MCP2317_REG_NOBANKSPLIT)/ 2];
               
               for (ucBitPos= 0; ucBitPos < 16; ucBitPos++)
               {
                  u16Pin= (1 << ucBitPos);
                  if (usMask & u16Pin)
                  {
                     if (u16Pin != SMcp2317RO[ucUnit].pSMcp2317List[ucBitPos].usMask)
                     {
                        // Seek to pin inside list
                        for (ucListIdx= 0; (ucListIdx < SMcp2317RO[ucUnit].ucMcp2317ListSize) && (u16Pin != SMcp2317RO[ucUnit].pSMcp2317List[ucListIdx].usMask); ucListIdx++);
                     }
                     else
                     {
                        // Already at right position
                        ucListIdx= ucBitPos;
                     }
                     
                     // Handle found pin
                     if (ucListIdx < SMcp2317RO[ucUnit].ucMcp2317ListSize)
                     {
                        if (u16Pin == SMcp2317RO[ucUnit].pSMcp2317List[ucListIdx].usMask)
                        {
                           if (SMcp2317RO[ucUnit].pSMcp2317List[ucListIdx].EType != MCP2317_TYPE_INPUT)
                           {
                              // Show success of each pin
                              usResultPin |= u16Pin;
                                                
                              if (usLevel & u16Pin)
                              {
                                 u16Gpio |= u16Pin;
                              }
                              else
                              {
                                 u16Gpio &= ~u16Pin;  // default
                              }
                           }
                        }
                     }
                  }
               }     

               // Send only changed value
               if ((SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPIOA(MCP2317_REG_NOBANKSPLIT)/ 2] != u16Gpio) || (!SMcp2317RW[ucUnit].bMcp2317RegValid))
               {
                  // Update written value for GPIOA...GPIOB (update local cache)
                  SMcp2317RW[ucUnit].u16Mcp2317Reg[MCP2317_REG_GPIOA(MCP2317_REG_NOBANKSPLIT)/ 2]= u16Gpio;
                  
                  // Setup output values
                  ucDataIdx= 0;
                  ucDataTx[ucDataIdx++]= MCP2317_ACCESS(SMcp2317RO[ucUnit].u8HAID, MCP2317_ACCESS_WRITE);
                  ucDataTx[ucDataIdx++]= MCP2317_REG_GPIOA(MCP2317_REG_NOBANKSPLIT);
                  // MCP2317_GPIOA...MCP2317_GPIOB
                  memcpy(&ucDataTx[ucDataIdx], &u16Gpio, 2);
                  usByteCnt= ucDataIdx+ 2;
                  usResult= SpiTxTag(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, ucDataTx, usByteCnt, SMcp2317RO[ucUnit].u16Timeout, MCP2317_TAG_SETOUTPUT);

                  // Debug
                  if (!usResult)
                  {
                     usResult= usResult;
                  }
                  
                  // Retrieve/wipe reply for successful tx only
                  if (usResult >= usByteCnt)
                  {
                     usPendCnt += usResult;
                     
                     // Semaphore is given only once regardless of the number of messages
                     if (OS_SEM_TAKE_CHK(os_sem_take(SMcp2317RW[ucUnit].oiSemMsgNew, SMcp2317RO[ucUnit].u16Timeout)))
                     {
                        // Force scheduler to run other tasks yet
                        RESCHEDULE_TICKS(0);
                        
                        do
                        {                    
                           // Retrieve MCP2317_TAG_SETOUTPUT tagged jobs only
                           ucTagJobRx= MCP2317_TAG_SETOUTPUT;
                           usByteCnt= SpiRxTag(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, NULL, SPI_BUFFERCNTRX, SMcp2317RO[ucUnit].u16Timeout, &ucTagJobRx);
                           if (usByteCnt > 0)
                           {
                              usPendCnt -= _umin(usPendCnt, usByteCnt);
                              
                              // Decrement message counters
                              SpiMsgChannel(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, usByteCnt, SMcp2317RO[ucUnit].u16Timeout);
                              SpiMsgChannel(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, AT91C_SPI_TXBUFE, usByteCnt, SMcp2317RO[ucUnit].u16Timeout);
                           }
                           
                           // Pend for next incoming job data when necessary
                           if (usPendCnt)
                           {
                              ulMsgCnt= SpiMsgChannelCheck(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SMcp2317RO[ucUnit].u16Timeout);
                              if (!ulMsgCnt)
                              {
                                 if (OS_SEM_TAKE_CHK(os_sem_take(SMcp2317RW[ucUnit].oiSemMsgNew, SMcp2317RO[ucUnit].u16Timeout)))
                                 {
                                    // Check for already retrieved next job data
                                    ulMsgCnt= SpiMsgChannelCheck(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, AT91C_SPI_RXBUFF, SMcp2317RO[ucUnit].u16Timeout);
                                 }
                              }
                           }
                           
                        } while (usPendCnt && (ucTagJobRx == MCP2317_TAG_SETOUTPUT));

                        // Set valid flag of cache
                        SMcp2317RW[ucUnit].bMcp2317RegValid= TRUE;

                        // Activate tasks pending for left messages
                        SpiMsgChannelNext(SMcp2317RO[ucUnit].ucUnit, SMcp2317RO[ucUnit].ucChannel, ~AT91C_SPI_OVRES, SMcp2317RW[ucUnit].oiSemMsgNew, SMcp2317RO[ucUnit].u16Timeout);
                     }
                  }
               }
               else
               {
                  // Fake success
                  usResult= 1;
               }
               
               // Give semaphore to unlock u16Mcp2317Reg
               os_sem_give(SemMcp2317Reg[ucUnit]);
            }
                        
            // Unlock device
            os_sem_give(SMcp2317RW[ucUnit].SemStart);
         }
      }
   }

   // Debug
   if (!usResult)
   {
      usResult= usResult;
   }
   
   return ((usResult != 0)* usResultPin);
}

//----------------------------------------------------------------------------
// __task void TskNotifyMcp2317
//----------------------------------------------------------------------------
__task void TskNotifyMcp2317(void *pvParam)
{
   struct _SMcp2317Tsk *pSMcp2317Tsk= (struct _SMcp2317Tsk *) pvParam;
   
   const USHORT usDataRxLen= SPI_BUFFERCNTRX;
   UCHAR ucDataRx[usDataRxLen];

   const UCHAR ucDataTxLen= 6;
   UCHAR ucDataTx[ucDataTxLen];

   ULONG ulMsgCnt;
   ULONG ulValue;

   U16 *pu16IntF;
   U16 *pu16IntCap;
   
   USHORT usResult;
   USHORT usFail;
   
   UCHAR ucDataIdx;
   
   UCHAR ucTagJobRx;
   UCHAR ucTagJobTx= MCP2317_TAG_ISRQUERY;
       
   FOREVER
   {
      // Pend for IRQ notification
      if (OS_SEM_TAKE_CHK(os_sem_take(SemNotifyMcp2317[pSMcp2317Tsk->ucUnit], OS_T_FOREVER)))
      {
         // Check assigment of news semaphore
         if (SMcp2317RW[pSMcp2317Tsk->ucUnit].oiSemMsgNew)
         {
            // Lock device
            if (OS_SEM_TAKE_CHK(os_sem_take(SMcp2317RW[pSMcp2317Tsk->ucUnit].SemStart, OS_T_FOREVER)))
            {
               // Clear value to send later
               ulValue= 0;
               
               ucDataIdx= 0;
               ucDataTx[ucDataIdx++]= MCP2317_ACCESS(MCP2317_HAID, MCP2317_ACCESS_READ);
               ucDataTx[ucDataIdx++]= MCP2317_REG_INTFA(MCP2317_REG_NOBANKSPLIT);
               ucDataTx[ucDataIdx++]= 0xFF; // Dummy tx data to get rx data of INTFA
               ucDataTx[ucDataIdx++]= 0xFF; // Dummy tx data to get rx data of INTFB
               ucDataTx[ucDataIdx++]= 0xFF; // Dummy tx data to get rx data of INTCAPA (clears pending irq)
               ucDataTx[ucDataIdx++]= 0xFF; // Dummy tx data to get rx data of INTCAPB (clears pending irq)
      
               usResult= SpiTxTag(SMcp2317RO[pSMcp2317Tsk->ucUnit].ucUnit, SMcp2317RO[pSMcp2317Tsk->ucUnit].ucChannel, ucDataTx, ucDataIdx, SMcp2317RO[pSMcp2317Tsk->ucUnit].u16Timeout, ucTagJobTx);
               if (usResult >= ucDataIdx)
               {                
                  // Semaphore is given only once regardless of the number of messages
                  if (OS_SEM_TAKE_CHK(os_sem_take(SMcp2317RW[pSMcp2317Tsk->ucUnit].oiSemMsgNew, OS_T_FOREVER)))
                  {
                     // Force scheduler to run other tasks yet
                     RESCHEDULE_TICKS(0);
                     
                     do
                     {                        
                        // Debug
                        usFail= SpiMsgChannel(SMcp2317RO[pSMcp2317Tsk->ucUnit].ucUnit, SMcp2317RO[pSMcp2317Tsk->ucUnit].ucChannel, AT91C_SPI_OVRES, 0, SMcp2317RO[pSMcp2317Tsk->ucUnit].u16Timeout);
                        usFail= usFail;
                     
                        // Retrieve ANY job by setting tag to default value
                        ucTagJobRx= SPI_USERTAG_DEFAULT;
                        usResult= SpiRxTag(SMcp2317RO[pSMcp2317Tsk->ucUnit].ucUnit, SMcp2317RO[pSMcp2317Tsk->ucUnit].ucChannel, ucDataRx, SPI_BUFFERCNTRX, SMcp2317RO[pSMcp2317Tsk->ucUnit].u16Timeout, &ucTagJobRx);
                        if (usResult > 0)
                        {
                           // Decrement message counters
                           SpiMsgChannel(SMcp2317RO[pSMcp2317Tsk->ucUnit].ucUnit, SMcp2317RO[pSMcp2317Tsk->ucUnit].ucChannel, AT91C_SPI_RXBUFF, usResult, SMcp2317RO[pSMcp2317Tsk->ucUnit].u16Timeout);
                           SpiMsgChannel(SMcp2317RO[pSMcp2317Tsk->ucUnit].ucUnit, SMcp2317RO[pSMcp2317Tsk->ucUnit].ucChannel, AT91C_SPI_TXBUFE, usResult, SMcp2317RO[pSMcp2317Tsk->ucUnit].u16Timeout);
      
                           if (usResult == ucDataIdx)
                           {                     
                              if (ucTagJobRx == ucTagJobTx)
                              {
                                 usResult= usResult;
                                 ucTagJobRx= ucTagJobRx;
                                                      
                                 pu16IntF= (U16 *)&ucDataRx[2];    // Assign INTFA (LSB) & INTFB (MSB)
                                 pu16IntCap= (U16 *)&ucDataRx[4];  // Assign INTCAPA (LSB) & INTCAPB (MSB)
                                 
                                 // Take semaphore to lock u16Mcp2317Reg
                                 if (OS_SEM_TAKE_CHK(os_sem_take(SemMcp2317Reg[pSMcp2317Tsk->ucUnit], OS_T_FOREVER)))
                                 {
                                    // Update read value for INTFA...INTFB
                                    SMcp2317RW[pSMcp2317Tsk->ucUnit].u16Mcp2317Reg[MCP2317_REG_INTFA(MCP2317_REG_NOBANKSPLIT)/ 2]= *pu16IntF;
   
                                    // Update read value for INTCAPA...INTCAPB
                                    SMcp2317RW[pSMcp2317Tsk->ucUnit].u16Mcp2317Reg[MCP2317_REG_INTCAPA(MCP2317_REG_NOBANKSPLIT)/ 2]= *pu16IntCap;
   
                                    // Give semaphore to unlock u16Mcp2317Reg
                                    os_sem_give(SemMcp2317Reg[pSMcp2317Tsk->ucUnit]);
                                 }
                                 
                                 // Just for debug visibility
                                 pu16IntF= pu16IntF;
                                 pu16IntCap= pu16IntCap;
                                 
                                 // Debug
                                 if (*pu16IntF == 0xFF)
                                 {
                                    *pu16IntF= *pu16IntF;
                                 }

                                 // Prepare value to send later
                                 ulValue= ((*pu16IntF << 16) | *pu16IntCap);
                              }
                           }
                        }
                        
                        // Pend for next incoming job data when necessary
                        ulMsgCnt= SpiMsgChannelCheck(SMcp2317RO[pSMcp2317Tsk->ucUnit].ucUnit, SMcp2317RO[pSMcp2317Tsk->ucUnit].ucChannel, AT91C_SPI_RXBUFF, SMcp2317RO[pSMcp2317Tsk->ucUnit].u16Timeout);
                        if (!ulMsgCnt)
                        {
                           if (OS_SEM_TAKE_CHK(os_sem_take(SMcp2317RW[pSMcp2317Tsk->ucUnit].oiSemMsgNew, SMcp2317RO[pSMcp2317Tsk->ucUnit].u16Timeout)))
                           {
                              // Check for already retrieved next job data
                              ulMsgCnt= SpiMsgChannelCheck(SMcp2317RO[pSMcp2317Tsk->ucUnit].ucUnit, SMcp2317RO[pSMcp2317Tsk->ucUnit].ucChannel, AT91C_SPI_RXBUFF, SMcp2317RO[pSMcp2317Tsk->ucUnit].u16Timeout);
                           }
                        }
      
                     } while (usResult && ulMsgCnt);
                     
                     // Activate tasks pending for left messages
                     SpiMsgChannelNext(SMcp2317RO[pSMcp2317Tsk->ucUnit].ucUnit, SMcp2317RO[pSMcp2317Tsk->ucUnit].ucChannel, ~AT91C_SPI_OVRES, SMcp2317RW[pSMcp2317Tsk->ucUnit].oiSemMsgNew, SMcp2317RO[pSMcp2317Tsk->ucUnit].u16Timeout);
                  }
               }
               else
               {
                  // Give semaphore again to clear any pending irq state of the mcp2317 by a new try
                  os_sem_give(SemNotifyMcp2317[pSMcp2317Tsk->ucUnit]);
               }
             
               // Unlock device
               os_sem_give(SMcp2317RW[pSMcp2317Tsk->ucUnit].SemStart);
               
               if (ulValue)
               {
                  if (os_mbx_check(&MbxNotifyHelperMcp2317[pSMcp2317Tsk->ucUnit]) > 0)
                  {
                     // Send value to helper task to execute user function call now after all processing is complete
                     os_mbx_send_safe(&MbxNotifyHelperMcp2317[pSMcp2317Tsk->ucUnit], OS_MBX_VALUE_PUT(ulValue), SMcp2317RO[pSMcp2317Tsk->ucUnit].u16Timeout);
                  }
               }
            }
         }
         
         // Delay to limit maximum response frequency (at least one system tick for zero set delay)
         RESCHEDULE_TICKS(_umax(1, SMcp2317RO[pSMcp2317Tsk->ucUnit].u16DelayIrqQuery));
      }      
   }

   DETACH;
}

//----------------------------------------------------------------------------
// __task void TskNotifyHelperMcp2317
//----------------------------------------------------------------------------
__task void TskNotifyHelperMcp2317(void *pvParam)
{
   struct _SMcp2317Tsk *pSMcp2317Tsk= (struct _SMcp2317Tsk *) pvParam;
     
   ULONG ulValue;
   U16 u16IntF;
   U16 u16IntCap;
   U16 u16Pin;
   UCHAR ucListIdx;
   UCHAR ucBitPos;
   
   bool bLevel;
   
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxNotifyHelperMcp2317[pSMcp2317Tsk->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         u16IntF= (ulValue & BITS_ULONGHX) >> 16; 
         u16IntCap= (ulValue & BITS_ULONGLX);
         
         for (ucBitPos= 0; ucBitPos < 16; ucBitPos++)
         {
            u16Pin= (1 << ucBitPos);
            if (u16IntF & u16Pin)
            {
               // Retrieve current pin level
               bLevel= (u16IntCap & (1 << ucBitPos)) != 0;
         
               if (u16Pin != SMcp2317RO[pSMcp2317Tsk->ucUnit].pSMcp2317List[ucBitPos].usMask)
               {
                  // Seek to pin inside list
                  for (ucListIdx= 0; (ucListIdx < SMcp2317RO[pSMcp2317Tsk->ucUnit].ucMcp2317ListSize) && (u16Pin != SMcp2317RO[pSMcp2317Tsk->ucUnit].pSMcp2317List[ucListIdx].usMask); ucListIdx++);
               }
               else
               {
                  // Already at right position
                  ucListIdx= ucBitPos;
               }
               
               // Handle found pin
               if (ucListIdx < SMcp2317RO[pSMcp2317Tsk->ucUnit].ucMcp2317ListSize)
               {
                  if (u16Pin == SMcp2317RO[pSMcp2317Tsk->ucUnit].pSMcp2317List[ucListIdx].usMask)
                  {
                     // Filter notification by specified trigger condition
                     if ((SMcp2317RO[pSMcp2317Tsk->ucUnit].pSMcp2317List[ucListIdx].ETrigger == MCP2317_TRG_ANY) ||
                        ((SMcp2317RO[pSMcp2317Tsk->ucUnit].pSMcp2317List[ucListIdx].ETrigger == MCP2317_TRG_FALL) && !bLevel) ||
                        ((SMcp2317RO[pSMcp2317Tsk->ucUnit].pSMcp2317List[ucListIdx].ETrigger == MCP2317_TRG_RAISE) && bLevel))
                     {                    
                        // Call user notification function with args
                        if (SMcp2317RO[pSMcp2317Tsk->ucUnit].pSMcp2317List[ucListIdx].Notify)
                        {
                           RESCHEDULE_TICKS(0);
                           SMcp2317RO[pSMcp2317Tsk->ucUnit].pSMcp2317List[ucListIdx].Notify(u16IntF, u16IntCap, u16Pin, bLevel);
                        }
                     }
                  }
               }               
            }
         }
         
         RESCHEDULE_TICKS(0);
      }
   }
   
   DETACH;
}

//----------------------------------------------------------------------------
// UCHAR Mcp2317IrqCtrl
//----------------------------------------------------------------------------
UCHAR Mcp2317IrqCtrl(UCHAR ucUnit, bool bEnable)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < MCP2317_UNIT_MAX)
   {
      if ((SMcp2317RW[ucUnit].SIrq.pPio) && (SMcp2317RW[ucUnit].SIrq.u32Pin))
      {
         // Control hardware interrupt mask
         if (bEnable)
         {
            AT91F_PIO_InterruptEnable(SMcp2317RW[ucUnit].SIrq.pPio, SMcp2317RW[ucUnit].SIrq.u32Pin);
         }
         else
         {
            AT91F_PIO_InterruptDisable(SMcp2317RW[ucUnit].SIrq.pPio, SMcp2317RW[ucUnit].SIrq.u32Pin);
         }
         
         ucResult++;
      }
      
      // Update software interrupt flag
      SMcp2317RW[ucUnit].bIrq= bEnable;
      
      ucResult++;
   }
      
   return (ucResult);
}

//----------------------------------------------------------------------------
// bool InitMcp2317Handler
//----------------------------------------------------------------------------
bool InitMcp2317Handler(UCHAR ucUnit, U32 u32Pin, AT91PS_PIO pPio)
{
   if (ucUnit < MCP2317_UNIT_MAX)
   {
      // Set interrupt pin data once for valid params only
      if ((u32Pin) && (pPio) && !(SMcp2317RW[ucUnit].SIrq.u32Pin) && !(SMcp2317RW[ucUnit].SIrq.pPio))
      {
         SMcp2317RW[ucUnit].SIrq.u32Pin= u32Pin;
         SMcp2317RW[ucUnit].SIrq.pPio= pPio;
      }
   }
   
   // Disable interrupt yet, it's enabled later manually
   return (FALSE);
}

//----------------------------------------------------------------------------
// void IsrMcp2317IrqHandler
//----------------------------------------------------------------------------
void IsrMcp2317IrqHandler(UCHAR ucUnit, U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio)
{
   if (SMcp2317RW[ucUnit].bIrq)
   {
      if (!ISR_SEM_TAKE_CHK(isr_sem_check(SemNotifyMcp2317[ucUnit])))
      {
         isr_sem_give(SemNotifyMcp2317[ucUnit]);
      }
   }
}

