#include <stdio.h>
#include <string.h>

#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.h>
#include <RTL.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\..\cpu\at91\pio\piodata.h"
#include "..\..\..\cpu\at91\pio\pioext.h"

#include "..\..\..\cpu\at91\pwm\pwm.h"
#include "..\..\..\cpu\at91\pwm\pwmext.h"

#include "..\..\..\cpu\at91\spi\spi.h"
#include "..\..\..\cpu\at91\spi\spiext.h"

#include "..\..\..\cpu\at91\usart\usart.h"
#include "..\..\..\cpu\at91\usart\usartext.h"

#include "..\..\..\cpu\at91\rtt\rttext.h"

#include "..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085.h"
#include "..\..\..\ext\spi\dac\dacrrcs085\dacrrcs085ext.h"

#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbmsg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbdev.h"  
#include "..\..\..\protocol\modbus\mbpbext.h"

#include "..\..\..\cpu\at91\misc\stpclkdir\stpclkdir.h"
#include "..\..\..\cpu\at91\misc\stpclkdir\stpclkdirext.h"
#include "..\..\..\cpu\at91\misc\stpclkdir\auxiliary\stpclkdirauxext.h"

#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"

#include "..\..\..\DEVICE\SHARED\SRC\utils_funcs.h"
#include "..\..\..\DEVICE\SHARED\SRC\utils_tasks.h"


#include "..\..\..\DEVICE\S435C\SRC\S435_Cmds_Owner.h"
#include "..\..\..\DEVICE\S435C\SRC\S435_Cmds_User.h"
#include "..\..\..\DEVICE\S435C\SRC\S435_Modbus_Tasks.h"
#include "..\..\..\DEVICE\S435C\SRC\S435_Modbus_funcs.h"
#include "..\..\..\DEVICE\S435C\SRC\S435_Registers.h"
#include "..\..\..\DEVICE\S435C\SRC\S435_Register_funcs.h"
#include "..\..\..\DEVICE\S435C\SRC\S435_Utilities_Funcs.h"
#include "..\..\..\DEVICE\S435C\SRC\S435_Utilities_Funcs.h"

ushort   u16_JobIndex;
ushort   u16_Counter;
//OS_TID   TID_ValveTask;
//OS_TID   TID_CounterTask;

//extern OS_MBX   MBX_SensorFlag;

extern unsigned int ADC_GetConvertedData(AT91S_ADC *pAdc, unsigned int channel);

/*
__task void TSK_Counter(void)
{
   static ushort u16_DeviceState;
   FOREVER
   {
//      //MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_READ, MBPB_ADDR_IREG_VALVE_STATE, 1, &u16_DeviceState, NULL, OS_T_MS(100));      
      //if(u16_DeviceState == STATE_DEVICE_BUSY) u16_Counter++;
      RESCHEDULE(100);
   }
}
*/

//----------------------------------------------------------------------------

__task void TSK_ValveControl(void)
{
   static ushort  u16_MotorSpeed;        // motor speed
   static ushort  u16_MotorAcc;          // Motor acceleration
   static ushort  u16_StepMode;          // stepper motor step-mode
   static uchar    u8_StepMode;          // stepper motor step-mode
   static ushort  u16_ValvePos;
   static ushort  u16_DeviceState;
   static ushort  u16_Flag;
   ulong    u32_msg;
   
   u16_JobIndex = JOB_INIT;
   // load settings from ModBus
   //MBPBRegAccess(MBPB_REGTYPE_HOLDING, MBPB_REGRW_READ, MBPB_ADDR_HREG_MOTOR_SPEED  , 1, &u16_MotorSpeed, NULL, OS_T_MS(100));
   //MBPBRegAccess(MBPB_REGTYPE_HOLDING, MBPB_REGRW_READ, MBPB_ADDR_HREG_MOTOR_ACC  , 1, &u16_MotorAcc, NULL, OS_T_MS(100));
   //MBPBRegAccess(MBPB_REGTYPE_HOLDING, MBPB_REGRW_READ, MBPB_ADDR_HREG_MOTOR_MODE  , 1, &u16_StepMode, NULL, OS_T_MS(100));
   switch(u16_StepMode)
   {
      case 0:     Mcp2317SetOutput(0, AT91SAM7X__MCP2317_VALVE_MS1 , ~AT91SAM7X__MCP2317_VALVE_MS1, OS_T_MS(100)) ;
                  Mcp2317SetOutput(0, AT91SAM7X__MCP2317_VALVE_MS2 , ~AT91SAM7X__MCP2317_VALVE_MS2, OS_T_MS(100)) ;
                  u8_StepMode = 1;
                  break;                                     
      
      case 1:     Mcp2317SetOutput(0, AT91SAM7X__MCP2317_VALVE_MS1 , AT91SAM7X__MCP2317_VALVE_MS1, OS_T_MS(100)) ;
                  Mcp2317SetOutput(0, AT91SAM7X__MCP2317_VALVE_MS2 , ~AT91SAM7X__MCP2317_VALVE_MS2, OS_T_MS(100)) ;
                  u8_StepMode = 2;
                  break;
      
      case 2:     Mcp2317SetOutput(0, AT91SAM7X__MCP2317_VALVE_MS1 , ~AT91SAM7X__MCP2317_VALVE_MS1, OS_T_MS(100)) ;
                  Mcp2317SetOutput(0, AT91SAM7X__MCP2317_VALVE_MS2 , AT91SAM7X__MCP2317_VALVE_MS2, OS_T_MS(100)) ;
                  u8_StepMode = 4;
                  break;

      case 3:     Mcp2317SetOutput(0, AT91SAM7X__MCP2317_VALVE_MS1 , AT91SAM7X__MCP2317_VALVE_MS1, OS_T_MS(100)) ;
                  Mcp2317SetOutput(0, AT91SAM7X__MCP2317_VALVE_MS2 , AT91SAM7X__MCP2317_VALVE_MS2, OS_T_MS(100)) ;
                  u8_StepMode = 16;
                  break;
   }


   FOREVER
   {
      switch(u16_JobIndex)
      {
         case JOB_INIT:          
                                 //u16_DeviceState = STATE_DEVICE_BUSY; 
                                 u16_Counter = 0;
//                                 MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_WRITE, MBPB_ADDR_IREG_VALVE_STATE, 1, &u16_DeviceState, NULL, OS_T_MS(100));      
                                 u16_Flag = StpClkDirMove(0, STPCLKDIR_CLOCKWISE, 100, u16_MotorAcc, u16_MotorAcc, u16_MotorSpeed/4, u16_MotorSpeed, u8_StepMode, TRUE, NULL, OS_T_MS(100));
                                 //os_mbx_wait( (OS_ID)MBX_SensorFlag, (void *)(&u32_msg), OS_T_MS(3000));
                                 StpClkDirQuit(0, OS_T_MS(100));
                                 u16_ValvePos = VALVE_POS_LOAD;
                                 u16_JobIndex = JOB_IDLE;
//                                 MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_WRITE, MBPB_ADDR_IREG_VALVE_POSITION, 1, &u16_ValvePos, NULL, OS_T_MS(100));
                                 //u16_DeviceState       = STATE_DEVICE_STOP; 
//                                 MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_WRITE, MBPB_ADDR_IREG_VALVE_STATE, 1, &u16_DeviceState, NULL, OS_T_MS(100));
                                 break;
         case JOB_SWITCH_LOAD:   
                                 //MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_READ, MBPB_ADDR_IREG_VALVE_POSITION, 1, &u16_ValvePos, NULL, OS_T_MS(100));
                                 if(u16_ValvePos==VALVE_POS_LOAD) 
                                 {
                                    u16_JobIndex = JOB_IDLE;
                                    break;
                                 }
                                 else
                                 {
                                    //u16_DeviceState       = STATE_DEVICE_BUSY; 
                                    u16_Counter = 0;
//                                    MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_WRITE, MBPB_ADDR_IREG_VALVE_STATE, 1, &u16_DeviceState, NULL, OS_T_MS(100));      
                                    u16_Flag = StpClkDirMove(0, STPCLKDIR_CLOCKWISE, 1000, u16_MotorAcc, u16_MotorAcc, u16_MotorSpeed/4, u16_MotorSpeed, u8_StepMode, TRUE, NULL, OS_T_MS(100));
                                    //os_mbx_wait( (OS_ID)MBX_SensorFlag, (void *)(&u32_msg),  OS_T_MS(3000));
                                    StpClkDirQuit(0, OS_T_MS(100));
                                    u16_ValvePos = VALVE_POS_LOAD;
                                    u16_JobIndex = JOB_IDLE;
//                                    MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_WRITE, MBPB_ADDR_IREG_VALVE_POSITION, 1, &u16_ValvePos, NULL, OS_T_MS(100));
                                    //u16_DeviceState       = STATE_DEVICE_STOP; 
//                                    MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_WRITE, MBPB_ADDR_IREG_VALVE_STATE, 1, &u16_DeviceState, NULL, OS_T_MS(100));
                                 }
                                 break;

         case JOB_SWITCH_INJECT: 
                                 //MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_READ, MBPB_ADDR_IREG_VALVE_POSITION, 1, &u16_ValvePos, NULL, OS_T_MS(100));
                                 if(u16_ValvePos==VALVE_POS_INJECT) 
                                 {
                                    u16_JobIndex = JOB_IDLE;
                                    break;
                                 }
                                 else
                                 {
                                    //u16_DeviceState       = STATE_DEVICE_BUSY; 
                                    u16_Counter = 0;
                                    //MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_WRITE, MBPB_ADDR_IREG_VALVE_STATE, 1, &u16_DeviceState, NULL, OS_T_MS(100));      
                                    u16_Flag = StpClkDirMove(0, STPCLKDIR_COUNTERCLOCKWISE, 1000, u16_MotorAcc, u16_MotorAcc, u16_MotorSpeed/4, u16_MotorSpeed, u8_StepMode, TRUE, NULL, OS_T_MS(100));
                                    //os_mbx_wait( (OS_ID)MBX_SensorFlag, (void *)(&u32_msg),  OS_T_MS(3000));
                                    StpClkDirQuit(0, OS_T_MS(100));
                                    u16_ValvePos = VALVE_POS_INJECT;
                                    u16_JobIndex = JOB_IDLE;
                                    //MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_WRITE, MBPB_ADDR_IREG_VALVE_POSITION, 1, &u16_ValvePos, NULL, OS_T_MS(100));
                                    //u16_DeviceState       = STATE_DEVICE_STOP; 
                                    //MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_WRITE, MBPB_ADDR_IREG_VALVE_STATE, 1, &u16_DeviceState, NULL, OS_T_MS(100));
                                 }
                                 break;
         case JOB_IDLE:          RESCHEDULE(10);
                                 break;
      }
      RESCHEDULE(100);
   }
   DETACH;

}

//----------------------------------------------------------------------------


void Digital_IO(U32 u32ISR, U32 u32PDSR, U32 u32Pin, bool bLevel, AT91PS_PIO pPio)
{
  ushort   u16_DeviceState;
  ushort   u16_ValvePos;
  ulong    u32_msg;

   // move to LOAD position 1
   //if( (u32Pin == AT91C_PIO_PB0) && (!bLevel) )
   if( (u32Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_1) && (!bLevel) )
   {
      MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_READ, MBPB_ADDR_IREG_VALVE_POSITION, 1, &u16_ValvePos, NULL, OS_T_MS(100));
      
      if(u16_ValvePos != VALVE_POS_UNKNOWN)
      {
         u16_JobIndex = JOB_SWITCH_LOAD;
      }
      else
      {
         u16_JobIndex = JOB_INIT;
      }
   }
   
   // move to LOAD position 2
   //if( (u32Pin == AT91C_PIO_PB0) && (!bLevel) )
   if( (u32Pin == AT91SAM7X__MCP2317_VALVE_GO_TO_2) && (!bLevel) )
   {
      MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_READ, MBPB_ADDR_IREG_VALVE_POSITION, 1, &u16_ValvePos, NULL, OS_T_MS(100));
      
      if(u16_ValvePos != VALVE_POS_UNKNOWN)
      {
         u16_JobIndex = JOB_SWITCH_LOAD;
      }
      else
      {
         u16_JobIndex = JOB_INIT;
      }
   }
   // move to INJECT position
   //if( (u32Pin == AT91C_PIO_PB1) && (!bLevel) )
   if( (u32Pin == AT91SAM7X__MCP2317_NINJECT) && (!bLevel) )
   {
      //u16_DeviceState       = STATE_DEVICE_BUSY; 
      //MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_WRITE, MBPB_ADDR_IREG_VALVE_STATE, 1, &u16_DeviceState, NULL, OS_T_MS(100));
      MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_READ, MBPB_ADDR_IREG_VALVE_POSITION, 1, &u16_ValvePos, NULL, OS_T_MS(100));
      
      if(u16_ValvePos != VALVE_POS_UNKNOWN)
      {
         u16_JobIndex = JOB_SWITCH_INJECT;
      }
      else
      {
         u16_JobIndex = JOB_INIT;
      }
   }       
  // valve sensor trigger !!!
   //if( (u32Pin == AT91C_PIO_PB6) && (bLevel))
   if( (u32Pin == AT91B_LIGHT_BARRIER_INPUT_1) && (bLevel))
   {
      if(u16_Counter > 100)
      {         
         u32_msg = 1;
         //isr_mbx_send( (OS_ID)MBX_SensorFlag, (void *)(&u32_msg));
      }
   }
}

//----------------------------------------------------------------------------
