//!	data definitions of AT91SAM7X PWM driver
/**
		\file		pwmdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PWM/PWMDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-15 initial version
// 2010-07-01 added _SPwmChannelRO, _SPwmChannelRW and _SPwmTsk
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-11-23 added data comments
// 2011-10-20 added public doxygen documentation
// 2011-11-24 added public doxygen documentation
// 2011-11-24 added internal doxygen documentation
// 2013-08-22 changed delays from milliseconds to ticks
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PWM_PWMDATA_H
#define SYKAM_CPU_AT91_PWM_PWMDATA_H 1


//! \internal \brief <b>Internal data:</b> Task parameter data
struct _SPwmChannelTsk
{
   UCHAR ucUnit;                          //!< Unit task is assigned to
   UCHAR ucChannel;                       //!< Channel task is assigned to
};

//! Hardware descriptor data (read only) for each unit
struct _SPwmRO
{
   AT91PS_PWMC       pBase;               //!< Base address of peripheral to handle
   U32               u32PeriphId;         //!< Bit-Id (!) of peripheral to handle
   struct _SPwmItem *pSPwmList;           //!< List of used pwm's configuration
   UCHAR             ucPwmListSize;       //!< Size of the list of used pwm's configuration
   void (*Isr) (void)__irq;               //!< Isr handler function to call
};

//! Hardware descriptor data (read only) for each channel
struct _SPwmChannelRO
{
   UCHAR             ucChannelId;         //!< Bit-Id (!) of channel to handle
   U8                u8TskPriTx;          //!< Priority of tx task
   U16               u16MaxTx;            //!< Maximum number of transfer with a single cycle
   U16               u16DelayTx;          //!< Delay after each transfer cycle <b>(in ticks)</b>
};

//! <b>Internal data:</b> Runtime data (read/write) for each channel
struct _SPwmChannelRW
{
   OS_TID            tidTskTx;            //!< OS_TID of tx task
   OS_SEM            SemTx;               //!< Semaphore to lock tx data
   OS_SEM            SemDone;             //!< Semaphore to indicate all tx data sent yet
};

//! List structure item (for building list array struct _SPwmItem SPwmList[AT91F_PWMLIST_SIZE] placed in "at91sam7x.c")
/**           
   data flow thru SPwmList
   \dot
   digraph SPwmList {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Pwm [label="Pwm peripheral" shape="parallelogram"];
      PwmIrqHandler [label="PwmIrqHandler"];
      SPwmList [label="SPwmList" style="filled"];
      User [label="User irq handler(s)" shape="ellipse" style="dashed" color="dimgray" style="dashed"];

      Pwm -> PwmIrqHandler [arrowhead="odot"];
      PwmIrqHandler -> SPwmList;
      SPwmList -> User;
   }
   \enddot
   
      <b>Example code: the following array is REQUIRED in "at91sam7x.c" to re-direct the pwm interrupt to pwm's common channel irq handler</b><br>
      \include _SPwmItem.h
*/
struct _SPwmItem
{
   UCHAR uiMask;                          //!< <b>CHID</b> of pwm channel
   /**
         \param   u32ISR                     pwm peripheral InteruptStatusRegister with all interrupt triggering port bits set (refer to <a href="#_SPwmItem">AT91SAM7X manual 33.6.8 page 441</a>)
         \param   u32Pin                     pwm peripheral <b>CH</b>annel<b>ID</b> (refer to <a href="#_SPwmItem">AT91SAM7X manual 33.6.2 page 435</a>)
         
         \note
                  - for multiple pwm channels triggered at the same time you'll get MORE THAN ONE set bit for <b>u32ISR</b> for dependency checking<br>
                  
         <b>Manual reference(s):</b><br>
         <a name="_SPwmItem">
         <a href="../../../MANUAL/AT91/AT91SAM7X/PWM/AT91SAM7X_33.6.8_p441_PWM_ISR.pdf" type="application/pdf">AT91SAM7X manual 33.6.8 page 441: PWM_ISR</a><br>
         <a href="../../../MANUAL/AT91/AT91SAM7X/PWM/AT91SAM7X_33.6.2_p435_PWM_ENA.pdf" type="application/pdf">AT91SAM7X manual 33.6.2 page 435: PWM_ENA</a><br>
   */   
   void (*Isr) (U32 u32ISR, U32 u32Pin);  //!< Isr handler to call
};

#endif
