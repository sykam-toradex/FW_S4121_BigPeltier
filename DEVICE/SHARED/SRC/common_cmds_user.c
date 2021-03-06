/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------*//**
*
*  \file                common_cmds_user.c
*
*  \brief               This file contains the \em user command functions which will be executed if someone 
*                       writes the appropriate command argument to specified holding register of modbus (MBPB). Execution
*                       of these functions is allowed to owner (manufacturer), service technician \b and user. Further 
*                       details about register bank layout are externally documentated. \n
*                       Every function returns an software base and sub error code in case any error occured.
*                       It is recommended to add new command functions in this file \b only to the end of function pointer
*                       arrays since there exists a documentation about common commands and corresponding modbus addresses.
*
*
*  \author              Ruediger Radtke
*  \version             1.0
*
*  \date                2011-XX-XX     Initial version.
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
*   Copyright(c) 2010-2014 Sykam GmbH. All rights reserved.
*--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

#include "..\..\..\rtl\support.h"

#include "..\..\..\protocol\modbus\mbpbcfg.h"
#include "..\..\..\protocol\modbus\mbpbframe.h"
#include "..\..\..\protocol\modbus\mbpbdata.h"
#include "..\..\..\protocol\modbus\mbpbext.h"

#include "..\..\shared\lib_exceptions_drv\lib_exceptions_drv.h"

#include "lib_exceptions.h"
#include "common_cmds_user.h"
#include "common_support_funcs.h"
#include "common_registers.h"
#include "utils_funcs.h"


//---------------------------------------------- globals definitions ---------------------------------------------------------------------------

// The size and order must be the same like in documentation (excel table)!

tfp_CommonCmdsUser_CMD   fpA_CommonCmdsUser_CMDs[] = 
{
   { &UtilsFuncs_Dummy                          }
};

//---------------------------------------------- externals used in this file -------------------------------------------------------------------

extern   const    struct   _S_LibExceptions_ErrorStringsSW  cSA_LibExceptions_ErrorStringsSW[LIB_EXCEPTIONS__ERROR_STRINGS_SW_STRUCT_ENTRIES_MAX];

extern   const    ushort   cu16_UtilsFuncs_TimeoutShort;

//---------------------------------------------- user command functions ------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------------------------
