//!	external data definitions of ENCGAPDISK driver
/**
		\file		encgapdiskdataext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/MISC/ENCGAPDISK/ENCGAPDISKDATAEXT.H
// Purpose: PERIPHERAL SUPPORT
// Author:  rradtke, hschoettner
//----------------------------------------------------------------------------
// 2010-09-23 initial version
// 2010-11-18 hs: imported ...EncEdgeSimple & ...CalibrationData from ...data.h
// 2010-11-18 hs: removed ulClkPosZeroOffset from _SEncGapDiskCalibrationData
// 2010-11-23 hs: added/fixed data comments for _SEncGapDiskCalibrationData
// 2011-11-02 hs: added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISKDATAEXT_H
#define SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISKDATAEXT_H


//! Reduced edge data structure <b>[CAVEAT!!! ANONYMOUSLY byte by byte access ONLY!!!]</b>
struct _SEncGapDiskEncEdgeSimple
{
   ULONG                            ulClks;                                                              //!< Position in clocks
};


// Alignment set manually to avoid any padding in structure (to reduce permanent storage size)

//! Reduced calibration data structure <b>[CAVEAT!!! ANONYMOUSLY byte by byte access ONLY!!!]</b>
struct _SEncGapDiskCalibrationData
{
   struct _SEncGapDiskEncEdgeSimple    SEncEdge[ENCGAPDISK_EDGETYPE_MAX][ENCGAPDISK_EDGEITEM_MAX];       //!< Edge data structure array <b>(ulConfirm is ommited)</b>
   UCHAR                               ucEdgeSeqMin[ENCGAPDISK_EDGETYPE_MAX];                            //!< Edge minimum index (zero if no additional false edges were detected)
   UCHAR                               ucEdgeSeqMax[ENCGAPDISK_EDGETYPE_MAX];                            //!< Edge maximum index (equal to true gap number if no additional edges were detected)
   UCHAR                               ucEdgeZero[ENCGAPDISK_EDGETYPE_MAX];                              //!< User home edge index (offset value)
   ULONG                               ulConfirm[ENCGAPDISK_EDGETYPE_MAX];                               //!< Unified (global) confirm count for each edge type
   USHORT                              usEdgeDriftMax;                                                   //!< Maximum edge drift clock count allowed for scan/meaning state
};


// Three of the additional variables HERE are runtime copies of original variables from StpClkDir:
// 1) usDacValueHold    (application: u16_DAC_DataIn_HoldMotorDriver)
// 2) usDacValueRun     (application: u16_DAC_DataIn_RunMotorDriver)
// 3) u32ClkCntAbsolute (application: Motor_UStepsSinceStartByteLow/Motor_UStepsSinceStartByteHigh)
// Alignment set manually to avoid any padding in structure (to reduce permanent storage size)

//! Runtime parameters for application <b>[DIRECT item by item access ALLOWED]</b>
struct _SEncGapDiskRunParams
{
   ULONG    ulClkInitHz;                //!< Clock frequency [Hz] for initial (NON-ACCELERATING) pre-movement (with edge/gap irq locked) with ulClksInit clocks before execution of full movement
   ULONG    ulClksInit;                 //!< Clock count for initial (NON-ACCELERATING) pre-movement (with edge/gap irq locked) before execution of full movement
   ULONG    ulClkLowHz;                 //!< Low (start and end) clocks frequency [Hz] of motor
   ULONG    ulClkHighHz;                //!< Run clock frequency [Hz] of motor to accellerate to (and to brake from)
   ULONG    ulClks;                     //!< Clock count for a full turn
   USHORT   usClksAcc;                  //!< Clock count for acceleration ramp
   USHORT   usClksBrk;                  //!< Clock count for breaking ramp
   USHORT   usDacValueHold;             //!< DAC data value for motor driver's hold state
   USHORT   usDacValueRun;              //!< DAC data value forn motor driver's run state
   ULONG    u32ClkCntAbsolute;          //!< Absolute motor position in clocks <b>(is set to ZERO before each new calibration)</b>
   bool     bContinous;                 //!< #TRUE == calibration by multiple continuous turns <b>(without mechanical hard stop)</b>, #FALSE == calibration by ping-pong moves <b>(WITH mechanical hard stop)</b>
};

#endif
