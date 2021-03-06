/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                lib_exceptions.h
*
*  \brief               This is the header file of \ref lib_exceptions.c
*                       It contains exceptions codes for all application functions. Driver exceptions are defined in 
*                       \ref lib_exceptions_drv.h. 
*                       
*                       The following idea is underlied for exceptions in application functions:
*
*                       1) Return type of any application command: An unsigned short value (\em {ushort u16_ErrorCodeSW_Base}) out of enum 
*                          _E_LibExceptions_Error_MODULE_NAME serving as software base error code.
*                          Application command (function) arguments: Modbus data from driver (ushort - 16 bit) and a pointer to a variable for 
*                          software sub errors (ulong - 32 bit)
*                       2) Return type of any other application function than commands: An unsigned short value (\em {ulong u32_ErrorCodeSW_Sub}) 
*                          out of enum _E_LibExceptions_Error_MODULE_NAME.
*                          Example: u32_ErrorCodeSW_Sub = 0x32326655 means that the function with error code 0x6655 failed to execute because 
*                          the function with error code 0x3232 failed. The upper 16-bit can be set to an valid error code, but do not have to.
*                       3) Return type of any driver function which is called inside an application function: 
*                          A unsigned short value which is set to into the upper word of application function return value, out of enum 
*                          _E_LibExceptionsDrv_Error_MODULE_NAME.
*
*
*  \author              Ruediger Radtke, David Jaeger, Frank Meier
*  \version             1.3
*
*  \date                2011-07-20     Initial version.
*  \date                2011-11-23     <em>File version 1.1</em>. Every device gets own space for error codes (2000 entries).
*  \date                2012-08-17     Extended structure \ref tS_LibExceptions_Exceptions.
*  \date                2012-08-21     <em>File version 1.2</em>. Changed entry name of \ref _S_LibExceptions_ErrorStringsSW.
*  \date                2012-08-21     Removed error strings for calibration data load/save of valve actuator because 
*                                      of removed commands. Changed error strings name "LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS__TSK_CMD_EXECUTION_PIN_CONTACT_STRIP"
*                                      tp \ref LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS__TSK_CMD_EXECUTION_EXTERN_CONTROL_CONTACT_HANDLER.
*  \date                2012-11-13     Added error code \ref LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__UV_FILTER_SWITCHING_ON_THRESHOLD_IN_NANOMETER.
*  \date                2012-11-26     <em>File version 1.3</em>. Added error codes by enum \ref _E_LibExceptions_Error_S1130DegasControl.
*  \date                2012-12-10     RR: Added error codes.
*  \date                2013-01-16     RR: Added error codes for column oven.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef LIB_EXCEPTIONS__ERROR__H
#define LIB_EXCEPTIONS__ERROR__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

#define  LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS               (20000)
#define  LIB_EXCEPTIONS__OFFSET_TO_SW_ERRORS                   (20000)
#define  LIB_EXCEPTIONS__ERROR_STRING_SW_CHARS_MAX             (50)
#define  LIB_EXCEPTIONS__ERROR_STRINGS_SW_STRUCT_ENTRIES_MAX   (300) //*<  increase if no space left for additional module error strings */
#define  LIB_EXCEPTIONS__ERROR_CNT_MAX_TO_SET_ENTRY            (14)  //*<  maximum error count is 16 but at least two entries must be free to set a valid error code pair */
#define  LIB_EXCEPTIONS__ERROR_CNT_MAX                         (16)
#define  LIB_EXCEPTIONS__ERROR_NONE                            (0)

// exception inquiry macros for application functions
#define  SUCCESS_APP_FUNC( Function, RetVal )                  ((RetVal = Function) == LIB_EXCEPTIONS__ERROR_NONE ? (TRUE) : (FALSE))
//#define  SUCCESS_APP_FUNC( Function, RetVal )                  ( (RetVal = Function) == ERROR_BASE__NONE ? (TRUE) : (FALSE) )
#define  FAIL_APP_FUNC( Function, RetVal )                     ((RetVal = Function) != LIB_EXCEPTIONS__ERROR_NONE ? (TRUE) : (FALSE))
//#define  FAIL_APP_FUNC( Function, RetVal )                     ( (RetVal = Function) != ERROR_BASE__NONE ? (TRUE) : (FALSE) )
#define  TRY_AND_CATCH_APP_FUNC( Function, RetVal )            ((RetVal = Function)) // application functions allready deliver an exception code as return arg
//#define  TRY_AND_CATCH_APP_FUNC( Function, RetVal )            ( (RetVal = Function) ) // application functions allready deliver error code as return arg
// exception inquiry macros for framwork functions
// be careful with macro logic, framework functions return an arg > 0 if successful in contrast to application functions which
// return an arg = 0 if successful
#define  TRY_AND_CATCH( Function, RetVal, ErrorCode )          ((RetVal = Function) > 0  ? (RetVal = LIB_EXCEPTIONS__ERROR_NONE ) : (RetVal = ErrorCode))
//#define  TRY_AND_CATCH( Function, RetVal, ErrorCode )          ( (RetVal = Function) > 0  ? (RetVal = ERROR_BASE__NONE) : (RetVal = ErrorCode) )
#define  SUCCESS( Function, RetVal, ErrorCode )                ((RetVal = Function) > 0  ? (~(RetVal = LIB_EXCEPTIONS__ERROR_NONE )) : ((RetVal = ErrorCode) & 0x00000000ul))
//#define  SUCCESS( Function, RetVal, ErrorCode )                ( (RetVal = Function) > 0  ? (~(RetVal = ERROR_BASE__NONE)) : ((RetVal = ErrorCode) & 0x00000000ul) )
#define  FAIL( Function, RetVal, ErrorCode )                   ((RetVal = Function) == 0 ? (RetVal = ErrorCode) : (RetVal = LIB_EXCEPTIONS__ERROR_NONE))
//#define  FAIL( Function, RetVal, ErrorCode )                   ( (RetVal = Function) == 0 ? (RetVal = ErrorCode) : (RetVal = ERROR_BASE__NONE) )

//---------------------------------------------- enum errors common ----------------------------------------------------------------------------

enum _E_LibExceptions_Error_CommonRegistersFuncs
{
   LIB_EXCEPTIONS__ERROR__COMMON_REGISTERS_FUNCS__INIT_REGISTER_BANK                               =  1 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_REGISTERS_FUNCS__INIT_DEVICE_PARAMS                               =  2 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_REGISTERS_FUNCS__INIT_DEVICE_STATUS_DATA                          =  3 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_REGISTERS_FUNCS__INIT_SYKAM_DEVICE_DATA                           =  4 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_REGISTERS_FUNCS__SYKAM_DEVICE_DATA_CONSISTENCY_CHECK              =  5 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_REGISTERS_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_CommonModbusFuncs
{
   LIB_EXCEPTIONS__ERROR__COMMON_MODBUS_FUNCS__INIT_TASKS                                          =  101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_MODBUS_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_CommonModbusTasks
{
   LIB_EXCEPTIONS__ERROR__COMMON_MODBUS_TASKS__CMD_EXECUTION                                       =  201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, 
   LIB_EXCEPTIONS__ERROR__COMMON_MODBUS_TASKS__RESPONSE_ON_DISPLAY_READ                            =  202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_MODBUS_TASKS_COUNTER
};

enum _E_LibExceptions_Error_CommonCmdsOwner
{
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__SYKAM_DEVICE_DATA_LOAD                                =  301 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__SYKAM_DEVICE_DATA_SAVE                                =  302 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__MODBUS_SET_SLAVE_ADDRESS                              =  303 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__DEVICE_RUN_TIME_COUNTER_RESET                         =  304 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__ERROR_CODE_SW_CLEAR_CURR                              =  305 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__ERROR_CODE_SW_CLEAR_ALL                               =  306 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__SYKAM_DEVICE_DATA_WRITE_ENABLE                        =  307 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER_COUNTER,
};

enum _E_LibExceptions_Error_CommonCmdsUser
{
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_USER_COUNTER,
};

enum _E_LibExceptions_Error_CommonCmdsReg
{
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_REG__DEVICE_ACTIVE_INTERFACE                                 =  501 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_CMDS_REG_COUNTER,
};

enum _E_LibExceptions_Error_UtilsFuncs
{
   LIB_EXCEPTIONS__ERROR__UTILS_FUNCS__SWAP_BYTES                                                  =  601 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__UTILS_FUNCS__SWAP_WORDS                                                  =  602 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__UTILS_FUNCS__CALC_MEAN                                                   =  603 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__UTILS_FUNCS__FIXED_MEM_DATA_PACKAGES_RW                                  =  604 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__UTILS_FUNCS__DUMMY                                                       =  605 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__UTILS_FUNCS__FIXED_MEM_ERASE                                             =  606 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,   
   LIB_EXCEPTIONS__ERROR__UTILS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_UtilsTasks
{
   LIB_EXCEPTIONS__ERROR__UTILS_TASKS__TSK_HEARTBEAT                                               =  701 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__UTILS_TASKS_COUNTER
};

enum _E_LibExceptions_Error_LibExceptionsFuncs
{
   LIB_EXCEPTIONS__ERROR__LIB_EXCEPTIONS_FUNCS__EXCEPTION_SET                                      =  801 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__LIB_EXCEPTIONS_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_CommonSupportFuncs
{
   LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__ERROR_STRING_SW_FIND_AND_SET                       =  901 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__ERROR_CODE_SW_CLEAR_CURR_PAIR                      =  902 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__ERROR_STRING_SW_CLEAR                              =  903 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__INIT_TASKS                                         =  904 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS__DEVICE_STATE_RW                                    =  905 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_CommonSupportTasks
{
   LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_TASKS__TSK_DEVICE_RUN_TIME_COUNTER                        =  1001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_TASKS__TSK_MAINBOARD_VOLTAGE_MONITORING                   =  1002 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COMMON_SUPPORT_TASKS_COUNTER
};

// Use this enum for special exceptions like "wrong user input". Application error corresponds 
// to most special errors [but not all]. Errors are shown to users as direct errors of called function but
// the reason why this function failed - an user application error - is not shown at moment. 
enum _E_LibExceptions_ErrorSpecial
{
   LIB_EXCEPTIONS__ERROR_SPECIAL__UNKNOWN_ERROR                                                    =  1101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__NO_SPACE_LEFT_ON_HEAP                                            =  1102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_VALVE_TYPE_NOT_SUPPORTED                               =  1103 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_DAC_RUN_MOTOR_DRIVER_VALUE_TOO_SMALL                   =  1104 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_DAC_HOLD_MOTOR_DRIVER_VALUE_TO_BIG                     =  1105 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_CALIBRATION_DATA_NOT_AVAILABLE                         =  1106 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_CALIBRATION_DATA_NOT_SAVED                             =  1107 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_DEVICE_PARAMS_NOT_AVAILABLE                            =  1108 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_DEVICE_PARAMS_NOT_SAVED                                =  1109 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__VALVE_ACT_CALIBRATION_NOT_FINISHED                               =  1110 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_SENSOR_DISCONNECTED                                     =  1111 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_SENSOR_SPI_DATA_NOT_AVAILABLE                           =  1112 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_WAVELENGTH_PROGRAM_IS_INVALID                           =  1113 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_WAVELENGTH_PROGRAM_WAS_NOT_SELECTED_TO_MODIFY           =  1114 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_SCAN_PROGRAM_IS_INVALID                                 =  1115 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_SCAN_PROGRAM_WAS_NOT_SELECTED_TO_MODIFY                 =  1116 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_PEAK_CENTER_SEARCH_WINDOW_IS_TOO_SMALL                  =  1117 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_PEAK_WIDTH_EXCEEDED                                     =  1118 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_INVALID_CALIBRATION_COEFFICIENT                         =  1119 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_TUNGSTEN_LAMP_BROKEN_OR_WEAK                            =  1120 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_DEUTERIUM_LAMP_BROKEN_OR_WEAK                           =  1121 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_CALIBRATION_FAILED_LAMP_ENERGY_TOO_LOW                  =  1122 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_CALIBRATION_FAILED_LAMP_ENERGY_TOO_HIGH                 =  1123 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_CALIBRATION_FAILED_MAX_REPETITIONS_EXCEEDED             =  1124 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_LAMP_ENERGY_LOW                                         =  1125 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_INVALID_CHANNEL                                         =  1126 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_INVALID_STATE                                           =  1127 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_INVALID_ANALOG_OUTPUT_OFFSETS                           =  1128 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_MODBUS_DATA_DISASSEMLING_FAILED                         =  1129 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__MAINBOARD_LOW_VOLTAGE                                            =  1130 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__MAINBOARD_HIGH_VOLTAGE                                           =  1131 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__ADC_INTERNAL_NOT_INITIALIZED                                     =  1132 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DEVICE_IS_NOT_READY                                              =  1133 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DEVICE_OPTION_IS_NOT_ACTIVATED                                   =  1134 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_FUNCTION_ARGUMENTS                                       =  1135 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__MAILBOX_FULL                                                     =  1136 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__MAILBOX_TIMEOUT                                                  =  1137 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__MODBUS_DATA_DISASSEMLING_FAILED                                  =  1138 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_PROGRAM_ID                                               =  1139 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_PROGRAM                                                  =  1140 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__STATUS_DATA_NOT_LOADED                                           =  1141 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__DEVICE_PARAMS_NOT_LOADED                                         =  1142 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_CONTROL_PARAMS_NOT_AVAILABLE                         =  1143 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_INTERNAL_ERROR                                       =  1144 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_TEMPERATURE_SENSOR_FAILURE                           =  1145 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_GAS_SENSOR_FAILURE                                   =  1146 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_EXTERNAL_ADC_FAILURE                                 =  1147 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_TRIGGER_FROM_TIMER_TASK_FAILED                       =  1148 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_GAS_SENSOR_PARAMS_MISSING                            =  1149 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__S435_OVEN_TEMPERATURE_SENSOR_FAILURE                              =  1150 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR_SPECIAL__S435_REACTOR_TEMPERATURE_SENSOR_FAILURE                           =  1151 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,   
   
   //
   LIB_EXCEPTIONS__ERROR_SPECIAL_COUNTER
};

//---------------------------------------------- enum errors valve actuator S6510 --------------------------------------------------------------

enum _E_LibExceptions_Error_ValveActMain
{
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN__TSK_INIT                                                 =  2001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN_COUNTER,
};

enum _E_LibExceptions_Error_ValveActRegistersFuncs
{
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_REGISTERS_FUNCS__INIT_DEVICE_PARAMS                            =  2101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_REGISTERS_FUNCS__INIT_STATUS_DATA                              =  2102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_REGISTERS_FUNCS__INIT_REGISTER_BANK                            =  2103 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_REGISTERS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_ValveActModbusFuncs
{
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_MODBUS_FUNCS__INIT_DRIVER                                      =  2201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_MODBUS_FUNCS__INIT_TASKS                                       =  2202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_MODBUS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_ValveActModbusTasks
{
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_MODBUS_TASKS__CMD_EXECUTION                                    =  2301 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_MODBUS_TASKS_COUNTER,
};

enum _E_LibExceptions_Error_ValveActCmdsOwner
{
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_LOAD                                 =  2401 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params load failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_LOAD_DEFAULT                         =  2402 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params load failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_SAVE                                 =  2403 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params save failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__STATUS_DATA_LOAD                                   =  2404 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve status data load failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__CAL_WITH_DEVICE_PARAMS                             =  2405 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve calibration failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__CAL_WITH_DEFAULT_PARAMS                            =  2406 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve calibration failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__RESET_TOTAL_SWITCH_COUNTER                         =  2407 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch counter reset failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER_COUNTER
};

enum _E_LibExceptions_Error_ValveActCmdsUser
{
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_HOME                                          =  2501 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_NEXT                                          =  2502 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_INQUIRED                                      =  2503 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_PREVIOUS                                      =  2504 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER_COUNTER
};

enum _E_LibExceptions_Error_ValveActCmdsReg
{   
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__DAC_HOLD_MOTOR_DRIVER                                =  2601 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve change DAC value failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__DAC_RUN_MOTOR_DRIVER                                 =  2602 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve change DAC value failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__CURRENT_VALVE_TYPE                                   =  2603 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG_COUNTER
};

enum _E_LibExceptions_Error_ValveActSupportFuncs
{   
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__INIT_TASKS                                      =  2701 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__POSITIONS_VALID_GET                             =  2701 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__POSITION_VALID_CHECK                            =  2702 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__POSITIONS_RW                                    =  2703 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__FIND_MAX_POS_IDX                                =  2704 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__STATUS_DATA_SET                                 =  2705 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__EXECUTE_MOVEMENT                                =  2706 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__CALIBRATION_FLAGS_RW                            =  2707 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__DEVICE_PARAM_FLAGS_RW                           =  2708 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__STATE_RW                                        =  2709 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__LOAD_CALIBRATION_DATA_AND_PARAMS                =  2710 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS_COUNTER
};


enum _E_LibExceptions_Error_ValveActSupportTasks
{   
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS__TSK_CMD_EXECUTION_EXTERN_CONTROL_CONTACT_HANDLER=  2801 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS__TSK_GENERATE_INJECT_SIGNAL                      =  2802 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS_COUNTER
};

//---------------------------------------------- enum errors uv/vis detector S3250/S3245 -------------------------------------------------------

enum _E_LibExceptions_Error_DetectorMain
{
   LIB_EXCEPTIONS__ERROR__DETECTOR_MAIN__TSK_INIT                                                  =  4001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_MAIN_COUNTER
};

enum _E_LibExceptions_Error_DetectorRegistersFuncs
{
   LIB_EXCEPTIONS__ERROR__DETECTOR_REGISTERS_FUNCS__INIT_REGISTER_BANK                             =  4101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_REGISTERS_FUNCS__INIT_DEVICE_PARAMS                             =  4102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_REGISTERS_FUNCS__INIT_STATUS_DATA                               =  4103 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_REGISTERS_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_DetectorModbusFuncs
{
   LIB_EXCEPTIONS__ERROR__DETECTOR_MODBUS_FUNCS__INIT_DRIVER                                       =  4201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_MODBUS_FUNCS__INIT_TASKS                                        =  4202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_MODBUS_FUNCS__FILTER_FOR_MODBUS_REQUESTS                        =  4203 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_MODBUS_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_DetectorModbusTasks
{
   LIB_EXCEPTIONS__ERROR__DETECTOR_MODBUS_TASKS__TSK_CMD_EXECUTION                                 =  4301 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_MODBUS_TASKS__TSK_ABSORPTION_DATA_COUNT_RESET                   =  4302 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_MODBUS_TASKS_COUNTER
};

enum _E_LibExceptions_Error_DetectorCmdsOwner
{
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER__REF_SIG_DATA_TRANSMISSION_ON_OFF                    =  4401 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER__DEUTERIUM_LAMP_STATUS_DATA_RESET                    =  4402 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER__DEVICE_PARAMS_LOAD                                  =  4403 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER__SCAN_PROGRAM_START                                  =  4404 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER__ANALOG_OUTPUT_TESTMODE_ON_OFF                       =  4405 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER_COUNTER
};

enum _E_LibExceptions_Error_DetectorCmdsUser
{
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__AUTOZERO                                             =  4501 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__CALIBRATION_START                                    =  4502 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__ABSORPTION_DATA_TRANSMISSION_ON_OFF                  =  4503 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__MULTI_CHANNEL_ON_OFF                                 =  4504 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__PEAK_DETECTION_ON_OFF                                =  4505 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__DETECTOR_LAMPS_ON_OFF                                =  4506 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__WAVELENGTH_PROGRAM_START                             =  4507 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__WAVELENGTH_PROGRAM_STOP                              =  4508 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__WAVELENGTH_PROGRAM_DELETE                            =  4509 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__WAVELENGTH_PROGRAM_SAVE                              =  4510 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__WAVELENGTH_PROGRAM_SELECT_TO_MODIFY                  =  4511 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_PROGRAM_START                                   =  4512 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_PROGRAM_STOP                                    =  4513 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_PROGRAM_DELETE                                  =  4514 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_PROGRAM_SAVE                                    =  4515 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_PROGRAM_SELECT_TO_MODIFY                        =  4516 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_DATA_REPLAY                                     =  4517 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER_COUNTER
};

enum _E_LibExceptions_Error_DetectorCmdsReg
{
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_RISE_TIME_IN_MILLI_SECONDS                  =  4601 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_RISE_TIME_IN_MILLI_SECONDS                  =  4602 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_ANALOG_OUTPUT_RANGE_IN_MILLI_AU             =  4603 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_ANALOG_OUTPUT_RANGE_IN_MILLI_AU             =  4604 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_ANALOG_OUTPUT_OFFSET_IN_PERCENT             =  4605 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_ANALOG_OUTPUT_OFFSET_IN_PERCENT             =  4606 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_WAVELENGTH                                  =  4607 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_WAVELENGTH                                  =  4608 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__PEAK_DETECTOR_THRESHOLD_START_IN_MILLI_AU             =  4609 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__PEAK_DETECTOR_NOTIFICATION_DELAY_IN_SECONDS           =  4610 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__DETECTOR_DATA_RATE_IN_HZ                              =  4611 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__EXTERN_CONTROL_CONTACT_CONFIG_PIN_10                  =  4612 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_ANALOG_OUTPUT_OFFSET_BOTTOM                 =  4613 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_ANALOG_OUTPUT_OFFSET_BOTTOM                 =  4614 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_ANALOG_OUTPUT_OFFSET_TOP                    =  4615 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_ANALOG_OUTPUT_OFFSET_TOP                    =  4616 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__INTEGRATION_TIME                                      =  4617 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__DISPLAY_PROGRAM_KEY_FUNCTION                          =  4618 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__UV_FILTER_SWITCHING_ON_THRESHOLD_IN_NANOMETER         =  4619 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG_COUNTER
};

enum _E_LibExceptions_Error_DetectorSupportFuncs
{
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__INIT_TASKS                                       =  4701 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__INIT_TIMER_COUNTER_FOR_ADC_INTEGRATION_TIME      =  4702 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__CALC_TIMER_COUNTER_CMP_VAL_FROM_ADC_INT_TIME     =  4703 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__DETERMINE_PEAK_CENTER                            =  4704 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__CALC_MOTOR_STEPS_FROM_WAVELENGTH                 =  4705 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__CALC_WAVELENGTH_FROM_MOTOR_STEPS                 =  4706 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__DETERMINE_DARK_CURRENT                           =  4707 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__TEST_TUNGSTEN_AND_DEUTERIUM_LAMP                 =  4708 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__TEST_TUNGSTEN_LAMP                               =  4709 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__TEST_DEUTERIUM_LAMP                              =  4710 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__REFERENCE_AND_SIGNAL_CHECK                       =  4711 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__REFERENCE_AND_SIGNAL_RW                          =  4712 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__CALC_MOTOR_STEPS_BREAK_AND_ACC                   =  4713 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__CALC_ABSORPTION                                  =  4714 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__DETERMINE_DEUTERIUM_LAMP_ENERGY                  =  4715 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__CALC_AND_SET_ABSORPTION_SMOOTHING_FACTOR         =  4716 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__DEVICE_PARAMS_AND_STATUS_DATA_RW                 =  4717 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__GO_TO_INQUIRED_WAVELENGTH                        =  4718 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__TEST_AND_CORRECT_EXTERN_DEVICE_PARAMS            =  4719 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__TEST_AND_CORRECT_INTERN_DEVICE_PARAMS            =  4720 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__SCAN_PROGRAM_LOAD                                =  4721 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__CHANGE_DEVICE_OPERATION_MODE                     =  4722 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__WAVELENGTH_PROGRAM                               =  4723 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__WAVELENGTH_PROGRAM_LOAD                          =  4724 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__STATUS_DATA_LOAD                                 =  4725 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS__ISR_SENSOR_DATA_NOTIFY                           =  4726 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_DetectorSupportTasks
{
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_TASKS__TSK_EXTERN_CONTROL_CONTACT_HANDLER               =  4801 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_TASKS__TSK_DETECTOR_SIGNAL_PROCESSING                   =  4802 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_TASKS__TSK_STEP_MOTOR_CONTROL                           =  4803 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_TASKS__TSK_ANALOG_OUTPUT                                =  4804 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_TASKS__TSK_VALVE_OUT_SWITCH_ON                          =  4805 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_TASKS__TSK_VALVE_OUT_SWITCH_OFF                         =  4806 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_SUPPORT_TASKS_COUNTER
};

enum _E_LibExceptions_Error_DetectorDataOutputTasks
{
   LIB_EXCEPTIONS__ERROR__DETECTOR_DATA_OUTPUT_TASKS__INIT_TASKS                                   =  4901 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_DATA_OUTPUT_TASKS__TSK_DATA_OUT                                 =  4902 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_DATA_OUTPUT_TASKS__TSK_DATA_OUT_TRIGGER                         =  4903 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_DATA_OUTPUT_TASKS__TSK_DATA_OUT_TRIGGER_CHECK                   =  4904 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__DETECTOR_DATA_OUTPUT_TASKS_COUNTER
};

//---------------------------------------------- enum errors column oven S4121 -----------------------------------------------------------------

enum _E_LibExceptions_Error_ColumnOvenMain
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT                                               =  5001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN_COUNTER
};

enum _E_LibExceptions_Error_ColumnOvenRegistersFuncs
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_REGISTERS_FUNCS__INIT_REGISTER_BANK                          =  5101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_REGISTERS_FUNCS__INIT_DEVICE_PARAMS                          =  5102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_REGISTERS_FUNCS__INIT_STATUS_DATA                            =  5103 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_REGISTERS_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_ColumnOvenModbusFuncs
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MODBUS_FUNCS__INIT_DRIVER                                    =  5201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MODBUS_FUNCS__INIT_TASKS                                     =  5202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MODBUS_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_ColumnOvenModbusTasks
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MODBUS_TASKS__CMD_EXECUTION                                  =  5301 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MODBUS_TASKS_COUNTER
};

enum _E_LibExceptions_Error_ColumnOvenCmdsOwner
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__CONTROL_PARAMS_SAVE                              =  5401 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD                               =  5402 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__STATUS_DATA_LOAD                                 =  5403 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__HEAT_COOL_PROGRAMS_RESET                         =  5404 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER_COUNTER
};

enum _E_LibExceptions_Error_ColumnOvenCmdsUser
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_START                           =  5501 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_STOP                            =  5502 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_DELETE                          =  5503 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_SAVE                            =  5504 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_SELECT_TO_MODIFY                =  5505 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_CALIBRATION_START                       =  5506 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER_COUNTER
};

enum _E_LibExceptions_Error_ColumnOvenCmdsReg
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__CONTROL_SECTIONS_PARAMS                            =  5601 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__GAS_SENSOR_ALARM_THRESHOLD_PPM                     =  5602 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__GAS_SENSOR_CALIBRATION_RESISTANCE                  =  5603 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__GAS_SENSOR_ALARM_ENABLED                           =  5604 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__TEMPERATURE_OFFSET                                 =  5605 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__TEMPERATURE_UNIT                                   =  5606 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND           =  5607 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG_COUNTER
};

enum _E_LibExceptions_Error_ColumnOvenSupportFuncs
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__INIT_TASKS                                    =  5701 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__HEAT_COOL_PROGRAM_LOAD                        =  5702 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__DEVICE_PARAMS_INTERN_CHECK                    =  5703 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__SET_DAC_OUTPUT                                =  5704 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__CALC_HEAT_COOL_PROGRAM_TIMES                  =  5705 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__STORE_GAS_SENSOR_DEFAULT_VALUES               =  5706 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_ColumnOvenSupportTasks
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__EXTERN_CONTROL_CONTACT_HANDLER                =  5801 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GAS_SENSING                               =  5802 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS__TSK_GLOBAL_TIMER                              =  5803 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_TASKS_COUNTER
};

enum _E_LibExceptions_Error_ColumnOvenTemperatureControlFuncs
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__INIT_TASKS                          =  5901 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__GET_TEMPERATURE                     =  5902 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__CALC_CONTROL_PARAMS                 =  5903 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__GET_CONTROL_OUTPUT                  =  5904 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_ColumnOvenTemperatureControlTasks
{
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_TASKS__TEMPERATURE_CONTROL                 =  6001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_TASKS_COUNTER
};


//---------------------------------------------- enum errors pump S1010 ------------------------------------------------------------------------

enum _E_LibExceptions_Error_S1010Main
{
   LIB_EXCEPTIONS__ERROR__S1010_MAIN__TSK_INIT                                                     =  7001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010_MAIN_COUNTER,
};

enum _E_LibExceptions_Error_S1010RegistersFuncs
{
   LIB_EXCEPTIONS__ERROR__S1010_REGISTERS_FUNCS__INIT_DEVICE_PARAMS                                =  7101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010_REGISTERS_FUNCS__INIT_STATUS_DATA                                  =  7102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010_REGISTERS_FUNCS__INIT_REGISTER_BANK                                =  7103 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010_REGISTERS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_S1010ModbusFuncs
{
   LIB_EXCEPTIONS__ERROR__S1010_MODBUS_FUNCS__INIT_DRIVER                                          =  7201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010_MODBUS_FUNCS__INIT_TASKS                                           =  7202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010_MODBUS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_S1010ModbusTasks
{
   LIB_EXCEPTIONS__ERROR__S1010_MODBUS_TASKS__CMD_EXECUTION                                        =  7301 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010_MODBUS_TASKS_COUNTER,
};

enum _E_LibExceptions_Error_S1010CmdsOwner
{
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_OWNER__DEVICE_PARAMS_LOAD_DEFAULT                             =  7401 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params load failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_OWNER__DEVICE_PARAMS_SAVE                                     =  7402 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params load failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_OWNER__DEVICE_PARAMS_LOAD                                     =  7403 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params save failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_OWNER__RESET_RUNTIME_COUNTER                                  =  7404 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve status data load failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_OWNER_COUNTER
};

enum _E_LibExceptions_Error_S1010CmdsUser
{
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_USER__PUMP_START                                              =  7501 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_USER__PUMP_STOP                                               =  7502 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_USER__PURGE_START                                             =  7503 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_USER__PURGE_STOP                                              =  7504 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_USER_COUNTER
};

enum _E_LibExceptions_Error_S1010CmdsReg
{   
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__PUMP_FLOW                                                =  7601 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve change DAC value failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__MIN_PRESSURE                                             =  7602 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve change DAC value failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__MAX_PRESSURE                                             =  7603 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__MAX_PRESSURE_PURGE                                       =  7604 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__UNIT                                                     =  7605 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__COMMPRESSIBILITY                                         =  7606 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__RUNTIME                                                  =  7607 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__DELAY_TIME                                               =  7608 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__START_RAMP                                               =  7609 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__STOP_RAMP                                                =  7610 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__NETWORK_ADRESS                                           =  7611 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__HEAD_INSTALLED                                           =  7612 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__HEAD_CONSTANT                                            =  7613 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__PRESSURE_COMPENSATION                                    =  7614 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__PRESSURE_ZERO                                            =  7615 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__USTEPS_ACC                                               =  7616 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__USTEPS_BREAK                                             =  7617 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__USTEPS_START                                             =  7618 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__DAC_HOLD                                                 =  7619 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG__DAC_RUN                                                  =  7620 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1010_CMDS_REG_COUNTER
};

enum _E_LibExceptions_Error_S1010SupportFuncs
{   
   LIB_EXCEPTIONS__ERROR__S1010_SUPPORT_FUNCS__Load_DAC_Values                                     =  7701 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010_SUPPORT_FUNCS__LOAD_CALIBRATION_DATA_AND_PARAMS                    =  7702 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010_SUPPORT_FUNCS_COUNTER
};


enum _E_LibExceptions_Error_S1010SupportTasks
{   
   LIB_EXCEPTIONS__ERROR__S1010_SUPPORT_TASKS__NOTIFY_MCP2317                                      =  7801 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010_SUPPORT_TASKS_COUNTER
};

enum _E_LibExceptions_Error_S1010PressureControlTasks
{
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PRESSURE_CONTROL                                             =  7901 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PRESSURE_CONTROL_PRESSURE_MAX                                =  7902 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PRESSURE_CONTROL_PRESSURE_MIN                                =  7903 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PRESSURE_CONTROL_PRESSURE_MAX_PURGE                          =  7904 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__EXCEPTIONS_TASK_PRESSURE_CONTROL_COUNTER
};

enum _E_LibExceptions_Error_S1010ControlTasks
{
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PUMP_CONTROL                                                 =  8001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PUMP_CONTROL_START                                           =  8002 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PUMP_CONTROL_DELAY_TIME                                      =  8003 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PUMP_CONTROL_START_RAMP                                      =  8004 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PUMP_CONTROL_STOP_RAMP                                       =  8005 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PUMP_CONTROL_RUN                                             =  8006 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PUMP_CONTROL_STOP                                            =  8007 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PUMP_CONTROL_PRESSURE_MIN_MAX                                =  8008 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PUMP_CONTROL_PRESSURE_MAX                                    =  8009 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
};

enum _E_LibExceptions_Error_S1010RuntimeTasks
{
   LIB_EXCEPTIONS__ERROR__S1010__TASK_PUMP_RUNTIME                                                        =  8010 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1010__LIB_EXCEPTIONS_TASK_RUNTIME_CONTROL_COUNTER
};


//---------------------------------------------- enum errors pump S1420 ------------------------------------------------------------------------

enum _E_LibExceptions_Error_S1420Main
{
   LIB_EXCEPTIONS__ERROR__S1420_MAIN__TSK_INIT                                                     =  7001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420_MAIN_COUNTER,
};

enum _E_LibExceptions_Error_S1420RegistersFuncs
{
   LIB_EXCEPTIONS__ERROR__S1420_REGISTERS_FUNCS__INIT_DEVICE_PARAMS                                =  7101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420_REGISTERS_FUNCS__INIT_STATUS_DATA                                  =  7102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420_REGISTERS_FUNCS__INIT_REGISTER_BANK                                =  7103 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420_REGISTERS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_S1420ModbusFuncs
{
   LIB_EXCEPTIONS__ERROR__S1420_MODBUS_FUNCS__INIT_DRIVER                                          =  7201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420_MODBUS_FUNCS__INIT_TASKS                                           =  7202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420_MODBUS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_S1420ModbusTasks
{
   LIB_EXCEPTIONS__ERROR__S1420_MODBUS_TASKS__CMD_EXECUTION                                        =  7301 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420_MODBUS_TASKS_COUNTER,
};

enum _E_LibExceptions_Error_S1420CmdsOwner
{
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_OWNER__DEVICE_PARAMS_LOAD_DEFAULT                             =  7401 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params load failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_OWNER__DEVICE_PARAMS_SAVE                                     =  7402 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params load failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_OWNER__DEVICE_PARAMS_LOAD                                     =  7403 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params save failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_OWNER__RESET_RUNTIME_COUNTER                                  =  7404 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve status data load failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_OWNER_COUNTER
};

enum _E_LibExceptions_Error_S1420CmdsUser
{
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_USER__PUMP_START                                              =  7501 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_USER__PUMP_STOP                                               =  7502 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_USER__PURGE_START                                             =  7503 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_USER__PURGE_STOP                                              =  7504 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_USER_COUNTER
};

enum _E_LibExceptions_Error_S1420CmdsReg
{   
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__PUMP_FLOW                                                =  7601 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve change DAC value failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__MIN_PRESSURE                                             =  7602 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve change DAC value failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__MAX_PRESSURE                                             =  7603 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__MAX_PRESSURE_PURGE                                       =  7604 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__UNIT                                                     =  7605 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__COMMPRESSIBILITY                                         =  7606 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__RUNTIME                                                  =  7607 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__DELAY_TIME                                               =  7608 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__START_RAMP                                               =  7609 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__STOP_RAMP                                                =  7610 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__NETWORK_ADRESS                                           =  7611 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__HEAD_INSTALLED                                           =  7612 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__HEAD_CONSTANT                                            =  7613 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__PRESSURE_COMPENSATION                                    =  7614 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__PRESSURE_ZERO                                            =  7615 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__USTEPS_ACC                                               =  7616 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__USTEPS_BREAK                                             =  7617 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__USTEPS_START                                             =  7618 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__DAC_HOLD                                                 =  7619 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG__DAC_RUN                                                  =  7620 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1420_CMDS_REG_COUNTER
};

enum _E_LibExceptions_Error_S1420SupportFuncs
{   
   LIB_EXCEPTIONS__ERROR__S1420_SUPPORT_FUNCS__Load_DAC_Values                                     =  7701 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420_SUPPORT_FUNCS__LOAD_CALIBRATION_DATA_AND_PARAMS                    =  7702 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420_SUPPORT_FUNCS_COUNTER
};


enum _E_LibExceptions_Error_S1420SupportTasks
{   
   LIB_EXCEPTIONS__ERROR__S1420_SUPPORT_TASKS__NOTIFY_MCP2317                                      =  7801 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420_SUPPORT_TASKS_COUNTER
};

enum _E_LibExceptions_Error_S1420PressureControlTasks
{
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PRESSURE_CONTROL                                             =  7901 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PRESSURE_CONTROL_PRESSURE_MAX                                =  7902 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PRESSURE_CONTROL_PRESSURE_MIN                                =  7903 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PRESSURE_CONTROL_PRESSURE_MAX_PURGE                          =  7904 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__EXCEPTIONS_TASK_PRESSURE_CONTROL_COUNTER
};

enum _E_LibExceptions_Error_S1420ControlTasks
{
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL                                                 =  8001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_START                                           =  8002 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_DELAY_TIME                                      =  8003 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_START_RAMP                                      =  8004 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_STOP_RAMP                                       =  8005 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_RUN                                             =  8006 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_STOP                                            =  8007 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_PRESSURE_MIN_MAX                                =  8008 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_PRESSURE_MAX                                    =  8009 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__LIB_EXCEPTIONS_TASK_PUMP_CONTROL_COUNTER
};

enum _E_LibExceptions_Error_S1420RuntimeTasks
{
   LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_RUNTIME                                                        =  8010 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1420__LIB_EXCEPTIONS_TASK_RUNTIME_CONTROL_COUNTER
};

//---------------------------------------------- enum errors pump S1430 ------------------------------------------------------------------------

enum _E_LibExceptions_Error_S1430Main
{
   LIB_EXCEPTIONS__ERROR__S1430_MAIN__TSK_INIT                                                     =  8011 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430_MAIN_COUNTER,
};

enum _E_LibExceptions_Error_S1430RegistersFuncs
{
   LIB_EXCEPTIONS__ERROR__S1430_REGISTERS_FUNCS__INIT_DEVICE_PARAMS                                =  8101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430_REGISTERS_FUNCS__INIT_STATUS_DATA                                  =  8102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430_REGISTERS_FUNCS__INIT_REGISTER_BANK                                =  8103 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430_REGISTERS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_S1430ModbusFuncs
{
   LIB_EXCEPTIONS__ERROR__S1430_MODBUS_FUNCS__INIT_DRIVER                                          =  8201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430_MODBUS_FUNCS__INIT_TASKS                                           =  8202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430_MODBUS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_S1430ModbusTasks
{
   LIB_EXCEPTIONS__ERROR__S1430_MODBUS_TASKS__CMD_EXECUTION                                        =  8301 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430_MODBUS_TASKS_COUNTER,
};

enum _E_LibExceptions_Error_S1430CmdsOwner
{
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_OWNER__DEVICE_PARAMS_LOAD_DEFAULT                             =  8401 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params load failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_OWNER__DEVICE_PARAMS_SAVE                                     =  8402 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params load failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_OWNER__DEVICE_PARAMS_LOAD                                     =  8403 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params save failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_OWNER__RESET_RUNTIME_COUNTER                                  =  8404 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve status data load failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_OWNER_COUNTER
};

enum _E_LibExceptions_Error_S1430CmdsUser
{
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_USER__PUMP_START                                              =  8501 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_USER__PUMP_STOP                                               =  8502 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_USER__PURGE_START                                             =  8503 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_USER__PURGE_STOP                                              =  8504 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve switch failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_USER_COUNTER
};

enum _E_LibExceptions_Error_S1430CmdsReg
{   
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__PUMP_FLOW                                                =  8601 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve change DAC value failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__MIN_PRESSURE                                             =  8602 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve change DAC value failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__MAX_PRESSURE                                             =  8603 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__MAX_PRESSURE_PURGE                                       =  8604 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__UNIT                                                     =  8605 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__COMMPRESSIBILITY                                         =  8606 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__RUNTIME                                                  =  8607 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__DELAY_TIME                                               =  8608 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__START_RAMP                                               =  8609 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__STOP_RAMP                                                =  8610 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__NETWORK_ADRESS                                           =  8611 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__HEAD_INSTALLED                                           =  8612 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__HEAD_CONSTANT                                            =  8613 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__PRESSURE_COMPENSATION                                    =  8614 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__PRESSURE_ZERO                                            =  8615 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__USTEPS_ACC                                               =  8616 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__USTEPS_BREAK                                             =  8617 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__USTEPS_START                                             =  8618 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__DAC_HOLD                                                 =  8619 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG__DAC_RUN                                                  =  8620 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1430_CMDS_REG_COUNTER
};

enum _E_LibExceptions_Error_S1430SupportFuncs
{   
   LIB_EXCEPTIONS__ERROR__S1430_SUPPORT_FUNCS__Load_DAC_Values                                     =  8701 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430_SUPPORT_FUNCS__LOAD_CALIBRATION_DATA_AND_PARAMS                    =  8702 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430_SUPPORT_FUNCS_COUNTER
};


enum _E_LibExceptions_Error_S1430SupportTasks
{   
   LIB_EXCEPTIONS__ERROR__S1430_SUPPORT_TASKS__NOTIFY_MCP2317                                      =  8801 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430_SUPPORT_TASKS_COUNTER
};

enum _E_LibExceptions_Error_S1430PressureControlTasks
{
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PRESSURE_CONTROL                                             =  8901 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PRESSURE_CONTROL_PRESSURE_MAX                                =  8902 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PRESSURE_CONTROL_PRESSURE_MIN                                =  8903 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PRESSURE_CONTROL_PRESSURE_MAX_PURGE                          =  8904 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__EXCEPTIONS_TASK_PRESSURE_CONTROL_COUNTER
};

enum _E_LibExceptions_Error_S1430ControlTasks
{
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PUMP_CONTROL                                                 =  9001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PUMP_CONTROL_START                                           =  9002 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PUMP_CONTROL_DELAY_TIME                                      =  9003 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PUMP_CONTROL_START_RAMP                                      =  9004 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PUMP_CONTROL_STOP_RAMP                                       =  9005 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PUMP_CONTROL_RUN                                             =  9006 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PUMP_CONTROL_STOP                                            =  9007 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PUMP_CONTROL_PRESSURE_MIN_MAX                                =  9008 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PUMP_CONTROL_PRESSURE_MAX                                    =  9009 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   
};

enum _E_LibExceptions_Error_S1430RuntimeTasks
{
   LIB_EXCEPTIONS__ERROR__S1430__TASK_PUMP_RUNTIME                                                        =  9101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1430__LIB_EXCEPTIONS_TASK_RUNTIME_CONTROL_COUNTER
};


//---------------------------------------------- enums errors pump S1130/S1132/S1134 -----------------------------------------------------------

enum _E_LibExceptions_Error_S1130Main
{
   LIB_EXCEPTIONS__ERROR__S1130_MAIN__TSK_INIT                                                     =  10001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_MAIN_COUNTER,
};

enum _E_LibExceptions_Error_S1130RegistersFuncs
{
   LIB_EXCEPTIONS__ERROR__S1130_REGISTERS_FUNCS__INIT_DEVICE_PARAMS                                =  10101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_REGISTERS_FUNCS__INIT_STATUS_DATA                                  =  10102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_REGISTERS_FUNCS__INIT_REGISTER_BANK                                =  10103 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_REGISTERS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_S1130ModbusFuncs
{
   LIB_EXCEPTIONS__ERROR__S1130_MODBUS_FUNCS__INIT_DRIVER                                          =  10201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_MODBUS_FUNCS__INIT_TASKS                                           =  10202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_MODBUS_FUNCS_COUNTER,
};

enum _E_LibExceptions_Error_S1130ModbusTasks
{
   LIB_EXCEPTIONS__ERROR__S1130_MODBUS_TASKS__CMD_EXECUTION                                        =  10301 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_MODBUS_TASKS_COUNTER,
};

enum _E_LibExceptions_Error_S1130CmdsOwner
{
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__DEVICE_PARAMS_LOAD_DEFAULT                             =  10401 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params load failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__DEVICE_PARAMS_SAVE                                     =  10402 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params load failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__DEVICE_PARAMS_LOAD                                     =  10403 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve params save failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__RESET_RUNTIME_COUNTER                                  =  10404 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve status data load failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__TIME_PROG_DEFAULT                                      =  10405 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve status data load failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__TIME_PROG_SAVE                                         =  10406 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve status data load failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__TIME_PROG_LOAD                                         =  10407 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve status data load failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER_COUNTER
};

enum _E_LibExceptions_Error_S1130CmdsUser
{
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER__PUMP_START                                              =  10501 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"start pump failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER__PUMP_STOP                                               =  10502 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"stop pump failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER__PURGE_START                                             =  10503 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"start purge failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER__PURGE_STOP                                              =  10504 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"stop purge failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER__VALVE_SET_NEW_VALUES                                    =  10505 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"set new percent values failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER_COUNTER
};

enum _E_LibExceptions_Error_S1130CmdsReg
{   
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__PUMP_FLOW                                                =  10601 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve change DAC value failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__MIN_PRESSURE                                             =  10602 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve change DAC value failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__MAX_PRESSURE                                             =  10603 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__MAX_PRESSURE_PURGE                                       =  10604 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__UNIT                                                     =  10605 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__COMMPRESSIBILITY                                         =  10606 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__RUNTIME                                                  =  10607 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__DELAY_TIME                                               =  10608 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__START_RAMP                                               =  10609 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__STOP_RAMP                                                =  10610 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__NETWORK_ADRESS                                           =  10611 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__HEAD_INSTALLED                                           =  10612 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__HEAD_CONSTANT                                            =  10613 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__PRESSURE_COMPENSATION                                    =  10614 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__PRESSURE_ZERO                                            =  10615 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__USTEPS_ACC                                               =  10616 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__USTEPS_BREAK                                             =  10617 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__USTEPS_START                                             =  10618 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__DAC_HOLD                                                 =  10619 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__DAC_RUN                                                  =  10620 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS, //"valve current type change failed"
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__VALVE_A_CORR                                             =  10621 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__VALVE_B_CORR                                             =  10622 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__VALVE_C_CORR                                             =  10623 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__VALVE_D_CORR                                             =  10624 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__VALVE_PURGE                                              =  10625 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__DEGAS_ON_OFF                                             =  10626 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__DEGAS_MODE                                               =  10627 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__DEGAS_SPEED                                              =  10628 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__PROG_RUN                                                 =  10629 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG__PROG_EDIT                                                =  10630 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_CMDS_REG_COUNTER
};

enum _E_LibExceptions_Error_S1130SupportFuncs
{   
   LIB_EXCEPTIONS__ERROR__S1130_SUPPORT_FUNCS__Load_DAC_Values                                     =  10701 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_SUPPORT_FUNCS__LOAD_CALIBRATION_DATA_AND_PARAMS                    =  10702 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_SUPPORT_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_S1130SupportTasks
{   
   LIB_EXCEPTIONS__ERROR__S1130_SUPPORT_TASKS__NOTIFY_MCP2317                                      =  10801 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_SUPPORT_TASKS_COUNTER
};

enum _E_LibExceptions_Error_S1130PressureControl
{
   LIB_EXCEPTIONS__ERROR__S1130_PRESSURE_CONTROL                                                   =  10901 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PRESSURE_CONTROL_PRESSURE_MAX                                      =  10902 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PRESSURE_CONTROL_PRESSURE_MIN                                      =  10903 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PRESSURE_CONTROL_PRESSURE_PURGE_MAX                                =  10904 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PRESSURE_CONTROL_COUNTER
};

enum _E_LibExceptions_Error_S1130DegasControl
{
   LIB_EXCEPTIONS__ERROR__S1130_DEGAS_CONTROL                                                      =  10951 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_DEGAS_CONTROL_REACH_VACUUM                                         =  10952 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_DEGAS_CONTROL_COUNTER
};

enum _E_LibExceptions_Error_S1130Control
{
   LIB_EXCEPTIONS__ERROR__S1130_PUMP_CONTROL                                                       =  11001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PUMP_CONTROL_START                                                 =  11002 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PUMP_CONTROL_DELAY_TIME                                            =  11003 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PUMP_CONTROL_START_RAMP                                            =  11004 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PUMP_CONTROL_STOP_RAMP                                             =  11005 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PUMP_CONTROL_RUN                                                   =  11006 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PUMP_CONTROL_STOP                                                  =  11007 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PUMP_CONTROL_PRESSURE_MIN_MAX                                      =  11008 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_PUMP_CONTROL_COUNTER
};

enum _E_LibExceptions_Error_S1130Runtime
{
   LIB_EXCEPTIONS__ERROR__S1130_PUMP_RUNTIME                                                       =  11101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_RUNTIME_CONTROL_COUNTER
};

enum _E_LibExceptions_Error_S1130ValveControl
{
   LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG                                                          =  11201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_NO_TIME_PROG_SAVED                                       =  11202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_HOLD_FLOW                                                =  11203 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_HOLD_VALVE                                               =  11204 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_FLOW_ENTRY_WRONG                                         =  11205 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_TIME_ENTRY_WRONG                                         =  11206 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_VALVE_ENTRY_WRONG                                        =  11207 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_COUNTER
};

//---------------------------------------------- enum errros autosampler S5300 -----------------------------------------------------------------

enum _E_LibExceptions_Error_AutosamplerRegistersFuncs
{
   LIB_EXCEPTIONS__ERROR__S5151_REGISTERS_FUNCS__INIT_DEVICE_PARAMS                                =  12001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_REGISTERS_FUNCS__INIT_STATUS_DATA                                  =  12002 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
};

enum _E_LibExceptions_Error_AutosamplerModbusTasks
{
   LIB_EXCEPTIONS__ERROR__S5151_MODBUS_TASKS__HREG_FRAM_WRITE                                      =  12101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_MODBUS_TASKS__HREG_FRAM_READ                                       =  12102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_MODBUS_TASKS__CMD_EXECUTION                                        =  12103 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
};


enum _E_LibExceptions_Error_AutosamplerCmdsOwner
{
   LIB_EXCEPTIONS__ERROR__S5151_CMDS_OWNER__RESET_IVALVE                                           =  12201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_CMDS_OWNER__RESET_SVALVE                                           =  12202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_CMDS_OWNER__RESET_ALL                                              =  12203 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,   
};

enum _E_LibExceptions_Error_AutosamplerCmdsUser
{
   LIB_EXCEPTIONS__ERROR__S5151_CMDS_USER__START_PROGRAM                                           =  12301 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_CMDS_USER__DO_WASH                                                 =  12302 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_CMDS_USER__LOAD_PROGRAM                                            =  12303 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
};

enum _E_LibExceptions_Error_AutosamplerGeneral
{
   LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_X                                                       =  12401 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_Y                                                       =  12402 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_S                                                       =  12403 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_NI                                                      =  12404 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_NA                                                      =  12405 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_VI                                                      =  12406 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_VS                                                      =  12407 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
};


//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================
//---------------------------------------------- enum errors S435-----------------------------------------------------------------

enum _E_LibExceptions_Error_S435Main
{
   LIB_EXCEPTIONS__ERROR__S435_MAIN_TSK_INIT                                               =  13001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_MAIN_COUNTER
};


enum _E_LibExceptions_Error_S435General
{
   LIB_EXCEPTIONS__ERROR__S435_MAIN__TSK_INIT                                                =  13501 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_MODBUS_FUNCS_INIT_DRIVER                                      =  13502 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_MODBUS_FUNCS_INIT_TASKS                                       =  13503 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_REGISTERS_FUNCS_STATUS_DATA                                   =  13504 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,  
};								  

enum _E_LibExceptions_Error_S435RegistersFuncs
{
   LIB_EXCEPTIONS__ERROR__S435_REGISTERS_FUNCS_INIT_REGISTER_BANK                          =  13101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_REGISTERS_FUNCS_INIT_DEVICE_PARAMS                          =  13102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_REGISTERS_FUNCS_INIT_STATUS_DATA                            =  13103 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_REGISTERS_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_S435ModbusFuncs
{
   LIB_EXCEPTIONS__ERROR__S435_MODBUS_FUNCS_DRIVER                                    =  13201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_MODBUS_FUNCS_TASKS                                     =  13202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_MODBUS_FUNCS_COUNTER
};

enum _E_LibExceptions_Error_S435ModbusTasks
{
   LIB_EXCEPTIONS__ERROR__S435_MODBUS_TASKS_CMD_EXECUTION                                  =  13301 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_MODBUS_TASKS__HREG_FRAM_WRITE						       =  13302 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_MODBUS_TASKS_COUNTER
};

enum _E_LibExceptions_Error_S435CmdsOwner
{   
   LIB_EXCEPTIONS__ERROR__S435_CMDS_OWNER__DEVICE_PARAMS_LOAD                               =  13401 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_OWNER__STATUS_DATA_LOAD                                 =  13402 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_OWNER__COUNTER
};

enum _E_LibExceptions_Error_S435CmdsUser
{
   LIB_EXCEPTIONS__ERROR__S435_CMDS_USER__HEAT_COOL_PROGRAM_START                           =  13501 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_USER__HEAT_COOL_PROGRAM_STOP                            =  13502 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_USER__HEAT_COOL_PROGRAM_DELETE                          =  13503 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_USER__HEAT_COOL_PROGRAM_SAVE                            =  13504 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_USER__HEAT_COOL_PROGRAM_SELECT_TO_MODIFY                =  13505 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_USER__HEAT_COOL_CALIBRATION_START                       =  13506 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_USER__COUNTER
};

enum _E_LibExceptions_Error_S435CmdsReg
{
   LIB_EXCEPTIONS__ERROR__S435_CMDS_REG__CONTROL_SECTIONS_PARAMS                            =  13601 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_REG__TEMPERATURE_OFFSET                                 =  13602 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_REG__TEMPERATURE_UNIT                                   =  13603 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CMDS_REG__COUNTER
};

enum _E_LibExceptions_Error_S435SupportTasks
{
   LIB_EXCEPTIONS__ERROR__S435_SUPPORT_TASKS_EXTERN_CONTROL_CONTACT_HANDLER                =  13801 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_SUPPORT_FUNCS__SET_DAC_OUTPUT                               =  13802 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_SUPPORT_TASKS_TSK_GLOBAL_TIMER                              =  13803 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_SUPPORT_TASKS_COUNTER
};

enum _E_LibExceptions_Error_S435TemperatureControlFuncs
{
   LIB_EXCEPTIONS__ERROR__S435_INIT_TASKS                               =  13901 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_GET_TEMPERATURE                          =  13902 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_CALC_CONTROL_PARAMS                      =  13903 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_OVEN_GET_CONTROL_OUTPUT                  =  13904 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_REACTOR_GET_CONTROL_OUTPUT               =  13905 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_COUNTER
};

enum _E_LibExceptions_Error_S435TemperatureControlTasks
{
   LIB_EXCEPTIONS__ERROR__S435_SIGNAL_PROCESSING_TASKS_TEMPERATURE_CONTROL                 =  13001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S435_SIGNAL_PROCESSING_TASKS_COUNTER
};

//========================================================================================================================================================
//========================================================================================================================================================
//========================================================================================================================================================


//---------------------------------------------- enum errors S7515-----------------------------------------------------------------

enum _E_LibExceptions_Error_S7515Main
{
   LIB_EXCEPTIONS__ERROR__S7515_MAIN__TSK_INIT                                               =  14001 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__ERROR__S7515_MAIN_COUNTER                                                
};

enum _E_LibExceptions_Error_S7515General
{
LIB_EXCEPTIONS__ERROR__VACUUM_FAIL                                                         =  14101 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
LIB_EXCEPTIONS__ERROR__VACUUM_BUILDUP                                                      =  14102 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
};

enum _E_LibExceptions_Error_S7515ModbusTasks
{
LIB_EXCEPTIONS__ERROR__S7515_MODBUS_TASKS__HREG_FRAM_WRITE                                =  14201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
LIB_EXCEPTIONS__ERROR__S7515_MODBUS_TASKS__CMD_EXECUTION                                  =  14202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
};

enum _E_LibExceptions_Error_S7515RegistersFuncs
{
LIB_EXCEPTIONS__ERROR__S7515_REGISTERS_FUNCS__INIT_DEVICE_PARAMS                         =  14201 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
LIB_EXCEPTIONS__ERROR__S7515_REGISTERS_FUNCS__INIT_STATUS_DATA                           =  14202 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
LIB_EXCEPTIONS__ERROR__S7515_REGISTERS_FUNCS_COUNTER
};

//---------------------------------------------- enums exception warnings ----------------------------------------------------------------------

enum _E_LibExceptions_WarningSpecial
{
   LIB_EXCEPTIONS__WARNING_SPECIAL__DEVICE_BUSY                                                    =  1 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_SW_ERRORS + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
   LIB_EXCEPTIONS__WARNING_SPECIAL__DEVICE_ACTIVE_INTERFACE_WRONG                                  =  2 + (ushort)LIB_EXCEPTIONS__OFFSET_TO_SW_ERRORS + (ushort)LIB_EXCEPTIONS__OFFSET_TO_DRIVER_ERRORS,
};

//---------------------------------------------- unions ----------------------------------------------------------------------------------------

// expand this union with every new enum of exceptions
typedef union
{
   enum _E_LibExceptions_Error_CommonRegistersFuncs               E_LibExceptions_Error_CommonRegistersFuncs;
   enum _E_LibExceptions_Error_CommonModbusFuncs                  E_LibExceptions_Error_CommonModbusFuncs;
   enum _E_LibExceptions_Error_CommonModbusTasks                  E_LibExceptions_Error_CommonModbusTasks;
   enum _E_LibExceptions_Error_CommonCmdsOwner                    E_LibExceptions_Error_CommonCmdsOwner;
   enum _E_LibExceptions_Error_CommonCmdsUser                     E_LibExceptions_Error_CommonCmdsUser;
   enum _E_LibExceptions_Error_CommonCmdsReg                      E_LibExceptions_Error_CommonCmdsReg;
   enum _E_LibExceptions_Error_UtilsFuncs                         E_LibExceptions_Error_UtilsFuncs;
   enum _E_LibExceptions_Error_UtilsTasks                         E_LibExceptions_Error_UtilsTasks;
   enum _E_LibExceptions_Error_LibExceptionsFuncs                 E_LibExceptions_Error_LibExceptionsFuncs;
   enum _E_LibExceptions_Error_CommonSupportFuncs                 E_LibExceptions_Error_CommonSupportFuncs;
   enum _E_LibExceptions_Error_CommonSupportTasks                 E_LibExceptions_Error_CommonSupportTasks;
   enum _E_LibExceptions_ErrorSpecial                             E_LibExceptions_ErrorSpecial;
   enum _E_LibExceptions_Error_ValveActMain                       E_LibExceptions_Error_ValveActMain;
   enum _E_LibExceptions_Error_ValveActRegistersFuncs             E_LibExceptions_Error_ValveActRegistersFuncs;
   enum _E_LibExceptions_Error_ValveActModbusFuncs                E_LibExceptions_Error_ValveActModbusFuncs;
   enum _E_LibExceptions_Error_ValveActModbusTasks                E_LibExceptions_Error_ValveActModbusTasks;
   enum _E_LibExceptions_Error_ValveActCmdsOwner                  E_LibExceptions_Error_ValveActCmdsOwner;
   enum _E_LibExceptions_Error_ValveActCmdsUser                   E_LibExceptions_Error_ValveActCmdsUser;
   enum _E_LibExceptions_Error_ValveActCmdsReg                    E_LibExceptions_Error_ValveActCmdsReg;
   enum _E_LibExceptions_Error_ValveActSupportFuncs               E_LibExceptions_Error_ValveActSupportFuncs;
   enum _E_LibExceptions_Error_ValveActSupportTasks               E_LibExceptions_Error_ValveActSupportTasks;
   enum _E_LibExceptions_Error_DetectorMain                       E_LibExceptions_Error_DetectorMain;
   enum _E_LibExceptions_Error_DetectorRegistersFuncs             E_LibExceptions_Error_DetectorRegistersFuncs;
   enum _E_LibExceptions_Error_DetectorModbusFuncs                E_LibExceptions_Error_DetectorModbusFuncs;
   enum _E_LibExceptions_Error_DetectorModbusTasks                E_LibExceptions_Error_DetectorModbusTasks;
   enum _E_LibExceptions_Error_DetectorCmdsOwner                  E_LibExceptions_Error_DetectorCmdsOwner;
   enum _E_LibExceptions_Error_DetectorCmdsUser                   E_LibExceptions_Error_DetectorCmdsUser;
   enum _E_LibExceptions_Error_DetectorCmdsReg                    E_LibExceptions_Error_DetectorCmdsReg;
   enum _E_LibExceptions_Error_DetectorSupportFuncs               E_LibExceptions_Error_DetectorSupportFuncs;
   enum _E_LibExceptions_Error_DetectorSupportTasks               E_LibExceptions_Error_DetectorSupportTasks;
   enum _E_LibExceptions_Error_DetectorDataOutputTasks            E_LibExceptions_Error_DetectorDataOutputTasks;
   enum _E_LibExceptions_Error_ColumnOvenMain                     E_LibExceptions_Error_ColumnOvenMain;
   enum _E_LibExceptions_Error_ColumnOvenRegistersFuncs           E_LibExceptions_Error_ColumnOvenRegistersFuncs;
   enum _E_LibExceptions_Error_ColumnOvenModbusFuncs              E_LibExceptions_Error_ColumnOvenModbusFuncs;
   enum _E_LibExceptions_Error_ColumnOvenModbusTasks              E_LibExceptions_Error_ColumnOvenModbusTasks;
   enum _E_LibExceptions_Error_ColumnOvenCmdsOwner                E_LibExceptions_Error_ColumnOvenCmdsOwner;
   enum _E_LibExceptions_Error_ColumnOvenCmdsUser                 E_LibExceptions_Error_ColumnOvenCmdsUser;
   enum _E_LibExceptions_Error_ColumnOvenCmdsReg                  E_LibExceptions_Error_ColumnOvenCmdsReg;
   enum _E_LibExceptions_Error_ColumnOvenSupportFuncs             E_LibExceptions_Error_ColumnOvenSupportFuncs;
   enum _E_LibExceptions_Error_ColumnOvenSupportTasks             E_LibExceptions_Error_ColumnOvenSupportTasks;
   enum _E_LibExceptions_Error_ColumnOvenTemperatureControlFuncs  E_LibExceptions_Error_ColumnOvenTemperatureControlFuncs;
   enum _E_LibExceptions_Error_ColumnOvenTemperatureControlTasks  E_LibExceptions_Error_ColumnOvenTemperatureControlTasks;
   enum _E_LibExceptions_Error_S1420Main                          E_LibExceptions_Error_S1420Main;
   enum _E_LibExceptions_Error_S1420RegistersFuncs                E_LibExceptions_Error_S1420RegistersFuncs;
   enum _E_LibExceptions_Error_S1420ModbusFuncs                   E_LibExceptions_Error_S1420ModbusFuncs;
   enum _E_LibExceptions_Error_S1420ModbusTasks                   E_LibExceptions_Error_S1420ModbusTasks;
   enum _E_LibExceptions_Error_S1420CmdsOwner                     E_LibExceptions_Error_S1420CmdsOwner;
   enum _E_LibExceptions_Error_S1420CmdsUser                      E_LibExceptions_Error_S1420CmdsUser;
   enum _E_LibExceptions_Error_S1420CmdsReg                       E_LibExceptions_Error_S1420CmdsReg;
   enum _E_LibExceptions_Error_S1420SupportFuncs                  E_LibExceptions_Error_S1420SupportFuncs;
   enum _E_LibExceptions_Error_S1420SupportTasks                  E_LibExceptions_Error_S1420SupportTasks;
   enum _E_LibExceptions_Error_S1420PressureControlTasks          E_LibExceptions_Error_S1420PressureControlTasks;
   enum _E_LibExceptions_Error_S1420ControlTasks                  E_LibExceptions_Error_S1420ControlTasks;
   enum _E_LibExceptions_Error_S1130Main                          E_LibExceptions_Error_S1130Main;
   enum _E_LibExceptions_Error_S1130RegistersFuncs                E_LibExceptions_Error_S1130RegistersFuncs;
   enum _E_LibExceptions_Error_S1130ModbusFuncs                   E_LibExceptions_Error_S1130ModbusFuncs;
   enum _E_LibExceptions_Error_S1130ModbusTasks                   E_LibExceptions_Error_S1130ModbusTasks;
   enum _E_LibExceptions_Error_S1130CmdsOwner                     E_LibExceptions_Error_S1130CmdsOwner;
   enum _E_LibExceptions_Error_S1130CmdsUser                      E_LibExceptions_Error_S1130CmdsUser;
   enum _E_LibExceptions_Error_S1130CmdsReg                       E_LibExceptions_Error_S1130CmdsReg;
   enum _E_LibExceptions_Error_S1130SupportFuncs                  E_LibExceptions_Error_S1130SupportFuncs;
   enum _E_LibExceptions_Error_S1130SupportTasks                  E_LibExceptions_Error_S1130SupportTasks;
   enum _E_LibExceptions_Error_S1130PressureControl               E_LibExceptions_Error_S1130PressureControl;
	enum _E_LibExceptions_Error_S1130DegasControl   					E_LibExceptions_Error_S1130DegasControl;
   enum _E_LibExceptions_Error_S1130Control                       E_LibExceptions_Error_S1130Control;
   enum _E_LibExceptions_Error_S1130Runtime                       E_LibExceptions_Error_S1130Runtime;
   enum _E_LibExceptions_Error_S1130ValveControl                  E_LibExceptions_Error_S1130ValveControl;
   enum _E_LibExceptions_Error_AutosamplerRegistersFuncs          E_LibExceptions_Error_AutosamplerRegistersFuncs;
   enum _E_LibExceptions_Error_AutosamplerModbusTasks             E_LibExceptions_Error_AutosamplerModbusTasks;
   enum _E_LibExceptions_Error_AutosamplerCmdsOwner               E_LibExceptions_Error_AutosamplerCmdsOwner;
   enum _E_LibExceptions_Error_AutosamplerCmdsUser                E_LibExceptions_Error_AutosamplerCmdsUser;
   enum _E_LibExceptions_Error_AutosamplerGeneral                 E_LibExceptions_Error_AutosamplerGeneral;
   
} tU_LibExceptions_ErrorCodeSW;

// expand this union with every new enum of exceptions
typedef union
{
   enum _E_LibExceptions_WarningSpecial                           E_LibExceptions_WarningSpecial;

} tU_LibExceptions_WarningCodeSW;

//---------------------------------------------- structures ------------------------------------------------------------------------------------

struct _S_LibExceptions_ErrorStringsSW
{
   ushort   u16_ErrorCodeSW;
   uchar    u8A_ErrorStringSW[LIB_EXCEPTIONS__ERROR_STRING_SW_CHARS_MAX];
};


typedef struct
{
   ushort   u16_WarningCode;
   ushort   u16_ErrorCodeApp;
   ushort   u16_ErrorCodeSW_Base;
   ulong    u32_ErrorCodeSW_Sub;

} tS_LibExceptions_Exceptions, *tpS_LibExceptions_Exceptions;

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern const struct _S_LibExceptions_ErrorStringsSW  cSA_LibExceptions_ErrorStringsSW[LIB_EXCEPTIONS__ERROR_STRINGS_SW_STRUCT_ENTRIES_MAX];


#endif


// docu of variables
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_CommonRegistersFuncs
*
*  \brief      This enum contains software error codes for source files \ref common_registers_funcs.c and 
*              \ref common_registers_funcs.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_CommonModbusFuncs
*
*  \brief      This enum contains software error codes for source files \ref common_modbus_funcs.c and 
*              \ref common_modbus_funcs.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_CommonModbusTasks
*
*  \brief      This enum contains software error codes for source files \ref common_modbus_tasks.c and 
*              \ref common_modbus_tasks.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_CommonCmdsOwner
*
*  \brief      This enum contains software error codes for source files \ref common_cmds_owner.c and 
*              \ref common_cmds_owner.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_CommonCmdsUser
*
*  \brief      This enum contains software error codes for source files \ref common_cmds_user.c and 
*              \ref common_cmds_user.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_CommonCmdsReg
*
*  \brief      This enum contains software error codes for source files \ref common_cmds_reg.c and 
*              \ref common_cmds_reg.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_UtilsFuncs
*
*  \brief      This enum contains software error codes for source files \ref utils_funcs.c and 
*              \ref utils_funcs.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_UtilsTasks
*
*  \brief      This enum contains software error codes for source files \ref utils_tasks.c and 
*              \ref utils_tasks.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_ValveActMain
*
*  \brief      This enum contains software error codes for source files \ref valve_act_main.c.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_ValveActRegistersFuncs
*
*  \brief      This enum contains software error codes for source files \ref valve_act_registers_funcs.c and 
*              \ref valve_act_registers.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_ValveActModbusFuncs
*
*  \brief      This enum contains software error codes for source files \ref valve_act_modbus_funcs.c and 
*              \ref valve_act_modbus_funcs.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_ValveActModbusTasks
*
*  \brief      This enum contains software error codes for source files \ref valve_act_modbus_tasks.c and 
*              \ref valve_act_modbus_tasks.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_ValveActCmdsOwner
*
*  \brief      This enum contains software error codes for source files \ref valve_act_cmds_owner.c and 
*              \ref valve_act_cmds_owner.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_ValveActCmdsUser
*
*  \brief      This enum contains software error codes for source files \ref valve_act_cmds_user.c and 
*              \ref valve_act_cmds_user.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_ValveActCmdsReg
*
*  \brief      This enum contains software error codes for source files \ref valve_act_cmds_reg.c and 
*              \ref valve_act_cmds_reg.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_ValveActSupportFuncs
*
*  \brief      This enum contains software error codes for source files \ref valve_act_support_funcs.c and 
*              \ref valve_act_support_funcs.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_ValveActSupportTasks
*
*  \brief      This enum contains software error codes for source files \ref valve_act_support_tasks.c and 
*              \ref valve_act_support_tasks.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_LibExceptionsFuncs
*
*  \brief      This enum contains software error codes for source files \ref lib_exceptions_funcs.c and 
*              \ref lib_exceptions_funcs.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_CommonSupportFuncs
*
*  \brief      This enum contains software error codes for source files \ref common_support_funcs.c and 
*              \ref common_support_funcs.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_ErrorSpecial
*
*  \brief      This enum contains software error codes which can be used by any application function if needed.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_Error_UtilsTasks
*
*  \brief      This enum contains software error codes for source files \ref utils_tasks.c and 
*              \ref utils_tasks.h.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_LibExceptions_WarningSpecial
*
*  \brief      This enum contains software warning codes which can be used by any application function if needed.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \union      tU_LibExceptions_ErrorCodeSW
*
*  \brief      This union contains all enums of software errors which are used by different modules.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \union      tU_LibExceptions_WarningCodeSW
*
*  \brief      This union contains all enums of software warnings.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     _S_LibExceptions_ErrorStringsSW
*
*  \brief      Structure contains a field for a software base error code which corresponds to a software error string.
*
*  \param      u16_ErrorCodeSW_Base                                              This is the software base error code which
*                                                                                defines which command finished with an error.
*  \param      u8A_ErrorStringSW[LIB_EXCEPTIONS__ERROR_STRING_SW_CHARS_MAX]      Inside this array the software error string
*                                                                                with corresponding base error code \em
*                                                                                u16_ErrorCodeSW_Base is written.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     cSA_LibExceptions_ErrorStringsSW
*
*  \brief      This structure is used as constant structure array which contains all valid software error strings. See \ref 
*              _S_LibExceptions_ErrorStringsSW for details.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     tS_LibExceptions_Exceptions
*
*  \brief      This structure typedef is the basic exception structure including a software base and sub error code as well
*              as a warning code. It is commonly used inside tasks to pass a structure pointer of tS_LibExceptions_Exceptions 
*              to \ref LibExceptionsFuncs_ExceptionSet.
*
*  \param      u16_WarningCode                                                   The software warning code is basically used to show
*                                                                                a warning. For example if the user executes
*                                                                                application functions in wrong order a warning can occur.
*                                                                                <em>Currently unused.</em>
*                                                                                This is not implemented at moment by \ref LibExceptionsFuncs_ExceptionSet!
*  \param      u16_ErrorCodeApp                                                  This is the device application error code which shows 
*                                                                                an user operating error. <em>Currently unused.</em>
*  \param      u16_ErrorCodeSW_Base                                              This is the software base error code which
*                                                                                shall show which command finished with an error.
*  \param      u32_ErrorCodeSW_Sub                                               The software sub error code. Sub errors occur usually
*                                                                                inside an command. At maximum two 16 bit sub error
*                                                                                codes from error code enums can be set.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
