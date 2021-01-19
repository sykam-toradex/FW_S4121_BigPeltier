//----------------------------------------------------------------------------
// A T 9 1  C P U  S U P P O R T  L I B R A R Y
//----------------------------------------------------------------------------
// Name:    SYKAM/DEVICE/S4121/SRC/AT91SAM7X.h
// Purpose: AT91 CPU SUPPORT
// Author:  hschoettner, rradtke
//----------------------------------------------------------------------------
// 2010-05-05 imported from at91sam7x-s1025m.h
// 2010-05-06 added STPCLKDIR_DAC_HOLD, STPCLKDIR_DAC_RUN & STPCLKDIR_MODULO
// 2010-05-06 added defines for ENCGAPDISK
// 2010-05-07 rr: changed ENCGAPDISK0_CONTINOUS to TRUE
// 2010-07-01 changed AT91F_PWMLIST_SIZE for pwm channel support
// 2010-07-15 rr: fixed MCK value according to settings in SAM7.s, changed OS_SIGNAL to OS_TIMER
// 2010-12-02 increased MCP2317_STACK_TSKNOTIFYMCP2317 to 2048
// 2011-02-09 rr: imported AT91SAM7X.h from S6510, changed SPI channel for DAC
// 2011-03-17 reduced MCP2317_STACK_TSKNOTIFYMCP2317 to 512
// 2011-03-17 added MCP2317_STACK_TSKNOTIFYHELPERMCP2317 at 1024
// 2011-05-11 increased MCP2317_STACK_TSKNOTIFYMCP2317 to 1536
// 2011-05-11 increased MCP2317_STACK_TSKNOTIFYHELPERMCP2317 to 2048
//----------------------------------------------------------------------------
// Copyright(c) 2008-2013 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef AT91SAM7X_H
#define AT91SAM7X_H

// define ONLY ONE of the following flags for code configuration of SPI0
// display is master on SPI0 (schematic name)
#define  AT91SAM7X__SPI0_MASTER_IS_DISPLAY
// board S5151 is master on SPI0 (schematic name)
//#define  AT91SAM7X__SPI0_MASTER_IS_AUTOSAMPLER


#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   #undef   AT91SAM7X__SPI0_MASTER_IS_AUTOSAMPLER
   #undef   AT91SAM7X__SPI0_MASTER_IS_S5300
#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_AUTOSAMPLER
   #undef   AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   #define  AT91SAM7X__SPI0_MASTER_IS_S5300
#endif



/**<  Remarks for on-chip clocks
 
 1) According to settings in SAM7.s the following values are present: "PLL_MUL = 25, PLL_DIV = 5 and MCK prescaler = 2".  
 2) The resultant PLL output is (18432000 * 26) / 5 = 95.486400 MHz = PLLCK.

 3) The selected operating clock is PLLCK; master clock prescaler = 2 
    ==> ( ((18432000 * 26)/5) / 2 ) = 47.923200 MHz = MCK.   

 4) These settings in SAM7.s (section PMC) should not be changed, otherwise the PLL filter must
    be adapted to desired frequency.
*/
#define  AT91B_MAIN_OSC                                           (18432000)        /**<  main oscillator ==> MAINCK                 */
#define  AT91B_MCK                                                (47923200)        /**<  master clock    ==> MCK (after prescaling) */
#define  OS_TICK_RATE                                             (1000)            /**< 	set to same value as OS_TICK in RTX_Config.c !!! */


// modbus default slave identifier
#define  AT91SAM7X__MODBUS_DEFAULT_SLAVE_ID                       (0x0F)
#define  AT91SAM7X__MODBUS_MAX_SLAVE_ID                           (0x0F)
// modbus device baudrates
#define  AT91SAM7X__BAUDRATE_RS232                                (115200)
#define  AT91SAM7X__BAUDRATE_RS485                                (115200)

// LEDs
#define  AT91B_LED_ALIVE			                                 (1 << 2)
#define  AT91D_BASE_PIO_LED		                                 (AT91C_BASE_PIOA)


// on-chip peripherals
// pins
#define  AT91SAM7X__BASE_PIO__DISP0_ATN                           (AT91C_BASE_PIOA)
#define  AT91SAM7X__PIN__DISP0_ATN                                (AT91C_PIO_PA7)
#define  AT91SAM7X__PIN__COLUMN_OVEN_POWER                        (AT91C_PIO_PB14)
#define  AT91SAM7X__PIN__COLUMN_OVEN_HEAT_COOL                    (AT91C_PIO_PB13)

// list sizes used in coressponding driver modules
#define  AT91F_TMRLIST_SIZE               	                     (1)

#define  PWM_CHANNEL_MAX                  	                     (0)

#define  AT91F_PWMLIST_SIZE    		                              ((STPCLKDIR_UNIT_MAX) + (PWM_CHANNEL_MAX))
#define  AT91F_PIOLIST_SIZEA   		                              (31)
#define  AT91F_PIOLIST_SIZEB   		                              (31)
#undef   ETX

// ADC config
#define  AT91SAM7X__ADC_CHANNELS_MAX                              (8)
#define  AT91SAM7X__ADC_CH4_VOLTAGE_DIVIDER_RESISTANCE_TO_GND     (3000)                       // in Ohm
#define  AT91SAM7X__ADC_CH4_VOLTAGE_DIVIDER_RESISTANCE_TO_POWER   (27000)
#define  AT91SAM7X__ADC_CH4_VOLTAGE_THRESHOLD_IN_MILLIVOLT_LOW    (23280)                       // VRef_max = (3% * 24V) + 24 V = 23.28 V; VAdc_nom = 24 V - (24 V * (100k/(100k+10k))) = 2.181 V; VAdc_max = VAdc_nom + tolerance_3%@24V_with_tolerance_1%@VoltageDivider = 2.18 V + 0.655 V = 2.836 V
#define  AT91SAM7X__ADC_CH4_VOLTAGE_THRESHOLD_IN_MILLIVOLT_HIGH   (24720)                       // VRef_min = - (3% * 24V) + 24 V = 24.72 V; VAdc_min = 1.526 V;
#define  AT91SAM7X__ADC_CH4_VOLTAGE_REFERENCE                     (24000)                       // in [mV]
#define  AT91SAM7X__ADC_CH5_VOLTAGE_DIVIDER_RESISTANCE_TO_GND     (10000)                       // in Ohm
#define  AT91SAM7X__ADC_CH5_VOLTAGE_DIVIDER_RESISTANCE_TO_POWER   (10000)
#define  AT91SAM7X__ADC_CH5_VOLTAGE_REFERENCE                     (5000)                        // in [mV]
#define  AT91SAM7X__ADC_CH5_VOLTAGE_THRESHOLD_IN_MILLIVOLT_LOW    (4850)                        // VRef_max = (3% * 5V) + 5 V = 5.15 V; VAdc_nom = 5 V - (5 V * (10k/(10k+10k))) = 2.500 V
#define  AT91SAM7X__ADC_CH5_VOLTAGE_THRESHOLD_IN_MILLIVOLT_HIGH   (5150)                        // VRef_min = - (3% * 5V) + 5 V = 4.85 V
#define  AT91SAM7X__ADC_CHANNEL_GAS_HEAT_SENSE                    (AT91C_ADC_CH3)
#define  AT91SAM7X__ADC_CHANNEL_24000_MILLIVOLT                   (AT91C_ADC_CH4)
#define  AT91SAM7X__ADC_CHANNEL_5000_MILLIVOLT                    (AT91C_ADC_CH5)
#define  AT91SAM7X__ADC_CHANNEL_GAS_SENSING                       (AT91C_ADC_CH6)
#define  AT91SAM7X__ADC_CHANNEL_3300_MILLIVOLT                    (NULL)
#define  AT91SAM7X__ADC_CHANNELS_FOR_MAINBOARD_VOLTAGE_MONITORING (AT91SAM7X__ADC_CHANNEL_24000_MILLIVOLT | AT91SAM7X__ADC_CHANNEL_5000_MILLIVOLT | AT91SAM7X__ADC_CHANNEL_3300_MILLIVOLT)
#define  AT91SAM7X__ADC_CLOCK_IN_HZ                               (5000000)
#define  AT91SAM7X__ADC_STARTUP_TIME_IN_MICRO_SECONDS             (10)
#define  AT91SAM7X__ADC_SAMPLE_AND_HOLD_TIME_IN_NANO_SECONDS      (1200)
#define  AT91SAM7X__ADC_SLEEP_ON_OFF                              (AT91C_ADC_SLEEP_NORMAL_MODE) // power save mode off
#define  AT91SAM7X__ADC_RESOLUTION                                (AT91C_ADC_LOWRES_10_BIT)     // 10 bit resolution
#define  AT91SAM7X__ADC_TRIGGER_ON_OFF                            (AT91C_ADC_TRGEN_DIS)         // ADC triggering disabled
#define  AT91SAM7X__ADC_TRIGGER_SELECTION                         (0)                           // unsed as long as ADC triggering is disabled
#define  AT91SAM7X__ADC_VREF_IN_MILLIVOLT                         (3300)
#define  AT91SAM7X__ADC_MAINBOARD_VOLTAGE_MONITORING_CLOCK_IN_MS  (1000)                        // [ms]

// off-chip peripherals
// FRAM driver defines
#define  FM25CLSS_UNIT                                            (0)
#define  FM25CLSS_CHANNEL                                         (0)
#define  FM25CLSS_SPI_CHANNEL                                     (FM25CLSS_CHANNEL)
#define  FM25CLSS_SCBR                                            (20000000)           // in [Hz]
#define  FM25CLSS_SPI_BAUDRATE                                    (FM25CLSS_SCBR)
#define  FM25CLSS_NUM_MULTI_STORAGES                              (2)

// IO-expander driver defines
#define  MCP2317_HAID                                             (MCP2317_HAID_0)
#define  MCP2317_UNIT                                             (0)
#define  MCP2317_CHANNEL                                          (4)
#define  MCP2317_SPI_CHANNEL                                      (MCP2317_CHANNEL)
#define  MCP2317_SCBR                                             (10000000)           // in [Hz]
#define  MCP2317_SPI_BAUDRATE                                     (MCP2317_SCBR)
#define  MCP2317_IOCON                                            (MCP2317_IOCON_HAEN | MCP2317_IOCON_INTMIRROR)

// IO-expander application defines
#define  AT91SAM7X__MCP2317_SYS0_RDY                              (MCP2317_PA0)
#define  AT91SAM7X__MCP2317_COLUMN_OVEN_START                     (MCP2317_PB3)

#define  AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT                      (MCP2317_PB4)
#define  AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT_POS                  (MCP2317_PB4)
#define  AT91SAM7X__MCP2317_VALVE_GO_TO_HOME                      (MCP2317_PB5)
#define  AT91SAM7X__MCP2317_VALVE_GO_TO_HOME_POS                  (MCP2317_PB5)

//#define  AT91SAM7X__MCP2317_VALVE_GO_TO_HOME                      (MCP2317_PB4)
//#define  AT91SAM7X__MCP2317_VALVE_GO_TO_HOME_POS                  (MCP2317_PB4)
//#define  AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT                      (MCP2317_PB5)
//#define  AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT_POS                  (MCP2317_PB5)

#define  AT91SAM7X__MCP2317_NINJECT                               (MCP2317_PB6)
#define  AT91SAM7X__MCP2317_N_SYS_ERROR                           (MCP2317_PB7)

// external ADC (temperature measurement)
#define  AT91SAM7X__ADC_EXT_AD7788__UNIT0                         (0)
#define  AT91SAM7X__ADC_EXT_AD7788__SPI_CHANNEL                   (2)
#define  AT91SAM7X__ADC_EXT_AD7788__SPI_BAUDRATE                  (5000000)            // in [Hz]
#define  AT91SAM7X__ADC_EXT_AD7788__BYTES_PER_TRANSFER            (2)

// DAC driver defines
#define  DACRRCS085_UNIT                                          (0)
#define  DACRRCS085_CHANNEL                                       (1)                  //SPI Chip select of DAC 
#define  DACRRCS085_SCBR                                          (10000000)           // in [Hz]
#define  DACRRCS085_NUM_CHANNELS                                  (4)
// DAC application defines
#define  AT91SAM7X__DACRRCS085_SPI_CHANNEL                        (DACRRCS085_CHANNEL)
#define  AT91SAM7X__DACRRCS085_SPI_BAUDRATE                       (DACRRCS085_SCBR)
#define  AT91SAM7X__DACRRCS085_NUM_CHANNELS                       (DACRRCS085_NUM_CHANNELS)
#define  AT91SAM7X__DACRRCS085_OUTPUT_MAX_PELTIER                 (750) //(1023)//(/*650 DAC max for 150 W power part*//*DAC 585 max. - 10% for 150W power part*/375  /* 470 max for 110 W power part*//*536*/)
#define  AT91SAM7X__DACRRCS085_OUTPUT_MAX_HEATING_FOIL            (750) //(1023)//(/*650 DAC max for 150 W power part*//*DAC 585 max. - 10% for 150W power part*/375  /* 470 max for 110 W power part*//*536*/)
#define  AT91SAM7X__DACRRCS085_OUTPUT_MAX                         (750)//(/*650 DAC max for 150 W power part*//*DAC 585 max. - 10% for 150W power part*/375  /* 470 max for 110 W power part*//*536*/)
#define  AT91SAM7X__DACRRCS085_OUTPUT_MIN                         (0)
#define  AT91SAM7X__DACRRCS085_OUTPUT_VOLTAGE_MAX                 (3297)               // Vout in [mV] = Vref * (1023/1024)
#define  AT91SAM7X__DACRRCS085_OUTPUT_VOLTAGE_MIN                 (0)
#define  AT91SAM7X__DACRRCS085_VREF                               (3300)               // is equal to 3300 mV

#define  AT91SAM7X__DACRRCS085_CHANNEL_POWER_MOTOR1               (0)
#define  AT91SAM7X__DACRRCS085_CHANNEL_POWER_MOTOR2               (1)
#ifdef AT91SAM7X__SPI0_MASTER_IS_AUTOSAMPLER
 #define  AT91SAM7X__DACRRCS085_CHANNEL_POWER_HEAT_COOL      		(0)
#else
 #define  AT91SAM7X__DACRRCS085_CHANNEL_POWER_HEAT_COOL           (3)
#endif

// step motor driver defines
#define  STPCLKDIR0_FLAGS                                         (/*STPCLKDIR_FLAGS_AUTOPOWEROFF |*/ STPCLKDIR_FLAGS_INVERSE /*STPCLKDIR_FLAGS_NONE*/ )
#define  STPCLKDIR0_CLKSMICRO                                     (16)
#define  STPCLKDIR0_DAC_HOLD                                      (100)             /**<  DAC data input to hold motor driver */
#define  STPCLKDIR0_DAC_RUN                                       (1000) /*850*/            /**<  DAC data input to run motor driver (1.8 A motor and 0.082 Ohm board shunt) */
#define  STPCLKDIR0_MODULO                                        (10667)           /**<  Clocks (usteps) for a full circle. 
                                                                                          10667 = 200 (fullsteps according to data sheet) * 16 (usteps) * 10/3 (gear ratio) */
// step motor application defines
#define  AT91SAM7X__STPCLKDIR0_DAC_MIN                            (10)             /**<  Vout_DAC = Vref_DAC * (STPCLKDIR0_DAC_MIN / 1024) */
#define  AT91SAM7X__STPCLKDIR0_DAC_MAX                            (1023)
#define  AT91SAM7X__STPCLKDIRWAIT_TIMEOUT_MS                      (600)

// gap disk driver defines
#define  ENCGAPDISK0_CONTINUOUS                                   (1)               /**<  calibration with continuous driven motor (1) or left-right (0) for mechanical stop bar */
#define  ENCGAPDISK0_CLKINITHZ                                    (30*16)           /**<  motor speed in usteps/[s] before calibration/movement starts */
#define  ENCGAPDISK0_CLKSINIT                                     (4*16)              /**<  usteps to prepare motor movement before calibration starts */
#define  ENCGAPDISK0_CLKLOWHZ                                     (3840)            /**<  start(end) "frequency" of step motor for acceleration/breaking ramp in usteps/[s]
                                                                                          Caution! ((ENCGAPDISK0_CLKHIGHHZ - ENCGAPDISK0_CLKLOWHZ)/ENCGAPDISK0_CLKSACC) !> ENCGAPDISK0_CLKLOWHZ  */
#define  ENCGAPDISK0_CLKHIGHHZ                                    (20000)           /**<  run "frequency" of step motor in usteps/[s]  */
#define  ENCGAPDISK0_CLKS                                         (10667)           /**<  Clocks (microsteps) for a full circle.
                                                                                          10667 = 200 (fullsteps acc. to data sheet) * 16 (usteps) * 10/3 (gear ratio)
                                                                                          Caution! If changing this value, also change STPCLKDIR0_MODULO to the same value.
                                                                                          If gear ratio or microstepping is changed this value MUST be changed! */
#define  ENCGAPDISK0_CLKSACC                                      (160)             /**<  total usteps for acceleration ramp */
#define  ENCGAPDISK0_CLKSBRK                                      (160)             /**<  total usteps for breaking ramp */
#define  ENCGAPDISK0_TOLERANCESTEPS                               (3*16)            /**<  total deviance in usteps for SAME gaps after 360° rotation of encoder gap disk during calibration
                                                                                          (scan+averaging) */
#define  ENCGAPDISK0_THRESHOLD                                    (1*3)             /**<  minimum distance in microsteps to accept neighboured gaps */
#define  ENCGAPDISK0_GAPDRIFTMAX                                  (3*16)            /**<  total deviance in usteps for SAME gaps after 360° rotation of encoder gap disk during calibration (verify) */
#define  ENCGAPDISK0_TRUE_GAP_NUMBER                              (22)              /**<  true gap number of encoder gap disk  */
#define  ENCGAPDISK0_DEFAULT_EDGE_TYPE                            (ENCGAPDISK_EDGE_RAISE)

#define  ENCGAPDISK0_SCAN_CYCLES                                  (3)               /**<  scan cyles for calibration routine */
#define  ENCGAPDISK0_RETRY_CYCLES                                 (2)               /**<  max number of retry cycles if calibration routine fails */
#define  ENCGAPDISK0_VERIFY_CYCLES                                (3)               /**<  verify cyles for calibration routine */
#define  ENCGAPDISK0_MIN_DIST_REPEAT_CNT                          (1)               /**<  max number of minimum distances repetitions (home edge pattern) */


// override defaults of driver defines dependend on application
#define  MCP2317_STACK_TSKNOTIFYMCP2317                           (1536)
#define  MCP2317_STACK_TSKNOTIFYHELPERMCP2317                     (2048)

#define  MBPB_REG_HOLDING_NREGS                                   (750)                // Caution: Size depends on given modbus addresses, see excel table!

#define  MBPB_REG_INPUT_NREGS                                     (350)                // Caution: Size depends on given modbus addresses, see excel table!

#define  MBPB_REG_COILS_SIZE                                      (8)

#define  MBPB_REG_DISCRETE_SIZE                                   (8)

#define  ENCGAPDISK_EDGEITEM_MAX                                  (18+ 8)

#define  ENCGAPDISK_OFFSETBYDIRECTION                             (0)

#define  STPCLKDIR_MBXOFFSETCNTMSG                                ((ENCGAPDISK_EDGEITEM_MAX)* 4)

#define  STPCLKDIR_OFFSETBYDIRECTION                              (0)

#define  RTT_ACTIVE                                               (1)
#define  RTT_IRQ                                                  (1)

#define  SYSIRQ_RTT_ENABLE  				                           (RTT_IRQ)

#define  SPI_CHANNEL_MAX                                          (8)
#define 	SPI_PDCMAX                                               (64)                 /**<  size in [items] which contains 8-16 data bits depending on each slave's indiv. cfg */
#define 	SPI_BUFFERCNTRX                                          (SPI_PDCMAX* 2)
#define 	SPI_BUFFERCNTTX                                          (SPI_PDCMAX* 1)
#define 	SPI_MBXCNTRX                                             (SPI_PDCMAX* 1)
#define 	SPI_MBXCNTMSG                                            (SPI_PDCMAX* 1)


// defines used primary in application
#define  AT91SAM7X__TICK_0                                        (0)
#define  AT91SAM7X__TICK_1                                        (1)
#define  AT91SAM7X__TICK_2                                        (2)
#define  AT91SAM7X__TICK_3                                        (3)
#define  AT91SAM7X__TICK_4                                        (4)
#define  AT91SAM7X__TICK_5                                        (5)
#define  AT91SAM7X__TICK_6                                        (6)
#define  AT91SAM7X__TICK_7                                        (7)
#define  AT91SAM7X__TICK_8                                        (8)
#define  AT91SAM7X__TICK_9                                        (9)
#define  AT91SAM7X__TICK_10                                       (10)
#define  AT91SAM7X__TICK_20                                       (20)
#define  AT91SAM7X__TICK_30                                       (30)
#define  AT91SAM7X__TICK_40                                       (40)
#define  AT91SAM7X__TICK_50                                       (50)
#define  AT91SAM7X__TICK_60                                       (60)
#define  AT91SAM7X__TICK_70                                       (70)
#define  AT91SAM7X__TICK_80                                       (80)
#define  AT91SAM7X__TICK_90                                       (90)
#define  AT91SAM7X__TICK_100                                      (100)
#define  AT91SAM7X__TICK_200                                      (200)
#define  AT91SAM7X__TICK_300                                      (300)
#define  AT91SAM7X__TICK_400                                      (400)
#define  AT91SAM7X__TICK_500                                      (500)
#define  AT91SAM7X__TICK_600                                      (600)
#define  AT91SAM7X__TICK_700                                      (700)
#define  AT91SAM7X__TICK_800                                      (800)
#define  AT91SAM7X__TICK_900                                      (900)
#define  AT91SAM7X__TICK_1000                                     (1000)
#define  AT91SAM7X__TICK_1500                                     (1500)
#define  AT91SAM7X__TICK_2000                                     (2000)

#define  AT91SAM7X__NUM_REGS_1                                    (1)
#define  AT91SAM7X__NUM_REGS_2                                    (2)
#define  AT91SAM7X__NUM_REGS_3                                    (3)
#define  AT91SAM7X__NUM_REGS_4                                    (4)
#define  AT91SAM7X__NUM_REGS_5                                    (5)
#define  AT91SAM7X__NUM_REGS_6                                    (6)
#define  AT91SAM7X__NUM_REGS_7                                    (7)
#define  AT91SAM7X__NUM_REGS_8                                    (8)
#define  AT91SAM7X__NUM_REGS_9                                    (9)
#define  AT91SAM7X__NUM_REGS_10                                   (10)

#define  AT91SAM7X__NUM_BYTES_1                                   (1)
#define  AT91SAM7X__NUM_BYTES_2                                   (2)
#define  AT91SAM7X__NUM_BYTES_3                                   (3)
#define  AT91SAM7X__NUM_BYTES_4                                   (4)
#define  AT91SAM7X__NUM_BYTES_5                                   (5)
#define  AT91SAM7X__NUM_BYTES_6                                   (6)
#define  AT91SAM7X__NUM_BYTES_7                                   (7)
#define  AT91SAM7X__NUM_BYTES_8                                   (8)
#define  AT91SAM7X__NUM_BYTES_9                                   (9)
#define  AT91SAM7X__NUM_BYTES_10                                  (10)

#define  AT91SAM7X__DUMMY_ARG                                     (0)

#define  AT91SAM7X__VALVE_ACT_POS_INDEX_INJECT                    (0x0001)                   /**< valve actuator position index to release an inject signal on extern control contacts */

#define  AT91SAM7X__REPETITION_TICKS_FOR_TSK_GLOBAL_TIMER                                             (AT91SAM7X__TICK_200)   /**< sample time of ADC temperature control tasks */

//#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   #define  AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT                               (150+1)  /**< leave one degree space for column oven control 
                                                                                                                    which may overshoot for a short time above user level*/
   #define  AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL                      (100+1)
   
   #define  AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT                                (150)
   #define  AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL                       (100)
//#else
   #define  AT91SAM7X__MAX_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER     							(61)     /**< leave one degree space for column oven control 
                                                                                               						     which may overshoot for a short time above user level*/
   #define  AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER                         (60)
//#endif

#define  AT91SAM7X__MIN_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL                         (3)
#define  AT91SAM7X__MIN_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL                          (4)
#define  AT91SAM7X__MIN_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT                                  (29)
#define  AT91SAM7X__MIN_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT                                   (30)
#define  AT91SAM7X__MIN_OWNER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER                           (3)
#define  AT91SAM7X__MIN_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER                            (4)

#define  AT91SAM7X__MAX_HEAT_PROGRAM_SECTIONS                                                         (1)      // for Sykam device options where only one temperature set point is allowed
#define  AT91SAM7X__MAX_HEAT_PROGRAM_ID                                                               (1)      // DO NOT change (one profile, i.e. one hold set point is standard for devices which do not use temperature profiles)
#define  AT91SAM7X__MAX_HEAT_COOL_PROGRAM_SECTIONS                                                    (30)     // amount must be an even number
#define  AT91SAM7X__MAX_HEAT_COOL_PROGRAM_ID                                                          (10)     // amount must be an even number
#define  AT91SAM7X__MAX_CONTROL_PARAM_SECTIONS                                                        (30)     // amount must be an even number

#define  AT91SAM7X__DEFAULT_HEAT_COOL_PROGRAM_SET_POINT_IN_DEGREE_MUL_TEN                             (300)    // default: 30.0 °C * 10 = 300
//#define  COLUMN_OVEN_REGISTERS__DEFAULT_HEAT_COOL_PROGRAM_HOLD_TIME_IN_SECONDS_DIV_SIX  (100)    // default: 10 min = 600 s / 6 = 100 ticks
//#define  COLUMN_OVEN_REGISTERS__DEFAULT_HEAT_COOL_PROGRAM_NUM_ACTIVE_SECTIONS           (1)
#define  AT91SAM7X__DEFAULT_TEMPERATURE_OFFSET_MUL_TEN                                                (0)
#define  AT91SAM7X__DEFAULT_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN                                  (1)
#define  AT91SAM7X__MIN_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN                                      (1)
#define  AT91SAM7X__MAX_SET_POINT_REACHED_BAND_IN_DEGREE_MUL_TEN                                      (100)
#define  AT91SAM7X__MAX_TEMPERATURE_OFFSET_MUL_TEN                                                    (20)

#define  AT91SAM7X__MAINBOARD_VOLTAGE_REFERENCE_24000_MILLIVOLT                                       (24000)
#define  AT91SAM7X__MAINBOARD_VOLTAGE_REFERENCE_5000_MILLIVOLT                                        (5000)
#define  AT91SAM7X__MAINBOARD_VOLTAGE_REFERENCE_3300_MILLIVOLT                                        (3300)

//#define  AT91SAM7X__CONVERT_ADC_VALUE_INTO_VOLTAGE( AdcValue, AdcResolutionSteps, VRefInMilliVolt)    (AdcValue > AdcResolutionSteps) ?                  \
//                                                                                                       (AdcResolutionSteps) : ((AdcValue * VRefInMilliVolt) / (AdcResolutionSteps + 1) )


//---------------------------------------------- typedefs --------------------------------------------------------------------------------------

typedef struct _S_AT91SAM7X_MainboardVoltageMonitoring
{
   ulong    u32_VoltageDividerResistanceNear;
   ulong    u32_VoltageDividerResistanceFar;
   ushort   u16_VoltageThresholdInMilliVoltLow;
   ushort   u16_VoltageThresholdInMilliVoltHigh;
   ushort   u16_VoltageReferenceInMilliVolt;
   
}  tS_AT91SAM7X_MainboardVoltageMonitoring;

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   tS_AT91SAM7X_MainboardVoltageMonitoring   SA_AT91SAM7X_MainboardVoltageMonitoring[];

extern   const    float    cf_AT91SAM7X_TemperatureSampleTimeInSysTicks;

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

bool AT91SAM7X_FilterForModbusRequests  (uchar u8_ModbusDataType, U32 u32_ModbusAddress/*ushort u16_ModbusAddress*/, ushort *pu16_ModbusData);


#endif


// docu of functions 
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         bool AT91SAM7X_FilterForModbusRequests (uchar u8_ModbusDataType, ushort u16_ModbusAddress, ushort *pu16_ModbusData)
*
*  \brief      A special prefilter callback function which can be called inside modbus driver module. It is possible to 
*              change the data value of an incoming <b>write</b> request, to decline (return value \em FALSE) or to 
*              accept it (return value \em TRUE).
*
*
*  \param      u8_ModbusDataType    This is the modbus data type, i. e. a holding register or a coil can be the type here. The 
*                                   value is one out of \ref EMBPBRegType.
*                                   
*  \param      u16_ModbusAddress    This is the requested modbus address to write a value to.
*
*  \param      *pu16_ModbusData     This is a pointer to the content of u16_ModbusAddress. A modifcation occurs if this value is 
*                                   set to another value.
*
*
*  \return     An exception inside application software design: This function returns an boolean value. If modbus value shall be
*              rejected by driver module return value must be \em FALSE otherwise \em TRUE.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
