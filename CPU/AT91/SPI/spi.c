//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/SPI/SPI.C
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-10-07 initial version
// 2009-10-09 added SpiInitUnit, SpiInitChannel
// 2009-10-14 completed TskSpiRx
// 2009-10-15 added SpiMsgUnit, SpiMsgChannel, TskSpiTx
// 2009-10-23 changed style of SpiTx and SpiRx for support of ushort blocks
// 2009-11-04 moved SpiTsk data into _SSpiRW and _SSpiChannelRW
// 2009-11-04 debugged, tested and reviewed all of the code in total
// 2009-11-05 changed design of TskSpiMsgChannel to single tsk
// 2009-11-05 Msg counters changed to show number of bytes sent and received
// 2009-11-05 changed style of SpiInitUnit (TORX still unsupported)
// 2009-11-05 changed tx irq re-enable in TskSpiTx from ulSpiTx to ulSpiCntTx
// 2009-11-06 using MSGCOUNTERTYPE and MSGCOUNTERMAX for configurable counters
// 2009-11-06 changed return type of SpiMsgUnit and SpiMsgChannel to USHORT
// 2009-11-11 TskSpiMsg and TskSpiMsgChannel give news semaphore only once now
// 2009-12-11 Fixed count for circular_write(ucSpiChannelRx...) in TskSpiRx
// 2009-12-11 HWFIX: Retrieving jobs non-fragmented only for AT91C_SPI_TXBUFE
// 2009-12-11 HWFIX: Changed address src for AT91C_SPI_RXBUFF to ulPDCBufferTx
// 2009-12-17 Added size check to SpiTx (max SPI_PDCMAX for non-fragment dma)
// 2009-12-17 Fixed msg counter inc in TskSpiRx for message AT91C_SPI_OVRES
// 2009-12-18 Changed SpiRx to retrive minimum of user count and job size
// 2009-12-18 SpiRx retrieves now only up to rx job count boundary (split rx)
// 2009-12-18 SpiMsgUnit and SpiMsgChannel now support true counter decrement
// 2009-12-21 added circular buffers ucSpiChannelTagRx and ucSpiChannelTagTx
// 2009-12-22 SpiRxTag retrieves data for matching or unspecified tag only yet
// 2009-12-22 added SpiMsgUnitCheck SpiMsgChannelCheck functions
// 2009-12-22 fixed generation of msg u32IrqRx and u32IrqTx in SpiIrqHandler
// 2009-12-23 added support of circular_wipe to SpiRxTag for easy data ignore
// 2009-12-23 optimized msg create & transmit for job rx/tx in SpiIrqHandler
// 2009-12-23 TskSpiRx corrects now job count rx already set by ISR by overrun
// 2009-12-23 using SPI_STACK_TSKTSKSPIRX and SPI_STACK_TSKTSKSPITX for stacks
// 2010-04-08 added SpiTxLock, SpiTx(Tag)Unlock, and SpiTx(Tag)Queue functions
// 2010-04-08 fixed calc of usCountPCS which failed for channel 0 in TskSpiRx
// 2010-05-21 added auto reconfiguration feature for hardware CSR in TskSpiTx
// 2010-05-26 fixed new message notification by semaphore in TskSpiChannelMsg
// 2010-06-03 fixed and completed success check in SpiInitUnit
// 2010-06-04 added initial version of spi slave support
// 2010-06-09 completed handling of nss raise irq of spi slave support
// 2010-06-16 completed hw-bug workarround for spi slave tx pdc bug
// 2010-06-17 fixed multiple channel message combo splitting in TskSpiRx
// 2010-06-17 fixed handler for AT91C_SPI_RXBUFF for spi slave odd count bytes
// 2010-06-17 removed forced option AT91C_SPI_PCS in SpiInitUnit
// 2010-06-17 completed total hw-bug workarround for spi slave tx pdc bug
// 2010-06-18 added range check in handler for AT91C_SPI_RXBUFF for ucChannel
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-08-04 removed obsolete params ulTORXUS & ulTORXModeInit in SpiInitUnit
// 2010-08-05 added NSSR msg to channel 0 for slave mode unit in SpiIrqHandler
// 2010-08-05 fixed channel rx buffer free check in TskSpiRx
// 2010-08-05 fixed item size determination for tx pdc queue in SpiIrqHandler
// 2010-08-11 moved pdc rx data processing and hw-fix into modified TskSpiRx
// 2010-08-12 optimized lock/unlock access for channel buffers in TskSpiRx
// 2010-08-12 added no-source feature to SpiTx to queue zeros without src data
// 2010-08-26 added init of circular ulSpiCntTx in SpiInitUnit
// 2010-08-27 added special init of custom SCircular buffer
// 2010-08-27 enabled u32IrqRx immediately after initialization in SpiInitUnit
// 2010-08-27 changed TskSpiRx for new memory data transfer design
// 2010-08-27 removed obsolete msg transmission of u32IrqTx in TskSpiTx
// 2010-08-27 changed TskSpiTx for new tx clearing by tx irq beat design
// 2010-08-27 SpiIrqHandler completely *rewritten* for new memory data transfer
// 2010-09-01 added initialization of usSpiChannelCntRxQ in SpiInitUnit
// 2010-09-01 changed SpiRxTag to retrieve data without queued tag set to null
// 2010-09-01 TskSpiRx completely *rewritten* for new intermediate count queue
// 2010-09-01 changed TskSpiTx for new intermediate count queue
// 2010-09-01 changed tx pdc buffer data setup in SpiIrqHandler
// 2010-09-02 fixed job count limiting in TskSpiRx (especially for slave mode)
// 2010-09-02 stabilized determination of usTCROffset in SpiIrqHandler
// 2010-09-02 migrated from ATOMIC... to LOCKED macro
// 2010-09-02 send MbxSpiMsgChannel only for usSpiChannelCntRx ok in TskSpiRx
// 2010-09-02 stabilized consecutive circular write in SpiTxTag
// 2010-09-02 stabilized consecutive circular write in SpiTxTagUnlock
// 2010-09-02 stabilized consecutive circular write in TskSpiRx and TskSpiTx
// 2010-09-09 omitting reschedule delay when set to zero in TskSpiRx
// 2010-09-09 changed reschedule delay to u16DelayTxMS in TskSpiTx
// 2010-09-22 fixed new job triggering for same channel in TskSpiRx
// 2010-09-22 changed prototype of SpiInitChannel (usMaxAgeTxMS -> u16MaxAge)
// 2010-09-22 renamed u16AgeStepTxMS to u16AgeStepTx (ticks)
// 2010-09-22 renamed usMaxAgeTxMS to u16MaxAgeTx
// 2010-09-22 renamed usAgeTxMS to u16AgeTx
// 2010-09-23 added SemTickTx to _SSpiRW for re-triggerabel cycle of TskSpiTx
// 2010-09-23 fixed reversed non-timed sem_take logic in SpiTxTagQueue
// 2010-09-23 fixed initial setting of ucFlags in SpiInitChannel
// 2010-09-23 changed to _u(l)min & _u(l)max intrinsics
// 2010-10-07 removed obsolete usBytesPerJob fix in TskSpiRx
// 2010-10-07 fixed re-arm check of just completed rx by tx in SpiIrqHandler
// 2010-10-07 added CSR32 queue map and checking in TskSpiTx to void conflicts
// 2010-10-22 added forced task switch to SpiMsgChannelCheck by RESCHEDULE(0)
// 2010-10-27 fixed new message notification in TskSpiMsgChannel (hint by rr)
// 2010-10-27 fixed new message notification in TskSpiMsg (hint by rr)
// 2010-10-27 added sem check before new message notification in TskSpiMsgChannel
// 2010-10-27 added sem check before new message notification in TskSpiMsg
// 2010-11-23 removed obsolete data item ucChannel of _SSpiChannelRW
// 2010-12-09 re-introduced the pdx rx address fix to TskSpiRx & SpiIrqHandler
// 2011-01-19 fixed race condition of tx irq state in SpiIrqHandler
// 2011-01-19 new method for SPI_HWFIX_PDCRX to save memory & avoid false fix
// 2011-01-27 fixed semaphore SSpiChannelRW[U][C].Sem?x pairs in TskSpiTx
// 2011-02-03 fixed query of SPI_CHANNEL_FLAGS_QUEUELOCKED in SpiTxLock & SpiTxUnLock
// 2011-02-03 rewrote SpiTxLock for scheduler interrupt protected flag changing
// 2011-03-04 changed SpiRxTag to return current tag value also for non-matches
// 2011-03-04 fixed some circular buffer comments in TskSpiTx
// 2011-04-06 added rx irq locking/unlocking to SpiIrqHandler/TskSpiRx
// 2011-04-06 fixed control logic for tx irq state machine in SpiIrqHandler
// 2011-04-06 fixed possible data overrun for bursts for rx in SpiIrqHandler
// 2011-04-06 fixed data set-up logic for tx in SpiIrqHandler
// 2011-04-06 moved ucIrqTxState from local static to _SSpiRW
// 2011-04-06 added several debug break cases (assertions) to SpiRxTag and TskSpiRx
// 2011-04-06 rewrote SpiTxLock to give channel semaphore before function return
// 2011-04-06 rewrote SpiTxTagUnlock to give channel semaphore before function return
// 2011-04-06 rewrote SpiTxTagQueue to give channel semaphore before function return
// 2011-04-06 rewrote SpiTxTagQueue to use flag indicators instead long locked sema
// 2011-04-07 secured take/give sequence of sema in TskSpiMsg and TskSpiMsgChannel
// 2011-04-07 removed RESCHEDULE call in TskSpiMsg and TskSpiMsgChannel
// 2011-04-07 fixed take/give state machine for channel semaphores in TskSpiRx
// 2011-04-07 fixed required SPI_HWFIX_PDCRX hardware workarround for data bursts
// 2011-04-07 fixed control logic of tx irq and tx data set-up in TskSpiTx
// 2011-04-07 added RESCHEDULE call after every os_mbx_send_safe call
// 2011-04-07 new take order of multiple semas to 'global 1st' (gTx, chTx, chRx)
// 2011-04-26 added circular buffer ucSpiAddrRx in SpiInitUnit/SpiTskTx/SpiTskRx
// 2011-04-26 rewrote SPI_HWFIX_PDCRX hardware workarround for data bursts
// 2011-04-26 fixed ucChannelCurrLock clearing before item processing in TskSpiRx
// 2011-04-26 fixed circular buffer comments in TskSpiTx
// 2011-04-26 added SemRx to in SpiInitUnit/SpiTskTx/SpiTskRx
// 2011-05-04 added function SpiMsgChannelNext to clean up periph drivers code
// 2011-05-26 fixed limit check of SpiTxTag for 16bit pdc item awareness
// 2011-05-26 fixed limit check of SpiTxLock for 16bit pdc item awareness
// 2011-05-26 fixed initialization of usCountPCS before checking in TskSpiRx
// 2011-05-26 added usCountPCS to ulSpiCntTx's item for TskSpiTx/SpiIrqHandler
// 2011-05-26 fixed filling of pdc data block for true pdc item awareness
// 2011-05-26 fixed initial setting of ucFlags in SpiInitUnit/SpiInitChannel
// 2011-05-26 fixed TskSpiMsg for full 16bit (USHORT) msg counter support
// 2011-05-26 fixed TskSpiMsgChannel for full 16bit (USHORT) msg counter support
// 2011-06-01 moved news semaphore assignment before task start in SpiInitUnit
// 2011-06-01 splitted old SPI_UNIT_MSGBITS to SPI_UNIT_/SPI_DATA_MSG_MSGBITS
// 2011-06-30 forever timeout of SemTickTx in TskSpiTx to avoid useless polling
// 2011-06-30 added retriggering of tick cycle for TskSpiTx in SpiIrqhandler
// 2011-07-07 fixed and optimized (Tsk)SpiMsgUnit/Channel (+ multiple bit feature)
// 2011-07-07 SPI_UNIT/CHANNEL_MSGMAX are now a zero based settings
// 2011-07-07 removed os_mbx_check to activate timeout in TskSpiRx for MbxSpiMsgChannel
// 2011-07-13 changed non-static ucChannelCurrLock from array to base type in TskSpiRx
// 2011-07-13 added masking of (usually enabled) messages in TskSpiRx
// 2011-07-13 fixed masking of (usually enabled) message in TskSpiTx
// 2011-07-13 added 2nd masking of messages by consideration of SPI_..._MSGMAX
// 2011-07-13 added message count validation check in TskSpiMsg(Channel)
// 2011-09-21 fixed triggering of TskSpiTx semaphore in SpiTxTag function
// 2013-04-11 fixed function header's return data type comments
// 2013-08-21 changed delays from milliseconds to ticks
// 2013-08-21 changed RESCHEDULE (ms) to RESCHEDULE_TICKS (ticks)
// 2013-08-29 changed to _umax instrinsics suitable for used USHORTs
// 2013-12-05 added ucChannelIdx to _SSpiRW, added use of struct SSpiChannelMapRW
// 2014-01-08 fixed use of struct SSpiChannelMapRW in TskSpiRx & TskSpiTx
// 2014-01-09 fixed checking of array range for channel-id
// 2014-11-12 fixed loop limit in SpiMsgUnitCheck and SpiMsgChannelCheck
// 2016-01-21 fixed broken sync between TskSpiTx and SpiIrqhandler which caused RCs
// 2016-01-21 re-added usTCROffset[] calculation for slave mode hw-bug workarround
// 2016-02-04 removed usTCROffset[] calculation for slave mode hw-bug for tx irq
// 2016-02-04 added non-parallel irq event comparison for rx dection for tx irq
// 2016-02-10 fixed code exclusions for SPI_HWFIX_PDCRX define
// 2016-02-10 removed obsolete irq sync between spi unit tx & rx for slave mode
// 2016-02-10 removed obsolete data transfer of spi unit tx to rx for slave mode
// 2016-03-02 removed obsolete pdc/dma stop check & force restart code from TskSpiRx
// 2016-03-02 data rx interrupt disabled only for full pdc/dma circular buffer yet
// 2016-03-03 replaced AT91C_SPI_RXBUFF & ..._ENDRX by u32IrqRx for ..._TXBUFE in SpiIrqHandler
// 2016-03-03 moved AT91C_SPI_NSSR case to the end (but before msgs) of SpiIrqHandler
// 2016-03-03 added double bank pdc/dma support for latency free bank switching
// 2016-12-20 fixed existing comments and added new comments in SpiIrqHandler
// 2016-12-21 fixed data access conditions and enhanced comments in TskSpiRx
// 2017-01-18 fixed checks and access to circular buffers ucSpiAddrRx & ulSpiTx in TskSpiTx
// 2017-01-19 fixed two byte (9...16 bit) data support in TskSpiTx
// 2017-01-26 added odd job size support for two byte (9...16 bit) per pdc in TskSpiRx/Tx
// 2017-02-01 moved write to usSpiChannelCntRxQ after transfer to unit buffer in TskSpiTx
// 2017-02-01 moved write to ulSpiCntTx after transfer to unit buffer in TskSpiTx
// 2017-02-02 fixed auto-/cs-cycle fragmentation feature by usBytesPerXfer in TskSpiTx
// 2017-03-29 fixed two byte (9...16 bit) data support in SpiIrqHandler for Tx
// 2017-04-04 fixed AT91C_SPI_RXBUFF handler to support also SpiIrq Rx overrun
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <math.h>
#include <limits.h>
#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "spi.h"
#include "spiext.h"
#include "spidata.h"
#include "spidev.h"

//----------------------------------------------------------------------------
// void IsrSpi __irq
//----------------------------------------------------------------------------
// Each supported spi unit requires a function definition here
#if SPI_UNIT_MAX > 0
   SPI_ISR_FUNC(0);
#endif

#if SPI_UNIT_MAX > 1
   SPI_ISR_FUNC(1);
#endif

//----------------------------------------------------------------------------
// UCHAR SpiInitUnit
//----------------------------------------------------------------------------
UCHAR SpiInitUnit(UCHAR ucUnit, UCHAR ucChannel, U32 u32MR, OS_ID oiSemMsgNew, ULONG ulMsgMask)
{
   UCHAR ucResult= 0;
   
   U32 u32MRCurr;

   if (ucUnit < SPI_UNIT_MAX)
   {
      // Identification
      SSpiRW[ucUnit].ucUnit= ucUnit;
      
      // Channel count reset
      SSpiRW[ucUnit].ucChannelIdx= 0;
      
      // Circular array init that cannot be done by the macro itsself
      ulSpiTx[ucUnit].Size= SPI_BUFFERCNTTX;
      pulSpiTx[ucUnit]= &ulSpiTx[ucUnit];
      
      ulSpiCntTx[ucUnit].Size= SPI_BUFFERCNTTX;
      pulSpiCntTx[ucUnit]= &ulSpiCntTx[ucUnit];
      
#if SPI_HWFIX_PDCRX > 0      
      ucSpiAddrRx[ucUnit].Size= SPI_BUFFERCNTADDRRX;
      pucSpiAddrRx[ucUnit]= &ucSpiAddrRx[ucUnit];

      circular_reset(ucSpiAddrRx[ucUnit]);
#endif

      circular_reset(ulSpiTx[ucUnit]);
      circular_reset(ulSpiCntTx[ucUnit]);
      
      // Circular buffer handling for rx pdc
      SSpiRW[ucUnit].SCircular.ucSize= SPI_PDCDEPTH;
      SSpiRW[ucUnit].SCircular.ucCntEntries= 0;
      SSpiRW[ucUnit].SCircular.ucIdxRead= 0;
      SSpiRW[ucUnit].SCircular.ucIdxWrite= 0;
      
      OS_MBX_INIT_EX(MbxSpiRx[ucUnit]);
      OS_MBX_INIT_EX(MbxSpiMsg[ucUnit]);
      OS_MBX_INIT_EX(MbxSpiMsgChannel[ucUnit]);
            
      // Init
   
      // Clock
      AT91F_PMC_EnablePeriphClock(AT91C_BASE_PMC, 1 << SSpiRO[ucUnit].u32PeriphId);
      
      // Reset (see errata 41.3.8.7 of AT91SAM7X manual)
      AT91F_SPI_Reset(SSpiRO[ucUnit].pBase);
      AT91F_SPI_Reset(SSpiRO[ucUnit].pBase);
      
      // Config
      u32MRCurr= 0;
      u32MRCurr |= (u32MR | AT91C_SPI_PS);
      //-u32MR |= (u32MR | AT91C_SPI_PCS | AT91C_SPI_MODFDIS | AT91C_SPI_PCSDEC | AT91C_SPI_MSTR);
      
      AT91F_SPI_CfgMode(SSpiRO[ucUnit].pBase, u32MRCurr);
   
      // pdc
      //-AT91F_PDC_Close((AT91PS_PDC) &(SSpiRO[ucUnit].pBase->SPI_RPR));
          
      // Rx Peripheral DMA Controller (pdc)
      SSpiRO[ucUnit].pBase->SPI_PTCR= AT91C_PDC_RXTDIS;
      
      // Clear channel rx
      SSpiRO[ucUnit].pBase->SPI_RPR= 0L;
      SSpiRO[ucUnit].pBase->SPI_RCR= 0;   
   
      // Clear channel rx
      SSpiRO[ucUnit].pBase->SPI_RNPR= 0L;
      SSpiRO[ucUnit].pBase->SPI_RNCR= 0;
   
      SSpiRO[ucUnit].pBase->SPI_PTCR= AT91C_PDC_RXTEN;
      
      // Tx Peripheral DMA Controller (pdc)
      SSpiRO[ucUnit].pBase->SPI_PTCR= AT91C_PDC_TXTDIS;
      
      // Clear channel tx
      SSpiRO[ucUnit].pBase->SPI_TPR= 0L;
      SSpiRO[ucUnit].pBase->SPI_TCR= 0;
      
      // Set reload values for above channel tx
      SSpiRO[ucUnit].pBase->SPI_TNPR= 0L;
      SSpiRO[ucUnit].pBase->SPI_TNCR= 0;
      
      SSpiRO[ucUnit].pBase->SPI_PTCR= AT91C_PDC_TXTEN;
   
      // Semaphores
      os_sem_init(SSpiRW[ucUnit].SemMsg, 1);
      os_sem_init(SSpiRW[ucUnit].SemRx, 1);

      os_sem_init(SSpiRW[ucUnit].SemTx, 1);
      os_sem_init(SSpiRW[ucUnit].SemTickTx, 0);

      // Assign given semaphore and mask for new unit messages
      SSpiRW[ucUnit].oiSemMsgNew= oiSemMsgNew;
      SSpiRW[ucUnit].ulMsgMask= ulMsgMask;
      
      // Tasks
      
      SSpiRW[ucUnit].ucUnit= ucUnit;
      
      SSpiRW[ucUnit].tidTskMsg= os_tsk_create_ex(TskSpiMsg, SSpiRO[ucUnit].u8TskPriMsg, &SSpiRW[ucUnit]);
      SSpiRW[ucUnit].tidTskMsgChannel= os_tsk_create_ex(TskSpiMsgChannel, SSpiRO[ucUnit].u8TskPriMsg, &SSpiRW[ucUnit]);
      SSpiRW[ucUnit].tidTskRx= OS_TSK_CREATE_USER_EX(TskSpiRx, SSpiRO[ucUnit].u8TskPriRx, StkTskSpiRx[ucUnit], &SSpiRW[ucUnit]);
      SSpiRW[ucUnit].tidTskTx= OS_TSK_CREATE_USER_EX(TskSpiTx, SSpiRO[ucUnit].u8TskPriTx, StkTskSpiTx[ucUnit], &SSpiRW[ucUnit]);
   
      // Interrupt

      // State
      SSpiRW[ucUnit].ucIrqTxState= SPI_TXSTATE_START_TX;
      
      // Clear pending IRQs
      AT91F_AIC_ClearIt(AT91C_BASE_AIC, SSpiRO[ucUnit].u32PeriphId);
      
      // Tx IRQ is enabled later by TskSpiTx
      AT91F_SPI_EnableIt(SSpiRO[ucUnit].pBase, (SSpiRO[ucUnit].u32IrqMsg & ~(SSpiRO[ucUnit].u32IrqTx)));
      AT91F_SPI_DisableIt(SSpiRO[ucUnit].pBase, ~((SSpiRO[ucUnit].u32IrqMsg)) | (SSpiRO[ucUnit].u32IrqTx));
          
      // IRQ ctrl
      AT91F_AIC_ConfigureIt(AT91C_BASE_AIC, SSpiRO[ucUnit].u32PeriphId, 0, 0, IRQ_CAST(SSpiRO[ucUnit].Isr));
      AT91F_AIC_EnableIt(AT91C_BASE_AIC, SSpiRO[ucUnit].u32PeriphId);
          
      // Enable
      AT91F_SPI_Enable(SSpiRO[ucUnit].pBase);
  
      // Verify success
      if (SSpiRW[ucUnit].tidTskMsg && SSpiRW[ucUnit].tidTskMsgChannel && SSpiRW[ucUnit].tidTskRx && SSpiRW[ucUnit].tidTskTx)
      {
         SSpiRW[ucUnit].ucFlags= SPI_UNIT_FLAGS_CONFIGDONE;
         ucResult= 1;
      }
   }
      
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR SpiInitChannel
//----------------------------------------------------------------------------
UCHAR SpiInitChannel (UCHAR ucUnit, UCHAR ucChannel, U32 u32CSR, USHORT usBytesPerXfer, U16 u16MaxAge, UCHAR ucPrio, UCHAR ucFlags, OS_ID oiSemMsgNew, ULONG ulMsgMask)
{

   UCHAR ucResult= 0;
   
   UCHAR ucFlagsCurr;
   UCHAR ucCSRIdx;

   if (ucUnit < SPI_UNIT_MAX)
   {
      if ((ucChannel < SPI_CHANNELID_MAX) && (SSpiRW[ucUnit].ucChannelIdx < SPI_CHANNEL_MAX))
      {
         LOCKED(ucFlagsCurr= SSpiRW[ucUnit].ucFlags);
         
         if (ucFlagsCurr & SPI_UNIT_FLAGS_CONFIGDONE)
         {
            // Storage mapping
            SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx= SSpiRW[ucUnit].ucChannelIdx++;

            LOCKED(ucFlagsCurr= SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags);

            // Determine target hardware CSR index (group)
            ucCSRIdx= SPI_CHANNEL2CSRIDX(SSpiRO[ucUnit].pBase, ucChannel);
            
            if (!(ucFlagsCurr & SPI_CHANNEL_FLAGS_CONFIGDONE))
            {
               // Identification
               SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucUnit= ucUnit;
               
               // Circular array init that cannot be done by the macro itsself
               ucSpiChannelRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].Size= SPI_BUFFERCNTRX;
               pucSpiChannelRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]= &ucSpiChannelRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx];
               ucSpiChannelTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].Size= SPI_BUFFERCNTTX;
               pucSpiChannelTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]= &ucSpiChannelTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx];
               
               usSpiChannelCntRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].Size= SPI_BUFFERCNTRX;
               pusSpiChannelCntRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]= &usSpiChannelCntRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx];
               usSpiChannelCntRxQ[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].Size= SPI_BUFFERCNTRX;
               pusSpiChannelCntRxQ[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]= &usSpiChannelCntRxQ[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx];
               
               usSpiChannelCntTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].Size= SPI_BUFFERCNTTX;
               pusSpiChannelCntTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]= &usSpiChannelCntTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx];
               
               ucSpiChannelTagRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].Size= SPI_BUFFERCNTRX;
               pucSpiChannelTagRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]= &ucSpiChannelTagRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx];
               ucSpiChannelTagTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].Size= SPI_BUFFERCNTTX;
               pucSpiChannelTagTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]= &ucSpiChannelTagTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx];
                             
               circular_reset(ucSpiChannelRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]);
               circular_reset(ucSpiChannelTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]);
               
               circular_reset(usSpiChannelCntRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]);
               circular_reset(usSpiChannelCntRxQ[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]);
               
               circular_reset(usSpiChannelCntTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]);
               
               circular_reset(ucSpiChannelTagRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]);
               circular_reset(ucSpiChannelTagTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]);               

               os_sem_init(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemMsg, 1);
               os_sem_init(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemRx, 1);
               os_sem_init(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTx, 1);

               SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].usRxPendingCnt= 0;
               SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucRxPendingTag= 0;
               
               if (SSpiRO[ucUnit].pBase->SPI_MR & AT91C_SPI_MSTR)
               {
                  // Add AT91C_SPI_CSAAT for master channels (required although the LASTXFER feature works erroneously for all channels simultaneously only)
                  SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u32CSR= (u32CSR  | AT91C_SPI_CSAAT);
               }
               else
               {
                  SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u32CSR= u32CSR;
               }
                                 
               // Update target hardware CSR
               if (ucFlags & SPI_CHANNEL_FLAGS_CSRUPDATE)
               {
                  *(SSpiRO[ucUnit].pBase->SPI_CSR+ ucCSRIdx)= SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u32CSR;
               }
            }
            
            // Check for matching CSR value
            if (SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u32CSR == *(SSpiRO[ucUnit].pBase->SPI_CSR+ ucCSRIdx))
            {
               // Update channel para           
               SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].usBytesPerXfer= usBytesPerXfer;
               SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u16MaxAge= u16MaxAge;
               SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucPrio= ucPrio;
               SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags= SPI_CHANNEL_FLAGS_CONFIGDONE;
               SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].oiSemMsgNew= oiSemMsgNew;
               SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ulMsgMask= ulMsgMask;
               
               // Update unit para
               SSpiRW[ucUnit].ucPrioMask |= ucPrio;
               
               ucResult= 1;
            }
         }
      }
   }
      
   return (ucResult); 
}

//----------------------------------------------------------------------------
// ULONG SpiMsgUnitCheck
//----------------------------------------------------------------------------
ULONG SpiMsgUnitCheck(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsgMask, U16 u16Timeout)
{
   ULONG ulResult= 0;
   
   ULONG ulMsg;
   UCHAR ucBitPos;
   
   for (ucBitPos= 0; ucBitPos < SPI_UNIT_MSGMAX; ucBitPos++)
   {
      ulMsg= (1 << ucBitPos);
      if (ulMsgMask & ulMsg)
      {
         ulResult += SpiMsgUnit(ucUnit, ucChannel, ulMsg, 0, u16Timeout);
      }
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// USHORT SpiMsgUnit
//----------------------------------------------------------------------------
USHORT SpiMsgUnit(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   USHORT usCounter;
   UCHAR ucIndex;
      
   if (ucUnit < SPI_UNIT_MAX)
   {
      RESCHEDULE_TICKS(0);
      
      // Lock counters
      if (OS_SEM_TAKE_CHK(os_sem_take(SSpiRW[ucUnit].SemMsg, u16Timeout)))
      {    
         for (ucIndex= 0; ucIndex < SPI_UNIT_MSGMAX; ucIndex++)
         {
            if ((1 << ucIndex) & ulMsg)
            {
               usCounter= SSpiRW[ucUnit].xMsgCounter[ucIndex];
               if (usCounter)
               {
                  RAISEMAX(usResult, usCounter, USHRT_MAX);
   
                  if (usDecrement) 
                  {
                     if (usDecrement < usCounter)
                     {
                        SSpiRW[ucUnit].xMsgCounter[ucIndex] -= usDecrement;
                     }
                     else
                     {   
                        SSpiRW[ucUnit].xMsgCounter[ucIndex]= 0;
                     }
                  }
               }
            }
         }

         // Unlock counters
         os_sem_give(SSpiRW[ucUnit].SemMsg);   
      }
   }
         
   return (usResult);
}

//----------------------------------------------------------------------------
// ULONG SpiMsgChannelNext
//----------------------------------------------------------------------------
ULONG SpiMsgChannelNext(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsgMask, OS_ID oiSemMsgNew, U16 u16Timeout)
{
   ULONG ulResult= 0;
   
   ulResult= SpiMsgChannelCheck(ucUnit, ucChannel, ulMsgMask, u16Timeout);
   if (ulResult)
   {
      // Give semaphore again for messages of other interest (but don't increment the semaphore above one)
      if (oiSemMsgNew)
      {
         if (!OS_SEM_TAKE_CHK(os_sem_check(oiSemMsgNew, FALSE)))
         {
            os_sem_give(oiSemMsgNew);
         }
         UNLOCK;
      }
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// ULONG SpiMsgChannelCheck
//----------------------------------------------------------------------------
ULONG SpiMsgChannelCheck(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsgMask, U16 u16Timeout)
{
   ULONG ulResult= 0;
   
   ULONG ulMsg;
   UCHAR ucBitPos;
   
   // Force scheduler to run other tasks yet
   RESCHEDULE_TICKS(0);
   
   for (ucBitPos= 0; ucBitPos < SPI_CHANNEL_MSGMAX; ucBitPos++)
   {
      ulMsg= (1 << ucBitPos);
      if (ulMsgMask & ulMsg)
      {
         ulResult += SpiMsgChannel(ucUnit, ucChannel, ulMsg, 0, u16Timeout);
      }
   }
   
   return (ulResult);
}

//----------------------------------------------------------------------------
// USHORT SpiMsgChannel
//----------------------------------------------------------------------------
USHORT SpiMsgChannel(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   USHORT usCounter;
   UCHAR ucIndex;
      
   if (ucUnit < SPI_UNIT_MAX)
   {
      if (ucChannel < SPI_CHANNELID_MAX)
      {
         // Lock counters
         if (OS_SEM_TAKE_CHK(os_sem_take(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemMsg, u16Timeout)))
         {    
            for (ucIndex= 0; ucIndex < SPI_CHANNEL_MSGMAX; ucIndex++)
            {
               if ((1 << ucIndex) & ulMsg)
               {
                  usCounter= SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].xMsgCounter[ucIndex];
                  if (usCounter)
                  {
                     RAISEMAX(usResult, usCounter, USHRT_MAX);
   
                     if (usDecrement)
                     {
                        if (usDecrement < usCounter)
                        {
                           SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].xMsgCounter[ucIndex] -= usDecrement;
                        }
                        else
                        {
                           SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].xMsgCounter[ucIndex]= 0;
                        }
                     }
                  }
               }
            }

            // Unlock counters
            os_sem_give(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemMsg);
         }
      }
   }

   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT SpiRx
//----------------------------------------------------------------------------
USHORT SpiRx(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout)
{
   return (SpiRxTag(ucUnit, ucChannel, pvData, usCount, u16Timeout, NULL));
}

//----------------------------------------------------------------------------
// USHORT SpiRxTag
//----------------------------------------------------------------------------
USHORT SpiRxTag(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout, UCHAR *pucTag)
{
   USHORT usResult= 0;
   USHORT usResult2= 1;
   USHORT usResultTag= 0;
   
   USHORT usCountJob;
   UCHAR ucTagJob;
   UCHAR ucFlags;

   if (ucUnit < SPI_UNIT_MAX)
   {
      if (ucChannel < SPI_CHANNELID_MAX)
      {
         LOCKED(ucFlags= SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags);
      
         if (ucFlags & SPI_CHANNEL_FLAGS_CONFIGDONE)
         {
            // Lock channel buffer rx
            if (OS_SEM_TAKE_CHK(os_sem_take(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemRx, u16Timeout)))
            {
               if (!SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].usRxPendingCnt)
               {
                  usResult= circular_read(usSpiChannelCntRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], (USHORT *)&usCountJob, 1);
                  if (usResult > 0)
                  {
                     // Preset ucTagJob
                     ucTagJob= SPI_USERTAG_DEFAULT;
                     
                     // Attempt to read new tag and update ucTagJob
                     usResultTag= circular_read(ucSpiChannelTagRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], (UCHAR *)&ucTagJob, 1);
                     
#ifdef SPI_DEBUG
                     if (!usResultTag)
                     {
                        usResultTag= usResultTag;
                     }
#endif
                     // Accept empty taq queue for unspecified or default tag
                     if (!pucTag) usResultTag= 1;
                     if (pucTag && (*pucTag == SPI_USERTAG_DEFAULT)) usResultTag= 1;
                  }
               }
               else
               {
                  usCountJob= SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].usRxPendingCnt;
                  ucTagJob= SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucRxPendingTag;
                  usResult= 1;
                  usResultTag= 1;
               }

               if (usResult > 0 && usResultTag > 0)
               {
                  // Retrieve data for matching or unspecified tag data only
                  if ((!pucTag) ? (TRUE) : ((*pucTag == ucTagJob) || (*pucTag == SPI_USERTAG_DEFAULT)) ? (TRUE) : (FALSE))
                  {                     
                     if (pvData)
                     {
                        // Read buffer
                        usResult= circular_read(ucSpiChannelRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], (UCHAR *)pvData, _umin(usCountJob, usCount));
                     }
                     else
                     {
                        // Wipe buffer (read without target memory)
                        usResult= circular_wipe(ucSpiChannelRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], _umin(usCountJob, usCount));
                     }
                                         
                     // Handle partially read job
                     usCount= circular_countEntries(ucSpiChannelRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]);
                     
#ifdef SPI_DEBUG
                     // Debug
                     if (usCount)
                     {
                        if (ucChannel == 4)
                        {
                           if (!circular_countEntries(usSpiChannelCntRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]) || !circular_countEntries(ucSpiChannelTagRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]))
                           {
                              usCount= usCount;
                           }
                        }
                     }
#endif
                     if ((usResult < usCountJob) && usCount)
                     {
#ifdef SPI_DEBUG
                        // Debug
                        if (ucChannel == 4)
                        {
                           ucChannel= ucChannel;
                        }
#endif
                        SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].usRxPendingCnt= _min((usCountJob- usResult), usCount);
                        SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucRxPendingTag= ucTagJob;
                     }
                     else
                     {
                        SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].usRxPendingCnt= 0;
                        SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucRxPendingTag= 0;
                     }
                  }
                  else
                  {
                     if (!SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].usRxPendingCnt)
                     {
                        circular_unRead(usSpiChannelCntRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], 1);
                        circular_unRead(ucSpiChannelTagRx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], 1);
                        
                        usResult2= 0;
                     }
                  }
               }
               
               // Write user tag data
               if (pucTag) *pucTag= (usResult > 0 && usResultTag > 0) ? (ucTagJob) : (0);
               
               // Unlock channel buffer rx
               os_sem_give(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemRx);
            }
         }
      }
   }
      
   return (usResult* usResult2);
}

//----------------------------------------------------------------------------
// USHORT SpiTx
//----------------------------------------------------------------------------
USHORT SpiTx(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout)
{
   return (SpiTxTag(ucUnit, ucChannel, pvData, usCount, u16Timeout, SPI_USERTAG_DEFAULT));
}

//----------------------------------------------------------------------------
// USHORT SpiTxTag
//----------------------------------------------------------------------------
USHORT SpiTxTag(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout, UCHAR ucTag)
{
   USHORT usResult= 0;
   USHORT usResultCnt= 0;
   USHORT usResultTag= 0;
   
   UCHAR ucFlags;
   
#ifdef SPI_DEBUG
   if (!ucUnit)
   {
      ucUnit= ucUnit;
   }
   else
   {
      ucUnit= ucUnit;
   }
#endif
      
   if (ucUnit < SPI_UNIT_MAX)
   {
      if (ucChannel < SPI_CHANNELID_MAX)
      {
         // The LASTXFER bug prevents fragmented transfers!!!
         if (usCount <= (SPI_PDCMAX* SPI_PDCITEMBYTES(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u32CSR)))
         {
            LOCKED(ucFlags= SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags);
            
            if (ucFlags & SPI_CHANNEL_FLAGS_CONFIGDONE)
            {
               if (TRUE) //-if (pvData) // Always true for "no-source" feature of support.c
               {                  
                  // Lock channel buffer tx
                  if (OS_SEM_TAKE_CHK(os_sem_take(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTx, u16Timeout)))
                  {
                     // Check for free space in all required circular buffers
                     if (circular_freeWrite(ucSpiChannelTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]) >= usCount)
                     {
                        if (circular_freeWrite(usSpiChannelCntTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]) >= 1)
                        {
                           if (circular_freeWrite(ucSpiChannelTagTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]) >= 1)
                           {                  
                              // Write data to buffer (succeeds only when all data fits yet)
                              usResult= circular_write(ucSpiChannelTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], (UCHAR *)pvData, usCount);
                              if (usResult >= usCount)
                              {
                                 // Write count of data to buffer
                                 usResultCnt= circular_write(usSpiChannelCntTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], (USHORT *)&usResult, 1);
                                 if (usResultCnt >= 1)
                                 {
                                    // Write user tag data to buffer
                                    usResultTag= circular_write(ucSpiChannelTagTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], (UCHAR *)&ucTag, 1);
                                    if (usResultTag >= 1)
                                    {
                                       // Indicate queued data for the channel scheduler fast check
                                       LOCKED(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags |=  SPI_CHANNEL_FLAGS_DATAQUEUEDTX);
                                    }
                                 }
                              }
                           }
                        }
                     }
                     
                     // Unlock channel buffer tx
                     os_sem_give(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTx);
                     
                     // Trigger TskSpiTx now
                     if (!OS_SEM_TAKE_CHK(os_sem_check(SSpiRW[ucUnit].SemTickTx, FALSE)))
                     {
                        if (SSpiRW[ucUnit].SemTickTx) os_sem_give(SSpiRW[ucUnit].SemTickTx);
                     }
                  }
               }
            }
         }
      }
   }
      
   return (usResult* usResultCnt* usResultTag);
}

//----------------------------------------------------------------------------
// USHORT SpiTxLock
//----------------------------------------------------------------------------
USHORT SpiTxLock(UCHAR ucUnit, UCHAR ucChannel, USHORT usCount, U16 u16Timeout)
{
   USHORT usResult= 0;
   
   UCHAR ucFlags;
   
   if (ucUnit < SPI_UNIT_MAX)
   {
      if (ucChannel < SPI_CHANNELID_MAX)
      {
         // The LASTXFER bug prevents fragmented transfers!!!
         if (usCount <= (SPI_PDCMAX* SPI_PDCITEMBYTES(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].u32CSR)))
         {
            // Lock channel buffer tx                 
            if (OS_SEM_TAKE_CHK(os_sem_take(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTx, u16Timeout)))
            {
               LOCK;
               {
                  ucFlags= SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags;
                  
                  if (ucFlags & SPI_CHANNEL_FLAGS_CONFIGDONE)
                  {
                     if (!(ucFlags & SPI_CHANNEL_FLAGS_QUEUELOCKED))
                     {
                        // Indicate channel locked
                        SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags |= SPI_CHANNEL_FLAGS_QUEUELOCKED;
                        usResult= usCount;
                     }
                  }
               }            
               UNLOCK;
               
               // Unlock channel buffer tx
               os_sem_give(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTx);
            }
         }
      }
   }

   return (usResult);
}

//----------------------------------------------------------------------------
// USHORT SpiTxUnlock
//----------------------------------------------------------------------------
USHORT SpiTxUnlock(UCHAR ucUnit, UCHAR ucChannel, USHORT usCount, U16 u16Timeout)
{
   return (SpiTxTagUnlock(ucUnit, ucChannel, usCount, u16Timeout, SPI_USERTAG_DEFAULT));
}

//----------------------------------------------------------------------------
// USHORT SpiTxTagUnlock
//----------------------------------------------------------------------------
USHORT SpiTxTagUnlock(UCHAR ucUnit, UCHAR ucChannel, USHORT usCount, U16 u16Timeout, UCHAR ucTag)
{
   USHORT usResultCnt= 0;
   USHORT usResultTag= 0;
   
   UCHAR ucFlags;
   
   if (ucUnit < SPI_UNIT_MAX)
   {
      if (ucChannel < SPI_CHANNELID_MAX)
      {
         // Lock channel buffer tx                 
         if (OS_SEM_TAKE_CHK(os_sem_take(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTx, u16Timeout)))
         {
            // Check required flags
            LOCK;
            {
               ucFlags= SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags;
               if (ucFlags & (SPI_CHANNEL_FLAGS_CONFIGDONE | SPI_CHANNEL_FLAGS_QUEUELOCKED))
               {
                  // Check for free space in all required circular buffers
                  if (circular_freeWrite(usSpiChannelCntTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]) >= 1)
                  {
                     if (circular_freeWrite(ucSpiChannelTagTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx]) >= 1)
                     {
                        // Write specified count of data to buffer
                        usResultCnt= circular_write(usSpiChannelCntTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], (USHORT *)&usCount, 1);
                        if (usResultCnt >= 1)
                        {
                           // Write user tag data to buffer
                           usResultTag= circular_write(ucSpiChannelTagTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], (UCHAR *)&ucTag, 1);
                           if (usResultTag >= 1)
                           {
                              // Remove queue locked flag
                              SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags &= ~SPI_CHANNEL_FLAGS_QUEUELOCKED;
                              
                              // Indicate queued data for the channel scheduler fast check
                              SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags |= SPI_CHANNEL_FLAGS_DATAQUEUEDTX;
                           }
                        }
                     }
                  }
               }
            }
            UNLOCK;
               
            // Unlock channel buffer tx
            os_sem_give(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTx);
         }
            
         if (usResultTag >= 1)
         {
            // Trigger TskSpiTx now
            if (!OS_SEM_TAKE_CHK(os_sem_check(SSpiRW[ucUnit].SemTickTx, FALSE)))
            {
               if (SSpiRW[ucUnit].SemTickTx) os_sem_give(SSpiRW[ucUnit].SemTickTx);
            }
         }
      }
   }
      
   return (usCount * usResultCnt);
}

//----------------------------------------------------------------------------
// USHORT SpiTxQueue
//----------------------------------------------------------------------------
USHORT SpiTxQueue(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout)
{
   return (SpiTxTagQueue(ucUnit, ucChannel, pvData, usCount, u16Timeout, SPI_USERTAG_DEFAULT));
}

//----------------------------------------------------------------------------
// USHORT SpiTxTagQueue
//----------------------------------------------------------------------------
USHORT SpiTxTagQueue(UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout, UCHAR ucTag)
{
   USHORT usResult= 0;
   
   UCHAR ucFlags;
   
   if (ucUnit < SPI_UNIT_MAX)
   {
      if (ucChannel < SPI_CHANNELID_MAX)
      {
         // Lock channel buffer tx                 
         if (OS_SEM_TAKE_CHK(os_sem_take(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTx, u16Timeout)))
         {  
            LOCK;
            {
               ucFlags= SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].ucFlags;            
               if (ucFlags & (SPI_CHANNEL_FLAGS_CONFIGDONE | SPI_CHANNEL_FLAGS_QUEUELOCKED))
               {
                  if (TRUE) //-if (pvData) // Always true for "no-source" feature of support.c
                  {                    
                     // Write data to buffer (succeeds only when all data fits yet)
                     usResult= circular_write(ucSpiChannelTx[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx], (UCHAR *)pvData, usCount);
                  }
               }
            }
            UNLOCK;
            
            // Unlock channel buffer tx
            os_sem_give(SSpiChannelRW[ucUnit][SSpiChannelMapRW[ucUnit][ucChannel].ucStorageIdx].SemTx);
         }
      }
   }
      
   return (usResult);
}

//----------------------------------------------------------------------------
// __task void TskSpiMsg
//----------------------------------------------------------------------------
__task void TskSpiMsg(void *pvParam)
{
   struct _SSpiRW *pSSpiRW= (struct _SSpiRW *)pvParam;
   
   ULONG ulValue;
   USHORT usCount;
   UCHAR ucIndex;
 
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxSpiMsg[pSSpiRW->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         usCount= MSG_GETCNT(SPI_UNIT_MSG_MSGBITS, ulValue);
         if (usCount)
         {
            // Lock unit counters
            if (OS_SEM_TAKE_CHK(os_sem_take(SSpiRW[pSSpiRW->ucUnit].SemMsg, OS_T_FOREVER)))
            {
               // Check message bits
               for (ucIndex= 0; ucIndex < SPI_UNIT_MSGMAX; ucIndex++)
               {
                  if (ulValue & (1 << ucIndex))
                  {
                     // Count until max-1 and set max to show overrun condition
                     RAISEMAX(SSpiRW[pSSpiRW->ucUnit].xMsgCounter[ucIndex], usCount, SPI_MSGCOUNTERMAX);
                  }
               }
            
               // Unlock unit counters
               os_sem_give(SSpiRW[pSSpiRW->ucUnit].SemMsg);
            }

            // New message notification
            if (SSpiRW[pSSpiRW->ucUnit].oiSemMsgNew)
            {
               if (!OS_SEM_TAKE_CHK(os_sem_check(SSpiRW[pSSpiRW->ucUnit].oiSemMsgNew, FALSE)))
               {
                  os_sem_give(SSpiRW[pSSpiRW->ucUnit].oiSemMsgNew);
               }
               UNLOCK;
            }
         }
      }
   }

   DETACH;
}

//----------------------------------------------------------------------------
// __task void TskSpiChannelMsg
//----------------------------------------------------------------------------
__task void TskSpiMsgChannel(void *pvParam)
{
   struct _SSpiRW *pSSpiRW= (struct _SSpiRW *)pvParam;
   
   ULONG ulValue;
   ULONG ulCntCh;
   USHORT usCount;
   
   UCHAR ucIndex;
   UCHAR ucChannel;
 
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxSpiMsgChannel[pSSpiRW->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         ulCntCh= MSG_GETCNT(SPI_CHANNEL_MSG_MSGBITS, ulValue);
         ucChannel= (ulCntCh & 0x000F0000) >> BITS_OF(USHORT);
         usCount= (ulCntCh & BITS_ULONGLX);

         if (usCount)
         {
            if (ucChannel < SPI_CHANNELID_MAX)
            {
               // Lock channel counters
               if (OS_SEM_TAKE_CHK(os_sem_take(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].SemMsg, OS_T_FOREVER)))
               {
                  // Check message bits
                  for (ucIndex= 0; ucIndex < SPI_CHANNEL_MSGMAX; ucIndex++)
                  {
                     if (ulValue & (1 << ucIndex))
                     {
                        // Count until max-1 and set max to show overrun condition
                        RAISEMAX(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].xMsgCounter[ucIndex], usCount, SPI_MSGCOUNTERMAX);
                     }
                  }
                  
                  // Unlock channel counters
                  os_sem_give(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].SemMsg);
               }
                           
               // New message notification (but don't increment the semaphore above one)
               if (SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].oiSemMsgNew)
               {
                  if (!OS_SEM_TAKE_CHK(os_sem_check(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].oiSemMsgNew, FALSE)))
                  {
                     os_sem_give(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].oiSemMsgNew);
                  }
                  UNLOCK;
               }
            }
         }
      }
   }

   DETACH;
}

//----------------------------------------------------------------------------
// __task void TskSpiRx
//----------------------------------------------------------------------------
__task void TskSpiRx(void *pvParam)
{
   struct _SSpiRW *pSSpiRW= (struct _SSpiRW *)pvParam;
    
   ULONG ulMsg;
   ULONG ulMsgMask;
   ULONG ulValue;
   ULONG ulData;

   USHORT usCount;
   USHORT usCountPCS;
   USHORT usItem;
   USHORT usItemCnt;
   USHORT usItemBytes;
   USHORT usBytesDone;
   USHORT usBytesFail;
   USHORT usBytesTotal;
   USHORT usBytesPerJob;
   USHORT usResult;
   USHORT usResult2;
   
   UCHAR ucChannelCurrLock;
   UCHAR ucChannel;
   UCHAR ucIdxRead;
   UCHAR ucFlags;
   
   bool bLock;
   bool bMatch;
   bool bNew;
   
   // Disable warning
   usResult2= usResult2;
     
   FOREVER
   {
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxSpiRx[pSSpiRW->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         usCount= MSG_GETCNT(SPI_UNIT_DATA_MSGBITS, ulValue);
         ulData= MSG_GETMSG(SPI_UNIT_DATA_MSGBITS, ulValue);
         ucIdxRead= (ulData & BITS_ULONGHL) >> BITS_OF(USHORT);
         usItemCnt= ulData & BITS_USHORT;
         
#ifdef SPI_DEBUG
         if (!pSSpiRW->ucUnit)
         {
            pSSpiRW->ucUnit= pSSpiRW->ucUnit;
         }
         else
         {
            pSSpiRW->ucUnit= pSSpiRW->ucUnit;
         }        
#endif

         if (usCount && usItemCnt)
         {
            // Lock unit rx
            if (OS_SEM_TAKE_CHK(os_sem_take(SSpiRW[pSSpiRW->ucUnit].SemRx, OS_T_FOREVER)))
            {
               // Clear compare channel storage (0 is a valid id!)
               ucChannelCurrLock= SPI_CHANNELID_MAX;
   
               for (bLock= FALSE, bMatch= FALSE, bNew= FALSE, usItem= 0; usItem < usItemCnt; usItem++)
               {
                  // Determine channel
                  
#if SPI_HWFIX_PDCRX > 0
                  if (SSpiRO[pSSpiRW->ucUnit].pBase->SPI_MR & AT91C_SPI_MSTR)
                  {
                     // This information is set by TskSpiTx and replaces the sporadically wrong information set by spi pdc (esp. in master mode)
                     ucChannel= SPI_CHANNELID_MAX;
                     usResult2= circular_read(ucSpiAddrRx[pSSpiRW->ucUnit], &ucChannel, 1);
                  }
                  else
#endif
                  {
                     // This information is sporadically set wrong (channel change too early/late) by spi pdc (esp. in master mode)
                     ucChannel= (SSpiRO[pSSpiRW->ucUnit].pBase->SPI_MR & AT91C_SPI_MSTR) ? (SPI_PCS2CHANNEL(SSpiRO[pSSpiRW->ucUnit].pBase, ((SSpiRW[pSSpiRW->ucUnit].ulPDCBufferRx[ucIdxRead][usItem] & AT91C_SPI_RPCS) >> BITS_OF(USHORT)))) : (0);
                  }
                  
                  // Check ucChannel range for array access here
                  if (ucChannel < SPI_CHANNELID_MAX)
                  {
                     bMatch= (ucChannel == ucChannelCurrLock);
                     if (!bMatch)
                     {
                        bNew= TRUE;
                        if (ucChannelCurrLock < SPI_CHANNELID_MAX)
                        {
                           // Unlock previous channel buffer rx
                           if (!OS_SEM_TAKE_CHK(os_sem_check(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurrLock].ucStorageIdx].SemRx, FALSE)))
                           {
                              os_sem_give(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurrLock].ucStorageIdx].SemRx);
                           }
                           else
                           {
                              // Debug
                              bNew= bNew;
                           }
                           UNLOCK;
                           
                           // Clear compare channel storage (0 is a valid id!)
                           ucChannelCurrLock= SPI_CHANNELID_MAX;
                           bLock= FALSE;
                        }
                           
                        // Check new channel's configuration
                        LOCKED(ucFlags= SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].ucFlags);
                        if (ucFlags & SPI_CHANNEL_FLAGS_CONFIGDONE)
                        {
                           // Lock channel buffer rx
                           bLock= OS_SEM_TAKE_CHK(os_sem_take(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].SemRx, OS_T_FOREVER));
                           if (bLock)
                           {
                              // Update current channel lock info for future channel change
                              ucChannelCurrLock= ucChannel;
   
                              // Debug                           
                              if (OS_SEM_TAKE_CHK(os_sem_check(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].SemRx, TRUE)))
                              {
                                 bNew= bNew;
                              }
                           }
                           else
                           {
                              // Debug
                              bNew= bNew;
                           }
                        }
                        else
                        {
                           bLock= FALSE;
                        }
                     }
                     
                     if (bNew)
                     {
                        bNew= FALSE;
                        
                        // Determine bytes per PCS
                        usCountPCS= SPI_PDCITEMBYTES(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].u32CSR);
                        
                        // Read size of this new job
                        usBytesPerJob= 0;
                        usResult2= circular_read(usSpiChannelCntRxQ[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx], &usBytesPerJob, 1);
                        if (usResult2 >= 1)
                        {
                           // Increase odd usBytesPerJob for 9...16 bit PCS
                           usBytesPerJob= (usCountPCS == 1) ? (usBytesPerJob)  \
                                                            : ( /* usCountPCS == 2 */ \
                                                                  (usBytesPerJob & 0x0001)   ? (usBytesPerJob+ 1) \
                                                                                             : (usBytesPerJob) \
                                                              );
                        }
                        
                        usBytesDone= 0;
                        usBytesFail= 0;
                     }
                     
                     if (bLock /*|| bMatch*/)
                     {
                        // Retrieve message mask
                        LOCKED(ulMsgMask= SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].ulMsgMask);

                        // Check for free space in all required circular buffers
                        if (circular_freeWrite(ucSpiChannelRx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx]) >= usCountPCS)
                        {
                           if (circular_freeWrite(usSpiChannelCntRx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx]) >= 1)
                           {
                              // Distribute data to channel
                              usResult= 0;
                              usResult= circular_write(ucSpiChannelRx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx], ((UCHAR *)&SSpiRW[pSSpiRW->ucUnit].ulPDCBufferRx[ucIdxRead][usItem]), usCountPCS);
                              usBytesDone += usResult;
                              usBytesFail += (usCountPCS- usResult);
                              
                              // Limit job data to job size (and pdc block size - especially for slave mode) - also force job closing at last item
                              usBytesTotal= usBytesDone+ usBytesFail;
                              usItemBytes= usItemCnt* usCountPCS;
                              if ((usBytesPerJob && (usBytesTotal >= usBytesPerJob)) || (usBytesTotal >= usItemBytes) || ((usItem+ 1) >= usItemCnt))
                              {
                                 // Write successful done job bytes
                                 usResult2= circular_write(usSpiChannelCntRx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx], (USHORT *)&usBytesDone, 1);
                                 if (usResult2 >= 1)
                                 {
                                    // Send msg to counter task
                                    ulMsg= (SSpiRO[pSSpiRW->ucUnit].u32IrqRx | SSpiRO[pSSpiRW->ucUnit].u32IrqTx);
                                    
                                    // Apply message filter
                                    ulMsg&= ulMsgMask;

                                    // Apply max counter index available
                                    ulMsg&= ((1 << (SPI_CHANNEL_MSGMAX+ 1))- 1);
                                    
                                    if (ulMsg)
                                    {
                                       os_mbx_send_safe(&MbxSpiMsgChannel[pSSpiRW->ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(SPI_CHANNEL_MSG_MSGBITS, ulMsg, ((ULONG)ucChannel << BITS_OF(USHORT)) | usBytesDone)), OS_T_FOREVER);
                                       RESCHEDULE_TICKS(0);
                                    }
                                 }
                                 
                                 // Trigger reading size of next new job because matching will not trigger this above (we stay on the same unit and channel!)
                                 if (usBytesTotal >= usBytesPerJob)
                                 {
                                    bNew= TRUE;                                    
                                 }
                                 
                                 // Debug
                                 if (!pSSpiRW->ucUnit)
                                 {
                                    if (usBytesPerJob != usBytesDone)
                                    {
                                       usBytesPerJob= usBytesPerJob;
                                       usBytesDone= usBytesDone;
                                    }
                                 }
                              }
                              
                              if (usResult != usCountPCS)
                              {                                                           
                                 // Send msg AT91C_SPI_OVRES to this channel counter
                                 ulMsg= AT91C_SPI_OVRES;
                                 
                                 // Apply message filter
                                 ulMsg&= ulMsgMask;

                                 // Apply max counter index available
                                 ulMsg&= ((1 << (SPI_CHANNEL_MSGMAX+ 1))- 1);

                                 if (ulMsg)
                                 {
                                    os_mbx_send_safe(&MbxSpiMsgChannel[pSSpiRW->ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(SPI_CHANNEL_MSG_MSGBITS, ulMsg, ((ULONG)ucChannel << BITS_OF(USHORT)) | (usCountPCS- usResult))), OS_T_FOREVER);
                                    RESCHEDULE_TICKS(0);
                                 }
                              }
                           }
                        }
                     }
                  }
               }
              
               if (bLock)
               {
                  if (!OS_SEM_TAKE_CHK(os_sem_check(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].SemRx, FALSE)))
                  {
                     // Unlock last active channel buffer rx
                     os_sem_give(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannel].ucStorageIdx].SemRx);
                  }
                  else
                  {
                     // Debug
                     bLock= bLock;
                  }
                  UNLOCK;
               
                  // Clear unlock request inside loop above (0 is a valid id!)
                  ucChannelCurrLock= SPI_CHANNELID_MAX;
                  
                  bLock= FALSE;
               }
               
               // Unlock unit rx
               os_sem_give(SSpiRW[pSSpiRW->ucUnit].SemRx);
            }
         }
         
         // Enable the rx irq again
         AT91F_SPI_EnableIt(SSpiRO[pSSpiRW->ucUnit].pBase, SSpiRO[pSSpiRW->ucUnit].u32IrqRx);
      }

      // Limit maximum check cycle speed
      if (SSpiRO[pSSpiRW->ucUnit].u16DelayRx) RESCHEDULE_TICKS(SSpiRO[pSSpiRW->ucUnit].u16DelayRx);
   }

   DETACH;
}

//----------------------------------------------------------------------------
// __task void TskSpiTx
//----------------------------------------------------------------------------
__task void TskSpiTx(void *pvParam)
{
   struct _SSpiRW *pSSpiRW= (struct _SSpiRW *)pvParam;
   
   static bool bRunTx[SPI_UNIT_MAX]= { FALSE };
   static bool bQueueTx[SPI_UNIT_MAX]= { FALSE };
   static bool bCSR32[SPI_UNIT_MAX][SPI_UNIT_CSR32COUNT]= { FALSE };
   
   register USHORT usCount;
   
   U32   u32CSR;
   ULONG ulPDCItem;
   ULONG ulMsgMask;
   ULONG ulCntTx;
   ULONG ulMsg;
   
   USHORT usIndex;
   USHORT usResult;
   U16    u16MaxAgeTx;
   U16    u16AgeTx;
   USHORT usPDCItemsPerXferJob;
   USHORT usBytesPerPDCItem;
   USHORT usBytesPerXferJob;   
   USHORT usBytesPerXfer;
   USHORT usByteCount;
   USHORT usDataQueuedTx;
   
   UCHAR ucChannelCurr;
   UCHAR ucPrioCurr;
   UCHAR ucTagJob;
   
   UCHAR ucPrioMask;
   UCHAR ucPrio;
   UCHAR ucFlags;
   UCHAR ucCSRIdx;
   UCHAR ucPCS;

   FOREVER
   {
#ifdef SPI_DEBUG
      if (!pSSpiRW->ucUnit)
      {
         pSSpiRW->ucUnit= pSSpiRW->ucUnit;
      }
      else
      {
         pSSpiRW->ucUnit= pSSpiRW->ucUnit;
      }        
#endif

      // Start pdc for any queued pdc data
      
      // Clear queue flag
      bQueueTx[pSSpiRW->ucUnit]= FALSE;
      
      // Lock unit buffer tx (include pending until ISR complete)
      if (OS_SEM_TAKE_CHK(os_sem_take(SSpiRW[pSSpiRW->ucUnit].SemTx, OS_T_FOREVER)))
      {
         // Check for bytes to send
         usCount= circular_countEntries(ulSpiCntTx[pSSpiRW->ucUnit]);
         if (usCount)
         {
            bRunTx[pSSpiRW->ucUnit]= TRUE;
            
            // Enable tx
            AT91F_SPI_EnableIt(SSpiRO[pSSpiRW->ucUnit].pBase, SSpiRO[pSSpiRW->ucUnit].u32IrqTx);
            
            // Disable Tx and unlocking unit buffer (after pdc buffer size bytes) located inside ISR
         }
         else
         {
            if (bRunTx[pSSpiRW->ucUnit])
            {
               bRunTx[pSSpiRW->ucUnit]= FALSE;
               
               // Retrieve current msg mask
               LOCKED(ulMsgMask= SSpiRW[pSSpiRW->ucUnit].ulMsgMask);

               // Add irq without data action to messages
               ulMsg= SSpiRO[pSSpiRW->ucUnit].u32IrqTx;
               
               // Apply message filter
               ulMsg&= ulMsgMask;

               // Apply max counter index available
               ulMsg&= ((1 << (SPI_UNIT_MSGMAX+ 1))- 1);

               if (ulMsg)
               {
                  os_mbx_send_safe(&MbxSpiMsg[pSSpiRW->ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(SPI_UNIT_MSG_MSGBITS, ulMsg, 1)), _umax(1, SSpiRO[pSSpiRW->ucUnit].u16DelayTx));
                  RESCHEDULE_TICKS(0);
               }
            }
            
            // Unlocking unit buffer (after pdc buffer size bytes) located at end of queue code below
         }
         
         // Queue new data for stopped pdc only (SPI_CSR[...] registers are not allowed to change during active pdc)
         if (!bRunTx[pSSpiRW->ucUnit])
         {
            // Check channels for new data to queue into pdc data buffer
      
            // Clear CSR32 allocation map
            for (usIndex= 0; usIndex < SPI_UNIT_CSR32COUNT; usIndex++)
            {
               bCSR32[pSSpiRW->ucUnit][usIndex]= FALSE;
            }
            
            for (ucChannelCurr= 0; ucChannelCurr < SPI_CHANNELID_MAX; ucChannelCurr++)
            {
               // Safely access ucPrioMask and update local copy
               LOCKED(ucPrioMask= SSpiRW[pSSpiRW->ucUnit].ucPrioMask);
               
               // Check all priority levels (2^7 - 2^0)
               for (ucPrioCurr= (1 << 7); ucPrioCurr; ucPrioCurr= ucPrioCurr >> 1)
               {
                  // Check for used priority
                  if (ucPrioCurr & ucPrioMask)
                  {
                     // Safely access ucFlags and update local copy
                     LOCKED(ucFlags= SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].ucFlags);
                        
                     if (ucFlags & SPI_CHANNEL_FLAGS_DATAQUEUEDTX)
                     {
                        // Safely access ucPrio, usAgeMS, usMaxAgeMS and update local copy
                        LOCK;
                        {
                           ucPrio= SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].ucPrio;
                           u16AgeTx= SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].u16AgeTx;
                           u16MaxAgeTx= SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].u16MaxAge;
                        }
                        UNLOCK;
                        
                        // Check for channel priority match and obtained max age of its data
                        if ((ucPrioCurr & ucPrio) || (u16AgeTx >= u16MaxAgeTx))
                        {
                           // Unit buffer tx (SSpiRW[pSSpiRW->ucUnit].SemTx) already locked on top of this task
                           {
                              // Lock unit rx
                              if (OS_SEM_TAKE_CHK(os_sem_take(SSpiRW[pSSpiRW->ucUnit].SemRx, OS_T_FOREVER)))
                              {
                                 // Lock channel buffer tx
                                 if (OS_SEM_TAKE_CHK(os_sem_take(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].SemTx, OS_T_FOREVER)))
                                 {
                                    // Lock channel buffer rx
                                    if (OS_SEM_TAKE_CHK(os_sem_take(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].SemRx, OS_T_FOREVER)))
                                    {
                                       // Check for already activated required channel configuration
                                       ucCSRIdx= SPI_CHANNEL2CSRIDX(SSpiRO[pSSpiRW->ucUnit].pBase, ucChannelCurr);
                                       
                                       // Safely access u32CSR and update local copy        
                                       LOCKED(u32CSR= SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].u32CSR);
                                       
                                       // Reconfigure hardware CSR in case software CSR is different for this channel when pdc is idle
                                       if (u32CSR != *(SSpiRO[pSSpiRW->ucUnit].pBase->SPI_CSR+ ucCSRIdx))
                                       {
                                          // Only when no other yet queued channel already allocated the true hardware CSR32
                                          if (!bCSR32[pSSpiRW->ucUnit][ucCSRIdx])
                                          {
                                             // Only for inactive transfer
                                             if (!bRunTx[pSSpiRW->ucUnit])
                                             {
                                                bCSR32[pSSpiRW->ucUnit][ucCSRIdx]= TRUE;
                                                *(SSpiRO[pSSpiRW->ucUnit].pBase->SPI_CSR+ ucCSRIdx)= u32CSR;
                                             }
                                          }
                                       }
                                                                       
                                       if (u32CSR == *(SSpiRO[pSSpiRW->ucUnit].pBase->SPI_CSR+ ucCSRIdx))
                                       {                         
                                          // Retrieve length of queued data job
                                          usBytesPerXferJob= 0;
                                          usResult= circular_read(usSpiChannelCntTx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx], (USHORT *)&usBytesPerXferJob, 1);
                                          if ((usResult > 0) && (usBytesPerXferJob > 0))
                                          {
                                             // Determine (1 or 2) bytes per pdc item for the current channel
                                             usBytesPerPDCItem= SPI_PDCITEMBYTES(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].u32CSR);
                                             
                                             // Determine pdc items per transfer
                                             usPDCItemsPerXferJob= (usBytesPerPDCItem == 1)  ? (usBytesPerXferJob) \
                                                                                             : ( /* usBytesPerPDCItem == 2 */ \
                                                                                                   (usBytesPerXferJob & 0x0001) ? ((usBytesPerXferJob/ usBytesPerPDCItem)+ 1) \
                                                                                                                                 : (usBytesPerXferJob/ usBytesPerPDCItem) \
                                                                                               );
                                             
                                             // Check for free space in all required circular buffers
                                             if (circular_freeWrite(usSpiChannelCntRxQ[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx]) >= 1)
                                             {
                                                if (circular_freeWrite(ucSpiChannelTagRx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx]) >= 1)
                                                {
                                                   if (circular_freeWrite(ulSpiCntTx[pSSpiRW->ucUnit]) >= 1)
                                                   {
#if SPI_HWFIX_PDCRX > 0
                                                      // Required for master mode only
                                                      if ((SSpiRO[pSSpiRW->ucUnit].pBase->SPI_MR & AT91C_SPI_MSTR) ? (circular_freeWrite(ucSpiAddrRx[pSSpiRW->ucUnit]) >= usPDCItemsPerXferJob) : (TRUE))
                                                      {
#endif
                                                         // Putting partial data multiple times is not supported because it's prevented by LASTXFER-BUG!!!
                                                         if (circular_freeWrite(ulSpiTx[pSSpiRW->ucUnit]) >= usPDCItemsPerXferJob)
                                                         {                                                         
                                                            // Set length for next rx cycle (starts simultaneously together with this tx as soon as started) after transfer to unit buffer (usSpiChannelCntRxQ)
                                                            {
                                                               // Retrieve user tag data tx
                                                               usResult= circular_read(ucSpiChannelTagTx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx], (UCHAR *)&ucTagJob, 1);
                                                               if (usResult > 0)
                                                               {
                                                                  // Set user tag data rx
                                                                  usResult= circular_write(ucSpiChannelTagRx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx], (UCHAR *)&ucTagJob, 1);
                                                                  if (usResult > 0)
                                                                  {
                                                                     // Set length for tx complete cycle that triggers channel data sent msg by irq tx after transfer to unit buffer (ulSpiCntTx)
                                                                     {
                                                                        // Safely access usBytesPerXfer and update local copy
                                                                        LOCKED(usBytesPerXfer= SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].usBytesPerXfer);                           
                                          
                                                                        // Transfer channel buffer to unit buffer (and add channel number for pdc)
                                                                        usByteCount= 0;
                                                                        do
                                                                        {
                                                                           // Clear all item bits (data maybe only partially overwritten later)
                                                                           ulPDCItem= 0;
                                                                           
                                                                           // Read plain data item (1 or 2 bytes) from channel buffer into item
                                                                           usResult= circular_read(ucSpiChannelTx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx], (UCHAR *)&ulPDCItem, _ulmin(ABS(usBytesPerXferJob, usByteCount), usBytesPerPDCItem));
                                                                           if (usResult > 0)
                                                                           {                                         
                                                                              usByteCount+= usResult;
                                                                              
                                                                              // Set pdc flag LASTXFER for complete transfer job or maximum transfer setting to close cycle for channels using CSAAT option
                                                                              if ((usByteCount >= usBytesPerXferJob) || (!(usByteCount % usBytesPerXfer)))
                                                                              {
                                                                                 // Caution!!!
                                                                                 // LASTXFER-BUG closes ALL cycles at pdc's end also when just set for an individual channel!!!
                                                                                 // This prevents fragmented transfers!!!
                                                                                 ulPDCItem |= AT91C_SPI_LASTXFER;
                                                                              }
                                                                              
                                                                              // Determine pcs bit pattern (mode dependend!)
                                                                              ucPCS= SPI_CHANNEL2PCS(SSpiRO[pSSpiRW->ucUnit].pBase, ucChannelCurr);
                                                                              ulPDCItem |= ucPCS << 16;
                                                                                                          
                                                                              // Write one pdc item to unit buffer
                                                                              usResult= circular_write(ulSpiTx[pSSpiRW->ucUnit], (ULONG *)&ulPDCItem, 1);
#if SPI_HWFIX_PDCRX > 0
                                                                              // Required for master mode only)
                                                                              if (SSpiRO[pSSpiRW->ucUnit].pBase->SPI_MR & AT91C_SPI_MSTR)
                                                                              {
                                                                                 if (usResult > 0)
                                                                                 {
                                                                                    // Write rx channel address (always true binary)
                                                                                    usResult= circular_write(ucSpiAddrRx[pSSpiRW->ucUnit], &ucChannelCurr, 1);
                                                                                 }
                                                                              }
#endif
                                                                              usDataQueuedTx= circular_countEntries(ucSpiChannelTx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx]);
                                                                           }
                                                                        }                           
                                                                        while ((usByteCount < usBytesPerXferJob) && usDataQueuedTx);
                                                                        
                                                                        // Compose 32bit value containing channel number (8), bytes per pdc item (8) and job byte count (16)
                                                                        ulCntTx= 0;
                                                                        ulCntTx |= usByteCount;
                                                                        ulCntTx |= (usBytesPerPDCItem & BITS_USHORTL) << BITS_OF(USHORT);
                                                                        ulCntTx |= ucChannelCurr << (BITS_OF(USHORT)+ BITS_OF(UCHAR));
                                                                     
                                                                        // Set length for tx complete cycle that triggers channel data sent msg by irq tx
                                                                        usResult= circular_write(ulSpiCntTx[pSSpiRW->ucUnit], (ULONG *)&ulCntTx, 1);
                                                                        if (usResult > 0)
                                                                        {
                                                                           // Set length for next rx cycle (starts simultaneously together with this tx as soon as started)
                                                                           usResult= circular_write(usSpiChannelCntRxQ[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx], (USHORT *)&usByteCount, 1);                                       
                                                                           if (usResult < 1)
                                                                           {
                                                                              // Debug
                                                                              usByteCount= usByteCount;
                                                                              usBytesPerXferJob= usBytesPerXferJob;
                                                                           }
                                                                        }
                                                                        
                                                                        // Safely clear the age of the processed channel
                                                                        LOCKED(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].u16AgeTx= 0);
                                                                        
                                                                        // Set queue flag
                                                                        bQueueTx[pSSpiRW->ucUnit]= TRUE;
                                                                        
                                                                        if (!usDataQueuedTx)
                                                                        {
                                                                           // Safely clear data queued tx flag for empty channel buffer
                                                                           LOCKED(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].ucFlags &= ~SPI_CHANNEL_FLAGS_DATAQUEUEDTX);
                                                                        }
                                                                        else
                                                                        {
                                                                           // Debug
                                                                           usDataQueuedTx= usDataQueuedTx;
                                                                        }
                                                                     }
                                                                  }
                                                               }
                                                            }
                                                         }
                                                         else
                                                         {
                                                            // Not enough space in ulSpiTx queue this time
                                                            
                                                            // Put the read count data back for next cycle
                                                            circular_unRead(usSpiChannelCntTx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx], 1);
                                                         }
#if SPI_HWFIX_PDCRX > 0
                                                      }
                                                      else
                                                      {
                                                         // Required for master mode only)
                                                         if (SSpiRO[pSSpiRW->ucUnit].pBase->SPI_MR & AT91C_SPI_MSTR)
                                                         {
                                                            // Not enough space in ucSpiAddrRx queue this time
                                                            
                                                            // Put the read count data back for next cycle
                                                            circular_unRead(usSpiChannelCntTx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx], 1);
                                                         }
                                                      }
#endif
                                                   }
                                                   else
                                                   {
                                                      // Not enough space in ulSpiCntTx queue this time
                                                      
                                                      // Put the read count data back for next cycle
                                                      circular_unRead(usSpiChannelCntTx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx], 1);
                                                   }
                                                }
                                                else
                                                {
                                                   // Not enough space in ucSpiChannelTagRx queue this time
                                                   
                                                   // Put the read count data back for next cycle
                                                   circular_unRead(usSpiChannelCntTx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx], 1);
                                                }
                                             }
                                             else
                                             {
                                                // Not enough space in usSpiChannelCntRxQ queue this time
                                                
                                                // Put the read count data back for next cycle
                                                circular_unRead(usSpiChannelCntTx[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx], 1);
                                             }
                                          }
                                       }
                                       
                                       // Unlock channel buffer rx
                                       os_sem_give(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].SemRx);
                                    }
   
                                    // Unlock channel buffer tx
                                    os_sem_give(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].SemTx);
                                 }

                                 // Unlock unit rx
                                 os_sem_give(SSpiRW[pSSpiRW->ucUnit].SemRx);
                              }
                           }
                        }
                        else
                        {
                           // Safely increase AgeMS of waiting queued data up to its maximum value
                           ATOMIC_RAISEMAX(SSpiChannelRW[pSSpiRW->ucUnit][SSpiChannelMapRW[pSSpiRW->ucUnit][ucChannelCurr].ucStorageIdx].u16AgeTx, SSpiRO[pSSpiRW->ucUnit].u16AgeStepTx, USHRT_MAX);
                        }                  
                     }
                  }
               }
            }

            // Unlock unit buffer tx
            os_sem_give(SSpiRW[pSSpiRW->ucUnit].SemTx);
         }
      }

      // Process queued data without scheduling other task(s) yet
      if (!bQueueTx[pSSpiRW->ucUnit])
      {
         // Suspend task until new job trigger by SpiTx... or post job trigger by SpiIrqHandler
         os_sem_take(SSpiRW[pSSpiRW->ucUnit].SemTickTx, OS_T_FOREVER);
      }
      
      // Limit cycle time
      if (SSpiRO[pSSpiRW->ucUnit].u16DelayTx) RESCHEDULE_TICKS(SSpiRO[pSSpiRW->ucUnit].u16DelayTx);
   }

   DETACH;
}

//----------------------------------------------------------------------------
// void SpiIrqHandler
//----------------------------------------------------------------------------
void SpiIrqHandler(UCHAR ucUnit)
{
   static USHORT usRxCancel[SPI_UNIT_MAX]= { 0 };
   static USHORT usTxCancel[SPI_UNIT_MAX]= { 0 };
   
   static USHORT usRxPrev[SPI_UNIT_MAX]= { 0 };
   static USHORT usTxPrev[SPI_UNIT_MAX]= { 0 };
   
   static ULONG ulSpiIrqPrev[SPI_UNIT_MAX]= { 0 };
   
#if SPI_HWFIX_PDCTX == TRUE
   static USHORT usTCROffset[SPI_UNIT_MAX]= { 0 };
#endif

   register ULONG ulSpiIrq;
   register ULONG ulSpiMsg;
   register ULONG ulSpiMsgJob;
   
   ULONG ulJobDetail;

   register USHORT usResult;
   register USHORT usBytesPerJob;
   register USHORT usBytesPerPDC;
   register USHORT usItemsPerJob;
   register USHORT usItemsPerPDC;
   register USHORT usItemCnt;
   
   register UCHAR ucChannel;

   // Copy and clear the IRQ source to handle yet
   ulSpiIrq= SSpiRO[ucUnit].pBase->SPI_IMR & SSpiRO[ucUnit].pBase->SPI_SR;
   
   // Clear all handled data IRQ sources (they may be set later when there is no more data action when they occur)
   ulSpiMsg= ((ulSpiIrq & ~(SSpiRO[ucUnit].u32IrqRx | SSpiRO[ucUnit].u32IrqTx)));
   
#ifdef SPI_DEBUG
   if (!ucUnit)
   {
      ucUnit= ucUnit;
   }
   else
   {
      ucUnit= ucUnit;
   }
#endif
   
   // Rx double bank pdc counters are zero after startup (IRQ occurs also for double bank overrun!)
   if (ulSpiIrq & AT91C_SPI_RXBUFF)
   {
      for (ucChannel= SSpiRW[ucUnit].SCircular.ucCntEntries, usResult= 0; (ucChannel < SSpiRW[ucUnit].SCircular.ucSize) && (usResult < 2); ucChannel++, usResult++)
      {
         // Queue pdx rx bank
         SSpiRO[ucUnit].pBase->SPI_RNPR= (UINT) &SSpiRW[ucUnit].ulPDCBufferRx[SSpiRW[ucUnit].SCircular.ucIdxWrite];
         SSpiRO[ucUnit].pBase->SPI_RNCR= (USHORT) (SPI_PDCMAX);
         
         // Increment write index
         SSpiRW[ucUnit].SCircular.ucIdxWrite= (SSpiRW[ucUnit].SCircular.ucIdxWrite+ 1) % SSpiRW[ucUnit].SCircular.ucSize;  
      }

      // usCntEntries is incremented in advance
      if (SSpiRW[ucUnit].SCircular.ucCntEntries < SSpiRW[ucUnit].SCircular.ucSize)
      {
         // Raise usCntEntries upto usSize+ 1
         RAISEMAX(SSpiRW[ucUnit].SCircular.ucCntEntries, 1, (SSpiRW[ucUnit].SCircular.ucSize+ 1));
      }
   }      
   // Rx single bank pdc counter is zero (just queue a new bank for next bank switch)
   else if (ulSpiIrq & AT91C_SPI_ENDRX)
   {
      // Check for a free buffer to set the rx pdc to write to
      if (SSpiRW[ucUnit].SCircular.ucCntEntries <= SSpiRW[ucUnit].SCircular.ucSize)
      {        
         // Prepare pdc rx
         SSpiRO[ucUnit].pBase->SPI_RNPR= (UINT) &SSpiRW[ucUnit].ulPDCBufferRx[SSpiRW[ucUnit].SCircular.ucIdxWrite];
         SSpiRO[ucUnit].pBase->SPI_RNCR= (USHORT) (SPI_PDCMAX);

         // Check for rx data
         if (SSpiRW[ucUnit].SCircular.ucCntEntries > 0)
         {
            // Determine item count            
            if (!(SSpiRO[ucUnit].pBase->SPI_MR & AT91C_SPI_MSTR))
            {
               usItemCnt= (usRxPrev[ucUnit]- usRxCancel[ucUnit]);
            }
            else
            {
               usItemCnt= usRxPrev[ucUnit];
            }
            
            // Send rx msg to task including fixed count of received items and current rx buffer index to read from
            if (isr_mbx_check(&MbxSpiRx[ucUnit]) > 0)
            {
               // Trigger rx task to process received data with given count
               isr_mbx_send(&MbxSpiRx[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(SPI_UNIT_DATA_MSGBITS, (((ULONG)SSpiRW[ucUnit].SCircular.ucIdxRead) << 16) | (ULONG)usItemCnt, SSpiRW[ucUnit].SCircular.ucCntEntries)));
               
               // Increment read index in advance
               SSpiRW[ucUnit].SCircular.ucIdxRead= (SSpiRW[ucUnit].SCircular.ucIdxRead+ 1) % SSpiRW[ucUnit].SCircular.ucSize;
               
               // Decrement count in advance
               if (SSpiRW[ucUnit].SCircular.ucCntEntries) SSpiRW[ucUnit].SCircular.ucCntEntries--;
            }
            else
            {
               // Debug
               usItemCnt= usItemCnt;
            }
         }
         
         // usCntEntries and usIdxWrite are incremented in advance
         
         if (SSpiRW[ucUnit].SCircular.ucCntEntries < SSpiRW[ucUnit].SCircular.ucSize)
         {
            // Raise usCntEntries upto usSize+ 1
            RAISEMAX(SSpiRW[ucUnit].SCircular.ucCntEntries, 1, (SSpiRW[ucUnit].SCircular.ucSize+ 1));
               
            // Increment write index
            SSpiRW[ucUnit].SCircular.ucIdxWrite= (SSpiRW[ucUnit].SCircular.ucIdxWrite+ 1) % SSpiRW[ucUnit].SCircular.ucSize;         
         }
         else
         {
            // Disable the rx irq yet - completed task will re-enable it again
            AT91F_SPI_DisableIt(SSpiRO[ucUnit].pBase, SSpiRO[ucUnit].u32IrqRx);

            // Debug
            usItemCnt= usItemCnt;
         }
      }
   }
   
   // Tx using single bank pdc
   if (ulSpiIrq & AT91C_SPI_TXBUFE)
   {
      usItemsPerPDC= 0;
      
      // Queue tx data
      if (SSpiRW[ucUnit].ucIrqTxState == SPI_TXSTATE_START_TX)
      {
         // Mark next tx irq as clearing irq (required for master mode only)
         if (SSpiRO[ucUnit].pBase->SPI_MR & AT91C_SPI_MSTR)
         {
            SSpiRW[ucUnit].ucIrqTxState= SPI_TXSTATE_STOP_RX;
         }

         // Check for new items to send
         do
         {
            usResult= circular_read(ulSpiCntTx[ucUnit], &ulJobDetail, 1);
            if (usResult)
            {
               // 32bit ulJobDetail value containing channel number (8), bytes per pdc item (8) and job byte count (16)
               usBytesPerJob= ulJobDetail & BITS_USHORT;
               if (usBytesPerJob)
               {
                  usBytesPerPDC= _umin(((ulJobDetail & BITS_ULONGHL) >> BITS_OF(USHORT)), 2);

                  // Determine pdc items per transfer
                  usItemsPerJob= ((usBytesPerPDC == 0) || (usBytesPerPDC == 1)) ? (usBytesPerJob) \
                                                      : ( /* usBytesPerPDC == 2 */ \
                                                            (usBytesPerJob & 0x0001) ? ((usBytesPerJob/ usBytesPerPDC)+ 1) \
                                                                                     : (usBytesPerJob/ usBytesPerPDC) \
                                                        );
                                                                                                  
                  if (((USHORT)SPI_PDCMAX- usItemsPerPDC) >= usItemsPerJob)
                  {
                     usItemsPerPDC+= usItemsPerJob;
                  }
                  else
                  {
                     circular_unRead(ulSpiCntTx[ucUnit], 1);
                     usResult= 0;
                  }
               }
            }
   
         } while (usResult);
         
         // Check for bytes to send yet
         if (usItemsPerPDC)
         {
            // Fill up pdc tx buffer
            usResult= circular_read(ulSpiTx[ucUnit], SSpiRW[ucUnit].ulPDCBufferTx, usItemsPerPDC);
            if (usResult)
            {
               usTxPrev[ucUnit]= usResult;
               usRxPrev[ucUnit]= usResult;

               // Prepare tx pdc
               SSpiRO[ucUnit].pBase->SPI_TNPR= (UINT) SSpiRW[ucUnit].ulPDCBufferTx;
               SSpiRO[ucUnit].pBase->SPI_TNCR= (USHORT) usTxPrev[ucUnit];
            }
            
            // Disable Tx and unlocking buffer done after next state change back to SPI_TXSTATE_START_TX (for master mode only)

            // Disable Tx and unlocking buffer immediately for slave mode only
            if (!(SSpiRO[ucUnit].pBase->SPI_MR & AT91C_SPI_MSTR))
            {            
               AT91F_SPI_DisableIt(SSpiRO[ucUnit].pBase, SSpiRO[ucUnit].u32IrqTx);
               
               // Trigger TskSpiTx for post processing (re-enables irq processing)
               isr_sem_give(SSpiRW[ucUnit].SemTx);
               
               // Trigger TskSpiTx to process queued data without scheduling other task(s) yet
               if (!ISR_SEM_TAKE_CHK(isr_sem_check(SSpiRW[ucUnit].SemTickTx)))
               {
                  isr_sem_give(SSpiRW[ucUnit].SemTickTx);
               }
            }
         }
      }
      // Stop pending rx (force tx/rx/tx/rx ... sequence)
      else if (SSpiRW[ucUnit].ucIrqTxState == SPI_TXSTATE_STOP_RX)
      {     
         // Stop - still yet pending only - rx pdc to trigger rx irq
         if ((!(ulSpiIrqPrev[ucUnit] & SSpiRO[ucUnit].u32IrqRx)) && (!(ulSpiIrq & SSpiRO[ucUnit].u32IrqRx)))
         {
            // Pend tx irq until next rx irq triggered

            if (SSpiRO[ucUnit].pBase->SPI_RCR)
            {
               // Stop pdc
               SSpiRO[ucUnit].pBase->SPI_PTCR= AT91C_PDC_RXTDIS;

               // Determine unprocessed rx bytes
               usRxCancel[ucUnit]= SSpiRO[ucUnit].pBase->SPI_RCR;

               // Abort current transfer
               SSpiRO[ucUnit].pBase->SPI_RCR= 0;

               // Continue pdc
               SSpiRO[ucUnit].pBase->SPI_PTCR= AT91C_PDC_RXTEN;
            }
         }
         // Reenable tx irq processing for next cycle and back to task control
         else
         {
            // Tx and rx complete - reenable tx irq processing
            SSpiRW[ucUnit].ucIrqTxState= SPI_TXSTATE_START_TX;
                        
            AT91F_SPI_DisableIt(SSpiRO[ucUnit].pBase, SSpiRO[ucUnit].u32IrqTx);
            
            // Trigger TskSpiTx for post processing (re-enables irq processing)
            isr_sem_give(SSpiRW[ucUnit].SemTx);
            
            // Trigger TskSpiTx to process queued data without scheduling other task(s) yet
            if (!ISR_SEM_TAKE_CHK(isr_sem_check(SSpiRW[ucUnit].SemTickTx)))
            {
               isr_sem_give(SSpiRW[ucUnit].SemTickTx);
            }            
         }
      }
   }
   
   // NSS raising
   if (ulSpiIrq & AT91C_SPI_NSSR)
   {
      // Check mode of spi - only slave mode supports nss irq
      if (!(SSpiRO[ucUnit].pBase->SPI_MR & AT91C_SPI_MSTR))
      {
         // Check tx pdc for still unfinished transfer
         if (SSpiRO[ucUnit].pBase->SPI_TCR)
         {                             
            // Stop pdc
            SSpiRO[ucUnit].pBase->SPI_PTCR= AT91C_PDC_TXTDIS;

            // Determine unprocessed tx bytes
#if SPI_HWFIX_PDCTX == TRUE
            usTxCancel[ucUnit]= SSpiRO[ucUnit].pBase->SPI_TCR+ usTCROffset[ucUnit];
#else
            usTxCancel[ucUnit]= SSpiRO[ucUnit].pBase->SPI_TCR;
#endif

#ifdef SPI_DEBUG
            usTxCancel[ucUnit]= usTxCancel[ucUnit];
#endif
            
            // Abort current transfer
            SSpiRO[ucUnit].pBase->SPI_TCR= 0;

            // Continue pdc
            SSpiRO[ucUnit].pBase->SPI_PTCR= AT91C_PDC_TXTEN;
         }
         else
         {
#if SPI_HWFIX_PDCTX == TRUE
            usTxCancel[ucUnit]= usTCROffset[ucUnit];
#else
            usTxCancel[ucUnit]= 0;
#endif

#ifdef SPI_DEBUG
            usTxCancel[ucUnit]= usTxCancel[ucUnit];
#endif

         }

         // Check pdc rx for still unfinished transfer  
         if (SSpiRO[ucUnit].pBase->SPI_RCR)
         {                             
            // Stop pdc
            SSpiRO[ucUnit].pBase->SPI_PTCR= AT91C_PDC_RXTDIS;
            
            // Determine unprocessed tx bytes
            usRxCancel[ucUnit]= SSpiRO[ucUnit].pBase->SPI_RCR;
            
            // Abort current transfer
            SSpiRO[ucUnit].pBase->SPI_RCR= 0;
            
            // Continue pdc
            SSpiRO[ucUnit].pBase->SPI_PTCR= AT91C_PDC_RXTEN;

#if SPI_HWFIX_PDCTX == TRUE           
            // Clear pdc and tdr queues to avoid transmission of the already by pdc hw-bug queued byte (undocumented feature!)
            AT91F_SPI_Disable(SSpiRO[ucUnit].pBase);
            AT91F_SPI_Enable(SSpiRO[ucUnit].pBase);
#endif
         }
         else
         {
            usRxCancel[ucUnit]= 0;
         }
      }

      // Send AT91C_SPI_NSSR msg also to channel zero
      if (isr_mbx_check(&MbxSpiMsgChannel[ucUnit]) > 0)
      {
         ucChannel= 0;
         ulSpiMsgJob= AT91C_SPI_NSSR;
         usBytesPerJob= 1;
         isr_mbx_send(&MbxSpiMsgChannel[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(SPI_CHANNEL_MSG_MSGBITS, ulSpiMsgJob, ((ULONG)ucChannel << BITS_OF(USHORT)) | usBytesPerJob)));
      }
   }
   
   // Store last irq
   ulSpiIrqPrev[ucUnit]= ulSpiIrq;
   
   // Record all other yet unhandled IRQ sources (includes communication errors)
   
   // Apply message filter
   ulSpiMsg&= SSpiRW[ucUnit].ulMsgMask;
   
   // Apply max counter index available
   ulSpiMsg&= ((1 << (SPI_UNIT_MSGMAX+ 1))- 1);
   
   if (ulSpiMsg)
   {
      if (isr_mbx_check(&MbxSpiMsg[ucUnit]) > 0)
      {
         isr_mbx_send(&MbxSpiMsg[ucUnit], OS_MBX_VALUE_PUT(MSG_ADDCNT(SPI_UNIT_MSG_MSGBITS, ulSpiMsg, 1)));  
      }
   }
   
   // ARM needs EXPLICIT acknowledge (qualifier __irq doesn't do this) for (E)nd (O)f (I)RQ!!
   EOI;   
}
