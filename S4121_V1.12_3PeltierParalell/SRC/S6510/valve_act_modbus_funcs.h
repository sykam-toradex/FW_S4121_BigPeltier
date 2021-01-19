/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_modbus_funcs.h
*
*  \brief               Header file for \ref valve_act_modbus_funcs.c
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-08-11     Initial version imported S4121_modbus_communication.c.
*  \date                2011-08-11     Renamed file from S4121_modbus_communication.c and included only init function prototypes.
*  \date                2011-08-26     Added modbus prefilter function.
*  \date                2012-11-07     Changed minimum start address for executable commands.
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

#ifndef VALVE_ACT_MODBUS_FUNCS__H
#define VALVE_ACT_MODBUS_FUNCS__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

#define  VALVE_ACT_MODBUS_FUNCS__MODBUS_ADDR_MASK__VALVE_ACT_CMDS    (0xFFFF)
#define  VALVE_ACT_MODBUS_FUNCS__MODBUS_VAL_MASK__VALVE_ACT_CMDS     (0xFFFF)
#define  VALVE_ACT_MODBUS_FUNCS__MODBUS_ADDR_MIN__VALVE_ACT_CMDS     (VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS_INTERN__START)
#define  VALVE_ACT_MODBUS_FUNCS__MODBUS_ADDR_MAX__VALVE_ACT_CMDS     (VALVE_ACT_REGISTERS__MODBUS_ADDR_HREG__CMDS_USER__END)
#define  VALVE_ACT_MODBUS_FUNCS__MODBUS_VAL_MIN__VALVE_ACT_CMDS      (0x0000)
#define  VALVE_ACT_MODBUS_FUNCS__MODBUS_VAL_MAX__VALVE_ACT_CMDS      (0xFFFF)

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   OS_MBX    MBX_ValveActModbusFuncs_CmdExecution;

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

ulong    ValveActModbusFuncs_InitDriver               (void);
ulong    ValveActModbusFuncs_InitTasks                (void);
// exception for return type of following function because this is a callback function from modbus driver
bool     ValveActModbusFuncs_FilterForModbusRequests  (uchar u8_ModbusDataType, U32 u32_ModbusAddress, ushort *pu16_ModbusData);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong ValveActModbusFuncs_InitDriver (void)
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
*  \fn         ulong ValveActModbusFuncs_InitTasks (void)
*
*  \brief      This function initializes tasks of file \ref valve_act_modbus_tasks.c. It configures and sets trigger filter(s)
*              for appropriate task(s).
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         bool ValveActModbusFuncs_FilterForModbusRequests (uchar u8_ModbusDataType, ushort u16_ModbusAddress, ushort *pu16_ModbusData)
*
*  \brief      A special prefilter callback function which can be called inside \ref AT91SAM7X_FilterForModbusRequests. It is possible to 
*              change the data value of an incoming <b>write</b> request, to decline (return value \em FALSE) or to 
*              accept it (return value \em TRUE).
*
*
*  \param      u8_ModbusDataType    This is the modbus data type, i.e. a holding register or a coil can be the type here. The values
*                                   is one out of \ref EMBPBRegType.
*                                   
*  \param      u16_ModbusAddress    This is the requested modbus address to write a value to.
*
*  \param      *pu16_ModbusData     This is a pointer to the content of u16_ModbusAddress. A modifcation occurs if this value is set
*                                   to another value.
*
*
*  \return     An exception inside application software design: This function returns an boolean value. If modbus value shall be
*              rejected by driver module return value must be \em FALSE otherwise \em TRUE.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
