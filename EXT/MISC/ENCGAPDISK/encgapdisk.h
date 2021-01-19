//!	defines and configuration of ENCGAPDISK driver
/**
		\file		encgapdisk.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/MISC/ENCGAPDISK/ENCGAPDISK.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-03-03 initial version
// 2010-03-31 renamed ENCGAPDISK_GAP_MAX to ENCGAPDISK_EDGE_MAX define
// 2010-03-31 added ENCGAPDISK_EDGE_FALL and ENCGAPDISK_EDGE_RAISE defines
// 2010-03-31 added ENCGAPDISK_EDGE_MAX define
// 2010-05-12 added ENCGAPDISK_COMPAREITEM_MAX
// 2010-07-02 added ENCGAPDISK_REPEAT_MAX
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-30 added piobase para to pio irq function and irq handler
// 2010-09-30 added ENCGAPDISK_REREADPIN_COUNT
// 2010-10-01 removed obsolete ENCGAPDISK_REPEAT_MAX
// 2010-10-07 decreased ENCGAPDISK_REREADPIN_COUNT (64 is highly critical)
// 2010-10-20 ENCGAPDISK_REREADPIN_COUNT option may be disabled now by value 0
// 2010-11-15 rr: added new define ENCGAPDISK_MAX_FREQUENCY_SLOPE
// 2010-11-16 added new state define ENCGAPDISK_STATE_OFFSET
// 2010-11-18 added SIZEOF_SENCGAPDISKCALIBRATIONDATA for array declarations
// 2010-11-19 added optional define ENCGAPDISK_OFFSETCORRECTION
// 2010-11-19 added optional define ENCGAPDISK_OFFSETBYDIRECTION
// 2010-11-23 increased stack size definition STKTSKENCGAPDISKMSG
// 2010-12-17 removed ENCGAPDISK_MAX_FREQUENCY_SLOPE (via StpClkDirProfileAdjust)
// 2011-09-28 added optional define ENCGAPDISK_OFFRANGEMODULOCHECK
// 2011-09-29 added optional define ENCGAPDISK_EDGEDRIFTMAXFACTOR
// 2011-11-02 added public doxygen documentation
// 2011-11-30 added internal doxygen documentation
// 2016-03-01 disabled ENCGAPDISK_OFFSETCORRECTION per default to avoid bursts
// 2016-03-02 added macros and prototype for InitEncGapDiskHandler
// 2016-06-01 re-enabled ENCGAPDISK_OFFSETCORRECTION (requires u32GateMask of _SEncGapDiskRO)
// 2016-06-01 added name and prototype macros for signal gate interrupt function
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISK_H
#define SYKAM_EXT_MISC_ENCGAPDISK_ENCGAPDISK_H


// Defines


// States

//! \internal \brief <b>Internal state</b>: none (initial)
#define ENCGAPDISK_STATE_NONE       (0)

//! \internal \brief <b>Internal state</b>: teach-in edge data
#define ENCGAPDISK_STATE_TEACHIN    (1)

//! \internal \brief <b>Internal state</b>: mean-in (averaging) edge data
#define ENCGAPDISK_STATE_MEANIN     (2)

//! \internal \brief <b>Internal state</b>: verifying average data
#define ENCGAPDISK_STATE_VERIFY     (4)

//! \internal \brief <b>Internal state</b>: edge data offset determination (for position error detection)
#define ENCGAPDISK_STATE_OFFSET     (5)


//! <b>Edge type</b>: falling edge
#define ENCGAPDISK_EDGE_FALL        (0)

//! <b>Edge type</b>: raising edge
#define ENCGAPDISK_EDGE_RAISE       (1)

//! <b>Edge type</b>: count
#define ENCGAPDISK_EDGETYPE_MAX     (2)

//! <b>Size in bytes</b>: _SEncGapDiskCalibrationData
#define SIZEOF_SENCGAPDISKCALIBRATIONDATA (sizeof(struct _SEncGapDiskCalibrationData))


// Macros

// Function template IsrEncGapDisk
#define ENCGAPDISK_ISR_FUNC(unit)   void IsrEncGapDisk_##unit(U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio) { IsrEncGapDiskIrqHandler(unit, u32ISR, u32PDSR, u32Pin, bLevel, pPio); }

// Function prototype IsrEncGapDisk
#define ENCGAPDISK_ISR_PROTO(unit)  void IsrEncGapDisk_##unit(U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio)

// Function name IsrEncGapDisk
#define ENCGAPDISK_ISR_NAME(unit)   IsrEncGapDisk_##unit

// Function template InitEncGapDisk
#define ENCGAPDISK_INIT_FUNC(unit)   bool InitEncGapDisk_##unit(U32 u32Pin, AT91PS_PIO pPio) { return(InitEncGapDiskHandler(unit, u32Pin, pPio)); }

// Function prototype InitEncGapDisk
#define ENCGAPDISK_INIT_PROTO(unit)  bool InitEncGapDisk_##unit(U32 u32Pin, AT91PS_PIO pPio)

// Function name InitEncGapDisk
#define ENCGAPDISK_INIT_NAME(unit)   InitEncGapDisk_##unit


// Function template IsrEncGapDiskGate
#define ENCGAPDISK_ISRG_FUNC(unit)   void IsrEncGapDiskGate_##unit(U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio) { IsrEncGapDiskGateIrqHandler(unit, u32ISR, u32PDSR, u32Pin, bLevel, pPio); }

// Function prototype IsrEncGapDiskGate
#define ENCGAPDISK_ISRG_PROTO(unit)  void IsrEncGapDiskGate_##unit(U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio)

// Function name IsrEncGapDiskGate
#define ENCGAPDISK_ISRG_NAME(unit)   IsrEncGapDiskGate_##unit

// Function template InitEncGapDiskGate
#define ENCGAPDISK_INITG_FUNC(unit)   bool InitEncGapDiskGate_##unit(U32 u32Pin, AT91PS_PIO pPio) { return(InitEncGapDiskGateHandler(unit, u32Pin, pPio)); }

// Function prototype InitEncGapDiskGate
#define ENCGAPDISK_INITG_PROTO(unit)  bool InitEncGapDiskGate_##unit(U32 u32Pin, AT91PS_PIO pPio)

// Function name InitEncGapDiskGate
#define ENCGAPDISK_INITG_NAME(unit)   InitEncGapDiskGate_##unit


//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------

//! <b>Overrideable configuration:</b> multiply EdgeDriftMax param for offset mode
#ifndef ENCGAPDISK_EDGEDRIFTMAXFACTOR
   #define ENCGAPDISK_EDGEDRIFTMAXFACTOR (3)
#endif

//! <b>Overrideable configuration:</b> post move offset correction to keep position in sync with encoder
#ifndef ENCGAPDISK_OFFSETCORRECTION
   #define ENCGAPDISK_OFFSETCORRECTION (1)
#endif

//! <b>Overrideable configuration:</b> offset inversion by direction
#ifndef ENCGAPDISK_OFFSETBYDIRECTION
   #define ENCGAPDISK_OFFSETBYDIRECTION (0)
#endif

//! <b>Overrideable configuration:</b> modulo range checking (to avoid ignoring "off range" edges just shifted "off range" by encoder jittering)
#ifndef ENCGAPDISK_OFFRANGEMODULOCHECK
   #define ENCGAPDISK_OFFRANGEMODULOCHECK (1)
#endif

//! <b>Overrideable configuration:</b> number of encgapdisk units supported simultaneously
#ifndef ENCGAPDISK_UNIT_MAX
   #define ENCGAPDISK_UNIT_MAX (1)
#endif

//! <b>Overrideable configuration:</b> storage count of edges (for each type)
#ifndef ENCGAPDISK_EDGEITEM_MAX
   #define ENCGAPDISK_EDGEITEM_MAX (32+ 8)
#endif

//! <b>Overrideable configuration:</b> pattern length count to identify re-start of turn edge data within several continous turns <b>[for each type of edge INDIVIDUALLY]</b>
#ifndef ENCGAPDISK_COMPAREITEM_MAX
   #define ENCGAPDISK_COMPAREITEM_MAX (2)
#endif

//! <b>Overrideable configuration:</b> number of messages buffered for IRQ to task
#ifndef ENCGAPDISK_MBXCNTMSG
   #define ENCGAPDISK_MBXCNTMSG (ENCGAPDISK_EDGEITEM_MAX* 4)
#endif

//! <b>Overrideable configuration:</b> number of pin re-reads after IRQ to determine pin's true level <b>[one of both states needs to be seen NEVER during the re-reading(s)]</b>
#ifndef ENCGAPDISK_REREADPIN_COUNT
   #define ENCGAPDISK_REREADPIN_COUNT (3)
#endif

//! <b>Overrideable configuration:</b> stack size of message task
#ifndef ENCGAPDISK_STACK_STKTSKENCGAPDISKMSG
   #define ENCGAPDISK_STACK_STKTSKENCGAPDISKMSG (640)
#endif

// Each supported encgapdisk unit requires a prototype definition here
#if ENCGAPDISK_UNIT_MAX > 0
   ENCGAPDISK_ISR_PROTO(0);
   ENCGAPDISK_ISRG_PROTO(0);
   ENCGAPDISK_INIT_PROTO(0);
   ENCGAPDISK_INITG_PROTO(0);
#endif

#if ENCGAPDISK_UNIT_MAX > 1
   ENCGAPDISK_ISR_PROTO(1);
   ENCGAPDISK_ISRG_PROTO(1);
   ENCGAPDISK_INIT_PROTO(1);
   ENCGAPDISK_INITG_PROTO(1);
#endif

#if ENCGAPDISK_UNIT_MAX > 2
   ENCGAPDISK_ISR_PROTO(2);
   ENCGAPDISK_ISRG_PROTO(2);
   ENCGAPDISK_INIT_PROTO(2);
   ENCGAPDISK_INITG_PROTO(2);
#endif

// Prototypes
__task void TskEncGapDiskMsg(void *pvParam);
void IsrEncGapDiskIrqHandler(UCHAR ucUnit, U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio);
bool InitEncGapDiskHandler(UCHAR ucUnit, U32 u32Pin, AT91PS_PIO pPio);
void IsrEncGapDiskGateIrqHandler(UCHAR ucUnit, U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio);
bool InitEncGapDiskGateHandler(UCHAR ucUnit, U32 u32Pin, AT91PS_PIO pPio);

#endif
