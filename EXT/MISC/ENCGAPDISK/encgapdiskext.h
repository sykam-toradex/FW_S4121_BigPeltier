//!	external interface of ENCGAPDISK driver
/**
		\file		encgapdiskext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/MISC/ENCGAPDISK/ENCGAPDISKEXT.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-03-03 initial version
// 2010-03-10 added bMean option to EncGapDiskGapCnt
// 2010-03-10 added EncGapDiskErrGet
// 2010-03-25 added ucGapNumber to EncGapDiskTeachIn
// 2010-03-31 added EncGapDiskClear
// 2010-03-31 renamed EncGapDiskGapGet to EncGapDiskEdgeGet
// 2010-03-31 renamed/changed EncGapDiskGapGoto to EncGapDiskEdgeGoto
// 2010-03-31 renamed/changed EncGapDiskGapCnt to EncGapDiskEdgeCnt
// 2010-03-31 renamed/changed EncGapDiskGapGet to EncGapDiskEdgeGet
// 2010-03-31 renamed/changed EncGapDiskErrGet to EncGapDiskEdgeErrGet
// 2010-03-31 added EncGapDiskCycleGet
// 2010-05-12 added bContinous to EncGapDiskTeachIn
// 2010-05-12 added ulClkLowHz, ulClkHighHz to EncGapDiskTeachIn
// 2010-05-12 added usClksAcc & usClksBrk to EncGapDiskTeachIn
// 2010-05-12 added usToleranceSteps &  ucThreshold to EncGapDiskTeachIn
// 2010-05-12 added usEdgeDriftMax to EncGapDiskTeachIn
// 2010-05-13 added ucEdgeType to EncGapDiskEdgeErrGet
// 2010-05-13 added pusEdgeCntTotal to EncGapDiskEdgeCnt
// 2010-05-19 added EncGapDiskEdgeHomeGet
// 2010-05-20 added EncGapDiskEdgeGotoZeroSet
// 2010-07-02 renamed EncGapDiskEdgeGotoZeroSet to EncGapDiskEdgeZeroSet
// 2010-07-02 renamed EncGapDiskEdgeGet to EncGapDiskEdgeDataGet
// 2010-07-02 added EncGapDiskEdgeGet
// 2010-07-02 added ZeroOffset param to EncGapDiskEdgeZeroSet
// 2010-07-02 removed EncGapDiskEdgeZeroSet
// 2010-07-03 re-added EncGapDiskEdgeZeroSet
// 2010-07-07 changed EncGapDiskEdgeZeroSet for edge type independency
// 2010-07-07 merged EncGapDiskGapErrGet and EncGapDiskEdgeErrGet
// 2010-07-07 removed EncGapDiskGapErrGet
// 2010-07-07 removed ulIdent in EncGapDiskEdgeDataGet
// 2010-07-07 renamed ucGapNumber to ucGapCnt
// 2010-07-08 renamed ulErrorSteps[] to ulErrorClks[]
// 2010-07-08 renamed usToleranceSteps to usToleranceClks
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-15 added prototype for _EncGapDiskEdgeCnt
// 2010-09-16 moved internal/private prototypes to encgapdiskint.h
// 2010-09-16 changed prototype of EncGapDiskTeachIn (added init move params)
// 2010-09-16 added prototype for EncGapDiskStpClkDirUnitGet
// 2010-09-16 added prototypes for EncGapDiskSnapShot, -SizeGet & -AddrGet
// 2010-09-29 rr: added new function EncGapDiskCalibrationDataSizeGet/AddrGet
// 2010-09-29 rr: added new function EncGapDiskCalibrationDataRW
// 2010-09-29 rr: added new function EncGapDiskRunParamsRW
// 2010-11-03 added par usMicroSteps to EncGapDiskTeachIn & EncGapDiskEdgeGoto
// 2010-11-18 removed para ucEdgeType from EncGapDiskCalibrationDataRW
// 2010-11-18 removed para ucEdgeType from EncGapDiskSnapShot
// 2010-12-08 added bClear and changed other param for EncGapDiskEdgeErrGet
// 2010-12-08 renamed EncGapDiskCycleGet to EncGapDiskStatGet plus new param
// 2011-11-02 moved EncGapDiskClear to encgapdiskint.h
// 2011-11-02 added public doxygen documentation
// 2016-06-23 added new function EncGapDiskOffsetGet
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISKEXT_H
#define SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISKEXT_H


// Prototypes

//!   init a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)

      \return										> 0 for success, == 0 for failure

      \note
               - 1 task is spanned by this function

*/
UCHAR EncGapDiskInit(UCHAR ucUnit);

//!   teach-in the gap disk of the encoder
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param	ucEdgeType					   type of edge to check detection count after teach-in turn(s) (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   bContinous                 #TRUE == multiple continuous turns <b>(without mechanical hard stop)</b>, #FALSE == ping-pong moves <b>(WITH mechanical hard stop)</b>
      \param	ucGapCnt				         count of gaps of the encoder's gap disk for success verification
      \param	ulClks						   lenght of the complete movement in clocks (for min clocks refer to stpclkdir.h)
      \param   usClksAcc                  lenght of the acceleration part of movement in clocks (for min clocks refer to stpclkdir.h)
      \param   usClksBrk                  lenght of the brake part of movement in clocks (for min clocks refer to stpclkdir.h)
      \param   ulClkLowHz                 clock frequency [Hz] of start and end (low) of movement
      \param   ulClkHighHz                run (high) clocks frequency [Hz] movement is accellerating to / braking from
      \param   ulClksInit                 clock frequeny [Hz] for initial (NON-ACCELLERATING) pre-movement (with edge/gap irq locked)
      \param   ulClkInitHz                clock count for initial (NON-ACCELLERATING) pre-movement (with edge/gap irq locked)
      \param   ucThreshold                clock count delta of edges required for new position detection
      \param   usToleranceClks            clock count allowed for drifting of edges in verify state
      \param   usEdgeDriftMax             maximum edge drift clock count allowed for scan/meaning/offset state
      \param   usMicroSteps               microsteps per full-step <b>information, NOT the hardware setting</b> (which is usually done in "at91sam7x.c") for the #STPCLKDIR_FULLSTEPSHIFT calculation option (stpclkdir.h)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										number of detected edges of specified type: <b>ucGapCnt</b> success, < <b>ucGapCnt</b> for failure
*/
UCHAR EncGapDiskTeachIn(UCHAR ucUnit, UCHAR ucEdgeType, bool bContinous, UCHAR ucGapCnt, ULONG ulClks, USHORT usClksAcc, USHORT usClksBrk, ULONG ulClkLowHz, ULONG ulClkHighHz, ULONG ulClksInit, ULONG ulClkInitHz, UCHAR ucThreshold, USHORT usToleranceClks, USHORT usEdgeDriftMax, USHORT usMicroSteps, U16 u16Timeout);

//!   retrieve selected error data <b>for all edges of specified type</b> stored by previous call to #EncGapDiskTeachIn
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param	ucEdgeType					   type of edge to retrieve data for (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   *pucEdgeError              (optional, set to NULL to disable) storage for edge number mismatch delta
      \param	*pulErrorClks				   (optional, set to NULL to disable) storage for clocks position delta (also for non-errors)
      \param	*pucEdgeErrorCnt				(optional, set to NULL to disable) storage for number of gap number mismatches
      \param   *pucToleranceErrorCnt      (optional, set to NULL to disable) storage for count of edges off-tolerance (usToleranceClks)
      \param   *pucEdgeIgnoreCnt          (optional, set to NULL to disable) storage for count of edges off-drift (usEdgeDriftMax)
      \param   bClear                     TRUE == clear specified and successful stored error data
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 (number of specified and successful updated storages) for success, == 0 for failure
      
      \note
               - setting ALL optional storages to NULL will return always == 0 == failure
*/
UCHAR EncGapDiskEdgeErrGet(UCHAR ucUnit, UCHAR ucEdgeType, UCHAR *pucEdgeError, ULONG *pulErrorClks, UCHAR *pucEdgeErrorCnt, UCHAR *pucToleranceErrorCnt, UCHAR *pucEdgeIgnoreCnt, bool bClear, U16 u16Timeout);

//!   retrieve selected data <b>for all edges of specified type</b> stored by previous call to #EncGapDiskTeachIn
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param	ucEdgeType					   type of edge to retrieve data for (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   *pusEdgeCnt                (optional, set to NULL to disable) storage for detected edge count
      \param	*pulCycleCnt				   (optional, set to NULL to disable) storage for completed teach-in cycles [<b>count -1</b>]
      \param	*pulEdgeIterationCnt		   (optional, set to NULL to disable) storage for completed teach-in repeat (retry) cycles
      \param   bClear                     TRUE == clear specified and successful stored data
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 (number of specified and successful updated storages) for success, == 0 for failure

      \note
               - setting ALL optional storages to NULL will return always == 0 == failure
*/
UCHAR EncGapDiskStatGet(UCHAR ucUnit, UCHAR ucEdgeType, UCHAR *pusEdgeCnt, ULONG *pulCycleCnt, ULONG *pulEdgeIterationCnt, bool bClear, U16 u16Timeout);

//!   retrieve selected offset data <b>for all edges of specified type</b> stored by <b>TskEncGapDiskMsg</b> for every edge verified and used for position offsetting (after teach-in)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param	ucEdgeType					   type of edge to retrieve data for (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   *pusEdgeOffsetCnt          (optional, set to NULL to disable) storage for edge offset count during move (after teach-in)
      \param   bClear                     TRUE == clear specified and successful stored data
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 (number of specified and successful updated storages) for success, == 0 for failure

      \note
               - setting ALL optional storages to NULL will return always == 0 == failure
*/
UCHAR EncGapDiskOffsetGet(UCHAR ucUnit, UCHAR ucEdgeType, UCHAR *pusEdgeOffsetCnt, bool bClear, U16 u16Timeout);

//!   retrieve the number of detected (and confirmed) edges <b>for all edges of specified type</b> stored by previous call to #EncGapDiskTeachIn
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param	ucEdgeType					   type of edge to retrieve data for (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   ulConfirm                  minimum detection count (== number of turns by #EncGapDiskTeachIn) required for valid edge to count
      \param   *pusEdgeCntTotal           (optional, set to NULL to disable) storage for total count of ALL detected edges (independend of <b>ulConfirm</b>)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 (number of specified and successful updated storages) for success, == 0 for failure

      \note
               - setting ALL optional storages to NULL will return always == 0 == failure
*/
UCHAR EncGapDiskEdgeCnt(UCHAR ucUnit, UCHAR ucEdgeType, ULONG ulConfirm, USHORT *pusEdgeCntTotal, U16 u16Timeout);

//!   retrieve <b>previous edge item number moved to</b> by #EncGapDiskEdgeGoto
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param   *pucEdgeCurr               (optional, set to NULL to disable) storage for previous edge item number moved to
      \param	ucEdgeType					   type of edge to retrieve data for (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 (number of specified and successful updated storages) for success, == 0 for failure
      
      \note
               - trying to retrieve <b>previous edge item number moved to</b> without any previous move returns == 0 == failure
               - setting ALL optional storages to NULL will return always == 0 == failure
*/
UCHAR EncGapDiskEdgeGet(UCHAR ucUnit, UCHAR *pucEdgeCurr, UCHAR ucEdgeType, U16 u16Timeout);

//!   retrieve selected data <b>for single specfified edge</b> of detected edges stored by previous call to #EncGapDiskTeachIn
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param   ucEdgeItem                 edge item number (lower than #ENCGAPDISK_EDGEITEM_MAX and <b>usually lower or equal than your encoder's gap count</b>)
      \param	ucEdgeType					   type of edge to retrieve data for (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   *pulClks                   (optional, set to NULL to disable) storage for clocks position of specified edge
      \param   *pulConfirm                (optional, set to NULL to disable) storage for confirm counter of specified edge
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 (number of specified and successful updated storages) for success, == 0 for failure
      
      \note
               - trying to retrieve <b>a edge item number >= #ENCGAPDISK_EDGEITEM_MAX</b> returns == 0 == failure
               - trying to retreive <b>a edge item number >= "your encoder's gap count"</b> returns > 0 (success) but stores ZERO data only
               - setting ALL optional storages to NULL will return always == 0 == failure
*/
UCHAR EncGapDiskEdgeDataGet(UCHAR ucUnit, UCHAR ucEdgeItem, UCHAR ucEdgeType, ULONG *pulClks, ULONG *pulConfirm, U16 u16Timeout);

//!   move to specified edge
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param   ucEdgeItem                 edge item number (<b>lower or equal than DETECTED encoder's gap count</b>)
      \param	ucEdgeType					   type of edge to move to (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   usMicroSteps               microsteps per full-step <b>information, NOT the hardware setting</b> (which is usually done in "at91sam7x.c") for the #STPCLKDIR_FULLSTEPSHIFT calculation option (stpclkdir.h)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
*/
UCHAR EncGapDiskEdgeGoto(UCHAR ucUnit, UCHAR ucEdgeItem, UCHAR ucEdgeType, USHORT usMicroSteps, U16 u16Timeout);

//!   set specified edge item number to ZERO item index (e.g. home edge item == zero item index)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param	ucEdgeType					   type of edge to move to (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   ucZeroItem                 edge item number to translate to zero (<b>lower or equal than DETECTED encoder's gap count</b>)
      \param   cZeroOffset                (optional, set 0 for default) offset for edge item number (useful when specified edge item number is before/after target edge item number)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \remarks
               - when specified edge item number is before/after the target edge item number use <b>cZeroOffset</b> to adjust it to the target one <b>WITHOUT the need of taking care of ANY LIMITS</b> you otherwise need to take care of
*/
bool  EncGapDiskEdgeZeroSet(UCHAR ucUnit, UCHAR ucEdgeType, UCHAR ucZeroItem, CHAR cZeroOffset, U16 u16Timeout);

//!   locate the home gap mark (WITHOUT MOVEMENT) in edges stored by previous call to #EncGapDiskTeachIn
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param	ucEdgeType					   type of edge to move to (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   ulConfirm                  minimum detection count (== number of turns by #EncGapDiskTeachIn) required for valid edge to count
      \param   ucRepeat                   number of repeats (<b>> 0!</b>) of the <b>smallest equal "gap to gap" distance</b> which can be found on the whole encoder's gap disk
      \param   bRepeatEqual               #TRUE == distance must <b>repeat EXACTLY</b> the specified count ucRepeat, #FALSE == distance must <b>repeat AT LEAST</b> the specified count <b>ucRepeat</b>
      \param   *pucEdgeItem               (optional, set NULL for default) storage for the gap number <b>of the end</b> of the validated pattern (required for #EncGapDiskEdgeZeroSet)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \remarks
               - to use <b>*pucEdgeItem</b> (which is the gap number <b>of the end</b> of the validated pattern) for #EncGapDiskEdgeZeroSet easily use the <b>cZeroOffset</b> (e.g. for <b>ucRepeat= 1</b> and <b>bRepeatEqual= TRUE</b> (3 gaps with 2 small and equal distances) use <b>-1</b>)
*/
bool  EncGapDiskHomeEdgeGet(UCHAR ucUnit, UCHAR ucEdgeType, ULONG ulConfirm, UCHAR ucRepeat, bool bRepeatEqual, UCHAR *pucEdgeItem, U16 u16Timeout);


// Prototypes for calibration data functions

//!   save/restore calibration params to/from INTERNAL <b>ANONYMOUSLY byte by byte accessible</b> calibration data structure #_SEncGapDiskCalibrationData
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param   ulConfirm                  minimum detection count (== number of turns by #EncGapDiskTeachIn) required for valid edge to transfer
      \param   bRestore                   #TRUE == restore calibration params, #FALSE == save calibration params
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \remarks
               - #_SEncGapDiskCalibrationData is internally allocated and is ONLY <b>ANONYMOUSLY byte by byte accessible</b>
*/
UCHAR EncGapDiskCalibrationDataRW(UCHAR ucUnit, ULONG ulConfirm, bool bRestore, U16 u16Timeout);

//!   determine size of INTERNAL <b>ANONYMOUSLY byte by byte accessible</b> calibration data structure #_SEncGapDiskCalibrationData in bytes
/**
      \return										size of INTERNAL <b>ANONYMOUSLY byte by byte accessible</b> calibration data structure #_SEncGapDiskCalibrationData in bytes
      
      \remarks
               - for anonymous byte by byte transfer you may retrieve its size in bytes (for retrieving address use #EncGapDiskCalibrationDataAddrGet)
*/
size_t EncGapDiskCalibrationDataSizeGet(void);

//!   determine address of INTERNAL <b>ANONYMOUSLY byte by byte accessible</b> calibration data structure #_SEncGapDiskCalibrationData in memory
/**
      \return										address of INTERNAL <b>item by item accessible</b> calibration data structure #_SEncGapDiskCalibrationData in memory
      
      \remarks
               - for anonymous byte by byte transfer you may retrieve its address in memory (for retrieving size in bytes use #EncGapDiskCalibrationDataSizeGet)
*/
void * EncGapDiskCalibrationDataAddrGet(UCHAR ucUnit);


// Prototypes for encgapdisk run param functions

//!   save/restore runtime params to/from SPECIFIED EXTERNAL runtime data structure #_SEncGapDiskRunParams
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param   *pvEncGapDiskRunParams     storage of type struct #_SEncGapDiskRunParams to transfer runtime data to/from
      \param   bRestore                   #TRUE == restore runtime params, #FALSE == save runtime params
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \remarks
               - you need to allocate YOURSELF a structure of type #_SEncGapDiskRunParams to transfer the data
*/
UCHAR EncGapDiskRunParamsRW(UCHAR ucUnit, void *pvEncGapDiskRunParams, bool bRestore, U16 u16Timeout);


// Prototypes for snapshot functions

//!   determine size of INTERNAL <b>ANONYMOUSLY byte by byte accessible</b> #_SEncGapDiskSnapshot
/**
      \return										size of INTERNAL <b>ANONYMOUSLY byte by byte accessible</b> snapshot data structure #_SEncGapDiskSnapshot in bytes
      
      \remarks
               - for anonymous byte by byte transfer you may retrieve its size in bytes (for retrieving address use #EncGapDiskSnapShotAddrGet)
*/
size_t EncGapDiskSnapShotSizeGet(void);

//!   determine address of INTERNAL <b>ANONYMOUSLY byte by byte accessible</b> #_SEncGapDiskSnapshot in memory
/**
      \return										address of INTERNAL <b>ANONYMOUSLY byte by byte accessible</b> snapshot data structure #_SEncGapDiskSnapshot in memory
      
      \remarks
               - for anonymous byte by byte transfer you may retrieve its address in memory (for retrieving size in bytes use #EncGapDiskSnapShotSizeGet)
*/
void * EncGapDiskSnapShotAddrGet(UCHAR ucUnit);

//!   save/restore snapshot to/from INTERNAL <b>ANONYMOUSLY byte by byte accessible</b> runtime data structure #_SEncGapDiskSnapshot
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param   ulConfirm                  minimum detection count (== number of turns by #EncGapDiskTeachIn) required for valid edge to transfer
      \param   bRestore                   #TRUE == restore runtime params, #FALSE == save runtime params
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \note
               - #_SEncGapDiskSnapshot is internally allocated and is ONLY <b>ANONYMOUSLY byte by byte accessible</b>
               - for restore option the STORED minimum detection count IS RAISED to ulConfirm (when necessary)
*/
UCHAR EncGapDiskSnapShot(UCHAR ucUnit, ULONG ulConfirm, bool bRestore, U16 u16Timeout);

//!   retrieve configured unit of stpclkdir (for direct motor access)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)

      \return										configured unit of stpclkdir
*/
UCHAR EncGapDiskStpClkDirUnitGet(UCHAR ucUnit);

#endif
