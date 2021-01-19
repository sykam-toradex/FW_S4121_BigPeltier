//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/MISC/ENCGAPDISK/ENCGAPDISK.C
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-03-03 initial version
// 2010-03-04 added automatic shortest path selection
// 2010-03-04 added non-continous reverse processing feature
// 2010-03-10 added ucGapCnt to _SEncGapDiskRO
// 2010-03-10 added bMean option to EncGapDiskGapCnt
// 2010-03-10 added EncGapDiskErrGet
// 2010-03-10 added ucRetry to _SEncGapDiskRO
// 2010-03-12 added creep move support to EncGapDiskGapGoto
// 2010-03-12 optimized teamwork of EncGapDiskTeachIn & TskEncGapDiskMsg
// 2010-03-17 added vibration resistant edge filter to IsrEncGapDiskIrqHandler
// 2010-03-18 added bLevel reverse to place "fall" and "raise" clks into right
// 2010-03-24 changes for renamed vars of encgapdiskdata.h
// 2010-03-24 changed edge storage from SEncGap to SEncGapFall/Raise
// 2010-03-24 rewrote TskEncGapDiskMsg to handle edges instead of atomic gaps
// 2010-03-25 fixed edge filter inside IsrEncGapDiskIrqHandler (bReverse)
// 2010-03-25 introduced iteration detection and checking to TskEncGapDiskMsg
// 2010-03-26 fixed and enhanced TskEncGapDiskMsg to support sequence adjust
// 2010-03-30 changed remaining gap names to edge names where edges meant
// 2010-03-31 added function EncGapDiskClear
// 2010-03-31 fixed broken retry feature of EncGapDiskTeachIn
// 2010-03-31 changed storage from fall/rise to two dim array of edges
// 2010-03-31 renamed EncGapDiskGapGet to EncGapDiskEdgeGet
// 2010-03-31 renamed/changed EncGapDiskGapGoto to EncGapDiskEdgeGoto
// 2010-03-31 renamed/changed EncGapDiskGapCnt to EncGapDiskEdgeCnt
// 2010-03-31 renamed/changed EncGapDiskGapGet to EncGapDiskEdgeGet
// 2010-03-31 renamed/changed EncGapDiskErrGet to EncGapDiskEdgeErrGet
// 2010-03-31 added new function EncGapDiskCycleGet
// 2010-04-09 fixed count of gaps returned by EncGapDiskEdgeCnt
// 2010-04-09 fixed count of gaps source of EncGapDiskEdgeGet
// 2010-04-09 fixed count of gaps source of EncGapDiskEdgeGoto
// 2010-05-06 fixed determination for StpClkDirMove move mode selection
// 2010-05-07 fixed limit check in EncGapDiskEdgeGet and EncGapDiskEdgeGoto
// 2010-05-07 fixed calculation of ulClks in EncGapDiskTeachIn for continous
// 2010-05-07 fixed confirmed edge count calculation of EncGapDiskEdgeCnt
// 2010-05-07 fixed bCompare by changing it to array type in TskEncGapDiskMsg
// 2010-05-07 changed bCompare of _SEncGapDiskRW to array typeEncGapDiskInit
// 2010-05-07 changed bCompare of _SEncGapDiskRW to array typeEncGapDiskClear
// 2010-05-12 moved bContinous from ...RO to ...RW; init by EncGapDiskTeachIn
// 2010-05-12 moved ulClkLowHz, ulClkHighHz & ulClks from ...RO to ...RW
// 2010-05-12 moved usClksAcc & usClksBrk from ...RO to ...RW
// 2010-05-12 moved usToleranceSteps &  ucThreshold from ...RO to ...RW
// 2010-05-12 moved usEdgeDriftMax from ...RO to ...RW
// 2010-05-12 added bContinous to EncGapDiskTeachIn
// 2010-05-12 added ulClkLowHz, ulClkHighHz to EncGapDiskTeachIn
// 2010-05-12 added usClksAcc & usClksBrk to EncGapDiskTeachIn
// 2010-05-12 added usToleranceSteps &  ucThreshold to EncGapDiskTeachIn
// 2010-05-12 added usEdgeDriftMax to EncGapDiskTeachIn
// 2010-05-12 changed ucGapCnt to usGapCnt in EncGapDiskTeachIn (UCHAR>USHORT)
// 2010-05-13 init of all from ...RO to ...RW params in EncGapDiskTeachIn
// 2010-05-13 renamed ulClks to ulCycleClks in EncGapDiskTeachIn
// 2010-05-13 added ucEdgeType to EncGapDiskEdgeErrGet for edge independency
// 2010-05-13 added pusEdgeCntTotal to EncGapDiskEdgeCnt
// 2010-05-13 EncGapDiskEdgeGet now supports SeqMin
// 2010-05-13 EncGapDiskEdgeGoto now supports SeqMin
// 2010-05-13 added auto alternate cycle detection feature to TskEncGapDiskMsg
// 2010-05-13 fixed calculation of ucEdgeSeqMax in TskEncGapDiskMsg
// 2010-05-13 cleaned up pointer re-set code in TskEncGapDiskMsg
// 2010-05-13 fixed calculation of usToGo in EncGapDiskTeachIn
// 2010-05-13 auto retry now also supports continous mode in EncGapDiskTeachIn
// 2010-05-13 fixed check of ucToleranceErrorCnt[] in EncGapDiskTeachIn
// 2010-05-19 fixed calculation of SeqMin in TskEncGapDiskMsg
// 2010-05-19 added EncGapDiskEdgeHomeGet
// 2010-05-20 changed EdgeDriftMax to ToleranceSteps in EncGapDiskEdgeHomeGet
// 2010-05-20 added user index zero set feature to EncGapDiskInit
// 2010-05-20 added EncGapDiskEdgeGotoZeroSet (user index zero set feature)
// 2010-05-21 added user index zero set feature to EncGapDiskEdgeGoto
// 2010-07-02 added ucEdgeCurr and bEdgeCurrValid
// 2010-07-02 renamed EncGapDiskEdgeGet to EncGapDiskEdgeDataGet
// 2010-07-02 added EncGapDiskEdgeGet
// 2010-07-02 added ZeroOffset param to EncGapDiskEdgeZeroSet
// 2010-07-02 removed user index zero set feature to EncGapDiskEdgeDataGet
// 2010-07-02 removed user index zero set feature to EncGapDiskEdgeGoto
// 2010-07-02 removed EncGapDiskEdgeZeroSet
// 2010-07-02 removed ucEdgeSeqMod and ucEdgeSeqOffset
// 2010-07-03 fixed EncGapDiskEdgeZeroSet
// 2010-07-03 re-added user index zero set feature to EncGapDiskEdgeDataGet
// 2010-07-03 re-added user index zero set feature to EncGapDiskEdgeGoto
// 2010-07-03 now using StpClkDirModuloClksGet in EncGapDiskHomeEdgeGet
// 2010-07-03 reduced stack load of EncGapDiskHomeEdgeGet by UCHAR index array
// 2010-07-07 changed EncGapDiskEdgeDataGet for edge type independency
// 2010-07-07 changed EncGapDiskEdgeZeroSet for edge type independency
// 2010-07-07 changed EncGapDiskEdgeGoto for edge type independency
// 2010-07-07 renamed ucGapErrorCnt to ucGapErrorCnt
// 2010-07-07 renamed ucErrorGaps to ucGapError
// 2010-07-07 merged EncGapDiskGapErrGet and EncGapDiskEdgeErrGet
// 2010-07-07 removed EncGapDiskGapErrGet
// 2010-07-07 changed ucEdgeZero to ucEdgeZero[]
// 2010-07-07 removed ulIdent in EncGapDiskEdgeDataGet
// 2010-07-07 moved ucEdgeCurr & bEdgeCurrValid to _SEdgeCurr (ucItem, bValid)
// 2010-07-07 renamed ucGapNumber to ucGapCnt
// 2010-07-07 fixed adjustment check of acc and brk in EncGapDiskEdgeGoto
// 2010-07-07 fixed auto retry for continous mode in EncGapDiskTeachIn
// 2010-07-08 fixed adjustment calc of acc, brk & clkhi in EncGapDiskEdgeGoto
// 2010-07-08 renamed ulErrorSteps[] to ulErrorClks[]
// 2010-07-08 renamed usToleranceSteps to usToleranceClks
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-10 raised diff pass limit in EncGapDiskHomeEdgeGet by +ucThreshold
// 2010-09-15 changed EncGapDiskEdgeCnt to return count of sequential edges
// 2010-09-15 renamed core of EncGapDiskEdgeCnt to _EncGapDiskEdgeCnt
// 2010-09-16 changed _EncGapDiskEdgeCnt to skip before count sequential edges
// 2010-09-16 renamed struct _SEncEdge to _SEncGapDiskEncEdge
// 2010-09-16 added structs _SEncGapDiskStpClkDir, _S...Var & _S...Permanent
// 2010-09-16 reorganized data structures (added permanent tree to rw data)
// 2010-09-16 added functions EncGapDiskSnapShot, -SizeGet & -AddrGet
// 2010-09-16 added function EncGapDiskStpClkDirUnitGet
// 2010-09-17 fixed retry count to low by one in EncGapDiskTeachIn
// 2010-09-23 changed to _u(l)min & _u(l)max intrinsics
// 2010-09-29 rr: added new function EncGapDiskCalibrationDataSizeGet/AddrGet
// 2010-09-29 rr: added new function EncGapDiskCalibrationDataRW
// 2010-09-29 rr: added new function EncGapDiskRunParamsRW
// 2010-09-30 EncGapDiskHomeEdgeGet() changed to use _EncGapDiskEdgeCnt
// 2010-09-30 added piobase para to pio irq handler
// 2010-09-30 added re-read feature to IsrEncGapDiskIrqHandler
// 2010-10-01 rewrote repeat finder of EncGapDiskHomeEdgeGet
// 2010-10-07 fixed returned gap index of EncGapDiskHomeEdgeGet
// 2010-10-07 fixed end repeat checker/invalidator of EncGapDiskHomeEdgeGet
// 2010-10-07 init clks may be set to zero to skip init in EncGapDiskTeachIn
// 2010-10-08 added modulo based clks restart handler in TskEncGapDiskMsg
// 2010-10-20 fixed modulo based clks restart handler in TskEncGapDiskMsg
// 2010-10-20 removed previous level comparison on IsrEncGapDiskIrqHandler
// 2010-10-20 ENCGAPDISK_REREADPIN_COUNT option may be disabled now by value 0
// 2010-10-27 fixed modulo based clks restart for iteration in TskEncGapDiskMsg
// 2010-10-28 changed continous mode modulo checking in EncGapDiskEdgeGoto
// 2010-11-03 added par usMicroSteps to EncGapDiskTeachIn & EncGapDiskEdgeGoto
// 2010-11-15 rr: new re-calculation of acc/brk ramp in EncGapDiskEdgeGoto
// 2010-11-16 changed all ready check condition for post move offset correction
// 2010-11-16 fixed position re-offseting in EncGapDiskTeachIn for several runs
// 2010-11-17 added offset recording to TskEncGapDiskMsg for post move offset corr
// 2010-11-17 changed irq control of EncGapDiskTeachIn for post move offset corr
// 2010-11-17 added irq control in EncGapDiskEdgeGoto for post move offset corr
// 2010-11-18 move usEdgeDriftMax into SPermanent.SVar structure
// 2010-11-18 changed EncGapDiskCalibrationDataRW to transfer all edge types
// 2010-11-18 changed EncGapDiskSnapShot to transfer all edge types (at once)
// 2010-11-18 fixed EncGapDiskCalibrationDataRW to not lift up invalid edges
// 2010-11-18 fixed EncGapDiskSnapShot to not lift up invalid edges confirm cnt
// 2010-11-18 restore of EncGapDiskCalibrationDataRW enables post offset move corr
// 2010-11-18 restore of EncGapDiskSnapShot enables post offset move correction
// 2010-11-10 post move offset correction may be permanent disabled by define
// 2010-11-19 optional offset correction feature being direction dependent now
// 2010-11-19 replaced StpClkDirModuloClksGet by StpClkDirModuloMaxClksGet
// 2010-11-23 changed offset determinated in TskEncGapDiskMsg to best fit edge
// 2010-11-23 added mbx clear of MbxEncGapDiskMsg in EncGapDiskClear
// 2010-11-23 fixed TskEncGapDiskMsg detach by new best fit edge determination
// 2010-11-23 added clearing of ucEdgeZero[] & SEdgeCurr[] in EncGapDiskClear
// 2010-12-01 EncGapDiskEdgeGoto now sets success state when already in position
// 2010-12-08 fixed success inc for ucEdgeIgnoreCnt in EncGapDiskEdgeErrGet
// 2010-12-08 added bClear and changed other param for EncGapDiskEdgeErrGet
// 2010-12-08 TskEncGapDiskMsg handles ucEdgeIgnoreCnt in offset mode yet
// 2010-12-08 TskEncGapDiskMsg handles ulErrorClks & usEdgeCnt in offset mode yet
// 2010-12-08 renamed EncGapDiskCycleGet to EncGapDiskStatGet plus new param
// 2010-12-08 EncGapDiskIrqHandler changed to ignore non-definite edges yet
// 2010-12-17 replaced move profile code in EncGapDiskEdgeGoto by StpClkDirProfileAdjust
// 2011-09-07 added new param u16Timeout to StpClkDirClksPosOffsVal calls
// 2011-09-28 added ENCGAPDISK_OFFRANGEMODULOCHECK option to control this check
// 2011-09-28 added new function StpClkDirClksAbsShiftGet
// 2011-09-29 added optional define ENCGAPDISK_EDGEDRIFTMAXFACTOR (for offset mode)
// 2013-04-11 fixed function header's return data type comments
// 2013-08-22 changed timeout from milliseconds to ticks
// 2016-03-02 added macros and function InitEncGapDiskHandler
// 2016-03-02 added wrapper & check function EncGapDiskIrqCtrl (controls hw & sw irq)
// 2016-06-01 added support for extra signal gate option to IsrEncGapDiskIrqHandler
// 2016-06-01 added IsrEncGapDiskGate(Irq)Handler for gate interrupt support
// 2016-06-09 added support for different port group signal gate source to IsrEncGapDiskIrqHandler
// 2016-06-23 fixed offset counting in TskEncGapDiskMsg for offset mode
// 2016-06-23 added new function EncGapDiskOffsetGet
// 2016-06-23 IsrEncGapDiskIrqHandler requires only greater level count >= ENCGAPDISK_REREADPIN_COUNT
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <limits.h>

#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\..\cpu\at91\misc\stpclkdir\stpclkdir.h"
#include "..\..\..\cpu\at91\misc\stpclkdir\stpclkdirext.h"

#include "encgapdisk.h"
#include "encgapdiskdata.h"
#include "encgapdiskdataext.h"
#include "encgapdiskdev.h"
#include "encgapdiskext.h"
#include "encgapdiskint.h"

//----------------------------------------------------------------------------
// void IsrEncGapDisk_x __irq
//----------------------------------------------------------------------------
// Each supported unit requires a function definition here
#if ENCGAPDISK_UNIT_MAX > 0
   ENCGAPDISK_ISR_FUNC(0);
   ENCGAPDISK_ISRG_FUNC(0);
   ENCGAPDISK_INIT_FUNC(0);
   ENCGAPDISK_INITG_FUNC(0);
#endif

#if ENCGAPDISK_UNIT_MAX > 1
   ENCGAPDISK_ISR_FUNC(1);
   ENCGAPDISK_ISRG_FUNC(1);
   ENCGAPDISK_INIT_FUNC(1);
   ENCGAPDISK_INITG_FUNC(1);
#endif

#if ENCGAPDISK_UNIT_MAX > 2
   ENCGAPDISK_ISR_FUNC(2);
   ENCGAPDISK_ISRG_FUNC(2);
   ENCGAPDISK_INIT_FUNC(2);
   ENCGAPDISK_INITG_FUNC(2);
#endif

//----------------------------------------------------------------------------
// UCHAR EncGapDiskInit
//----------------------------------------------------------------------------
UCHAR EncGapDiskInit(UCHAR ucUnit)
{
   UCHAR ucResult= 0;
     
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      EncGapDiskIrqCtrl(ucUnit, FALSE);
      
      SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_FALL].bValid= FALSE;
      SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_RAISE].bValid= FALSE;
      SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_FALL].ucItem= 0;
      SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_RAISE].ucItem= 0;
      SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ENCGAPDISK_EDGE_FALL]= 0;
      SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ENCGAPDISK_EDGE_RAISE]= 0;
      SEncGapDiskRW[ucUnit].ucState= ENCGAPDISK_STATE_NONE;
      SEncGapDiskRW[ucUnit].ucJobId= 0;
      SEncGapDiskRW[ucUnit].ucJobIdPrev= SEncGapDiskRW[ucUnit].ucJobId;
      SEncGapDiskRW[ucUnit].bReverse= FALSE;
      SEncGapDiskRW[ucUnit].ucCompare[ENCGAPDISK_EDGE_FALL]= 0;
      SEncGapDiskRW[ucUnit].ucCompare[ENCGAPDISK_EDGE_RAISE]= 0;
      SEncGapDiskRW[ucUnit].ucCompareDelay[ENCGAPDISK_EDGE_FALL]= 0;
      SEncGapDiskRW[ucUnit].ucCompareDelay[ENCGAPDISK_EDGE_RAISE]= 0;      
      SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ENCGAPDISK_EDGE_FALL]= 0;
      SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ENCGAPDISK_EDGE_RAISE]= 0;

      // Prepare OS tools
      os_sem_init(SEncGapDiskRW[ucUnit].SemStart, 1);
      
      OS_MBX_INIT_EX(MbxEncGapDiskMsg[ucUnit]);
      
      // Tasks
      SEncGapDiskTsk[ucUnit].ucUnit= ucUnit;      
      SEncGapDiskRW[ucUnit].tidTskMsg= OS_TSK_CREATE_USER_EX(TskEncGapDiskMsg, SEncGapDiskRO[ucUnit].ucTskPriMsg, StkTskEncGapDiskMsg[ucUnit], &SEncGapDiskTsk[ucUnit]);
      
      ucResult= (SEncGapDiskRW[ucUnit].tidTskMsg != 0) ? (1) : (0);
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskClear
//----------------------------------------------------------------------------
UCHAR EncGapDiskClear(UCHAR ucUnit, bool bReverse, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   register ULONG ulIndex;
   ULONG ulValue;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      // Check ready condition
      if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
      {
         // Clear state
         SEncGapDiskRW[ucUnit].ucState= ENCGAPDISK_STATE_NONE;

         // Disable IRQ
         EncGapDiskIrqCtrl(ucUnit, FALSE);
         
         // Clear mailbox for isr to task communication
         while (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxEncGapDiskMsg[ucUnit], OS_MBX_VALUE_GET(ulValue), u16Timeout)));
         
         // Re-init task
         SEncGapDiskRW[ucUnit].ucJobIdPrev= SEncGapDiskRW[ucUnit].ucJobId;
         SEncGapDiskRW[ucUnit].ucCompare[ENCGAPDISK_EDGE_FALL]= 0;
         SEncGapDiskRW[ucUnit].ucCompare[ENCGAPDISK_EDGE_RAISE]= 0;
         SEncGapDiskRW[ucUnit].ucCompareDelay[ENCGAPDISK_EDGE_FALL]= 0;
         SEncGapDiskRW[ucUnit].ucCompareDelay[ENCGAPDISK_EDGE_RAISE]= 0;
         SEncGapDiskRW[ucUnit].ulCycleCnt= 0;
         SEncGapDiskRW[ucUnit].ulEdgeIterationCnt= 0;

         // Clear errors
         SEncGapDiskRW[ucUnit].ulErrorClks[ENCGAPDISK_EDGE_FALL]= 0;
         SEncGapDiskRW[ucUnit].ulErrorClks[ENCGAPDISK_EDGE_RAISE]= 0;
         SEncGapDiskRW[ucUnit].ucEdgeError[ENCGAPDISK_EDGE_FALL]= 0;
         SEncGapDiskRW[ucUnit].ucEdgeError[ENCGAPDISK_EDGE_RAISE]= 0;
         SEncGapDiskRW[ucUnit].ucToleranceErrorCnt[ENCGAPDISK_EDGE_FALL]= 0;
         SEncGapDiskRW[ucUnit].ucToleranceErrorCnt[ENCGAPDISK_EDGE_RAISE]= 0;
         SEncGapDiskRW[ucUnit].ucEdgeErrorCnt[ENCGAPDISK_EDGE_FALL]= 0;
         SEncGapDiskRW[ucUnit].ucEdgeErrorCnt[ENCGAPDISK_EDGE_RAISE]= 0;
         SEncGapDiskRW[ucUnit].ucEdgeIgnoreCnt[ENCGAPDISK_EDGE_FALL]= 0;
         SEncGapDiskRW[ucUnit].ucEdgeIgnoreCnt[ENCGAPDISK_EDGE_RAISE]= 0;

         // Clear compares (nice to have)
         for (ulIndex= 0; ulIndex < ENCGAPDISK_COMPAREITEM_MAX; ulIndex++)
         {
            SEncGapDiskRW[ucUnit].ulEdgeClksCmp[ENCGAPDISK_EDGE_FALL][ulIndex]= 0;
            SEncGapDiskRW[ucUnit].ulEdgeClksCmp[ENCGAPDISK_EDGE_RAISE][ulIndex]= 0;
         }
         
         // Re-init sequences
         if (!bReverse)
         {
            SEncGapDiskRW[ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_FALL]= 0;
         }
         else
         {
            SEncGapDiskRW[ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_FALL]= (SEncGapDiskRW[ucUnit].SPermanent.SVar.ucGapCnt- 1);
         }
                  
         SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ENCGAPDISK_EDGE_FALL]= 0;
         SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ENCGAPDISK_EDGE_RAISE]= 0;
         
         SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ENCGAPDISK_EDGE_FALL]= SEncGapDiskRW[ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_FALL];
         SEncGapDiskRW[ucUnit].usEdgeCnt[ENCGAPDISK_EDGE_FALL]= 0;
         SEncGapDiskRW[ucUnit].usEdgeOffsetCnt[ENCGAPDISK_EDGE_FALL]= 0;

         SEncGapDiskRW[ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_RAISE]= SEncGapDiskRW[ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_FALL];
         SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ENCGAPDISK_EDGE_RAISE]= SEncGapDiskRW[ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_FALL];
         SEncGapDiskRW[ucUnit].usEdgeCnt[ENCGAPDISK_EDGE_RAISE]= 0;
         SEncGapDiskRW[ucUnit].usEdgeOffsetCnt[ENCGAPDISK_EDGE_RAISE]= 0;
         
         // Clear offset and current edge settings
         SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ENCGAPDISK_EDGE_FALL]= 0;
         SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ENCGAPDISK_EDGE_RAISE]= 0;

         SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_FALL].bValid= FALSE;
         SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_FALL].ucItem= 0;
         
         SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_RAISE].bValid= FALSE;
         SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_RAISE].ucItem= 0;
        
         // Clear edge storages
         
         SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_FALL].bValid= FALSE;
         SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_RAISE].bValid= FALSE;
         SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_FALL].ucItem= 0;
         SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_RAISE].ucItem= 0;
      
         for (ulIndex= 0; ulIndex < ENCGAPDISK_EDGEITEM_MAX; ulIndex++)
         {
            SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ENCGAPDISK_EDGE_FALL][ulIndex].ulClks= 0;
            SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ENCGAPDISK_EDGE_FALL][ulIndex].ulConfirm= 0;
            
            SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ENCGAPDISK_EDGE_RAISE][ulIndex].ulClks= 0;
            SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ENCGAPDISK_EDGE_RAISE][ulIndex].ulConfirm= 0;
         }
         
         ucResult= (ulIndex >= ENCGAPDISK_EDGEITEM_MAX);
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskTeachIn
//----------------------------------------------------------------------------
UCHAR EncGapDiskTeachIn(UCHAR ucUnit, UCHAR ucEdgeType, bool bContinous, UCHAR ucGapCnt, ULONG ulClks, USHORT usClksAcc, USHORT usClksBrk, ULONG ulClkLowHz, ULONG ulClkHighHz, ULONG ulClksInit, ULONG ulClkInitHz, UCHAR ucThreshold, USHORT usToleranceClks, USHORT usEdgeDriftMax, USHORT usMicroSteps, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   ULONG ulCycleClks;
   USHORT usToGo;
   USHORT usEdgeCnt;
   UCHAR ucRetry;
   UCHAR ucWipeIdx;
   bool bDir;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
      {
         // Lock data
         if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
         {
            // Check ready condition
            if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
            {   
               // Disable IRQ
               EncGapDiskIrqCtrl(ucUnit, FALSE);
               
               // Clear state
               SEncGapDiskRW[ucUnit].ucState= ENCGAPDISK_STATE_NONE;
   
               // Init/update storage
               SEncGapDiskRW[ucUnit].SPermanent.SVar.ucGapCnt= ucGapCnt;
               SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous= bContinous;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClksInit= ulClksInit;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkInitHz= ulClkInitHz;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkLowHz= ulClkLowHz;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkHighHz= ulClkHighHz;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClks= ulClks;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksAcc= usClksAcc;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksBrk= usClksBrk;
               SEncGapDiskRW[ucUnit].SPermanent.SVar.usToleranceClks= usToleranceClks;
               SEncGapDiskRW[ucUnit].SPermanent.SVar.ucThreshold= ucThreshold;
               SEncGapDiskRW[ucUnit].SPermanent.SVar.usEdgeDriftMax= usEdgeDriftMax;
                    
               EncGapDiskClear(ucUnit, FALSE, u16Timeout);
              
               // Clear mbx (*NOT* safe here because a task is pending on it (which will never get any msg by the mbx after that call) !!!)
               //-OS_MBX_INIT_EX(MbxEncGapDiskMsg[ucUnit]);
                     
               // Init
               SEncGapDiskRW[ucUnit].ucJobId++;
               SEncGapDiskRW[ucUnit].bReverse= FALSE;
               SEncGapDiskRW[ucUnit].ucState= ENCGAPDISK_STATE_TEACHIN;
                          
               // Prepare stepper
               ulCycleClks= (SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous) ? (SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClks* (SEncGapDiskRO[ucUnit].ucScan+ SEncGapDiskRO[ucUnit].ucVerify)) : (SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClks);
               usToGo= (SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous) ? (1) : (SEncGapDiskRO[ucUnit].ucScan);
               bDir= SEncGapDiskRO[ucUnit].bStpClkDirDir;
               ucWipeIdx= (SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous) ? (SEncGapDiskRO[ucUnit].ucScan+ SEncGapDiskRO[ucUnit].ucVerify) : (1);
               ucRetry= 0;
                              
               // Prepare motor position (sync with driver) before enabling IRQ
               
               // Force zero position
               StpClkDirClksPosOffsVal(SEncGapDiskRO[ucUnit].ucStpClkDirUnit, 0, TRUE, u16Timeout);
               
               if (SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClksInit)
               {
                  ucResult= StpClkDirMove(SEncGapDiskRO[ucUnit].ucStpClkDirUnit, bDir, SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClksInit, 0, 0, SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkInitHz, SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkInitHz, usMicroSteps, FALSE, NULL, u16Timeout);
               }
               else
               {
                  ucResult= 1;
               }
               
               if (ucResult)
               {
                  // Wait until movement is done
                  StpClkDirWait(SEncGapDiskRO[ucUnit].ucStpClkDirUnit, OS_T_FOREVER);            
                  
                  // Force zero position (hide prepare move)
                  StpClkDirClksPosOffsVal(SEncGapDiskRO[ucUnit].ucStpClkDirUnit, 0, TRUE, u16Timeout);

                  // Prepare absolute position
                  if (!SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous)
                  {
                     if ((bDir == STPCLKDIR_COUNTERCLOCKWISE) && (StpClkDirClksAbsGet(SEncGapDiskRO[ucUnit].ucStpClkDirUnit) == 0))
                     {
                        StpClkDirClksPosOffsVal(SEncGapDiskRO[ucUnit].ucStpClkDirUnit, ulCycleClks, TRUE, u16Timeout);
                     }
                  } 

                  do
                  {
                     // Enable IRQ
                     EncGapDiskIrqCtrl(ucUnit, TRUE);
                     
                     ucResult= StpClkDirMove(SEncGapDiskRO[ucUnit].ucStpClkDirUnit, bDir, ulCycleClks, SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksAcc, SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksBrk, SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkLowHz, SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkHighHz, usMicroSteps, FALSE, NULL, u16Timeout);
                     if (ucResult)
                     {
                        // Sleep until movement (and running background processing of gaps) is done
                        StpClkDirWait(SEncGapDiskRO[ucUnit].ucStpClkDirUnit, OS_T_FOREVER);
                                         
                        // Disable IRQ
                        EncGapDiskIrqCtrl(ucUnit, FALSE);
                        {
                           // Determine count of confirmed sequential edges
                           usEdgeCnt= _EncGapDiskEdgeCnt(ucUnit, ucEdgeType, ucWipeIdx, NULL, NULL);
                           if (usEdgeCnt < SEncGapDiskRW[ucUnit].SPermanent.SVar.ucGapCnt)
                           {
                              // Add additional scan because we've seen to less gaps by starting with unsuitable direction (and probably hung at hard stop)
                              ucRetry++;  
                              if ((ucRetry <= SEncGapDiskRO[ucUnit].ucRetry)) //- && ((SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_TEACHIN) || (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_MEANIN)))
                              {
                                 usToGo++;
                                 
                                 // Re-initialization (caveat: bReverse is toggled^ later at bottom of outer loop!)
                                 EncGapDiskClear(ucUnit, (SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous) ? (FALSE) : (!SEncGapDiskRW[ucUnit].bReverse), u16Timeout);
                                 SEncGapDiskRW[ucUnit].ucState= ENCGAPDISK_STATE_TEACHIN;
   
                                 // Reload ucWipeIdx for new retry (caveat: ucWipeIdx is incremented later at bottom of outer loop!)
                                 ucWipeIdx= 0;                              
                              }
                              else
                              {
                                 // Cancel the processing
                                 ucResult= 0;
                              }
                           }
                           else
                           {
                              if (!SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous)
                              {
                                 if ((SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_MEANIN) && (usToGo <= 1))
                                 {
                                    // Reload usToGo for next state (caveat: usToGo is decremented later at bottom of outer loop!)
                                    usToGo= SEncGapDiskRO[ucUnit].ucVerify+ 1;
                                 }
                              }
                           }
                           
                           // Toggle^ reverse mode
                           SEncGapDiskRW[ucUnit].bReverse= (SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous) ? (FALSE) : (!SEncGapDiskRW[ucUnit].bReverse);
                        }
                        
                        // Reverse^ direction
                        bDir= (SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous) ? (bDir) : (!bDir);
                     }
                     
                     ucWipeIdx++;
                     usToGo--;
                     
                  } while (usToGo && ucResult);
               }
            }
            
            if (ucResult)
            {
               // Sleep until movement is done
               StpClkDirWait(SEncGapDiskRO[ucUnit].ucStpClkDirUnit, OS_T_FOREVER);
            }
   
            // Disable IRQ
            EncGapDiskIrqCtrl(ucUnit, FALSE);
            
            os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
         }
      }
   }
   
   // Final checks
   if (ucResult)
   {
      // Determine count of confirmed sequential edges (caveat: ucWipeIdx is already incremented once more!)
      ucResult= EncGapDiskEdgeCnt(ucUnit, ucEdgeType, (ucWipeIdx- 1), NULL, u16Timeout);
      if (ucResult)
      {
         // Compare with count of exspected gaps
         if (ucResult < SEncGapDiskRW[ucUnit].SPermanent.SVar.ucGapCnt)
         {
            ucResult= 0;
         }
      }
      
      if (SEncGapDiskRW[ucUnit].ucToleranceErrorCnt[ucEdgeType])
      {
         ucResult= 0;
      }
   }
      
   // Successful teach-in post actions
   if (ucResult)
   {
      // Change to offset correction state
      SEncGapDiskRW[ucUnit].ucState= ENCGAPDISK_STATE_OFFSET;
   }
      
   return (ucResult);
}


//----------------------------------------------------------------------------
// UCHAR EncGapDiskEdgeErrGet
//----------------------------------------------------------------------------
UCHAR EncGapDiskEdgeErrGet(UCHAR ucUnit, UCHAR ucEdgeType, UCHAR *pucEdgeError, ULONG *pulErrorClks, UCHAR *pucEdgeErrorCnt, UCHAR *pucToleranceErrorCnt, UCHAR *pucEdgeIgnoreCnt, bool bClear, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
      {
         // Lock data
         if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
         {
            // Check ready condition
            if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
            {
               if (pucEdgeError)
               {
                  *pucEdgeError= SEncGapDiskRW[ucUnit].ucEdgeError[ucEdgeType];
                  if (bClear) SEncGapDiskRW[ucUnit].ucEdgeError[ucEdgeType]= 0;
                  
                  ucResult++;
               }
               
               if (pulErrorClks)
               {
                  *pulErrorClks= SEncGapDiskRW[ucUnit].ulErrorClks[ucEdgeType];
                  if (bClear) SEncGapDiskRW[ucUnit].ulErrorClks[ucEdgeType]= 0;
                  
                  ucResult++;
               }
               
               if (pucEdgeErrorCnt)
               {
                  *pucEdgeErrorCnt= SEncGapDiskRW[ucUnit].ucEdgeErrorCnt[ucEdgeType];
                  if (bClear) SEncGapDiskRW[ucUnit].ucEdgeErrorCnt[ucEdgeType]= 0;
                  
                  ucResult++;
               }
               
               if (pucToleranceErrorCnt)
               {
                  *pucToleranceErrorCnt= SEncGapDiskRW[ucUnit].ucToleranceErrorCnt[ucEdgeType];
                  if (bClear) SEncGapDiskRW[ucUnit].ucToleranceErrorCnt[ucEdgeType]= 0;
                  
                  ucResult++;
               }
                             
               if (pucEdgeIgnoreCnt)
               {
                  *pucEdgeIgnoreCnt= SEncGapDiskRW[ucUnit].ucEdgeIgnoreCnt[ucEdgeType];
                  if (bClear) SEncGapDiskRW[ucUnit].ucEdgeIgnoreCnt[ucEdgeType]= 0;
                  
                  ucResult++;
               }              
            }
            
            os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskStatGet
//----------------------------------------------------------------------------
UCHAR EncGapDiskStatGet(UCHAR ucUnit, UCHAR ucEdgeType, UCHAR *pusEdgeCnt, ULONG *pulCycleCnt, ULONG *pulEdgeIterationCnt, bool bClear, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
      {   
         // Lock data
         if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
         {
            // Check ready condition
            if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
            {           
               if (pusEdgeCnt)
               {
                  *pusEdgeCnt= SEncGapDiskRW[ucUnit].usEdgeCnt[ucEdgeType];
                  if (bClear) SEncGapDiskRW[ucUnit].usEdgeCnt[ucEdgeType]= 0;
                  
                  ucResult++;
               }
   
               if (pulCycleCnt)
               {
                  *pulCycleCnt= SEncGapDiskRW[ucUnit].ulCycleCnt;
                  if (bClear) SEncGapDiskRW[ucUnit].ulCycleCnt= 0;
                  ucResult++;
               }
               
               if (pulEdgeIterationCnt)
               {
                  *pulEdgeIterationCnt= SEncGapDiskRW[ucUnit].ulEdgeIterationCnt;
                  if (bClear) SEncGapDiskRW[ucUnit].ulEdgeIterationCnt= 0;
                  ucResult++;
               }            
            }
            
            os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskOffsetGet
//----------------------------------------------------------------------------
UCHAR EncGapDiskOffsetGet(UCHAR ucUnit, UCHAR ucEdgeType, UCHAR *pusEdgeOffsetCnt, bool bClear, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
      {   
         // Lock data
         if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
         {
            // Check ready condition
            if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
            {
               if (pusEdgeOffsetCnt)
               {
                  *pusEdgeOffsetCnt= SEncGapDiskRW[ucUnit].usEdgeOffsetCnt[ucEdgeType];
                  if (bClear) SEncGapDiskRW[ucUnit].usEdgeOffsetCnt[ucEdgeType]= 0;
                  
                  ucResult++;
               }
            }
            
            os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskEdgeCnt
//----------------------------------------------------------------------------
UCHAR EncGapDiskEdgeCnt(UCHAR ucUnit, UCHAR ucEdgeType, ULONG ulConfirm, USHORT *pusEdgeCntTotal, U16 u16Timeout)
{
   UCHAR ucResult= 0;
     
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
      {
         // Lock data
         if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
         {
            ucResult= _EncGapDiskEdgeCnt(ucUnit, ucEdgeType, ulConfirm, pusEdgeCntTotal, NULL);
            
            os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR _EncGapDiskEdgeCnt
//----------------------------------------------------------------------------
UCHAR _EncGapDiskEdgeCnt(UCHAR ucUnit, UCHAR ucEdgeType, ULONG ulConfirm, USHORT *pusEdgeCntTotal, USHORT *usEdgeIdxStart)
{
   UCHAR ucResult= 0;
   
   UCHAR ucIndex;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
      {
         if (pusEdgeCntTotal) *pusEdgeCntTotal= SEncGapDiskRW[ucUnit].usEdgeCnt[ucEdgeType];
         
         // Check ready condition
         if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
         {
            if (ulConfirm)
            {
               // Skip unconfirmed edges
               for (ucIndex= SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ucEdgeType]; ucIndex < SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType]; ucIndex++)
               {
                  if (SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucIndex].ulConfirm >= ulConfirm)
                  {
                     break;
                  }
               }
               
               if (usEdgeIdxStart) *usEdgeIdxStart= ucIndex;
               
               // Count sequential confirmed edges
               for (; ucIndex < SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType]; ucIndex++)
               {
                  if (SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucIndex].ulConfirm >= ulConfirm)
                  {
                     ucResult++;
                  }
                  else
                  {
                     break;
                  }
               }
            }
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskEdgeGet
//----------------------------------------------------------------------------
UCHAR EncGapDiskEdgeGet(UCHAR ucUnit, UCHAR *pucEdgeCurr, UCHAR ucEdgeType, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
      {   
         // Lock data
         if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
         {
            // Check ready condition
            if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
            {
               if (SEncGapDiskRW[ucUnit].SEdgeCurr[ucEdgeType].bValid)
               {
                  if (pucEdgeCurr) *pucEdgeCurr= SEncGapDiskRW[ucUnit].SEdgeCurr[ucEdgeType].ucItem;
                  ucResult= 1;
               }
            }
            
             os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
         }
      }
   }           
               
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskEdgeDataGet
//----------------------------------------------------------------------------
UCHAR EncGapDiskEdgeDataGet(UCHAR ucUnit, UCHAR ucEdgeItem, UCHAR ucEdgeType, ULONG *pulClks, ULONG *pulConfirm, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   UCHAR ucSeqMax;
   UCHAR ucEdgeItemCurr;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeItem < ENCGAPDISK_EDGEITEM_MAX)
      {
         if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
         {
            // Lock data
            if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
            {
               // Check ready condition
               if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
               {
                  ucSeqMax= SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType];
                  if (ucSeqMax)
                  {
                     ucEdgeItemCurr= (ucEdgeItem+ SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ucEdgeType]) % ucSeqMax;
                                  
                     ucEdgeItemCurr+= SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ucEdgeType];
                     if (ucEdgeItemCurr <= SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType])
                     {
                        ucResult++;
                        
                        if (pulClks)
                        {
                           *pulClks= SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucEdgeItemCurr].ulClks;
                           ucResult++;
                        }
                                              
                        if (pulConfirm)
                        {                  
                           *pulConfirm= SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucEdgeItemCurr].ulConfirm;
                           ucResult++;
                        }
                     }
                  }
               }
               
               os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
            }
         }
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// bool EncGapDiskHomeEdgeGet
//----------------------------------------------------------------------------
bool EncGapDiskHomeEdgeGet(UCHAR ucUnit, UCHAR ucEdgeType, ULONG ulConfirm, UCHAR ucRepeat, bool bRepeatEqual, UCHAR *pucEdgeItem, U16 u16Timeout)
{
   bool bResult= FALSE;

   ULONG ulDiff;
   ULONG ulDistanceMinClks;
   ULONG ulDistanceMinThreshold;
   ULONG ulDistanceClks[ENCGAPDISK_EDGEITEM_MAX+ 1];
     
   USHORT usEdgeCount;
   USHORT usEdgeStartSeq;
   USHORT usCheckIdx;
   USHORT usSearchIdx;
   USHORT usCompareIdx;
   USHORT usDistanceSeqMax;
   USHORT usLoop;
   USHORT usTry;
   USHORT usMod;

   USHORT usRepeatCnt;

   UCHAR ucDistanceIdx[ENCGAPDISK_EDGEITEM_MAX+ 1];

   UCHAR ucDistanceMinIdx;
   bool bDistanceMinThresholdSet;
      
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if ((ucEdgeType < ENCGAPDISK_EDGETYPE_MAX) && (ucRepeat < ENCGAPDISK_EDGEITEM_MAX))
      {
         if (pucEdgeItem)
         {
            if (ucRepeat)
            {
               // Lock data
               if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
               {
                  // Determine count and start index of valid edges
                  usEdgeCount= _EncGapDiskEdgeCnt(ucUnit, ucEdgeType, ulConfirm, NULL, &usEdgeStartSeq);
   
                  if (usEdgeCount > 1)
                  {
                     // Build distance array data
                     usMod= usEdgeCount+ usEdgeStartSeq;
                     for (usDistanceSeqMax= 0, usSearchIdx= usEdgeStartSeq, usCompareIdx= usSearchIdx+ 1, usLoop= 0; usLoop < usEdgeCount; usSearchIdx= (usSearchIdx+ 1) % usMod, usCompareIdx= (usCompareIdx+ 1) % usMod, usLoop++)
                     {
                        if (SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][usSearchIdx].ulClks < SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][usCompareIdx].ulClks)
                        {
                           ulDistanceClks[usDistanceSeqMax]= SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][usCompareIdx].ulClks- SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][usSearchIdx].ulClks;
                        }
                        else
                        {
                           ulDistanceClks[usDistanceSeqMax]= (SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][usCompareIdx].ulClks+ StpClkDirModuloMaxClksGet(SEncGapDiskRO[ucUnit].ucStpClkDirUnit))- SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][usSearchIdx].ulClks;
                        }
                        
                        ucDistanceIdx[usDistanceSeqMax]= usCompareIdx;
                        usDistanceSeqMax= _umin(usDistanceSeqMax+ 1, (ENCGAPDISK_EDGEITEM_MAX+ 1));
                     }
                       
                     if (usDistanceSeqMax >= ucRepeat)
                     {
                        ulDistanceMinThreshold= 0;
                        bDistanceMinThresholdSet= FALSE;
                        usTry= 0;
                        
                        do
                        {
                           // Find minimum distance (but raise minimum threshold for every failed sequence tried)
                           for (usLoop= 0, ulDistanceMinClks= 0xFFFFFFFF; usLoop < usDistanceSeqMax; usLoop++)
                           {                 
                              if ((ulDistanceClks[usLoop] < ulDistanceMinClks) && ((ulDistanceClks[usLoop] > ulDistanceMinThreshold) || (!bDistanceMinThresholdSet) || (!usTry)))
                              {
                                 ulDistanceMinClks= ulDistanceClks[usLoop];
                                 ucDistanceMinIdx= usLoop;
                              }
                           }
                           
                           // Store last minimum data found
                           ulDistanceMinThreshold= ulDistanceMinClks;
                           bDistanceMinThresholdSet= TRUE;
                           
                           // Find given number of repeats for minimum distance found yet
                           for (usRepeatCnt= 0, usCompareIdx= ucDistanceMinIdx+ 1, usLoop= 1; usLoop < usDistanceSeqMax; usCompareIdx= (usCompareIdx+ 1) % usMod, usLoop++)
                           {
                              ulDiff= ABS(ulDistanceClks[ucDistanceMinIdx], ulDistanceClks[usCompareIdx]);
                              if (ulDiff <= SEncGapDiskRW[ucUnit].SPermanent.SVar.usToleranceClks)
                              {
                                 ulDiff= ABS(ucDistanceMinIdx, usCompareIdx);
                                 if ((ulDiff == 1) || ((ucDistanceMinIdx == usEdgeStartSeq) && (usCompareIdx == (usEdgeStartSeq+ (usEdgeCount- 1)))))
                                 {
                                    usRepeatCnt++;
                                    usSearchIdx= _umax(ucDistanceMinIdx, usCompareIdx);
                                 }
                              }
                           }
                                                                          
                           // Check for - more than specified - repeats (this may invalidate current repeat sequence found)
                           if (bRepeatEqual)
                           {
                              if (usRepeatCnt >= ucRepeat)
                              {
                                 // Avoid comparison of 2nd item with 1st item for end repeat check
                                 usCheckIdx= (((usSearchIdx+ 1) % usMod) == ucDistanceMinIdx) ? (ucDistanceMinIdx) : (usSearchIdx);
                                 
                                 ulDiff= ABS(ulDistanceClks[usCheckIdx], ulDistanceClks[(usCheckIdx+ 1) % usMod]);
                                 if (ulDiff <= SEncGapDiskRW[ucUnit].SPermanent.SVar.usToleranceClks)
                                 {
                                    usRepeatCnt= 0;
                                 }
                              }
                           }

                           usTry++;
                           
                        } while ((usRepeatCnt < ucRepeat) && usTry <= usDistanceSeqMax);
                        
                        // Return the gap number of the end of the valid pattern
                        if (usRepeatCnt >= ucRepeat)
                        {
                           *pucEdgeItem= ucDistanceIdx[usCheckIdx];
                           bResult= TRUE;
                        }
                     }
                  }
                  
                  os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
               }
            }

            if (!bResult)
            {
               *pucEdgeItem= 0;
            }
         }
      }
   }
      
   return (bResult);
}

//----------------------------------------------------------------------------
// bool EncGapDiskEdgeZeroSet
//----------------------------------------------------------------------------
bool EncGapDiskEdgeZeroSet(UCHAR ucUnit, UCHAR ucEdgeType, UCHAR ucZeroItem, CHAR cZeroOffset, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   UCHAR ucSeqMax;
   UCHAR ucZeroItemCurr;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
      {
         // Lock data
         if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
         {
            // Check ready condition
            if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
            {
               ucSeqMax= SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType];
               if (ucSeqMax)
               {
                  if ((cZeroOffset < 0) && ((cZeroOffset* (-1)) > ucZeroItem))
                  {
                     ucZeroItemCurr= (ucSeqMax+ cZeroOffset)+ ucZeroItem;
                  }
                  else
                  {
                     ucZeroItemCurr= (ucZeroItem+ cZeroOffset);
                  }            
                  
                  if (ucZeroItemCurr < ucSeqMax)
                  {
                     SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ucEdgeType]= ucZeroItemCurr;
         
                     SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_FALL].bValid= FALSE;
                     SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_RAISE].bValid= FALSE;
                     SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_FALL].ucItem= 0;
                     SEncGapDiskRW[ucUnit].SEdgeCurr[ENCGAPDISK_EDGE_RAISE].ucItem= 0;
                  
                     bResult= TRUE;
                  }
               }
            }
         
            os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
         }
      }   
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// size_t EncGapDiskCalibrationDataSizeGet
//----------------------------------------------------------------------------
size_t EncGapDiskCalibrationDataSizeGet(void)
{
   return (sizeof(struct _SEncGapDiskCalibrationData));
}

//----------------------------------------------------------------------------
// void * EncGapDiskCalibrationDataAddrGet
//----------------------------------------------------------------------------
void * EncGapDiskCalibrationDataAddrGet(UCHAR ucUnit)
{
   void *pvResult= NULL;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      pvResult= (void *) &SEncGapDiskCalibrationData[ucUnit];
   }
   
   return (pvResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskCalibrationDataRW
//----------------------------------------------------------------------------
UCHAR EncGapDiskCalibrationDataRW(UCHAR ucUnit, ULONG ulConfirm, bool bRestore, U16 u16Timeout)
{
   UCHAR ucResult= 1;

   UCHAR ucEdgeType;
   UCHAR ucIndex;
   UCHAR ucIndexSub;
   ULONG ulCalibrationDataConfirm;

   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      // Lock data
      if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check ready condition
         if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
         {  
            if (!bRestore)
            {
               // Get calibration data - get private module structure entries
               
               for (ucEdgeType= 0; ucEdgeType < ENCGAPDISK_EDGETYPE_MAX; ucEdgeType++)
               {
                  for (ucIndex= 0, ucIndexSub= 0; ucIndex < SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType]; ucIndex++)
                  {
                     if (SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucIndex].ulConfirm >= ulConfirm)
                     {
                        SEncGapDiskCalibrationData[ucUnit].SEncEdge[ucEdgeType][ucIndexSub++].ulClks= SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucIndex].ulClks;
                     }
                  }

                  //-SEncGapDiskCalibrationData[ucUnit].ucEdgeSeqMin[ucEdgeType] = _min(ENCGAPDISK_EDGEITEM_MAX, SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ucEdgeType]);
                  SEncGapDiskCalibrationData[ucUnit].ucEdgeSeqMin[ucEdgeType] = 0;
                  //-SEncGapDiskCalibrationData[ucUnit].ucEdgeSeqMax[ucEdgeType] = _min(ENCGAPDISK_EDGEITEM_MAX, SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType]);
                  SEncGapDiskCalibrationData[ucUnit].ucEdgeSeqMax[ucEdgeType] = _min(ENCGAPDISK_EDGEITEM_MAX, ucIndexSub);
                  SEncGapDiskCalibrationData[ucUnit].ucEdgeZero[ucEdgeType]   = SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ucEdgeType];
                  SEncGapDiskCalibrationData[ucUnit].usEdgeDriftMax           = SEncGapDiskRW[ucUnit].SPermanent.SVar.usEdgeDriftMax;
                  
                  // Additional variables
                  SEncGapDiskCalibrationData[ucUnit].ulConfirm[ucEdgeType]    = ulConfirm;
               }
            }                     
            else
            {
               for (ucEdgeType= 0; ucEdgeType < ENCGAPDISK_EDGETYPE_MAX; ucEdgeType++)
               {
                  // Set ulConfirm passed by function if it is higher than the last passed one
                  // First time after MC restart ulConfirm is always retrieved (SEncGapDiskCalibrationData = ZI-Data)
                  ulCalibrationDataConfirm= SEncGapDiskCalibrationData[ucUnit].ulConfirm[ucEdgeType];
                  ulCalibrationDataConfirm= (ulCalibrationDataConfirm >= ulConfirm) ? (ulCalibrationDataConfirm) : (ulConfirm);
                                 
                  // Set calibration data - set private module structure entries
                  
                  for (ucIndex= 0; ucIndex < SEncGapDiskCalibrationData[ucUnit].ucEdgeSeqMax[ucEdgeType]; ucIndex++)
                  {
                     SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucIndex].ulClks=
                        SEncGapDiskCalibrationData[ucUnit].SEncEdge[ucEdgeType][ucIndex].ulClks;
   
                     SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucIndex].ulConfirm= ulCalibrationDataConfirm;                      
                  }
                  
                  SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ucEdgeType]   = _min(ENCGAPDISK_EDGEITEM_MAX, SEncGapDiskCalibrationData[ucUnit].ucEdgeSeqMin[ucEdgeType]);
                  SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType]   = _min(ENCGAPDISK_EDGEITEM_MAX, SEncGapDiskCalibrationData[ucUnit].ucEdgeSeqMax[ucEdgeType]);
                  SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ucEdgeType]     = SEncGapDiskCalibrationData[ucUnit].ucEdgeZero[ucEdgeType];
                  SEncGapDiskRW[ucUnit].SPermanent.SEdge.ulConfirm[ucEdgeType]      = ulCalibrationDataConfirm;
                  SEncGapDiskRW[ucUnit].SPermanent.SVar.usEdgeDriftMax              = SEncGapDiskCalibrationData[ucUnit].usEdgeDriftMax;
               }

#if ENCGAPDISK_OFFSETCORRECTION > 0
               // Enable offset correction state based on re-loaded data
               SEncGapDiskRW[ucUnit].ucState= ENCGAPDISK_STATE_OFFSET;
#endif
            }
         }
         os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
      }
   }

   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskRunParamsRW
//----------------------------------------------------------------------------
UCHAR EncGapDiskRunParamsRW(UCHAR ucUnit, void *pvEncGapDiskRunParams, bool bRestore, U16 u16Timeout)
{
   UCHAR    ucResult= 1;
   struct   _SEncGapDiskRunParams *pSEncGapDiskRunParams;
         
   // cast input arg (void *) to encgapdisk run params structure pointer
   pSEncGapDiskRunParams= (struct _SEncGapDiskRunParams*) pvEncGapDiskRunParams;       

   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      // Lock data
      if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
      {
         // Check ready condition
         if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
         {
            if (!bRestore)
            {
               // get encoder gap disk run params from private module structure entries
               pSEncGapDiskRunParams->ulClkInitHz  = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkInitHz;
               pSEncGapDiskRunParams->ulClksInit   = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClksInit;
               pSEncGapDiskRunParams->ulClkLowHz   = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkLowHz;
               pSEncGapDiskRunParams->ulClkHighHz  = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkHighHz;
               pSEncGapDiskRunParams->ulClks       = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClks;
               pSEncGapDiskRunParams->usClksAcc    = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksAcc;
               pSEncGapDiskRunParams->usClksBrk    = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksBrk;
               pSEncGapDiskRunParams->bContinous   = SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous;

               ucResult= StpClkDirDacGet(ucUnit, &(pSEncGapDiskRunParams->usDacValueHold), &(pSEncGapDiskRunParams->usDacValueRun), u16Timeout);

               if (ucResult)
               {
                  // get current motor position in clocks since first device start
                  pSEncGapDiskRunParams->u32ClkCntAbsolute= StpClkDirClksAbsGet(ucUnit);
               }
            }                     
            else
            {
               // set encoder gap disk run params into private module structure entries

               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkInitHz = pSEncGapDiskRunParams->ulClkInitHz;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClksInit  = pSEncGapDiskRunParams->ulClksInit;              
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkLowHz  = pSEncGapDiskRunParams->ulClkLowHz;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkHighHz = pSEncGapDiskRunParams->ulClkHighHz;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClks      = pSEncGapDiskRunParams->ulClks;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksAcc   = pSEncGapDiskRunParams->usClksAcc;
               SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksBrk   = pSEncGapDiskRunParams->usClksBrk;
               SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous            = pSEncGapDiskRunParams->bContinous;

               ucResult= StpClkDirDacSet(ucUnit, pSEncGapDiskRunParams->usDacValueHold, pSEncGapDiskRunParams->usDacValueRun, u16Timeout);

               if (ucResult)
               {
                  // set absolute motor position in clocks since first device start
                  ucResult= StpClkDirClksAbsSet(ucUnit, pSEncGapDiskRunParams->u32ClkCntAbsolute, u16Timeout);
               }
            }
         }        
         os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
      }
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskEdgeGoto
//----------------------------------------------------------------------------
UCHAR EncGapDiskEdgeGoto(UCHAR ucUnit, UCHAR ucEdgeItem, UCHAR ucEdgeType, USHORT usMicroSteps, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   ULONG ulClksCurr;
   ULONG ulClksGap;
   ULONG ulClksAcc;
   ULONG ulClksBrk;
   ULONG ulClkHighHz;
   ULONG ulClksOvr;
   ULONG ulClks;
   
   UCHAR ucSeqMax;
   UCHAR ucEdgeItemCurr;

   bool bDir;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeItem < ENCGAPDISK_EDGEITEM_MAX)
      {
         if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
         {
            // Lock data
            if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
            {
               // Check ready condition
               if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
               {
                  ucSeqMax= SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType];
                  if (ucSeqMax)
                  {
                     // Determine array index from given user index
                     ucEdgeItemCurr= (ucEdgeItem+ SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ucEdgeType]) % ucSeqMax;
                     ucEdgeItemCurr+= SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ucEdgeType];
                                  
                     if (ucEdgeItemCurr <= SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType])
                     {
                        ulClksCurr= StpClkDirClksAbsGet(SEncGapDiskRO[ucUnit].ucStpClkDirUnit);                  
                        ulClksGap= SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucEdgeItemCurr].ulClks;
      
                        if (ulClksCurr != ulClksGap)
                        {
                           // Standard Single mode - we cannot shorten movements because we assume yet rewind is required
      
                           if (ulClksCurr < ulClksGap)
                           {
                              bDir= SEncGapDiskRO[ucUnit].bStpClkDirDir;
                              ulClks= ulClksGap- ulClksCurr;
                           }
                           else
                           {
                              bDir= !SEncGapDiskRO[ucUnit].bStpClkDirDir;
                              ulClks= ulClksCurr- ulClksGap;
                           }
                           
                           if (SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous)
                           {
                              // Continous mode - we may use overrun to shorten movement
                              
                              if (SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClks >= ulClks)
                              {
                                 ulClksOvr= SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClks- ulClks;
                              }
                              else
                              {
                                 ulClksOvr= ulClks- SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClks;
                              }
                              
                              // Use overrun when move is shorter
                              if (ulClksOvr < ulClks)
                              {
                                 bDir= !bDir;
                                 ulClks= ulClksOvr;
                              }
                           }

#if ENCGAPDISK_OFFSETCORRECTION > 0
                           // Control IRQ for offset recording
                           EncGapDiskIrqCtrl(ucUnit, (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET));
#endif
                          
                           ulClksAcc= SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksAcc;
                           ulClksBrk= SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksBrk;
                           ulClkHighHz= SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkHighHz;
                           
                           // Prepare, setup and start move
                           if (StpClkDirProfileAdjust(ucUnit, ulClks, &ulClksAcc, &ulClksBrk, SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkLowHz, &ulClkHighHz))
                           {
                              ucResult= StpClkDirMove(SEncGapDiskRO[ucUnit].ucStpClkDirUnit, bDir, ulClks, ulClksAcc, ulClksBrk, SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkLowHz, ulClkHighHz, usMicroSteps, FALSE, NULL, u16Timeout);
                           }
                           
                           // Store valid destination
                           if (ucResult)
                           {
                              SEncGapDiskRW[ucUnit].SEdgeCurr[ucEdgeType].bValid= TRUE;
                              SEncGapDiskRW[ucUnit].SEdgeCurr[ucEdgeType].ucItem= ucEdgeItem;
                           }
                        }
                        else
                        {
                           // Set success state when already in position
                           ucResult= 1;
                        }
                     }
                  }
               }
               
               os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
            }
         }
      }
   }
   
   // Debug
   if (!ucResult)
   {
      ucResult= ucResult;
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// size_t EncGapDiskSnapShotSizeGet
//----------------------------------------------------------------------------
size_t EncGapDiskSnapShotSizeGet(void)
{
   return (sizeof(struct _SEncGapDiskSnapshot));
}

//----------------------------------------------------------------------------
// void * EncGapDiskSnapShotAddrGet
//----------------------------------------------------------------------------
void * EncGapDiskSnapShotAddrGet(UCHAR ucUnit)
{
   void *pvResult= NULL;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      pvResult= (void *) &SEncGapDiskSnapShot[ucUnit];
   }
   
   return (pvResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskSnapShot
//----------------------------------------------------------------------------
UCHAR EncGapDiskSnapShot(UCHAR ucUnit, ULONG ulConfirm, bool bRestore, U16 u16Timeout)
{
   UCHAR ucResult= 0;
   
   ULONG ulSnapShotConfirm;
   UCHAR ucSnapShotEdgeType;
   UCHAR ucEdgeType;
   UCHAR ucIndex;
   UCHAR ucIndexSub;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      if (ucEdgeType < ENCGAPDISK_EDGETYPE_MAX)
      {
         // Lock data
         if (OS_SEM_TAKE_CHK(os_sem_take(SEncGapDiskRW[ucUnit].SemStart, u16Timeout)))
         {
            // Check ready condition
            if (!SEncGapDiskRW[ucUnit].bIrq || (SEncGapDiskRW[ucUnit].bIrq && (SEncGapDiskRW[ucUnit].ucState == ENCGAPDISK_STATE_OFFSET)))
            {  
               if (!bRestore)
               {
                  // Transfer permanent structure to snapshot structure (single edge only)
               
                  SEncGapDiskSnapShot[ucUnit].SVar.usToleranceClks      = SEncGapDiskRW[ucUnit].SPermanent.SVar.usToleranceClks;
                  SEncGapDiskSnapShot[ucUnit].SVar.ucThreshold          = SEncGapDiskRW[ucUnit].SPermanent.SVar.ucThreshold;
                  SEncGapDiskSnapShot[ucUnit].SVar.ucGapCnt             = SEncGapDiskRW[ucUnit].SPermanent.SVar.ucGapCnt;
                  SEncGapDiskSnapShot[ucUnit].SVar.bContinous           = SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous;
                  SEncGapDiskSnapShot[ucUnit].SVar.usEdgeDriftMax       = SEncGapDiskRW[ucUnit].SPermanent.SVar.usEdgeDriftMax;
                  
                  SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.ulClkInitHz= SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkInitHz;
                  SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.ulClksInit = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClksInit;
                  SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.ulClkLowHz = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkLowHz;
                  SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.ulClkHighHz= SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkHighHz;
                  SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.ulClks     = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClks;
                  SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.usClksAcc  = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksAcc;
                  SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.usClksBrk  = SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksBrk;

                  for (ucEdgeType= 0; ucEdgeType < ENCGAPDISK_EDGETYPE_MAX; ucEdgeType++)
                  {                  
                     SEncGapDiskSnapShot[ucUnit].SEdge.ulConfirm[ucEdgeType]   = ulConfirm;
                     //-SEncGapDiskSnapShot[ucUnit].SEdge.ucEdgeSeqMin[ucEdgeType]= _umin(ENCGAPDISK_EDGEITEM_MAX, SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ucEdgeType]);
                     //-SEncGapDiskSnapShot[ucUnit].SEdge.ucEdgeSeqMax[ucEdgeType]= _umin(ENCGAPDISK_EDGEITEM_MAX, SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType]);
                     SEncGapDiskSnapShot[ucUnit].SEdge.ucEdgeZero[ucEdgeType]  = SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ucEdgeType];
                  
                     // Transfer only clk position
                     
                     for (ucIndex= 0, ucIndexSub= 0; ucIndex < SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucEdgeType]; ucIndex++)
                     {
                        if (SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucIndex].ulConfirm >= ulConfirm)
                        {
                           SEncGapDiskSnapShot[ucUnit].SEdge.SEncEdge[ucEdgeType][ucIndexSub++].ulClks= SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucEdgeType][ucIndex].ulClks;
                        }
                     }
                     
                     SEncGapDiskSnapShot[ucUnit].SEdge.ucEdgeSeqMin[ucEdgeType]= 0;
                     SEncGapDiskSnapShot[ucUnit].SEdge.ucEdgeSeqMax[ucEdgeType]= _umin(ENCGAPDISK_EDGEITEM_MAX, ucIndexSub);
                  }                  
               }
               else
               {                 
                  // Transfer snapshot structure to permanent structure (single edge only)
                  
                  SEncGapDiskRW[ucUnit].SPermanent.SVar.usToleranceClks       = SEncGapDiskSnapShot[ucUnit].SVar.usToleranceClks;
                  SEncGapDiskRW[ucUnit].SPermanent.SVar.ucThreshold           = SEncGapDiskSnapShot[ucUnit].SVar.ucThreshold;
                  SEncGapDiskRW[ucUnit].SPermanent.SVar.ucGapCnt              = SEncGapDiskSnapShot[ucUnit].SVar.ucGapCnt;
                  SEncGapDiskRW[ucUnit].SPermanent.SVar.bContinous            = SEncGapDiskSnapShot[ucUnit].SVar.bContinous;
                  SEncGapDiskRW[ucUnit].SPermanent.SVar.usEdgeDriftMax        = SEncGapDiskSnapShot[ucUnit].SVar.usEdgeDriftMax;
                  
                  SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkInitHz = SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.ulClkInitHz;
                  SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClksInit  = SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.ulClksInit;
                  SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkLowHz  = SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.ulClkLowHz;
                  SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClkHighHz = SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.ulClkHighHz;
                  SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.ulClks      = SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.ulClks;
                  SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksAcc   = SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.usClksAcc;
                  SEncGapDiskRW[ucUnit].SPermanent.SStpClkDirArgs.usClksBrk   = SEncGapDiskSnapShot[ucUnit].SStpClkDirArgs.usClksBrk;

                  for (ucEdgeType= 0; ucEdgeType < ENCGAPDISK_EDGETYPE_MAX; ucEdgeType++)
                  {
                     ulSnapShotConfirm= SEncGapDiskSnapShot[ucUnit].SEdge.ulConfirm[ucEdgeType];
                     ulSnapShotConfirm= (ulSnapShotConfirm >= ulConfirm) ? (ulSnapShotConfirm) : (ulConfirm);

                     SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ucSnapShotEdgeType] = _umin(ENCGAPDISK_EDGEITEM_MAX, SEncGapDiskSnapShot[ucUnit].SEdge.ucEdgeSeqMin[ucEdgeType]);
                     SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucSnapShotEdgeType] = _umin(ENCGAPDISK_EDGEITEM_MAX, SEncGapDiskSnapShot[ucUnit].SEdge.ucEdgeSeqMax[ucEdgeType]);
                     SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeZero[ucSnapShotEdgeType]   = SEncGapDiskSnapShot[ucUnit].SEdge.ucEdgeZero[ucEdgeType];
                     SEncGapDiskRW[ucUnit].SPermanent.SEdge.ulConfirm[ucEdgeType]            = ulSnapShotConfirm;
                     
                     // Transfer only clk position (confirm is globally set to user specified value)
                     
                     for (ucIndex= 0; ucIndex < SEncGapDiskRW[ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ucSnapShotEdgeType]; ucIndex++)
                     {
                        SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucSnapShotEdgeType][ucIndex].ulClks    = SEncGapDiskSnapShot[ucUnit].SEdge.SEncEdge[ucEdgeType][ucIndex].ulClks;
                        SEncGapDiskRW[ucUnit].SPermanent.SEdge.SEncEdge[ucSnapShotEdgeType][ucIndex].ulConfirm = ulSnapShotConfirm;
                     }
                  }

#if ENCGAPDISK_OFFSETCORRECTION > 0
                  // Enable offset correction state based on re-loaded data
                  SEncGapDiskRW[ucUnit].ucState= ENCGAPDISK_STATE_OFFSET;
#endif
               }

               ucResult= 1;
            }
         
            os_sem_give(SEncGapDiskRW[ucUnit].SemStart);
         }
      }   
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskStpClkDirUnitGet
//----------------------------------------------------------------------------
UCHAR EncGapDiskStpClkDirUnitGet(UCHAR ucUnit)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      ucResult= SEncGapDiskRO[ucUnit].ucStpClkDirUnit;
   }
   
   return (ucResult);
}

//----------------------------------------------------------------------------
// __task void TskEncGapDiskMsg
//----------------------------------------------------------------------------
__task void TskEncGapDiskMsg(void *pvParam)
{
   struct _SEncGapDiskTsk *pSEncGapDiskTsk= (struct _SEncGapDiskTsk *) pvParam;
   
   register ULONG ulClks;
   register ULONG ulDiff;
   register ULONG ulIndex;
   register ULONG ulIndex2;

#if ENCGAPDISK_OFFRANGEMODULOCHECK > 0
   register ULONG ulDiff2;
   register ULONG ulModulo;
#endif

   register ULONG ulMinDiff;
   register ULONG ulMinDiffErr;
   register ULONG ulMinIndex;

   register bool bLevel;
   register bool bValid;
   register bool bSeqAdjust;
   
   struct _SEncGapDiskEncEdge *pSEdge;
   
   ULONG *pulEdgeClkCmp;
   ULONG *pulErrorClks;
   USHORT *pusEdgeCnt;
   USHORT *pusEdgeOffsetCnt;
   UCHAR *pucEdgeSeq;
   UCHAR *pucEdgeSeqMin;
   UCHAR *pucEdgeSeqMax;
   UCHAR *pucToleranceErrorCnt;
   UCHAR *pucEdgeIgnoreCnt;
   UCHAR *pucEdgeError;
   UCHAR *pucEdgeErrorCnt;
   UCHAR *pucCompare;
   UCHAR *pucCompareDelay;

   ULONG ulValue;
   LONG  lValue;
   
   bSeqAdjust= FALSE;

#if ENCGAPDISK_OFFRANGEMODULOCHECK > 0
   ulModulo= StpClkDirModuloMaxClksGet(SEncGapDiskRO[pSEncGapDiskTsk->ucUnit].ucStpClkDirUnit);
#endif   

   FOREVER
   {
      // Pend on new clks message
      if (OS_MBX_VALUE_CHK(os_mbx_wait(&MbxEncGapDiskMsg[pSEncGapDiskTsk->ucUnit], OS_MBX_VALUE_GET(ulValue), OS_T_FOREVER)))
      {
         ulClks= (ulValue & ~0x80000000);
         bLevel= (ulValue & 0x80000000) >> 31;
                 
         // Toggle level suitable to bReverse to place "fall" and "raise" clks into right storage
         bLevel= (!SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].bReverse) ? (bLevel) : (!bLevel);
        
         // Set pointers to current edge specified by bLevel
         if (bLevel == SEncGapDiskRO[pSEncGapDiskTsk->ucUnit].bGapLevel)
         {
            pSEdge= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SEdge.SEncEdge[ENCGAPDISK_EDGE_FALL][SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_FALL]];

            pusEdgeCnt= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].usEdgeCnt[ENCGAPDISK_EDGE_FALL];
            pusEdgeOffsetCnt= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].usEdgeOffsetCnt[ENCGAPDISK_EDGE_FALL];
            pucEdgeSeq= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_FALL];
            pucEdgeSeqMin= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ENCGAPDISK_EDGE_FALL];
            pucEdgeSeqMax= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ENCGAPDISK_EDGE_FALL];
            
            pulEdgeClkCmp= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ulEdgeClksCmp[ENCGAPDISK_EDGE_FALL][0];
            pucCompare= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucCompare[ENCGAPDISK_EDGE_FALL];
            pucCompareDelay= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucCompareDelay[ENCGAPDISK_EDGE_FALL];
            
            pucToleranceErrorCnt= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucToleranceErrorCnt[ENCGAPDISK_EDGE_FALL];
            pucEdgeIgnoreCnt= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeIgnoreCnt[ENCGAPDISK_EDGE_FALL];
            pulErrorClks= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ulErrorClks[ENCGAPDISK_EDGE_FALL];
            pucEdgeError= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeError[ENCGAPDISK_EDGE_FALL];
            pucEdgeErrorCnt= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeErrorCnt[ENCGAPDISK_EDGE_FALL];
         }
         else
         {
            pSEdge= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SEdge.SEncEdge[ENCGAPDISK_EDGE_RAISE][SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_RAISE]];

            pusEdgeCnt= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].usEdgeCnt[ENCGAPDISK_EDGE_RAISE];
            pusEdgeOffsetCnt= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].usEdgeOffsetCnt[ENCGAPDISK_EDGE_RAISE];
            pucEdgeSeq= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_RAISE];
            pucEdgeSeqMin= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SEdge.ucEdgeSeqMin[ENCGAPDISK_EDGE_RAISE];
            pucEdgeSeqMax= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SEdge.ucEdgeSeqMax[ENCGAPDISK_EDGE_RAISE];
            
            pulEdgeClkCmp= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ulEdgeClksCmp[ENCGAPDISK_EDGE_RAISE][0];
            pucCompare= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucCompare[ENCGAPDISK_EDGE_RAISE];
            pucCompareDelay= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucCompareDelay[ENCGAPDISK_EDGE_RAISE];
            
            pucToleranceErrorCnt= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucToleranceErrorCnt[ENCGAPDISK_EDGE_RAISE];
            pucEdgeIgnoreCnt= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeIgnoreCnt[ENCGAPDISK_EDGE_RAISE];
            pulErrorClks= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ulErrorClks[ENCGAPDISK_EDGE_RAISE];
            pucEdgeError= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeError[ENCGAPDISK_EDGE_RAISE];
            pucEdgeErrorCnt= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeErrorCnt[ENCGAPDISK_EDGE_RAISE];
         }

         // Teach-in
         
         if (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucState != ENCGAPDISK_STATE_OFFSET)
         {         
            // Check for iteration of already received edge
            if (*pucCompare && !*pucCompareDelay)
            {
               for (ulIndex= 0; (ulIndex < *pucCompare) && (!*pucCompareDelay); ulIndex++)
               {
                  ulDiff= ABS(*(pulEdgeClkCmp+ ulIndex), ulClks);

#if ENCGAPDISK_OFFRANGEMODULOCHECK > 0
                  if (ulDiff >= SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax)
                  {
                     // Handle modulo based clks restarts
                     ulDiff2= ABS(ulDiff, ulModulo);
                     if (ulDiff2 <= SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax)
                     {
                        ulDiff= ulDiff2;
                        
                        if (ulClks < pSEdge->ulClks)
                        {
                           ulClks += ulModulo;
                        }
                        else
                        {
                           ulClks -= _ulmin(ulClks, ulModulo);
                        }
                     }
                     else
                     {
                        // Debug
                        ulDiff2= ulDiff2;
                     }
                  }
#endif

                  if (ulDiff <= SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax)
                  {
   						// Caveat: *pucCompareDelay is decremented at end of THIS cycle, so we need to increase it by one
                     *pucCompareDelay= (!SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].bReverse) ? ((ENCGAPDISK_COMPAREITEM_MAX)- ulIndex) : (ulIndex+ 1);                     
                     *pucEdgeSeqMin= _umin(*pucEdgeSeqMin, (ulIndex & BITS_UCHAR));
                     
                     // Debug
                     if (ulIndex > 0)
                     {
                        ulIndex= ulIndex;
                     }
                     
                     RAISEMAX(SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ulEdgeIterationCnt, 1, ULONG_MAX);
                     bSeqAdjust= TRUE;
                     
                     if (!(SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ulEdgeIterationCnt % 2))
                     {
                        RAISEMAX(SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ulCycleCnt, 1, ULONG_MAX);
                     }
                                   
                     ulDiff= (*pusEdgeCnt % SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.ucGapCnt);
                     if (ulDiff)
                     {
                        *pucEdgeError= _ulmin(ulDiff, UCHAR_MAX);
                        RAISEMAX(*pucEdgeErrorCnt, 1, UCHAR_MAX);
                     }
                  }
               }
            }
            
            if (bSeqAdjust)
            {
               if (!SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].bReverse)
               {
                  *pucEdgeSeq %= SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.ucGapCnt;
               }
               else
               {
                  *pucEdgeSeq= (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.ucGapCnt- 1);
                  *pucEdgeSeqMax= (*pucEdgeSeq)+ 1;
               }
               
               // Clear bSeqAdjust request
               bSeqAdjust= FALSE;
            
               // Re-Set pointers to current edge specified by bLevel
               if (bLevel == SEncGapDiskRO[pSEncGapDiskTsk->ucUnit].bGapLevel)
               {
                  pSEdge= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SEdge.SEncEdge[ENCGAPDISK_EDGE_FALL][SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_FALL]];
               }
               else
               {
                  pSEdge= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SEdge.SEncEdge[ENCGAPDISK_EDGE_RAISE][SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucEdgeSeq[ENCGAPDISK_EDGE_RAISE]];
               }
            }
   
            // Change from TeachIn to MeanIn after iteration is detected
            if (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucState == ENCGAPDISK_STATE_TEACHIN)
            {
               if (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ulCycleCnt >= 1)
               {
                  SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucState= ENCGAPDISK_STATE_MEANIN;
               }
            }
            // Change from MeanIn to Verify after iteration is specified round times detected
            else if (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucState == ENCGAPDISK_STATE_MEANIN)
            {
               if (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ulCycleCnt >= SEncGapDiskRO[pSEncGapDiskTsk->ucUnit].ucVerify)
               {               
                  SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucState= ENCGAPDISK_STATE_VERIFY;
                  
                  // Clear Error counters
                  *pulErrorClks= 0;
               }
            }

            // Assume current edge is invalid by default
            bValid= FALSE;
            
            if (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucState == ENCGAPDISK_STATE_TEACHIN)
            {
               // TeachIn - store received edge clks without further checking (primary filtering already done within isr)
               pSEdge->ulClks= ulClks;
               bValid= TRUE;
            }
            else
            {
               ulDiff= ABS(pSEdge->ulClks, ulClks);

#if ENCGAPDISK_OFFRANGEMODULOCHECK > 0
               if (ulDiff > SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax)
               {
                  // Handle modulo based clks restarts
                  ulDiff2= ABS(ulDiff, ulModulo);
                  if (ulDiff2 <= SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax)
                  {
                     ulDiff= ulDiff2;
                     
                     if (ulClks < pSEdge->ulClks)
                     {
                        ulClks += ulModulo;
                     }
                     else
                     {
                        ulClks -= _ulmin(ulClks, ulModulo);
                     }
                  }
                  else
                  {
                     // Debug
                     ulDiff2= ulDiff2;
                  }
               }
#endif

               if (ulDiff <= SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax)
               {
                  bValid= TRUE;
                  
                  if (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucState == ENCGAPDISK_STATE_MEANIN)
                  {
                     // MeanIn - update edge clks by meaning
                     pSEdge->ulClks= (pSEdge->ulClks+ ulClks) >> 1;   
                  }
                  else if (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucState == ENCGAPDISK_STATE_VERIFY)
                  {                           
                     // Increment error counter when out of tolerance
                     if (*pulErrorClks > SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usToleranceClks)
                     {
                        RAISEMAX(*pucToleranceErrorCnt, 1, UCHAR_MAX);
                     }
                     
                     // Debug
                     if (*pulErrorClks > 10000)
                     {
                        ulClks= ulClks;
                     }
                  }
               }
               else
               {
                  RAISEMAX(*pucEdgeIgnoreCnt, 1, UCHAR_MAX);
               }
               
               // Determine number of error steps
               *pulErrorClks= _ulmax(*pulErrorClks, ulDiff);
            }
                     
            if (bValid)
            {
               // Set compare clks
               if (!SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.bContinous)
               {
                  // For non-continous mode set compare to recent edge(s) received
                  if (*pucCompare > 1)
                  {
                     for (ulIndex= 0, ulIndex2= 1; ulIndex2 < *pucCompare; ulIndex++, ulIndex2++)
                     {
                        *(pulEdgeClkCmp+ ulIndex2)= *(pulEdgeClkCmp+ ulIndex);
                     }
                  }
                  
                  *pulEdgeClkCmp= pSEdge->ulClks;
               }
               else
               {
                  // For continous mode set compare to first edge(s) seen in first cycle
                  if ((*pucCompare < ENCGAPDISK_COMPAREITEM_MAX) || (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucJobIdPrev != SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucJobId))
                  {
                     SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucJobIdPrev= SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].ucJobId;
                     *(pulEdgeClkCmp+ *pucCompare)= pSEdge->ulClks;
                  }
               }
   
               // Compare clks set - enable compare for next edge received
               RAISEMAX(*pucCompare, 1, ENCGAPDISK_COMPAREITEM_MAX);
               
               // Update counters
               RAISEMAX(pSEdge->ulConfirm, 1, ULONG_MAX);
               RAISEMAX(*pusEdgeCnt, 1, USHRT_MAX);
               
               LOWERMIN(*pucCompareDelay, 1, 0);
            
               // Increment/decrement allocated/processed index        
               if (!SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].bReverse)
               {
                  RAISEMAX(*pucEdgeSeq, 1, (ENCGAPDISK_EDGEITEM_MAX- 1));
                  *pucEdgeSeqMax= _umax(*pucEdgeSeqMax, *pucEdgeSeq);
               }
               else
               {                
                  if (*pucEdgeSeq > 0)
                  {
                     (*pucEdgeSeq)--;
                  }
               }
            }
         }
         else
         {
            // Offset correction         

            if (*pucEdgeSeqMax && SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax)
            {
               // Seek to first edge
               if (bLevel == SEncGapDiskRO[pSEncGapDiskTsk->ucUnit].bGapLevel)
               {
                  pSEdge= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SEdge.SEncEdge[ENCGAPDISK_EDGE_FALL][0];
               }
               else
               {
                  pSEdge= &SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SEdge.SEncEdge[ENCGAPDISK_EDGE_RAISE][0];
               }
            
               // Seek thru all available edges
               for (ulMinIndex= *pucEdgeSeqMax, ulMinDiff= ULONG_MAX, ulMinDiffErr= ULONG_MAX, ulIndex= 0; ulIndex < *pucEdgeSeqMax; ulIndex++)
               {
                  ulDiff= ABS((pSEdge+ ulIndex)->ulClks, ulClks);
                  
                  // Determine lowest error clocks
                  if (ulDiff < ulMinDiffErr)
                  {
                     ulMinDiffErr= ulDiff;
                  }

#if ENCGAPDISK_OFFRANGEMODULOCHECK > 0
                  if (ulDiff > (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax* ENCGAPDISK_EDGEDRIFTMAXFACTOR))
                  {
                     // Handle modulo based clks restarts
                     ulDiff2= ABS(ulDiff, ulModulo);
                     if (ulDiff2 <= (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax* ENCGAPDISK_EDGEDRIFTMAXFACTOR))
                     {
                        ulDiff= ulDiff2;
                        
                        if (ulClks < (pSEdge+ ulIndex)->ulClks)
                        {
                           ulClks += ulModulo;
                        }
                        else
                        {
                           ulClks -= _ulmin(ulClks, ulModulo);
                        }
                     }
                     else
                     {
                        // Debug
                        ulDiff2= ulDiff2;
                     }
                  }
#endif

                  if (ulDiff <= (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax* ENCGAPDISK_EDGEDRIFTMAXFACTOR))
                  {
                     // Find best fitting edge data
                     if (ulDiff < ulMinDiff)
                     {
                        ulMinDiff= ulDiff;
                        ulMinIndex= ulIndex;
                     }
                  }
                  else
                  {
                     // Debug
                     ulDiff= ulDiff;
                  }
               }
               
               // Offset correction - push best fitting edge's valid offset into queue for processing after movement
               if (ulMinIndex < *pucEdgeSeqMax)
               {
                  if (ulMinDiff <= (SEncGapDiskRW[pSEncGapDiskTsk->ucUnit].SPermanent.SVar.usEdgeDriftMax* ENCGAPDISK_EDGEDRIFTMAXFACTOR))
                  {
                     // Compose a 31bit signed offset value
                     lValue= (ulMinDiff & ~0x80000000);
                     
#if ENCGAPDISK_OFFSETBYDIRECTION > 0
                     lValue= ((pSEdge+ ulMinIndex)->ulClks >= ulClks) ? (-lValue) : (lValue);
#else
                     lValue= ((pSEdge+ ulMinIndex)->ulClks < ulClks) ? (-lValue) : (lValue);
#endif

                     StpClkDirOffsetPush(EncGapDiskStpClkDirUnitGet(pSEncGapDiskTsk->ucUnit), lValue, SEncGapDiskRO[pSEncGapDiskTsk->ucUnit].u16Timeout);
                     
                     // Increment edge offset count for enhanced movement checking
                     RAISEMAX(*pusEdgeOffsetCnt, 1, USHRT_MAX);
                     
                     // Determine number of error clocks
                     *pulErrorClks= _ulmax(*pulErrorClks, ulMinDiff);
                  }
                  else
                  {
                     // Debug
                     ulMinDiff= ulMinDiff;
                  }                  
               }
               else
               {
                  // Increment ignore count
                  RAISEMAX(*pucEdgeIgnoreCnt, 1, UCHAR_MAX);

                  // Determine number of error clocks
                  *pulErrorClks= _ulmax(*pulErrorClks, ulMinDiffErr);
               }
            }
         }
      }
   }

   DETACH;
}

//----------------------------------------------------------------------------
// UCHAR EncGapDiskIrqCtrl
//----------------------------------------------------------------------------
UCHAR EncGapDiskIrqCtrl(UCHAR ucUnit, bool bEnable)
{
   UCHAR ucResult= 0;
   
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      // Control signal
      if ((SEncGapDiskRW[ucUnit].SIrq.pPio) && (SEncGapDiskRW[ucUnit].SIrq.u32Pin))
      {
         // Control hardware interrupt mask
         if (bEnable)
         {
            AT91F_PIO_InterruptEnable(SEncGapDiskRW[ucUnit].SIrq.pPio, SEncGapDiskRW[ucUnit].SIrq.u32Pin);
         }
         else
         {
            AT91F_PIO_InterruptDisable(SEncGapDiskRW[ucUnit].SIrq.pPio, SEncGapDiskRW[ucUnit].SIrq.u32Pin);
         }
         
         ucResult++;
      }
      
      // Control gate
      if ((SEncGapDiskRW[ucUnit].SGate.pPio) && (SEncGapDiskRW[ucUnit].SGate.u32Pin))
      {
         // Control hardware interrupt mask
         if (bEnable)
         {
            AT91F_PIO_InterruptEnable(SEncGapDiskRW[ucUnit].SGate.pPio, SEncGapDiskRW[ucUnit].SGate.u32Pin);
         }
         else
         {
            AT91F_PIO_InterruptDisable(SEncGapDiskRW[ucUnit].SGate.pPio, SEncGapDiskRW[ucUnit].SGate.u32Pin);
         }
         
         ucResult++;
      }
      
      // Update software interrupt flag
      SEncGapDiskRW[ucUnit].bIrq= bEnable;
      
      ucResult++;
   }
      
   return (ucResult);
}

//----------------------------------------------------------------------------
// bool InitEncGapDiskHandler
//----------------------------------------------------------------------------
bool InitEncGapDiskHandler(UCHAR ucUnit, U32 u32Pin, AT91PS_PIO pPio)
{
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      // Set interrupt pin data once for valid params only
      if ((u32Pin) && (pPio) && !(SEncGapDiskRW[ucUnit].SIrq.u32Pin) && !(SEncGapDiskRW[ucUnit].SIrq.pPio))
      {
         SEncGapDiskRW[ucUnit].SIrq.u32Pin= u32Pin;
         SEncGapDiskRW[ucUnit].SIrq.pPio= pPio;
      }
   }
   
   // Disable interrupt yet, it's enabled later manually
   return (FALSE);
}

//----------------------------------------------------------------------------
// bool InitEncGapDiskGateHandler
//----------------------------------------------------------------------------
bool InitEncGapDiskGateHandler(UCHAR ucUnit, U32 u32Pin, AT91PS_PIO pPio)
{
   if (ucUnit < ENCGAPDISK_UNIT_MAX)
   {
      // Set interrupt pin data once for valid params only
      if ((u32Pin) && (pPio) && !(SEncGapDiskRW[ucUnit].SGate.u32Pin) && !(SEncGapDiskRW[ucUnit].SGate.pPio))
      {
         SEncGapDiskRW[ucUnit].SGate.u32Pin= u32Pin;
         SEncGapDiskRW[ucUnit].SGate.pPio= pPio;
      }
   }
   
   // Disable interrupt yet, it's enabled later manually
   return (FALSE);
}

//----------------------------------------------------------------------------
// void IsrEncGapDiskIrqHandler
//----------------------------------------------------------------------------
void IsrEncGapDiskIrqHandler(UCHAR ucUnit, U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio)
{
   static ULONG ulClksPrev= 0;
   //-static bool bLevelPrev= FALSE;
   static bool bReversePrev= FALSE;
   static bool bPrev= FALSE;
   
   register ULONG ulClks;
   register ULONG ulDiff;
   register ULONG ulLevelHiCnt;
   register ULONG ulLevelLoCnt;
   register bool bGate;
   register bool bNew;
   
#if ENCGAPDISK_REREADPIN_COUNT > 0
   register ULONG ulIndex;
#endif   
   
   ULONG ulValue;
   
   // Catch stepper position and bLevel and pack them into mbx (bit 32= bLevel, 31...0= Clks)
   
   if (SEncGapDiskRW[ucUnit].bIrq)
   {
      // Determine level of gate pin defined by pio init handler callback
      if ((SEncGapDiskRW[ucUnit].SGate.pPio) && (SEncGapDiskRW[ucUnit].SGate.u32Pin))
      {
         bGate= ((SEncGapDiskRW[ucUnit].SGate.pPio->PIO_PDSR & SEncGapDiskRW[ucUnit].SGate.u32Pin) != 0);
      }
      // Determine level of gate pin defined by descriptor setting
      else if ((SEncGapDiskRO[ucUnit].SGate.pPio) && (SEncGapDiskRO[ucUnit].SGate.u32Pin))
      {
         bGate= ((SEncGapDiskRO[ucUnit].SGate.pPio->PIO_PDSR & SEncGapDiskRO[ucUnit].SGate.u32Pin) != 0);
      }
      // Self gated mode
      else
      {
         bGate= TRUE;
      }

      if (bGate)
      {
         // StpClkDirClksAbsGet() is ISR safe
         ulClks= (StpClkDirClksAbsGet(SEncGapDiskRO[ucUnit].ucStpClkDirUnit) & ~0x80000000);
         ulDiff= ABS(ulClks, ulClksPrev);

         ulLevelHiCnt= (bLevel != 0);
         ulLevelLoCnt= (bLevel == 0);

#if ENCGAPDISK_REREADPIN_COUNT > 0
         // Re-read level of pin
         for (ulIndex= 0; ulIndex < ENCGAPDISK_REREADPIN_COUNT; ulIndex++)
         {
            if ((pPio->PIO_PDSR & u32Pin) != 0)
            {
               ulLevelHiCnt++;
            }
            else
            {
               ulLevelLoCnt++;
            }
         }
#endif
   
         // Process only definite levels
         if (ulLevelHiCnt != ulLevelLoCnt)
         {
#if ENCGAPDISK_REREADPIN_COUNT > 0
            if ((ulLevelHiCnt >= ENCGAPDISK_REREADPIN_COUNT) || (ulLevelLoCnt >= ENCGAPDISK_REREADPIN_COUNT))
#endif
            {
               if (ulLevelLoCnt >= ulLevelHiCnt)
               {
                  bLevel= 0;
               }
               else if (ulLevelHiCnt >= ulLevelLoCnt)
               {
                  bLevel= 1;
               }
   
#ifndef ENCGAPDISK_DEBUG
               if (isr_mbx_check(&MbxEncGapDiskMsg[ucUnit]) > 0) // DEBUG= comment this line
#endif
               {                               
                  bNew= (!bPrev || ((ulDiff >= SEncGapDiskRW[ucUnit].SPermanent.SVar.ucThreshold) /* && (bLevel != bLevelPrev)*/) || (SEncGapDiskRW[ucUnit].bReverse != bReversePrev));
                  if (bNew)
                  {                  
                     ulValue= ulClks | ((ULONG)bLevel << 31);
#ifndef ENCGAPDISK_DEBUG
                     isr_mbx_send(&MbxEncGapDiskMsg[ucUnit], OS_MBX_VALUE_PUT(ulValue)); // DEBUG= isr_mbx_write, STD= isr_mbx_send
#else
                     isr_mbx_write(&MbxEncGapDiskMsg[ucUnit], OS_MBX_VALUE_PUT(ulValue)); // DEBUG= isr_mbx_write, STD= isr_mbx_send
#endif
                     
                     ulClksPrev= ulClks;
                     //-bLevelPrev= bLevel;
                     bReversePrev= SEncGapDiskRW[ucUnit].bReverse;
                     bPrev= TRUE;            
                  }
               }
#ifndef ENCGAPDISK_DEBUG
               else
               {
                  // Debug
                  bLevel= bLevel;
               }
#endif
            }
         }
      }
   }
}

//----------------------------------------------------------------------------
// void IsrEncGapDiskGateIrqHandler
//----------------------------------------------------------------------------
void IsrEncGapDiskGateIrqHandler(UCHAR ucUnit, U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio)
{
   // Disable interrupts on falling edge of the high active gate (already by pio pin trigger option AT91C_PIOTRG_FALL)
   if (!bLevel)
   {
      EncGapDiskIrqCtrl(ucUnit, FALSE);
   }
   else
   {
      // Debug
      bLevel= bLevel;
   }
}

