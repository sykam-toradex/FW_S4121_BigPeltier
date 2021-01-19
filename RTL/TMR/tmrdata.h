//!	data definitions of RTL TMR dispatcher
/**
		\file		tmrdata.h
*/

//----------------------------------------------------------------------------
// R T L   K E R N E L   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/RTL/TMR/TMRDATA.H
// Purpose: RTL KERNEL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-08-04 initial version
// 2011-08-10 added u16Info2 to _STmrItem
// 2011-08-10 imported defines from tmr.h and changed them to enum
// 2011-08-17 renamed u16Info/2 to u16Val/2 in _STmrItem
// 2011-10-26 added public doxygen documentation
// 2015-01-15 added more manual references to public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_RTL_TMR_TMRDATA_H
#define SYKAM_RTL_TMR_TMRDATA_H 1


//! Match type
typedef enum
{
   TMR_MATCH,                             //!< Timer number match 1st value u16Val of _STmrItem
   TMR_RANGE,                             //!< Timer number <b>in</b> range (incl. limits) of u16Val and u16Val2 of _STmrItem
   TMR_RANGE_OUT,                         //!< Timer number <b>out off</b> range (incl. limits) of u16Val and u16Val2 of _STmrItem
   TMR_MASK_AND,                          //!< Timer number & u16Val of _STmrItem == u16Val of _STmrItem
   TMR_MASK_AND_ANY                       //!< Timer number & u16Val of _STmrItem > 0
} ETmrOption;

//! List structure item (for building list array struct _STmrItem STmrItemList[AT91F_TMRLIST_SIZE] placed in "at91sam7x.c" <b>with valid setting for AT91F_TMRLIST_SIZE in "at91sam7x.h"</b>)
/**
   data flow thru STmrItemList
   \dot
   digraph STmrItemList {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Tmr [label="RTOS timer"];
      TmrHandler [label="os_tmr_call"];
      STmrItemList [label="STmrItemList" style="filled"];
      User [label="User timer handler(s)" shape="ellipse" style="dashed" color="dimgray" style="dashed"];

      Tmr -> TmrHandler [arrowhead="odot"];
      TmrHandler -> STmrItemList;
      STmrItemList -> User;
   }
   \enddot
   
      <b>Example: the following array is usually placed in "at91sam7x.c"</b>)<br>
      \include _STmrItem.h
*/
struct _STmrItem
{
   U16 u16Val;                            //!< Info match value for handler
   U16 u16Val2;                           //!< Info match value two for handler
   ETmrOption EOption;                    //!< Info match mode for handler
   /**
         \param   u16Info                 number of elapsed (fired) timer
         
         <b>Manual reference(s):</b><br>
         <a href="../../../MANUAL/RTL/TMR/RTL_os_tmr_call.pdf" type="application/pdf">RL-ARM User's Guide: os_tmr_call</a><br>
         <a href="../../../MANUAL/RTL/TMR/RTL_os_tmr_create.pdf" type="application/pdf">RL-ARM User's Guide: os_tmr_create</a><br>
         <a href="../../../MANUAL/RTL/TMR/RTL_os_tmr_kill.pdf" type="application/pdf">RL-ARM User's Guide: os_tmr_kill</a><br>
   */
   void (*Isr) (U16 u16Info);             //!< Isr handler function to call
};

#endif
