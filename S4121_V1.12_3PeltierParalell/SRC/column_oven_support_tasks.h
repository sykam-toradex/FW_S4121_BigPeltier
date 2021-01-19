/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_support_tasks.h
*
*  \brief               Header file for \ref column_oven_support_tasks.c
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2011-02-03     Initial version imported from S6510_valves.h.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test
*
*  \todo                set gas sensing threshold define somewhere else!
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef COLUMN_OVEN_SUPPORT_TASKS__H
#define COLUMN_OVEN_SUPPORT_TASKS__H
//---------------------------------------------- defines ---------------------------------------------------------------------------------------
#define  COLUMN_OVEN_SUPPORT_TASKS__EVENT_FOR_TSK_TEMPERATURE_CONTROL               (0x0001)
#define  COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_ALARM_THRESHOLD_PPM          (0x1000)
#define  COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_ALARM_ENABLED                (0x0001)
#define  COLUMN_OVEN_SUPPORT_TASKS__DEFAULT_GAS_SENSOR_CALIBRATION_RESISTANCE       (2500)
#define  COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_CODE_PARAMS_STORED                   (0xA8E4)
#define  COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_MASK_ALARM_DISABLED                  (0xFFFE)
#define  COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_SERIES_RESISTANCE_IN_OHM             (470)
#define  COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_CALIBRATION_CONCENTRATION_IN_PPM     (300)
#define  COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_ENABLED                              (0x0001)
#define  COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_DISABLED                             (0x0000)
#define  COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_ALARM                                (0x0001)
#define  COLUMN_OVEN_SUPPORT_TASKS__GAS_SENSOR_NO_ALARM                             (0x0000)
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
void     ColumnOvenSupportTasks_ExternControlContactHandler (U16 u16INTF, U16 u16INTCAP, U16 u16Pin, bool bLevel); // task context!
__task   void     TSK_ColumnOvenSupportTasks_GasSensing              (void);
__task   void     TSK_ColumnOvenSupportTasks_GlobalTimer             (void);
#endif
// docu of tasks
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         void ColumnOvenSupportTasks_ExternControlContactHandler (U16 u16INTF, U16 u16INTCAP, U16 u16Pin, bool bLevel)
*
*  \brief      This function, referenced in \ref SMcp2317List and called via task \ref TskNotifyHelperMcp2317, reacts on
*              pin contact strip events of pins \ref AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT and \ref AT91SAM7X__MCP2317_VALVE_GO_TO_HOME.
*              An event on \ref AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT results in calling the user command function
*              \ref ValveActCmdsUser_GoToHomePos.
*              On the other side an event on \ref AT91SAM7X__MCP2317_VALVE_GO_TO_NEXT results in calling \ref ValveActCmdsUser_GoToNextPos. \n
*              The function sets an ulong software base and sub error code which is greater than zero into valve actuator register
*              bank if any error occurrs. See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*
*  \param      u16INTF        This is the original <em>interrupt flag register</em> of MCP23S17. A bit which is set
*                             corresponds to the pin which caused the interrupt. For example <em>u16INTF = 0x0001</em> shows
*                             that pin GPA0 caused the interrupt.
*
*  \param      u16INTCAP      This is the original <em>interrupt capture register</em> of MCP23S17. A bit which is set
*                             corresponds to the pin state, that means logic low/high, if an interrupt occured. For example
*                             <em>u16INTCAP = 0x0001</em> shows that pin GPA0 which caused the interrupt had a high level
*                             when the interrupt occured. \b Caution: This example is only valid if MCP23S17 was configured
*                             as active high!
*
*  \param      u16Pin         This is the pin mask, that means the corresponding bits are set for a pin on which a event
*                             (rising/falling edge) occured. For example u16Pin = 0x0008 corresponds to an event on pin
*                             GPB3/\ref MCP2317_PB3.
*
*  \param      bLevel         This is the pin level. It works like described for \em u16Pin with exception that a pin
*                             which is set here corresponds to logic level \em high or \em low.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         __task void TSK_ValveActSupportTasks_GenerateInjectSignal (void)
*
*  \brief      This function clears port expander pin \ref AT91SAM7X__MCP2317_N_SYS_ERROR for one second (1s low voltage level) if a
*              injection valve is selected and inject position \ref AT91SAM7X__VALVE_ACT_POS_INDEX_INJECT is the current position. \n
*              The function sets an ulong software base and sub error code which is greater than zero into valve actuator register
*              bank if any error occurrs. See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
