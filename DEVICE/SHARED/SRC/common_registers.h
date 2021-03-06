/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_registers.h
*
*  \brief               This header file contains all common register bank addresses, FRAM addresses, the Sykam device data structure
*                       and the Sykam device class enumeration.
*
*  \author              Ruediger Radtke
*  \version             1.2
*
*
*  \date                2010-04-26     Initial version of 'sykam_device_params.h' (FM).
*  \date                2010-06-17     rradtke imported file from FM,
*                                      changed U16 to ushort, changed entry order in S_SykamDeviceParams.
*  \date                2010-07-13     Changed types according to coding rules by rradtke.
*  \date                2011-05-25     RR: Added options field in structure to distinct between different device types,
*                                      i.e. autosampler cooling or autosampler standard; added degaser entry in
*                                      \em _E_SykamDeviceClass.
*  \date                2011-07-28     RR: Imported file layout from S6510_register.h, renamed and moved structure and enum
*                                      of Sykam device data (no change of entries) to this new file.
*  \date                2011-08-22     Assigned new register bank addresses.
*  \date                2012-07-13     <em>File version 1.1</em>. RR: Added mainboard power line voltage status fields
*                                      and a device params to enable/disable voltage monitoring in common register bank.
*  \date                2012-11-09     <em>File version 1.2</em>. RR: Removed register bank entry "mainboard voltage
*                                      monitoring disabling" from device params.
*  \date                2013-01-18     RR: Added device option for column oven.
*
*
**//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef COMMON_REGISTERS__H
#define COMMON_REGISTERS__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

// Internal modbus addresses start from one ("0x0001"). Fifty entries for every category are allowed. If expansion
// is desired maybe modify for example at MBPB_REG_HOLDING_NREGS in AT91SAM7X.h.
// Adapt start and end adresses if register bank is extended!

// Abbreviations:
//                HREG  -  holding register
//                IREG  -  input register
//                DISC  -  discrete input
//                COIL  -  coils

// holding register start and end addresses
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__START                              (0x0001)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__MANUFACTURER_DEVICE_ID_START       (0x0001)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__MANUFACTURER_DEVICE_ID_END         (0x000A)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__MANUFACTURER_ID_START              (0x000B)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__MANUFACTURER_ID_END                (0x0014)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OEM_DEVICE_ID_START                (0x0015)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OEM_DEVICE_ID_END                  (0x001E)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OEM_ID_START                       (0x001F)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OEM_ID_END                         (0x0028)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__SERIAL_START                       (0x0029)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__SERIAL_END                         (0x002C)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_START                 (0x002D)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__DEVICE_CLASS_END                   (0x002D)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__VERSION_START                      (0x002E)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__VERSION_END                        (0x002E)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_START                      (0x002F)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__OPTIONS_END                        (0x002F)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA__END                                (0x002F)    // gap for three entries


#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS__START                                  (0x0033)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS__END                                    (0x0033)    // gap for 49 entries 
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START                                     (0x0065)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__END                                       (0x006B)    // gap for 43 entries
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__START                                      (0x0097)
#define  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__END                                        (0x0097)

// input register start and end addresses
#define  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START                             (0x0001)
#define  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__END                               (0x0030)


// addresses on permanent memory -  start/end addresses must be changed if register bank addresses were changed
#define  COMMON_REGISTERS__FRAM_ADDR__SYKAM_DEVICE_DATA__START                                     (0x0001)
#define  COMMON_REGISTERS__FRAM_ADDR__SYKAM_DEVICE_DATA__END                                       (0x005E)    //60### gap for two entries
#define  COMMON_REGISTERS__FRAM_ADDR__DEVICE_PARAMS__START                                         (0x0065)
#define  COMMON_REGISTERS__FRAM_ADDR__DEVICE_PARAMS__END                                           (0x0066)    // gap for 49 entries 
#define  COMMON_REGISTERS__FRAM_ADDR__CMDS_OWNER__START                                            (0x00C9)
#define  COMMON_REGISTERS__FRAM_ADDR__CMDS_OWNER__END                                              (0x00D6)    // gap for 44 entries
#define  COMMON_REGISTERS__FRAM_ADDR__CMDS_USER__START                                             (0x012D)
#define  COMMON_REGISTERS__FRAM_ADDR__CMDS_USER__END                                               (0x012D)    // gap for 50 entries
#define  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START                                    (0x0191)
#define  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__END                                      (0x01F0)

// other defines
#define  COMMON_REGISTERS__MAX_CHARS__ERROR_STRING_SW                                              (50)
#define  COMMON_REGISTERS__MAX_CHARS__MANUFACTURER_DEVICE_ID                                       (20)        // only even num of chars
#define  COMMON_REGISTERS__MAX_CHARS__MANUFACTURER_ID                                              (20)
#define  COMMON_REGISTERS__MAX_CHARS__OEM_DEVICE_ID                                                (20)
#define  COMMON_REGISTERS__MAX_CHARS__OEM_ID                                                       (20)
#define  COMMON_REGISTERS__MAX_CHARS__SERIAL                                                       (8)

#define  COMMON_REGISTERS__SYKAM_DEVICE_DATA_CHAR_ARRAYS_TOTAL_WORD_SIZE                           ((COMMON_REGISTERS__MAX_CHARS__MANUFACTURER_DEVICE_ID + COMMON_REGISTERS__MAX_CHARS__MANUFACTURER_ID + COMMON_REGISTERS__MAX_CHARS__OEM_DEVICE_ID + COMMON_REGISTERS__MAX_CHARS__OEM_ID + COMMON_REGISTERS__MAX_CHARS__SERIAL) / 2 )
#define  COMMON_REGISTERS__SYKAM_DEVICE_DATA_WRITE_ENABLE_CODE                                     (0x5379)

//---------------------------------------------- enums -----------------------------------------------------------------------------------------

enum _E_CommonRegisters_SykamDeviceClass
{
  COMMON_REGISTERS__DC_PUMP               = 1,
  COMMON_REGISTERS__DC_AUTOSAMPLER        = 2,
  COMMON_REGISTERS__DC_FRACTION_COLLECTOR = 4,
  COMMON_REGISTERS__DC_COLUMN_OVEN        = 8,
  COMMON_REGISTERS__DC_REACTOR            = 16,
  COMMON_REGISTERS__DC_DETECTOR           = 32,
  COMMON_REGISTERS__DC_VALVE              = 64,
  COMMON_REGISTERS__DC_SUPPRESSOR         = 128,
  COMMON_REGISTERS__DC_DEGASSER           = 256,
};

enum _E_CommonRegisters_ColumnOvenOptions
{
  COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT                                =  1,
  COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT_AND_COOL                       =  2,
  COMMON_REGISTERS__COLUMN_OVEN_OPTION_VALVE_ACTUATOR                      =  4,
  COMMON_REGISTERS__COLUMN_OVEN_OPTION_AUTOSAMPLER                         =  8,
  COMMON_REGISTERS__COLUMN_OVEN_OPTION_PROGRAM                             =  16,
};

enum _E_CommonRegisters_PumpOptions
{
  COMMON_REGISTERS__PUMP_QUAD                                              =  1,
  COMMON_REGISTERS__PUMP_HIGH_PRESSURE_GRADIENT                            =  2,
  COMMON_REGISTERS__PUMP_DEGASER                                           =  4,
  COMMON_REGISTERS__PUMP_MIXING_UNIT                                       =  8,
};

enum _E_CommonRegisters_DetectorOptions
{
  COMMON_REGISTERS__DETECTOR_OPTION_SCAN_PROGRAM                           =  1,
  COMMON_REGISTERS__DETECTOR_OPTION_TWO_CHANNELS                           =  2,
};


// status data -- order must be the same as in the corresponding struct
enum _E_CommonRegisters_ModbusAddresses_DeviceStatusData
{
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_RUN_TIME_COUNTER                   =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START,        // long value requires two registers
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB1                 =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  2,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB2                 =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  3,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB3                 =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  4,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB4                 =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  5,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB5                 =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  6,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB6                 =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  7,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB7                 =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  8,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_SUB8                 =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  9,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE1                =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  10,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE2                =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  11,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE3                =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  12,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE4                =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  13,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE5                =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  14,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE6                =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  15,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE7                =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  16,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CODE_SW_BASE8                =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  17,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW                       =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  18,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_STRING_SW                    =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  19,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATE                              =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  44,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__MAINBOARD_CURRENT_VOLTAGE_AT_24000_MV     =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  45,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__MAINBOARD_CURRENT_VOLTAGE_AT_5000_MV      =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  46,
   COMMON_REGISTERS__MODBUS_ADDR_IREG__MAINBOARD_CURRENT_VOLTAGE_AT_3300_MV      =  COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_STATUS_DATA__START  +  47,
   //
};

// FRAM status data -- order must be the same as in the corresponding struct
enum _E_CommonRegisters_FRAM_Addresses_DeviceStatusData
{
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_RUN_TIME_COUNTER                          =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START,        // long value requires four bytes
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_SUB1                        =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  4,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_SUB2                        =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  6,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_SUB3                        =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  8,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_SUB4                        =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  10,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_SUB5                        =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  12,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_SUB6                        =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  14,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_SUB7                        =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  16,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_SUB8                        =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  18,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_BASE1                       =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  20,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_BASE2                       =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  22,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_BASE3                       =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  24,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_BASE4                       =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  26,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_BASE5                       =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  28,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_BASE6                       =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  30,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_BASE7                       =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  32,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CODE_SW_BASE8                       =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  34,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_CNT_SW                              =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  36,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ERROR_STRING_SW                           =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  38,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATE                                     =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  88,
   COMMON_REGISTERS__FRAM_ADDR__MAINBOARD_CURRENT_VOLTAGE_AT_24000_MV            =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  90,
   COMMON_REGISTERS__FRAM_ADDR__MAINBOARD_CURRENT_VOLTAGE_AT_5000_MV             =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  92,
   COMMON_REGISTERS__FRAM_ADDR__MAINBOARD_CURRENT_VOLTAGE_AT_3300_MV             =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_STATUS_DATA__START   +  94,
   //
};

enum _E_CommonRegisters_ModbusAddresses_DeviceParams
{
   COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_ACTIVE_INTERFACE                   =  COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS__START,
   //
};

enum _E_CommonRegisters_FRAM_Addresses_DeviceParams
{
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_ACTIVE_INTERFACE                          =  COMMON_REGISTERS__FRAM_ADDR__DEVICE_PARAMS__START,
   //
};

enum _E_CommonRegisters_ModbusAddresses_CmdsOwner
{
   COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA_LOAD                    =  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START,
   COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA_SAVE                    =  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  1,
   COMMON_REGISTERS__MODBUS_ADDR_HREG__MODBUS_SET_SLAVE_ADDRESS                  =  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  2,
   COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_RUN_TIME_COUNTER_RESET             =  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  3,
   COMMON_REGISTERS__MODBUS_ADDR_HREG__ERROR_CODE_SW_CLEAR_CURR                  =  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  4,
   COMMON_REGISTERS__MODBUS_ADDR_HREG__ERROR_CODE_SW_CLEAR_ALL                   =  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  5,
   COMMON_REGISTERS__MODBUS_ADDR_HREG__SYKAM_DEVICE_DATA_WRITE_ENABLE            =  COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__START  +  6,
   //
};

enum _E_CommonRegisters_FRAM_Addresses_CmdsOwner
{
   COMMON_REGISTERS__FRAM_ADDR__SYKAM_DEVICE_DATA_LOAD                           =  COMMON_REGISTERS__FRAM_ADDR__CMDS_OWNER__START,
   COMMON_REGISTERS__FRAM_ADDR__SYKAM_DEVICE_DATA_SAVE                           =  COMMON_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  2,
   COMMON_REGISTERS__FRAM_ADDR__MODBUS_SET_SLAVE_ADDRESS                         =  COMMON_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  4,
   COMMON_REGISTERS__FRAM_ADDR__DEVICE_RUN_TIME_COUNTER_RESET                    =  COMMON_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  6,
   COMMON_REGISTERS__FRAM_ADDR__ERROR_CODE_SW_CLEAR_CURR                         =  COMMON_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  8,
   COMMON_REGISTERS__FRAM_ADDR__ERROR_CODE_SW_CLEAR_ALL                          =  COMMON_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  10,
   COMMON_REGISTERS__FRAM_ADDR__SYKAM_DEVICE_DATA_WRITE_ENABLE                   =  COMMON_REGISTERS__FRAM_ADDR__CMDS_OWNER__START  +  12,
   //
};

//enum _E_CommonRegisters_ModbusAddresses_CmdsUser
//{
//   //
//};

//enum _E_CommonRegisters_FRAM_Addresses_CmdsUser
//{
//   //
//};

enum _E_CommonRegisters_DeviceStates
{
  COMMON_REGISTERS__DEVICE_STATE__NONE,
  COMMON_REGISTERS__DEVICE_STATE__INIT,
  COMMON_REGISTERS__DEVICE_STATE__IDLE,
  COMMON_REGISTERS__DEVICE_STATE__BUSY,
  COMMON_REGISTERS__DEVICE_STATE__ERROR,
  COMMON_REGISTERS__DEVICE_STATES_COUNTER
};

enum  _E_CommonRegisters_DeviceActiveInterfaceList
{
   COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__NONE,
   COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__DISPLAY_KEYS,
   COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__EXTERN_CONTROL_CONTACT,
   COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__RS232,
   COMMON_REGISTERS__ACTIVE_INTERFACES_COUNTER
};

//---------------------------------------------- structures ------------------------------------------------------------------------------------

typedef struct
{
   uchar    u8A_ManufacturerDeviceID[COMMON_REGISTERS__MAX_CHARS__MANUFACTURER_DEVICE_ID];
   uchar    u8A_ManufacturerID[COMMON_REGISTERS__MAX_CHARS__MANUFACTURER_ID];
   uchar    u8A_OEM_DeviceID[COMMON_REGISTERS__MAX_CHARS__OEM_DEVICE_ID];
   uchar    u8A_OEM_ID[COMMON_REGISTERS__MAX_CHARS__OEM_ID];
   uchar    u8A_Serial[COMMON_REGISTERS__MAX_CHARS__SERIAL];
   ushort   u16_DeviceClass;
   ushort   u16_Version;
   ushort   u16_Options;
   // to be continued...
} tS_CommonRegisters_SykamDeviceData, *tpS_CommonRegisters_SykamDeviceData;


// manually data alignment correction
struct _S_CommonRegisters_DeviceStatusData
{
   ulong    u32_DeviceRunTimeCounter;
   ushort   u16_DeviceErrorCodeSW_Sub1;
   ushort   u16_DeviceErrorCodeSW_Sub2;
   ushort   u16_DeviceErrorCodeSW_Sub3;
   ushort   u16_DeviceErrorCodeSW_Sub4;
   ushort   u16_DeviceErrorCodeSW_Sub5;
   ushort   u16_DeviceErrorCodeSW_Sub6;
   ushort   u16_DeviceErrorCodeSW_Sub7;
   ushort   u16_DeviceErrorCodeSW_Sub8;
   ushort   u16_DeviceErrorCodeSW_Base1;
   ushort   u16_DeviceErrorCodeSW_Base2;
   ushort   u16_DeviceErrorCodeSW_Base3;
   ushort   u16_DeviceErrorCodeSW_Base4;
   ushort   u16_DeviceErrorCodeSW_Base5;
   ushort   u16_DeviceErrorCodeSW_Base6;
   ushort   u16_DeviceErrorCodeSW_Base7;
   ushort   u16_DeviceErrorCodeSW_Base8;
   ushort   u16_DeviceErrorCntSW;
   uchar    u8A_DeviceErrorStringSW[COMMON_REGISTERS__MAX_CHARS__ERROR_STRING_SW];
   ushort   u16_DeviceState;
   ushort   u16_MainboardCurrentVoltageAt24000mV;
   ushort   u16_MainboardCurrentVoltageAt5000mV;
   ushort   u16_MainboardCurrentVoltageAt3300mV;
   //
};

struct _S_CommonRegisters_DeviceParams
{
   ushort   u16_DeviceActiveInterface;
   //
};


#endif


// docu of variables
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_CommonRegisters_SykamDeviceClass
*
*  \brief      This enum contains the Sykam device classes. Though a single bit represents a device class, a special device
*              can incorporate multiple device classes.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_CommonRegisters_ModbusAddresses_DeviceStatusData
*
*  \brief      This enum contains the modbus input register addresses for common device status data. Caution some 
*              entries are determined for ulong values, i.e. they need two (2) registers.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_CommonRegisters_FRAM_Addresses_DeviceStatusData
*
*  \brief      This enum contains the FRAM addresses for common device status data. Caution some entries are 
*              determined for ulong values, i.e. they need four (4) bytes for storage.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_CommonRegisters_ModbusAddresses_DeviceParams
*
*  \brief      This enum contains the modbus holding register addresses for common device parameters.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_CommonRegisters_FRAM_Addresses_DeviceParams
*
*  \brief      This enum contains the FRAM addresses for common device parameters.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_CommonRegisters_ModbusAddresses_CmdsOwner
*
*  \brief      This enum contains the modbus holding register addresses for common owner commands. Commands are externally
*              documentated in an excel table.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_CommonRegisters_FRAM_Addresses_CmdsOwner
*
*  \brief      This enum contains the FRAM addresses for common owner commands. They are used if the modbus PDU data or another
*              param inside a command, for which no address on FRAM exists, shall be saved.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_CommonRegisters_DeviceStates
*
*  \brief      This enum contains the common device states. They are set inside application control tasks every time
*              before and after a command is excuted.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_CommonRegisters_DeviceActiveInterfaceList
*
*  \brief      This enum contains the common device interfaces which can be selected. Set is possible by user via display
*              (->Mobus->SPI) or by owner/service technician via RS232. Only one interface can be active at once. If interface
*              \ref COMMON_REGISTERS__DEVICE_ACTIVE_INTERFACE__EXTERN_CONTROL_CONTACT is selected no user commands can be executed
*              via RS232.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

// structures
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     tS_CommonRegisters_SykamDeviceData
*
*  \brief      Structure contains Sykam device data.
*
*  \param      u8A_ManufacturerDeviceID[COMMON_REGISTERS__MAX_CHARS__MANUFACTURER_DEVICE_ID]    Manufacturer device identifier, e.g. "S6510".
*  \param      u8A_ManufacturerID[COMMON_REGISTERS__MAX_CHARS__MANUFACTURER_ID]                 Manufacturer identifier, e.g. "Sykam".
*  \param      u8A_OEM_DeviceID[COMMON_REGISTERS__MAX_CHARS__OEM_DEVICE_ID]                     OEM device identifier, e.g. "VE6510". <b>Editable in register bank.</b>
*  \param      u8A_OEM_ID[COMMON_REGISTERS__MAX_CHARS__OEM_ID]                                  OEM identifier, e.g. "Viscotec". <b>Editable in register bank.</b>
*  \param      u8A_Serial[COMMON_REGISTERS__MAX_CHARS__SERIAL]                                  Serial number string, e.g. "010211" stands for series "01", device "02"  \n
*														                                                      and jear 20"11". <b>Editable in register bank.</b>
*  \param      u16_DeviceClass                                                                  Device class code, e.g. "0x0040" (sykam valve).
*  \param      u16_Version                                                                      Software version. First dezimal digit represents the release and last two the bugfix.
*                                                                                               E.g. 102 us read as "v1.02"
*  \param      u16_Options                                                                      This option field declares the device specific type in bit-ORed manner. E.g. for 
*                                                                                               the column oven 0x0001 means oven with heat, 0x0002 means oven with heat/cool and 
*                                                                                               0x0004 means column oven has as valve actuator inside. Therefore 0x0005 means column
*                                                                                               has heat and valve actuator.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     _S_CommonRegisters_DeviceStatusData
*
*  \brief      Structure which contains the common device status data. Status data are needed to store or load some values to operate
*              device, e.g. the current device run time shall be restored if device is restarted.
*     
*  \param      u32_DeviceRunTimeCounter                                                   This is the device run-time counter. Its unit is [s] and is mostly 
*                                                                                         updated every 10 s. This depends on RTT init and restart configuration.
*  \param      u16_DeviceErrorCodeSW_Sub1                                                 This is the first software sub error code in an error stack. It is greater 
*                                                                                         than zero if any sub error occurred. See \ref tU_LibExceptions_ErrorCodeSW or 
*                                                                                         \ref tU_LibExceptionsDrv_ErrorCodeSW.
*  \param      u16_DeviceErrorCodeSW_Sub2                                                 See \ref u16_DeviceErrorCodeSW_Sub1.
*  \param      u16_DeviceErrorCodeSW_Sub3                                                 See \ref u16_DeviceErrorCodeSW_Sub1.
*  \param      u16_DeviceErrorCodeSW_Sub4                                                 See \ref u16_DeviceErrorCodeSW_Sub1.
*  \param      u16_DeviceErrorCodeSW_Sub5                                                 See \ref u16_DeviceErrorCodeSW_Sub1.
*  \param      u16_DeviceErrorCodeSW_Sub6                                                 See \ref u16_DeviceErrorCodeSW_Sub1.
*  \param      u16_DeviceErrorCodeSW_Sub7                                                 See \ref u16_DeviceErrorCodeSW_Sub1.
*  \param      u16_DeviceErrorCodeSW_Sub8                                                 See \ref u16_DeviceErrorCodeSW_Sub1.
*  \param      u16_DeviceErrorCodeSW_Base1                                                This is the first software base error code in an error stack. It is greater 
*                                                                                         than zero if any base error occurred. See \ref tU_LibExceptions_ErrorCodeSW  
*                                                                                         for any base error.
*  \param      u16_DeviceErrorCodeSW_Base2                                                See \ref u16_DeviceErrorCodeSW_Base1.
*  \param      u16_DeviceErrorCodeSW_Base3                                                See \ref u16_DeviceErrorCodeSW_Base1.
*  \param      u16_DeviceErrorCodeSW_Base4                                                See \ref u16_DeviceErrorCodeSW_Base1.
*  \param      u16_DeviceErrorCodeSW_Base5                                                See \ref u16_DeviceErrorCodeSW_Base1.
*  \param      u16_DeviceErrorCodeSW_Base6                                                See \ref u16_DeviceErrorCodeSW_Base1.
*  \param      u16_DeviceErrorCodeSW_Base7                                                See \ref u16_DeviceErrorCodeSW_Base1.
*  \param      u16_DeviceErrorCodeSW_Base8                                                See \ref u16_DeviceErrorCodeSW_Base1.
*  \param      u16_DeviceErrorCntSW                                                       This is the error counter of error stack. It is incremented by
*                                                                                         two if one software base and sub error occured. In case of two
*                                                                                         sub errors with one correponding base error it is incremented
*                                                                                         by four.
*  \param      u8A_DeviceErrorStringSW[COMMON_REGISTERS__MAX_CHARS__ERROR_STRING_SW]      An array which contains the software error string for correponding
*                                                                                         software base error.
*  \param      u16_DeviceState                                                            The common device state can be written to values of \ref 
*                                                                                         _E_CommonRegisters_DeviceActiveInterfaceList.
*  \param      u16_MainboardCurrentVoltageAt24000mV                                       The current mainboard voltage at 24 V reference power line.
*  \param      u16_MainboardCurrentVoltageAt5000mV                                        The current mainboard voltage with 5 V reference power line.
*  \param      u16_MainboardCurrentVoltageAt3300mV                                        The current mainboard voltage with 3.3 V reference power line.
*
*
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     _S_CommonRegisters_DeviceParams
*
*  \brief      This Structure contains the common device parameters.
*     
*  \param      u16_DeviceActiveInterface                                                  This is the common active device interface which can be set by user via device display
*                                                                                         or via modbus. See \ref _E_CommonRegisters_DeviceActiveInterfaceList for possible interfaces.
*  \param      u16_MainboardVoltageMonitoring                                             If set to zero (0) voltage monitoring of mainboard is activated. If set to one (1) voltage
*                                                                                         monitoring is disabled.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
