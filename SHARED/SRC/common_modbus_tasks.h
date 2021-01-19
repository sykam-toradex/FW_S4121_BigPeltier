/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_modbus_tasks.h
*
*  \brief               Header file for \ref common_modbus_tasks.c
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-08     Initial version imported from S4121_modbus_communication.h.
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef COMMON_MODBUS_TASKS__H
#define COMMON_MODBUS_TASKS__H


//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

__task void    TSK_CommonModbusTasks_CmdExecution            (void);
//__task void    TSK_CommonModbusTasks_ResponseOnDisplayRead   (void);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         __task void TSK_CommonModbusTasks_CmdExecution (void)
*
*  \brief      This task will be wake up by RTOS if a common command is received by driver module. For this purpose the "trigger" filter
*              of this task is configured for common command addresses. Received commands will be executed if device is not in "busy state".
*              It is not possible to execute user commands if RS232 \b or display keys are \b not selected. Every error which occurs will
*              be set into register bank. At the end a display update will be done by setting the display attention pin low (clear action).
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         __task void TSK_CommonModbusTasks_ResponseOnDisplayRead (void)
*
*  \brief      This task will be wake up by RTOS if the display tries to read the common error counter on register bank address
*              \ref COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW. 
*              At the end the display attention pin is set to high again and display update cycle is complete.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
