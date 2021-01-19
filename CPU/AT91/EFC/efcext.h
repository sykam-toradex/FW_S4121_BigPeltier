//!	external interface of AT91SAM7X EFC driver
/**
		\file		efcext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/EFC/EFCEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-02-28 initial version
// 2013-03-07 fixed public doxygen documentation
// 2013-03-13 updated linked pdf documentation
// 2013-03-28 extended EfcLocate to accept address or absolute page number
// 2013-03-28 added bitmap retrieve functions for GPNVM and locks
// 2013-04-03 updated public doxygen documentation
// 2013-04-11 fixed public doxygen documentation
// 2013-11-20 added example to EfcPageOpen and EfcPageClose
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_EFC_EFCEXT_H
#define SYKAM_CPU_AT91_EFC_EFCEXT_H 1


// Prototypes for functions

//!   init a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)

      \return										> 0 for success, == 0 for failure

      \note
               - 1 task is spanned by this function with a stack size defined by #EFC_STACK_TSKMSG<br>
               - message bits are composed of filtered (#EFC_IRQ_MASK) hardware bits (refer to <a href="#EfcInit">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>)<br>
               - GPNVM bits are <b>permanently</b> set/cleared to the values specified in efcdev.h (refer to <a href="#EfcInit">AT91SAM7X manual 8.5.4 - 8.7. page 21-23: GPNVM</a>)<br>
               - LOCK bits are <b>permanently</b> set/cleared to the values specified in efcdev.h (refer to <a href="#EfcInit">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a>)<br>
               
      <b>Manual reference(s):</b><br>
      <a name="EfcInit">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.3.3_p118_MC_FSR.pdf" type="application/pdf">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/MC/AT91SAM7X_8.5.4-8.7_p21-23_GPNVM.pdf" type="application/pdf">AT91SAM7X manual 8.5.4 - 8.7. page 21-23: GPNVM</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
*/
UCHAR EfcInit(UCHAR ucUnit);

//!   retrieve message counters of a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param	ulMsg				            message bits to retrieve (refer to <a href="#EfcMsg">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>)
      \param	usDecrement				      decrement count (0 for no decrement) for selected message bits <b>(var type's max value USHRT_MAX* just clears the counter(s))</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										summarized count of all selected message bits <b>before decrementing</b>

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max<br>
               - message bits are composed of filtered (#EFC_IRQ_MASK) hardware bits (refer to <a href="#EfcMsg">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>)<br>
               
      \remarks
               <b>this function is helpful to</b><br>
               - determine the reason for the sent/given "news semaphore" when more than one message is masked
               
      <br><b>*)</b> refer to .\\Keil\\ARM\\RV31\\Inc\\limits.h for compiler type limit definitions
      
      <b>Manual reference(s):</b><br>
      <a name="EfcMsg">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.3.3_p118_MC_FSR.pdf" type="application/pdf">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a><br>
*/
USHORT EfcMsg(UCHAR ucUnit, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout);

//!   check efc is ready
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)

      \return										> 0 for success, == 0 for failure
*/
bool EfcIsReady(UCHAR ucUnit);

//!   check efc for error
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)

      \return										> 0 for pending error (AT91C_MC_LOCKE, AT91C_MC_PROGE), == 0 for no pending error
*/
UCHAR EfcIsError(UCHAR ucUnit);

//!   pend for new efc message (ready/error) to pend cpu-load free after a new efc command
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
*/
bool EfcMsgWait(UCHAR ucUnit, U16 u16Timeout);

//!   set general purpose non volatile memory bit(s)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   u16GPNVMSGPB               general purpose non volatile memory set bit mask <b>(only upper 8 bits are used</b>, refer to <a href="#EfcGPNVMSet">AT91SAM7X manual 8.5.4 - 8.7. page 21-23: GPNVM</a>)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success (also for zero bit mask), == 0 for failure

      \note
               - the u16GPNVMSGPB bits are expected right justified <b>but starting at rightmost bit #AT91C_MC_GPNVM0</b> (refer to <a href="#EfcGPNVMSet">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>)
        
      <b>Manual reference(s):</b><br>
      <a name="EfcGPNVMSet">
      <a href="../../../MANUAL/AT91/AT91SAM7X/MC/AT91SAM7X_8.5.4-8.7_p21-23_GPNVM.pdf" type="application/pdf">AT91SAM7X manual 8.5.4 - 8.7. page 21-23: GPNVM</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.3.3_p118_MC_FSR.pdf" type="application/pdf">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a><br>
*/
bool EfcGPNVMSet(UCHAR ucUnit, U16 u16GPNVMSGPB, U16 u16Timeout);

//!   clear general purpose non volatile memory bit(s)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   u16GPNVMCGPB               general purpose non volatile memory set bit mask <b>(only upper 8 bits are used</b>, refer to <a href="#EfcGPNVMClear">AT91SAM7X manual 8.5.4 - 8.7. page 21-23: GPNVM</a>)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success (also for zero bit mask), == 0 for failure

      \note
               - the u16GPNVMCGPB bits are expected right justified <b>but starting at rightmost bit #AT91C_MC_GPNVM0</b> (refer to <a href="#EfcGPNVMClear">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>)

      <b>Manual reference(s):</b><br>
      <a name="EfcGPNVMClear">
      <a href="../../../MANUAL/AT91/AT91SAM7X/MC/AT91SAM7X_8.5.4-8.7_p21-23_GPNVM.pdf" type="application/pdf">AT91SAM7X manual 8.5.4 - 8.7. page 21-23: GPNVM</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.3.3_p118_MC_FSR.pdf" type="application/pdf">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a><br>
*/
bool EfcGPNVMClear(UCHAR ucUnit, U16 u16GPNVMCGPB, U16 u16Timeout);

//!   set lock bit(s)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   u32LOCKSLB                 lock set bit mask <b>(only upper 16 bits are used</b>, refer to <a href="#EfcLockSet">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a>)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success (also for zero bit mask), == 0 for failure

      \note
               - the u32LOCKSLB bits are expected right justified <b>but starting at rightmost bit #AT91C_MC_LOCKS0</b> (refer to <a href="#EfcLockSet">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>)

      <b>Manual reference(s):</b><br>
      <a name="EfcLockSet">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.3.3_p118_MC_FSR.pdf" type="application/pdf">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a><br>
*/
bool EfcLockSet(UCHAR ucUnit, U32 u32LOCKSLB, U16 u16Timeout);

//!   clear lock bit(s)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   u32LOCKCLB                 lock clear bit mask <b>(only upper 16 bits are used</b>, refer to <a href="#EfcLockClear">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a>)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success (also for zero bit mask), == 0 for failure

      \note
               - the u32LOCKCLB bits are expected right justified <b>but starting at rightmost bit #AT91C_MC_LOCKS0</b> (refer to <a href="#EfcLockClear">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>)

      <b>Manual reference(s):</b><br>
      <a name="EfcLockClear">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.3.3_p118_MC_FSR.pdf" type="application/pdf">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a><br>
*/
bool EfcLockClear(UCHAR ucUnit, U32 u32LOCKCLB, U16 u16Timeout);

//!   \internal \brief change non volatile bit(s) (general purpose non volatile memory / lock region)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   u32Mask                    bit mask for specified command <b>(only upper 8/16 bits are used, depening on u8Cmd</b>, refer to <a href="#EfcBitChange">AT91SAM7X manual links (GPNVM and LOCKS)</a>)
      \param   u8Cmd                      command for set (#AT91C_MC_FCMD_SET_GP_NVM / #AT91C_MC_FCMD_LOCK) or clear (#AT91C_MC_FCMD_CLR_GP_NVM / #AT91C_MC_FCMD_UNLOCK) bit(s)
      \param   u32Max                     highst valid bit number (0, 1, 2, 3, ...) for interration limiting
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success (also for zero bit mask), == 0 for failure

      \note
               - the u32Mask bits are expected right justified <b>but starting at rightmost bit #AT91C_MC_GPNVM0 or #AT91C_MC_LOCKS0</b>, depening on u8Cmd
               
      \remark
               - for the bit alignment refer to <a href="#EfcBitChange">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>

      <b>Manual reference(s):</b><br>
      <a name="EfcBitChange">
      <a href="../../../MANUAL/AT91/AT91SAM7X/MC/AT91SAM7X_8.5.4-8.7_p21-23_GPNVM.pdf" type="application/pdf">AT91SAM7X manual 8.5.4 - 8.7. page 21-23: GPNVM</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.3.3_p118_MC_FSR.pdf" type="application/pdf">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a><br>
*/
bool EfcBitChange(UCHAR ucUnit, U32 u32Mask, U8 u8Cmd, U32 u32Max, U16 u16Timeout);

//!   get GPNVM bits
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   pu32Map                    (optional, set NULL for default) storage for retrieved bits <b>(only upper 8 bits are used</b>, refer to <a href="#EfcGPNVMMapGet">AT91SAM7X manual 8.5.4 - 8.7. page 21-23: GPNVM</a>)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - the retrieved bits are returned right justified <b>but starting at rightmost bit #AT91C_MC_GPNVM0</b> (refer to <a href="#EfcGPNVMMapGet">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>)
               
      <b>Manual reference(s):</b><br>
      <a name="EfcGPNVMMapGet">
      <a href="../../../MANUAL/AT91/AT91SAM7X/MC/AT91SAM7X_8.5.4-8.7_p21-23_GPNVM.pdf" type="application/pdf">AT91SAM7X manual 8.5.4 - 8.7. page 21-23: GPNVM</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.3.3_p118_MC_FSR.pdf" type="application/pdf">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a><br>
*/
bool EfcGPNVMMapGet(UCHAR ucUnit, U32 *pu32Map, U16 u16Timeout);

//!   get LOCK bits
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   pu32Map                    (optional, set NULL for default) storage for retrieved bits <b>(only upper 16 bits are used</b>, refer to <a href="#EfcLockMapGet">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a>)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - the retrieved bits are returned right justified <b>but starting at rightmost bit #AT91C_MC_LOCKS0</b> (refer to <a href="#EfcLockMapGet">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>)

      <b>Manual reference(s):</b><br>
      <a name="EfcLockMapGet">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.3.3_p118_MC_FSR.pdf" type="application/pdf">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a><br>
*/
bool EfcLockMapGet(UCHAR ucUnit, U32 *pu32Map, U16 u16Timeout);

//!   get specified non volatile bits of type GPNVM or LOCK
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   u8Cmd                      command to identify bit type GPNVM (#AT91C_MC_FCMD_SET_GP_NVM / #AT91C_MC_FCMD_LOCK) or LOCK (#AT91C_MC_FCMD_CLR_GP_NVM / #AT91C_MC_FCMD_UNLOCK) bit(s), refer to <a href="#EfcBitMapGet">AT91SAM7X manual links (GPNVM and LOCKS)</a>)
      \param   pu32Map                    (optional, set NULL for default) storage for retrieved bits <b>(only upper 8/16 bits are used, depening on u8Cmd)</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure

      \note
               - the retrieved bits are returned right justified <b>but starting at rightmost bit #AT91C_MC_GPNVM0 or #AT91C_MC_LOCKS0</b>, depening on u8Cmd
               
      \remark
               - for the bit alignment refer to <a href="#EfcBitMapGet">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a>

      <b>Manual reference(s):</b><br>
      <a name="EfcBitMapGet">
      <a href="../../../MANUAL/AT91/AT91SAM7X/MC/AT91SAM7X_8.5.4-8.7_p21-23_GPNVM.pdf" type="application/pdf">AT91SAM7X manual 8.5.4 - 8.7. page 21-23: GPNVM</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.3.3_p118_MC_FSR.pdf" type="application/pdf">AT91SAM7X manual 19.3.3 page 118: MC_FSR</a><br>
*/
bool EfcBitMapGet(UCHAR ucUnit, U8 u8Cmd, U32 *pu32Map, U16 u16Timeout);

//!   open a flash page for modification or complete replacement by (start/end/inside page) address
/**
      \param   bRCB                       value of the non-readable RCB bit in the MC_RCR register of the MemoryController
      \param   ucFlags                    flags to enable flash actions (#EFC_OPEN_CLEAR, #EFC_OPEN_COPY, #EFC_OPEN_UNLOCK),<br>refer to <a href="#EfcPageOpenByAddress">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a>)
      \param   u32Arg                     argument (page start/end/inside address) to open <b>(there can only one page opened at a time)</b>      
      \param	*pucUnit					      (optional, set NULL for default) storage for the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   **pu32Start 	            (optional, set NULL for default) storage for start address of the page for access after opening<br>(<b>write access is 32 bit only!!!</b>, refer to <a href="#EfcPageOpenByAddress">AT91SAM7X manual 19.2.4.1 page 109: WR_BUF</a>)
      \param   **pu32End   	            (optional, set NULL for default) storage for end address of the page for access after opening<br>(<b>write access is 32 bit only!!!</b>, refer to <a href="#EfcPageOpenByAddress">AT91SAM7X manual 19.2.4.1 page 109: WR_BUF</a>)
      \param   *pusPageSize               (optional, set NULL for default) storage for page size of the unit in bytes
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)     
      
      \return										> 0 for success (number of storages updated), == 0 for failure

      \remarks
               - this function allocates the <b>single</b> write only hardware buffer
               
      <b>Manual reference(s):</b><br>
      <a name="EfcPageOpenByAddress">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.4.1_p109_WR_BUF.pdf" type="application/pdf">AT91SAM7X manual 19.2.4.1 page 109: WR_BUF</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
*/
UCHAR EfcPageOpenByAddress(bool bRCB, UCHAR ucFlags, U32 u32Arg, UCHAR *pucUnit, U32 **pu32Start, U32 **pu32End, USHORT *pusPageSize, U16 u16Timeout);

//!   open a flash page for modification or complete replacement by absolute page number 
/**
      \param   bRCB                       value of the non-readable RCB bit in the MC_RCR register of the MemoryController
      \param   ucFlags                    flags to enable flash actions (#EFC_OPEN_CLEAR, #EFC_OPEN_COPY, #EFC_OPEN_UNLOCK),<br>refer to <a href="#EfcPageOpenByPageAbs">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a>)
      \param   u32Arg                     argument (absolute page number) to open <b>(there can only one page opened at a time)</b>      
      \param	*pucUnit					      (optional, set NULL for default) storage for the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   **pu32Start 	            (optional, set NULL for default) storage for start address of the page for access after opening<br>(<b>write access is 32 bit only!!!</b>, refer to <a href="#EfcPageOpenByPageAbs">AT91SAM7X manual 19.2.4.1 page 109: WR_BUF</a>)
      \param   **pu32End   	            (optional, set NULL for default) storage for end address of the page for access after opening<br>(<b>write access is 32 bit only!!!</b>, refer to <a href="#EfcPageOpenByPageAbs">AT91SAM7X manual 19.2.4.1 page 109: WR_BUF</a>)
      \param   *pusPageSize               (optional, set NULL for default) storage for page size of the unit in bytes
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)     
      
      \return										> 0 for success (number of storages updated), == 0 for failure

      \remarks
               - this function allocates the <b>single</b> write only hardware buffer
               
      <b>Manual reference(s):</b><br>
      <a name="EfcPageOpenByPageAbs">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.4.1_p109_WR_BUF.pdf" type="application/pdf">AT91SAM7X manual 19.2.4.1 page 109: WR_BUF</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
*/
UCHAR EfcPageOpenByPageAbs(bool bRCB, UCHAR ucFlags, U32 u32Arg, UCHAR *pucUnit, U32 **pu32Start, U32 **pu32End, USHORT *pusPageSize, U16 u16Timeout);

//!   open a flash page for modification or complete replacement by (start/end/inside page) address or absolute page number 
/**
      \param   bRCB                       value of the non-readable RCB bit in the MC_RCR register of the MemoryController
      \param   ucFlags                    flags to enable flash actions (#EFC_OPEN_CLEAR, #EFC_OPEN_COPY, #EFC_OPEN_UNLOCK) and type of u32Arg (#EFC_LOCATE_ADDRESS, #EFC_LOCATE_PAGEABS),<br>refer to <a href="#EfcPageOpen">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a>)
      \param   u32Arg                     argument (page start/end/inside address for ucFlags #EFC_LOCATE_ADDRESS or absolute page number for ucFlags #EFC_LOCATE_PAGEABS) to open <br><b>(there can only one page opened at a time)</b>
      \param	*pucUnit					      (optional, set NULL for default) storage for the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   **pu32Start 	            (optional, set NULL for default) storage for start address of the page for access after opening<br>(<b>write access is 32 bit only!!!</b>, refer to <a href="#EfcPageOpen">AT91SAM7X manual 19.2.4.1 page 109: WR_BUF</a>)
      \param   **pu32End   	            (optional, set NULL for default) storage for end address of the page for access after opening<br>(<b>write access is 32 bit only!!!</b>, refer to <a href="#EfcPageOpen">AT91SAM7X manual 19.2.4.1 page 109: WR_BUF</a>)
      \param   *pusPageSize               (optional, set NULL for default) storage for page size of the unit in bytes
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)     
      
      \return										> 0 for success (number of storages updated), == 0 for failure

      \remarks
               - this function allocates the <b>single</b> write only hardware buffer
               
      <b>Manual reference(s):</b><br>
      <a name="EfcPageOpen">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.4.1_p109_WR_BUF.pdf" type="application/pdf">AT91SAM7X manual 19.2.4.1 page 109: WR_BUF</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
      
      <b>Example:</b><br>
      \include    Efc.c
*/
UCHAR EfcPageOpen(bool bRCB, UCHAR ucFlags, U32 u32Arg, UCHAR *pucUnit, U32 **pu32Start, U32 **pu32End, USHORT *pusPageSize, U16 u16Timeout);

//!   close previously opened flash page to store (to any page) or discard the previous modification by (start/end/inside page) address
/**
      \param   bRCB                       value of the non-readable RCB bit in the MC_RCR register of the MemoryController
      \param   ucFlags                    flags to enable flash actions (#EFC_CLOSE_LOCK, #EFC_CLOSE_CANCEL), refer to <a href="#EfcPageCloseByAddress">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a>)
      \param   u32Arg                     argument (page start/end/inside address) of previously opened page to <b>(there can only one page opened at a time)</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \remarks
               - this function frees the <b>single</b> write only hardware buffer <b>on success only</b>
               
      <b>Manual reference(s):</b><br>
      <a name="EfcPageCloseByAddress">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
*/
bool EfcPageCloseByAddress(bool bRCB, UCHAR ucFlags, U32 u32Arg, U16 u16Timeout);

//!   close previously opened flash page to store (to any page) or discard the previous modification by absolute page number 
/**
      \param   bRCB                       value of the non-readable RCB bit in the MC_RCR register of the MemoryController
      \param   ucFlags                    flags to enable flash actions (#EFC_CLOSE_LOCK, #EFC_CLOSE_CANCEL), refer to <a href="#EfcPageCloseByPageAbs">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a>)
      \param   u32Arg                     argument (absolute page number) of previously opened page to <b>(there can only one page opened at a time)</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \remarks
               - this function frees the <b>single</b> write only hardware buffer <b>on success only</b>
               
      <b>Manual reference(s):</b><br>
      <a name="EfcPageCloseByPageAbs">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
*/
bool EfcPageCloseByPageAbs(bool bRCB, UCHAR ucFlags, U32 u32Arg, U16 u16Timeout);

//!   close previously opened flash page to store (to any page) or discard the previous modification by (start/end/inside page) address or absolute page number 
/**
      \param   bRCB                       value of the non-readable RCB bit in the MC_RCR register of the MemoryController
      \param   ucFlags                    flags to enable flash actions (#EFC_CLOSE_LOCK, #EFC_CLOSE_CANCEL) and type of u32Arg (#EFC_LOCATE_ADDRESS, #EFC_LOCATE_PAGEABS),<br>refer to <a href="#EfcPageClose">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a>)
      \param   u32Arg                     argument (page start/end/inside address for ucFlags #EFC_LOCATE_ADDRESS or absolute page number for ucFlags #EFC_LOCATE_PAGEABS) to open <br><b>(there can only one page opened at a time)</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \remarks
               - this function frees the <b>single</b> write only hardware buffer <b>on success only</b>
               
      <b>Manual reference(s):</b><br>
      <a name="EfcPageClose">
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
      
      <b>Example:</b><br>
      \include    Efc.c
*/
bool EfcPageClose(bool bRCB, UCHAR ucFlags, U32 u32Arg, U16 u16Timeout);

//!   determine unit, closest page start/end address and page/block numbers from (start/end/inside page) address
/**
      \param   bRCB                       value of the non-readable RCB bit in the MC_RCR register of the MemoryController
      \param   u32Arg                     page (start/end/inside) address to find start and end for
      \param	*pucUnit					      (optional, set NULL for default) storage for the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   *pvPageStart               (optional, set NULL for default) storage for closest page start address
      \param   *pvPageEnd                 (optional, set NULL for default) storage for closest page end address
      \param   *pusPage                   (optional, set NULL for default) storage for page number <b>of the unit</b>
      \param   *pucBlock                  (optional, set NULL for default) storage for block (lock range) number <b>of the unit</b>
      \param   *pucBlockPage              (optional, set NULL for default) storage for relative page number of block (lock range) <b>of the unit</b>

      \return										> 0 for success (number of storages updated), == 0 for failure

      <b>Manual reference(s):</b><br>
      <a name="EfcLocateByAddress">
      <a href="../../../MANUAL/AT91/AT91SAM7X/MC/AT91SAM7X_8.4-8.5_p19-20_MAPPING.pdf" type="application/pdf">AT91SAM7X manual 8.4-8.5 page 19-20: MAPPING</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
*/
UCHAR EfcLocateByAddress(bool bRCB, U32 u32Arg, UCHAR *pucUnit, void **pvPageStart, void **pvPageEnd, USHORT *pusPage, UCHAR *pucBlock, UCHAR *pucBlockPage);

//!   determine unit, closest page start/end address and page/block numbers from absolute page number
/**
      \param   bRCB                       value of the non-readable RCB bit in the MC_RCR register of the MemoryController
      \param   u32Arg                     absolute page number to find start and end for
      \param	*pucUnit					      (optional, set NULL for default) storage for the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   *pvPageStart               (optional, set NULL for default) storage for closest page start address
      \param   *pvPageEnd                 (optional, set NULL for default) storage for closest page end address
      \param   *pusPage                   (optional, set NULL for default) storage for page number <b>of the unit</b>
      \param   *pucBlock                  (optional, set NULL for default) storage for block (lock range) number <b>of the unit</b>
      \param   *pucBlockPage              (optional, set NULL for default) storage for relative page number of block (lock range) <b>of the unit</b>

      \return										> 0 for success (number of storages updated), == 0 for failure

      <b>Manual reference(s):</b><br>
      <a name="EfcLocateByPageAbs">
      <a href="../../../MANUAL/AT91/AT91SAM7X/MC/AT91SAM7X_8.4-8.5_p19-20_MAPPING.pdf" type="application/pdf">AT91SAM7X manual 8.4-8.5 page 19-20: MAPPING</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
*/
UCHAR EfcLocateByPageAbs(bool bRCB, U32 u32Arg, UCHAR *pucUnit, void **pvPageStart, void **pvPageEnd, USHORT *pusPage, UCHAR *pucBlock, UCHAR *pucBlockPage);

//!   determine unit, closest page start/end address and page/block numbers from (start/end/inside page) address or absolute page number
/**
      \param   bRCB                       value of the non-readable RCB bit in the MC_RCR register of the MemoryController
      \param   ucFlags                    flags to determine argument type (#EFC_LOCATE_ADDRESS, #EFC_LOCATE_PAGEABS)
      \param   u32Arg                     argument (page start/end/inside address or absolute page number) to find start and end for
      \param	*pucUnit					      (optional, set NULL for default) storage for the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   *pvPageStart               (optional, set NULL for default) storage for closest page start address
      \param   *pvPageEnd                 (optional, set NULL for default) storage for closest page end address
      \param   *pusPage                   (optional, set NULL for default) storage for page number <b>of the unit</b>
      \param   *pucBlock                  (optional, set NULL for default) storage for block (lock range) number <b>of the unit</b>
      \param   *pucBlockPage              (optional, set NULL for default) storage for relative page number of block (lock range) <b>of the unit</b>

      \return										> 0 for success (number of storages updated), == 0 for failure
      
      <b>Manual reference(s):</b><br>
      <a name="EfcLocate">
      <a href="../../../MANUAL/AT91/AT91SAM7X/MC/AT91SAM7X_8.4-8.5_p19-20_MAPPING.pdf" type="application/pdf">AT91SAM7X manual 8.4-8.5 page 19-20: MAPPING</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/EFC/AT91SAM7X_19.2.1_p103-104_LOCKs.pdf" type="application/pdf">AT91SAM7X manual 19.2.1 page 103-104: LOCKS</a><br>
*/
UCHAR EfcLocate(bool bRCB, UCHAR ucFlags, U32 u32Arg, UCHAR *pucUnit, void **pvPageStart, void **pvPageEnd, USHORT *pusPage, UCHAR *pucBlock, UCHAR *pucBlockPage);

//!   determine lowest/highest page start/end (lowest/highest location) address of the unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   bRCB                       value of the non-readable RCB bit in the MC_RCR register of the MemoryController
      \param   *pu32Start                 (optional, set NULL for default) storage for lowest start address
      \param   *pu32End                   (optional, set NULL for default) storage for highest end address

      \return										> 0 for success (number of storages updated), == 0 for failure

      <b>Manual reference(s):</b><br>
      <a name="EfcAddrRange">
      <a href="../../../MANUAL/AT91/AT91SAM7X/MC/AT91SAM7X_8.4-8.5_p19-20_MAPPING.pdf" type="application/pdf">AT91SAM7X manual 8.4-8.5 page 19-20: MAPPING</a><br>
*/
UCHAR EfcAddrRange(UCHAR ucUnit, bool bRCB, U32 *pu32Start, U32 *pu32End);

//!   determine page size of the unit in bytes
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   *pusPageSize               (optional, set NULL for default) storage for page size of the unit in bytes

      \return										> 0 for success, == 0 for failure
*/
bool EfcPageSize(UCHAR ucUnit, USHORT *pusPageSize);

//!   determine block (lock range) size of the unit in bytes
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   *pusBlockSize              (optional, set NULL for default) storage for block (lock range) size of the unit in bytes

      \return										> 0 for success, == 0 for failure
*/
bool EfcBlockSize(UCHAR ucUnit, USHORT *pusBlockSize);

//!   determine block (lock range) count of the unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   *pusBlockCount              (optional, set NULL for default) storage for block (lock range) count

      \return										> 0 for success, == 0 for failure
*/
bool EfcBlockCount(UCHAR ucUnit, USHORT *pusBlockCount);

//!   determine page count of a block (lock range)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   *pusBlockPageCount         (optional, set NULL for default) storage for page count of a block (lock range)

      \return										> 0 for success, == 0 for failure
*/
bool EfcBlockPageCount(UCHAR ucUnit, USHORT *pusBlockPageCount);

//!   \internal \brief determine value from the drivers read-only data
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of efcdev.h)
      \param   EType                      type of data to retrieve (#EEFCRoGet)
      \param   *pusValue                  (optional, set NULL for default) storage for requested value

      \return										> 0 for success, == 0 for failure
*/
bool EfcRoGet(UCHAR ucUnit, EEFCRoGet EType, USHORT *pusValue);

#endif
