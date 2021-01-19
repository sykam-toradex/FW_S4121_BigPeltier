/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                valve_act_valvetypes.h
*
*  \brief               Header file for \ref valve_act_valvetypes.c
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
*//*---------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//*
*   Copyright(c) 2010-2012 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#ifndef VALVE_ACT_VALVE_TYPES__H
#define VALVE_ACT_VALVE_TYPES__H


//---------------------------------------------- defines ---------------------------------------------------------------------------------------

#define VALVE_ACT_VALVE_TYPES__MAX_VALVE_POSITIONS   (10)         /**<  max number of possible positions (include home position); 
                                                                        this is the length of u8A_ValvePositions */
#define VALVE_ACT_VALVE_TYPES__MAX_VALVE_ID_LENGTH   (6)

//---------------------------------------------- enums -------------------------------------------------------------------------------

enum _E_ValveActValveTypes_SykamValves
{
   VALVE_ACT_VALVE_TYPES__S6X10_ANAL_INJECT_VALVE_3x2,            /**<   S6010i -> Steel, S6110i -> Peek */
   VALVE_ACT_VALVE_TYPES__S6X11_PREP_INJECT_VALVE_3x2,            /**<   S6011i -> Steel, S6111i -> Peek */

   VALVE_ACT_VALVE_TYPES__S6X10_ANAL_VALVE_3x2,                   /**<   S6010 -> Steel, S6110 -> Peek */
   VALVE_ACT_VALVE_TYPES__S6X11_PREP_VALVE_3x2,                   /**<   S6011 -> Steel, S6111 -> Peek */

   VALVE_ACT_VALVE_TYPES__S6X30_ANAL_VALVE_2X3,                   /**<   S6030 -> Steel, S6130 -> Peek */
   VALVE_ACT_VALVE_TYPES__S6X31_PREP_VALVE_2X3,                   /**<   S6031 -> Steel, S6131 -> Peek */

   VALVE_ACT_VALVE_TYPES__S6X40_ANAL_VALVE_1X6,                   /**<   S6040 -> Steel, S6140 -> Peek */
   VALVE_ACT_VALVE_TYPES__S6X41_PREP_VALVE_1X6,                   /**<   S6041 -> Steel, S6141 -> Peek */

   VALVE_ACT_VALVE_TYPES__S6X50_ANAL_VALVE_1X10,                  /**<   S6050 -> Steel, S6150 -> Peek */
   VALVE_ACT_VALVE_TYPES__S6X51_PREP_VALVE_1X10,                  /**<   S6051 -> Steel, S6151 -> Peek */

   VALVE_ACT_VALVE_TYPES__S6X60_ANAL_VALVE_2X10,                  /**<   S6060 -> Steel, S6160 -> Peek */

   VALVE_ACT_VALVE_TYPES__S6070_COLUMN_SELECT_VALVE,              /**<   S6070 */
   // only extend this enum directly after this line and before "VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER"
   VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER
};

//---------------------------------------------- structures ------------------------------------------------------------------------------------

struct _S_ValveActValveTypes_SykamValves
{
   uchar    u8A_ID[VALVE_ACT_VALVE_TYPES__MAX_VALVE_ID_LENGTH];           /**<  valve identification, e.g. "S6010i"    */
   uchar    u8A_Positions[VALVE_ACT_VALVE_TYPES__MAX_VALVE_POSITIONS];    /**<  valid valve positions (edge positions) */
};

//---------------------------------------------- externals defined in this module --------------------------------------------------------------

extern const struct  _S_ValveActValveTypes_SykamValves            cSA_ValveActValveTypes_SykamValves[VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER];
extern const ushort  cu16_ValveActValveTypes_DefaultValveType;


#endif


// docu of variables
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \enum       _E_ValveActValveTypes_SykamValves
*
*  \brief      Thsi enum contains the actual available Sykam valve names. Use the names to access corresponding entry 
*              of \ref cSA_SykamValve, e.g. cSA_SykamValve[VALVE_ACT_VALVE_TYPES__S6X10_ANAL_INJECT_VALVE_3x2]. 
*              <em>Only extend this enum before \b VALVE_ACT_VALVE_TYPES__SYKAM_VALVES_COUNTER to avoid mixing name and 
*              corresponding positions from valves.</em>
*			
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     _S_ValveActValveTypes_SykamValves
*
*  \brief      Structure which contains the description of available Sykam valves.
*     
*  \param      u8A_ID		               This is the valve type (model) number stored as string. Example: "S6X10" 
*  \param      u8A_Positions              The stored numbers are the encoder gap disk edge numbers in increasing order. Zero (0) at 
*                                         the beginning stands for home position. The numbers increase in the way they mirror the 
*                                         real position. For example edge four (4) is equivalent to valve position of 60 degrees. 
*                                         Edge eight (8) consequently 120 degrees \b but edge three (3) is equivalent to 36 degrees 
*                                         and edge five (5) to 72 degrees. \n 
*                                         Furthermore if the number of available valve positions is less than 
*                                         \ref VALVE_ACT_VALVE_TYPES__MAX_VALVE_POSITIONS, zero padding is and must be done after 
*                                         the highest position number.
*
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
/**--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
*  \struct     cSA_ValveActValveTypes_SykamValves
*
*  \brief      A constant structure array which contains the ID and valid positions for every Sykam valve. See \ref _S_ValveActValveTypes_SykamValves
*              for details.
*              
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
