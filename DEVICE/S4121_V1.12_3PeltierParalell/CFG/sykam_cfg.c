/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                sykam_cfg.c
*
*  \brief               This file is used to set Sykam device data fields easily.
*
*  \author              rradtke
*  \version             1.0
*
*
*  \date                2011-08-08     Initial version.
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
*   Copyright (c) 2008-2013 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include <AT91SAM7X256.H>

#include "..\..\..\rtl\support.h"

#include "..\..\shared\src\common_registers.h"


//---------------------------------------------- sykam device data -----------------------------------------------------------------------------

                  uchar    u8A_SykamCfg_OEM_DeviceID[20]          =  "S4115";     /**< OEM device identifier */
                  uchar    u8A_SykamCfg_OEM_ID[20]                =  "Sykam";     /**< OEM identifier */
                  uchar    u8A_SykamCfg_Serial[20]                =  "010113";    /**< serial number string, e.g. "010212"
                                                                                      stands for series "01", device "02" and jear 20"12" */
         
                  ushort   u16_SykamCfg_Options                   =  (ushort)COMMON_REGISTERS__COLUMN_OVEN_OPTION_HEAT;//_AND_COOL;
         
                  uchar    u8A_SykamCfg_ManufacturerDeviceID[20]  =  "S4115";     /**< manufacturer device identifier */
                  uchar    u8A_SykamCfg_ManufacturerID[20]        =  "Sykam";     /**< manufacturer identifier */
         
         const    ushort   cu16_SykamCfg_DeviceClass              =  (ushort)COMMON_REGISTERS__DC_COLUMN_OVEN;   /**< device class code */
         
         const    ushort   cu16_SykamCfg_Version                  =  112;         /**< first dezimal digit release and last two bugfix */

//---------------------------------------------- externals defined in this file -------------------------------------------------------------------

extern            uchar    u8A_SykamCfg_OEM_DeviceID[20];
extern            uchar    u8A_SykamCfg_OEM_ID[20];
extern            uchar    u8A_SykamCfg_Serial[20];
extern            ushort   u16_SykamCfg_Options;
extern            uchar    u8A_SykamCfg_ManufacturerDeviceID[20];
extern            uchar    u8A_SykamCfg_ManufacturerID[20];

extern   const    ushort   cu16_SykamCfg_DeviceClass;
extern   const    ushort   cu16_SykamCfg_Version;
