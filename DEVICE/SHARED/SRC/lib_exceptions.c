/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                lib_exceptions.c
*
*  \brief               This file contains error strings which are related to system exception codes. If any errors occur,
*                       the corresponding error string is set to register bank.
*
*
*  \author              Ruediger Radtke, David Jaeger, Frank Meier
*  \version             1.5
*
*  \date                2011-07-21     Initial version.c.
*  \date                2011-11-23     <em>File version 1.2</em>. RR: Every device gets own space for error codes (2000 entries). Merged current codes.
*  \date                2012-08-21     <em>File version 1.3</em>. RR: Extended error strings in \ref cSA_LibExceptions_ErrorStringsSW.
*  \date                2012-11-12     <em>File version 1.4</em>. RR: Removed error strings for calibration data load/save of valve actuator because 
*                                      of removed commands. Changed error strings name "LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS__TSK_CMD_EXECUTION_PIN_CONTACT_STRIP"
*                                      tp \ref LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS__TSK_CMD_EXECUTION_EXTERN_CONTROL_CONTACT_HANDLER.
*  \date                2012-12-10     RR: Added error strings.
*  \date                2013-01-11     <em>File version 1.5</em>. RR: Corrected error string "Unknown error".
*  \date                2013-01-17     RR: Moved several error strings from detector strings to category "error special".
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
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include "..\..\..\rtl\support.h"

#include "lib_exceptions.h"


//---------------------------------------------- structures ------------------------------------------------------------------------------------

const struct _S_LibExceptions_ErrorStringsSW  cSA_LibExceptions_ErrorStringsSW[LIB_EXCEPTIONS__ERROR_STRINGS_SW_STRUCT_ENTRIES_MAX] =
{

//------------------------------------ error strings common ------------------------------------------------------------------------------------
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__SYKAM_DEVICE_DATA_LOAD,
      "Load sykam device data failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_OWNER__SYKAM_DEVICE_DATA_SAVE,
      "Save sykam device data failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COMMON_CMDS_REG__DEVICE_ACTIVE_INTERFACE,
      "Change device interface failed!"
   },

//------------------------------------ error strings special -----------------------------------------------------------------------------------

   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__UNKNOWN_ERROR,
      "Unknown error!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__MAINBOARD_LOW_VOLTAGE,
      "Low mainboard voltage!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__MAINBOARD_HIGH_VOLTAGE,
      "High mainboard voltage!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__ADC_INTERNAL_NOT_INITIALIZED,
      "ADC internal not initialized!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DEVICE_IS_NOT_READY,
      "Device is not ready!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DEVICE_OPTION_IS_NOT_ACTIVATED,
      "Device option is not activated!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__INVALID_FUNCTION_ARGUMENTS,
      "Invalid function arguments!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__MAILBOX_FULL,
      "Mailbox full!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__MAILBOX_TIMEOUT,
      "Mailbox timeout expired!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__MODBUS_DATA_DISASSEMLING_FAILED,
      "Modbus data disassembling failed!"
   },

//------------------------------------ error strings valve actuator ----------------------------------------------------------------------------

   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_MAIN__TSK_INIT,
      "Valve actuator: Initialization failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_LOAD,
      "Valve actuator: Load device params failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_LOAD_DEFAULT,
      "Valve actuator: Load default device params failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__DEVICE_PARAMS_SAVE,
      "Valve actuator: Save device params failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__STATUS_DATA_LOAD,
      "Valve actuator: Load status data failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_FUNCS__LOAD_CALIBRATION_DATA_AND_PARAMS,
      "Valve actuator: Calibration failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__CAL_WITH_DEVICE_PARAMS,
      "Valve actuator: Calibration failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__CAL_WITH_DEFAULT_PARAMS,
      "Valve actuator: Calibration failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_OWNER__RESET_TOTAL_SWITCH_COUNTER,
      "Valve actuator: Reset switch counter failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_HOME,
      "Valve actuator: Move failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_NEXT,
      "Valve actuator: Move failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_INQUIRED,
      "Valve actuator: Move failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_USER__GO_TO_PREVIOUS,
      "Valve actuator: Move failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__DAC_HOLD_MOTOR_DRIVER,
      "Valve actuator: DAC hold value cannot be set!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_CMDS_REG__DAC_RUN_MOTOR_DRIVER,
      "Valve actuator: DAC run value cannot be set!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS__TSK_GENERATE_INJECT_SIGNAL,
      "Valve actuator: Inject signal generation failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__VALVE_ACT_SUPPORT_TASKS__TSK_CMD_EXECUTION_EXTERN_CONTROL_CONTACT_HANDLER,
      "Valve actuator: Cannot execute desired command!"
   },

//------------------------------------ errors strings pump S1125 -------------------------------------------------------------------------------

   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PRESSURE_CONTROL,   
      "Error in Task Pressure Control"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL,   
      "Error in Task Pump Control"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_START,   
      "Error in Task Pump Control Start"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_DELAY_TIME,   
      "Error in Task Pump Control Delay Time"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_START_RAMP,   
      "Error in Task Pump Control Start Ramp"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_STOP_RAMP,   
      "Error in Task Pump Control Stop Ramp"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_RUN,   
      "Error in Task Pump Control Run"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_STOP,   
      "Error in Task Pump Control Stop"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_PRESSURE_MIN_MAX,   
      "Pump Control Min Max Pressure"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PUMP_CONTROL_PRESSURE_MAX,   
      "Max Pressure at Purge start"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PRESSURE_CONTROL_PRESSURE_MAX,
      "Error Pressure Max"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PRESSURE_CONTROL_PRESSURE_MIN,
      "Error Pressure Min"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420__TASK_PRESSURE_CONTROL_PRESSURE_MAX_PURGE,
      "Error Pressure Purge"
   },

//------------------------------------ errors strings pump S1130 -------------------------------------------------------------------------------

   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_MAIN__TSK_INIT,   
      "Pump Initialization failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__DEVICE_PARAMS_SAVE,   
      "Save device data failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__DEVICE_PARAMS_LOAD,   
      "Load device data failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__RESET_RUNTIME_COUNTER,   
      "Reset runtime counter failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER__PUMP_START,   
      "Start pump failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER__PUMP_STOP,   
      "Stop pump failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER__PURGE_START,   
      "Purge start failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER__PURGE_STOP,   
      "Purge stop failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_CMDS_USER__VALVE_SET_NEW_VALUES,   
      "Set new Valve Percent Values failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_CMDS_OWNER__DEVICE_PARAMS_LOAD_DEFAULT,   
      "Load default device data failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG,
      "Error in Gradient Program!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_HOLD_FLOW,
      "Hold state flow wrong"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_HOLD_VALVE,
      "Hold state contains wrong percent entry"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_FLOW_ENTRY_WRONG,
      "Time Program flow entry wrong"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_TIME_ENTRY_WRONG,
      "Time Program time entry wrong"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_VALVE_ENTRY_WRONG,
      "Time Program percent entry wrong"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_TIME_PROG_NO_TIME_PROG_SAVED,
      "No Time Progam saved"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1420_SUPPORT_FUNCS__LOAD_CALIBRATION_DATA_AND_PARAMS,
      "No Device Parameter saved!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S1130_PRESSURE_CONTROL_PRESSURE_MIN,
      "Error Pressure Min"
   },

//------------------------------------ errors strings autosampler ------------------------------------------------------------------------------

   {
      (ushort)LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_S,
      "Motor/Sensor Error: Syringe"
   },
   {         
      (ushort)LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_X,
      "Motor/Sensor Error: X-Axis"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_Y,
      "Motor/Sensor Error: Y-Axis"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_NI,
      "Motor/Sensor Error: I-Needle"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_NA,
      "Motor/Sensor Error: A-Needle"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_VI,
      "Motor/Sensor Error: I-Valve"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__S5151_AXIS_ERROR_VS,
      "Motor/Sensor Error: S-Valve"
   },

//------------------------------------ errors strings detector S3250/S3245 ---------------------------------------------------------------------

   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_SENSOR_DISCONNECTED,
      "Detector: Absorption sensor disconnected!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_SENSOR_SPI_DATA_NOT_AVAILABLE,
      "Detector: Absorption sensor data unavailable!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_WAVELENGTH_PROGRAM_IS_INVALID,
      "Detector: Wavelength program is invalid!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_WAVELENGTH_PROGRAM_WAS_NOT_SELECTED_TO_MODIFY,
      "Detector: Wavelength program was not modified!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_SCAN_PROGRAM_IS_INVALID,
      "Detector: Scan program is invalid!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_SCAN_PROGRAM_WAS_NOT_SELECTED_TO_MODIFY,
      "Detector: Scan program was not modified!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_PEAK_CENTER_SEARCH_WINDOW_IS_TOO_SMALL,
      "Detector: Window of peak search is too small!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_PEAK_WIDTH_EXCEEDED,
      "Detector: Maximum peak width exceeded!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_INVALID_CALIBRATION_COEFFICIENT,
      "Detector: Invalid calibration coeffcient!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_TUNGSTEN_LAMP_BROKEN_OR_WEAK,
      "Detector: Tungsten lamp broken or weak!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_DEUTERIUM_LAMP_BROKEN_OR_WEAK,
      "Detector: Deuterium lamp broken or weak!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_CALIBRATION_FAILED_LAMP_ENERGY_TOO_LOW,
      "Detector: Calibration failed (low lamp energy)!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_CALIBRATION_FAILED_LAMP_ENERGY_TOO_HIGH,
      "Detector: Calibration failed (high lamp energy)!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_CALIBRATION_FAILED_MAX_REPETITIONS_EXCEEDED,
      "Detector: Calibration failed (max. rep. reached)!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_LAMP_ENERGY_LOW,
      "Detector: Low lamp energy!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_INVALID_CHANNEL,
      "Detector: Invalid detector channel!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_INVALID_STATE,
      "Detector: Invalid detector state!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__DETECTOR_INVALID_ANALOG_OUTPUT_OFFSETS,
      "Detector: Invalid analog output offsets!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_MAIN__TSK_INIT,
      "Detector: Initialization failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER__REF_SIG_DATA_TRANSMISSION_ON_OFF,
      "Detector: Enable/disable of data output failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER__DEUTERIUM_LAMP_STATUS_DATA_RESET,
      "Detector: Reset of D2-lamp status data failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER__DEVICE_PARAMS_LOAD,
      "Detector: Load of device params failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER__SCAN_PROGRAM_START,
      "Detector: Start of scan program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_OWNER__ANALOG_OUTPUT_TESTMODE_ON_OFF,
      "Detector: Enable/disable analog-out test failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__AUTOZERO,
      "Detector: Autozero failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__CALIBRATION_START,
      "Detector: Start of calibration failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__ABSORPTION_DATA_TRANSMISSION_ON_OFF,
      "Detector: Enable/disable of data output failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__MULTI_CHANNEL_ON_OFF,
      "Detector: Enable/disable multi-channel failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__PEAK_DETECTION_ON_OFF,
      "Detector: Enable/disable peak detector failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__DETECTOR_LAMPS_ON_OFF,
      "Detector: Enable/disable D2/tungsten lamp failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__WAVELENGTH_PROGRAM_START,
      "Detector: Start of wavelength program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__WAVELENGTH_PROGRAM_STOP,
      "Detector: Stop of wavelength program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__WAVELENGTH_PROGRAM_DELETE,
      "Detector: Deletion of wavelength program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__WAVELENGTH_PROGRAM_SAVE,
      "Detector: Save of wavelength program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__WAVELENGTH_PROGRAM_SELECT_TO_MODIFY,
      "Detector: Modify wavelength program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_PROGRAM_START,
      "Detector: Start of scan program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_PROGRAM_STOP,
      "Detector: Stop of scan program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_PROGRAM_DELETE,
      "Detector: Deletion of scan program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_PROGRAM_SAVE,
      "Detector: Save of scan program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_PROGRAM_SELECT_TO_MODIFY,
      "Detector: Modification of scan program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_USER__SCAN_DATA_REPLAY,
      "Detector: Start of scan data replay failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_RISE_TIME_IN_MILLI_SECONDS,
      "Detector: Set rise time of channel 1 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_RISE_TIME_IN_MILLI_SECONDS,
      "Detector: Set rise time of channel 2 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_ANALOG_OUTPUT_RANGE_IN_MILLI_AU,
      "Detector: Set analog output range ch1 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_ANALOG_OUTPUT_RANGE_IN_MILLI_AU,
      "Detector: Set analog output range ch2 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_ANALOG_OUTPUT_OFFSET_IN_PERCENT,
      "Detector: Set analog output offset ch1 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_ANALOG_OUTPUT_OFFSET_IN_PERCENT,
      "Detector: Set analog output offset ch2 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_WAVELENGTH,
      "Detector: Set channel 1 wavelength failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_WAVELENGTH,
      "Detector: Set channel 2 wavelength failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__PEAK_DETECTOR_THRESHOLD_START_IN_MILLI_AU,
      "Detector: Set peak detector threshold failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__PEAK_DETECTOR_NOTIFICATION_DELAY_IN_SECONDS,
      "Detector: Set peak detector delay failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__DETECTOR_DATA_RATE_IN_HZ,
      "Detector: Set detector data output rate failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__EXTERN_CONTROL_CONTACT_CONFIG_PIN_10,
      "Detector: Set extern contact config pin 10 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_ANALOG_OUTPUT_OFFSET_BOTTOM,
      "Detector: Set analog-out bottom offset ch1 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_ANALOG_OUTPUT_OFFSET_BOTTOM,
      "Detector: Set analog-out bottom offset ch2 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_1_ANALOG_OUTPUT_OFFSET_TOP,
      "Detector: Set analog output top offset ch1 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__CHANNEL_2_ANALOG_OUTPUT_OFFSET_TOP,
      "Detector: Set analog output top offset ch2 failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__DETECTOR_CMDS_REG__INTEGRATION_TIME,
      "Detector: Set integration time failed!"
   },
   
//------------------------------------ errors strings column oven S4121 ---------------------------------------------------------------------

   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_MAIN__TSK_INIT,
      "Column Oven: Initialization failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__CONTROL_PARAMS_SAVE,
      "Column Oven: Save control parameters failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__DEVICE_PARAMS_LOAD,
      "Column Oven: Load device parameters failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_OWNER__STATUS_DATA_LOAD,
      "Column Oven: Load status data failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_START,
      "Column Oven: Start of device failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_STOP,
      "Column Oven: Stop of device failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_DELETE,
      "Column Oven: Delete heat/cool program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_SAVE,
      "Column Oven: Save heat/cool program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_PROGRAM_SELECT_TO_MODIFY,
      "Column Oven: Select heat/cool program failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_USER__HEAT_COOL_CALIBRATION_START,
      "Column Oven: Heat/cool calibration start failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__CONTROL_SECTIONS_PARAMS,
      "Column Oven: Modify control parameters failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__GAS_SENSOR_ALARM_THRESHOLD_PPM,
      "Column Oven: Save gas sensor threshold failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__GAS_SENSOR_CALIBRATION_RESISTANCE,
      "Column Oven: Save gas sensor cal. resist. failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__GAS_SENSOR_ALARM_ENABLED,
      "Column Oven: Save gas sensor alarm on/off failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__TEMPERATURE_OFFSET,
      "Column Oven: Save temperature offset failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__TEMPERATURE_UNIT,
      "Column Oven: Save temperature unit failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_CMDS_REG__HEAT_COOL_PROGRAM_SET_POINT_REACHED_BAND,
      "Column Oven: Save set point reached band failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__SET_DAC_OUTPUT,
      "Column Oven: Set output voltage for load failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SUPPORT_FUNCS__CALC_HEAT_COOL_PROGRAM_TIMES,
      "Column Oven: Calc heat/cool program times failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__GET_TEMPERATURE,
      "Column Oven: Under temperature (T < 0 C)!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__CALC_CONTROL_PARAMS,
      "Column Oven: Calc control params failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR__COLUMN_OVEN_SIGNAL_PROCESSING_FUNCS__GET_CONTROL_OUTPUT,
      "Column Oven: Get control output failed!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_CONTROL_PARAMS_NOT_AVAILABLE,
      "Column Oven: No control params available!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_TEMPERATURE_SENSOR_FAILURE,
      "Column Oven: Temperature sensor failure!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_GAS_SENSOR_FAILURE,
      "Column Oven: Gas sensor failure!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_EXTERNAL_ADC_FAILURE,
      "Column Oven: External ADC failure!"
   },
   {
      (ushort)LIB_EXCEPTIONS__ERROR_SPECIAL__COLUMN_OVEN_GAS_SENSOR_PARAMS_MISSING,
      "Column Oven: Missing gas sensor params!"
   },
   
};
