//!	external interface of FM25CLSSEXT peripheral driver
/**
		\file		fm25clssext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  P E R I P H E R A L  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/EXT/SPI/NVRAM/FM25CLSS/FM25CLSSEXT.H
// Purpose: PERIPHERAL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-04-07 initial version
// 2010-04-08 Fm25clssRDSR, Fm25clssWRSR, Fm25clssRead and Fm25clssRead
// 2010-04-09 added Fm25clssCtrl function
// 2010-04-09 splitted Fm25clssRead into Fm25clssRead and Fm25clssReadSafe
// 2010-04-09 changed Fm25clssWRSR style
// 2010-07-06 rr: added doxygen doc
// 2010-07-28 fixed dodygen doc
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2011-11-03 added/rewrote public doxygen documentation
// 2017-09-07 updated/fixed public doxygen documentation
//----------------------------------------------------------------------------
// Copyright(c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_EXT_SPI_NVRAM_FM25CLSS_FM25CLSSEXT_H
#define SYKAM_EXT_SPI_NVRAM_FM25CLSS_FM25CLSSEXT_H


// Prototypes

//!   init a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of fm25clssdev.h)
      \param	oiMsgNewSem				      OS_ID of <b>pre-initialized (to 0!)</b> semaphore which is assigned to the spi driver channel this peripheral is connected to

      \return										> 0 for success, == 0 for failure
*/
UCHAR  Fm25clssInit    (UCHAR ucUnit, OS_ID oiMsgNewSem);

//!   enable / disable the write access
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of fm25clssdev.h)
      \param	ucCode				         enable (#FM25CLSS_CODE_WREN) or disable (#FM25CLSS_CODE_WRDI) the write access for following chip select cycle
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
*/
USHORT Fm25clssCtrl    (UCHAR ucUnit, UCHAR ucCode, U16 u16Timeout);

//!   retrieve the status register
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of fm25clssdev.h)
      \param	*pvData				         storage for the retrieved status register data (<b>[single byte]</b> refer to <a href="#Fm25clssRDSR">FM25CLSS_SR_... </a>of fm25clss.h)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      <b>Manual reference(s):</b><br>
      <a name="Fm25clssRDSR">
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 6: Status Register & Write Protection</a><br>
*/
USHORT Fm25clssRDSR    (UCHAR ucUnit, void *pvData, U16 u16Timeout);

//!   write the status register
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of fm25clssdev.h)
      \param	ucCode				         data to write into the status register (refer to <a href="#Fm25clssWRSR">FM25CLSS_SR_... </a>of fm25clss.h)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      <b>Manual reference(s):</b><br>
      <a name="Fm25clssWRSR">
      <a href="../../../MANUAL/EXT/SPI/NVRAM/FM25CLss/FM25CL64_p5-8.pdf" type="application/pdf">FM25CL64.PDF page 6: Status Register & Write Protection</a><br>
*/
USHORT Fm25clssWRSR    (UCHAR ucUnit, UCHAR ucCode, U16 u16Timeout);

//!   read data <b>(WITHOUT any checking)</b>
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of fm25clssdev.h)
      \param	*pvData				         pointer to data to write retrieved chip memory
      \param   usCount                    count of data to retrieve
      \param   usAddress                  address of chip memory to retrieve from (0x0000 to <b>usSizeBytes/ucMultiStore</b> of #_SFm25clssRO)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \note
               - this function does NO CHECKING of data integrity ALTHOUGH <b>ucMultiStore</b> of _SFm25clssRO is set <b>(> 1)</b> !!!
               - therefore you should enable <b>bMultiTransfer</b> and do YOUR OWN CHECKING when needed
               - #Fm25clssRead is a wrapper of #Fm25clssReadSafe with pvTemp == NULL
*/
USHORT Fm25clssRead    (UCHAR ucUnit, void *pvData, USHORT usCount, USHORT usAddress, U16 u16Timeout);

//!   read data safely <b>(WITH checking)</b>
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of fm25clssdev.h)
      \param	*pvData				         pointer to data to write retrieved chip memory
      \param   *pvTemp                    (option, set NULL to disable) temporary compare buffer for automatic data integrity checking (copies only)
      \param   usCount                    count of data to retrieve
      \param   usAddress                  address of chip memory to retrieve from (0x0000 to <b>usSizeBytes/ucMultiStore</b> of #_SFm25clssRO)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \note
               - this function does AUTOMATIC CHECKING of data integrity WHEN <b>ucMultiStore</b> of _SFm25clssRO is set <b>(> 1)</b> and <b>pvTemp != NULL</b> !!!
               - therefore you'll usually DISABLE <b>bMultiTransfer</b> when using this automatic data integrity checking <b>(except you plan further majority checking)</b>
               - AUTOMATIC data integrity checking is done by COMPARING ALL COPIES of your data (<b>ucMultiStore</b>) which need to match to 100%
               
      \remarks
               - for DISABLED <b>bMultiTransfer</b> a <b>usCount= 2</b> (two byte read, byte a0 and b0) with <b>ucMultiStore= 3</b> #Fm25clssReadSafe stores at <b>pvData</b><br>a0 and b0<br>and at <b>pvTemp</b> their copies<br>a1, b1, a2 and b2
               - for ENABLED <b>bMultiTransfer</b> a <b>usCount= 2</b> (two byte read, byte a0 and b0) with <b>ucMultiStore= 3</b> #Fm25clssReadSafe stores the data and all its copies at <b>pvData</b><br>a0, b0, a1, b1, a2 and b2 <br>(pvTemp is ignored)
*/
USHORT Fm25clssReadSafe(UCHAR ucUnit, void *pvData, void *pvTemp, USHORT usCount, USHORT usAddress, U16 u16Timeout);

//!   write data (with option for automatic multiple copies)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of fm25clssdev.h)
      \param	*pvData				         pointer to data to write into chip memory
      \param   usCount                    count of data to write
      \param   usAddress                  address of chip memory to start write access (0x0000 to <b>usSizeBytes/ucMultiStore</b> of #_SFm25clssRO)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \note
               - this function saves <b>ucMultiStore</b> time(s) AUTOMATIC COPIES of your data WHEN <b>ucMultiStore</b> of _SFm25clssRO is set <b>(> 1)</b> !!!
               - therefore you'll usually DISABLE <b>bMultiTransfer</b> when using this AUTOMATIC COPY feature for your data <b>(except you plan further majority checking)</b>
               - AUTOMATIC data integrity checking is done by COMPARING ALL COPIES of your data (<b>ucMultiStore</b>) which need to match to 100% (#Fm25clssReadSafe <b>ONLY</b>)
*/
USHORT Fm25clssWrite   (UCHAR ucUnit, void *pvData, USHORT usCount, USHORT usAddress, U16 u16Timeout);

#endif
