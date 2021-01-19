//!	data definitions of AT91SAM7X ADC peripheral driver
/**
		\file		adcdata.h
*/

//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/CPU/AT91/ADC/ADCDATA.H
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2011-05-19 initial version
// 2011-05-25 added u8MR to _SAdcRO
// 2011-07-06 added old data argument to custom functions pAverage & pModify
// 2011-07-06 moved xData from _SAdcChannelRW to _SStorage and replace it with it
// 2011-07-07 changed _AdcTrg trigger values from ADC_PDCITEMTYPE to ADC_MEMITEMTYPE
// 2011-07-07 changed _SAdcCustom & _SStorage from ADC_PDCITEMTYPE to ADC_MEMITEMTYPE
// 2011-07-07 ADC_UNIT/CHANNEL_MSGMAX are now a zero based settings
// 2011-07-12 added ulMsgChangeMask to _SAdcRW, _SAdcChannelRW & _SAdcCfgChannel
// 2011-10-19 added public doxygen documentation
// 2011-10-26 added public doxygen documentation
// 2011-11-24 removed absolete _SAdcMsg structure
// 2011-12-16 update public doxygen documentation
// 2013-08-07 added SemTickRx to _SAdcRW for one-shot software mode
// 2013-08-07 added note to u16CycleRxMS of _SAdcRO for one-shot software mode
// 2013-08-22 changed cycle from milliseconds to ticks
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_CPU_AT91_ADC_ADCDATA_H
#define SYKAM_CPU_AT91_ADC_ADCDATA_H 1


//! Trigger band levels <b>(units are the result of the custom functions in _SAdcCustom and are therefore not always raw adc units)</b>
struct _SAdcTrg
{
   ADC_MEMITEMTYPE xMax;                                                         //!< Adc channel band max starts at this level (<b>greater</b> or equal) <b>[xMax and xAbove may have the same value to have MAX range only]</b>
   ADC_MEMITEMTYPE xAbove;                                                       //!< Adc channel band above starts at this level (<b>greater</b> or equal) <b>[xAbove and xMax may have the same value to have MAX range only]</b>
   ADC_MEMITEMTYPE xBelow;                                                       //!< Adc channel band below starts at this level (<b>less</b> or equal) <b>[xBelow and xMin may have the same value to have MIN range only]</b>
   ADC_MEMITEMTYPE xMin;                                                         //!< Adc channel band min starts at this level (<b>less</b> or equal) <b>[xMin and xBelow may have the same value to have MIN range only]</b>
};

//! Custom functions to modify raw data
/**
      <b>Example:</b><br>
      \include    _SAdcCustom.c
*/
struct _SAdcCustom
{
   /**
         \param   ucUnit                  number of the unit (descriptor array index <b>[not the hardware index]</b> of adcdev.h)
         \param   ucChannel               number of the channel
         \param   xDataNew                current (new) retrieved data value <b>(type returned by optional #pModify [type RAW for #pModify == NULL])</b>
         \param   xDataOld                previous (old) data value <b>(returned by optional #pModify [type RAW for #pModify == NULL])</b>
         \param   bValidOld               valid flag for <b>xDataOld</b> (#TRUE == valid)
         
         \return  modified (e.g. averaged) value <b>(you must at least return the unmodified value!!!)</b>

         \note
                  - function is called with the previous modified/scaled values (by optional #pModify) for xDataNew AND xDataOld<br>
*/
   ADC_MEMITEMTYPE (*pAverage)(UCHAR ucUnit, UCHAR ucChannel, ADC_MEMITEMTYPE xDataNew, ADC_MEMITEMTYPE xDataOld, bool bValidOld); //!< Adc channel custom function pointer for data averaging

   /**
         \param   ucUnit                  number of the unit (descriptor array index <b>[not the hardware index]</b> of adcdev.h)
         \param   ucChannel               number of the channel
         \param   xDataNew                current (new) retrieved <b>RAW</b> data value
         \param   xDataOld                previous (old) data value <b>(returned by optional #pModify [type RAW for #pModify == NULL])</b>
         \param   bValidOld               valid flag for <b>xDataOld</b> (#TRUE == valid)
         
         \return  modified (e.g. scaled) value <b>(you must at least return the unmodified value!!!)</b>
         
         \note
                  - function is called with the previous modified/scaled value (by optional #pModify) for xDataOld<br>
                  - function is called with the new <b>RAW</b> value of xDataNew
   */
   ADC_MEMITEMTYPE (*pModify)(UCHAR ucUnit, UCHAR ucChannel, ADC_PDCITEMTYPE xDataNew, ADC_MEMITEMTYPE xDataOld, bool bValidOld);  //!< Adc channel custom function pointer for data modification/scaling
};

//! Channel configuration structure item (for building array used by AdcInitUnit)
struct _SAdcCfgChannel
{
   UCHAR ucChannel;                                                              //!< Channel data is assigned to
   U16 u16To;                                                                    //!< Adc channel maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
   OS_ID oiSemMsgNew;                                                            //!< User supplied and <b>pre-initialized (to 0!)</b> samaphore (OS_ID) to give for each selected channel msg
   ULONG ulMsgMask;                                                              //!< Adc channel msg mask
   ULONG ulMsgChangeMask;                                                        //!< Adc channel msg change mask
   struct _SAdcTrg STrg;                                                         //!< Adc channel band trigger levels
   struct _SAdcCustom SCustom;                                                   //!< Adc channel custom function pointers
};

//! Hardware descriptor data (read only) for each unit
struct _SAdcRO
{
   AT91PS_ADC           pBase;                                                   //!< Base address of peripheral to handle
   U32                  u32PeriphId;                                             //!< Bit-Id (!) of peripheral to handle
   U32                  u32IrqRx;                                                //!< Mask of rx irq
   U32                  u32IrqMsg;                                               //!< Mask of irqs to monitor and count
   U8                   u8TskPriMsg;                                             //!< Priority of msg task
   U8                   u8TskPriRx;                                              //!< Priority of rx task
   U8                   u8MR;                                                    //!< Custom mode register options <b>(lowest 8 bit only: 2^7 to 2^0)</b>
   U32                  u32ClockHz;                                              //!< Clock of the unit in Hz
   U16                  u16StartupUS;                                            //!< Startup time in us
   U16                  u16SampleHoldNS;                                         //!< Sample and hold time in ns
   U16                  u16CycleRx;                                              //!< Delay before next rx trigger <b>(in ticks)</b> (set to 0 for one-shot software mode triggered by AdcRx/AdcContinue)
   void (*Isr) (void)__irq;                                                      //!< Isr handler to call for the peripheral
};

//! <b>Internal data:</b> Circular buffer handling (pdc rx buffer array handling)
struct _SCircular
{
   UCHAR                ucCntEntries;                                            //!< Count of used circular buffer entries
   UCHAR                ucIdxRead;                                               //!< Index for next read access
   UCHAR                ucIdxWrite;                                              //!< Index for next write access
   UCHAR                ucSize;                                                  //!< Number of available circular buffer entries
};

//! <b>Internal data:</b> Data storage
struct _SStorage
{
   ADC_MEMITEMTYPE      xData;                                                   //!< Adc channel rx data
   bool                 bValid;                                                  //!< Flag to indicate rx data is valid
};

//! <b>Internal data:</b> Runtime data (read/write) for each unit
struct _SAdcRW
{
   UCHAR                      ucUnit;                                            //!< Unit task is assigned to
   volatile ADC_PDCITEMTYPE   xPDCBufferRx[ADC_PDCDEPTHRX][ADC_CHANNEL_MAX];     //!< Pdc rx buffers
   ADC_MSGCOUNTERTYPE         xMsgCounter[ADC_UNIT_MSGMAX+ 1];                   //!< Array with msg counters 
   OS_ID                      oiSemMsgNew;                                       //!< User supplied samaphore (ID) to give for each enhanced and masked channel msg 
   ULONG                      ulMsgMask;                                         //!< Adc unit msg mask
   ULONG                      ulMsgChangeMask;                                   //!< Adc unit msg change mask
   USHORT                     usChannelMask;                                     //!< Channel mask to combine enhanced channel messages for setting oiSemMsgEnh
   U16                        u16PDCMaxRx;                                       //!< Size of dma (pdc) buffer (true maximum transfer block size)
   OS_SEM                     SemMsg;                                            //!< Semaphore to lock msg data
   OS_SEM                     SemRx;                                             //!< Semaphore to lock rx data
   OS_SEM                     SemTickRx;                                         //!< Semaphore to re-trigger tick cycle
   OS_TID                     tidTskMsg;                                         //!< OS_ID of msg task
   OS_TID                     tidTskMsgChannel;                                  //!< OS_ID of channel msg task
   OS_TID                     tidTskRx;                                          //!< OS_ID of rx task
   struct _SCircular          SCircular;                                         //!< Circular buffer handling (pdc rx buffer array handling)
   UCHAR                      ucFlags;                                           //!< Adc unit flags (refer to adc.h)
};

//! <b>Internal data:</b> Runtime data (read/write) for each channel
struct _SAdcChannelRW
{
   UCHAR                ucUnit;                                                  //!< Unit task is assigned to
   struct _SAdcTrg      STrg;                                                    //!< Adc channel band trigger levels
   struct _SAdcCustom   SCustom;                                                 //!< Adc channel custom function pointers
   ADC_MSGCOUNTERTYPE   xMsgCounter[ADC_CHANNEL_MSGMAX+ 1];                      //!< Adc channel msg array with msg counters
   OS_ID                oiSemMsgNew;                                             //!< User supplied samaphore (ID) to give for each selected channel msg
   ULONG                ulMsgMask;                                               //!< Adc channel msg mask
   ULONG                ulMsgChangeMask;                                         //!< Adc channel msg change mask
   OS_SEM               SemMsg;                                                  //!< Adc channel semaphore to lock msg data
   OS_SEM               SemRx;                                                   //!< Adc channel semaphore to lock rx data
   OS_SEM               SemRdy;                                                  //!< Adc channel semaphore to pend for rx data ready
   struct _SStorage     SStorage;                                                //!< Adc channel rx data
   U16                  u16To;                                                   //!< Adc channel time out
   UCHAR                ucFlags;                                                 //!< Adc channel flags (refer to adc.h)
};

#endif
