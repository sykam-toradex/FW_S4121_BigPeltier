//!	defines and configuration of MCP2317 peripheral driver
/**
		\file		mcp2317.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/GPIO/MCP2317/MCP2317.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-11-11 initial version (extracted from evaluation.c)
// 2009-11-11 added HAID definitions
// 2009-12-21 moved basic configuration defines to board specific include file
// 2009-12-22 added MCP2317_TAG_... definitions for "coloured" spi rx packets
// 2009-12-23 added board\AT91SAM7X.h for board specific configuration
// 2009-12-23 added overrideable define MCP2317_STACK_TSKNOTIFYMCP2317
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-09-30 added piobase para to Mcp2317Irq- and handler
// 2010-12-02 increased MCP2317_STACK_TSKNOTIFYMCP2317 to 1536
// 2011-01-19 increased MCP2317_STACK_TSKNOTIFYMCP2317 to 2048
// 2011-03-17 added new task TskNotifyHelperMcp2317 for safer user func calls
// 2011-03-17 added new define MCP2317_STACK_TSKNOTIFYHELPERMCP2317 at 2048
// 2011-03-17 added new define MCP2317_MBXCNTNOTIFYHELPERMCP2317 at 20
// 2011-03-17 reduced MCP2317_STACK_TSKNOTIFYMCP2317 to 1024
// 2011-03-17 increased MCP2317_STACK_TSKNOTIFYMCP2317 to 1536
// 2011-11-03 added public doxygen documentation
// 2011-11-30 added internal documentation
// 2016-03-02 added macros and prototype for InitMcp2317Handler
// 2016-03-02 renamed Mcp2317IrqHandler to IsrMcp2317IrqHandler (new name style)
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_GPIO_MCP2317_MCP2317_H
#define SYKAM_EXT_SPI_GPIO_MCP2317_MCP2317_H


// Macros

// Function template IsrMcp2317
#define MCP2317_ISR_FUNC(unit) void IsrMcp2317_##unit(U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio) { IsrMcp2317IrqHandler(unit, u32ISR, u32PDSR, u32Pin, bLevel, pPio); }

// Function prototype IsrMcp2317
#define MCP2317_ISR_PROTO(unit) void IsrMcp2317_##unit(U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio)

// Function name IsrMcp2317
#define MCP2317_ISR_NAME(unit) IsrMcp2317_##unit

// Function template InitMcp2317
#define MCP2317_INIT_FUNC(unit)   bool InitMcp2317_##unit(U32 u32Pin, AT91PS_PIO pPio) { return(InitMcp2317Handler(unit, u32Pin, pPio)); }

// Function prototype InitMcp2317
#define MCP2317_INIT_PROTO(unit)  bool InitMcp2317_##unit(U32 u32Pin, AT91PS_PIO pPio)

// Function name InitMcp2317
#define MCP2317_INIT_NAME(unit)   InitMcp2317_##unit


// Pack and return MCP2317_HAID_... and MCP2317_ACCESS_...
#define MCP2317_ACCESS(haen, read)  (0x40 | (((haen) << 1) & 0x0E) | ((read) & 0x01))


// Defines

//! <b>Hardware ID:</b> 0
#define MCP2317_HAID_0           (0)

//! <b>Hardware ID:</b> 1
#define MCP2317_HAID_1           (1)

//! <b>Hardware ID:</b> 2
#define MCP2317_HAID_2           (2)

//! <b>Hardware ID:</b> 3
#define MCP2317_HAID_3           (3)

//! <b>Hardware ID:</b> 4
#define MCP2317_HAID_4           (4)

//! <b>Hardware ID:</b> 5
#define MCP2317_HAID_5           (5)

//! <b>Hardware ID:</b> 6
#define MCP2317_HAID_6           (6)

//! <b>Hardware ID:</b> 7
#define MCP2317_HAID_7           (7)


//! <b>Access mode:</b> read
#define MCP2317_ACCESS_READ      (1)

//! <b>Access mode:</b> write
#define MCP2317_ACCESS_WRITE     (0)


// IOCON configuration bits (refer to mcp2317 1.6.6 page 17-18 manual AND annotation an1043 for further details)

//! <b>IOCON BANK:</b> Controls how the registers are addressed (1 == The registers associated with each port are separated into different banks, 0 == The registers are in the same bank (addresses are sequential))
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_1.6.6_p17-18_IOCON.pdf" type="application/pdf">MCP2317 1.6.6 page 17: IOCON</a><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_an1043_p2-8.pdf" type="application/pdf">MCP2317 annotation AN1043</a><br>
*/
#define MCP2317_IOCON_BANKSPLIT  (1 << 7)

//! <b>IOCON MIRROR:</b> INT Pins Mirror bit (1 == The INT pins are internally connected, 0 == The INT pins are not connected. INTA is associated with PortA and INTB is associated with PortB)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_1.6.6_p17-18_IOCON.pdf" type="application/pdf">MCP2317 1.6.6 page 17: IOCON</a><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_an1043_p2-8.pdf" type="application/pdf">MCP2317 annotation AN1043</a><br>
*/
#define MCP2317_IOCON_INTMIRROR  (1 << 6)

//! <b>IOCON SEQOP:</b> Sequential Operation mode bit (1 == Sequential operation disabled, address pointer does not increment, 0 == Sequential operation enabled, address pointer increments)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_1.6.6_p17-18_IOCON.pdf" type="application/pdf">MCP2317 1.6.6 page 17: IOCON</a><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_an1043_p2-8.pdf" type="application/pdf">MCP2317 annotation AN1043</a><br>
*/
#define MCP2317_IOCON_NSEQOP     (1 << 5)

//! <b>IOCON DISSLW:</b> Slew Rate control bit for SDA output (1 == Slew rate disabled, 0 == Slew rate enabled)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_1.6.6_p17-18_IOCON.pdf" type="application/pdf">MCP2317 1.6.6 page 17: IOCON</a><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_an1043_p2-8.pdf" type="application/pdf">MCP2317 annotation AN1043</a><br>
*/
#define MCP2317_IOCON_NDISSLW    (1 << 4)

//! <b>IOCON HAEN:</b> Hardware Address Enable bit (MCP23S17 only). Address pins are always enabled on MCP23017. (1 == Enables the MCP23S17 address pins, 0 == Disables the MCP23S17 address pins)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_1.6.6_p17-18_IOCON.pdf" type="application/pdf">MCP2317 1.6.6 page 17: IOCON</a><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_an1043_p2-8.pdf" type="application/pdf">MCP2317 annotation AN1043</a><br>
*/
#define MCP2317_IOCON_HAEN       (1 << 3)

//! <b>IOCON ODR:</b> This bit configures the INT pin as an open-drain output (1 == Open-drain output (overrides the INTPOL bit), 0 == Active driver output (INTPOL bit sets the polarity))
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_1.6.6_p17-18_IOCON.pdf" type="application/pdf">MCP2317 1.6.6 page 17: IOCON</a><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_an1043_p2-8.pdf" type="application/pdf">MCP2317 annotation AN1043</a><br>
*/
#define MCP2317_IOCON_ODR        (1 << 2)

//! <b>IOCON INTPOL:</b> This bit sets the polarity of the INT output pin (1 == Active-high, 0 == Active-low)
/**
      <b>Manual reference(s):</b><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_1.6.6_p17-18_IOCON.pdf" type="application/pdf">MCP2317 1.6.6 page 17: IOCON</a><br>
      <a href="../../../MANUAL/EXT/SPI/GPIO/MCP2317/MCP2317_an1043_p2-8.pdf" type="application/pdf">MCP2317 annotation AN1043</a><br>
*/
#define MCP2317_IOCON_INTPOL     (1 << 1)


//! <b>Bank mode:</b> The registers are in the same bank (addresses are sequential) [interleaved]
#define MCP2317_REG_NOBANKSPLIT  (0)

//! <b>Bank mode:</b> The registers associated with each port are separated into different banks [block by block]
#define MCP2317_REG_BANKSPLIT    (1)


// Register address definition for specified bank mode of FIRST 8 port line bits
#define MCP2317_REG_IODIRA(split)     (split) ? (0x00) : (0x00)
#define MCP2317_REG_IPOLA(split)      (split) ? (0x01) : (0x02)
#define MCP2317_REG_GPINTENA(split)   (split) ? (0x02) : (0x04)
#define MCP2317_REG_DEFVALA(split)    (split) ? (0x03) : (0x06)
#define MCP2317_REG_INTCONA(split)    (split) ? (0x04) : (0x08)
#define MCP2317_REG_IOCONA(split)     (split) ? (0x05) : (0x0A)
#define MCP2317_REG_GPPUA(split)      (split) ? (0x06) : (0x0C)
#define MCP2317_REG_INTFA(split)      (split) ? (0x07) : (0x0E)
#define MCP2317_REG_INTCAPA(split)    (split) ? (0x08) : (0x10)
#define MCP2317_REG_GPIOA(split)      (split) ? (0x09) : (0x12)
#define MCP2317_REG_OLATA(split)      (split) ? (0x0A) : (0x14)

// Register address definition for specified bank mode of SECOND 8 port line bits
#define MCP2317_REG_IODIRB(split)     (split) ? (0x10) : (0x01)
#define MCP2317_REG_IPOLB(split)      (split) ? (0x11) : (0x03)
#define MCP2317_REG_GPINTENB(split)   (split) ? (0x12) : (0x05)
#define MCP2317_REG_DEFVALB(split)    (split) ? (0x13) : (0x07)
#define MCP2317_REG_INTCONB(split)    (split) ? (0x14) : (0x09)
#define MCP2317_REG_IOCONB(split)     (split) ? (0x15) : (0x0B)
#define MCP2317_REG_GPPUB(split)      (split) ? (0x16) : (0x0D)
#define MCP2317_REG_INTFB(split)      (split) ? (0x17) : (0x0F)
#define MCP2317_REG_INTCAPB(split)    (split) ? (0x18) : (0x11)
#define MCP2317_REG_GPIOB(split)      (split) ? (0x19) : (0x13)
#define MCP2317_REG_OLATB(split)      (split) ? (0x1A) : (0x15)


//! <b>Port line:</b> A0
#define MCP2317_PA0  (1 << 0)

//! <b>Port line:</b> A1
#define MCP2317_PA1  (1 << 1)

//! <b>Port line:</b> A2
#define MCP2317_PA2  (1 << 2)

//! <b>Port line:</b> A3
#define MCP2317_PA3  (1 << 3)

//! <b>Port line:</b> A4
#define MCP2317_PA4  (1 << 4)

//! <b>Port line:</b> A5
#define MCP2317_PA5  (1 << 5)

//! <b>Port line:</b> A6
#define MCP2317_PA6  (1 << 6)

//! <b>Port line:</b> A7
#define MCP2317_PA7  (1 << 7)


//! <b>Port line:</b> B0 [<b>NO</b> bank split: addresses are sequential <b>AND</b> registers are 16 bit wide]
#define MCP2317_PB0  (MCP2317_PA0 << 8)

//! <b>Port line:</b> B1 [<b>NO</b> bank split: addresses are sequential <b>AND</b> registers are 16 bit wide]
#define MCP2317_PB1  (MCP2317_PA1 << 8)

//! <b>Port line:</b> B2 [<b>NO</b> bank split: addresses are sequential <b>AND</b> registers are 16 bit wide]
#define MCP2317_PB2  (MCP2317_PA2 << 8)

//! <b>Port line:</b> B3 [<b>NO</b> bank split: addresses are sequential <b>AND</b> registers are 16 bit wide]
#define MCP2317_PB3  (MCP2317_PA3 << 8)

//! <b>Port line:</b> B4 [<b>NO</b> bank split: addresses are sequential <b>AND</b> registers are 16 bit wide]
#define MCP2317_PB4  (MCP2317_PA4 << 8)

//! <b>Port line:</b> B5 [<b>NO</b> bank split: addresses are sequential <b>AND</b> registers are 16 bit wide]
#define MCP2317_PB5  (MCP2317_PA5 << 8)

//! <b>Port line:</b> B6 [<b>NO</b> bank split: addresses are sequential <b>AND</b> registers are 16 bit wide]
#define MCP2317_PB6  (MCP2317_PA6 << 8)

//! <b>Port line:</b> B7 [<b>NO</b> bank split: addresses are sequential <b>AND</b> registers are 16 bit wide]
#define MCP2317_PB7  (MCP2317_PA7 << 8)


// Size of port line configuration list
#define MCP2317_LIST_SIZE  (16)

// Count of cached registers
#define MCP2317_REG_COUNT  (22)


// Transfer tag definitions for spi
#define MCP2317_TAG_INIT      (0x01)
#define MCP2317_TAG_SETOUTPUT (0xA1)
#define MCP2317_TAG_ISRQUERY  (0xF1)


//! <b>Overrideable configuration:</b> default stack size for TskNotifyMcp2317
#ifndef MCP2317_STACK_TSKNOTIFYMCP2317
   #define MCP2317_STACK_TSKNOTIFYMCP2317 (1536)
#endif


//! <b>Overrideable configuration:</b> default stack size for TskNotifyHelperMcp2317
#ifndef MCP2317_STACK_TSKNOTIFYHELPERMCP2317
   #define MCP2317_STACK_TSKNOTIFYHELPERMCP2317 (2048)
#endif

//! <b>Overrideable configuration:</b>  number of messages buffered for TskNotifyMcp2317 to TskNotifyHelperMcp2317
#ifndef MCP2317_MBXCNTNOTIFYHELPERMCP2317
   #define MCP2317_MBXCNTNOTIFYHELPERMCP2317 (20)
#endif

//! <b>Overrideable configuration:</b> number of units supported simultaneously
#ifndef MCP2317_UNIT_MAX
   #define MCP2317_UNIT_MAX (1)
#endif

//! <b>Overrideable configuration:</b> maximum block length in bytes which can be transfered within a single chip select cycle
#ifndef MCP2317_BLOCKSIZEMAX
   #define MCP2317_BLOCKSIZEMAX  (24)
#endif

// Each supported usart unit requires a prototype definition here
#if MCP2317_UNIT_MAX > 0
   MCP2317_ISR_PROTO(0);
   MCP2317_INIT_PROTO(0);
#endif

#if MCP2317_UNIT_MAX > 1
   MCP2317_ISR_PROTO(1);
   MCP2317_INIT_PROTO(1);
#endif

// Prototypes
__task void TskNotifyMcp2317(void *pvParam);
__task void TskNotifyHelperMcp2317(void *pvParam);
void IsrMcp2317IrqHandler(UCHAR ucUnit, U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio);
bool InitMcp2317Handler(UCHAR ucUnit, U32 u32Pin, AT91PS_PIO pPio);

#endif
