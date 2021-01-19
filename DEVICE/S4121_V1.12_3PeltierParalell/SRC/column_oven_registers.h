/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_registers.h
*
*  \brief               Header file for \ref column_oven_registers.c
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*
*  \date                2011-02-03     Initial version imported S6510_registers.h.
*  \date                2011-12-05     Reorganized register bank.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test                ->>> 2010-11-03     Checked addresses. Modbus commands with addresses out of command function array are ignored by \ref TSK_ModbusCmdTrigger.
*
*  \todo                FILE OK - BUT extend docu!
*
*
**//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef COLUMN_OVEN_REGISTERS__H
#define COLUMN_OVEN_REGISTERS__H
//---------------------------------------------- defines ---------------------------------------------------------------------------------------
// Internal modbus addresses start from one ("0x0001"). Fifty entries for every category (device/run params...) are allowed.
// If expansion is desired have a look for example at MODBUS_REG_HOLDING_NREGS in AT91SAM7X.h.
// Adapt start an end addresses if register bank is extended!
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START             (0x0191)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__END               (0x020B)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START             (0x0227)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__END               (0x0269)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START                       (0x028B)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__END                         (0x028E)

#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_COMMUNICATION_STATUS_START       (0x0290)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_COMMUNICATION_STATUS_END         (0x0292)

#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START                        (0x02BD)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__END                          (0x02C2)    // last column oven HREG address
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START               (0x00C9)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__END                 (0x00E5)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START               (0x00FB)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__END                 (0x014D)    // last column oven IREG address

// addresses on permanent memory -  start/end addresses must be changed if register bank addresses were changed
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START                    (0x04B1)
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__END                      (0x05A6)
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__START                    (0x05DD)
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__END                      (0x065E)
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_OWNER__START                              (0x06A5)
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_OWNER__END                                (0x06AC)
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_USER__START                               (0x0709)
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_USER__END                                 (0x0714)
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START                      (0x076D)
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__END                        (0x07A4)
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START                      (0x07D1)    // inside extern status data nine (9) additional temperature profiles are stored
#define  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__END                        (0x0CD2)
// other application defines
#define  COLUMN_OVEN_REGISTERS__DEFAULT_CONTROL_DIFFERENTIAL_TIME_IN_SECONDS              (1450)
///*#define  COLUMN_OVEN_REGISTERS__DEFAULT_CONTROL_DIFFERENTIAL_GAIN                         (35734)*/
#define  COLUMN_OVEN_REGISTERS__DEFAULT_CONTROL_DIFFERENTIAL_GAIN                         (17867)//Differential Gain.
#define  COLUMN_OVEN_REGISTERS__HEAT_COOL_PROGRAM_SIZE_WITHOUT_SECTIONS_IN_BYTES          (4)
//---------------------------------------------- enums -----------------------------------------------------------------------------------------
// holding register  HREG
// input register    IREG
// discrete input    DISC
// coils             COIL
// For every single address two bytes are assigned internally.
// The order must be the same as in the corresponding structure.
// Every FRAM address corresponds to one byte, i. e. to save register data we need
// 16 bits (= 2 bytes) for a standard entry.
// The order must be the same as in the corresponding structure.
// Device Params Intern
enum _E_ColumnOvenRegisters_ModbusAddresses_DeviceParamsIntern
{
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CONTROL_SECTION_PARAMS                        =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_THRESHOLD_PPM                =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 120,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_CALIBRATION_RESISTANCE             =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 121,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__GAS_SENSOR_ALARM_ENABLED                      =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START + 122,
   //
};
// Device Params Intern
enum _E_ColumnOvenRegisters_FRAM_Addresses_DeviceParamsIntern
{
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_SECTION_PARAMS                               =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_THRESHOLD_PPM                       =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 240,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_CALIBRATION_RESISTANCE                    =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 242,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_ENABLED                             =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_INTERN__START + 244,
   //
};
// Device Params Extern
enum _E_ColumnOvenRegisters_ModbusAddresses_DeviceParamsExtern
{
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__HEAT_COOL_PROGRAM_TO_MODIFY                   =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_OFFSET                            =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START + 62,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_UNIT                              =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START + 63,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND      =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START + 64,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__TEMPERATURE_OFFSET_NEGATIVE_POSITIVE          =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_EXTERN__START + 65,
   //
};
// Device Params Extern
enum _E_ColumnOvenRegisters_FRAM_Addresses_DeviceParamsExtern
{
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_TO_MODIFY                          =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__START,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__TEMPERATURE_OFFSET                                   =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__START + 124,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__TEMPERATURE_UNIT                                     =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__START + 126,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND             =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_EXTERN__START + 128,
   //
};
// Status Data Intern
enum _E_ColumnOvenRegisters_ModbusAddresses_StatusDataIntern
{
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__GAS_SENSOR_CURRENT_RESISTANCE                 =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START,     // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_INT_CYCLE_COUNT             =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 2, // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_DEVIATION_ACCUMULATED                 =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 4, // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_DEVIATION_LAST                        =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 6, // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_INTEGRATION_TIME                      =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 8, // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_DIFFERENTIAL_TIME                     =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 10, // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_INTEGRATION_GAIN                      =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 12, // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_DIFFERENTIAL_GAIN                     =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 14, // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_PROPORTIONAL_GAIN                     =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 16, // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_HEAT_COOL_PROGRAM                       =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 18,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_TEMPERATURE_SENSOR                      =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 19,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_GAS_SENSOR                              =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 20,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATE_CONTROL_SECTION_PARAMS                  =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 21,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__DEVICE_PARAMS_LOADED                          =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 22,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_LOADED                            =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 23,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_DAC_OUTPUT_VOLTAGE                    =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 24,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_STORED                      =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 25,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__CONTROL_SECTIONS_NUM                          =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 26,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__GAS_SENSOR_ALARM_DETECTED                     =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 27,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__TEST                                          =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_INTERN__START + 28,
   //
};
// Status Data Intern
enum _E_ColumnOvenRegisters_FRAM_Addresses_StatusDataIntern
{
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_CURRENT_RESISTANCE                        =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START,     // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_INT_CYCLE_COUNT                    =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 4, // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_DEVIATION_ACCUMULATED                        =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 8, // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_DEVIATION_LAST                               =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 12, // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_INTEGRATION_TIME                             =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 16, // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_DIFFERENTIAL_TIME                            =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 20, // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_INTEGRATION_GAIN                             =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 24, // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_DIFFERENTIAL_GAIN                            =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 28, // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_PROPORTIONAL_GAIN                            =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 32, // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATE_HEAT_COOL_PROGRAM                              =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 36,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATE_TEMPERATURE_SENSOR                             =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 38,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATE_GAS_SENSOR                                     =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 40,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATE_CONTROL_SECTION_PARAMS                         =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 42,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__DEVICE_PARAMS_LOADED                                 =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 44,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_LOADED                                   =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 46,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_DAC_OUTPUT_VOLTAGE                           =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 48,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_STORED                             =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 50,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__CONTROL_SECTIONS_NUM                                 =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 52,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__GAS_SENSOR_ALARM_DETECTED                            =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_INTERN__START + 54,
   //
};

#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START               (0x00FB)
#define  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__END                 (0x014D)   
// Status Data Extern
enum _E_ColumnOvenRegisters_ModbusAddresses_StatusDataExtern
{
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_IN_USE                      =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_TIME_ELAPSED                =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 62,   // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_TIME_REMAIN                 =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 64,   // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_TIME_TOTAL                  =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 66,   // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_TIME_SECTION_REMAIN         =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 68,   // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_TIME_SECTION_TOTAL          =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 70,   // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_TEMPERATURE                       =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 72,   // long value requires two registers
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__COLUMN_OVEN_STATE                             =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 74,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_CURRENT_ID                  =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 75,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_CURRENT_SECTION_ID          =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 76,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_CURRENT_SECTIONS_DEFINED    =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 77,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_CURRENT_SECTION_SET_POINT   =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 78,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_MODIFIED_ID                 =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 79,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_MODIFIED_SECTIONS_DEFINED   =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 80,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_MAX_ID                      =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 81,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__HEAT_COOL_PROGRAM_MAX_SECTIONS                =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_IREG__STATUS_DATA_EXTERN__START + 82,
   //
};
// Status Data Extern
enum _E_ColumnOvenRegisters_FRAM_Addresses_StatusDataExtern
{
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_IN_USE                             =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_TIME_ELAPSED                       =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1240,  // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_TIME_REMAIN                        =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1244,  // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_TIME_TOTAL                         =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1248,  // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_TIME_SECTION_REMAIN                =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1252,  // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_TIME_SECTION_TOTAL                 =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1256,  // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__COLUMN_OVEN_TEMPERATURE                              =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1260,  // long value requires four bytes
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__COLUMN_OVEN_STATE                                    =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1264,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_CURRENT_ID                         =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1262,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_CURRENT_SECTION_ID                 =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1264,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_CURRENT_SECTIONS_DEFINED           =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1266,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_CURRENT_SECTION_SET_POINT          =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1268,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_MODIFIED_ID                        =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1270,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_MODIFIED_SECTIONS_DEFINED          =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1272,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_MAX_ID                             =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1274,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__HEAT_COOL_PROGRAM_MAX_SECTIONS                       =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__STATUS_DATA_EXTERN__START + 1276,
   //
};
// Commands Owner
enum _E_ColumnOvenRegisters_ModbusAddresses_CmdsOwner
{
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__OCMD_CONTROL_SECTIONS_PARAMS_SAVE             =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__OCMD_DEVICE_PARAMS_LOAD                       =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  1,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__OCMD_STATUS_DATA_LOAD                         =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  2,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__OCMD_HEAT_COOL_PROGRAMS_RESET                 =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  3,
   //
};
// Commands Owner
enum _E_ColumnOvenRegisters_FRAM_Addresses_CmdsOwner
{
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__OCMD_CONTROL_SECTIONS_PARAMS_SAVE                    =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_OWNER__START,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__OCMD_DEVICE_PARAMS_LOAD                              =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  2,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__OCMD_STATUS_DATA_LOAD                                =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  4,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__OCMD_HEAT_COOL_PROGRAMS_RESET                        =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  6,
   //
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Commands Owner
enum _E_ColumnOvenRegisters_ModbusCommunication
{
   MODBUS_ADDR_HREG__DEVICE_COMMUNICATION_STATUS             =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_COMMUNICATION_STATUS_START,
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Commands User
enum _E_ColumnOvenRegisters_ModbusAddresses_CmdsUser
{
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_PROGRAM_START                  =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_PROGRAM_STOP                   =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START  +  1,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_PROGRAM_DELETE                 =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START  +  2,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_PROGRAM_SAVE                   =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START  +  3,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_PROGRAM_SELECT_TO_MODIFY       =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START  +  4,
   COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__UCMD_HEAT_COOL_CALIBRATION_START              =  COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START  +  5,
   //
};
// Commands User
enum _E_ColumnOvenRegisters_FRAM_Addresses_CmdsUser
{
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__UCMD_HEAT_COOL_PROGRAM_START                         =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_USER__START,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__UCMD_HEAT_COOL_PROGRAM_STOP                          =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_USER__START  +  2,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__UCMD_HEAT_COOL_PROGRAM_DELETE                        =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_USER__START  +  4,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__UCMD_HEAT_COOL_PROGRAM_SAVE                          =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_USER__START  +  6,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__UCMD_HEAT_COOL_PROGRAM_SELECT_TO_MODIFY              =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_USER__START  +  8,
   COLUMN_OVEN_REGISTERS__FRAM_ADDR__UCMD_HEAT_COOL_CALIBRATION_START                     =  COLUMN_OVEN_REGISTERS__FRAM_ADDR__CMDS_USER__START  +  10,
   //
};
// states
enum _E_ColumnOvenRegisters_ColumnOvenStates
{
   COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__NONE,
   COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__INIT,
   COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__STOP,
   COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__HOLD,
   COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__RUN,
   COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__FINISHED,
   COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__UNDER_TEMPERATURE,
   COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__OVER_TEMPERATURE,
   COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATE__GAS_ALARM,
   COLUMN_OVEN_REGISTERS__COLUMN_OVEN_STATES_COUNTER
   //
};
enum _E_ColumnOvenRegisters_StatesHeatCoolProgram
{
   COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__NONE,
   COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__STOP,
   COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__HOLD,
   COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__HEATING,
   COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__COOLING,
   COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__SET_POINT_REACHED,
   COLUMN_OVEN_REGISTERS__STATE_HEAT_COOL_PROGRAM__FINSIHED,
   COLUMN_OVEN_REGISTERS__STATES_HEAT_COOL_PROGRAM_COUNTER
   //
};
enum _E_ColumnOvenRegisters_StatesTemperatureSensor
{
   COLUMN_OVEN_REGISTERS__STATE_TEMPERATURE_SENSOR__NONE,
   COLUMN_OVEN_REGISTERS__STATE_TEMPERATURE_SENSOR__CONNECTED,
   COLUMN_OVEN_REGISTERS__STATE_TEMPERATURE_SENSOR__DISCONNECTED,
   COLUMN_OVEN_REGISTERS__STATES_TEMPERATURE_SENSOR_COUNTER
   //
};
enum _E_ColumnOvenRegisters_StatesGasSensor
{
   COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__NONE,
   COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__CONNECTED,
   COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__DISCONNECTED,
   COLUMN_OVEN_REGISTERS__STATE_GAS_SENSOR__WARM_UP,
   COLUMN_OVEN_REGISTERS__STATES_GAS_SENSOR_COUNTER
   //
};
enum _E_ColumnOvenRegisters_StatesControlSectionParams
{
   COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__NONE,
   COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__LOADED,
   COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__SAVED,
   COLUMN_OVEN_REGISTERS__STATE_CONTROL_SECTION_PARAMS__MODIFIED,
   COLUMN_OVEN_REGISTERS__STATES_CONTROL_SECTION_PARAMS_COUNTER
   //
};
enum _E_ColumnOvenRegisters_StatesOther
{
   COLUMN_OVEN_REGISTERS__DEVICE_PARAMS_LOADED                             =  (0x0001),
   COLUMN_OVEN_REGISTERS__STATUS_DATA_LOADED                               =  (0x0001),
   //
};
enum _E_ColumnOvenRegisters_TemperatureUnits
{
   COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__NONE,
   COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__CELSIUS,
   COLUMN_OVEN_REGISTERS__TEMPERATURE_UNIT__FAHRENHEIT,
   COLUMN_OVEN_REGISTERS__TEMPERATURE_UNITS_MAX
   //
};
//---------------------------------------------- structures ------------------------------------------------------------------------------------
typedef struct _S_ColumnOvenRegisters_HeatCoolProgramSection
{
   ushort   u16_SetPoint;
   ushort   u16_HoldTime;
} tS_ColumnOvenRegisters_HeatCoolProgramSection;
typedef struct _S_ColumnOvenRegisters_HeatCoolProgram
{
   // the struct array must have the size according to external documentation!
   struct   _S_ColumnOvenRegisters_HeatCoolProgramSection   SA_Section[AT91SAM7X__MAX_HEAT_COOL_PROGRAM_SECTIONS];
   ushort   u16_NumActiveSections;
   ushort   u16_TimeTotal;
} tS_ColumnOvenRegisters_HeatCoolProgram;
typedef struct _S_ColumnOvenRegisters_ControlSectionParams
{
   ushort   u16_Ki;
   ushort   u16_TKi;
   ushort   u16_Kp;
   ushort   u16_TKp;
} tS_ColumnOvenRegisters_ControlSectionParams;
typedef struct _S_ColumnOvenRegisters_DeviceParamsIntern
{
   tS_ColumnOvenRegisters_ControlSectionParams  SA_ControlSectionParams[AT91SAM7X__MAX_CONTROL_PARAM_SECTIONS];
   ulong                                        u32_GasSensorAlarmThresholdPPM;
   ushort                                       u16_GasSensorCalibrationResistance;
   ushort                                       u16_GasSensorAlarmEnabled;
} tS_ColumnOvenRegisters_DeviceParamsIntern;
typedef struct _S_ColumnOvenRegisters_DeviceParamsExtern
{
   struct   _S_ColumnOvenRegisters_HeatCoolProgram    S_HeatCoolProgramToModify;
   ushort   u16_TemperatureOffset;
   ushort   u16_TemperatureUnit;
   ushort   u16_HeatCoolProgramSetPointReachedBand;
   ushort   u16_TemperatureOffset_Positive_Negative;
   
} tS_ColumnOvenRegisters_DeviceParamsExtern;
typedef struct _S_ColumnOvenRegisters_StatusDataIntern
{
   ulong    u32_GasSensorCurrentResistance;
   ulong    u32_HeatCoolProgramIntCycleCount;
   ulong    u32_ControlDeviationAccumulated;
   ulong    u32_ControlDeviationLast;
   ulong    u32_ControlIntegrationTime;
   ulong    u32_ControlDifferentialTime;
   ulong    u32_ControlIntegrationGain;
   ulong    u32_ControlDifferentialGain;
   ulong    u32_ControlProportionalGain;
   ushort   u16_StateHeatCoolProgram;
   ushort   u16_StateTemperatureSensor;
   ushort   u16_StateGasSensor;
   ushort   u16_StateControlSectionParams;
   ushort   u16_DeviceParamsLoaded;
   ushort   u16_StatusDataLoaded;
   ushort   u16_ControlDacOuputVoltage;
   ushort   u16_HeatCoolProgramStored;
   ushort   u16_ControlSectionsNum;
   ushort   u16_GasSensorAlarmDetected;
   ushort   u16_Test;
} tS_ColumnOvenRegisters_StatusDataIntern;
typedef struct _S_ColumnOvenRegisters_StatusDataExtern
{
   struct   _S_ColumnOvenRegisters_HeatCoolProgram    S_HeatCoolProgramInUse;
   ulong    u32_HeatCoolProgramTimeElapsed;
   ulong    u32_HeatCoolProgramTimeRemain;
   ulong    u32_HeatCoolProgramTimeTotal;
   ulong    u32_HeatCoolProgramTimeSectionRemain;
   ulong    u32_HeatCoolProgramTimeSectionTotal;
   ulong    u32_ColumnOvenTemperature;
   ushort   u16_ColumnOvenState;
   ushort   u16_HeatCoolProgramCurrentId;
   ushort   u16_HeatCoolProgramCurrentSectionId;
   ushort   u16_HeatCoolProgramCurrentSectionsDefined;
   ushort   u16_HeatCoolProgramCurrentSectionSetPoint;
   ushort   u16_HeatCoolProgramModifiedId;
   ushort   u16_HeatCoolProgramModifiedSectionsDefined;
   ushort   u16_HeatCoolProgramMaxId;
   ushort   u16_HeatCoolProgramMaxSections;
} tS_ColumnOvenRegisters_StatusDataExtern;
#endif
// docu of variables
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ColumnOvenRegisters_ModbusAddresses_DeviceParams
*
*  \brief      This enum contains the modbus holding register addresses for column oven device parameters.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ColumnOvenRegisters_FRAM_Addresses_DeviceParams
*
*  \brief      This enum contains the FRAM addresses for column oven device parameters.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ColumnOvenRegisters_ModbusAddresses_StatusData
*
*  \brief      This enum contains the modbus input register addresses for column oven status data. Caution some entries are determined for
*              ulong values, i.e. they need two registers.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ColumnOvenRegisters_FRAM_Addresses_StatusData
*
*  \brief      This enum contains the FRAM addresses for column oven status data. Caution some entries are determined for
*              ulong values, i.e. they need four bytes for storage.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ColumnOvenRegisters_ModbusAddresses_CmdsOwner
*
*  \brief      This enum contains the column oven \b owner command function holding register addresses.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ColumnOvenRegisters_FRAM_Addresses_CmdsOwner
*
*  \brief      This enum contains the column oven \b owner command function FRAM addresses.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ColumnOvenRegisters_ModbusAddresses_CmdsUser
*
*  \brief      This enum contains the column oven \b user command function holding register addresses.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ColumnOvenRegisters_FRAM_Addresses_CmdsUser
*
*  \brief      This enum contains the column oven \b user command function FRAM addresses.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ColumnOvenRegisters_CoulumOvenStates
*
*  \brief      This enum contains the different column oven states from which one is set into corresponding input
*              register by application.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     _S_ColumnOvenRegisters_DeviceParams
*
*  \brief      Structure which contains the available column oven device params. These params comprise
*              ...
*
*  \param      u16_                The .
*
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     _S_ColumnOvenRegisters_StatusData
*
*  \brief      Structure which contains the available column oven status data. Status data are needed to store or load some values to
*              operate device, e.g. ... or status flags to prevent wrong execution order of owner command functions.
*
*  \param      u16_                      A .
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
