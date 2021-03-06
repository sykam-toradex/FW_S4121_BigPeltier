/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_control_params.h
*
*  \brief               Header file for \ref column_oven_control_params.c
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2012-12-20     Initial version.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test
*
*  \todo
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef COLUMN_OVEN_CONTROL_PARAMS__H
#define COLUMN_OVEN_CONTROL_PARAMS__H
//---------------------------------------------- defines ---------------------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//If the system must remain online, one tuning method is to first set K_i and K_d values to zero. 											   //
//Increase the K_p until the output of the loop oscillates, then the K_p should be set to approximately half of that value				//
//for a "quarter amplitude decay" type response. Then increase K_i until any offset is corrected in sufficient time for the process.  //
//However, too much K_i will cause instability. Finally, increase K_d, if required, until the loop is acceptably quick to reach its 	//
//reference after a load disturbance. However, too much K_d will cause excessive response and overshoot. A fast PID loop tuning 		//
//usually overshoots slightly to reach the setpoint more quickly; however, some systems cannot accept overshoot, 							//
//in which case an over-damped closed-loop system is required, which will require a K_p setting significantly less than half 			//
//that of the K_p setting that was causing oscillation*/																										//													//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// control params for Oven with Heater elements
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION0_PARAM_KI               (7200)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION0_PARAM_TKI              (1)      // margin temperature in [�C]*10 for the given Ki (between sections linear interpolation is done)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION0_PARAM_KP               (2000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION0_PARAM_TKP              (1)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION1_PARAM_KI               (100)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION1_PARAM_TKI              (41)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION1_PARAM_KP               (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION1_PARAM_TKP              (41)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION2_PARAM_KI               (100)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION2_PARAM_TKI              (100)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION2_PARAM_KP               (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION2_PARAM_TKP              (100)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION3_PARAM_KI               (60)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION3_PARAM_TKI              (200)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION3_PARAM_KP               (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION3_PARAM_TKP              (200)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION4_PARAM_KI               (40)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION4_PARAM_TKI              (300)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION4_PARAM_KP               (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION4_PARAM_TKP              (300)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION5_PARAM_KI               (30)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION5_PARAM_TKI              (400)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION5_PARAM_KP               (1800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION5_PARAM_TKP              (400)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION6_PARAM_KI               (30)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION6_PARAM_TKI              (500)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION6_PARAM_KP               (1800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION6_PARAM_TKP              (500)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION7_PARAM_KI               (23)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION7_PARAM_TKI              (600)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION7_PARAM_KP               (1800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION7_PARAM_TKP              (600)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION8_PARAM_KI               (15)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION8_PARAM_TKI              (800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION8_PARAM_KP               (1800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION8_PARAM_TKP              (800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION9_PARAM_KI               (15)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION9_PARAM_TKI              (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION9_PARAM_KP               (1800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION9_PARAM_TKP              (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION10_PARAM_KI              (14)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION10_PARAM_TKI             (1200)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION10_PARAM_KP              (1700)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION10_PARAM_TKP             (1200)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION11_PARAM_KI              (11)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION11_PARAM_TKI             (1500)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION11_PARAM_KP              (1550)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT__CONTROL_SECTION11_PARAM_TKP             (1500)
// control params for column oven with peltier elements
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_KI      (7200) //      (7200)     
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_TKI     (1)    //      (1)                 // margin temperature in [�C]*10 for the given Ki (between sections linear interpolation is done)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_KP      (2000) //   (2000)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_TKP     (1*2)    //(1)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_KI      (1000*2)  //  (100)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_TKI     (41*2)   // (41)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_KP      (2000*2) //   (1000)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_TKP     (41*2)   // (41)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_KI      (1000*2)  //  (100)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_TKI     (100*2)   // (100)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_KP      (2000*2)   // (1000)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_TKP     (100*2)  ///  (100)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION3_PARAM_KI      (60*2)   // (60)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION3_PARAM_TKI     (200*2)  //  (200)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION3_PARAM_KP      (1000*2) //   (1000)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION3_PARAM_TKP     (200*2)  //  (200)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION4_PARAM_KI      (40*2)   // (40)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION4_PARAM_TKI     (300*2)  //  (300)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION4_PARAM_KP      (1000*2) //   (1000)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION4_PARAM_TKP     (300*2)  //  (300)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION5_PARAM_KI      (30*2)   // (30)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION5_PARAM_TKI     (400*2)  //  (400)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION5_PARAM_KP      (1800*2) //   (1800)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION5_PARAM_TKP     (400*2)  //  (400)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION6_PARAM_KI      (30*2)   // (30)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION6_PARAM_TKI     (500*2)  //  (500)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION6_PARAM_KP      (1800*2) //  (1800)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION6_PARAM_TKP     (500*2)  //  (500)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION7_PARAM_KI      (23*2)   // (23)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION7_PARAM_TKI     (600*2)  //  (600)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION7_PARAM_KP      (1800*2) //   (1800)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION7_PARAM_TKP     (600*2)  //  (600)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION8_PARAM_KI      (15*2)   // (15)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION8_PARAM_TKI     (800*2)  //  (800)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION8_PARAM_KP      (1800*2) //   (1800)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION8_PARAM_TKP     (800*2)  //  (800)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION9_PARAM_KI      (15*2)   // (15)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION9_PARAM_TKI     (1000*2) //   (1000)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION9_PARAM_KP      (1800*2) //   (1800)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION9_PARAM_TKP     (1000*2) //   (1000)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION10_PARAM_KI      (15*2)  //   (15)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION10_PARAM_TKI     (1000*2)//     (1000)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION10_PARAM_KP      (1800*2)//     (1800)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION10_PARAM_TKP     (1000*2)//     (1000)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION11_PARAM_KI      (15*2)  //   (15)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION11_PARAM_TKI     (1000*2)//     (1000)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION11_PARAM_KP      (1800*2)//     (1800)
//#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION11_PARAM_TKP     (1000*2)//     (1000)
//

#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_KI      (7200)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_TKI     (1)      // margin temperature in [�C]*10 for the given Ki (between sections linear interpolation is done)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_KP      (2000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_TKP     (1)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_KI      (100)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_TKI     (41)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_KP      (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_TKP     (41)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_KI      (100)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_TKI     (100)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_KP      (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_TKP     (100)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION3_PARAM_KI      (60)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION3_PARAM_TKI     (200)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION3_PARAM_KP      (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION3_PARAM_TKP     (200)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION4_PARAM_KI      (40)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION4_PARAM_TKI     (300)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION4_PARAM_KP      (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION4_PARAM_TKP     (300)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION5_PARAM_KI      (30)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION5_PARAM_TKI     (400)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION5_PARAM_KP      (1800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION5_PARAM_TKP     (400)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION6_PARAM_KI      (30)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION6_PARAM_TKI     (500)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION6_PARAM_KP      (1800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION6_PARAM_TKP     (500)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION7_PARAM_KI      (23)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION7_PARAM_TKI     (600)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION7_PARAM_KP      (1800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION7_PARAM_TKP     (600)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION8_PARAM_KI      (15)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION8_PARAM_TKI     (800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION8_PARAM_KP      (1800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION8_PARAM_TKP     (800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION9_PARAM_KI      (15)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION9_PARAM_TKI     (1000)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION9_PARAM_KP      (1800)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION9_PARAM_TKP     (1000)



// control params for autosampler with peltier elements
// Difference between Current and Set Temperatureerature is <= 1�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION0_PARAM_KI        (5000)//7200  //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION0_PARAM_TKI       (1)//1      // margin Temperatureerature in [�C]*10 for the given Ki (between sections linear interpolation is done)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION0_PARAM_KP        (1000)//2000 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION0_PARAM_TKP       (1)//1
// Difference between Current and Set Temperatureerature is 4�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION1_PARAM_KI        (100)//100   //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION1_PARAM_TKI       (41)//41
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION1_PARAM_KP        (2000)//1000 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION1_PARAM_TKP       (41)//41
// Difference between Current and Set Temperatureerature is 10�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION2_PARAM_KI        (100)//100   //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION2_PARAM_TKI       (100)//100
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION2_PARAM_KP        (5000)//1000 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION2_PARAM_TKP       (100)//100
// Difference between Current and Set Temperatureerature is 20�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION3_PARAM_KI        (100)//60     //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION3_PARAM_TKI       (200)//200
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION3_PARAM_KP        (5000)//1000  //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION3_PARAM_TKP       (200)//200
// Difference between Current and Set Temperatureerature is 30�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION4_PARAM_KI        (100)//30     //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION4_PARAM_TKI       (300)//300
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION4_PARAM_KP        (5000)//500   //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION4_PARAM_TKP       (300)//300
// Difference between Current and Set Temperatureerature is 40 �C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION5_PARAM_KI        (100)//30     //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION5_PARAM_TKI       (400)//400
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION5_PARAM_KP        (5000)//1800
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION5_PARAM_TKP       (400)//400
// Difference between Current and Set Temperatureerature is 50�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION6_PARAM_KI        (100)//30     //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION6_PARAM_TKI       (500)//500
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION6_PARAM_KP        (5000)//1800 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION6_PARAM_TKP       (500)//500
// Difference between Current and Set Temperatureerature 60�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION7_PARAM_KI        (50) //23    //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION7_PARAM_TKI       (600)//600
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION7_PARAM_KP        (5000) //1800 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION7_PARAM_TKP       (600)//600
// Difference between Current and Set Temperatureerature 70�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION8_PARAM_KI        (50) //23    //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION8_PARAM_TKI       (600)//600
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION8_PARAM_KP        (5000) //1800 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION8_PARAM_TKP       (600)//600
// Difference between Current and Set Temperatureerature 80�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION9_PARAM_KI        (50) //23    //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION9_PARAM_TKI       (600)//600
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION9_PARAM_KP        (5000) //1800 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION9_PARAM_TKP       (600)//600
// Difference between Current and Set Temperatureerature 90�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION10_PARAM_KI        (50) //23    //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION10_PARAM_TKI       (600)//600
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION10_PARAM_KP        (5000) //1800 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION10_PARAM_TKP       (600)//600
// Difference between Current and Set Temperatureerature 100�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION11_PARAM_KI        (50) //23    //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION11_PARAM_TKI       (600)//600
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION11_PARAM_KP        (5000) //1800 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION11_PARAM_TKP       (600)//600
// Difference between Current and Set Temperatureerature 110�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION12_PARAM_KI        (50) //23    //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION12_PARAM_TKI       (600)//600
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION12_PARAM_KP        (5000) //1800 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION12_PARAM_TKP       (600)//600
// Difference between Current and Set Temperatureerature 120�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION13_PARAM_KI        (50) //23    //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION13_PARAM_TKI       (600)//600
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION13_PARAM_KP        (5000) //1800 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION13_PARAM_TKP       (600)//600
// Difference between Current and Set Temperatureerature 130�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION14_PARAM_KI        (50) //23    //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION14_PARAM_TKI       (600)//600
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION14_PARAM_KP        (5000) //1800 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION14_PARAM_TKP       (600)//600
// Difference between Current and Set Temperatureerature 140�C
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION15_PARAM_KI        (50) //23    //Integrator Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION15_PARAM_TKI       (600)//600
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION15_PARAM_KP        (5000) //1800 //Proportional Gain
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION15_PARAM_TKP       (600)//600
#define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_TYPE_ADAPTIVE_PID_CONTROL                    (1)
//#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
//   #define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM                (1499)
//#else
//   #define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM                (599)
//#endif
#define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM_FOR_HEAT          ((AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT * 10) - 1)
#define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM_FOR_HEAT_AND_COOL ((AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL * 10) - 1)
#define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_SUM_FOR_AUTOSAMPLER   ((AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER * 10) - 1)
//#define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MIN                   (1)
//#define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MAX                   (AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS * 10)
#define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MIN                   (1)
#define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MAX_FOR_HEAT          (AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT * 10)
#define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MAX_FOR_HEAT_AND_COOL (AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_HEAT_AND_COOL * 10)
#define  COLUMN_OVEN_CONTROL_PARAMS__CONTROL_SECTION_PARAMS_DELTA_T_MAX_FOR_AUTOSAMPLER   (AT91SAM7X__MAX_USER_TEMPERATURE_IN_DEGREE_CELSIUS_FOR_AUTOSAMPLER * 10)
//---------------------------------------------- structures and enums ---------------------------------------------------------------------------
enum  _E_ColumnOvenControlParams_DeviceOption
{
   COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT,
   COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL,
   COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER
};
//---------------------------------------------- externals defined in this module --------------------------------------------------------------
extern   const    struct   _S_ColumnOvenRegisters_ControlSectionParams cSSA_ColumnOvenControlParams[][AT91SAM7X__MAX_CONTROL_PARAM_SECTIONS];
#endif
// control params for column oven with peltier elements
/*
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_KI      (50)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_TKI     (1)      // margin temperature in [�C]*10 for the given Ki (between sections linear interpolation is done)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_KP      (500)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION0_PARAM_TKP     (1)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_KI      (150)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_TKI     (41)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_KP      (200)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION1_PARAM_TKP     (41)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_KI      (150)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_TKI     (100)
#define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_HEAT_AND_COOL__CONTROL_SECTION2_PARAM_KP      (200)
*/
// control params for autosampler with peltier elements
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION0_PARAM_KI        (7200)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION0_PARAM_TKI       (1)      // margin temperature in [�C]*10 for the given Ki (between sections linear interpolation is done)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION0_PARAM_KP        (2000)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION0_PARAM_TKP       (1)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION1_PARAM_KI        (100)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION1_PARAM_TKI       (41)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION1_PARAM_KP        (1000)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION1_PARAM_TKP       (41)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION2_PARAM_KI        (100)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION2_PARAM_TKI       (100)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION2_PARAM_KP        (1000)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION2_PARAM_TKP       (100)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION3_PARAM_KI        (60)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION3_PARAM_TKI       (200)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION3_PARAM_KP        (1000)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION3_PARAM_TKP       (200)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION4_PARAM_KI        (40)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION4_PARAM_TKI       (300)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION4_PARAM_KP        (1000)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION4_PARAM_TKP       (300)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION5_PARAM_KI        (30)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION5_PARAM_TKI       (400)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION5_PARAM_KP        (1800)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION5_PARAM_TKP       (400)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION6_PARAM_KI        (30)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION6_PARAM_TKI       (500)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION6_PARAM_KP        (1800)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION6_PARAM_TKP       (500)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION7_PARAM_KI        (23)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION7_PARAM_TKI       (600)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION7_PARAM_KP        (1800)
// #define  COLUMN_OVEN_CONTROL_PARAMS__OPTION_AUTOSAMPLER__CONTROL_SECTION7_PARAM_TKP       (600)
