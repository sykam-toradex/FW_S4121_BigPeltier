/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_modbus_communication.h
*
*  \brief               Header file for \ref column_oven_modbus_communication.c
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2011-02-03     Initial file imported from S6510_modbus_communication.c.
*
*
*  \bug                 No bugs known at moment. \n
*
*  \test
*
*  \todo                FILE OK
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef COLUMN_OVEN_MODBUS_TASKS__H
#define COLUMN_OVEN_MODBUS_TASKS__H
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
__task void TSK_ColumnOvenModbusTasks_CmdExecution (void);
#endif
// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         __task void TSK_ColumnOvenModbusTasks_CmdExecution( void )
*
*  \brief      This task will be wake-up by RTOS if a new trigger message arrived. If this task is running it decodes message box
*              values (incoming modbus requests) and executes a received command, if device is not busy with preceding command. It is
*              not possible to execute user commands if RS232 \b or display keys are not selected. Every error occurred will be set
*              into register bank. At the end a display update will be done by setting the display attention pin low though it refreshes
*              automatically.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
