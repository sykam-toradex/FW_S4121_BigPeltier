//!   external interface of AT91SAM7X UDPRAW driver
/**
      \file    udprawext.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/UDP/RAW/UDPRAWEXT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2014-11-06 initial version
// 2014-11-12 cleanup of interface
// 2015-03-05 fixed interface of UdprawInitChannel
// 2015-04-16 removed obsolete para u8FAdd in UdprawInitUnit
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_UDP_RAW_UDPRAWEXT_H
#define SYKAM_CPU_AT91_UDP_RAW_UDPRAWEXT_H 1


// Prototypes for functions

//!   pre-init a unit (BEFORE RTX is available!!!)

//!   pre-init a channel (endpoint) (BEFORE RTX is available!!!)

//!   init a unit
/**
      \param   ucUnit                     number of the unit (descriptor array index <b>[not the hardware index]</b> of udprawdev.h)
      \param   ucChannel                  (unused, set 0) number of the sub-channel for units tree(s)
      \param   oiSemMsgNew                (optional, set to NULL as default) OS_ID of <b>pre-initialized (to 0!)</b> semaphore to send/give for matching messages
      \param   ulMsgMask                  (optional, set to NULL as default) and-mask to include triggered messages for semaphore triggering

      \return                             > 0 for success, == 0 for failure

      \note
               - 2 tasks are spawned by this function
               - message bits are composed of filtered hardware bits (refer to <a href="#UdprawInitUnit">AT91SAM7X manual 34.6.7 pages 474-475: UDP_ISR</a>) by <b>u32IrqMsg</b> of #SUdprawRO
               
      <b>Manual reference(s):</b><br>
      <a name="UdprawInitUnit">
      <a href="../../../MANUAL/AT91/AT91SAM7X/UDP/AT91SAM7X_34.6.7_p474-475_UDP_ISR.pdf" type="application/pdf">AT91SAM7X manual 34.6.7 pages 474-475: UDP_ISR</a><br>
*/
UCHAR  UdprawInitUnit       (UCHAR ucUnit, UCHAR ucChannel, OS_ID oiSemMsgNew, ULONG ulMsgMask);

//!   init a channel (endpoint)
/**
      \param   ucUnit                     number of the unit (descriptor array index <b>[not the hardware index]</b> of udprawdev.h)
      \param   ucChannel                  number of the channel (<b>hardware</b> endpoint number)
      \param   usType                     type of channel (endpoint) to enable rx/host-out (#UDPRAW_IS_EPTYPE_OUT() != 0) or tx/host-in (#UDPRAW_IS_EPTYPE_IN() != 0) or both (#UDPRAW_IS_EPTYPE_BOTH() != 0), refer to #AT91C_UDP_EPTYPE and udpraw.h
      \param   *pucRx                     (required for rx / host out support) external supplied data buffer rx
      \param   oiMbxRxQ                   (required for rx / host out support) os_id of external supplied mailbox to annouce new data rx by its UDP_CSR[x] (27 bit, 26...0), RxBkFirst (at unused 14, 0= RX_DATA_BK0, 1= RX_DATA_BK1) and channel id (3 bit at unused 31...29)
      \param   *poiSemRx                  (required for rx / host out support) storage for os_id of driver taken internal semaphore to unlock data rx (triggers channel rx task to arm for next rx) after processing by consumer/handler to prepare/arm for next data
      \param   *pucTx                     (required for tx / host in support) external supplied data buffer tx
      \param   *poiMbxTxQ                 (required for tx / host in support) storage for os_id of driver internal mailbox (triggers channel tx task to copy ext buffer to fifo) to annouce (prepare/arm for) new data tx by size in bytes (11 bit, 26...16 like UDP_CSR), timeout (16 bit, 15...0) and new dir state (1 bit, 27) to set (in UDP_CSR, 7) after completion (tx only for IN types; forces dir IN for control type before data tx)
      \param   oiMbxTxQ                   (required for tx / host in support) os_id of external mailbox to unlock data buffer tx after processing and to send result code to (27 bit, 26...0)
      \param   oiSemMsgNew                (optional, set to NULL as default) OS_ID of <b>pre-initialized (to 0!)</b> semaphore to send/give for matching messages
      \param   ulMsgMask                  (optional, set to NULL as default) and-mask to include triggered messages for semaphore triggering

      \return                             > 0 for success, == 0 for failure

      \note
               - up to 2 tasks are spawned by this function (one task for each supported data direction of the specified type)
               - message bits are composed of filtered hardware bits (refer to <a href="#UdprawInitChannel">AT91SAM7X manual 34.6.10 pages 478-482: UDP_CSRx</a>) by <b>u32IrqMsg</b> of #SUdprawRO
               
      <b>Manual reference(s):</b><br>
      <a name="UdprawInitChannel">
      <a href="../../../MANUAL/AT91/AT91SAM7X/UDP/AT91SAM7X_34.6.10_p478-482_UDP_CSRx.pdf" type="application/pdf">AT91SAM7X manual 34.6.10 pages 478-482: UDP_CSRx</a><br>
*/
UCHAR  UdprawInitChannel    (UCHAR ucUnit, UCHAR ucChannel, USHORT usType, UCHAR *pucRx, OS_ID oiMbxRxQ, OS_ID *poiSemRx, UCHAR *pucTx, OS_ID *poiMbxTxQ, OS_ID oiMbxTxQ, OS_ID oiSemMsgNew, ULONG ulMsgMask);

//!   cancel any already read (fifo buffered) rx data (host-out) of specified channel (endpoint) on specified unit
/**
      \param   ucUnit                     number of the unit (descriptor array index <b>[not the hardware index]</b> of udprawdev.h)
      \param   ucChannel                  number of the channel (<b>hardware</b> endpoint number)

      \return                             > 0 for success, == 0 for failure

      \remarks
               - Dual-bank channels (endpoints) are automatically handled properly too
*/
UCHAR UdprawCancelRx        (UCHAR ucUnit, UCHAR ucChannel);

//!   cancel any pending tx data (host-in) of specified channel (endpoint) on specified unit
/**
      \param   ucUnit                     number of the unit (descriptor array index <b>[not the hardware index]</b> of udprawdev.h)
      \param   ucChannel                  number of the channel (<b>hardware</b> endpoint number)

      \return                             > 0 for success, == 0 for failure

      \note
               - UdprawCancelTx calls #UdprawCancelRx at its end

      \remarks
               - Dual-bank channels (endpoints) are automatically handled properly too
*/
UCHAR UdprawCancelTx        (UCHAR ucUnit, UCHAR ucChannel);

//!   retrieve message counters of a unit
/**
      \param   ucUnit                     number of the unit (descriptor array index <b>[not the hardware index]</b> of udprawdev.h)
      \param   ucChannel                  (unused, set 0) number of the sub-channel for units tree(s)
      \param   ulMsg                      message bits to retrieve (refer to <a href="#UdprawMsgUnit">AT91SAM7X manual 34.6.7 pages 474-475: UDP_ISR</a>)
      \param   usDecrement                decrement count (0 for no decrement) for selected message bits <b>(var type's max value USHRT_MAX* just clears the counter(s))</b>
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return                             summarized count of all selected message bits <b>before decrementing</b>

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max
               - message bits are composed of filtered hardware bits (refer to <a href="#UdprawMsgUnit">AT91SAM7X manual 34.6.7 pages 474-475: UDP_ISR</a>) by <b>u32IrqMsg</b> of #SUdprawRO
               
      \remarks
               - this function is helpful to determine the reason for the sent/given "news semaphore" when more than one message is masked
               
      <br><b>*)</b> refer to .\\Keil\\ARM\\RV31\\Inc\\limits.h for compiler type limit definitions
      
      <b>Manual reference(s):</b><br>
      <a name="UdprawMsgUnit">
      <a href="../../../MANUAL/AT91/AT91SAM7X/UDP/AT91SAM7X_34.6.7_p474-475_UDP_ISR.pdf" type="application/pdf">AT91SAM7X manual 34.6.7 pages 474-475: UDP_ISR</a><br>
*/
USHORT UdprawMsg            (UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout);


//!   retrieve message counters of a channel
/**
      \param   ucUnit                     number of the unit (descriptor array index <b>[not the hardware index]</b> of udprawdev.h)
      \param   ucChannel                  number of the channel (<b>hardware</b> endpoint number)
      \param   ulMsg                      message bits to retrieve count for (refer to <a href="#UdprawMsgChannel">AT91SAM7X manual 34.6.10 pages 478-482: UDP_CSRx</a>)
      \param   usDecrement                decrement count (0 for no decrement) for selected message bits <b>(var type's max value USHRT_MAX* just clears the counter(s))</b>
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)

      \return                             summarized count of all selected message bits <b>before decrementing</b>

      \note
               - when selecting MORE than ONE message bit, the true summarized count may be higher than the return value which is limited to the return data type's max
               - message bits are composed of filtered hardware bits (refer to <a href="#UdprawMsgChannel">AT91SAM7X manual 34.6.10 pages 478-482: UDP_CSRx</a>) by u32IrqMsg of _SUdprawRO

      \remarks
               <b>this function is useful to</b><br>
               - determine the reason for the sent/given "news semaphore" when more than one message is masked
               - track count of messages for easier debugging and verification
      
      <br><b>*)</b> refer to .\\Keil\\ARM\\RV31\\Inc\\limits.h for compiler type limit definitions
      
      <b>Manual reference(s):</b><br>
      <a name="UdprawMsgChannel">
      <a href="../../../MANUAL/AT91/AT91SAM7X/UDP/AT91SAM7X_34.6.10_p478-482_UDP_CSRx.pdf" type="application/pdf">AT91SAM7X manual 34.6.10 pages 478-482: UDP_CSRx</a><br>
*/
USHORT UdprawMsgChannel     (UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout);


#endif
