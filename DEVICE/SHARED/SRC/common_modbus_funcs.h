/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_modbus_funcs.h
*
*  \brief               Header file for \ref common_modbus_funcs.c
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

#ifndef COMMON_MODBUS_FUNCS__H
#define COMMON_MODBUS_FUNCS__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

#define  COMMON_MODBUS_FUNCS__MODBUS_ADDR_MASK__COMMON_CMDS                (0xFFFF)
#define  COMMON_MODBUS_FUNCS__MODBUS_VAL_MASK__COMMON_CMDS                 (0xFFFF)
#define  COMMON_MODBUS_FUNCS__MODBUS_ADDR_MIN__COMMON_CMDS                 (COMMON_REGISTERS__MODBUS_ADDR_HREG__DEVICE_PARAMS__START)
#define  COMMON_MODBUS_FUNCS__MODBUS_ADDR_MAX__COMMON_CMDS                 (COMMON_REGISTERS__MODBUS_ADDR_HREG__CMDS_OWNER__END)
#define  COMMON_MODBUS_FUNCS__MODBUS_VAL_MIN__COMMON_CMDS                  (0x0000)
#define  COMMON_MODBUS_FUNCS__MODBUS_VAL_MAX__COMMON_CMDS                  (0xFFFF)

//#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY

   #define  COMMON_MODBUS_FUNCS__MODBUS_ADDR_MASK__RESPONSE_ON_DISPLAY_READ   (0xFFFF)
   #define  COMMON_MODBUS_FUNCS__MODBUS_VAL_MASK__RESPONSE_ON_DISPLAY_READ    (0xFFFF)
   #define  COMMON_MODBUS_FUNCS__MODBUS_ADDR_MIN__RESPONSE_ON_DISPLAY_READ    (COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW)
   #define  COMMON_MODBUS_FUNCS__MODBUS_ADDR_MAX__RESPONSE_ON_DISPLAY_READ    (COMMON_REGISTERS__MODBUS_ADDR_IREG__DEVICE_ERROR_CNT_SW)
   #define  COMMON_MODBUS_FUNCS__MODBUS_VAL_MIN__RESPONSE_ON_DISPLAY_READ     (0x0000) // value min-max requested by driver but unused
   #define  COMMON_MODBUS_FUNCS__MODBUS_VAL_MAX__RESPONSE_ON_DISPLAY_READ     (0xFFFF) // if trigger type for modbus filter is "input read"

//#endif
//#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
//#endif

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern   OS_MBX    MBX_CommonModbusFuncs_CmdExecution;

#ifdef AT91SAM7X__SPI0_MASTER_IS_DISPLAY
   extern   OS_MBX    MBX_CommonModbusFuncs_ResponseOnDisplayRead;

#endif
#ifdef AT91SAM7X__SPI0_MASTER_IS_S5300
#endif

//---------------------------------------------- function prototypes ---------------------------------------------------------------------------

ulong    CommonModbusFuncs_InitTasks               (void);
// exception for return type of following function because the function is called in application and originally a callback 
// function from modbus driver
bool     CommonModbusFuncs_FilterForModbusRequests (uchar u8_ModbusDataType, U32 u32_ModbusAddress, ushort *pu16_ModbusData);


#endif


// docu of functions
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         ulong CommonModbusFuncs_InitTasks (void)
*
*  \brief      This function initializes common modbus tasks. It sets a "trigger" filter for the tasks that they are called only if 
*              certain addresses are read/written via modbus.
*
*
*  \return     Returns an ulong software sub error code which is greater than zero if any error occurred. 
*              See \ref tU_LibExceptions_ErrorCodeSW and \ref tU_LibExceptionsDrv_ErrorCodeSW.
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*
*  \fn         bool CommonModbusFuncs_FilterForModbusRequests (uchar u8_ModbusDataType, ushort u16_ModbusAddress, ushort *pu16_ModbusData)
*
*  \brief      A special prefilter callback function which can be called inside \ref AT91SAM7X_FilterForModbusRequests. It is possible to 
*              change the data value of an incoming <b>write</b> request, to decline or to accept it.
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
