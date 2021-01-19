/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                lib_exceptions_drv.h
*
*  \brief               This header file contains exception codes for all driver modules.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-07-13     Initial version.
*  \date                
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                
*
*  \todo                
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef LIB_EXCEPTIONS_DRV__ERROR__H
#define LIB_EXCEPTIONS_DRV__ERROR__H


//---------------------------------------------- enums -------------------------------------------------------------------------------

// sub exception codes for drivers
enum _E_LibExceptionsDrv__Modbus
{
   LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_DESCRIPTION_SET         =  1,
   LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_STATE_SET               =  2,
   LIB_EXCEPTIONS_DRV__ERROR__MODBUS__PRODUCT_ADDRESS_SET             =  3,
   LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_WAIT_DECODE             =  4,
   LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_REBUILD                 =  5,
   LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_FILTER_SET              =  6,
   LIB_EXCEPTIONS_DRV__ERROR__MODBUS__TRIGGER_FILTER_STATE_SET        =  7,
   LIB_EXCEPTIONS_DRV__ERROR__MODBUS__REGACCESS                       =  8,
   LIB_EXCEPTIONS_DRV__ERROR__MODBUS__INIT                            =  9
   //
};

enum _E_LibExceptionsDrv__Usart
{
   LIB_EXCEPTIONS_DRV__ERROR__USART__INIT_UNIT                        =  51,
   LIB_EXCEPTIONS_DRV__ERROR__USART__RX                               =  52,
   LIB_EXCEPTIONS_DRV__ERROR__USART__TX                               =  53
};

enum _E_LibExceptionsDrv__SPI
{
   LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_UNIT                           =  101,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_UNIT_CHECK                     =  102,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL                        =  103,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL_CHECK                  =  104,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__MSG_CHANNEL_NEXT                   =  105,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__RX                                 =  106,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__RX_TAG                             =  107,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__TX                                 =  108,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__TX_TAG                             =  109,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__TX_LOCK                            =  110,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__TX_UNLOCK                          =  111,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__TX_TAG_UNLOCK                      =  112,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__TX_QUEUE                           =  113,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__TX_TAG_QUEUE                       =  114,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_UNIT                          =  115,
   LIB_EXCEPTIONS_DRV__ERROR__SPI__INIT_CHANNEL                       =  116   
   //  
};

enum _E_LibExceptionsDrv__Stepmotor
{
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__START                        =  151,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__POWER                        =  152,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__PROFILE_ADJUST               =  153,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__MOVE                         =  154,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__MOVE_MAX                     =  155,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__SPEED_CHANGE                 =  156,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__SPEED_CHANGE_WAIT            =  157,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__BREAK                        =  158,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__QUIT                         =  159,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__WAIT                         =  160,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__POSITION_OFFSET_SET          =  161,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__POSITION_OFFSET_GET          =  162,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__POSITION_OFFSET_VALUE        =  163,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__POSITION_OFFSET_MAKE_PLUS    =  164,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__ABSOLUTE_RAW_SET             =  165,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__ABSOLUTE_RAW_GET             =  166,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__ABSOLUTE_SET                 =  167,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__DAC_SET                      =  168,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__DAC_GET                      =  169,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__SNAP_SHOT_SIZE_GET           =  170,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__SNAP_SHOT_ADDR_GET           =  171,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__SNAP_SHOT                    =  172,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__HZ_GET                       =  173,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__CLOCKS_GET                   =  174,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__CLOCKS_ABS_GET               =  175,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__JOB_CLOCKS_GET               =  176,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__MODULO_CLOCKS_GET            =  177,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__MODULO_MAX_CLOCKS_GET        =  178,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__OFFSET_PUSH                  =  179,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__INIT_UNIT                    =  180,
   LIB_EXCEPTIONS_DRV__ERROR__STEPMOTOR__INIT_UNIT_EX                 =  181
   //  
};

enum _E_LibExceptionsDrv__PWM
{
   LIB_EXCEPTIONS_DRV__ERROR__PWM__CHANNEL_ID_GET                     =  201,
   LIB_EXCEPTIONS_DRV__ERROR__PWM__START_CHANNEL                      =  202,
   LIB_EXCEPTIONS_DRV__ERROR__PWM__STOP_CHANNEL                       =  203,
   LIB_EXCEPTIONS_DRV__ERROR__PWM__CHANNEL_TX_VALUE                   =  204,
   LIB_EXCEPTIONS_DRV__ERROR__PWM__CHANNEL_TX                         =  205,
   LIB_EXCEPTIONS_DRV__ERROR__PWM__INACTIVE_GET                       =  206,
   LIB_EXCEPTIONS_DRV__ERROR__PWM__WAIT                               =  207,
   LIB_EXCEPTIONS_DRV__ERROR__PWM__CALC_PERIOD_AND_DUTY               =  208,
   LIB_EXCEPTIONS_DRV__ERROR__PWM__CALC_HZ                            =  209,
   LIB_EXCEPTIONS_DRV__ERROR__PWM__INIT_UNIT                          =  210
   //
};

enum _E_LibExceptionsDrv__Mcp2317
{
   LIB_EXCEPTIONS_DRV__ERROR__MCP2317__INIT_UNIT                      =  251,
   LIB_EXCEPTIONS_DRV__ERROR__MCP2317__SET_OUTPUT                     =  252
};

enum _E_LibExceptionsDrv__Dacrrcs085
{
   LIB_EXCEPTIONS_DRV__ERROR__DACRRCS085__INIT_UNIT                   =  301,
   LIB_EXCEPTIONS_DRV__ERROR__DACRRCS085__SET_OUTPUT                  =  302,
   LIB_EXCEPTIONS_DRV__ERROR__DACRRCS085__GET_OUTPUT                  =  303
};

enum _E_LibExceptionsDrv__Engapdisk
{
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__CLEAR                        =  351,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__TEACH_IN                     =  352,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_ERROR_GET               =  353,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__STATE_GET                    =  354,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_COUNT                   =  355,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_GET                     =  356,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_DATA_GET                =  357,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_GOTO                    =  358,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__EDGE_ZERO_SET                =  359,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__HOME_EDGE_GET                =  360,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__CALIBRATION_DATA_RW          =  361,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__CALIBRATION_DATA_SIZE_GET    =  362,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__CALIBRATION_DATA_ADDR_GET    =  363,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__RUN_PARAMS_RW                =  364,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__SNAPSHOT_SIZE_GET            =  365,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__SNAPSHOT_ADDR_GET            =  366,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__SNAPSHOT                     =  367,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__STEPMOTOR_UNIT_GET           =  368,
   LIB_EXCEPTIONS_DRV__ERROR__ENGAPDISK__INIT_UNIT                    =  369
   //
};

enum _E_LibExceptionsDrv__Fm25clss
{
   LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__CONTROL                       =  401,
   LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__READ_STATUS_REGISTER          =  402,
   LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE_STATUS_REGISTER         =  403,
   LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__READ_SAFE                     =  404,
   LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__WRITE                         =  405,
   LIB_EXCEPTIONS_DRV__ERROR__FMCLSS25__INIT                          =  406,
   //
};

enum _E_LibExceptionsDrv__ADC
{
   LIB_EXCEPTIONS_DRV__ERROR__ADC__INIT_UNIT                          =  451,
   LIB_EXCEPTIONS_DRV__ERROR__ADC__INIT_CHANNEL                       =  452,
   LIB_EXCEPTIONS_DRV__ERROR__ADC__MESSAGE_UNIT                       =  453,
   LIB_EXCEPTIONS_DRV__ERROR__ADC__MESSAGE_CHANNEL                    =  454,
   LIB_EXCEPTIONS_DRV__ERROR__ADC__RX                                 =  455,
   LIB_EXCEPTIONS_DRV__ERROR__ADC__WAIT                               =  456,
   LIB_EXCEPTIONS_DRV__ERROR__ADC__INIT                               =  457,
   LIB_EXCEPTIONS_DRV__ERROR__ADC__GET_CONVERTED_DATA                 =  458,
   //
};

enum _E_LibExceptionsDrv__RTT
{
   LIB_EXCEPTIONS_DRV__ERROR__RTT__INIT_UNIT                          =  501,
   LIB_EXCEPTIONS_DRV__ERROR__RTT__RESTART                            =  502,
   LIB_EXCEPTIONS_DRV__ERROR__RTT__SLOW_CLOCK_GET                     =  503,
   LIB_EXCEPTIONS_DRV__ERROR__RTT__ALARM_SET                          =  504,
   LIB_EXCEPTIONS_DRV__ERROR__RTT__MESSAGE                            =  505,
   LIB_EXCEPTIONS_DRV__ERROR__RTT__READ                               =  506
   //
};

enum _E_LibExceptionsDrv__PIO
{
   LIB_EXCEPTIONS_DRV__ERROR__PIO__INIT                               =  551
   //
};

// add here new driver exception elements (enums)

//---------------------------------------------- structures ----------------------------------------------------------------------------

typedef union
{
   enum  _E_LibExceptionsDrv__Modbus         E_LibExceptionsDrv__Modbus;
   enum  _E_LibExceptionsDrv__Usart          E_LibExceptionsDrv__Usart;
   enum  _E_LibExceptionsDrv__SPI            E_LibExceptionsDrv__SPI;
   enum  _E_LibExceptionsDrv__Stepmotor      E_LibExceptionsDrv__Stepmotor;
   enum  _E_LibExceptionsDrv__PWM            E_LibExceptionsDrv__PWM;
   enum  _E_LibExceptionsDrv__Mcp2317        E_LibExceptionsDrv__Mcp2317;
   enum  _E_LibExceptionsDrv__Dacrrcs085     E_LibExceptionsDrv__Dacrrcs085;
   enum  _E_LibExceptionsDrv__Engapdisk      E_LibExceptionsDrv__Engapdisk;
   enum  _E_LibExceptionsDrv__Fm25clss       E_LibExceptionsDrv__Fmclss25;
   enum  _E_LibExceptionsDrv__ADC            E_LibExceptionsDrv__ADC;
   enum  _E_LibExceptionsDrv__RTT            E_LibExceptionsDrv__RTT;
   enum  _E_LibExceptionsDrv__PIO            E_LibExceptionsDrv__PIO;
   // add here new structure elements
} tU_LibExceptionsDrv_ErrorCodeSW;


#endif
