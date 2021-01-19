//!	external interface of AT91SAM7X PIO driver
/**
		\file		pioext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PIO/PIOEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-09-10 initial version
// 2009-11-11 renamed init function to PioInit
// 2009-11-11 removed function AT91F_PIO_CfgPinList
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-10-20 added public doxygen documentation
// 2016-03-02 updated public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PIO_PIOEXT_H
#define SYKAM_CPU_AT91_PIO_PIOEXT_H 1


// Externals (located inside AT91SAM7X...h)

//! PioListA reference (located in "at91sam7x.c")
extern struct _SPioItem SPioListA[];

//! PioListB reference (located in "at91sam7x.c")
extern struct _SPioItem SPioListB[];


// Prototypes

//! Init pio pin(s) according to specified array item(s)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of piodev.h)
      \param	*pSPioList					   the array of struct _SPioItem with a defined item for every pio pin to configure
      \param	ucItemCnt				      the count of items in the specified array to configure

      \return										> 0 for success, == 0 for failure
      
      \note
               - this will enable <b>immediately interrupts</b> of all pins <b>with pin interrupt handler</b> and <b>without (NULL)</b> pin init handler or <b>with pin init handler returning</b> #TRUE
*/
bool PioInit(UCHAR ucUnit, struct _SPioItem *pSPioList, UCHAR ucItemCnt);

#endif
