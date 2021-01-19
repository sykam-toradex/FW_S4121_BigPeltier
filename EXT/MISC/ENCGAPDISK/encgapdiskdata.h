//!	data definitions of ENCGAPDISK driver
/**
		\file		encgapdiskdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/MISC/ENCGAPDISK/ENCGAPDISKDATA.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-03-03 initial version
// 2010-03-04 added non-continous reverse processing feature
// 2010-03-04 removed ulClksStableHz
// 2010-03-10 added ucRetry to _SEncGapDiskRO
// 2010-03-17 added ucThreshold to _SEncGapDiskRO
// 2010-03-18 renamed bLevelGap to bGapLevel in _SEncGapDiskRO
// 2010-03-24 changed edge storage from SEncGap to SEncGapFall/Raise
// 2010-03-24 changed edge names from start/stop to fall/raise
// 2010-03-25 renamed ucGapCnt to ucGapNumber in _SEncGapDiskRO
// 2010-03-25 added usGapDriftMax to _SEncGapDiskRO
// 2010-03-25 added ucErrorGaps, ucGapIgnoreCnt, ucGapNumber to _SEncGapDiskRW
// 2010-03-26 added JobId, removed ucIteration to/from _SEncGapDiskTsk
// 2010-03-26 added usEdgeIterationCnt, ucCycleCnt to _SEncGapDiskRW
// 2010-03-30 renamed item ucGapErrorCnt to ucEdgeErrorCnt
// 2010-03-30 renamed item ucGapIgnoreCnt to ucEdgeIgnoreCnt
// 2010-03-30 renamed item ucGapFallCnt to ucEdgeFallCnt
// 2010-03-30 renamed item ucGapRaiseCnt to ucEdgeRaiseCnt
// 2010-03-30 renamed item ucGapRaiseSeqMax to ucEdgeRaiseSeqMax
// 2010-03-30 renamed item ucGapFallSeqMax to ucEdgeFallSeqMax
// 2010-03-30 renamed item ucGapRaiseSeq to ucEdgeRaiseSeq
// 2010-03-30 renamed item ucGapFallSeq to ucEdgeFallSeq
// 2010-03-30 renamed item SEncGapFall to SEncEdgeFall
// 2010-03-30 renamed item SEncGapRaise to SEncEdgeRaise
// 2010-03-30 renamed item usGapDriftMax to usEdgeDriftMax
// 2010-03-30 renamed item ulGapFallClksCmp to ulEdgeFallClksCmp
// 2010-03-30 renamed item ulGapRaiseClksCmp to ulEdgeRaiseClksCmp
// 2010-03-31 added bCompare and bSeqAdjust to _SEncGapDiskTsk
// 2010-03-31 moved all but not ucUnit from _SEncGapDiskTsk to _SEncGapDiskRW
// 2010-03-31 changed storage from fall/rise to two dim array of edges
// 2010-04-01 added ulClkInitHz to _SEncGapDiskRO
// 2010-05-07 changed bCompare of _SEncGapDiskRW to array type
// 2010-05-12 moved bContinous from ...RO to ...RW
// 2010-05-12 moved ulClkLowHz, ulClkHighHzfrom & ulClks from ...RO to ...RW
// 2010-05-12 moved usClksAcc & usClksBrk from ...RO to ...RW
// 2010-05-12 moved usToleranceSteps &  ucThreshold from ...RO to ...RW
// 2010-05-12 moved usEdgeDriftMax from ...RO to ...RW
// 2010-05-12 added 2nd dim to ulEdgeClksCmp for alternate cycle detection
// 2010-05-12 added 2nd dim to ulErrorSteps for edge independent stats
// 2010-05-12 added 2nd dim to ucToleranceErrorCnt for edge independent stats
// 2010-05-12 added 2nd dim to ucEdgeIgnoreCnt for edge independent stats
// 2010-05-13 changed ucEdgeCnt to usEdgeCnt (UCHAR to USHORT)
// 2010-05-13 added ucCompareDelay[] to avoid multiple cycle detection
// 2010-05-13 added ucEdgeSeqMin[] to store index of alternate cycle detection
// 2010-05-20 added and updated comments to all data elements
// 2010-05-21 added ucEdgeSeqMod & ucEdgeSeqOffset to _SEncGapDiskRW
// 2010-07-02 added ucEdgeCurr, bEdgeCurrValid & ucEdgeZero to _SEncGapDiskRW
// 2010-07-02 removed ucEdgeSeqMod and ucEdgeSeqOffset
// 2010-07-07 renamed ucGapErrorCnt to ucGapErrorCnt
// 2010-07-07 renamed ucErrorGaps to ucGapError
// 2010-07-07 changed ucEdgeZero to ucEdgeZero[]
// 2010-07-07 removed ulIdent of _SEncEdge
// 2010-07-07 moved ucEdgeCurr & bEdgeCurrValid to _SEdgeCurr (ucItem, bValid)
// 2010-07-07 renamed ucGapNumber to ucGapCnt
// 2010-07-08 renamed ulErrorSteps[] to ulErrorClks[]
// 2010-07-08 renamed usToleranceSteps to usToleranceClks
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-16 renamed struct _SEncEdge to _SEncGapDiskEncEdge
// 2010-09-16 renamed struct _SEdgeCurr to _SEncGapDiskEdgeCurr
// 2010-09-16 added structs _SEncGapDiskStpClkDir, _S...Var & _S...Permanent
// 2010-09-16 reorganized data structures (added permanent tree to rw data)
// 2010-09-29 rr: added _SEncGapDiskCalibrationData
// 2010-11-18 changed for having all edge types permanent
// 2010-11-18 moved usEdgeDriftMax from _SEncGapDiskRW to _SEncGapDiskVar
// 2010-11-18 moved ...EncEdgeSimple & ...CalibrationData to ...dataext.h
// 2010-11-23 added/fixed data comments
// 2011-11-02 added public doxygen documentation
// 2011-11-24 added internal doxygen documentation
// 2013-08-22 changed timeout from milliseconds to ticks
// 2016-03-02 added struct _SEncGapDiskIrq
// 2016-03-02 added struct _SEncGapDiskIrq to struct _SEncGapDiskRW
// 2016-06-08 added u32GateMask to struct _SEncGapDiskRO
// 2016-06-08 added SGate to struct _SEncGapDiskRW
// 2016-06-09 replaced u32GateMask with SGate in struct _SEncGapDiskRO
// 2016-06-09 added support for different port group signal gate source to _SEncGapDiskRO
// 2016-06-23 added usEdgeOffsetCnt[] to _SEncGapDiskRW
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISKDATA_H
#define SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISKDATA_H


//! \internal \brief <b>Internal data:</b> Task parameter data
struct _SEncGapDiskTsk
{
   UCHAR                ucUnit;                                                                          //!< Unit task is assigned to
};

//! <b>Internal data:</b> Interrupt data of port pin connected to
struct _SEncGapDiskIrq
{
   U32                              u32Pin;                                                              //!< Pio port pin mask
   AT91PS_PIO                       pPio;                                                                //!< Pio base address
};

//! Hardware descriptor data (read only) for each unit
struct _SEncGapDiskRO
{
   UCHAR                            ucStpClkDirUnit;                                                     //!< Unit of stpclkdir
   bool                             bStpClkDirDir;                                                       //!< Initial direction (refer to stpclkdir.h #STPCLKDIR_CLOCKWISE and #STPCLKDIR_COUNTERCLOCKWISE)
   bool                             bGapLevel;                                                           //!< Logic level of (inside) gap (0 == low and 1 == high)
   UCHAR                            ucScan;                                                              //!< Number of scan/meaning cycles (turns) of teach-in
   UCHAR                            ucRetry;                                                             //!< Number of retry cycles (turns) of teach-in
   UCHAR                            ucVerify;                                                            //!< Number of verify cycles (turns) of teach-in
   UCHAR                            ucTskPriMsg;                                                         //!< Priority of message processing task
   struct _SEncGapDiskIrq           SGate;                                                               //!< Mask and port of gate pin for signal (optional, set to NULL to disable) for separate pins of level/interrupt and gate
   U16                              u16Timeout;                                                          //!< Maximum cpu-load free resource pending time before any resource claim is canceled by the rtos <b>(in ticks)</b>
};

//! <b>Internal data:</b> Last position cache
struct _SEncGapDiskEdgeCurr
{
   UCHAR                            ucItem;                                                              //!< Last set edge position
   bool                             bValid;                                                              //!< Last set edge position valid flag
};

//--------------------------------- EncEdge ----------------------------------

//! <b>Internal data:</b> Basic edge data
struct _SEncGapDiskEncEdge
{
   ULONG                            ulClks;                                                              //!< Position in clocks
   ULONG                            ulConfirm;                                                           //!< Confirm count
};

//---------------------------------------------------------------------------- 

//--------------------------------- Edge -------------------------------------
//! <b>Internal data:</b> Complete edge data
struct _SEncGapDiskEdge
{
   struct _SEncGapDiskEncEdge       SEncEdge[ENCGAPDISK_EDGETYPE_MAX][ENCGAPDISK_EDGEITEM_MAX];          //!< Basic edge data structure array
   UCHAR                            ucEdgeSeqMin[ENCGAPDISK_EDGETYPE_MAX];                               //!< Edge minimum index array
   UCHAR                            ucEdgeSeqMax[ENCGAPDISK_EDGETYPE_MAX];                               //!< Edge maximum index array
   UCHAR                            ucEdgeZero[ENCGAPDISK_EDGETYPE_MAX];                                 //!< Real edge data array index for user zero index (#EncGapDiskEdgeZeroSet, #EncGapDiskEdgeGoto)
   
   ULONG                            ulConfirm[ENCGAPDISK_EDGETYPE_MAX];                                  //!< Unified confirm count for each edge type
};

/*
struct _SEncGapDiskEdgeSimple
{
   struct _SEncGapDiskEncEdgeSimple SEncEdge[1][ENCGAPDISK_EDGEITEM_MAX];                                // Basic edge data structure array
   UCHAR                            ucEdgeSeqMin[1];                                                     // Edge minimum index array
   UCHAR                            ucEdgeSeqMax[1];                                                     // Edge maximum index array
   UCHAR                            ucEdgeZero[1];                                                       // Real edge data array index for user zero index (EncGapDiskEdgeGoto(ZeroSet))
   
   UCHAR                            ucEdgeType;                                                          // Type of edge stored in structure
   ULONG                            ulConfirm[1]                                                         // Unified confirm count
};
*/

//----------------------------------------------------------------------------

//! <b>Internal data:</b> Arguments for StpClkDir
struct _SEncGapDiskStpClkDirArgs
{
   ULONG                            ulClkInitHz;                                                         //!< Clock frequeny [Hz] for initial (NON-ACCELERATING) pre-movement (with edge/gap irq locked)
   ULONG                            ulClksInit;                                                          //!< Clock count for initial (NON-ACCELERATING) pre-movement (with edge/gap irq locked)
   ULONG                            ulClkLowHz;                                                          //!< Clock frequency [Hz] of start and end (low) of movement
   ULONG                            ulClkHighHz;                                                         //!< Run (high) clocks frequency [Hz] movement is accellerating to / braking from
   ULONG                            ulClks;                                                              //!< Lenght of the complete movement in clocks (for min clocks refer to stpclkdir.h)
   USHORT                           usClksAcc;                                                           //!< Lenght of the acceleration part of movement in clocks (for min clocks refer to stpclkdir.h)
   USHORT                           usClksBrk;                                                           //!< Lenght of the brake part of movement in clocks (for min clocks refer to stpclkdir.h)
};

//! <b>Internal data:</b> Params of EncGapDisk
struct _SEncGapDiskVar
{
   USHORT                           usEdgeDriftMax;                                                      //!< Maximum edge drift clock count allowed for scan/meaning/offset state
   USHORT                           usToleranceClks;                                                     //!< Clock count allowed for drifting in verify state
   UCHAR                            ucThreshold;                                                         //!< Clock count delta required for new position detection
   UCHAR                            ucGapCnt;                                                            //!< Count of gaps exspected (for result validation only)
   bool                             bContinous;                                                          //!< Flag to select 'ping-pong' (FALSE) or continous mechanics (TRUE)
};

//! <b>Internal data:</b> Permanent data (read/write) for each unit
struct _SEncGapDiskPermanent
{
   struct _SEncGapDiskEdge          SEdge;                                                               //!< Complete edge data structure array
   struct _SEncGapDiskStpClkDirArgs SStpClkDirArgs;                                                      //!< All required motor parameters
   struct _SEncGapDiskVar           SVar;                                                                //!< All permanent vars required to skip teach-in
};

//! <b>Internal data:</b> Snapshot data
struct _SEncGapDiskSnapshot
{
   struct _SEncGapDiskEdge          SEdge;                                                               //!< Complete edge data structure array
   struct _SEncGapDiskStpClkDirArgs SStpClkDirArgs;                                                      //!< All required motor parameters
   struct _SEncGapDiskVar           SVar;                                                                //!< All permanent vars required to skip teach-in
};

//! <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SEncGapDiskRW
{
   struct _SEncGapDiskIrq           SIrq;                                                                //!< Interrupt data of port pin connected to
   struct _SEncGapDiskIrq           SGate;                                                               //!< Interrupt data of port pin gate connected to
   struct _SEncGapDiskPermanent     SPermanent;                                                          //!< All permanent vars required to skip teach-in
   struct _SEncGapDiskEdgeCurr      SEdgeCurr[ENCGAPDISK_EDGETYPE_MAX];                                  //!< Last set edge position storage
   ULONG                            ulEdgeClksCmp[ENCGAPDISK_EDGETYPE_MAX][ENCGAPDISK_COMPAREITEM_MAX];  //!< Edge compare clock position array for cycle repeat detection
   OS_TID                           tidTskMsg;                                                           //!< Task id of message processing task
   OS_SEM                           SemStart;                                                            //!< Semaphore of module (available= !started)
   ULONG                            ulErrorClks[ENCGAPDISK_EDGETYPE_MAX];                                //!< Clocks position delta (also for non-errors)
   UCHAR                            ucToleranceErrorCnt[ENCGAPDISK_EDGETYPE_MAX];                        //!< Count of edges off-tolerance (usToleranceClks)
   UCHAR                            ucEdgeIgnoreCnt[ENCGAPDISK_EDGETYPE_MAX];                            //!< Count of edges off-drift (usEdgeDriftMax)
   UCHAR                            ucEdgeError[ENCGAPDISK_EDGETYPE_MAX];                                //!< Edge number mismatch delta
   UCHAR                            ucEdgeErrorCnt[ENCGAPDISK_EDGETYPE_MAX];                             //!< Number of gap number mismatches
   USHORT                           usEdgeCnt[ENCGAPDISK_EDGETYPE_MAX];                                  //!< Detected edge count
   USHORT                           usEdgeOffsetCnt[ENCGAPDISK_EDGETYPE_MAX];                            //!< Edge offset count during move (after teach-in)
   UCHAR                            ucEdgeSeq[ENCGAPDISK_EDGETYPE_MAX];                                  //!< Detected edge index
   ULONG                            ulCycleCnt;                                                          //!< Teach-in cycle count '-1'
   ULONG                            ulEdgeIterationCnt;                                                  //!< Teach-in repeat cycle detection count
   UCHAR                            ucState;                                                             //!< Teach-in state value
   UCHAR                            ucJobId;                                                             //!< Teach-in current job id
   UCHAR                            ucJobIdPrev;                                                         //!< Teach-in previous job id
   UCHAR                            ucCompare[ENCGAPDISK_EDGETYPE_MAX];                                  //!< Cycle repeat detection enabled flag array
   UCHAR                            ucCompareDelay[ENCGAPDISK_EDGETYPE_MAX];                             //!< Cycle repeat detection delay count array
   bool                             bReverse;                                                            //!< Teach-in reverse flag (for 'ping-pong' mechanics)
   bool                             bIrq;                                                                //!< Irq enable flag
};

// Number of units located inside encgapdisk.h
struct _SEncGapDiskTsk              SEncGapDiskTsk[ENCGAPDISK_UNIT_MAX];
struct _SEncGapDiskRW               SEncGapDiskRW[ENCGAPDISK_UNIT_MAX];
struct _SEncGapDiskSnapshot         SEncGapDiskSnapShot[ENCGAPDISK_UNIT_MAX];
struct _SEncGapDiskCalibrationData  SEncGapDiskCalibrationData[ENCGAPDISK_UNIT_MAX];

#endif
