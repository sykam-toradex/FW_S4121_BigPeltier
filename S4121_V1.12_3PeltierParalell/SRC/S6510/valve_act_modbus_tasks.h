/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_modbus_tasks.h
*
*  \brief               Header file for \ref valve_act_modbus_tasks.c
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-11     Initial version imported from S4121_modbus_communication.h.
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
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef VALVE_ACT_MODBUS_TASKS__H
#define VALVE_ACT_MODBUS_TASKS__H


//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

__task void TSK_ValveActModbusTasks_CmdExecution (void);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         __task void TSK_ValveActModbusTasks_CmdExecution (void)
*
*  \brief      This task will be wake up by RTOS if a valve actuator write inquiry on a holding register is received by driver 
*              module. The task then executes a corresponding command from one of different function pointer arrays. For this 
*              purpose the "trigger" filter of this task is configured for valve actuator command addresses. 
*              Received commands will be executed if device is not in "busy", "init" or undefined state. It is not possible to 
*              execute user commands if "extern" (extern control contacts) is selected. 
*              Every error which occurs will be set into register bank.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
