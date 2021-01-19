//!	external interface of AT91SAM7X PCKO driver
/**
		\file		pckoext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PMC/PCKO/PCKOEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2013-10-23 initial version
// 2013-10-30 initial revision
// 2013-10-31 renamed PckoChannelInactiveGet to PpckoInactiveGetChannel to fit overal naming style
// 2013-10-31 renamed PckoChannelWait to PckoWaitChannel to fit overal naming style
// 2013-11-06 renamed PckoIdGetChannel to PckoChannelIdGet to fit overal naming style
// 2013-11-06 renamed PckoInactiveGetChannel to PckoInactiveGet to fit overal naming style
// 2013-11-06 renamed PckoConfigChannel to PckoConfig to fit overal naming style
// 2013-11-06 renamed PckoStartChannel to PckoStart to fit overal naming style
// 2013-11-06 renamed PckoStopChannel to PckoStop to fit overal naming style
// 2013-11-06 renamed PckoWaitChannel to PckoWait to fit overal naming style
// 2013-11-20 added example to PckoInitChannel
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PMC_PCKO_PCKOEXT_H
#define SYKAM_CPU_AT91_PMC_PCKO_PCKOEXT_H 1


// Prototypes

//!   init a pcko unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pckodev.h)

      \return										> 0 for success, == 0 for failure
*/
bool   PckoInitUnit     (UCHAR ucUnit);

//!   init a channel and optionally start it with initial parameters from the specified descriptor
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pckodev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>

      \return										> 0 for success, == 0 for failure
      
      \note
               - for true autostart flag in the channel descriptor the channel is configured and started, <b>but NO pending to running state is included</b> (use #PckoWait to pend for running state)
               
      <b>Example:</b><br>
      \include    Pcko.c
*/
bool   PckoInitChannel  (UCHAR ucUnit, UCHAR ucChannel);

//!   get the <b>CH</b>annel-<b>ID</b> (1 << (8+ channel)) for specified unit and channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pckodev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>

      \return										CHannel-ID (1 << (8+ channel)) for specified unit and channel
      
      <b>Manual reference(s):</b><br>
      <a name="PckoChannelIdGet">
      <a href="../../../MANUAL/AT91/AT91SAM7X/PMC/PCKO/AT91SAM7X_25.9.1_p191-193_PMC_SCxR.pdf" type="application/pdf">AT91SAM7X manual 25.9.11 pages 191-193: PMC_SCxR</a><br>
*/
U16   PckoChannelIdGet  (UCHAR ucUnit, UCHAR ucChannel);

//!   check a channel for being ready/inactive
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pckodev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>

      \return										> 0 for channel is ready/inactive, 0 == channel busy/active
      
      \note
               - a channel may have one of these ascending states: ready/inactive, busy/active and running (use #PckoWait to pend for change to running state)
*/
bool   PckoInactiveGet  (UCHAR ucUnit, UCHAR ucChannel);

//!   config a channel (after stopping it)
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pckodev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>
      \param	u8Pcr                      value of (P)rogrammable (C)lock (R)egister value (refer to <a href="#PckoConfigChannel">AT91SAM7X manual 25.9.11 page 200: PMC_PCKx</a>)
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for channel is ready/inactive, 0 == channel running/active

      \note
               - a active or running channel is stopped to set its new configuration <b>and NOT automatically restarted</b> (use #PckoStart to restart the channel)

      \remarks
               - use the #PCKO_PCR8_PRES3_CSS2(pres, css) macro (pcko.h) to pack the 3 bit (P)(r)(e)(s)caler and 2 bit (C)lock (S)ource (S)election into 8 bit (P)rogrammable (C)lock (R)egister value
               
      <b>Manual reference(s):</b><br>
      <a name="PckoConfigChannel">
      <a href="../../../MANUAL/AT91/AT91SAM7X/PMC/PCKO/AT91SAM7X_25.9.11_p200_PMC_PCKx.pdf" type="application/pdf">AT91SAM7X manual 25.9.11 page 200: PMC_PCKx</a><br>
*/
bool  PckoConfig        (UCHAR ucUnit, UCHAR ucChannel, U8 u8Pcr, U16 u16Timeout);

//!   start a channel with initial parameters from the specified descriptor
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pckodev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>

      \return										> 0 for success, == 0 for failure

      \note
               - the channel is just started, <b>NOT</b> configured (use #PckoConfig to configure before starting it!)
               
      \remarks
               - the default hardware configuration after power-on is <b>SLCK/ 1</b> but default descriptor in pckodev.h is different (refer to <a href="#PckoStartChannel">AT91SAM7X manual 25.9.11 page 200: PMC_PCKx</a>)
               - to determine the true frequency of SLCK use the RTT function #RttSLCKGet
               
      <b>Manual reference(s):</b><br>
      <a name="PckoStartChannel">
      <a href="../../../MANUAL/AT91/AT91SAM7X/PMC/PCKO/AT91SAM7X_25.9.11_p200_PMC_PCKx.pdf" type="application/pdf">AT91SAM7X manual 25.9.11 page 200: PMC_PCKx</a><br>

*/
bool   PckoStart        (UCHAR ucUnit, UCHAR ucChannel);

//!   stop a running channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pckodev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure

      \remarks
               - the channel is stopped after pending on it for being running when it's at least active yet
*/
bool   PckoStop         (UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout);

//!   pend for a channel until it's active and running
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pckodev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for channel is ready/inactive, 0 == channel is busy/active

      \remarks
               - to successfully pend (wait without cpu load) you need a u16Timeout value longer than the duration of the activation of the channel
*/
bool   PckoWait         (UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout);

#endif
