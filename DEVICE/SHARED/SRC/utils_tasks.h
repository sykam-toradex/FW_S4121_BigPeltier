/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                utils_tasks.h
*
*  \brief               The header file for \ref utils_tasks.c
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-25     Initial version imported from S4121_utils.c
*
*
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef UTILS_TASKS__H
#define UTILS_TASKS__H


//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

__task   void  TSK_UtilsTasks_Heartbeat (void);


#endif


/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         __task void TSK_UtilsTasks_Heartbeat (void)
*
*  \brief      This function lets the board led blink in the rhythm of a heart. It is used as external control if processor 
*              is working.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
