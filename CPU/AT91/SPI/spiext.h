//!	external interface of AT91SAM7X SPI driver
/**
		\file		spiext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/SPI/SPIEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-10-07 initial version
// 2009-10-09 added SpiInitChannel
// 2009-10-15 added SpiMsgUnit, SpiMsgChannel
// 2009-10-21 added ulMsgMask to SpiInitChannel
// 2009-10-23 changed style of SpiTx and SpiRx for support of ushort blocks
// 2009-11-05 changed style of SpiInitUnit (TORX still unsupported)
// 2009-11-06 changed return type of SpiMsgUnit and SpiMsgChannel to USHORT
// 2009-12-18 SpiMsgUnit and SpiMsgChannel now support true counter decrement
// 2009-12-21 added SpiRxTag and SpiTxTag functions
// 2009-12-22 added SpiMsgUnitCheck SpiMsgChannelCheck functions
// 2010-04-08 added SpiTxLock, SpiTx(Tag)Unlock, and SpiTx(Tag)Queue functions
// 2010-08-04 removed obsolete params ulTORXUS & ulTORXModeInit in SpiInitUnit
// 2010-09-22 changed prototype of SpiInitChannel (usMaxAgeTxMS -> u16MaxAge)
// 2011-05-04 added function SpiMsgChannelNext to clean up periph drivers code
// 2011-10-26 added public doxygen documentation
// 2011-11-24 added public doxygen documentation
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_SPI_SPIEXT_H
#define SYKAM_CPU_AT91_SPI_SPIEXT_H 1


// Prototypes for functions

//!   tagged (colored) data concept
/**
      \example SPI_JobSegmentation_TaggedData.txt
*/

//!   notifications for easy task triggering
/**
      \example NewsSemaphore_MessageCounters.txt
      \include TskMBPBMsg.c
*/

//!   zero-pointer memory operation feature
/**
      \example ZeroPointerMemoryOperation.txt
*/

//!   init a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   (unused, set 0) number of the sub-channel for units tree(s)
      \param   u32MR                      settings for the Mode-Register of the spi peripheral unit (refer to <a href="#SpiInitUnit">AT91SAM7X manual 28.7.2 page 269: SPI_MR</a>)
      \param	oiSemMsgNew				      (optional, set to NULL as default) OS_ID of <b>pre-initialized (to 0!)</b> semaphore to send/give for matching messages
      \param	ulMsgMask						(optional, set to NULL as default) and-mask to include triggered messages for semaphore triggering

      \return										> 0 for success, == 0 for failure

      \note
               - 4 tasks are spanned by this function
               - message bits are composed of filtered hardware bits (refer to <a href="#SpiInitUnit">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>) by <b>u32IrqMsg</b> of #SSpiRO
               
      <b>Example:</b> for usual <b>single master with external decoder</b> set <b>U32MR=</b> (#AT91C_SPI_PCS | #AT91C_SPI_MODFDIS | #AT91C_SPI_PCSDEC | #AT91C_SPI_MSTR)<br>
      
      <b>Manual reference(s):</b><br>
      <a name="SpiInitUnit">
      <a href="../../../MANUAL/AT91/AT91SAM7X/SPI/AT91SAM7X_28.7.2_p269-270_SPI_MR.pdf" type="application/pdf">AT91SAM7X manual 28.7.2 page 269: SPI_MR</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/SPI/AT91SAM7X_28.7.5_p273-274_SPI_SR.pdf" type="application/pdf">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a><br>
*/
UCHAR  SpiInitUnit       (UCHAR ucUnit, UCHAR ucChannel, U32 u32MR, OS_ID oiSemMsgNew, ULONG ulMsgMask);

//!   init a channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param   u32CSR                     settings for the Chip-Select-Register of the spi peripheral unit (refer to <a href="#SpiInitChannel">AT91SAM7X manual 28.7.9 page 278: SPI_CSR</a>)
      \param   usBytesPerXfer             maximum transfer block size within a single chip select cycle <b>(driver splits larger transfers into multiple chip select cycles)</b>
      \param   u16MaxAge                  (optional, set OS_T_FOREVER as default) maximum age of queued data
      \param   ucPrio                     (optional, set #SPI_P_NORMAL as default) re-arrange processing order from given slave channel number 0...15 (spi.h)
      \param   ucFlags                    (optional, set #SPI_CHANNEL_FLAGS_CSRUPDATE as default) flag/option of driver for specified channel (spi.h)
      \param	oiSemMsgNew				      (optional, set to NULL as default) OS_ID of <b>pre-initialized (to 0!)</b> semaphore to send/give for matching messages
      \param	ulMsgMask						(optional, set to NULL as default) and-mask to include triggered messages for semaphore triggering

      \return										> 0 for success, == 0 for failure

      \note
               - message bits are composed of filtered hardware bits (refer to <a href="#SpiInitChannel">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>) by <b>u32IrqMsg</b> of #SSpiRO
               - settings in u32CSR for the Chip-Select-Register are stored in virtual driver registers from which the hardware Chip-Select-Register is updated when needed
               
      <b>Example:</b> for usual <b>single master with external decoder</b> set <b>U32MR=</b> (#AT91C_SPI_PCS | #AT91C_SPI_MODFDIS | #AT91C_SPI_PCSDEC | #AT91C_SPI_MSTR)<br>
      
      <b>Manual reference(s):</b><br>
      <a name="SpiInitChannel">
      <a href="../../../MANUAL/AT91/AT91SAM7X/SPI/AT91SAM7X_28.7.9_p278-279_SPI_CSR.pdf" type="application/pdf">AT91SAM7X manual 28.7.9 page 278: SPI_CSR</a><br>
      <a href="../../../MANUAL/AT91/AT91SAM7X/SPI/AT91SAM7X_28.7.5_p273-274_SPI_SR.pdf" type="application/pdf">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a><br>
*/
UCHAR  SpiInitChannel    (UCHAR ucUnit, UCHAR ucChannel, U32 u32CSR, USHORT usBytesPerXfer, U16 u16MaxAge, UCHAR ucPrio, UCHAR ucFlags, OS_ID oiSemMsgNew, ULONG ulMsgMask);

//!   retrieve message counters of a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   (unused, set 0) number of the sub-channel for units tree(s)
      \param	ulMsg				            message bits to retrieve (refer to <a href="#SpiMsgUnit">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>)
      \param	usDecrement				      decrement count (0 for no decrement) for selected message bits <b>(var type's max value USHRT_MAX* just clears the counter(s))</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										summarized count of all selected message bits <b>before decrementing</b>

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max
               - message bits are composed of filtered hardware bits (refer to <a href="#SpiMsgUnit">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>) by <b>u32IrqMsg</b> of #SSpiRO
               
      \remarks
               - this function is helpful to determine the reason for the sent/given "news semaphore" when more than one message is masked
               
      <br><b>*)</b> refer to .\\Keil\\ARM\\RV31\\Inc\\limits.h for compiler type limit definitions
      
      <b>Manual reference(s):</b><br>
      <a name="SpiMsgUnit">
      <a href="../../../MANUAL/AT91/AT91SAM7X/SPI/AT91SAM7X_28.7.5_p273-274_SPI_SR.pdf" type="application/pdf">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a><br>
*/
USHORT SpiMsgUnit        (UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout);

//!   fast check of multiple message counters of a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   (unused, set 0) number of the sub-channel for units tree(s)
      \param	ulMsgMask   	            message bits to retrieve (refer to <a href="#SpiMsgUnitCheck">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										summarized count of all selected message bits

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max
               - message bits are composed of filtered hardware bits (refer to <a href="#SpiMsgUnitCheck">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>) by <b>u32IrqMsg</b> of #SSpiRO
               
      \remarks
               - this function is helpful to verify all messages are handled for the sent/given "news semaphore"
               
      <br><b>*)</b> refer to .\\Keil\\ARM\\RV31\\Inc\\limits.h for compiler type limit definitions
      
      <b>Manual reference(s):</b><br>
      <a name="SpiMsgUnitCheck">
      <a href="../../../MANUAL/AT91/AT91SAM7X/SPI/AT91SAM7X_28.7.5_p273-274_SPI_SR.pdf" type="application/pdf">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a><br>
*/
ULONG  SpiMsgUnitCheck   (UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsgMask, U16 u16Timeout);

//!   retrieve message counters of a channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param	ulMsg				            message bits to retrieve count for (refer to <a href="#SpiMsgChannel">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>)
      \param	usDecrement				      decrement count (0 for no decrement) for selected message bits <b>(var type's max value USHRT_MAX* just clears the counter(s))</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										summarized count of all selected message bits <b>before decrementing</b>

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max
               - message bits are composed of filtered hardware bits (refer to <a href="#SpiMsgChannel">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>) by u32IrqMsg of _SSpiRO

      \remarks
               <b>this function is useful to</b><br>
               - determine the reason for the sent/given "news semaphore" when more than one message is masked <b>before</b> any data retrival using #SpiRx /#SpiRxTag
               - track count of messages just <b>after</b> a normal data access using #SpiRx /#SpiRxTag or #SpiTx /#SpiTxTag for easier debugging and verification
      
      <br><b>*)</b> refer to .\\Keil\\ARM\\RV31\\Inc\\limits.h for compiler type limit definitions
      
      <b>Manual reference(s):</b><br>
      <a name="SpiMsgChannel">
      <a href="../../../MANUAL/AT91/AT91SAM7X/SPI/AT91SAM7X_28.7.5_p273-274_SPI_SR.pdf" type="application/pdf">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a><br>
*/
USHORT SpiMsgChannel     (UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout);

//!   fast check of multiple message counters of a channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param	ulMsgMask   	            message bits to retrieve (refer to <a href="#SpiMsgChannelCheck">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										summarized count of all selected message bits

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max
               - message bits are composed of filtered hardware bits (refer to <a href="#SpiMsgChannelCheck">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>) by <b>u32IrqMsg</b> of #SSpiRO
               
      \remarks
               - this function is helpful to verify all messages are handled for the sent/given "news semaphore"
               
      <br><b>*)</b> refer to .\\Keil\\ARM\\RV31\\Inc\\limits.h for compiler type limit definitions
      
      <b>Manual reference(s):</b><br>
      <a name="SpiMsgChannelCheck">
      <a href="../../../MANUAL/AT91/AT91SAM7X/SPI/AT91SAM7X_28.7.5_p273-274_SPI_SR.pdf" type="application/pdf">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a><br>
*/
ULONG  SpiMsgChannelCheck(UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsgMask, U16 u16Timeout);

//!   retrigger news semaphore for any unhandled specified messages to activate other pending user data tasks
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param	ulMsgMask   	            message bits to retrieve (refer to <a href="#SpiMsgChannelNext">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>)
      \param   oiSemMsgNew                OS_ID of the "news semaphore" to send/give for any unhandled specified messages
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										summarized count of all selected message bits

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max
               - message bits are composed of filtered hardware bits (refer to <a href="#SpiMsgChannelNext">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a>) by <b>u32IrqMsg</b> of #SSpiRO
               
      \remarks
               - this function is helpful to retrigger the "news semaphore" for any unhandled specified messages to activate other pending user data tasks
               
      <br><b>*)</b> refer to .\\Keil\\ARM\\RV31\\Inc\\limits.h for compiler type limit definitions
      
      <b>Manual reference(s):</b><br>
      <a name="SpiMsgChannelNext">
      <a href="../../../MANUAL/AT91/AT91SAM7X/SPI/AT91SAM7X_28.7.5_p273-274_SPI_SR.pdf" type="application/pdf">AT91SAM7X manual 28.7.5 page 273: SPI_SR</a><br>
*/
ULONG  SpiMsgChannelNext (UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsgMask, OS_ID oiSemMsgNew, U16 u16Timeout);

//!   retrieve received data
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param	*pvData					      storage for data <b>(set NULL to wipe requested count of data)</b>
      \param   usCount                    count of data to retrieve
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet)
      
      \remarks
               - user tagged (colored) receiving is available by #SpiTxTag
               - you may wipe data <b>without writing to any storage</b> by setting <b>pvData == NULL</b>
*/
USHORT SpiRx             (UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout);

//!   retrieve received data for specified tag (color) only
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param	*pvData					      storage for data <b>(set NULL to wipe requested count of data)</b>
      \param   usCount                    count of data to retrieve
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      \param   *pucTag                    pointer to storage of tag which content is <b>used ALWAYS for filtering</b> (RECEIVE ONLY ON MATCH) <b>and for storing retrieved tag data</b> (when set to "wildcard" #SPI_USERTAG_DEFAULT)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet)
      
      \remarks
               - you may easily receive non-tagged (ucTag == #SPI_USERTAG_DEFAULT) by #SpiRx
               - you may wipe data <b>without writing to any storage</b> by setting <b>pvData == NULL</b>
*/
USHORT SpiRxTag          (UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout, UCHAR *pucTag);

//!   queue data to send
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param	*pvData					      pointer to data to send <b>(set NULL to queue zero set data)</b>
      \param   usCount                    count of data to send
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet <b>or current tag does not match</b>)
      
      \remarks
               - user tagged (colored) transmission is available by #SpiTxTag
*/
USHORT SpiTx             (UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout);

//!   queue tagged (coloured) data to send
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param	*pvData					      pointer to data to send <b>(set NULL to queue zero set data)</b>
      \param   usCount                    count of data to send
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      \param   ucTag                      tag for the data to queue here (for filtered retrival of tagged (coloured) reply data)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet)
      
      \remarks
               - you may easily transmit non-tagged (ucTag == #SPI_USERTAG_DEFAULT) by #SpiTx
*/
USHORT SpiTxTag          (UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout, UCHAR ucTag);

//!   allocate and lock (untagged and tagged) data queue for later transmission
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param   usCount                    count of data to allocate
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet)
*/
USHORT SpiTxLock         (UCHAR ucUnit, UCHAR ucChannel, USHORT usCount, U16 u16Timeout);

//!   release (already by #SpiTxLock allocated and locked) data for transmission
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param   usCount                    count of data (<b>should match usCount of previous</b> #SpiTxLock)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet)
*/
USHORT SpiTxUnlock       (UCHAR ucUnit, UCHAR ucChannel, USHORT usCount, U16 u16Timeout);

//!   release and tag (already by #SpiTxLock allocated and locked) data for transmission
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param   usCount                    count of data (<b>should match usCount of previous</b> #SpiTxLock)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      \param   ucTag                      tag for the all the queued data here (for filtered retrival of tagged (coloured) reply data)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet)
*/
USHORT SpiTxTagUnlock    (UCHAR ucUnit, UCHAR ucChannel, USHORT usCount, U16 u16Timeout, UCHAR ucTag);

//!   queue (already by #SpiTxLock allocated and locked) data to combine further multiple queue accesses to single data transmission (data processing neeeds triggering by #SpiTxUnlock)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param	*pvData					      pointer to data to send
      \param   usCount                    count of data to send
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet)
*/
USHORT SpiTxQueue        (UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout);

//!   queue (already by #SpiTxLock allocated and locked) tagged data to combine further multiple queue accesses to single data transmission (coloured) (data processing neeeds triggering by #SpiTxTagUnlock)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of spidev.h)
      \param	ucChannel					   number of the channel (<b>hardware</b> chip select number of the slave circuit)
      \param	*pvData					      pointer to data to send
      \param   usCount                    count of data to send
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      \param   ucTag                      (unused yet, but <b>set to ucTag value of following #SpiTxTagUnlock</b>)
      
      \return										> 0 (== usCount) for success, == 0 for failure (e.g. requested count is not available yet)
*/
USHORT SpiTxTagQueue     (UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout, UCHAR ucTag);

#endif
