/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                adcle.c
*
*  \brief               AT91SAM7X ADCLE (non pdc/dma) peripheral driver
*
*  \author              David Jaeger, Ruediger Radtke, Helmut Schoettner
*  \version             1.0
*
*  \date                2012-07-06     RR: Changed unsigned int values to ulong values, added type specifiers,
*                                      renamed function names, adapted name scheme from application<br>
*                       2012-07-11     RR: Changed init function to check ADC board file params and config pheriperal with
*                                      given params<br>
*                       2012-12-12     RR: Disabled "overrun" and "end of conversion" check in AdcGetConvertedData<br>
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
#include <math.h>

#include <AT91SAM7X512.h>
#include <lib_AT91SAM7X256.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "adcle.h"

//---------------------------------------------- globals definitions ---------------------------------------------------------------------------

OS_MUT   MUT_Adc;

//---------------------------------------------- functions -------------------------------------------------------------------------------------

uchar AdcGetConvertedData (uchar u8_SelectedChannelIds, ushort *pu16_Data, ushort u16_TimeoutSampleAndHold, ushort u16_TimeoutDriver)
{
   uchar       u8_Result         =  1;
   uchar       u8_Channel        =  0;
   uchar       u8_ChannelId;
static   AT91PS_ADC  pS_Adc            =  AT91C_BASE_ADC;
   
   
   // wait for driver access/lock driver
   if (OS_R_TMO != os_mut_wait ((OS_ID)MUT_Adc, u16_TimeoutDriver))
   {
      // start !one! sample & hold cycle
      AdcStartConversion (pS_Adc, u16_TimeoutDriver);
      
      // wait for finish sample & hold
      os_dly_wait (u16_TimeoutSampleAndHold);
      
      // retrieve now data from all channels which were selected by user
      u8_ChannelId = 1;
//      for (u8_ChannelId = 1; u8_ChannelId < (uchar)AT91C_ADC_CH7; u8_ChannelId = u8_ChannelId << 1)
      for (u8_Channel = 0; u8_Channel < AT91SAM7X__ADC_CHANNELS_MAX; u8_Channel++)
      {
         if (u8_ChannelId & u8_SelectedChannelIds)
         {
            // => check for "overun error" and "end of conversion" of current channel (shift about 8 digits
            //    is equivalent to overrun error field of channel# in status register)
            
//            if ((!(pS_Adc->ADC_SR & (ulong)(u8_ChannelId << 8))) && (pS_Adc->ADC_SR & (ulong)u8_ChannelId))
            if (pS_Adc->ADC_SR & (ulong)u8_ChannelId)
            {
               // get ADC channel data
               if (!(AdcGetConvertedDataSingleChannel (pS_Adc, u8_ChannelId, pu16_Data, u16_TimeoutDriver)))
               {
                  u8_Result = 0;
                  break;
               }
            }
//            else
//            {
//               u8_Result = 0;
//               break;
//            }
         }
         // increment channel number and buffer address
         pu16_Data++;
         u8_ChannelId = u8_ChannelId << 1;
      }
   
      // unlock ADC driver
      os_mut_release ((OS_ID)MUT_Adc);
   }
   // timeout error
   else
      u8_Result = 0;
   
   
   return (u8_Result);
}

//----------------------------------------------------------------------------------------------------------------------------------------------

uchar AdcGetConvertedDataSingleChannel (AT91S_ADC *pS_Adc, uchar u8_ChannelId, ushort *pu16_Data, ushort u16_TimeoutDriver)
{
   uchar u8_Result = 1;
   
      
   // wait for driver access/lock driver
//   if (OS_R_TMO != os_mut_wait ((OS_ID)MUT_Adc, u16_TimeoutDriver))
   if (1)
   {
      switch (u8_ChannelId)
      {
         case (uchar)AT91C_ADC_CH0: *pu16_Data = pS_Adc->ADC_CDR0; break;
         case (uchar)AT91C_ADC_CH1: *pu16_Data = pS_Adc->ADC_CDR1; break;
         case (uchar)AT91C_ADC_CH2: *pu16_Data = pS_Adc->ADC_CDR2; break;
         case (uchar)AT91C_ADC_CH3: *pu16_Data = pS_Adc->ADC_CDR3; break;
         case (uchar)AT91C_ADC_CH4: *pu16_Data = pS_Adc->ADC_CDR4; break;
         case (uchar)AT91C_ADC_CH5: *pu16_Data = pS_Adc->ADC_CDR5; break;
         case (uchar)AT91C_ADC_CH6: *pu16_Data = pS_Adc->ADC_CDR6; break;
         case (uchar)AT91C_ADC_CH7: *pu16_Data = pS_Adc->ADC_CDR7; break;
         default: u8_Result = 0;
      }
      // unlock ADC driver
//      os_mut_release ((OS_ID)MUT_Adc);
   }
   // timeout error
   else
      u8_Result = 0;
   
   
   return (u8_Result);
}

//----------------------------------------------------------------------------------------------------------------------------------------------

uchar AdcInit (uchar u8_ChannelsToEnable)
{
   uchar          u8_ChannelId;
   uchar          u8_Result      =  1;
   ulong          u32_Prescal;
   ulong          u32_Startup;
   ulong          u32_Shtim;
   
   AT91PS_ADC     pS_Adc         =  AT91C_BASE_ADC;
   tS_AdcConfig   S_AdcConfig;
   
   
   // init driver mutex
   os_mut_init (&MUT_Adc);
   
   // Example: 5 MHz operation, 20 µs startup time, 600 ns sample and hold time
   
   // prescaler rate selection
   //       u32_DesiredAdcClockInHz =   MCK / ( (u32_Prescal+1) * 2 )
   // <=>               u32_Prescal =  [MCK / (u32_DesiredAdcClockInHz * 2)] - 1
   //                               =  [48/(5*2)]-1 = 3,8
   //
   // adc startup time
   // u32_DesiredAdcStartupTimeInSeconds        =  (u32_Startup+1) * 8 / u32_DesiredAdcClockInHz
   // <=>                         u32_Startup   =  [(u32_DesiredAdcStartupTimeInSeconds * u32_DesiredAdcClockInHz)/8] - 1
   //                                           =  [(20 10e-6 * 5000000)/8]-1 = 11,5
   //
   // sample & hold time
   // u32_DesiredAdcSampleAndHoldTimeInSeconds     =  (u32_Shtim+1) / u32_DesiredAdcClockInHz
   // <=>                                u32_Shtim =  (u32_DesiredAdcSampleAndHoldTimeInSeconds * u32_DesiredAdcClockInHz)-1 
   //                                              =  (600 10e-9 * 5000000)-1 
   //                                              =  2
   
   // configure ADC with user params from board file
   
   // check config trigger enable
   S_AdcConfig.u8_TriggerEnable  =  (uchar)AT91SAM7X__ADC_TRIGGER_ON_OFF;
   if (     (S_AdcConfig.u8_TriggerEnable != (uchar)AT91C_ADC_TRGEN_DIS)
         && (S_AdcConfig.u8_TriggerEnable != (uchar)AT91C_ADC_TRGEN_EN)
      )
   {
      S_AdcConfig.u8_TriggerEnable  =  (uchar)AT91C_ADC_TRGEN_DIS;
   }
   // check config trigger selection
   S_AdcConfig.u8_TriggerSelect  =  (uchar)AT91SAM7X__ADC_TRIGGER_SELECTION;
   if (     (S_AdcConfig.u8_TriggerSelect != (uchar)AT91C_ADC_TRGSEL_TIOA0)
         && (S_AdcConfig.u8_TriggerSelect != (uchar)AT91C_ADC_TRGSEL_TIOA1)
         && (S_AdcConfig.u8_TriggerSelect != (uchar)AT91C_ADC_TRGSEL_TIOA2)
         && (S_AdcConfig.u8_TriggerSelect != (uchar)AT91C_ADC_TRGSEL_TIOA3)
         && (S_AdcConfig.u8_TriggerSelect != (uchar)AT91C_ADC_TRGSEL_TIOA4)
         && (S_AdcConfig.u8_TriggerSelect != (uchar)AT91C_ADC_TRGSEL_TIOA5)
         && (S_AdcConfig.u8_TriggerSelect != (uchar)AT91C_ADC_TRGSEL_EXT)
      )
   {
      // if trigger is not disabled, disable it now because no trigger is selected
      if (S_AdcConfig.u8_TriggerEnable != (uchar)AT91C_ADC_TRGEN_DIS)
         S_AdcConfig.u8_TriggerEnable =  (uchar)AT91C_ADC_TRGEN_DIS;
      
      S_AdcConfig.u8_TriggerSelect  =  (uchar)0;
   }
   
   // check config sleep mode
   S_AdcConfig.u8_SleepMode      =  (uchar)AT91SAM7X__ADC_SLEEP_ON_OFF;
   if (     (S_AdcConfig.u8_SleepMode != (uchar)AT91C_ADC_SLEEP_NORMAL_MODE)
         && (S_AdcConfig.u8_SleepMode != (uchar)AT91C_ADC_SLEEP_MODE)
      )
   {
      S_AdcConfig.u8_SleepMode   =  (uchar)AT91C_ADC_SLEEP_NORMAL_MODE;
   }
   
   // check config ADC resolution
   S_AdcConfig.u8_BitResolution  =  (uchar)AT91SAM7X__ADC_RESOLUTION;
   if (     (S_AdcConfig.u8_BitResolution != (uchar)AT91C_ADC_LOWRES_10_BIT)
         && (S_AdcConfig.u8_BitResolution != (uchar)AT91C_ADC_LOWRES_8_BIT)
      )
   {
      S_AdcConfig.u8_BitResolution  =  (uchar)AT91C_ADC_LOWRES_10_BIT;
   }
   
   // set remaining config params
   S_AdcConfig.u32_DesiredAdcClockInHz                         =  (ulong)AT91SAM7X__ADC_CLOCK_IN_HZ;
   S_AdcConfig.u32_DesiredAdcStartupTimeInMicroSeconds         =  (ulong)AT91SAM7X__ADC_STARTUP_TIME_IN_MICRO_SECONDS;
   S_AdcConfig.u32_DesiredAdcSampleAndHoldTimeInNanoSeconds    =  (ulong)AT91SAM7X__ADC_SAMPLE_AND_HOLD_TIME_IN_NANO_SECONDS;
   
   S_AdcConfig.u32_MasterClock                                 =  (ulong)AT91B_MCK;
   S_AdcConfig.u8_IdAdc                                        =  (uchar)AT91C_ID_ADC;
   
   // calc some mode register values
   u32_Prescal   =  (ulong)(rintf ((float)S_AdcConfig.u32_MasterClock / ((float)2.0 * (float)S_AdcConfig.u32_DesiredAdcClockInHz))) - 1;
   u32_Startup   =  (ulong)(rintf (((float)S_AdcConfig.u32_DesiredAdcClockInHz * ((float)S_AdcConfig.u32_DesiredAdcStartupTimeInMicroSeconds / (float)1000000.0)) / (float)8.0)) - 1;
   u32_Shtim     =  (ulong)(rintf ((float)S_AdcConfig.u32_DesiredAdcClockInHz * ((float)S_AdcConfig.u32_DesiredAdcSampleAndHoldTimeInNanoSeconds / (float)1000000000.0))) - 1;
   
   // check calculated mode register values
   if ((u32_Prescal < 64) && (u32_Startup < 32) && (u32_Shtim < 16))
   {
      // Enable peripheral clock
      AT91C_BASE_PMC->PMC_PCER = 1 << S_AdcConfig.u8_IdAdc;
      
      // Reset the controller
      pS_Adc->ADC_CR = AT91C_ADC_SWRST;
      
      // Write to the MR register
      AdcCfgModeReg ( pS_Adc,
                    ( S_AdcConfig.u8_TriggerEnable  & AT91C_ADC_TRGEN    )
                  | ( S_AdcConfig.u8_TriggerSelect  & AT91C_ADC_TRGSEL   )
                  | ( S_AdcConfig.u8_BitResolution  & AT91C_ADC_LOWRES   )
                  | ( S_AdcConfig.u8_SleepMode      & AT91C_ADC_SLEEP    )
                  | ( (u32_Prescal  << 8 )            & AT91C_ADC_PRESCAL  ) 
                  | ( (u32_Startup  << 16)            & AT91C_ADC_STARTUP  ) 
                  | ( (u32_Shtim    << 24)            & AT91C_ADC_SHTIM)   );
      
      
      // enable all channels which were selected by user in board file
      for (u8_ChannelId = 1; u8_ChannelId < (uchar)AT91C_ADC_CH7; u8_ChannelId = u8_ChannelId << 1)
      {
         if (u8_ChannelsToEnable & u8_ChannelId)
            pS_Adc->ADC_CHER = u8_ChannelId;
      }
   }
   else
      u8_Result = 0;
   
   
   return   (u8_Result);
}

//----------------------------------------------------------------------------------------------------------------------------------------------

uchar AdcStartConversion (AT91S_ADC *pS_Adc, ushort u16_TimeoutDriver)
{
   uchar u8_Result = 1;
   
   // wait for driver access/lock driver
   if (OS_R_TMO != os_mut_wait ((OS_ID)MUT_Adc, u16_TimeoutDriver))
   {
      pS_Adc->ADC_CR = AT91C_ADC_START;

      // unlock ADC driver
      os_mut_release ((OS_ID)MUT_Adc);
   }
   // timeout error
   else
      u8_Result = 0;
   
   
   return (u8_Result);
}

//----------------------------------------------------------------------------------------------------------------------------------------------

uchar AdcEnableChannel (AT91S_ADC *pS_Adc, uchar u8_ChannelId, ushort u16_TimeoutDriver)
{
   uchar u8_Result = 1;
   
      
   // wait for driver access/lock driver
   if (OS_R_TMO != os_mut_wait ((OS_ID)MUT_Adc, u16_TimeoutDriver))
   {
      // write to the CHER register
      pS_Adc->ADC_CHER = u8_ChannelId;

      // unlock ADC driver
      os_mut_release ((OS_ID)MUT_Adc);
   }
   else
      u8_Result = 0;
   
   
   return (u8_Result);
}

//----------------------------------------------------------------------------------------------------------------------------------------------

uchar AdcDisableChannel (AT91S_ADC *pS_Adc, uchar u8_ChannelId, ushort u16_TimeoutDriver)
{
   uchar u8_Result = 1;
   
      
   // wait for driver access/lock driver
   if (OS_R_TMO != os_mut_wait ((OS_ID)MUT_Adc, u16_TimeoutDriver))
   {
      // write to the CHDR register
      pS_Adc->ADC_CHDR = u8_ChannelId;

      // unlock ADC driver
      os_mut_release ((OS_ID)MUT_Adc);
   }
   // timeout error
   else
      u8_Result = 0;
   
   
   return (u8_Result);
}

//----------------------------------------------------------------------------------------------------------------------------------------------

uchar AdcSoftReset (AT91S_ADC *pS_Adc, ushort u16_TimeoutDriver)
{
   uchar u8_Result = 1;
   
      
   // wait for driver access/lock driver
   if (OS_R_TMO != os_mut_wait ((OS_ID)MUT_Adc, u16_TimeoutDriver))
   {
      pS_Adc->ADC_CR = AT91C_ADC_SWRST;

      // unlock ADC driver
      os_mut_release ((OS_ID)MUT_Adc);
   }
   // timeout error
   else
      u8_Result = 0;
   
   
   return (u8_Result);
}

//----------------------------------------------------------------------------------------------------------------------------------------------

uchar AdcEnableInterrupt (AT91S_ADC *pS_Adc, ulong u32_Flag, ushort u16_TimeoutDriver)
{
   uchar u8_Result = 1;
   
      
   // wait for driver access/lock driver
   if (OS_R_TMO != os_mut_wait ((OS_ID)MUT_Adc, u16_TimeoutDriver))
   {
      // write to the IER register
      if (u32_Flag > 0xFFFF)
      {
         // general interrupt
         pS_Adc->ADC_IER = u32_Flag;
      }
      else if (u32_Flag > 0xFF)
      {
         // overun interrupt
         pS_Adc->ADC_IER = 1 << u32_Flag << 8;
      }
      else
      {
         // channel interrupt
         pS_Adc->ADC_IER = 1 << u32_Flag;
      }
    
      // unlock ADC driver
      os_mut_release ((OS_ID)MUT_Adc);
   }
   // timeout error
   else
      u8_Result = 0;
   
   
   return (u8_Result);
}

//----------------------------------------------------------------------------------------------------------------------------------------------

uchar AdcDisableInterrupt (AT91S_ADC *pS_Adc, ulong u32_Flag, ushort u16_TimeoutDriver)
{
   uchar u8_Result = 1;
   
      
   // wait for driver access/lock driver
   if (OS_R_TMO != os_mut_wait ((OS_ID)MUT_Adc, u16_TimeoutDriver))
   {
      // write to the IDR register
      if (u32_Flag > 0xFFFF)
      {
         // general interrupt
         pS_Adc->ADC_IDR = u32_Flag;
      }
      else if (u32_Flag > 0xFF)
      {
         // overun interrupt
         pS_Adc->ADC_IDR = 1 << u32_Flag << 8;
      }
      else 
      {
         // channel interrupt
         pS_Adc->ADC_IDR = 1 << u32_Flag;
      }
    
      // unlock ADC driver
      os_mut_release ((OS_ID)MUT_Adc);
   }
   // timeout error
   else
      u8_Result = 0;
   
   
   return (u8_Result);
}

//----------------------------------------------------------------------------------------------------------------------------------------------

void AdcIsChannelInterruptStatusSet (AT91S_ADC *pS_Adc, uchar u8_ChannelId, uchar* pu8_Status)
{
    if((pS_Adc->ADC_IMR & u8_ChannelId) == u8_ChannelId)
    {
        *pu8_Status = 1;
    }
    else 
    {
        *pu8_Status = 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------------

void AdcCfgModeReg (AT91S_ADC *pS_Adc, ulong u32_Mode)
{
    // Clear the register
    pS_Adc->ADC_MR = 0;
    // Write to the MR register
    pS_Adc->ADC_MR = u32_Mode;
}
