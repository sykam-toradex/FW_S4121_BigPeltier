/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                column_oven_modbus_funcs.h
*
*  \brief               Header file for \ref column_oven_modbus_funcs.c
*
*  \author              Ruediger Radtke, Jose Verhelst
*  \version             1.0
*
*  \date                2011-02-03     Initial version imported S6510_modbus_communication.c.
*  \date                2011-08-11     Renamed file to \em column_oven_modbus_communication.c and included only init function prototypes.
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
*   Copyright(c) 2010-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifndef COLUMN_OVEN_MODBUS_FUNCS__H
#define COLUMN_OVEN_MODBUS_FUNCS__H
//---------------------------------------------- defines ---------------------------------------------------------------------------------------
#define  COLUMN_OVEN_MODBUS_FUNCS__MODBUS_ADDR_MASK__COLUMN_OVEN_CMDS    (0xFFFF)
#define  COLUMN_OVEN_MODBUS_FUNCS__MODBUS_VAL_MASK__COLUMN_OVEN_CMDS     (0xFFFF)
#define  COLUMN_OVEN_MODBUS_FUNCS__MODBUS_ADDR_MIN__COLUMN_OVEN_CMDS     (COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START) // first HREG address
#define  COLUMN_OVEN_MODBUS_FUNCS__MODBUS_ADDR_MAX__COLUMN_OVEN_CMDS     (COLUMN_OVEN_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__END)              // last HREG address
#define  COLUMN_OVEN_MODBUS_FUNCS__MODBUS_VAL_MIN__COLUMN_OVEN_CMDS      (0x0000)
#define  COLUMN_OVEN_MODBUS_FUNCS__MODBUS_VAL_MAX__COLUMN_OVEN_CMDS      (0xFFFF)
//---------------------------------------------- externals defined in this module --------------------------------------------------------------
extern   OS_MBX   MBX_ColumnOvenModbusFuncs_CmdExecution;
//---------------------------------------------- function prototypes ---------------------------------------------------------------------------
ulong    ColumnOvenModbusFuncs_InitDriver               (void);
ulong    ColumnOvenModbusFuncs_InitTasks                (void);
// exception for return type of following function because this is a callback function from modbus driver
bool     ColumnOvenModbusFuncs_FilterForModbusRequests  (uchar u8_ModbusDataType, ushort u16_ModbusAddress, ushort *pu16_ModbusData);
#endif
// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ColumnOvenModbusFuncs_InitDriver (void)
*
*  \brief      This function configures and initializes modbus driver.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ColumnOvenModbusFuncs_InitTasks (void)
*
*  \brief      This function initializes tasks of file \ref column_oven_modbus_tasks.c. It configures and sets trigger filter
*              for appropriate task.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred.
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         bool ColumnOvenModbusFuncs_FilterForModbusRequests (void)
*
*  \brief      This function is used in function \ref AT91SAM7X_FilterForModbusRequests. It filters (rejects) modbus request with
*              invalid arguments for owner, user, register commands or other holding registers. CAUTION: It runs in task context.
*
*
*  \return     Returns boolean \b true if modbus argument was valid or a \b false if modbus argument was invalid.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
