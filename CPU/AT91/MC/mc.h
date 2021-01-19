//!	defines and configuration of AT91SAM7X MC driver
/**
		\file		mc.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/MC/MC.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-03-21 initial version
// 2013-03-28 updated doxygen documentation
// 2013-04-04 changed all numeric defines in formulas to type unsigned long
// 2013-04-04 enclosed all ternary conditional expressions in global brackets
// 2013-04-11 added define MC_IROM_BASE_LOW(rcb) for completeness & docs
// 2013-05-02 added MC_IS_IFLASH/IROM and MC_IS_ISRAM for easier address checking
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_MC_MC_H
#define SYKAM_CPU_AT91_MC_MC_H 1


// Macros

//! Base address of area number
#define MC_IAREA_BASE(x)   ((_min((x), 3UL))* MC_IAREA0_SIZE)

//! Size of area number in bytes
#define MC_IAREA_SIZE(x)   (((_min((x), 4)) > 3) ? (MC_IAREA4_SIZE) : (MC_IAREA0_SIZE))

//! Check for given mapping (rcb) the address (x) is inside the rom address range
#define MC_IS_IROM(rcb, x) ((((x) >= MC_IROM_BASE_LOW((rcb))) && ((x) < MC_IROM_BASE_LOW((rcb))+ AT91C_IFLASH_SIZE)) ? (TRUE) : (FALSE))

//! Check for given mapping (rcb) the address (x) is inside the flash address range
#define MC_IS_IFLASH(rcb, x) ((((x) >= MC_IFLASH_BASE_LOW((rcb))) && ((x) < MC_IFLASH_BASE_LOW((rcb))+ AT91C_IFLASH_SIZE)) ? (TRUE) : (FALSE))

//! Check for given mapping (rcb) the address (x) is inside the ram address range
#define MC_IS_ISRAM(rcb, x) ((((x) >= MC_ISRAM_BASE_LOW((rcb))) && ((x) < MC_ISRAM_BASE_LOW((rcb))+ AT91C_ISRAM_SIZE)) ? (TRUE) : (FALSE))

// Depending on the remap feature, either flash or sram is mirrored (_not_ moved/switched!) to area 0 base
// GPNVM2: 0= ROM, 1= FLASH (BOOT NonVolatileProgram TYPE)
// RCB: 0= FLASH, 1= SRAM (AREA0 MIRROR TYPE)

//! Lowest flash base address [remap (rcb) is <b>non</b>-active: rom mapped to standard <b>and</b> for GPNVM2= 0 'rom boot' (refer to AT91SAM7X manual 8.4-8.5 page 19-20) to lower mirror space, for <b>active</b>: rom mapped to standard space only]
#define MC_IROM_BASE_LOW(rcb)   ((!(rcb)) ? (MC_IREMAP_BASE) : (MC_IROM_BASE))

//! Lowest flash base address [remap (rcb) is <b>non</b>-active: flash mapped to standard <b>and</b> for GPNVM2= 1 'flash boot' (refer to AT91SAM7X manual 8.4-8.5 page 19-20) to lower mirror space, for <b>active</b>: flash mapped to standard space only]
#define MC_IFLASH_BASE_LOW(rcb)   ((!(rcb)) ? (MC_IREMAP_BASE) : (MC_IFLASH_BASE))

//! Lowest ram base address [remap (rcb) is <b>non</b>-active: sram mapped to standard space only, for <b>active</b>: sram mapped to standard <b>and</b> lower mirror space]
#define MC_ISRAM_BASE_LOW(rcb)    ((!(rcb)) ? (MC_ISRAM_BASE) : (MC_IREMAP_BASE))

// Defines

//! Base address of area 0
#define MC_IAREA0_BASE  0x00000000UL

//! Base address of area 1
#define MC_IAREA1_BASE  0x00100000UL

//! Base address of area 2
#define MC_IAREA2_BASE  0x00200000UL

//! Base address of area 3
#define MC_IAREA3_BASE  0x00300000UL

//! Base address of area 4
#define MC_IAREA4_BASE  0x00400000UL


//! Size of area 0 in bytes
#define MC_IAREA0_SIZE  0x00100000UL

//! Size of area 1 in bytes
#define MC_IAREA1_SIZE  0x00100000UL

//! Size of area 2 in bytes
#define MC_IAREA2_SIZE  0x00100000UL

//! Size of area 3 in bytes
#define MC_IAREA3_SIZE  0x00100000UL

//! Size of area 4 in bytes
#define MC_IAREA4_SIZE  0x0FC00000UL


//! Base address of remap (mirror) area
#define MC_IREMAP_BASE  MC_IAREA0_BASE

//! Base address of flash area
#define MC_IFLASH_BASE  AT91C_IFLASH

//! Base address of ram area
#define MC_ISRAM_BASE   AT91C_ISRAM

//! Base address of rom area
#define MC_IROM_BASE    MC_IAREA3_BASE

//! Base address of undefined area
#define MC_UNDEF_BASE   MC_IAREA4_BASE

//----------------------------------------------------------------------------
// Do *NOT* apply application specific changes here!
// Set defines to change in your application before including this to override
//----------------------------------------------------------------------------



// Prototypes


// Each supported unit requires a prototype definition here


#endif
