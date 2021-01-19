//!	error definitions of AT91SAM7X STPCLKDIR driver
/**
		\file		stpclkdirerr.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MISC/STPCLKDIR/STPCLKDIREXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-02-26 initial version
// 2011-02-17 renamed STPCLKDIR_ERR_STATE_BRK to STPCLKDIR_ERR_STATE_BRKACC
// 2011-09-16 added STPCLKDIR_ERR_MBX_CLEAR and STPCLKDIR_ERR_START
// 2011-10-19 added public doxygen documentation
// 2013-08-21 updated public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRERR_H
#define SYKAM_CPU_AT91_MISC_STPCLKDIR_STPCLKDIRERR_H 1


//! <b>Error - not enough space:</b> queue acceleration access
#define STPCLKDIR_ERR_SPACE_QACC				1

//! <b>Error - not enough space:</b> queue acceleration access 1st value
#define STPCLKDIR_ERR_SPACE_QACC1ST			5

//! <b>Error - not enough space:</b> queue acceleration access 2nd - next to last value
#define STPCLKDIR_ERR_SPACE_QACC2NDFF		10

//! <b>Error - not enough space:</b> queue acceleration access last value
#define STPCLKDIR_ERR_SPACE_QACCLAST		15


//! <b>Error - not enough space:</b> queue brake access
#define STPCLKDIR_ERR_SPACE_QBRK				20

//! <b>Error - not enough space:</b> queue brake access 1st value
#define STPCLKDIR_ERR_SPACE_1ST				25

//! <b>Error - not enough space:</b> queue brake access 2nd - next to last value
#define STPCLKDIR_ERR_SPACE_QBRK2NDFF		30

//! <b>Error - not enough space:</b> queue brake access last value
#define STPCLKDIR_ERR_SPACE_QBRKLAST		35


//! <b>Error - invalid para:</b> invalid clocks difference start/run (or current/new)
#define STPCLKDIR_ERR_INVALID_CLKDIFF		40

//! <b>Error - invalid para:</b> invalid clocks start value
#define STPCLKDIR_ERR_INVALID_CLKLOW		45

//! <b>Error - invalid para:</b> invalid clocks run value
#define STPCLKDIR_ERR_INVALID_CLKHIGH		50


//! <b>Error - invalid range:</b> clocks count
#define STPCLKDIR_ERR_RANGE_CLKS				55

//! <b>Error - invalid range:</b> clocks acceleration count
#define STPCLKDIR_ERR_RANGE_CLKSACC			60

//! <b>Error - invalid range:</b> clocks brake count
#define STPCLKDIR_ERR_RANGE_CLKSBRK			65

//! <b>Error - invalid range:</b> unit
#define STPCLKDIR_ERR_RANGE_UNIT				70


//! <b>Error - invalid state:</b> brake/acceleration
#define STPCLKDIR_ERR_STATE_BRKACC			75

//! <b>Error - invalid state:</b> unit (hardware)
#define STPCLKDIR_ERR_STATE_UNITHW			80

//! <b>Error - invalid state:</b> unit (rtos)
#define STPCLKDIR_ERR_STATE_UNITOS			85


//! <b>Error - general failure:</b> mailbox clear
#define STPCLKDIR_ERR_MBX_CLEAR           90

//! <b>Error - general failure:</b> start
#define STPCLKDIR_ERR_START               95

#endif
