//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/DEVICE/S5151_COOL/SRC/AT91SAM7X.c
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner, rradtke
//----------------------------------------------------------------------------
// 2010-05-05 imported from at91sam7x-s1025m.c
// 2010-05-05 added conditional compile query
// 2010-05-06 changes for different mcp2317 signals
// 2010-07-01 added pwm channel list entries to SPwmList
// 2010-07-28 added include AT91SAM7X.h
// 2010-07-29 fixed includes for merged "include cleanuped" framework files
// 2010-08-11 conditional compile of SPwmList by define AT91F_PWMLIST_SIZE
//----------------------------------------------------------------------------
// Copyright (c) 2008-2013 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Include
#include <AT91SAM7X256.H>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"

#include "..\..\..\rtl\tmr\tmr.h"
#include "..\..\..\rtl\tmr\tmrdata.h"

#include "..\..\..\cpu\at91\pio\piodata.h"

#include "..\..\..\cpu\at91\pwm\pwm.h"
#include "..\..\..\cpu\at91\pwm\pwmdata.h"

#include "..\..\..\cpu\at91\misc\sysirq\sysirq.h"
#include "..\..\..\cpu\at91\misc\sysirq\sysirqdata.h"

#include "..\..\..\cpu\at91\rtt\rtt.h"

#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317data.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"

#include "..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085.h"
#include "..\..\..\ext\spi\nvram\fm25clss\fm25clss.h"
#include "..\..\..\ext\misc\encgapdisk\encgapdisk.h"
#include "..\..\..\cpu\at91\misc\stpclkdir\stpclkdir.h"

#include "..\src\S6510\valve_act_support_tasks.h"
#include "..\src\S6510\valve_act_modbus_funcs.h"
#include "..\src\column_oven_support_tasks.h"
#include "..\src\column_oven_modbus_funcs.h"
#include "..\..\shared\src\common_modbus_funcs.h"

// Suppress warning for "#188-D: enumerated type mixed with another type" to allow ORed enum bit constants here
#pragma diag_remark 188

#ifdef AT91F_TMRLIST_SIZE
   #if AT91F_TMRLIST_SIZE > 0
      struct _STmrItem STmrItemList[AT91F_TMRLIST_SIZE]=
      {
      // val                    , val2,                                     compare          , fpointer
         _TIMER(MBPB_TIMER_BASE), _TIMER(MBPB_TIMER_BASE+ MBPB_PORT_COUNT), TMR_RANGE        , MBPBTmrHandler,       // MBPBTmrHandler requires timers with id range (MBPB_TIMER_BASE)- (MBPB_TIMER_BASE+ MBPB_PORT_COUNT)
      };
   #endif
#endif

#ifdef AT91F_SYSIRQLIST_SIZE
   #if AT91F_SYSIRQLIST_SIZE > 0
      struct _SSysIrqItem SSysIrqList[AT91F_SYSIRQLIST_SIZE]=
      {
         #if SYSIRQ_RTT_ENABLE > 0
            RTT_ISR_NAME(0)
         #endif
      };
   #endif
#endif


#ifdef AT91F_PWMLIST_SIZE
   #if AT91F_PWMLIST_SIZE > 0
      struct _SPwmItem SPwmList[AT91F_PWMLIST_SIZE]=
      {
         #ifdef PWM_CHANNEL_MAX
            #if PWM_CHANNEL_MAX > 0
               AT91C_PWMC_CHID0, IsrPwmChannel,
            #endif
            #if PWM_CHANNEL_MAX > 1
               AT91C_PWMC_CHID1, IsrPwmChannel,
            #endif
            #if PWM_CHANNEL_MAX > 2
               AT91C_PWMC_CHID2, IsrPwmChannel,
            #endif
            #if PWM_CHANNEL_MAX > 3
               AT91C_PWMC_CHID3, IsrPwmChannel,
            #endif
         #endif
         #ifdef STPCLKDIR_UNIT_MAX
	         #if STPCLKDIR_UNIT_MAX > 0
		         AT91C_PWMC_CHID1, IsrStpClkDirPwm,
            #endif
            #if STPCLKDIR_UNIT_MAX > 1
		         AT91C_PWMC_CHID2, IsrStpClkDirPwm,
            #endif
            #if STPCLKDIR_UNIT_MAX > 2
		         AT91C_PWMC_CHID3, IsrStpClkDirPwm
            #endif
         #endif
      };
   #endif
#endif
   
//                            				mask,                      pio_type,                     pio_trg_type,          user_fpointer called in driver task,                     pio_attr 

struct _SMcp2317Item SMcp2317List[MCP2317_LIST_SIZE]=
{
	/* GPIO0:SYS0_RDY       		      */ MCP2317_PA0             , MCP2317_TYPE_OUTPUT_SET1    , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET,
	/* GPIO0:/SYS0_STANDBY   	         */ MCP2317_PA1             , MCP2317_TYPE_OUTPUT_SET1    , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET,
	/* GPIO0:RS485_RXDIS      	         */ MCP2317_PA2             , MCP2317_TYPE_OUTPUT_SET1    , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET,
	/* GPIO0:TWI_RDY      			      */ MCP2317_PA3             , MCP2317_TYPE_INPUT          , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET,
	/* PA4 (AVAILABLE) 					   */ MCP2317_PA4             , MCP2317_TYPE_INPUT          , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET | MCP2317_ATTR_PULLUP_ON,
	/* MOT1:MS1 ('MOTOR 1 STEP WIDTH')  */ MCP2317_PA5             , MCP2317_TYPE_OUTPUT_SET1    , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET,
	/* MOT2:MS2 ('MOTOR 2 STEP WIDTH')  */ MCP2317_PA6             , MCP2317_TYPE_OUTPUT_SET1    , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET,
	/* MOT1:/MOT_RES1 ('MOTOR 1 RESET') */ MCP2317_PA7             , MCP2317_TYPE_OUTPUT_SET0    , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET,
	/* MOT2:/MOT_RES2 ('MOTOR 2 RESET') */ MCP2317_PB0             , MCP2317_TYPE_OUTPUT_SET0    , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET,
	/* PB1 (RESERVED)       			   */ MCP2317_PB1             , MCP2317_TYPE_INPUT          , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET | MCP2317_ATTR_PULLUP_ON,
	/* PB2 (RESERVED)       			   */ MCP2317_PB2             , MCP2317_TYPE_INPUT          , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET | MCP2317_ATTR_PULLUP_ON,
	/* UIO:/START_HEAT (INPUT)   		   */ MCP2317_PB3             , MCP2317_TYPE_INPUT          , MCP2317_TRG_FALL      , ColumnOvenSupportTasks_ExternControlContactHandler     , MCP2317_ATTR_RESET,
	/* UIO:/POSHOME (INPUT) 			   */ MCP2317_PB4             , MCP2317_TYPE_INPUT          , MCP2317_TRG_FALL      , ColumnOvenSupportTasks_ExternControlContactHandler     , MCP2317_ATTR_RESET,
	/* UIO:/POSNEXT (INPUT)				   */ MCP2317_PB5             , MCP2317_TYPE_INPUT          , MCP2317_TRG_FALL      , ColumnOvenSupportTasks_ExternControlContactHandler     , MCP2317_ATTR_RESET,
	/* UIO:/INJECT  (OUTPUT)			   */ MCP2317_PB6             , MCP2317_TYPE_OUTPUT_SET1    , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET,
	/* UIO:/ERROR   (OUTPUT)			   */ MCP2317_PB7             , MCP2317_TYPE_OUTPUT_SET1    , MCP2317_TRG_ANY       , NULL                                                   , MCP2317_ATTR_RESET
};

   
struct _SPioItem SPioListA[AT91F_PIOLIST_SIZEA]=
{
	/* USART0:RXD0          			   */ AT91C_PA0_RXD0          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* USART0:TXD0          			   */ AT91C_PA1_TXD0          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* LED0:/ALIVE          			   */ AT91C_PIO_PA2           , AT91C_PIOTYPE_OUTPUT_SET1   , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* USART0:RTS0          			   */ AT91C_PA3_RTS0          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* USART0:CTS0          			   */ AT91C_PA4_CTS0          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* USART1:RXD1 (RS485)  			   */ AT91C_PA5_RXD1          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* USART1:TXD1 (RS485)  			   */ AT91C_PA6_TXD1          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* DISP0:/ATN           			   */ AT91C_PIO_PA7           , AT91C_PIOTYPE_OUTPUT_SET1   , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* USART1:TX_EN (RS485) 			   */ AT91C_PA8_RTS1          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* ETH0 /IRQ (RESERVED) 			   */ AT91C_PIO_PA9           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* I2C0:TWD  (RESERVED) 			   */ AT91C_PA10_TWD          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* I2C0:TWCK (RESERVED) 			   */ AT91C_PA11_TWCK         , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* SPI0:NSS             			   */ AT91C_PA12_SPI0_NPCS0   , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* CLK0:PCK1 (AVAILABLE)			   */ AT91C_PA13_PCK1         , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* AIC0:IRQ1 (RESERVED) 			   */ AT91C_PA14_IRQ1         , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* TC0:TCLK2 (RESERVED) 			   */ AT91C_PA15_TCLK2        , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* SPI0:MISO            			   */ AT91C_PA16_SPI0_MISO    , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* SPI0:MOSI            			   */ AT91C_PA17_SPI0_MOSI    , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* SPI0:SPCK            			   */ AT91C_PA18_SPI0_SPCK    , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* MCP2317:/PA19:/GPIO0_IRQ ('INTA')*/ AT91C_PIO_PA19          , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_FALL     , MCP2317_ISR_NAME(0)             , MCP2317_INIT_NAME(0)            , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON | AT91C_PIOATTR_DEGLITCH_ON,
	/* MCP2317:/PA20:/GPIO1_IRQ ('INTB')*/ AT91C_PIO_PA20          , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_FALL     , NULL                            , NULL                           , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON | AT91C_PIOATTR_DEGLITCH_ON,
	/* SPI1:NPCS0           			   */ AT91C_PA21_SPI1_NPCS0   , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* SPI1:SPCK            			   */ AT91C_PA22_SPI1_SPCK    , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* SPI1:MOSI            			   */ AT91C_PA23_SPI1_MOSI    , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* SPI1:MISO            			   */ AT91C_PA24_SPI1_MISO    , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* SPI1:NPCS1           			   */ AT91C_PA25_SPI1_NPCS1   , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* SPI1:NPCS2           			   */ AT91C_PA26_SPI1_NPCS2   , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* CLK0:PCK3 (AVAILABLE)			   */ AT91C_PA27_PCK3         , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* GPIO1:/IRQ (RESERVED)			   */ AT91C_PIO_PA28          , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* AIC0:FIRQ (RESERVED) 			   */ AT91C_PA29_FIQ          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET,
	/* AIC0:IRQ0 (RESERVED) 			   */ AT91C_PA30_IRQ0         , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                            , NULL                           , AT91C_PIOATTR_RESET
};

//                            				mask,                        pio_type,                     pio_trg_type,        isr_fpointer,                 pio_attr 

struct _SPioItem SPioListB[AT91F_PIOLIST_SIZEB]=
{
#ifdef ETX
	/* ETH0:ETXCK           			   */ AT91C_PB0_ETXCK_EREFCK  , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* ETH0:ETXEN           			   */ AT91C_PB1_ETXEN         , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* ETH0:ETX0            			   */ AT91C_PB2_ETX0          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* ETH0:ETX1            			   */ AT91C_PB3_ETX1          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* ETH0:ERX0            			   */ AT91C_PB5_ERX0          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* ETH0:ERX1            			   */ AT91C_PB6_ERX1          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* ETH0:ERXER           			   */ AT91C_PB7_ERXER         , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* ETH0:EMDC            			   */ AT91C_PB8_EMDC          , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* ETH0:EMDIO           			   */ AT91C_PB9_EMDIO         , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* ETH0:ERXDV           			   */ AT91C_PB15_ERXDV_ECRSDV , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,   
#else // Unused pins all set for input
	/* (RESERVED)           			   */ AT91C_PIO_PB0           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* (RESERVED)           			   */ AT91C_PIO_PB1           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* (RESERVED)           			   */ AT91C_PIO_PB2           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* (RESERVED)           			   */ AT91C_PIO_PB3           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* (RESERVED)           			   */ AT91C_PIO_PB5           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* (RESERVED)           			   */ AT91C_PIO_PB6           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* (RESERVED)           			   */ AT91C_PIO_PB7           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* (RESERVED)           			   */ AT91C_PIO_PB8           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* (RESERVED)           			   */ AT91C_PIO_PB9           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
#endif
	/* OVEN:FAN           			      */ AT91C_PIO_PB15          , AT91C_PIOTYPE_OUTPUT_SET1   , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* SYNC:/LS1 ('LIGHT BARRIER 1')	   */ AT91C_PIO_PB4           , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , ENCGAPDISK_ISR_NAME(0), ENCGAPDISK_INIT_NAME(0), AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_OFF | AT91C_PIOATTR_DEGLITCH_ON,
	/* SYNC:/LS2 ('LIGHT BARRIER 2') 	*/ AT91C_PIO_PB10          , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_OFF | AT91C_PIOATTR_DEGLITCH_ON,
	/* OVEN:OVERCURRENT          		   */ AT91C_PIO_PB11          , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* MOT1:TC0:TCLK0 (PWM_OUT1) 		   */ AT91C_PB12_TCLK0        , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* OVEN:HEAT/COOL          		   */ AT91C_PIO_PB13          , AT91C_PIOTYPE_OUTPUT_SET1   , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* OVEN:/PELTIER_EN          		   */ AT91C_PIO_PB14          , AT91C_PIOTYPE_OUTPUT_SET1   , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* SPI1:NPCS3           			   */ AT91C_PB16_SPI1_NPCS3   , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* (AVAILABLE)          			   */ AT91C_PIO_PB17          , AT91C_PIOTYPE_INPUT         , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET | AT91C_PIOATTR_PULLUP_ON,
	/* ADC0:ADTRG (RESERVED)			   */ AT91C_PB18_ADTRG        , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* MOT2:TC1:TCLK1 (PWM_OUT2)		   */ AT91C_PB19_TCLK1        , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* MOT1:PWM1						      */ AT91C_PB20_PWM1         , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* MOT2:PWM2						      */ AT91C_PB21_PWM2         , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* PWM3 (RESERVED)        			   */ AT91C_PB22_PWM3         , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* MOT1:TC0:TIOA0 (TIOA0)    		   */ AT91C_PB23_TIOA0        , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_FALL     , ENCGAPDISK_ISRG_NAME(0), ENCGAPDISK_INITG_NAME(0), AT91C_PIOATTR_RESET,
	/* MOT1:DIR	(DIR)				         */ AT91C_PIO_PB24          , AT91C_PIOTYPE_OUTPUT_SET1   , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* MOT1:TC1:TIOA1 (MOT_EN1)         */ AT91C_PB25_TIOA1        , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* MOT2:DIR	(DIR)				         */ AT91C_PIO_PB26          , AT91C_PIOTYPE_OUTPUT_SET1   , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* MOT2:TC2:TIOA2 (MOT_EN2)    	   */ AT91C_PB27_TIOA2        , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* TC2:TIOB2 (RESERVED)  			   */ AT91C_PIO_PB28          , AT91C_PIOTYPE_OUTPUT_SET1   , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* (AVAILABLE)							   */ AT91C_PB29_PCK1         , AT91C_PIOTYPE_PERIPH        , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET,
	/* ADC0:AD3 (GAS_HEAT_SENSE)	      */ AT91C_PIO_PB30          , AT91C_PIOTYPE_PERIPH_ALT    , AT91C_PIOTRG_ANY      , NULL                  , NULL                  , AT91C_PIOATTR_RESET
};


tS_AT91SAM7X_MainboardVoltageMonitoring SA_AT91SAM7X_MainboardVoltageMonitoring[AT91SAM7X__ADC_CHANNELS_MAX] =
{
                        // voltage divider resistance near                                // voltage divider resistance far                              // voltage threshold low                                          // voltage threshold high                                         // reference voltage
   /* ADC channel 0 */  {0,                                                               0,                                                             0,                                                                0,                                                                0,                                           },
   /* ADC channel 1 */  {0,                                                               0,                                                             0,                                                                0,                                                                0,                                           },
   /* ADC channel 2 */  {0,                                                               0,                                                             0,                                                                0,                                                                0,                                           },
   /* ADC channel 3 */  {0,                                                               0,                                                             0,                                                                0,                                                                0,                                           },
   /* ADC channel 4 */  {(ulong)AT91SAM7X__ADC_CH4_VOLTAGE_DIVIDER_RESISTANCE_TO_GND,     (ulong)AT91SAM7X__ADC_CH4_VOLTAGE_DIVIDER_RESISTANCE_TO_POWER, (ushort)AT91SAM7X__ADC_CH4_VOLTAGE_THRESHOLD_IN_MILLIVOLT_LOW,    (ushort)AT91SAM7X__ADC_CH4_VOLTAGE_THRESHOLD_IN_MILLIVOLT_HIGH,   (ushort)AT91SAM7X__ADC_CH4_VOLTAGE_REFERENCE },
   /* ADC channel 5 */  {(ulong)AT91SAM7X__ADC_CH5_VOLTAGE_DIVIDER_RESISTANCE_TO_GND,     (ulong)AT91SAM7X__ADC_CH5_VOLTAGE_DIVIDER_RESISTANCE_TO_POWER, (ushort)AT91SAM7X__ADC_CH5_VOLTAGE_THRESHOLD_IN_MILLIVOLT_LOW,    (ushort)AT91SAM7X__ADC_CH5_VOLTAGE_THRESHOLD_IN_MILLIVOLT_HIGH,   (ushort)AT91SAM7X__ADC_CH5_VOLTAGE_REFERENCE },
   /* ADC channel 6 */  {0,                                                               0,                                                             0,                                                                0,                                                                0,                                           },
   /* ADC channel 7 */  {0,                                                               0,                                                             0,                                                                0,                                                                0,                                           },
   
};

const float cf_AT91SAM7X_TemperatureSampleTimeInSysTicks = (float)AT91SAM7X__REPETITION_TICKS_FOR_TSK_GLOBAL_TIMER;

//---------------------------------------------- functions -------------------------------------------------------------------------------------

bool AT91SAM7X_FilterForModbusRequests  (uchar u8_ModbusDataType, U32 u32_ModbusAddress/*ushort u16_ModbusAddress*/, ushort *pu16_ModbusData)
{
   bool  b_Result = (bool)TRUE;

   // Caution: Driver context!
   // Default action is TRUE to accept incoming write requests on holding register or coils.
   // Setting the b_Result to FALSE declines the request for u16_ModbusAddress and does NOT change the holding register or coil.
   // Furthermore an exception message will be send back to the requester.

   // call filter functions from different modules (use "&" to ensure "b_Result = FALSE" in case func1 returns "FALSE" and func2 "TRUE"
   b_Result &= CommonModbusFuncs_FilterForModbusRequests    (u8_ModbusDataType, u32_ModbusAddress/* u16_ModbusAddress*/, pu16_ModbusData);
   b_Result &= ValveActModbusFuncs_FilterForModbusRequests  (u8_ModbusDataType, u32_ModbusAddress/* u16_ModbusAddress*/, pu16_ModbusData);
   b_Result &= ColumnOvenModbusFuncs_FilterForModbusRequests(u8_ModbusDataType, u32_ModbusAddress/* u16_ModbusAddress*/, pu16_ModbusData);

   return b_Result;
}
