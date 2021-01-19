//!	data definitions of MCP2317 peripheral driver
/**
		\file		mcp2317data.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/GPIO/MCP2317/MCP2317DATA.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-12-18 initial version
// 2009-12-21 added struct _SMcp2317Tsk
// 2010-02-17 added u16Mcp2317Reg to _SMcp2317RW
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-11-23 added/fixed data comments
// 2010-12-02 added bIrq flag to _SMcp2317RW
// 2011-02-16 added semaphore SemStart to _SMcp2317RW
// 2011-03-17 added u8TskNotifyHelperMcp2317 to _SMcp2317RO
// 2011-03-17 added tidTskNotifyHelperMcp2317 to _SMcp2317RW
// 2011-04-14 added u16DelayIrqQuery to _SMcp2317RO
// 2011-04-14 added bMcp2317RegValid to _SMcp2317RW
// 2011-11-03 added public doxygen documentation
// 2011-11-24 added internal doxygen documentation
// 2013-08-22 changed delays/timeouts from milliseconds to ticks
// 2016-03-02 added struct _SMcp2317Irq
// 2016-03-02 added struct _SMcp2317Irq to struct _SMcp2317RW
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_GPIO_MCP2317_MCP2317DATA_H
#define SYKAM_EXT_SPI_GPIO_MCP2317_MCP2317DATA_H


//! Peripheral type
typedef enum
{
   MCP2317_TYPE_INPUT,                       //!< The pin is an input
   MCP2317_TYPE_OUTPUT_SET0,                 //!< The pin is an output and will set to level of 0
   MCP2317_TYPE_OUTPUT_SET1                  //!< The pin is an output and will set to level of 1
} EMcp2317Type;

//! Pin driver configration
typedef enum
{
   MCP2317_ATTR_RESET           = (1 << 0),  //!< Force default pin configuration (after reset like)
   MCP2317_ATTR_PULLUP_ON       = (1 << 1),  //!< The internal pin pull-up is active
   MCP2317_ATTR_PULLUP_OFF      = (1 << 2),  //!< The internal pin pull-up is inactive (default after reset)
   MCP2317_ATTR_INVERTER_ON     = (1 << 3),  //!< The internal inverter is active
   MCP2317_ATTR_INVERTER_OFF    = (1 << 4)   //!< The internal inverter is inactive (default after reset)
} EMcp2317Attr;

//! Trigger type (for input notification by interrupts)
typedef enum
{
   MCP2317_TRG_ANY              = (1 << 0),  //!< Call irq handler for <b>any irq edge</b> condition
   MCP2317_TRG_FALL             = (1 << 1),  //!< Call irq handler for <b>falling edge</b> condition only
   MCP2317_TRG_RAISE            = (1 << 2)   //!< Call irq handler for <b>raising edge</b> condition only
} EMcp2317Trigger;

//! \internal \brief <b>Internal data:</b> Task parameter data
struct _SMcp2317Tsk
{
   UCHAR ucUnit;                             //!< Unit task is assigned to
};

//! Hardware descriptor data (read only) for each unit
struct _SMcp2317RO
{
   UCHAR                ucUnit;              //!< Unit of the spi controller connected to
   UCHAR                ucChannel;           //!< Channel of the spi controller connected to
   USHORT               usBlockSizeMax;      //!< Maximum block length in bytes which can be transfered within a single chip select cycle
   ULONG                ulSCBR;              //!< Serial data clock speed
   U8                   u8HAID;              //!< Ha(rdware) (id) of the device (address pins of circuit)
   U8                   u8IOCON;             //!< IO (con)figuration of the device (refer to data sheet)
   struct _SMcp2317Item *pSMcp2317List;      //!< List of used pin's configuration
   UCHAR                ucMcp2317ListSize;   //!< Size of the list of used pin's configuration
   UCHAR                u8TskNotifyMcp2317;  //!< Priority of the notification task
   UCHAR                u8TskNotifyHelperMcp2317; //!< Priority of the notification helper task
   U16                  u16DelayIrqQuery;    //!< Delay before irq query service
   U16                  u16Timeout;          //!< Maximum cpu-load free resource pending time before any resource claim is canceled by the rtos <b>(in ticks)</b>
};

//! <b>Internal data:</b> Interrupt data of port pin connected to
struct _SMcp2317Irq
{
   U32                              u32Pin;  //!< Pio port pin mask
   AT91PS_PIO                       pPio;    //!< Pio base address
};

//! <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SMcp2317RW
{
   struct _SMcp2317Irq  SIrq;                //!< Interrupt data of port pin connected to
   OS_ID                oiSemMsgNew;         //!< OS_ID of the semaphore to give for any new message
   OS_SEM               SemStart;            //!< Semaphore to lock device
   OS_TID               tidTskNotifyMcp2317; //!< OS_TID of the created notification task
   OS_TID               tidTskNotifyHelperMcp2317; //!< OS_TID of the created notification helper task
   U16                  u16Mcp2317Reg[MCP2317_REG_COUNT/ 2]; //!< Local register cache
   bool                 bMcp2317RegValid;    //!< Valid flag of local register cache
   bool                 bIrq;                //!< Enable flag for irq
};

//! List structure item (for building list array struct _SMcp2317Item[MCP2317_LIST_SIZE] placed in "at91sam7x.c")
/**           
      <b>Example code: the following array is REQUIRED in "at91sam7x.c"</b><br>
      \include _SMcp2317Item.h
*/
struct _SMcp2317Item
{
   USHORT usMask;                            //!< Pin mask (name) for the used pin's configuration list
   EMcp2317Type EType;                       //!< Pin mode (input/output)
   EMcp2317Trigger ETrigger;                 //!< Irq trigger mode (see above)
   /**
      \param   u16INTF                       mcp2317 register INTF (INTERRUPT FLAG REGISTER) with all interrupt triggering port bits set
      \param   u16INTCAP                     mcp2317 register INTCAP (INTERRUPT CAPTURED VALUE FOR PORT REGISTER) with levels of port pins at interrupt's time
      \param   u16Pin                        mask of the pin (pin's bit is set)
      \param   bLevel                        level of the pin at interrupt's time

      \note
               - for multiple pins triggered at the same time you'll get MORE THAN ONE set bit for <b>u16INTF</b> and <b>u16INTCAP</b> for dependency checking<br>
   */
   void (*Notify) (U16 u16INTF, U16 u16INTCAP, U16 u16Pin, bool bLevel); //!< Handler function to call
   EMcp2317Attr EAttr;                       //!< Pin attributes (see above)
};

#endif
