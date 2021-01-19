//!	data definitions of AT91SAM7X PIO driver
/**
		\file		piodata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PIO/PIODATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-09-10 initial version
// 2009-09-10 added AT91C_PIOATTR_PULLUPOFF
// 2009-09-16 changed AT91C_PIOATTR_OPENDRAIN to AT91C_PIOATTR_OPENDRAIN_ON
// 2009-09-16 added ..._OPENDRAIN_OFF, ..._DIRECTDRIVE_OFF, ..._DEGLITCH_OFF
// 2009-09-16 renamed AT91C_PIOATTR_DEFAULT to AT91C_PIOATTR_RESET
// 2009-09-16 changed AT91C_PIOATTR_RESET to a true action flag
// 2009-11-06 added irq callback function pointer to _SPioItem
// 2009-11-11 added enum EPioTrigger
// 2009-11-12 added args u32PDSR, u32Pin and bLevel to user isr functions
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-30 added piobase arg to call of isr handlers
// 2010-11-23 added/fixed data comments
// 2011-10-20 added public doxygen documentation
// 2011-11-23 added digraphs of data for public doxygen documentation
// 2016-03-02 added pin init handler callback after pin irq handler callback
// 2016-03-02 updated public doxygen documentation
// 2016-06-10 updated public doxygen documentation for _SPioItem (*Init)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PIO_PIODATA_H
#define SYKAM_CPU_AT91_PIO_PIODATA_H 1


//! Peripheral type
typedef enum
{
   AT91C_PIOTYPE_PERIPH,                     //!< The pin is acting as primary peripheral type
   AT91C_PIOTYPE_PERIPH_ALT,                 //!< The pin is acting as alternate peripheral type
   AT91C_PIOTYPE_INPUT,                      //!< The pin is an input
   AT91C_PIOTYPE_OUTPUT_SET0,                //!< The pin is an output and will assigned a level of 0
   AT91C_PIOTYPE_OUTPUT_SET1                 //!< The pin is an output and will assigned a level of 1
} EPioType;

//! Pin driver configration
typedef enum
{
   AT91C_PIOATTR_RESET           = (1 << 0), //!< Force default pin configuration (after reset like)
   AT91C_PIOATTR_OPENDRAIN_ON    = (1 << 1), //!< The pin is open-drain 
   AT91C_PIOATTR_OPENDRAIN_OFF   = (1 << 2), //!< The pin is push-pull (default after reset)
   AT91C_PIOATTR_PULLUP_ON       = (1 << 3), //!< The internal pin pull-up is active (default after reset)
   AT91C_PIOATTR_PULLUP_OFF      = (1 << 4), //!< The internal pin pull-up is inactive
   AT91C_PIOATTR_DIRECTDRIVE_ON  = (1 << 5), //!< Assigned for direct control of PIO outputs by single write access to PIO_ODSR
   AT91C_PIOATTR_DIRECTDRIVE_OFF = (1 << 6), //!< Not assigned for direct control of PIO outputs by single write access to PIO_ODSR (default after reset)
   AT91C_PIOATTR_DEGLITCH_ON     = (1 << 7), //!< The internal glitch filter is active
   AT91C_PIOATTR_DEGLITCH_OFF    = (1 << 8)  //!< The internal glitch filter is inactive (default after reset)
} EPioAttr;

//! Trigger type (for input notification by interrupts)
typedef enum
{
   AT91C_PIOTRG_ANY              = (1 << 0), //!< Call irq handler for any irq edge condition
   AT91C_PIOTRG_FALL             = (1 << 1), //!< Call irq handler for falling edge condition only
   AT91C_PIOTRG_RAISE            = (1 << 2)  //!< Call irq handler for raising edge condition only
} EPioTrigger;

//! Hardware descriptor data (read only) for each unit
struct _SPioRO
{
   AT91PS_PIO        pBase;                  //!< Base address of peripheral to handle
   U32               u32PeriphId;            //!< Bit-Id (!) of peripheral to handle
   struct _SPioItem *pSPioList;              //!< List of used pin's configuration
   UCHAR             ucPioListSize;          //!< Size of the list of used pin's configuration
   void (*Isr) (void)__irq;                  //!< Isr handler to call
};

//! List structure item (for building list array struct _SPioItem SPioListA/B[AT91F_PIOLIST_SIZEA/B] placed in "at91sam7x.c")
/**           
   data flow thru SPioList
   \dot
   digraph SPioList {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Pin [label="PIO peripheral" shape="parallelogram"];
      PioIrqHandler [label="PioIrqHandler"];
      SPioList [label="SPioList" style="filled"];
      User [label="User irq handler(s)" shape="ellipse" style="dashed" color="dimgray" style="dashed"];
      Init [label="Init handler(s)" shape="ellipse" style="dashed" color="dimgray" style="dashed"];

      Pin -> PioIrqHandler [arrowhead="odot"];
      PioIrqHandler -> SPioList;
      SPioList -> User;
      SPioList -> Init;
   }
   \enddot
   
      <b>Example code: the following array is REQUIRED in "at91sam7x.c"</b><br>
      \include    _SPioItem.h
*/
struct _SPioItem
{
   UINT        uiMask;                       //!< Pin mask (name) for the used pin's configuration list
   EPioType    EType;                        //!< Pin mode
   EPioTrigger ETrigger;                     //!< Irq trigger mode (see above)
   /**
         \param   u32ISR                     pio peripheral InteruptStatusRegister with all interrupt triggering port bits set (refer to <a href="#Isr">AT91SAM7X manual 27.6.17 page 248</a>)
         \param   u32PDSR                    pio peripheral PinDataStatusRegister with levels of port pins at interrupt's time (refer to <a href="#Isr">AT91SAM7X manual 27.6.13 page 246</a>)
         \param   u32Pin                     mask (pin's bit is set) of the pin (refer to <a href="#Isr">AT91SAM7X manual 27.6.17 page 248</a>)
         \param   bLevel                     level of the pin at interrupt's time
         \param   pPio                       base address of the used pio peripheral <b>[for easy re-reads of stable pin level]</b>
         
         \note
                  - this function is called from <b>interrupt context</b>, which allows <b>only interrupt system calls</b> here
                  - for multiple pins triggered at the same time you'll get MORE THAN ONE set bit for <b>u32ISR</b> and <b>u32PDSR</b> for easy dependency checking<br>
                  
         \remarks
                  - for easy re-reads of stable pin level <b>(WITHOUT any need for the handler to know HARDCODED the pio base and pin)</b> use the supplied arguments <b>pPio</b> and <b>u32Pin</b><br>
                  
      <b>Manual reference(s):</b><br>
      <a name="Isr">
      <a href="../../../MANUAL/AT91/AT91SAM7X/PIO/AT91SAM7X_27.6.17_p248_PIO_ISR.pdf" type="application/pdf">AT91SAM7X manual 27.6.17 page 248: PIO_ISR</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/PIO/AT91SAM7X_27.6.13_p246_PIO_PDSR.pdf" type="application/pdf">AT91SAM7X manual 27.6.13 page 246: PIO_PDSR</a><br>
   */
   void (*Isr) (U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio); //!< Pin interrupt handler to call (<b>interrupt context</b>)
   /**
         \param   u32Pin                     mask (pin's bit is set) of the pin (refer to <a href="#Init">AT91SAM7X manual 27.6.17 page 248</a>)
         \param   pPio                       base address of the used pio peripheral <b>[for easy re-reads of stable pin level]</b>
         
         \note
                  - return the value #TRUE (or using NULL for this handler) to enable the interrupt for this pin immediately (old behaviour)
                  - return the value #FALSE to disable the interrupt for this pin yet (control the interrupt later manually)
                  - <b>save</b> the supplied arguments to control later the interrupt of this pin by hardware (using <b>AT91F_PIO_InterruptEnable</b> & <b>AT91F_PIO_InterruptDisable</b>)
                  - the return value is ignored for omitted (<b>set to NULL</b>) interrupt handler

         \remarks
                  - this function is <b>called only when pin interrupt handler is present</b> for extended interrupt control option
                  
      <b>Manual reference(s):</b><br>
      <a name="Init">
      <a href="../../../MANUAL/AT91/AT91SAM7X/PIO/AT91SAM7X_27.6.17_p248_PIO_ISR.pdf" type="application/pdf">AT91SAM7X manual 27.6.17 page 248: PIO_ISR</a><br>
   */
   UCHAR (*Init) (U32 u32Pin, AT91PS_PIO pPio); //!< Pin init handler to call
   EPioAttr    EAttr;                        //!< Pin attributes (see above)
};

#endif
