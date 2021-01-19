/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_valvetypes.c
*
*  \brief               The file contains the current available Sykam valves of \ref _E_ValveActValveTypes_SykamValves arranged 
*                       in a constant structure array \ref cSA_ValveActValveTypes_SykamValves.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2010-07-29     Initial version.
*  \date                2011-02-03     Imported S6510_valves.c.
*  \date                2011-08-12     Renamed file.
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


#include "..\..\..\rtl\support.h"

#include "valve_act_valvetypes.h"


//---------------------------------------------- globals definitions ---------------------------------------------------------------------------

const ushort cu16_ValveActValveTypes_DefaultValveType = (ushort)VALVE_ACT_VALVE_TYPES__S6X10_ANAL_VALVE_3x2;

// Quotation marks ("") initialize a string, i.e. also numbers are stored in ASCII format. Mixed init example: 'S',6,0,1,0                    
const struct _S_ValveActValveTypes_SykamValves cSA_ValveActValveTypes_SykamValves[VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER] = 
{
   {  /* u8A_ID        */   "S6X10i",                    /**< 3x2 analytical injection valves: S6010i -> steel stator, S6110i -> PEEK stator, 6 ports    */ 
      /* u8A_Positions */   0,4,0,0,0,0,0,0,0,0
   },
   {  /* u8A_ID        */   "S6X11i",                    /**< 3x2 preparative injection valves: S6011i -> steel stator, S6111i -> PEEK stator, 6 ports   */
      /* u8A_Positions */   0,4,0,0,0,0,0,0,0,0         
   },
   {  /* u8A_ID        */   "S6X10",                     /**< 3x2 analytical valves: S6010 -> steel stator, S6110 -> PEEK stator, 6 ports                */
      /* u8A_Positions */   0,4,0,0,0,0,0,0,0,0
   },
   {  /* u8A_ID        */   "S6X11",                     /**< 3x2 preparative valves: S6011 -> steel stator, S6111 -> PEEK stator, 6 ports               */
      /* u8A_Positions */   0,4,0,0,0,0,0,0,0,0
   },  
   {  /* u8A_ID        */   "S6X30",                     /**< 2x3 way analytical valves: S6030 -> steel stator, S6130 -> PEEK stator, 6 ports            */
      /* u8A_Positions */   0,4,0,0,0,0,0,0,0,0
   },
   {  /* u8A_ID        */   "S6X31",                     /**< 2x3 way preparative valves:  S6031 -> steel stator, S6131 -> PEEK stator, 6 ports          */
      /* u8A_Positions */   0,4,0,0,0,0,0,0,0,0
   },  
   {  /* u8A_ID        */   "S6X40",                     /**< 1x6 way analytical valves: S6040 -> steel stator, S6140 -> PEEK stator, 6 ports            */
      /* u8A_Positions */   0,4,8,11,14,18,0,0,0,0
   },
   {  /* u8A_ID        */   "S6X41",                     /**< 1x6 way preparative valves: S6041 -> steel stator, S6141 -> PEEK stator, 6 ports           */
      /* u8A_Positions */   0,4,8,11,14,18,0,0,0,0
   },
   {  /* u8A_ID        */   "S6X50",                     /**< 1x10 way analytical valves: S6050 -> steel stator, S6150 -> PEEK stator, 10 ports          */
      /* u8A_Positions */   0,3,5,7,9,11,13,15,17,19
   },
   {  /* u8A_ID        */   "S6X51",                     /**< 1x10 way preparative valves: S6051 -> steel stator, S6151 -> PEEK stator, 10 ports         */
      /* u8A_Positions */   0,3,5,7,9,11,13,15,17,19
   },
   {  /* u8A_ID        */   "S6X60",                     /**< 2x10 way analytical valves: S6060 -> steel stator, S6160 -> PEEK stator, 10 ports          */
      /* u8A_Positions */   0,3,0,0,0,0,0,0,0,0
   },   
   {  /* u8A_ID        */   "S6070",                     /**< column selection valve, 12 ports                                                           */     
      /* u8A_Positions */   0,2,4,6,8,10,0,0,0,0
   },    
};
