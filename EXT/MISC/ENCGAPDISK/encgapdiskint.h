//!	\internal \brief <b>INTERNAL</b> interface of ENCGAPDISK driver
/**
      \internal
		\file		encgapdiskint.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/MISC/ENCGAPDISK/ENCGAPDISKINT.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-09-16 initial version (imported internal/private prototypes from ext.h
// 2010-09-30 added para *usEdgeIdxStart to _EncGapDiskEdgeCnt
// 2011-11-02 removed public doxygen documentation for private functions
// 2011-11-02 moved EncGapDiskClear from encgapdiskext.h
// 2011-11-10 added internal doxygen documentation
// 2016-03-02 updated internal doxygen documentation
// 2016-03-02 added function EncGapDiskIrqCtrl (controls hw & sw irq)
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISKINT_H
#define SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISKINT_H


// Prototypes

//!   \internal \brief <b>Internal function:</b> clear data storage of #EncGapDiskTeachIn
/**   
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param   bReverse                   direction for next turn (#FALSE == same direction, #TRUE == reverse direction for ping-pong mode)
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return                             > 0 for success, == 0 for failure
*/
UCHAR EncGapDiskClear(UCHAR ucUnit, bool bReverse, U16 u16Timeout);

//!   \internal \brief <b>Internal function:</b> retrieve the number of detected (and confirmed) edges <b>for all edges of specified type</b> stored by previous call to #EncGapDiskTeachIn <b>WITHOUT resource locking</b>
/**   
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param   ucEdgeType                 type of edge to check detection count after teach-in turn(s) (#ENCGAPDISK_EDGE_FALL or #ENCGAPDISK_EDGE_RAISE)
      \param   ulConfirm                  minimum detection count (== number of turns by #EncGapDiskTeachIn) required for valid edge to count
      \param   *pusEdgeCntTotal           (optional, set to NULL to disable) storage for total count of ALL detected edges (independend of <b>ulConfirm</b>)
      \param   *usEdgeIdxStart            (optional, set to NULL to disable) storage for first (0 based) edge index with valid (ulConfirm) data

      \return                             > 0 (number of specified and successful updated storages) for success, == 0 for failure
      
      \note
               - <b>this function does NOT lock the resources</b> (use #EncGapDiskEdgeCnt for application purpose!)
*/
UCHAR _EncGapDiskEdgeCnt(UCHAR ucUnit, UCHAR ucEdgeType, ULONG ulConfirm, USHORT *pusEdgeCntTotal, USHORT *usEdgeIdxStart);

//!   control assigned hardware pin's interrupt
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of encgapdiskdev.h)
      \param	bEnable					      #TRUE == enable pin's interrupt, #FALSE == disable pin's interrupt


      \return										> 0 for success, == 0 for failure
      \remarks
               - the software flag for the interrupt state is updated by this function too
      \note
               - <b>#PioInit is required BEFORE calling</b> #EncGapDiskTeachIn
               - this function requires valid params thru the init callback first thru #PioInit to get the assigned hardware to control
*/
UCHAR EncGapDiskIrqCtrl(UCHAR ucUnit, bool bEnable);

#endif
