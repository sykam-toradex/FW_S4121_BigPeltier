//!	defines and configuration of AT91SAM7X ADCLE (non pdc/dma) peripheral driver

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                adcle.h
*
*  \brief               header of AT91SAM7X ADCLE (non pdc/dma) peripheral driver
*
*  \author              David Jaeger, Ruediger Radtke, Helmut Schoettner
*  \version             1.0
*
*  \date                2012-07-06     RR: Changed unsigned int values to ulong values, added type specifiers,
*                                      added struct "_S_AdcConfig"<br>
*                       2013-07-31     HS: Changed for co-existence (alternate driver!) with already existing dma/pdc ADC driver<br>
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                
*
*  \todo                Redundant to the already available ADC (with full dma/pdc support) driver!
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2012-2016 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef SYKAM_CPU_AT91_ADC_ADCLE_H
#define SYKAM_CPU_AT91_ADC_ADCLE_H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

//---------------------------------------------- structures ------------------------------------------------------------------------------------

typedef struct _S_AdcConfig
{
   uchar    u8_IdAdc;
   uchar    u8_TriggerEnable;
   uchar    u8_TriggerSelect;
   uchar    u8_SleepMode;
   uchar    u8_BitResolution;
   ulong    u32_MasterClock;
   ulong    u32_DesiredAdcClockInHz;
   ulong    u32_DesiredAdcStartupTimeInMicroSeconds;
   ulong    u32_DesiredAdcSampleAndHoldTimeInNanoSeconds;
   
}  tS_AdcConfig;

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

uchar    AdcGetConvertedData              (uchar      u8_Channel,             ushort   *pu16_Data,          ushort   u16_TimeoutSampleAndHold,  ushort   u16_TimeoutDriver);
uchar    AdcGetConvertedDataSingleChannel (AT91S_ADC  *pS_Adc,                uchar    u8_ChannelId,        ushort   *pu16_Data,                ushort   u16_TimeoutDriver);
uchar    AdcInit                          (uchar      u8_ChannelsToEnable);
uchar    AdcStartConversion               (AT91S_ADC  *pS_Adc,                ushort   u16_TimeoutDriver);
uchar    AdcEnableChannel                 (AT91S_ADC  *pS_Adc,                uchar    u8_ChannelId,        ushort   u16_TimeoutDriver);
uchar    AdcDisableChannel                (AT91S_ADC  *pS_Adc,                uchar    u8_ChannelId,        ushort   u16_TimeoutDriver);
uchar    AdcSoftReset                     (AT91S_ADC  *pS_Adc,                ushort   u16_TimeoutDriver);
uchar    AdcEnableInterrupt               (AT91S_ADC  *pS_Adc,                ulong    u32_Flag,            ushort   u16_TimeoutDriver);
uchar    AdcDisableInterrupt              (AT91S_ADC  *pS_Adc,                ulong    u32_Flag,            ushort   u16_TimeoutDriver);

void     AdcIsChannelInterruptStatusSet   (AT91S_ADC  *pS_Adc,                uchar    u8_ChannelId,        uchar*   pu8_Status);
void     AdcCfgModeReg                    (AT91S_ADC  *pS_Adc,                ulong    u32_Mode);


#endif
