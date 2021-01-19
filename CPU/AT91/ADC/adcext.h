//!	external interface of AT91SAM7X ADC peripheral driver
/**
		\file		adcext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/ADC/ADCEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-05-19 initial version
// 2011-05-25 added message functions AdcMsgUnit & AdcMsgChannel
// 2011-07-07 renamed oiSemMsgEnh to oiSemMsgNew in AdcInitUnit prototype
// 2011-07-12 added ulMsgChangeMask param to AdcInitUnit prototype
// 2011-10-19 added public doxygen documentation
// 2011-10-26 added public doxygen documentation
// 2013-08-01 added retrieve support of (non-contiguous) row of channels to AdcRx
// 2013-08-01 added retrieve support of (non-contiguous) row of channels to AdcWait
// 2013-08-01 changed return type of AdcWait to USHORT
// 2013-08-01 updated public doxygen documentation
// 2013-08-07 added AdcContinue for manual triggering in one-shot software mode
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_ADC_ADCEXT_H
#define SYKAM_CPU_AT91_ADC_ADCEXT_H 1


// Prototypes

//!	initialize a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of adcdev.h)
      \param	ucChannel					   (unused, set 0) number of the sub-channel for units tree(s)
      \param	oiSemMsgNew				      (optional, set to NULL as default) OS_ID of <b>pre-initialized (to 0!)</b> semaphore to send/give for matching messages
      \param	usChannelMask				   (optional, set to 0 as default) triggered channels to take into account for unit semaphore triggering
      \param	ulMsgMask						(optional, set to NULL as default) and-mask to include triggered channels' messages for unit semaphore triggering
      \param	ulMsgChangeMask				(optional, set to NULL as default) mask to suppress (bit set) same message triggering and to trigger only for a message change

      \return										> 0 for success, == 0 for failure

      \note
               - 3 tasks are spanned by this function
               - message bits are composed of software bits (trigger message bits of adc.h) and filtered hardware bits (refer to <a href="#AdcInitUnit">AT91SAM7X manual 35.6.6 page 498: ADC_SR</a>) by <b>u32IrqMsg</b> of _SAdcRO
               - <b>for easy adc channel data retrieval you may use #AdcRx only as alternate simple interface without the need of any notifications by semaphores</b>
               
      <b>Example:</b>  merge all channel semaphores to a single unit semaphore you need:<br>
               - <b>configure ulMsgMask & ulMsgChangeMask of all required channels</b> (#AdcInitChannel)
               - <b>select channels to merge:</b> usChannelMask= AT91C_ADC_CHx | AT91C_ADC_CHy | ...
               - <b>set unit message filter:</b> ulMsgMask= AdcCfgChannel[x].ulMsgMask | AdcCfgChannel[y].ulMsgMask | ...
               - <b>set unit change mask:</b> ulMsgChangeMask= (0xFFFFFFFF & AdcCfgChannel[x].ulMsgChangeMask) & (0xFFFFFFFF & AdcCfgChannel[y].ulMsgChangeMask) & ...
               - <b>assign #OS_ID of your pre-initialized (with 0!) semaphore:</b> oiSemMsgNew= ...
      
      <b>Manual reference(s):</b><br>
      <a name="AdcInitUnit">
      <a href="../../../MANUAL/AT91/AT91SAM7X/ADC/AT91SAM7X_35.6.6_p498_ADC_SR.pdf" type="application/pdf">AT91SAM7X manual 35.6.6 page 498: ADC_SR</a><br>
*/
UCHAR  AdcInitUnit   (UCHAR ucUnit, UCHAR ucChannel, OS_ID oiSemMsgNew, USHORT usChannelMask, ULONG ulMsgMask, ULONG ulMsgChangeMask);

//!	initialize a channel of a previously initialized unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of adcdev.h)
      \param	ucChannel					   (optional, set to 0 as default) offset to add to channel number in array element of struct _SAdcCfgChannel
      \param	*pSCfgChannel				   pointer to array of struct _SAdcCfgChannel which contains the channel(s)s configuration
      \param	ucCfgChannelCnt				count of channels to configure from array of struct _SAdcCfgChannel

      \return										> 0 for success, == 0 for failure

      \note
               - ucChannel specifies here an optional offset <b>[not an absolute index]</b>
               - <b>for easy adc channel data retrieval you may use #AdcRx only as alternate simple interface without the need of any notifications by semaphores</b>

      <b>Example:</b>  send/give the specified semaphore for every limit band change to below/above and min/max but for every sampled value when inside the normal band:<br>
               - ulMsgMask= #ADC_TRG_NORMAL | #ADC_TRG_MAX | #ADC_TRG_MIN | #ADC_TRG_ABOVE | #ADC_TRG_BELOW
               - ulMsgChangeMask= #ADC_TRG_MAX | #ADC_TRG_MIN | #ADC_TRG_ABOVE | #ADC_TRG_BELOW

      <b>Example:</b>  send/give the specified semaphore for every sampled value when outside the normal band but for normal band itsself only when changed to it:<br>
               - ulMsgMask= #ADC_TRG_NORMAL | #ADC_TRG_MAX | #ADC_TRG_MIN | #ADC_TRG_ABOVE | #ADC_TRG_BELOW
               - ulMsgChangeMask= #ADC_TRG_NORMAL
*/
UCHAR  AdcInitChannel(UCHAR ucUnit, UCHAR ucChannel, struct _SAdcCfgChannel *pSCfgChannel, UCHAR ucCfgChannelCnt);

//!   retrieve message counters of a unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of adcdev.h)
      \param	ucChannel					   (unused, set 0) number of the sub-channel for units tree(s)
      \param	ulMsg				            message bits to retrieve (refer to <a href="#AdcMsgUnit">AT91SAM7X manual 35.6.6 page 498: ADC_SR</a> and trigger message bits of adc.h)
      \param	usDecrement				      decrement count (0 for no decrement) for selected message bits <b>(var type's max value #USHRT_MAX just clears the counter(s))</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										summarized count of all selected message bits <b>before decrementing</b>

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max
               - message bits are composed of software bits (trigger message bits of adc.h) and filtered hardware bits (refer to <a href="#AdcMsgUnit">AT91SAM7X manual 35.6.6 page 498: ADC_SR</a>) by <b>u32IrqMsg</b> of _SAdcRO
               
      \remarks
               - this function is helpful to determine the reason for the sent/given "news semaphore" when more than one message is masked
      
      <b>Manual reference(s):</b><br>
      <a name="AdcMsgUnit">
      <a href="../../../MANUAL/AT91/AT91SAM7X/ADC/AT91SAM7X_35.6.6_p498_ADC_SR.pdf" type="application/pdf">AT91SAM7X manual 35.6.6 page 498: ADC_SR</a><br>
*/
USHORT AdcMsgUnit    (UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout);

//!   retrieve message counters of a channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of adcdev.h)
      \param	ucChannel					   number of the channel
      \param	ulMsg				            message bits to retrieve count for (refer to <a href="#AdcMsgChannel">AT91SAM7X manual 35.6.6 page 498: ADC_SR</a> and trigger message bits of adc.h)
      \param	usDecrement				      decrement count (0 for no decrement) for selected message bits <b>(var type's max value #USHRT_MAX just clears the counter(s))</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										summarized count of all selected message bits <b>before decrementing</b>

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max
               - message bits are composed of software bits (trigger message bits of adc.h) and filtered hardware bits (refer to <a href="#AdcMsgChannel">AT91SAM7X manual 35.6.6 page 498: ADC_SR</a>) by <b>u32IrqMsg</b> of _SAdcRO

      \remarks <b>this function is helpful to</b><br>
               - determine the reason for the sent/given "news semaphore" when more than one message is masked <b>before</b> any data retrival using #AdcRx
               - track count of messages just <b>after</b> a normal data retrival using #AdcRx for easier debugging and verification
               
      <b>Manual reference(s):</b><br>
      <a name="AdcMsgChannel">
      <a href="../../../MANUAL/AT91/AT91SAM7X/ADC/AT91SAM7X_35.6.6_p498_ADC_SR.pdf" type="application/pdf">AT91SAM7X manual 35.6.6 page 498: ADC_SR</a><br>
*/
USHORT AdcMsgChannel (UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout);

//!   retrieve (and remove) data of channel(<b>s</b>)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of adcdev.h)
      \param	ucChannel					   number of the channel
      \param	pvData				         pointer to data storage (array size must match usChannelCount) <b>(type #ADC_MEMITEMTYPE of adc.h)</b>
      \param	usChannelCount		         count of channel data to retrieve <b>(number of non-contiguous channels)</b> from starting channel <b>ucChannel</b> (limited to #ADC_CHANNEL_MAX)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success (== usChannelCount), == 0 for failure

      \note
               - because the adc driver does NOT buffer data the 2nd retrieve faster than the sample interval u16CycleRxMS of _SAdcRO will fail
               - for checking "data ready" only (without retrieve) use AdcWait
               - triggers next adc cycle in one-shot software mode automatically when required
               - this function retrieves at least 1 channel and up to <b>usCount</b> non-contiguous (non-configured channels are skipped) channels from starting channel <b>ucChannel</b>
               - <b>for easy adc channel data retrieval you may use #AdcRx only as alternate simple interface without the need of any notifications by semaphores</b>
*/
USHORT AdcRx         (UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usChannelCount, U16 u16Timeout);

//!   pend for ready data of channel(<b>s</b>)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of adcdev.h)
      \param	ucChannel					   number of the channel
      \param	usChannelCount		         count of channels to check <b>(number of non-contiguous channels)</b> from starting channel <b>ucChannel</b> (limited to #ADC_CHANNEL_MAX)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success (== usChannelCount), == 0 for failure

      \note
               - because the adc driver does NOT buffer data the 2nd retrieve faster than the sample interval u16CycleRxMS of _SAdcRO will fail
               - checking for data ready may be called faster than the sample interval u16CycleRxMS of _SAdcRO
               - this function checks at least 1 channel and up to <b>usCount</b> non-contiguous (non-configured channels are skipped) channels from starting channel <b>ucChannel</b>
               - <b>for easy adc channel data retrieval you may use #AdcRx only as alternate simple interface without the need of any notifications by semaphores</b>
*/
USHORT  AdcWait       (UCHAR ucUnit, UCHAR ucChannel, USHORT usChannelCount, U16 u16Timeout);

//!   trigger next adc cycle in one-shot mode
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of adcdev.h)
      \param	ucChannel					   (unused, set to 0) number of the channel
      \param	u16Timeout				      (unused, set to 0) maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure

      \note
               - this function allows AdcRx independent triggering of next adc cycle in one-shot software mode when required
*/
USHORT  AdcContinue(UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout);

#endif
