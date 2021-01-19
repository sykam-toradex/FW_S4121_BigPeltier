#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <AT91SAM7X512.H>
#include <lib_AT91SAM7X256.h>


#include <stdio.h>
#include <string.h>
#include <AT91SAM7X512.H>
//#include <AT91SAM7X256.H>
#include <lib_AT91SAM7X256.h>
#include <RTL.h>
#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"
#include "lib_exceptions.h"
#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317.h"
#include "..\..\..\ext\spi\gpio\mcp2317\mcp2317ext.h"
#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"
#include "common_registers.h"
#include "common_support_funcs.h"
#include "common_support_tasks.h"
#include "S5151_registers.h"

__task void TSK_OEM_LED(void)
{
   #ifdef OEM_LED
   {
      static USHORT     usDeviceState;
      static ulong    VialError;
      ushort   u16_ErrorCnt;
      FOREVER
      {
         // READ State Device
         MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_READ, S5151_REGISTERS__MODBUS_ADDR_IREG__PROGRAM_STATE , 1, &usDeviceState, NULL, OS_T_MS(100));
         switch (usDeviceState)
         {      
         case STATE_PROG_BUSY:
         case STATE_PROG_WAITING:
            AT91F_PIO_SetOutput (AT91C_BASE_PIOA, AT91SAM7X__PIN__LED_RUN);// RUN ON
            Mcp2317SetOutput (0, (ushort)AT91SAM7X__MCP2317_LED_READY, (ushort)~AT91SAM7X__MCP2317_LED_READY,100);//READY OFF                     
            break;
         case STATE_PROG_ERROR:         
   //            AT91F_PIO_SetOutput (AT91C_BASE_PIOA, AT91SAM7X__PIN__LED_ERROR);// ERROR ON
            break;
         case STATE_PROG_IDLE:
               Mcp2317SetOutput (0, (ushort)AT91SAM7X__MCP2317_LED_READY, (ushort)AT91SAM7X__MCP2317_LED_READY,100);//READY ON
               AT91F_PIO_ClearOutput (AT91C_BASE_PIOA, AT91SAM7X__PIN__LED_RUN);// RUN OFF
            break;
         default:
               AT91F_PIO_SetOutput (AT91C_BASE_PIOA, AT91SAM7X__PIN__LED_RUN);// RUN ON
               AT91F_PIO_SetOutput (AT91C_BASE_PIOA, AT91SAM7X__PIN__LED_ERROR);// ERROR ON
               Mcp2317SetOutput (0, (ushort)AT91SAM7X__MCP2317_LED_READY, (ushort)AT91SAM7X__MCP2317_LED_READY,100);//READY ON
               break;         
         }        
         // READ current vial error(s) 
         //If it is different than 0  then we light up the ERROR LED, independent of the Device State´.*/
         MBPBRegAccess(MBPB_REGTYPE_INPUT, MBPB_REGRW_READ, S5151_REGISTERS__MODBUS_ADDR_IREG__VIAL_ERROR , 1, &VialError, NULL, OS_T_MS(100));
         switch (VialError)
         {
            case 0: //If 0 there is no vial error
            AT91F_PIO_ClearOutput (AT91C_BASE_PIOA, AT91SAM7X__PIN__LED_ERROR);// ERROR OFF            
            //CommonSupportFuncs_ErrorCodeSW_ClearCurrPair(&u16_ErrorCnt);         
            break;
            default://Any other value in the register means there is a Vial Error.
////         AT91F_PIO_SetOutput (AT91C_BASE_PIOA, AT91SAM7X__PIN__LED_ERROR);// ERROR ON
////         RESCHEDULE(500);            
////         AT91F_PIO_ClearOutput (AT91C_BASE_PIOA, AT91SAM7X__PIN__LED_ERROR);// ERROR OFF
            break;
         }
         RESCHEDULE(500);
      }
      DETACH;
   }
   #endif
}
