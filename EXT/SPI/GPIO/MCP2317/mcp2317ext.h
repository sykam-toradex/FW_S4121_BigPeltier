//!	external interface of MCP2317 peripheral driver
/**
		\file		mcp2317ext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/GPIO/MCP2317/MCP2317EXT.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-12-18 initial version
// 2009-12-21 removed IsrMcp2317; renamed IsrMcp2317Sub to NotifyMcp2317
// 2009-12-22 added Mcp2317SetOutput
// 2010-01-28 fixed include trigger define name
// 2010-02-18 added param u16Timeout to Mcp2317SetOutput
// 2010-02-18 changed return type of Mcp2317Init to USHORT
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-11-03 added public doxygen documentation
// 2016-03-02 added function Mcp2317IrqCtrl (controls hw & sw irq)
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_GPIO_MCP2317_MCP2317EXT_H
#define SYKAM_EXT_SPI_GPIO_MCP2317_MCP2317EXT_H


// Externals (located inside AT91SAM7X...h)

//! SMcp2317List reference (located in "at91sam7x.c")
extern struct _SMcp2317Item SMcp2317List[];


// Prototypes

//!   init a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of mcp2317dev.h)
      \param	*pSMcp2317List					the array of struct #_SMcp2317Item with a defined item for every pio pin to configure
      \param   ucItemCnt                  the count of items in the specified array to configure
      \param	oiMsgNewSem				      OS_ID of <b>pre-initialized (to 0!)</b> semaphore which is assigned to the spi driver channel this peripheral is connected to

      \return										> 0 for success, == 0 for failure
*/
USHORT Mcp2317Init(UCHAR ucUnit, struct _SMcp2317Item *pSMcp2317List, UCHAR ucItemCnt, OS_ID oiMsgNewSem);

//!   set output
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of mcp2317dev.h)
      \param	usMask					      mask with all SET (OR-ed) port line bits to output to (refer to #MCP2317_PA0 ff. and #MCP2317_PB0 ff.)
      \param   usLevel                    mask with port line levels coresponding to <b>usMask</b> to determine a level (0 or 1) for each port line individually
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success (SET (OR-ed) port line bits), == 0 <b>(also != usMask)</b> for failure
      
      \note
               - <b>CAUTION:</b> do NOT mix up the input levels (specified in struct #_SMcp2317Item) and the output levels ABOVE!<br>
               - the mcp2317 device supports optional inversion of it's inputs but this feature is <b>NOT USED HERE</b> (refer to <a href="#Mcp2317SetOutput">mcp2317 manual 1.6.2. page 13</a>)
               
      <b>Manual reference(s):</b><br>
      <a name="Mcp2317SetOutput">
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_1.6.2_p13_IPOL.pdf" type="application/pdf">MCP2317 1.6.2 page 13: IPOL</a><br>
*/
USHORT Mcp2317SetOutput(UCHAR ucUnit, USHORT usMask, USHORT usLevel, U16 u16Timeout);

//!   <b>PROTOTYPE for application's</b> isr handler (usually called from entry of #_SMcp2317Item <b>ONLY</b>)
/**
      \param   u16INTF                    mcp2317 register INTF (INTERRUPT FLAG REGISTER) with all interrupt triggering port bits set
      \param   u16INTCAP                  mcp2317 register INTCAP (INTERRUPT CAPTURED VALUE FOR PORT REGISTER) with levels of port pins at interrupt's time
      \param   u16Pin                     mask of the pin (pin's bit is set)
      \param   bLevel                     level of the pin at interrupt's time

      \note
               - <b>you have to add your own isr handler CODE in your application</b> to get notifications of input port line(s) change (there is <b>NO Mcp2317GetInput</b>)
               - for multiple pins triggered at the same time you'll get MORE THAN ONE set bit for <b>u16INTF</b> and <b>u16INTCAP</b> for dependency checking<br>
               
      <b>Example code: basic input change notification</b><br>
      \include NotifyMcp2317.c
*/
void NotifyMcp2317(U16 u16INTF, U16 u16INTCAP, U16 u16Pin, bool bLevel);

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
