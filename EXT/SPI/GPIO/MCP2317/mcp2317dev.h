//!	hardware descriptor(s) of MCP2317 peripheral driver
/**
		\file		mcp2317dev.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/GPIO/MCP2317/MCP2317DEV.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-12-18 initial version
// 2009-12-21 moved basic configuration defines to board specific include file
// 2009-12-23 moved u8IOCON configuration to board specific MCP2317_IOCON
// 2010-02-18 added SemMcp2317Reg
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-12-09 raised task priority u8TskNotifyMcp2317 to OS_P_HIGH
// 2011-03-17 added u8TskNotifyHelperMcp2317 to _SMcp2317RO
// 2011-03-17 added new mailbox MbxNotifyHelperMcp2317
// 2011-04-14 added u16DelayIrqQuery to _SMcp2317RO
// 2011-11-03 added public doxygen documentation
// 2013-08-22 changed delays/timeouts from milliseconds to ticks (_OS_T_MS for compatibility)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_GPIO_MCP2317_MCP2317DEV_H
#define SYKAM_EXT_SPI_GPIO_MCP2317_MCP2317DEV_H


// Declare arrays of semaphores

//!   semaphore to trigger TskNotifyMcp2317 by Mcp2317IrqHandler
/**
   data flow thru SemNotifyMcp2317
   \dot
   digraph SemNotifyMcp2317 {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Mcp2317 [label="Mcp2317" shape="parallelogram"];
      Pin [label="I/O pin irq" shape="parallelogram"];
      Mcp2317IrqHandler [label="Mcp2317IrqHandler"];
      SemNotify [label="SemNotifyMcp2317" style="filled"];
      Mcp2317Reg [label="SMcp2317RW.Mcp2317Reg" URL="\ref _SMcp2317RW"];
      TskNotifyMcp2317 [label="TskNotifyMcp2317" shape="ellipse"];   
      MbxNotifyHelperMcp2317 [label="MbxNotifyHelperMcp2317"];
      TskNotifyHelperMcp2317 [label="TskNotifyHelperMcp2317" shape="ellipse"];
      User [label="User notification function(s)" shape="ellipse" style="dashed" color="dimgray"];

      Mcp2317 -> Mcp2317Reg [label=" Spi" dir="both" fontname="FreeSans" fontsize="10" fontcolor="dimgray"];
      Mcp2317Reg -> TskNotifyMcp2317 [dir="both"];
      Mcp2317 -> Pin [arrowhead="odot"];
      
      Pin -> Mcp2317IrqHandler;
      Mcp2317IrqHandler -> SemNotify;
      SemNotify -> TskNotifyMcp2317 [arrowhead="odot"];
      TskNotifyMcp2317 -> MbxNotifyHelperMcp2317;
      MbxNotifyHelperMcp2317 -> TskNotifyHelperMcp2317 [arrowhead="odot"];
      TskNotifyHelperMcp2317 -> User [label=" SMcp2317List" fontname="FreeSans" fontsize="10"];
   }
   \enddot
*/
OS_SEM SemNotifyMcp2317[MCP2317_UNIT_MAX];

//!   semaphore to lock device register cache
OS_SEM SemMcp2317Reg[MCP2317_UNIT_MAX];


// Declare arrays of mailboxes (size definitions are located inside mcp2317.h)

//!   input pin change queue from TskNotifyMcp2317 <b>(io handling)</b> to TskNotifyHelperMcp2317 <b>(notification function calling)</b>
/**
   data flow thru MbxNotifyHelperMcp2317
   \dot
   digraph MbxNotifyHelperMcp2317 {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Mcp2317 [label="Mcp2317" shape="parallelogram"];
      Pin [label="I/O pin irq" shape="parallelogram"];
      Mcp2317IrqHandler [label="Mcp2317IrqHandler"];
      SemNotify [label="SemNotifyMcp2317"];
      Mcp2317Reg [label="SMcp2317RW.Mcp2317Reg" URL="\ref _SMcp2317RW"];
      TskNotifyMcp2317 [label="TskNotifyMcp2317" shape="ellipse"];   
      MbxNotifyHelperMcp2317 [label="MbxNotifyHelperMcp2317" style="filled"];
      TskNotifyHelperMcp2317 [label="TskNotifyHelperMcp2317" shape="ellipse"];
      User [label="User notification function(s)" shape="ellipse" style="dashed" color="dimgray"];

      Mcp2317 -> Mcp2317Reg [label=" Spi" dir="both" fontname="FreeSans" fontsize="10" fontcolor="dimgray"];
      Mcp2317Reg -> TskNotifyMcp2317 [dir="both"];
      Mcp2317 -> Pin [arrowhead="odot"];
      
      Pin -> Mcp2317IrqHandler;
      Mcp2317IrqHandler -> SemNotify;
      SemNotify -> TskNotifyMcp2317 [arrowhead="odot"];
      TskNotifyMcp2317 -> MbxNotifyHelperMcp2317;
      MbxNotifyHelperMcp2317 -> TskNotifyHelperMcp2317 [arrowhead="odot"];
      TskNotifyHelperMcp2317 -> User [label=" SMcp2317List" fontname="FreeSans" fontsize="10"];
   }
   \enddot
*/
os_mbx_declare_ex(MbxNotifyHelperMcp2317, MCP2317_MBXCNTNOTIFYHELPERMCP2317, [MCP2317_UNIT_MAX]);


//! Hardware descriptors
struct _SMcp2317RO SMcp2317RO[MCP2317_UNIT_MAX] =
{
   {
      /* ucUnit                     */ MCP2317_UNIT,
      /* ucChannel                  */ MCP2317_CHANNEL,
      /* usBlockSizeMax             */ MCP2317_BLOCKSIZEMAX,
      /* ulSCBR                     */ MCP2317_SCBR,
      /* u8HAID                     */ MCP2317_HAID,
      /* u8IOCON                    */ MCP2317_IOCON,
      /* pSMcp2317List              */ NULL,
      /* SMcp2317ListSize           */ 0,
      /* u8TskNotifyMcp2317         */ OS_P_HIGH,
      /* u8TskNotifyHelperMcp2317   */ OS_P_NORMAL,
      /* u16DelayIrqQuery           */ _OS_T_MS(100, OS_TICK_RATE),
      /* u16Timeout                 */ _OS_T_MS(100, OS_TICK_RATE)
   }
#if MCP2317_UNIT_MAX > 1
   ,
   {
      /* ucUnit                     */ MCP2317_2_UNIT,
      /* ucChannel                  */ MCP2317_2_CHANNEL,
      /* usBlockSizeMax             */ MCP2317_BLOCKSIZEMAX,
      /* ulSCBR                     */ MCP2317_SCBR,
      /* u8HAID                     */ MCP2317_HAID,
      /* u8IOCON                    */ MCP2317_IOCON,
      /* pSMcp2317List              */ NULL,
      /* SMcp2317ListSize           */ 0,
      /* u8TskNotifyMcp2317         */ OS_P_HIGH,
      /* u8TskNotifyHelperMcp2317   */ OS_P_NORMAL,
      /* u16DelayIrqQuery           */ _OS_T_MS(100, OS_TICK_RATE),
      /* u16TimeoutMS               */ _OS_T_MS(100, OS_TICK_RATE)
   }
#endif
};

#endif
