//!	external interface of AT91SAM7X PWM driver
/**
		\file		pwmext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/PWM/PWMEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2010-01-15 initial version
// 2010-01-28 added PwmCalcModnPrdDty
// 2010-02-04 renamed PwmCalcModnPrdDty to PwmCalcCprePrdDty
// 2010-02-25 added PwmCalcHz
// 2010-07-01 renamed PwmInit to PwmInitUnit
// 2010-07-01 added PwmInitChannel, PwmChannelTx and PwmChannelTxValue
// 2010-07-01 added PwmChannelWait
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-12-16 added new function PwmChannelIdGet and PwmChannelInactiveGet
// 2010-12-16 added new function PwmStartChannel and PwmStopChannel
// 2010-12-17 changed return type of function PwmChannelWait
// 2011-10-20 added public doxygen documentation
// 2011-10-26 added public doxygen documentation
// 2013-10-31 renamed PwmChannelTxValue to PwmTxValueChannel to fit overal naming style
// 2013-10-31 renamed PwmChannelTx to PwmTxChannel to fit overal naming style
// 2013-10-31 renamed PwmChannelInactiveGet to PwmInactiveGetChannel to fit overal naming style
// 2013-10-31 renamed PwmChannelWait to PwmWaitChannel to fit overal naming style
// 2013-11-06 renamed PwmStartChannel to PwmStart to fit overal naming style
// 2013-11-06 renamed PwmStopChannel to PwmStop to fit overal naming style
// 2013-11-06 renamed PwmTxValueChannel to PwmTxValue to fit overal naming style
// 2013-11-06 renamed PwmTxChannel to PwmTx to fit overal naming style
// 2013-11-06 renamed PwmInactiveGetChannel to PwmInactiveGet to fit overal naming style
// 2013-11-06 renamed PwmWaitChannel to PwmWait to fit overal naming style
// 2013-11-06 changed to new doxygen example names
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_PWM_PWMEXT_H
#define SYKAM_CPU_AT91_PWM_PWMEXT_H 1


// Externals (located inside AT91SAM7X.h)
extern struct _SPwmItem SPwmList[];

// Prototypes

//!   init a pwm unit
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pwmdev.h)
      \param	*pSPwmList					   the array of struct _SPwmItem with a defined item for every pwm channel to configure
      \param	ucItemCnt				      the count of items in the specified array to configure

      \return										> 0 for success, == 0 for failure
*/
bool   PwmInitUnit          (UCHAR ucUnit, struct _SPwmItem *pSPwmList, UCHAR ucItemCnt);

//!   get the <b>CH</b>annel-<b>ID</b> (1 << channel) for specified unit and channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pwmdev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>

      \return										CHannel-ID (1 << channel) for specified unit and channel
*/
UCHAR  PwmChannelIdGet     (UCHAR ucUnit, UCHAR ucChannel);

//!   init a channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pwmdev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>

      \return										> 0 for success, == 0 for failure
*/
bool   PwmInitChannel       (UCHAR ucUnit, UCHAR ucChannel);

//!   start a channel with initial parameters
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pwmdev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>
      \param   u16PwmCmr                  lower 16 bits to set in the Channel-Mode-Register (esp. the lower 4 bits for the Channel-PREscaler)
      \param   u16PwmCprdr                value to set in the Channel-PeRioD-Register
      \param   u16PwmCdtyr                value to set in the Channel-DuTY-Register

      \note
               - 1 task is spanned by this function for every channel

      \return										> 0 for success, == 0 for failure
      
      \remarks
               - use the #PwmCalcCprePrdDty function for easy calculation of u16PwmCmr,  u16PwmCprdr and u16PwmCdtyr
*/
bool   PwmStart            (UCHAR ucUnit, UCHAR ucChannel, U16 u16PwmCmr, U16 u16PwmCprdr, U16 u16PwmCdtyr);

//!   stop a running channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pwmdev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
*/
bool   PwmStop             (UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout);

//!   send a single packed value/constant to a channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pwmdev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>
      \param   ulData                     packed 32 bit value/constant containing lower 16 bits to set in the Channel-Mode-Register and 16 bit value to set in the Channel-DuTY-Register
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure
      
      \note
               - <b>to modify the period of the pwm channel OR (|) the constant #AT91C_PWMC_CPD into your ulData</b>, otherwise the duty will be modified
               
      \remarks
               - use the #PWM32_CMR16_PRDDTY16(cmr, prddty) macro (pwm.h) to pack the lower 16 bits to set in the Channel-Mode-Register and the 16 bit value to set in the Channel-DuTY-Register into a 32 bit value
               
      <b>Example code:</b><br>
      \include PwmTxValue.c
*/
USHORT PwmTxValue          (UCHAR ucUnit, UCHAR ucChannel, ULONG ulData, U16 u16Timeout);

//!   send packed data item(s) to a channel
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pwmdev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>
      \param   *pulData                   pointer to packed 32 bit data item(s) containing lower 16 bits to set in the Channel-Mode-Register and value to set in the Channel-DuTY-Register
      \param   usCount                    count of data items to send
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for success, == 0 for failure

      \note
               - <b>to modify the period of the pwm channel OR (|) the constant #AT91C_PWMC_CPD into your ulData</b>, otherwise the duty will be modified

      \remarks
               - use the #PWM32_CMR16_PRDDTY16(cmr, prddty) macro (pwm.h) to pack the lower 16 bits to set in the Channel-Mode-Register and the 16 bit value to set in the Channel-DuTY-Register into a 32 bit value
               
      <b>Example code:</b><br>
      \include PwmTx.c
*/
USHORT PwmTx               (UCHAR ucUnit, UCHAR ucChannel, ULONG *pulData, USHORT usCount, U16 u16Timeout);

//!   check a channel for being ready/inactive
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pwmdev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for channel is ready/inactive, 0 == channel is busy/active
      
      \remarks
               - use the #PWM32_CMR16_PRDDTY16(cmr, prddty) macro (pwm.h) to pack the lower 16 bits to set in the Channel-Mode-Register and the 16 bit value to set in the Channel-DuTY-Register into a 32 bit value
*/
bool   PwmInactiveGet      (UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout);

//!   pend for a channel until all send data is processed
/**
      \param	ucUnit					      number of the unit (descriptor array index <b>[not the hardware index]</b> of pwmdev.h)
      \param	ucChannel					   number of <b>logical descriptor</b> (0, 1, 2, 3 ...) channel <b>(NOT CHID and NOT hardware)</b>
      \param	u16Timeout				      maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return										> 0 for channel is ready/inactive, 0 == channel is busy/active

      \remarks
               - to successfully pend (wait without cpu load) you need a u16Timeout value longer than the duration of the processing of all data for the channel
*/
bool   PwmWait             (UCHAR ucUnit, UCHAR ucChannel, U16 u16Timeout);

//!   calculate Channel-PREscaler, Channel-PeRioD and Channel-DuTY register contents from given frequencies
/**
      \param	ulClkHz					      the frequency to generate in Hz
      \param	ulStableHz					   the length of the <b>constant/stable</b> duty part in Hz
      \param	*pucCpre 				      storage for the Channel-PREscaler (the lower 4 bits from the lower 16 bits of the Channel-Mode-Register)
      \param	*pu16Prd 				      storage for the Channel-PeRioD-Register's value
      \param	*pu16Dty 				      storage for the Channel-DuTY-Register's value

      \return										> 0 for success, == 0 for failure (frequency not supported by hardware)
      
      \see                                PwmCalcHz

      \remarks
               - because the pwm seamless frequency modulation supports only period OR duty at a time the PWM driver is usually (e.g. stpclkdir) configured to keep the duty constant/stable and modifies only the period (#AT91C_PWMC_CPD set in PWMC_CUPDR of #_AT91S_PWMC_CH)
*/
bool   PwmCalcCprePrdDty    (ULONG ulClkHz, ULONG ulStableHz, UCHAR *pucCpre, U16 *pu16Prd, U16 *pu16Dty);

//!   calculate frequency from given Channel-PREscaler and Channel-PeRioD-Register contents
/**
      \param	ucCpre					      Channel-PREscaler (the lower 4 bits from the lower 16 bits of the Channel-Mode-Register)
      \param	u16Prd					      Channel-PeRioD-Register

      \return										frequency in Hz
      
      \see                                PwmCalcCprePrdDty

      \remarks
               - use the #PWM32_GET_CMR16 and #PWM32_GET_PRDDTY16 macros (pwm.h) for easy extraction of calculation of Channel-PREscaler (the lower 4 bits from the lower 16 bits of the Channel-Mode-Register) and the Channel-PeRioD-Register from a packed 32 bit value
*/
ULONG  PwmCalcHz            (UCHAR ucCpre, U16 u16Prd);

#endif
