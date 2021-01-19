//!   internal interface of AT91SAM7X UDPRAW driver
/**
      \file    udprawint.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/UDP/RAW/UDPRAWINT.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2014-12-03 initial version
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_UDP_RAW_UDPRAWINT_H
#define SYKAM_CPU_AT91_UDP_RAW_UDPRAWINT_H 1


// Prototypes for functions

//!   \internal \brief <b>Internal function:</b> write to asynchronously clocked CSR[x] register
/**
      \param   ucUnit                     number of the unit (descriptor array index <b>[not the hardware index]</b> of udprawdev.h)
      \param   ucChannel                  number of the channel (<b>hardware</b> endpoint number)
      \param   u32Mask                    mask value for the CSR[x] register
      \param   bSet                       mode of operation: TRUE to set the mask bits, FALSE to clear the mask bits
      
      \return                             #TRUE for success, #FALSE for failure

      \note
               - any write to asynchronously clocked CSR[x] register must be read-back until it's complete (refer to <a href="#UdprawWriteCSR">refer to AT91SAM7X manual 34.6.10 pages 478-482: UDP_CSRx</a><br>
               
      <b>Manual reference(s):</b><br>
      <a name="UdprawWriteCSR">
      <a href="../../../MANUAL/AT91/AT91SAM7X/UDP/AT91SAM7X_34.6.10_p478-482_UDP_CSRx.pdf" type="application/pdf">AT91SAM7X manual 34.6.10 pages 478-482: UDP_CSRx</a><br>
*/
bool  UdprawSafeWriteCSR        (UCHAR ucUnit, UCHAR ucChannel, U32 u32Mask, bool bSet);


#endif
