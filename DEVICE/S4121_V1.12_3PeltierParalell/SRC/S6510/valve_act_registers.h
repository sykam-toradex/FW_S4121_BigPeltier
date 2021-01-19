/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_registers.h
*
*  \brief               This header file contains all valve actuator register bank addresses and FRAM addresses.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2010-06-17     Initial version.
*  \date                2011-02-03     Imported S6510_registers.h.
*  \date                2011-08-09     Renamed several adresses and variables according to new name pattern (module name -> 
*                                      group name -> field name). Moved active interface and error codes to 
*                                      \ref common_registers.h. Reviewed documentation.
*  \date                2012-11-06     Removed addresses for deleted commands "ValveActCmdsOwner_CalDataLoad" and 
*                                      "ValveActCmdsOwner_CalDataLoad". Removed addresses and entries in struct 
*                                      \ref _S_ValveActRegisters_StatusDataIntern for deleted flags "u16_Cal_DataSaved" and 
*                                      "u16_Cal_DataLoaded".
*  \date                2012-11-07     Separated device parameters and status data in intern and extern, corresponding to  
*                                      other devices. Shifted addresses.
*
*
**//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef VALVE_ACT_REGISTERS__H
#define VALVE_ACT_REGISTERS__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

// Internal modbus addresses start from one ("0x0001"). Fifty entries for every category (device/run params...) are allowed. 
// If expansion is desired have a look for example at MODBUS_REG_HOLDING_NREGS in AT91SAM7X.h.
// Adapt start an end addresses if register bank is extended!


#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START         (0x00C9)
#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__END           (0x00D7)

#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START         (0x00FB)
#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__END           (0x00FD)

#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START                   (0x012D)
#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__END                     (0x0133)
#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START                    (0x015F)
#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__END                      (0x0162)

#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START           (0x0065)
#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__END             (0x0069)
#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START           (0x0097)
#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__END             (0x009D)

// addresses on permanent memory -  start/end addresses must be changed if register bank addresses were changed
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START                (0x0259)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__END                  (0x0276)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__START                (0x02BD)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__END                  (0x02BF)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_OWNER__START                          (0x0321)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_OWNER__END                            (0x032E)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_USER__START                           (0x0385)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_USER__END                             (0x038C)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START                  (0x03E9)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__END                    (0x03F2)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START                  (0x044D)
#define  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__END                    (0x045A)

//---------------------------------------------- enums -----------------------------------------------------------------------------------------

// Abbreviations:
//                HREG  -  holding register
//                IREG  -  input register
//                DISC  -  discrete input
//                COIL  -  coils

// device params (to every single address two bytes are assigned internally)
// -- order must be the same as in the corresponding struct

//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START         (0x00C9)
//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__END           (0x00D7)

enum _E_ValveActRegisters_ModbusAddresses_DeviceParamsIntern
{
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_USTEPS_INIT                  =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START,		 //ext. C8
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_USPS_INIT                    =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 1,	 //ext. C9
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_USTEPS_ACC                   =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 2,	 //ext. CA
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_USTEPS_BREAK                 =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 3,	 //ext. CB
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_USPS_START                   =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 4,	 //ext. CC
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_USPS_RUN                     =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 5,	 //ext. CD
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_MODE                         =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 6,	 //ext. CE
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_USTEPS_FULL_CIRCLE           =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 7,	 //ext. CF
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_TRUE_GAP_NUMBER              =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 8,	 //ext. D0
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_MIN_USTEPS_THRESHOLD         =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 9,	 //ext. D1
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_EDGE_TYPE_TRIGGERED_ON       =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 10, //ext. D2
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_TOL_USTEPS_SAME_GAPS_CAL     =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 11, //ext. D3
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_TOL_USTEPS_SAME_GAPS_RUN     =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 12, //ext. D4
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DAC_HOLD_MOTOR_DRIVER            =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 13, //ext. D5
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DAC_RUN_MOTOR_DRIVER             =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 14, //ext. D6
   																																																	
   //																																																	
};
// every address corresponds to one byte - to save reg data we need 16 bits = 2 bytes for every entry
// -- order must be the same as in the corresponding struct
//#define  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START                (0x0259)
//#define  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__END                  (0x0276)
enum _E_ValveActRegisters_FRAM_Addresses_DeviceParamsIntern
{
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USTEPS_INIT                       =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START,		//ext. 0x0259
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USPS_INIT                         =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 2,	//ext. 0x025B
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USTEPS_ACC                        =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 4,	//ext. 0x025D
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USTEPS_BREAK                      =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 6,	//ext. 0x025F
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USPS_START                        =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 8,	//ext. 0x0261
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USPS_RUN                          =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 10,//ext. 0x0263
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_MODE                              =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 12,//ext. 0x0265
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_USTEPS_FULL_CIRCLE                =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 14,//ext. 0x0267
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_TRUE_GAP_NUMBER                   =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 16,//ext. 0x0269
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_MIN_USTEPS_THRESHOLD              =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 18,//ext. 0x026B
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_EDGE_TYPE_TRIGGERED_ON            =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 20,//ext. 0x026D
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_TOL_USTEPS_SAME_GAPS_CAL          =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 22,//ext. 0x026F
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_TOL_USTEPS_SAME_GAPS_RUN          =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 24,//ext. 0x0270
   VALVE_ACT_REGISTERS__FRAM_ADDR__DAC_HOLD_MOTOR_DRIVER                 =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 26,//ext. 0x0271
   VALVE_ACT_REGISTERS__FRAM_ADDR__DAC_RUN_MOTOR_DRIVER                  =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 28,//ext. 0x0272
   //
};

//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START         (0x00FB)
//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__END           (0x00FB)

enum _E_ValveActRegisters_ModbusAddresses_DeviceParamsExtern
{
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CURRENT_VALVE_TYPE               =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START,//ext. 0x00FA
   //
};
enum _E_ValveActRegisters_FRAM_Addresses_DeviceParamsExtern
{
   VALVE_ACT_REGISTERS__FRAM_ADDR__CURRENT_VALVE_TYPE                    =  VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__START,
   //
};

//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START                   (0x012D)
//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__END                     (0x0133)
enum _E_ValveActRegisters_ModbusAddresses_CmdsOwner
{
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_LOAD               =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START,		//ext. 0x012C
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_LOAD_DEFAULT       =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  1,//ext. 0x012D
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_SAVE               =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  2,//ext. 0x012E
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__STATUS_DATA_LOAD                 =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  3,//ext. 0x012F
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_WITH_DEVICE_PARAMS           =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  4,//ext. 0x0130
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CAL_WITH_DEFAULT_PARAMS          =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  5,//ext. 0x0131
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__RESET_TOTAL_SWITCH_COUNTER       =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  6,//ext. 0x0132
   //
};
enum _E_ValveActRegisters_FRAM_Addresses_CmdsOwner
{
   VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOAD                      =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_OWNER__START,
   VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOAD_DEFAULT              =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  2,
   VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_SAVE                      =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  4,
   VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_LOAD                        =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  6,
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_WITH_DEVICE_PARAMS                  =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  8,
   VALVE_ACT_REGISTERS__FRAM_ADDR__CAL_WITH_DEFAULT_PARAMS                 =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  10,
   VALVE_ACT_REGISTERS__FRAM_ADDR__RESET_TOTAL_SWITCH_COUNTER              =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  12,
   //
};

//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START                    (0x015F)
//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__END                      (0x0162)
enum _E_ValveActRegisters_ModbusAddresses_CmdsUser
{
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__GO_TO_HOME_POS                   =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START,			//ext. 0x015E
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__GO_TO_NEXT_POS                   =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START  +  1,	//ext. 0x015F
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__GO_TO_PREVIOUS_POS               =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START  +  2,	//ext. 0x0160
   VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__GO_TO_INQUIRED_POS               =  VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START  +  3,	//ext. 0x0161
   //																																														
};																																															
enum _E_ValveActRegisters_FRAM_Addresses_CmdsUser																															
{
   VALVE_ACT_REGISTERS__FRAM_ADDR__GO_TO_HOME_POS                          =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_USER__START,
   VALVE_ACT_REGISTERS__FRAM_ADDR__GO_TO_NEXT_POS                          =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_USER__START  +  2,
   VALVE_ACT_REGISTERS__FRAM_ADDR__GO_TO_PREVIOUS_POS                      =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_USER__START  +  4,
   VALVE_ACT_REGISTERS__FRAM_ADDR__GO_TO_INQUIRED_POS                      =  VALVE_ACT_REGISTERS__FRAM_ADDR__CMDS_USER__START  +  6,
   //
};

//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START           (0x0065)
//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__END             (0x0069)
// status data -- order must be the same as in the corresponding struct
enum _E_ValveActRegisters_ModbusAddresses_StatusDataExtern
{
   /*// long value requires two registers*/
	VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__TOTAL_SWITCH_COUNTER             =  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START,     //ext. 0x0064
   VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__VALVE_ACT_STATE                  =  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 2, //ext. 0x0066
   VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__VALID_POSITIONS                  =  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 3, //ext. 0x0067
   VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_POSITION                 =  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 4, //ext. 0x0068
   //																																																 
};																																																	 
// status data -- order must be the same as in the corresponding struct																										 
enum _E_ValveActRegisters_FRAM_Addresses_StatusDataExtern
{
   VALVE_ACT_REGISTERS__FRAM_ADDR__TOTAL_SWITCH_COUNTER                    =  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START,// long value requires four bytes
   VALVE_ACT_REGISTERS__FRAM_ADDR__VALVE_ACT_STATE                         =  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 4,
   VALVE_ACT_REGISTERS__FRAM_ADDR__VALID_POSITIONS                         =  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 6,
   VALVE_ACT_REGISTERS__FRAM_ADDR__CURRENT_POSITION                        =  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 8,
   //
};

//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START           (0x0097)
//#define  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__END             (0x009D)
enum _E_ValveActRegisters_ModbusAddresses_StatusDataIntern
{
   // long value requires two registers
	VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_POS_IN_USTEPS            =  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START,     //ext. 0x0096
   VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CURRENT_ENCODER_EDGE             =  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 2, //ext. 0x0098
   VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__CALIBRATION_FINISHED             =  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 3, //ext. 0x0099
   VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__DEVICE_PARAMS_SAVED              =  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 4, //ext. 0x009A
   VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__DEVICE_PARAMS_LOADED             =  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 5, //ext. 0x009B
   VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_LOADED               =  VALVE_ACT_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 6, //ext. 0x009C
   //																																																
};																																																	
enum _E_ValveActRegisters_FRAM_Addresses_StatusDataIntern
{
   VALVE_ACT_REGISTERS__FRAM_ADDR__CURRENT_POS_IN_USTEPS                   =  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START,              // long value requires four bytes
   VALVE_ACT_REGISTERS__FRAM_ADDR__CURRENT_ENCODER_EDGE                    =  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 4,
   VALVE_ACT_REGISTERS__FRAM_ADDR__CALIBRATION_FINISHED                    =  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 6,
   VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_SAVED                     =  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 8,
   VALVE_ACT_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOADED                    =  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 12,
   VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_LOADED                      =  VALVE_ACT_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 14,
   //
};


enum _E_ValveActRegisters_ValveActStates
{
   VALVE_ACT_REGISTERS__VALVE_ACT_STATE__NONE,
   VALVE_ACT_REGISTERS__VALVE_ACT_STATE__IDLE,
   VALVE_ACT_REGISTERS__VALVE_ACT_STATE__RUN,
   VALVE_ACT_REGISTERS__VALVE_ACT_STATE__CALIBRATION,
   VALVE_ACT_REGISTERS__VALVE_ACT_STATES_COUNTER
   //
};

//---------------------------------------------- structures ------------------------------------------------------------------------------------

typedef struct _S_ValveActRegisters_DeviceParamsIntern
{
   ushort   u16_Cal_UStepsInit;
   ushort   u16_Cal_USPS_Init;
   ushort   u16_Cal_UStepsAcc;
   ushort   u16_Cal_UStepsBreak;
   ushort   u16_Cal_USPS_Start;
   ushort   u16_Cal_USPS_Run;
   ushort   u16_Cal_Mode;
   ushort   u16_Cal_UStepsFullCircle;
   ushort   u16_Cal_TrueGapNumber;
   ushort   u16_Cal_MinUStepsThreshold;
   ushort   u16_Cal_EdgeTypeTriggeredOn;
   ushort   u16_Cal_Tol_UStepsSameGapsCal;
   ushort   u16_Cal_Tol_UStepsSameGapsRun;
   ushort   u16_DAC_HoldMotorDriver;
   ushort   u16_DAC_RunMotorDriver;
   //
} tS_ValveActRegisters_DeviceParamsIntern;


typedef struct _S_ValveActRegisters_DeviceParamsExtern
{
   ushort   u16_CurrentValveType;
   //
} tS_ValveActRegisters_DeviceParamsExtern;


// manually data alignment correction
typedef struct _S_ValveActRegisters_StatusDataExtern
{
   ulong    u32_TotalSwitchCounter;
   ushort   u16_ValveActState;
   ushort   u16_ValidPositions;
   ushort   u16_CurrentPosition;                // usage by display
   //
} tS_ValveActRegisters_StatusDataExtern;

typedef struct _S_ValveActRegisters_StatusDataIntern
{
   ulong    u32_CurrentPosInUSteps;             // stpclkdir module internal usage
   ushort   u16_CurrentEncoderEdge;
   ushort   u16_CalibrationFinished;
   ushort   u16_DeviceParamsSaved;
   ushort   u16_DeviceParamsLoaded;
   ushort   u16_StatusDataLoaded;
   //
} tS_ValveActRegisters_StatusDataIntern;

#endif


// docu of variables

/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_ModbusAddresses_DeviceParamsExtern
*
*  \brief      This enum contains the modbus holding register addresses for valve actuator external device parameters.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_FRAM_Addresses_DeviceParamsExtern
*
*  \brief      This enum contains the FRAM addresses for valve actuator external device parameters.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_ModbusAddresses_DeviceParamsIntern
*
*  \brief      This enum contains the modbus holding register addresses for valve actuator internal device parameters.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_FRAM_Addresses_DeviceParamsIntern
*
*  \brief      This enum contains the FRAM addresses for valve actuator internal device parameters.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_ModbusAddresses_StatusDataExtern
*
*  \brief      This enum contains the valve actuator input register addresses for valve actuator external status data. 
*              Caution: Some entries are determined for unsigned long values, i. e. they need two registers.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_FRAM_Addresses_StatusDataExtern
*
*  \brief      This enum contains the FRAM addresses for valve actuator external status data. Caution: Some entries are 
*              determined for unsigned long values, i. e. they need four bytes for storage.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_ModbusAddresses_CmdsOwner
*
*  \brief      This enum contains the valve actuator \b owner command function holding register addresses.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_FRAM_Addresses_CmdsOwner
*
*  \brief      This enum contains the valve actuator \b owner command function FRAM addresses.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_ModbusAddresses_CmdsUser
*
*  \brief      This enum contains the valve actuator \b user command function holding register addresses.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_FRAM_Addresses_CmdsUser
*
*  \brief      This enum contains the valve actuator \b user command function FRAM addresses.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActRegisters_ValveActStates
*
*  \brief      This enum contains the different valve actuator states from which one is set into 
*              corresponding input register by application.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


// structures
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     _S_ValveActRegisters_DeviceParamsExtern
*
*  \brief      Structure which contains the available valve actuator extern device parameters.
*  
*  \param      u16_CurrentValveType                The valve type which must selected by user.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     _S_ValveActRegisters_DeviceParamsIntern
*
*  \brief      Structure which contains the available valve actuator intern device parameters. These parameters comprise 
*              calibration/motor params and DAC values.
*  
*  \param      u16_Cal_UStepsInit                  The usteps to prepare motor movement before calibration starts.
*  \param      u16_Cal_USPS_Init                   The motor speed in usteps/[s] before calibration starts.
*  \param      u16_Cal_UStepsAcc                   Total usteps for acceleration ramp.
*  \param      u16_Cal_UStepsBreak                 Total usteps for breaking ramp.
*  \param      u16_Cal_USPS_Start                  Start(end) "frequency" of step motor for acceleration/breaking ramp
*                                                  in usteps/[s]. Caution! 
*                                                  ( ((u16_Cal_USPS_Run - u16_Cal_USPS_Start) / u16_Cal_UStepsAcc)  !> u16_Cal_USPS_Start )
*  \param      u16_Cal_USPS_Run                    Run "frequency" of step motor, e.g. 110 * 16 * 16 usteps * [s^(-1)] = 28160 usteps/[s]
*  \param      u16_Cal_Mode                        Valve actuator calibration mode with continuous driven motor (1) or left-right (0)
*                                                  for mechanical stop bar.
*  \param      u16_Cal_UStepsFullCircle            Clocks (microsteps) for a full circle. Default:
*                                                  10667 = 200 (fullsteps according to data sheet) * 16 (usteps) * 10/3 (gear ratio).
*  \param      u16_Cal_TrueGapNumber               True gap number of encoder gap disk.
*  \param      u16_Cal_MinUStepsThreshold          Minimum amount of usteps between two neighboured gaps.
*  \param      u16_Cal_EdgeTypeTriggeredOn         Edge type to trigger on during calibration (active edge check not implemented).
*  \param      u16_Cal_Tol_UStepsSameGapsCal       Total deviance in usteps for SAME gaps during calibration (scan+averaging).
*  \param      u16_Cal_Tol_UStepsSameGapsRun       Total deviance in usteps for SAME gaps during calibration verify (run mode not implemented).
*  \param      u16_DAC_HoldMotorDriver             DAC data input to hold motor driver.
*  \param      u16_DAC_RunMotorDriver              DAC data input to run motor driver.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     _S_ValveActRegisters_StatusDataExtern
*
*  \brief      Structure which contains the available valve actuator extern status data. Status data are needed to store or load 
*              data of valve actuator and to operate it, e. g. the current valve position.
*
*  \param      u32_TotalSwitchCounter              The total amount of valve switches. Can be used as aging indicator
*                                                  of rotor seal.
*  \param      u16_ValveActState                   The current valve actuator state. See \ref _E_ValveActRegisters_ValveActStates.
*  \param      u16_ValidPositions                  This value represents the number of valid valve positions, e. g. S6X40
*                                                  has six available/usable positions.
*  \param      u16_CurrentPosition                 Current valve position numbers, valid from 1 to \ref VALVE_ACT_VALVE_TYPES__MAX_VALVE_POSITIONS.
*                                                  Usage by display.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     _S_ValveActRegisters_StatusDataIntern
*
*  \brief      Structure which contains the available valve actuator intern status data. Status data are needed to store or load 
*              data of valve actuator and to operate it, e. g. the current valve encoder edge position or status flags are used 
*              to know if owner command has worked properly.
*
*  \param      u32_CurrentPosInUSteps              The motor position in microsteps referring to home position.
*  \param      u16_CurrentEncoderEdge              Current encoder gap disk edge position starting at zero up to 21. See \ref _S_ValveActValveTypes_SykamValves.
*  \param      u16_CalibrationFinished             Flag that valve actuator calibration was \em finished recently.
*  \param      u16_DeviceParamsSaved               Flag that valve actuator device parameters were \em saved recently.
*  \param      u16_DeviceParamsLoaded              Flag that valve actuator device parameters were \em loaded recently
*  \param      u16_StatusDataLoaded                Flag that valve actuator status data was loaded recently, i. e. values of 
*                                                  this structure without flags.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
