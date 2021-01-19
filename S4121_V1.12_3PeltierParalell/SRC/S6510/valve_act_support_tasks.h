/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_support_tasks.h
*
*  \brief               Header file for \ref valve_act_support_tasks.c
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-10     Initial version extracted from S6510_cmd_functions.h and S6510_utils.h.
*  \date                2012-11-06     Changed name of function "TSK_ValveActSupportTasks_CmdExecutionPinContactStrip"
*                                      to \ref ValveActSupportTasks_ExternControlContactHandler.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef VALVE_ACT_SUPPORT_TASKS__H
#define VALVE_ACT_SUPPORT_TASKS__H


//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

         void     ValveActSupportTasks_ExternControlContactHandler      (U16 u16INTF, U16 u16INTCAP, U16 u16Pin, bool bLevel); // task context!
__task   void     TSK_ValveActSupportTasks_GenerateInjectSignal         (void);


#endif


// docu of tasks
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
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         void ValveActSupportTasks_ExternControlContactHandler (U16 u16INTF, U16 u16INTCAP, U16 u16Pin, bool bLevel)
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
*                             corresponds to the pin which caused the interrupt. E.g. <em>u16INTF = 0x0001</em> shows
*                             that pin GPA0 caused the interrupt.
*
*  \param      u16INTCAP      This is the original <em>interrupt capture register</em> of MCP23S17. A bit which is set 
*                             corresponds to the pin state, i.e. logic low/high, if an interrupt occured. E.g. 
*                             <em>u16INTCAP = 0x0001</em> shows that pin GPA0 which caused the interrupt had a high level
*                             when the interrupt occured. \b Caution: This example is only valid if MCP23S17 was configured
*                             as active high!
*
*  \param      u16Pin         This is the pin mask, i.e. the corresponding bits are set for a pin on which a event
*                             (rising/falling edge) occured. E.g. u16Pin = 0x0008 corresponds to an event on pin 
*                             GPB3/\ref MCP2317_PB3.
*
*  \param      bLevel         This is the pin level. It works like described for \em u16Pin with exception that a pin 
*                             which is set here corresponds to logic level \em high or \em low.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
