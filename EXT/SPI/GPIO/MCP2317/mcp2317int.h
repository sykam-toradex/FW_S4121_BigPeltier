//!	\internal \brief <b>INTERNAL</b> interface of MCP2317 peripheral driver
/**
      \internal
		\file		mcp2317int.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/GPIO/MCP2317/MCP2317INT.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2016-03-02 initial version
// 2016-03-02 added function Mcp2317IrqCtrl (controls hw & sw irq)
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_GPIO_MCP2317_MCP2317INT_H
#define SYKAM_EXT_SPI_GPIO_MCP2317_MCP2317INT_H


// Prototypes

//!   control assigned hardware pin's interrupt
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of Mcp2317dev.h)
      \param	bEnable					      #TRUE == enable pin's interrupt, #FALSE == disable pin's interrupt


      \return										> 0 for success, == 0 for failure
      \remarks
               - the software flag for the interrupt state is updated by this function too
      \note
               - <b>#PioInit is required BEFORE calling</b> #Mcp2317Init
               - this function requires valid params thru the init callback first thru #PioInit to get the assigned hardware to control
*/
UCHAR Mcp2317IrqCtrl(UCHAR ucUnit, bool bEnable);

#endif
