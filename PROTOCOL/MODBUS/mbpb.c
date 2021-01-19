//!	data and functions of MBPB (ModBusPortBus)
/**
		\file		mbpb.c
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPB.C
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-07-23 initial version
// 2009-07-29 creating demo
// 2009-07-30 creating demo
// 2009-08-05 implemented complete message notification
// 2009-08-06 added full code for MBPBSerNotifyTo
// 2009-08-06 changed code to use ATOMIC macros where possible
// 2009-08-07 added count parameter for notify functions
// 2009-08-12 added rtu support to MBPBSerNotifyRx
// 2009-08-12 changed translation mode to translation flag
// 2009-08-13 added parser validation function
// 2009-08-19 added request preprocessing support
// 2009-08-20 completed processing for incoming requests
// 2009-08-21 completed preprocessing of tx data for pure dma transmission
// 2009-08-21 completed notification of tx data completely sent out thru port
// 2009-08-26 modified notifyrx to retrieve cr and lf chars for asc mode only
// 2009-08-26 fixed TskMBPBData to limit driver tx requests to driver tx max
// 2009-08-26 moved device address write from parser cook to data task
// 2009-08-26 moved all ser parser functions to mbpbser.c
// 2009-08-27 added void pointer to code function calls to pass pSMBPBApp
// 2009-09-03 added MBPB sign on message
// 2009-09-03 first integration of mbpbtimeoutcalc
// 2009-09-04 changes for new TimeOutCalc function
// 2009-09-04 added input and output translator integration
// 2009-09-09 removed ucPending and ucForward
// 2009-09-09 added ucAddMaskTx and ucAddrForward
// 2009-09-09 added MBPBSerAddrSet and MBPBSerAddrFix to SParser
// 2009-09-09 fixed access to pSMBPBPara of TskMBPBData
// 2009-09-09 fixed call to STranslator's rx in TskMBPBMsg
// 2009-09-25 changed to fit single linked reentrant usart driver model
// 2009-10-01 prepared intertask communication tools for multiple mb ports
// 2009-10-08 changed for new os_mbx_declare_ex with x-dim array support
// 2009-11-06 changed TskMBPBMsg to support ushort msg counters
// 2009-12-18 changed TskMBPBMsg to support true msg counter decrement
// 2010-04-15 moved tidTskMBPBData/Msg to SMBPBApp.SDataRW
// 2010-04-15 renamed MbxEvent to MbxMBPBEvent
// 2010-04-15 added missing init of ...SPrevRW.SSemLock.u16To
// 2010-04-21 added task TskMBPBTrgFilter to filter and forwards trigger msgs
// 2010-04-22 added MBPBTrgWaitDecode for easy pending & decoding of trg msgs
// 2010-04-22 added semaphore to lock STrgMsg structure during update
// 2010-04-23 added MBPBTrgRebuild and MBPBTrgFilterSet
// 2010-04-23 added MBPBTrgFilterStateSet and MBPBTrgFilterStateGet
// 2010-04-23 enhanced address mask match filtering of TskMBPBTrgFilter
// 2010-04-29 changed MBPBTrgWaitDecode to return always the raw message
// 2010-04-29 fixed TskMBPBTrgFilter to match value zero only when option set
// 2010-04-29 optimized TskMBPBTrgFilter value match checking
// 2010-04-30 added lock semaphore for each register type
// 2010-04-30 added MBPBProduct... DescSet, StateSet and AddrSet
// 2010-05-05 moved params of SDriver to SDriver.SConfig
// 2010-05-05 added safety check for mbx in TskMBPBTrgFilter to avoid deadlock
// 2010-05-07 added defines for task priorities
// 2010-05-26 fixed SParser LenFix function pointer check in TskMBPBData
// 2010-06-16 fixed return code for timeout error in MBPBTrgWaitDecode
// 2010-06-18 removed splitted tx feature of TskMBPBData to keep data blocked
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-08-04 fixed inits of semaphore arrays in MBPBInit
// 2010-08-04 moved debug sign on message from TskMBPBData to MBPBInit
// 2010-08-04 renamed SemUsartMsgNew to SemMsgNew & using it by port index yet
// 2010-08-04 added MBPB_MBXCNT_EVENT and MBPB_MBXCNT_TRG
// 2010-08-04 added spi support to MBPBInit
// 2010-08-12 fixed index value for SModeRO[].SParser.LenGet in TskMBPBData
// 2010-08-12 added setting of bAsync driver attribute in MBPBInit
// 2010-08-12 added support of bAsync driver attribute to TskMBPBData
// 2010-08-12 added void pointer arg to all functions of _SMBPBFPTRsAux
// 2010-08-12 added support of SDriver.SMisc funcs to MBPBInit & TskMBPBData
// 2010-08-18 fixed index input in MBPBTrgFilterStateSet (suggestion of rr)
// 2010-08-19 fixed index input in MBPBTrgFilterStateGet
// 2010-08-19 optimized resource looking in MBPBTrgFilterStateSet/Get
// 2010-08-27 removed SDriver.SMisc.PrepareRx for MBPB_EVT_FRAME_SENT event
// 2010-09-02 migrated from ATOMIC... to LOCKED macro and built locked blocks
// 2010-09-08 fixed SParser DataGet function pointer check in TskMBPBData
// 2010-09-23 changed to _u(l)min & _u(l)max intrinsics
// 2010-10-07 added error checking and total return code for MBPBInit
// 2010-11-04 changed default driver options for MBPB_INDEX_RS232
// 2011-07-14 added user validate function pointer argument to MBPBInit
// 2011-08-04 added direct event forwarding feature to TskMBPBData
// 2011-08-10 added support of bRTU and bDisabled of _SConfig to MBPBInit
// 2011-08-10 added support of MBPB_INDEX_RS485 to MBPBInit
// 2011-08-10 added MBPBTmrHandler for flexible timeout support in TskMBPBData
// 2011-08-10 added requester/forwarding/backwarding support to TskMBPBData
// 2011-08-11 removed obsolete AddrFix call in TskMBPBData for MBPB_EVT_FRAME_SEND
// 2011-08-11 changed to direct event forwarding in TskMBPBData for MBPB_EVT_FRAME_RECEIVED
// 2011-08-11 optimized assignment of ucAddr in TskMBPBData for MBPB_EVT_FRAME_SEND
// 2011-08-11 optimized error handling for function pointer calls in TskMBPBData
// 2011-08-11 added support of user translators in MBPBInit for custom protocol emulation
// 2011-08-11 fixed manual raw buffer lock-unlock in TskMBPBData for foreign frames
// 2011-08-17 added range sub filter support for address & value to TskMBPBTrgFilter
// 2011-08-17 added range sub filter support for address & value to MBPBTrgFilterSet
// 2011-08-17 renamed MBPBTrgFilterSet to MBPBTrgFilterSetEx
// 2011-08-17 added wrapper function MBPBTrgFilterSet for compatibility
// 2011-08-17 added check of os_tmr_create in TskMBPBData for MBPB_EVT_FRAME_FORWARD_REQx
// 2011-08-17 added range check in MBPBTmrHandler for received u16Info param
// 2011-08-17 added support of MBPB_TIMER_BASE in MBPBTmrHandler and TskMBPBData
// 2011-08-24 completed port index checking to TskMBPBData for safe array access
// 2011-08-24 added checking of translation pointers for active MBPB_TRANSLATE_MASK
// 2011-08-24 optimized lock-unlock section for data retrival in TskMBPBMsg
// 2011-08-24 added control of translation flag in TskMBPBData for received forward requests
// 2011-08-31 added u16Timeout para to MBPBInit for app adjustment of STrgMsg.SSemLock.u16To
// 2011-10-27 changed type of bRx and bTx of MBPBMsgCtrl from BOOL to bool
// 2011-10-27 changed type of bActive of MBPBTrgFilterSet(Ex) from BOOL to bool
// 2011-10-27 changed type of bActive of MBPBTrgFilterStateSet from BOOL to bool
// 2011-11-30 added digraphs of data for public doxygen documentation
// 2011-12-16 changed MBPBProductDescSet prototype (ucClass -> ulClass)
// 2011-12-16 changed MBPBInit to support 32 bit device mask and var desc length
// 2013-01-24 added calculation of max item counts in MBPBInit
// 2013-01-30 changed last argument of handler call to pSMBPBTskPara in TskMBPBData (execute)
// 2013-02-13 moved buffer free action in TskMBPBData to MBPB_EVT_FRAME_SENT (from ...SEND)
// 2013-04-11 fixed function header's return data type comments
// 2013-04-24 added commands MBPB_CODE_READ/WRITE_GENERAL_REFERENCE in MBPBInit
// 2013-04-24 added support for general reference type to MBPBInit
// 2013-04-25 changed address argument of validate function pointer to U32
// 2016-03-17 fixed translating feature by new call order in TskMBPBMsg
// 2016-03-17 added RESCHEDULE(0) in TskMBPBMsg after each msg to avoid data burst(s)
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

// Includes
#include <limits.h>
#include <AT91SAM7X512.h>
#include <RTL.h>

#include "..\..\..\rtl\support.h"
#include "AT91SAM7X.h"

#include "..\..\cpu\at91\usart\usartext.h"
#include "..\..\cpu\at91\spi\spiext.h"
#include "..\..\cpu\at91\spi\spi.h"

#include "mbpb.h"
#include "mbpbcfg.h"
#include "mbpbmsg.h"
#include "mbpbframe.h"
#include "mbpbdata.h"
#include "mbpbdev.h"
#include "mbpbext.h"
#include "mbpbcode.h"
#include "mbpbser.h"

#include "function\mbpbholding.h"
#include "function\mbpbinput.h"
#include "function\mbpbdisc.h"
#include "function\mbpbcoils.h"
#include "function\mbpbother.h"
#include "function\mbpbgeneral.h"

// Globals
OS_SEM SemMsgNew[MBPB_PORT_COUNT];
   
OS_SEM SemLockPortRW[MBPB_PORT_COUNT];
OS_SEM SemLockModeRW[MBPB_PORT_COUNT];
OS_SEM SemLockAppRW;
OS_SEM SemLockTrgMsg;

OS_SEM SemLockRegHolding;
OS_SEM SemLockRegInput;
OS_SEM SemLockRegCoils;
OS_SEM SemLockRegDiscrete;

OS_SEM SemLockBufferRW[MBPB_PORT_COUNT][MBPB_BUFFER_COUNT];
OS_SEM SemLockPrevRW[MBPB_PORT_COUNT][MBPB_BUFFER_COUNT];

//! State change queue
/**
   data flow thru MbxMBPBEvent
   \dot
   digraph MbxMBPBEvent {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      Tmr [label="RTOS timer"];
      TmrHandler [label="os_tmr_call"];
      
      MBPBTmrHandler [label="MBPBTmrHandler" URL="\ref MBPBTmrHandler"];
      NotifyBitTxDone [label="NotifyBitTxDone\n(e.g. MBPBSerNotifyTxDone)" URL="\ref #_SMBPBFPTRsMsg"];
      NotifyBitTo [label="NotifyBitTo\n(e.g. MBPBSerNotifyTo)" URL="\ref #_SMBPBFPTRsMsg"];
      
      SMbxEvent [label="MbxMBPBEvent" style="filled"];
      TskMBPBData [label="TskMBPBData" style="dashed" shape="ellipse"];
      
      Tmr -> TmrHandler [arrowhead="odot"];
      TmrHandler -> MBPBTmrHandler;
      
      MBPBTmrHandler -> SMbxEvent;
      NotifyBitTxDone -> SMbxEvent;
      NotifyBitTo -> SMbxEvent;
      
      SMbxEvent -> TskMBPBData [arrowhead="odot"];      
   }
   \enddot
*/
os_mbx_declare_ex(MbxMBPBEvent, MBPB_MBXCNT_EVENT, [MBPB_PORT_COUNT]);

//! Register access trigger queue
/**
   data flow thru MbxMBPBTrg
   \dot
   digraph MbxMBPBTrg {
      node [shape="record" fontname="FreeSans" fontsize="10"];
      
      TskMBPBData [label="TskMBPBData" style="dashed" shape="ellipse"];
      
      MBPBRegInput [label="MBPBFuncReadInputRegister"];
      MBPBRegHolding [label="MBPBFuncWriteHoldingRegister\nMBPBFuncWriteMultipleHoldingRegister\nMBPBFuncReadHoldingRegister\nMBPBFuncReadWriteMultipleHoldingRegister"];
      MBPBRegCoils [label="MBPBFuncReadCoils\nMBPBFuncWriteCoil\nMBPBFuncWriteMultipleCoils"];
      MBPBRegDiscrete [label="MBPBFuncReadDiscreteInputs"];
      
      MBPBRegInputCB [label="MBPBRegInputCB" shape="parallelogram" URL="\ref MBPBRegInputCB"];
      MBPBRegHoldingCB [label="MBPBRegHoldingCB" shape="parallelogram" URL="\ref MBPBRegHoldingCB"];
      MBPBRegCoilsCB [label="MBPBRegCoilsCB" shape="parallelogram" URL="\ref MBPBRegCoilsCB"];
      MBPBRegDiscreteCB [label="MBPBRegDiscreteCB" shape="parallelogram" URL="\ref MBPBRegDiscreteCB"];
      
      MBPBRegMsg [label="MBPBRegMsg" URL="\ref MBPBRegMsg"];
      SMbxTrg [label="MbxMBPBTrg" style="filled" URL="\ref _SMBPBTriggerMessage"];
      TskMBPBTrgFilter [label="TskMBPBTrgFilter" shape="ellipse"];
      MBPBTrgWaitDecode [label="MBPBTrgWaitDecode" URL="\ref MBPBTrgWaitDecode"];
      SMbxTsk [label="User Mbx" URL="\ref _SMBPBFilterItem"];
      TskUser [label="User Tsk" style="dashed" shape="ellipse"];
      
      TskMBPBData -> MBPBRegInput [style="dotted"];
      TskMBPBData -> MBPBRegHolding [style="dotted"];
      TskMBPBData -> MBPBRegCoils [style="dotted"];
      TskMBPBData -> MBPBRegDiscrete [style="dotted"];
      
      MBPBRegInput -> MBPBRegInputCB;
      MBPBRegHolding -> MBPBRegHoldingCB;
      MBPBRegCoils -> MBPBRegCoilsCB;
      MBPBRegDiscrete -> MBPBRegDiscreteCB;
      
      MBPBRegInputCB -> MBPBRegMsg;
      MBPBRegHoldingCB -> MBPBRegMsg;
      MBPBRegCoilsCB -> MBPBRegMsg;
      MBPBRegDiscreteCB -> MBPBRegMsg;

      MBPBRegMsg -> SMbxTrg;
      SMbxTrg -> TskMBPBTrgFilter [arrowhead="odot"];
      TskMBPBTrgFilter -> SMbxTsk;
      SMbxTsk -> MBPBTrgWaitDecode [arrowhead="odot"];
      MBPBTrgWaitDecode -> TskUser [arrowhead="odot"];
   }
   \enddot
*/
os_mbx_declare(MbxMBPBTrg, MBPB_MBXCNT_TRG);

struct _SMBPBPara SMBPBPara[MBPB_PORT_COUNT];
struct _SMBPBTskPara SMBPBTskPara[MBPB_PORT_COUNT];

//! raw (binary) buffer
/**
   data flow thru ucBufferRaw
   \dot
   digraph ucBufferRaw {
      node [shape="record" fontname="FreeSans" fontsize="10"];

      NotifyBitRx [label="NotifyBitRx\n(e.g. MBPBSerNotifyRx)" style="dashed" URL="\ref _SMBPBFPTRsMsg"];
      ucBufferRaw [label="ucBufferRaw" style="filled" URL="\ref _SMBPBBufferRW"];
      TskMBPBData [label="TskMBPBData" shape="ellipse"];

      NotifyBitRx -> ucBufferRaw;
      ucBufferRaw -> TskMBPBData;
   }
   \enddot
*/
UCHAR ucBufferRaw[MBPB_PORT_COUNT][MBPB_BUFFER_COUNT][MBPB_SER_PDU_SIZE_MAX];

//! cooked (asc/rtu telegram style) buffer
/**
   data flow thru ucBufferCooked
   \dot
   digraph ucBufferCooked {
      node [shape="record" fontname="FreeSans" fontsize="10"];
            
      ucBufferCooked [label="ucBufferCooked" style="filled" URL="\ref _SMBPBBufferRW"];
      ucBufferRaw [label="ucBufferRaw" URL="\ref _SMBPBBufferRW"];
      TskMBPBData [label="TskMBPBData" shape="ellipse"];
           
      TskMBPBData -> ucBufferRaw;
      ucBufferRaw -> ucBufferCooked [style="dashed"];
   }
   \enddot
*/
UCHAR ucBufferCooked[MBPB_PORT_COUNT][MBPB_BUFFER_COUNT][MBPB_SER_PDU_SIZE_MAX];

struct _SMBPBApp SMBPBApp;

// Stacks
OS_TSK_STACK_USER(StkTskMBPBTrgFilter, MBPB_STACK_TSKMBPBTRGFILTER);
OS_TSK_STACK_USER_EX(StkTskMBPBData, MBPB_STACK_TSKMBPBDATA, [MBPB_PORT_COUNT]);

//----------------------------------------------------------------------------
// bool MBPBInit
//----------------------------------------------------------------------------
bool MBPBInit(struct _SMBPBCfgDriver *pSCfgDriver, bool (*Validate) (UCHAR ucType, U32 u32Address, USHORT *pusValue), U16 u16Timeout)
{  
   bool bResultAll= FALSE;

   bool bResult[MBPB_PORT_COUNT];
   UCHAR ucIdx;
   
   USHORT usCountRegTypeMax;
   USHORT usCountRegTypeMax2;
   
   // Init os controls
   
   os_sem_init(SemLockAppRW, 1);
   os_sem_init(SemLockTrgMsg, 1);
   os_sem_init(SemLockRegHolding, 1);
   os_sem_init(SemLockRegInput, 1);
   os_sem_init(SemLockRegCoils, 1);
   os_sem_init(SemLockRegDiscrete, 1);
   
   for (ucIdx= 0; ucIdx < MBPB_PORT_COUNT; ucIdx++)
   {
      os_sem_init(SemMsgNew[ucIdx], 0);
      
      os_sem_init(SemLockPortRW[ucIdx], 1);
      os_sem_init(SemLockModeRW[ucIdx], 1);
      os_sem_init(SemLockBufferRW[ucIdx][MBPB_BUFFER_RAW], 1);
      os_sem_init(SemLockBufferRW[ucIdx][MBPB_BUFFER_COOKED], 1);
      os_sem_init(SemLockPrevRW[ucIdx], 1);
      
      OS_MBX_INIT_EX(MbxMBPBEvent[ucIdx]);
      
      bResult[ucIdx]= TRUE;
   }
   
   OS_MBX_INIT(MbxMBPBTrg);
   
   // Set up application (common for all portbusses)
   SMBPBApp.SDataRW.SSemLock.oiID= &SemLockAppRW;
   SMBPBApp.SDataRW.SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBApp.SDataRW.STrgMsg.SSemLock.oiID= SemLockTrgMsg;
   SMBPBApp.SDataRW.STrgMsg.SSemLock.u16To= u16Timeout;
   
   SMBPBApp.SDataRW.STrgMsg.SMbxTrg.oiID= MbxMBPBTrg;
   SMBPBApp.SDataRW.STrgMsg.SMbxTrg.u16To= OS_T_FOREVER;
   
   SMBPBApp.SDataRW.SReg.SSemLockHolding.oiID= SemLockRegHolding;
   SMBPBApp.SDataRW.SReg.SSemLockHolding.u16To= OS_T_FOREVER;
   
   SMBPBApp.SDataRW.SReg.SSemLockInput.oiID= SemLockRegInput;
   SMBPBApp.SDataRW.SReg.SSemLockInput.u16To= OS_T_FOREVER;
   
   SMBPBApp.SDataRW.SReg.SSemLockCoils.oiID= SemLockRegCoils;
   SMBPBApp.SDataRW.SReg.SSemLockCoils.u16To= OS_T_FOREVER;
   
   SMBPBApp.SDataRW.SReg.SSemLockDiscrete.oiID= SemLockRegDiscrete;
   SMBPBApp.SDataRW.SReg.SSemLockDiscrete.u16To= OS_T_FOREVER;

   // Default product settings
   SMBPBApp.SDataRW.ucAddr= MBPB_ADDR_MIN;
   SMBPBApp.SDataRW.ERunState= MBPB_RUNSTATE_OFF;

   SMBPBApp.SDataRW.SProduct.ulClass= 0xAB;
   SMBPBApp.SDataRW.SProduct.ucDeviceLen= MBPB_PRODUCT_DEVICE_LENGTH;
   memcpy(SMBPBApp.SDataRW.SProduct.ucDevice, (UCHAR *)"ABCDE", MBPB_PRODUCT_DEVICE_LENGTH);

   // Function(s) of module mbpbholding (NOTE: adjust MBPB_HANDLERS_MAX in mbpbcfg.h when adding more here)
   SMBPBApp.SDataRO.SHandler[ucIdx=0].ucCode= MBPB_CODE_WRITE_REGISTER;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncWriteHoldingRegister;
   
   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_WRITE_MULTIPLE_REGISTERS;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncWriteMultipleHoldingRegister;

   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_READ_HOLDING_REGISTER;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncReadHoldingRegister;
   
   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_READWRITE_MULTIPLE_REGISTERS;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncReadWriteMultipleHoldingRegister;
   
   // Function(s) of module mbpbinput
   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_READ_INPUT_REGISTER;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncReadInputRegister;
   
   // Function(s) of module mbpbdisc
   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_READ_DISCRETE_INPUTS;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncReadDiscreteInputs;
   
   // Function(s) of module mbpbcoils
   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_READ_COILS;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncReadCoils;
   
   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_WRITE_SINGLE_COIL;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncWriteCoil;

   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_WRITE_MULTIPLE_COILS;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncWriteMultipleCoils;
   
   // Function(s) of module mbpbgenref
   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_READ_GENERAL_REFERENCE;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncReadGeneralReference;
   
   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_WRITE_GENERAL_REFERENCE;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncWriteGeneralReference;  
   
   // Function(s) of module mbpbother
   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= MBPB_CODE_OTHER_REPORT_SLAVEID;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= MBPBFuncReportSlaveID;   
   
   // Function list end marking
   SMBPBApp.SDataRO.SHandler[ucIdx  ].ucCode= 0;
   SMBPBApp.SDataRO.SHandler[ucIdx++].Function= NULL;
   
   // User functions
   SMBPBApp.SDataRO.Validate= Validate;
   
   // Calculate maximum item counts for MBPB_MODE_ASC and MBPB_MODE_RTU (up to MBPB_MODE_COUNT)
   for (ucIdx= MBPB_MODE_ASC; ucIdx <= MBPB_MODE_COUNT; ucIdx++)
   {
      // HOLDING
      if (MBPBSerParserMaximum((EMBPBMode)ucIdx, MBPB_REGTYPE_HOLDING, 2, 0, MBPB_REGRW_READ, MBPB_SER_PDU_SIZE_MAX, MBPB_SER_PDU_SIZE_MAX, &usCountRegTypeMax, &usCountRegTypeMax2))
      {
         SMBPBApp.SDataRO.SMaxItems[ucIdx].SHolding.usRead= usCountRegTypeMax;
      }
   
      if (MBPBSerParserMaximum((EMBPBMode)ucIdx, MBPB_REGTYPE_HOLDING, 2, 0, MBPB_REGRW_WRITE, MBPB_SER_PDU_SIZE_MAX, MBPB_SER_PDU_SIZE_MAX, &usCountRegTypeMax, &usCountRegTypeMax2))
      {  
         SMBPBApp.SDataRO.SMaxItems[ucIdx].SHolding.usWrite= usCountRegTypeMax;
      }
   
      if (MBPBSerParserMaximum((EMBPBMode)ucIdx, MBPB_REGTYPE_HOLDING, 2, 2, MBPB_REGRW_READWRITE, MBPB_SER_PDU_SIZE_MAX, MBPB_SER_PDU_SIZE_MAX, &usCountRegTypeMax, &usCountRegTypeMax2))
      {
         SMBPBApp.SDataRO.SMaxItems[ucIdx].SHolding.usReadCombo= usCountRegTypeMax;
         SMBPBApp.SDataRO.SMaxItems[ucIdx].SHolding.usWriteCombo= usCountRegTypeMax2;
      }
      
      // INPUT
      if (MBPBSerParserMaximum((EMBPBMode)ucIdx, MBPB_REGTYPE_INPUT, 2, 0, MBPB_REGRW_READ, MBPB_SER_PDU_SIZE_MAX, MBPB_SER_PDU_SIZE_MAX, &usCountRegTypeMax, &usCountRegTypeMax2))
      {
         SMBPBApp.SDataRO.SMaxItems[ucIdx].SInput.usRead= usCountRegTypeMax;
      }
      
   
      // COILS
      if (MBPBSerParserMaximum((EMBPBMode)ucIdx, MBPB_REGTYPE_COILS, 2, 0, MBPB_REGRW_READ, MBPB_SER_PDU_SIZE_MAX, MBPB_SER_PDU_SIZE_MAX, &usCountRegTypeMax, &usCountRegTypeMax2))
      {
         SMBPBApp.SDataRO.SMaxItems[ucIdx].SCoils.usRead= usCountRegTypeMax;
      }
      
      if (MBPBSerParserMaximum((EMBPBMode)ucIdx, MBPB_REGTYPE_COILS, 2, 0, MBPB_REGRW_WRITE, MBPB_SER_PDU_SIZE_MAX, MBPB_SER_PDU_SIZE_MAX, &usCountRegTypeMax, &usCountRegTypeMax2))
      {
         SMBPBApp.SDataRO.SMaxItems[ucIdx].SCoils.usWrite= usCountRegTypeMax;
      }
      
      // DISCRETE
      if (MBPBSerParserMaximum((EMBPBMode)ucIdx, MBPB_REGTYPE_DISCRETE, 2, 0, MBPB_REGRW_READ, MBPB_SER_PDU_SIZE_MAX, MBPB_SER_PDU_SIZE_MAX, &usCountRegTypeMax, &usCountRegTypeMax2))
      {   
         SMBPBApp.SDataRO.SMaxItems[ucIdx].SDiscrete.usRead= usCountRegTypeMax;
      }
      
      // GENERAL reference
      if (MBPBSerParserMaximum((EMBPBMode)ucIdx, MBPB_REGTYPE_GENERAL, 2, 0, MBPB_REGRW_READ, MBPB_SER_PDU_SIZE_MAX, MBPB_SER_PDU_SIZE_MAX, &usCountRegTypeMax, &usCountRegTypeMax2))
      {
         SMBPBApp.SDataRO.SMaxItems[ucIdx].SGeneral.usRead= usCountRegTypeMax;
      }
   
      if (MBPBSerParserMaximum((EMBPBMode)ucIdx, MBPB_REGTYPE_GENERAL, 2, 0, MBPB_REGRW_WRITE, MBPB_SER_PDU_SIZE_MAX, MBPB_SER_PDU_SIZE_MAX, &usCountRegTypeMax, &usCountRegTypeMax2))
      {  
         SMBPBApp.SDataRO.SMaxItems[ucIdx].SGeneral.usWrite= usCountRegTypeMax;
      }
   }
   
   //----------------------------------------------------------------------------
   // Set up params - RS232
   //----------------------------------------------------------------------------
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.Rx= &UsartRx;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.Tx= &UsartTx;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.Msg= &UsartMsg;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SMisc.Prepare= NULL;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SMisc.PrepareRx= NULL;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SMisc.PrepareTx= NULL;   
   
   // Set up driver params
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.bAsync= TRUE;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.usMaxTx= 0;
   
   if (!pSCfgDriver)
   {
      // Default driver params
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ucUnit= 0;
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ucChannel= 0;
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ulBaud= 19200;
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ulOption= (AT91C_US_USMODE_HWHSH | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT);
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.u16To= OS_T_MS(20);
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.bRTU= FALSE;
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.bDisabled= FALSE;

      // Default translator params
      SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].STranslator.Rx= NULL;
      SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].STranslator.Tx= NULL;      

      SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].STranslator.Rx= NULL;
      SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].STranslator.Tx= NULL;   
   }
   else
   {
      // User driver params
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ucUnit= (pSCfgDriver+ MBPB_INDEX_RS232)->ucUnit;
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ucChannel= (pSCfgDriver+ MBPB_INDEX_RS232)->ucChannel;
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ulBaud= (pSCfgDriver+ MBPB_INDEX_RS232)->ulBaud;
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ulOption= (pSCfgDriver+ MBPB_INDEX_RS232)->ulOption;
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.u16To= (pSCfgDriver+ MBPB_INDEX_RS232)->u16To;
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.bRTU= (pSCfgDriver+ MBPB_INDEX_RS232)->bRTU;
      SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.bDisabled= (pSCfgDriver+ MBPB_INDEX_RS232)->bDisabled;
      
      // User translators      
      SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].STranslator.Rx= (pSCfgDriver+ MBPB_INDEX_RS232)->STranslator[MBPB_MODE_ASC].Rx;
      SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].STranslator.Tx= (pSCfgDriver+ MBPB_INDEX_RS232)->STranslator[MBPB_MODE_ASC].Tx;
      
      SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].STranslator.Rx= (pSCfgDriver+ MBPB_INDEX_RS232)->STranslator[MBPB_MODE_RTU].Rx;
      SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].STranslator.Tx= (pSCfgDriver+ MBPB_INDEX_RS232)->STranslator[MBPB_MODE_RTU].Tx;
   }
      
   // Set up params
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SAux.Prepare= NULL;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SAux.PrepareRx= NULL;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SAux.PrepareTx= NULL;
   
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.ulMsgBitRx= AT91C_US_RXBUFF;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.ulMsgBitTxDone= AT91C_US_TXBUFE;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.ulMsgBitTo= AT91C_US_TIMEOUT;
   
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SSemMsg.oiID= &SemMsgNew[MBPB_INDEX_RS232];
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.SSemMsg.u16To= OS_T_FOREVER;

   //SMBPBPara[MBPB_INDEX_RS232].SPortRO.EModeMax= MBPB_MODE_ASC;
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.EModeMax= MBPB_MODE_RTU;
   
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.ucPortBusID= (MBPB_INDEX_RS232+ 1);
   SMBPBPara[MBPB_INDEX_RS232].SPortRO.bActive= !SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.bDisabled;
   
   SMBPBPara[MBPB_INDEX_RS232].SPortRW.SSemLock.oiID= &SemLockPortRW[MBPB_INDEX_RS232];
   SMBPBPara[MBPB_INDEX_RS232].SPortRW.SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS232].SPortRW.ulMsgMask= 0;
   SMBPBPara[MBPB_INDEX_RS232].SPortRW.SReq.ucPBAddr= 0;
   SMBPBPara[MBPB_INDEX_RS232].SPortRW.SFwd.ucPBAddr= 0;
   SMBPBPara[MBPB_INDEX_RS232].SPortRW.SReq.ucPortBusID= 0;
   SMBPBPara[MBPB_INDEX_RS232].SPortRW.SFwd.oiTmr= 0;
         
   SMBPBPara[MBPB_INDEX_RS232].SPortRW.EMode= (SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.bRTU) ? (MBPB_MODE_RTU) : (MBPB_MODE_ASC);

   // ASC
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.Enable= &MBPBSerParserEnable;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.Start= &MBPBSerParserStart;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.Stop= &MBPBSerParserStop;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.Verify= &MBPBSerParserVerify;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.Cook= &MBPBSerParserCook;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.AddrGet= &MBPBSerAddrGet;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.AddrSet= &MBPBSerAddrSet;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.AddrFix= &MBPBSerAddrFix;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.LenGet= &MBPBSerLenGet;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.LenFix= &MBPBSerLenFix;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.DataGet= &MBPBSerDataGet;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.TimeOutCalc= &MBPBSerTimeOutCalc;
   
   ///---SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].STranslator.Rx= NULL;
   ///---SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].STranslator.Tx= NULL;
 
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SMessage.NotifyBitRx= &MBPBSerNotifyRx;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SMessage.NotifyBitTxDone= &MBPBSerNotifyTxDone;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SMessage.NotifyBitTo= &MBPBSerNotifyTo;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SMbxEvent.oiID= &MbxMBPBEvent[MBPB_INDEX_RS232];
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SMbxEvent.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].SSemLock.oiID= &SemLockModeRW[MBPB_INDEX_RS232];
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].EStateRx= MBPB_STATE_RX_INIT;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].EStateTx= MBPB_STATE_TX_IDLE;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].EState= MBPB_STATE_NOT_INITIALIZED;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].ucTranslating= 0;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].EBytePos= MBPB_BYTE_HIGH_NIBBLE;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].ucCR= MBPB_ASC_DEFAULT_CR;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].ucLF= MBPB_ASC_DEFAULT_LF;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].SPrevRW.SSemLock.oiID= &SemLockPrevRW[MBPB_INDEX_RS232];
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].SPrevRW.SSemLock.u16To= OS_T_FOREVER;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].SPrevRW.ucDataAr[0]= 0;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].SPrevRW.ucDataAr[1]= 0;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].SPrevRW.ucIdx= 0;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].EState= MBPB_STATE_DISABLED;
   
   // RTU
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.Enable= &MBPBSerParserEnable;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.Start= &MBPBSerParserStart;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.Stop= &MBPBSerParserStop;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.Verify= &MBPBSerParserVerify;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.Cook= &MBPBSerParserCook;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.AddrGet= &MBPBSerAddrGet;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.AddrSet= &MBPBSerAddrSet;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.AddrFix= &MBPBSerAddrFix;   
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.LenGet= &MBPBSerLenGet;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.LenFix= &MBPBSerLenFix;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.DataGet= &MBPBSerDataGet;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SParser.TimeOutCalc= &MBPBSerTimeOutCalc;
   
   ///---SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].STranslator.Rx= NULL;
   ///---SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].STranslator.Tx= NULL;   
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SMessage.NotifyBitRx= &MBPBSerNotifyRx;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SMessage.NotifyBitTxDone= &MBPBSerNotifyTxDone;
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SMessage.NotifyBitTo= &MBPBSerNotifyTo;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SMbxEvent.oiID= &MbxMBPBEvent[MBPB_INDEX_RS232];
   SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_RTU].SMbxEvent.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].SSemLock.oiID= &SemLockModeRW[MBPB_INDEX_RS232];
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].EStateRx= MBPB_STATE_RX_INIT;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].EStateTx= MBPB_STATE_TX_IDLE;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].EState= MBPB_STATE_NOT_INITIALIZED;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].ucTranslating= 0;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].EBytePos= MBPB_BYTE_HIGH_NIBBLE;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].ucCR= MBPB_ASC_DEFAULT_CR;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].ucLF= MBPB_ASC_DEFAULT_LF;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].SPrevRW.SSemLock.oiID= &SemLockPrevRW[MBPB_INDEX_RS232];
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].SPrevRW.SSemLock.u16To= OS_T_FOREVER;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].SPrevRW.ucDataAr[0]= 0;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].SPrevRW.ucDataAr[1]= 0;
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].SPrevRW.ucIdx= 0;
   
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].EState= MBPB_STATE_DISABLED;
   
   // Buffer raw
   SMBPBPara[MBPB_INDEX_RS232].SBufferRO[MBPB_BUFFER_RAW].usSize= MBPB_SER_PDU_SIZE_MAX;
   
   SMBPBPara[MBPB_INDEX_RS232].SBufferRW[MBPB_BUFFER_RAW].SSemLock.oiID= &SemLockBufferRW[MBPB_INDEX_RS232][MBPB_BUFFER_RAW];
   SMBPBPara[MBPB_INDEX_RS232].SBufferRW[MBPB_BUFFER_RAW].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS232].SBufferRW[MBPB_BUFFER_RAW].pucData= &ucBufferRaw[MBPB_INDEX_RS232][MBPB_BUFFER_RAW][0];
   SMBPBPara[MBPB_INDEX_RS232].SBufferRW[MBPB_BUFFER_RAW].usPos= 0;
   SMBPBPara[MBPB_INDEX_RS232].SBufferRW[MBPB_BUFFER_RAW].usCnt= 0;
      
   // Buffer cooked
   SMBPBPara[MBPB_INDEX_RS232].SBufferRO[MBPB_BUFFER_COOKED].usSize= MBPB_SER_PDU_SIZE_MAX;
   
   SMBPBPara[MBPB_INDEX_RS232].SBufferRW[MBPB_BUFFER_COOKED].SSemLock.oiID= &SemLockBufferRW[MBPB_INDEX_RS232][MBPB_BUFFER_COOKED];
   SMBPBPara[MBPB_INDEX_RS232].SBufferRW[MBPB_BUFFER_COOKED].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS232].SBufferRW[MBPB_BUFFER_COOKED].pucData= &ucBufferCooked[MBPB_INDEX_RS232][MBPB_BUFFER_COOKED][0];
   SMBPBPara[MBPB_INDEX_RS232].SBufferRW[MBPB_BUFFER_COOKED].usPos= 0;
   SMBPBPara[MBPB_INDEX_RS232].SBufferRW[MBPB_BUFFER_COOKED].usCnt= 0;

   // Calculate timeouts
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_ASC].ulTimeOutMS= MBPBSerTimeOutCalc(MBPB_MODE_ASC, SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ulBaud);
   SMBPBPara[MBPB_INDEX_RS232].SModeRW[MBPB_MODE_RTU].ulTimeOutMS= MBPBSerTimeOutCalc(MBPB_MODE_RTU, SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ulBaud);

   if (!SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.bDisabled)
   {
      // Run functions to prepare (optional)
      if (SMBPBPara[MBPB_INDEX_RS232].SPortRO.SAux.Prepare) SMBPBPara[MBPB_INDEX_RS232].SPortRO.SAux.Prepare(&SMBPBPara[MBPB_INDEX_RS232]);
      if (SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SMisc.Prepare) SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SMisc.Prepare(&SMBPBPara[MBPB_INDEX_RS232]);
      if (SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.Enable) SMBPBPara[MBPB_INDEX_RS232].SModeRO[MBPB_MODE_ASC].SParser.Enable(&SMBPBPara[MBPB_INDEX_RS232]);
     
      // Init driver
      if (bResult[MBPB_INDEX_RS232])
      {
         bResult[MBPB_INDEX_RS232] &= UsartInit(SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ucUnit, SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ucChannel, SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ulBaud, SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ulOption, SMBPBPara[MBPB_INDEX_RS232].SModeRW[SMBPBPara[MBPB_INDEX_RS232].SPortRW.EMode].ulTimeOutMS, AT91C_US_RETTO, SMBPBPara[MBPB_INDEX_RS232].SPortRO.SSemMsg.oiID, 0xFFFFFFFF, 0);
         if (bResult[MBPB_INDEX_RS232])
         {
            // Ignore send errors here
            UsartTx(SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ucUnit, SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ucChannel, "Usart ready.\r\n", 14, u16Timeout);
            UsartTx(SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ucUnit, SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.ucChannel, "MBPB at Usart ready.\r\n", 22, u16Timeout);
         }
      }
   }
   
   // Link all para
   SMBPBTskPara[MBPB_INDEX_RS232].pSMBPBPara= &SMBPBPara[MBPB_INDEX_RS232];
   SMBPBTskPara[MBPB_INDEX_RS232].pSMBPBApp= &SMBPBApp;

   //----------------------------------------------------------------------------
   // Set up params - RS485
   //----------------------------------------------------------------------------
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.Rx= &UsartRx;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.Tx= &UsartTx;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.Msg= &UsartMsg;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SMisc.Prepare= NULL;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SMisc.PrepareRx= NULL;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SMisc.PrepareTx= NULL;   
   
   // Set up driver params
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.bAsync= TRUE;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.usMaxTx= 0;
   
   if (!pSCfgDriver)
   {
      // Default driver params
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ucUnit= 1;
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ucChannel= 0;
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ulBaud= 19200;
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ulOption= (AT91C_US_USMODE_RS485 | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE | AT91C_US_NBSTOP_1_BIT);
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.u16To= OS_T_MS(20);
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.bRTU= FALSE;
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.bDisabled= FALSE;
      
      // Default translator params
      SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].STranslator.Rx= NULL;
      SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].STranslator.Tx= NULL;      

      SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].STranslator.Rx= NULL;
      SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].STranslator.Tx= NULL;   
   }
   else
   {
      // User driver params
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ucUnit= (pSCfgDriver+ MBPB_INDEX_RS485)->ucUnit;
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ucChannel= (pSCfgDriver+ MBPB_INDEX_RS485)->ucChannel;
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ulBaud= (pSCfgDriver+ MBPB_INDEX_RS485)->ulBaud;
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ulOption= (pSCfgDriver+ MBPB_INDEX_RS485)->ulOption;
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.u16To= (pSCfgDriver+ MBPB_INDEX_RS485)->u16To;
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.bRTU= (pSCfgDriver+ MBPB_INDEX_RS485)->bRTU;
      SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.bDisabled= (pSCfgDriver+ MBPB_INDEX_RS485)->bDisabled;

      // User translators      
      SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].STranslator.Rx= (pSCfgDriver+ MBPB_INDEX_RS485)->STranslator[MBPB_MODE_ASC].Rx;
      SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].STranslator.Tx= (pSCfgDriver+ MBPB_INDEX_RS485)->STranslator[MBPB_MODE_ASC].Tx;
      
      SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].STranslator.Rx= (pSCfgDriver+ MBPB_INDEX_RS485)->STranslator[MBPB_MODE_RTU].Rx;
      SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].STranslator.Tx= (pSCfgDriver+ MBPB_INDEX_RS485)->STranslator[MBPB_MODE_RTU].Tx;
   }
      
   // Set up params
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SAux.Prepare= NULL;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SAux.PrepareRx= NULL;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SAux.PrepareTx= NULL;
   
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.ulMsgBitRx= AT91C_US_RXBUFF;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.ulMsgBitTxDone= AT91C_US_TXBUFE;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.ulMsgBitTo= AT91C_US_TIMEOUT;
   
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SSemMsg.oiID= &SemMsgNew[MBPB_INDEX_RS485];
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.SSemMsg.u16To= OS_T_FOREVER;

   //SMBPBPara[MBPB_INDEX_RS485].SPortRO.EModeMax= MBPB_MODE_ASC;
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.EModeMax= MBPB_MODE_RTU;
   
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.ucPortBusID= (MBPB_INDEX_RS485+ 1);
   SMBPBPara[MBPB_INDEX_RS485].SPortRO.bActive= !SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.bDisabled;
   
   SMBPBPara[MBPB_INDEX_RS485].SPortRW.SSemLock.oiID= &SemLockPortRW[MBPB_INDEX_RS485];
   SMBPBPara[MBPB_INDEX_RS485].SPortRW.SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS485].SPortRW.ulMsgMask= 0;
   SMBPBPara[MBPB_INDEX_RS485].SPortRW.SReq.ucPBAddr= 0;
   SMBPBPara[MBPB_INDEX_RS485].SPortRW.SFwd.ucPBAddr= 0;
   SMBPBPara[MBPB_INDEX_RS485].SPortRW.SReq.ucPortBusID= 0;
   SMBPBPara[MBPB_INDEX_RS485].SPortRW.SFwd.oiTmr= 0;
         
   SMBPBPara[MBPB_INDEX_RS485].SPortRW.EMode= (SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.bRTU) ? (MBPB_MODE_RTU) : (MBPB_MODE_ASC);

   // ASC
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.Enable= &MBPBSerParserEnable;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.Start= &MBPBSerParserStart;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.Stop= &MBPBSerParserStop;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.Verify= &MBPBSerParserVerify;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.Cook= &MBPBSerParserCook;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.AddrGet= &MBPBSerAddrGet;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.AddrSet= &MBPBSerAddrSet;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.AddrFix= &MBPBSerAddrFix;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.LenGet= &MBPBSerLenGet;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.LenFix= &MBPBSerLenFix;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.DataGet= &MBPBSerDataGet;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.TimeOutCalc= &MBPBSerTimeOutCalc;
   
   ///---SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].STranslator.Rx= NULL;
   ///---SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].STranslator.Tx= NULL;
 
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SMessage.NotifyBitRx= &MBPBSerNotifyRx;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SMessage.NotifyBitTxDone= &MBPBSerNotifyTxDone;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SMessage.NotifyBitTo= &MBPBSerNotifyTo;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SMbxEvent.oiID= &MbxMBPBEvent[MBPB_INDEX_RS485];
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SMbxEvent.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].SSemLock.oiID= &SemLockModeRW[MBPB_INDEX_RS485];
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].EStateRx= MBPB_STATE_RX_INIT;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].EStateTx= MBPB_STATE_TX_IDLE;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].EState= MBPB_STATE_NOT_INITIALIZED;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].ucTranslating= 0;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].EBytePos= MBPB_BYTE_HIGH_NIBBLE;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].ucCR= MBPB_ASC_DEFAULT_CR;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].ucLF= MBPB_ASC_DEFAULT_LF;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].SPrevRW.SSemLock.oiID= &SemLockPrevRW[MBPB_INDEX_RS485];
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].SPrevRW.SSemLock.u16To= OS_T_FOREVER;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].SPrevRW.ucDataAr[0]= 0;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].SPrevRW.ucDataAr[1]= 0;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].SPrevRW.ucIdx= 0;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].EState= MBPB_STATE_DISABLED;
   
   // RTU
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.Enable= &MBPBSerParserEnable;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.Start= &MBPBSerParserStart;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.Stop= &MBPBSerParserStop;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.Verify= &MBPBSerParserVerify;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.Cook= &MBPBSerParserCook;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.AddrGet= &MBPBSerAddrGet;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.AddrSet= &MBPBSerAddrSet;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.AddrFix= &MBPBSerAddrFix;   
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.LenGet= &MBPBSerLenGet;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.LenFix= &MBPBSerLenFix;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.DataGet= &MBPBSerDataGet;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SParser.TimeOutCalc= &MBPBSerTimeOutCalc;
   
   ///---SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].STranslator.Rx= NULL;
   ///---SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].STranslator.Tx= NULL;   
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SMessage.NotifyBitRx= &MBPBSerNotifyRx;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SMessage.NotifyBitTxDone= &MBPBSerNotifyTxDone;
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SMessage.NotifyBitTo= &MBPBSerNotifyTo;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SMbxEvent.oiID= &MbxMBPBEvent[MBPB_INDEX_RS485];
   SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_RTU].SMbxEvent.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].SSemLock.oiID= &SemLockModeRW[MBPB_INDEX_RS485];
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].EStateRx= MBPB_STATE_RX_INIT;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].EStateTx= MBPB_STATE_TX_IDLE;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].EState= MBPB_STATE_NOT_INITIALIZED;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].ucTranslating= 0;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].EBytePos= MBPB_BYTE_HIGH_NIBBLE;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].ucCR= MBPB_ASC_DEFAULT_CR;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].ucLF= MBPB_ASC_DEFAULT_LF;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].SPrevRW.SSemLock.oiID= &SemLockPrevRW[MBPB_INDEX_RS485];
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].SPrevRW.SSemLock.u16To= OS_T_FOREVER;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].SPrevRW.ucDataAr[0]= 0;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].SPrevRW.ucDataAr[1]= 0;
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].SPrevRW.ucIdx= 0;
   
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].EState= MBPB_STATE_DISABLED;
   
   // Buffer raw
   SMBPBPara[MBPB_INDEX_RS485].SBufferRO[MBPB_BUFFER_RAW].usSize= MBPB_SER_PDU_SIZE_MAX;
   
   SMBPBPara[MBPB_INDEX_RS485].SBufferRW[MBPB_BUFFER_RAW].SSemLock.oiID= &SemLockBufferRW[MBPB_INDEX_RS485][MBPB_BUFFER_RAW];
   SMBPBPara[MBPB_INDEX_RS485].SBufferRW[MBPB_BUFFER_RAW].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS485].SBufferRW[MBPB_BUFFER_RAW].pucData= &ucBufferRaw[MBPB_INDEX_RS485][MBPB_BUFFER_RAW][0];
   SMBPBPara[MBPB_INDEX_RS485].SBufferRW[MBPB_BUFFER_RAW].usPos= 0;
   SMBPBPara[MBPB_INDEX_RS485].SBufferRW[MBPB_BUFFER_RAW].usCnt= 0;
      
   // Buffer cooked
   SMBPBPara[MBPB_INDEX_RS485].SBufferRO[MBPB_BUFFER_COOKED].usSize= MBPB_SER_PDU_SIZE_MAX;
   
   SMBPBPara[MBPB_INDEX_RS485].SBufferRW[MBPB_BUFFER_COOKED].SSemLock.oiID= &SemLockBufferRW[MBPB_INDEX_RS485][MBPB_BUFFER_COOKED];
   SMBPBPara[MBPB_INDEX_RS485].SBufferRW[MBPB_BUFFER_COOKED].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_RS485].SBufferRW[MBPB_BUFFER_COOKED].pucData= &ucBufferCooked[MBPB_INDEX_RS485][MBPB_BUFFER_COOKED][0];
   SMBPBPara[MBPB_INDEX_RS485].SBufferRW[MBPB_BUFFER_COOKED].usPos= 0;
   SMBPBPara[MBPB_INDEX_RS485].SBufferRW[MBPB_BUFFER_COOKED].usCnt= 0;

   // Calculate timeouts
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_ASC].ulTimeOutMS= MBPBSerTimeOutCalc(MBPB_MODE_ASC, SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ulBaud);
   SMBPBPara[MBPB_INDEX_RS485].SModeRW[MBPB_MODE_RTU].ulTimeOutMS= MBPBSerTimeOutCalc(MBPB_MODE_RTU, SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ulBaud);

   if (!SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.bDisabled)
   {
      // Run functions to prepare (optional)
      if (SMBPBPara[MBPB_INDEX_RS485].SPortRO.SAux.Prepare) SMBPBPara[MBPB_INDEX_RS485].SPortRO.SAux.Prepare(&SMBPBPara[MBPB_INDEX_RS485]);
      if (SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SMisc.Prepare) SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SMisc.Prepare(&SMBPBPara[MBPB_INDEX_RS485]);
      if (SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.Enable) SMBPBPara[MBPB_INDEX_RS485].SModeRO[MBPB_MODE_ASC].SParser.Enable(&SMBPBPara[MBPB_INDEX_RS485]);
   
      // Init driver
      if (bResult[MBPB_INDEX_RS485])
      {
         bResult[MBPB_INDEX_RS485] &= UsartInit(SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ucUnit, SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ucChannel, SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ulBaud, SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.ulOption, SMBPBPara[MBPB_INDEX_RS485].SModeRW[SMBPBPara[MBPB_INDEX_RS485].SPortRW.EMode].ulTimeOutMS, AT91C_US_RETTO, SMBPBPara[MBPB_INDEX_RS485].SPortRO.SSemMsg.oiID, 0xFFFFFFFF, 0);
      }
   }
   
   // Link all para
   SMBPBTskPara[MBPB_INDEX_RS485].pSMBPBPara= &SMBPBPara[MBPB_INDEX_RS485];
   SMBPBTskPara[MBPB_INDEX_RS485].pSMBPBApp= &SMBPBApp;

   //----------------------------------------------------------------------------
   // Set up params - SPI
   //----------------------------------------------------------------------------
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.Rx= &SpiRx;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.Tx= &SpiTx;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.Msg= &SpiMsgChannel;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SMisc.Prepare= NULL;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SMisc.PrepareRx= &MBPBSerSyncPrepareRx;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SMisc.PrepareTx= NULL; 

   // Set up driver params
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.bAsync= FALSE;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.usMaxTx= SPI_PDCMAX;
   
   if (!pSCfgDriver)
   {
      // Default driver params
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ucUnit= 1;
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ucChannel= 0;      
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ulBaud= 0;
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ulOption= (AT91C_SPI_BITS_8 | AT91C_SPI_NCPHA);
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.u16To= OS_T_MS(20);
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.bRTU= FALSE;
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.bDisabled= FALSE;
      
      // Default translator params
      SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].STranslator.Rx= NULL;
      SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].STranslator.Tx= NULL;      

      SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].STranslator.Rx= NULL;
      SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].STranslator.Tx= NULL;  
   }
   else
   {
      // User driver params
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ucUnit= (pSCfgDriver+ MBPB_INDEX_SPI)->ucUnit;
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ucChannel= (pSCfgDriver+ MBPB_INDEX_SPI)->ucChannel;
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ulBaud= (pSCfgDriver+ MBPB_INDEX_SPI)->ulBaud;
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ulOption= (pSCfgDriver+ MBPB_INDEX_SPI)->ulOption;
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.u16To= (pSCfgDriver+ MBPB_INDEX_SPI)->u16To;
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.bRTU= (pSCfgDriver+ MBPB_INDEX_SPI)->bRTU;
      SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.bDisabled= (pSCfgDriver+ MBPB_INDEX_SPI)->bDisabled;
      
      // User translators      
      SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].STranslator.Rx= (pSCfgDriver+ MBPB_INDEX_SPI)->STranslator[MBPB_MODE_ASC].Rx;
      SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].STranslator.Tx= (pSCfgDriver+ MBPB_INDEX_SPI)->STranslator[MBPB_MODE_ASC].Tx;
      
      SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].STranslator.Rx= (pSCfgDriver+ MBPB_INDEX_SPI)->STranslator[MBPB_MODE_RTU].Rx;
      SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].STranslator.Tx= (pSCfgDriver+ MBPB_INDEX_SPI)->STranslator[MBPB_MODE_RTU].Tx;
   }

   // Set up params
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SAux.Prepare= NULL;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SAux.PrepareRx= NULL;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SAux.PrepareTx= NULL;
   
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.ulMsgBitRx= AT91C_SPI_RXBUFF;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.ulMsgBitTxDone= AT91C_SPI_TXBUFE;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.ulMsgBitTo= AT91C_SPI_NSSR;
   
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SSemMsg.oiID= &SemMsgNew[MBPB_INDEX_SPI];
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.SSemMsg.u16To= OS_T_FOREVER;

   //SMBPBPara[MBPB_INDEX_SPI].SPortRO.EModeMax= MBPB_MODE_ASC;
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.EModeMax= MBPB_MODE_RTU;
   
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.ucPortBusID= (MBPB_INDEX_SPI+ 1);
   SMBPBPara[MBPB_INDEX_SPI].SPortRO.bActive= !SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.bDisabled;
   
   SMBPBPara[MBPB_INDEX_SPI].SPortRW.SSemLock.oiID= &SemLockPortRW[MBPB_INDEX_SPI];
   SMBPBPara[MBPB_INDEX_SPI].SPortRW.SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_SPI].SPortRW.ulMsgMask= 0;
   SMBPBPara[MBPB_INDEX_SPI].SPortRW.SReq.ucPBAddr= 0;
   SMBPBPara[MBPB_INDEX_SPI].SPortRW.SFwd.ucPBAddr= 0;
   SMBPBPara[MBPB_INDEX_SPI].SPortRW.SReq.ucPortBusID= 0;
   SMBPBPara[MBPB_INDEX_SPI].SPortRW.SFwd.oiTmr= 0;

   SMBPBPara[MBPB_INDEX_SPI].SPortRW.EMode= (SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.bRTU) ? (MBPB_MODE_RTU) : (MBPB_MODE_ASC);
 
   // ASC
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.Enable= &MBPBSerParserEnable;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.Start= &MBPBSerParserStart;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.Stop= &MBPBSerParserStop;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.Verify= &MBPBSerParserVerify;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.Cook= &MBPBSerParserCook;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.AddrGet= &MBPBSerAddrGet;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.AddrSet= &MBPBSerAddrSet;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.AddrFix= &MBPBSerAddrFix;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.LenGet= &MBPBSerLenGet;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.LenFix= &MBPBSerLenFix;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.DataGet= &MBPBSerDataGet;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.TimeOutCalc= &MBPBSerTimeOutCalc;
   
   ///---SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].STranslator.Rx= NULL;
   ///---SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].STranslator.Tx= NULL;
 
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SMessage.NotifyBitRx= &MBPBSerNotifyRx;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SMessage.NotifyBitTxDone= &MBPBSerNotifyTxDone;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SMessage.NotifyBitTo= &MBPBSerNotifyTo;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SMbxEvent.oiID= &MbxMBPBEvent[MBPB_INDEX_SPI];
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SMbxEvent.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].SSemLock.oiID= &SemLockModeRW[MBPB_INDEX_SPI];
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].EStateRx= MBPB_STATE_RX_INIT;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].EStateTx= MBPB_STATE_TX_IDLE;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].EState= MBPB_STATE_NOT_INITIALIZED;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].ucTranslating= 0;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].EBytePos= MBPB_BYTE_HIGH_NIBBLE;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].ucCR= MBPB_ASC_DEFAULT_CR;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].ucLF= MBPB_ASC_DEFAULT_LF;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].SPrevRW.SSemLock.oiID= &SemLockPrevRW[MBPB_INDEX_SPI];
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].SPrevRW.SSemLock.u16To= OS_T_FOREVER;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].SPrevRW.ucDataAr[0]= 0;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].SPrevRW.ucDataAr[1]= 0;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].SPrevRW.ucIdx= 0;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].EState= MBPB_STATE_DISABLED;
     
   // RTU
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.Enable= &MBPBSerParserEnable;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.Start= &MBPBSerParserStart;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.Stop= &MBPBSerParserStop;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.Verify= &MBPBSerParserVerify;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.Cook= &MBPBSerParserCook;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.AddrGet= &MBPBSerAddrGet;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.AddrSet= &MBPBSerAddrSet;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.AddrFix= &MBPBSerAddrFix;   
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.LenGet= &MBPBSerLenGet;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.LenFix= &MBPBSerLenFix;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.DataGet= &MBPBSerDataGet;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SParser.TimeOutCalc= &MBPBSerTimeOutCalc;
   
   ///---SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].STranslator.Rx= NULL;
   ///---SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].STranslator.Tx= NULL;   
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SMessage.NotifyBitRx= &MBPBSerNotifyRx;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SMessage.NotifyBitTxDone= &MBPBSerNotifyTxDone;
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SMessage.NotifyBitTo= &MBPBSerNotifyTo;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SMbxEvent.oiID= &MbxMBPBEvent[MBPB_INDEX_SPI];
   SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_RTU].SMbxEvent.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].SSemLock.oiID= &SemLockModeRW[MBPB_INDEX_SPI];
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].EStateRx= MBPB_STATE_RX_INIT;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].EStateTx= MBPB_STATE_TX_IDLE;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].EState= MBPB_STATE_NOT_INITIALIZED;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].ucTranslating= 0;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].EBytePos= MBPB_BYTE_HIGH_NIBBLE;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].ucCR= MBPB_ASC_DEFAULT_CR;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].ucLF= MBPB_ASC_DEFAULT_LF;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].SPrevRW.SSemLock.oiID= &SemLockPrevRW[MBPB_INDEX_SPI];
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].SPrevRW.SSemLock.u16To= OS_T_FOREVER;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].SPrevRW.ucDataAr[0]= 0;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].SPrevRW.ucDataAr[1]= 0;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].SPrevRW.ucIdx= 0;
   
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].EState= MBPB_STATE_DISABLED;
   
   // Buffer raw
   SMBPBPara[MBPB_INDEX_SPI].SBufferRO[MBPB_BUFFER_RAW].usSize= MBPB_SER_PDU_SIZE_MAX;
   
   SMBPBPara[MBPB_INDEX_SPI].SBufferRW[MBPB_BUFFER_RAW].SSemLock.oiID= &SemLockBufferRW[MBPB_INDEX_SPI][MBPB_BUFFER_RAW];
   SMBPBPara[MBPB_INDEX_SPI].SBufferRW[MBPB_BUFFER_RAW].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_SPI].SBufferRW[MBPB_BUFFER_RAW].pucData= &ucBufferRaw[MBPB_INDEX_SPI][MBPB_BUFFER_RAW][0];
   SMBPBPara[MBPB_INDEX_SPI].SBufferRW[MBPB_BUFFER_RAW].usPos= 0;
   SMBPBPara[MBPB_INDEX_SPI].SBufferRW[MBPB_BUFFER_RAW].usCnt= 0;
      
   // Buffer cooked
   SMBPBPara[MBPB_INDEX_SPI].SBufferRO[MBPB_BUFFER_COOKED].usSize= MBPB_SER_PDU_SIZE_MAX;
   
   SMBPBPara[MBPB_INDEX_SPI].SBufferRW[MBPB_BUFFER_COOKED].SSemLock.oiID= &SemLockBufferRW[MBPB_INDEX_SPI][MBPB_BUFFER_COOKED];
   SMBPBPara[MBPB_INDEX_SPI].SBufferRW[MBPB_BUFFER_COOKED].SSemLock.u16To= OS_T_FOREVER;
   
   SMBPBPara[MBPB_INDEX_SPI].SBufferRW[MBPB_BUFFER_COOKED].pucData= &ucBufferCooked[MBPB_INDEX_SPI][MBPB_BUFFER_COOKED][0];
   SMBPBPara[MBPB_INDEX_SPI].SBufferRW[MBPB_BUFFER_COOKED].usPos= 0;
   SMBPBPara[MBPB_INDEX_SPI].SBufferRW[MBPB_BUFFER_COOKED].usCnt= 0;

   // Clear timeouts (spi doesn't support timeout msg)
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_ASC].ulTimeOutMS= 0;
   SMBPBPara[MBPB_INDEX_SPI].SModeRW[MBPB_MODE_RTU].ulTimeOutMS= 0;

   if (!SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.bDisabled)
   {
      // Run functions to prepare (optional)
      if (SMBPBPara[MBPB_INDEX_SPI].SPortRO.SAux.Prepare) SMBPBPara[MBPB_INDEX_SPI].SPortRO.SAux.Prepare(&SMBPBPara[MBPB_INDEX_SPI]);
      if (SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SMisc.Prepare) SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SMisc.Prepare(&SMBPBPara[MBPB_INDEX_SPI]);
      if (SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.Enable) SMBPBPara[MBPB_INDEX_SPI].SModeRO[MBPB_MODE_ASC].SParser.Enable(&SMBPBPara[MBPB_INDEX_SPI]);
   
      // Init driver
      if (bResult[MBPB_INDEX_SPI])
      {
         bResult[MBPB_INDEX_SPI] &= SpiInitChannel(SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ucUnit, SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ucChannel, SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.ulOption, USHRT_MAX, SMBPBPara[MBPB_INDEX_SPI].SModeRW[SMBPBPara[MBPB_INDEX_SPI].SPortRW.EMode].ulTimeOutMS, SPI_P_NORMAL, SPI_CHANNEL_FLAGS_CSRUPDATE, SMBPBPara[MBPB_INDEX_SPI].SPortRO.SSemMsg.oiID, 0xFFFFFFFF);
      }
   }
   
   // Link all para
   SMBPBTskPara[MBPB_INDEX_SPI].pSMBPBPara= &SMBPBPara[MBPB_INDEX_SPI];
   SMBPBTskPara[MBPB_INDEX_SPI].pSMBPBApp= &SMBPBApp;
        
   // Any successfull ports yet?
   for (bResultAll= FALSE, ucIdx= 0; (ucIdx < MBPB_PORT_COUNT) && (!bResultAll); ucIdx++)
   {
      bResultAll |= bResult[ucIdx];
   }
   
   // Run common tasks (for at least one successfull port)
   if (bResultAll)
   {
      SMBPBApp.SDataRW.tidTskTrg= OS_TSK_CREATE_USER(TskMBPBTrgFilter, MBPB_TSKTRGFILTERPRI, StkTskMBPBTrgFilter);
      bResultAll &= (SMBPBApp.SDataRW.tidTskTrg != 0);
   }
   
   // Run port tasks (common tasks required)
   if (bResultAll)
   {
      // Run port tasks - RS232
      if (!SMBPBPara[MBPB_INDEX_RS232].SPortRO.SDriver.SConfig.bDisabled)
      {
         if (bResult[MBPB_INDEX_RS232])
         {
            SMBPBApp.SDataRW.tidTskData[MBPB_INDEX_RS232]= OS_TSK_CREATE_USER_EX(TskMBPBData, MBPB_TSKDATAPRI, StkTskMBPBData[MBPB_INDEX_RS232], &SMBPBTskPara[MBPB_INDEX_RS232]);
            bResult[MBPB_INDEX_RS232] &= (SMBPBApp.SDataRW.tidTskData[MBPB_INDEX_RS232] != 0);
            
            if (bResult[MBPB_INDEX_RS232])
            {
               SMBPBApp.SDataRW.tidTskMsg[MBPB_INDEX_RS232]= os_tsk_create_ex(TskMBPBMsg, MBPB_TSKMSGPRI, &SMBPBTskPara[MBPB_INDEX_RS232]);
               bResult[MBPB_INDEX_RS232] &= (SMBPBApp.SDataRW.tidTskMsg[MBPB_INDEX_RS232] != 0);
            }
         }
      }
      
      // Run port tasks - RS485
      if (!SMBPBPara[MBPB_INDEX_RS485].SPortRO.SDriver.SConfig.bDisabled)
      {
         if (bResult[MBPB_INDEX_RS485])
         {
            SMBPBApp.SDataRW.tidTskData[MBPB_INDEX_RS485]= OS_TSK_CREATE_USER_EX(TskMBPBData, MBPB_TSKDATAPRI, StkTskMBPBData[MBPB_INDEX_RS485], &SMBPBTskPara[MBPB_INDEX_RS485]);
            bResult[MBPB_INDEX_RS485] &= (SMBPBApp.SDataRW.tidTskData[MBPB_INDEX_RS485] != 0);
            
            if (bResult[MBPB_INDEX_RS485])
            {
               SMBPBApp.SDataRW.tidTskMsg[MBPB_INDEX_RS485]= os_tsk_create_ex(TskMBPBMsg, MBPB_TSKMSGPRI, &SMBPBTskPara[MBPB_INDEX_RS485]);
               bResult[MBPB_INDEX_RS485] &= (SMBPBApp.SDataRW.tidTskMsg[MBPB_INDEX_RS485] != 0);
            }
         }
      }
       
      // Run port tasks - SPI
      if (!SMBPBPara[MBPB_INDEX_SPI].SPortRO.SDriver.SConfig.bDisabled)
      {
         if (bResult[MBPB_INDEX_SPI])
         {
            SMBPBApp.SDataRW.tidTskData[MBPB_INDEX_SPI]= OS_TSK_CREATE_USER_EX(TskMBPBData, MBPB_TSKDATAPRI, StkTskMBPBData[MBPB_INDEX_SPI], &SMBPBTskPara[MBPB_INDEX_SPI]);
            bResult[MBPB_INDEX_SPI] &= (SMBPBApp.SDataRW.tidTskData[MBPB_INDEX_SPI] != 0);
            
            if (bResult[MBPB_INDEX_SPI])
            {
               SMBPBApp.SDataRW.tidTskMsg[MBPB_INDEX_SPI]= os_tsk_create_ex(TskMBPBMsg, MBPB_TSKMSGPRI, &SMBPBTskPara[MBPB_INDEX_SPI]);
               bResult[MBPB_INDEX_SPI] &= (SMBPBApp.SDataRW.tidTskMsg[MBPB_INDEX_SPI] != 0);
            
               if (bResult[MBPB_INDEX_SPI])
               {
                  // Manually trigger timeout (spi doesn't support timeout msg) to switch to ready state
                  MBPBSerNotifyTo(&SMBPBPara[MBPB_INDEX_SPI], 1);
               }
            }
         }
      }
   }
   
   // Build global result
   for (bResultAll= TRUE, ucIdx= 0; (ucIdx < MBPB_PORT_COUNT) && (bResultAll); ucIdx++)
   {
      bResultAll &= bResult[ucIdx];
   }
   
   return (bResultAll);
}

//----------------------------------------------------------------------------
// bool MBPBProductDescSet
//----------------------------------------------------------------------------
bool MBPBProductDescSet(ULONG ulClass, void *pvDesc, UCHAR ucDescLen, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   if (ucDescLen <= MBPB_PRODUCT_DEVICE_LENGTH)
   {
      if (SSEM_LOCKUSR(SSemLock, SMBPBApp.SDataRW, u16Timeout))
      {
         SMBPBApp.SDataRW.SProduct.ulClass= ulClass;
         SMBPBApp.SDataRW.SProduct.ucDeviceLen= ucDescLen;
         memcpy(SMBPBApp.SDataRW.SProduct.ucDevice, pvDesc, ucDescLen);
         
         bResult= TRUE;
         
         SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW);
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// bool MBPBProductStateSet
//----------------------------------------------------------------------------
bool MBPBProductStateSet(bool bState, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   if (SSEM_LOCKUSR(SSemLock, SMBPBApp.SDataRW, u16Timeout))
   {
      SMBPBApp.SDataRW.ERunState= (bState) ? (MBPB_RUNSTATE_ON) : (MBPB_RUNSTATE_OFF);

      bResult= TRUE;
      
      SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW);
   }
      
   return (bResult);
}

//----------------------------------------------------------------------------
// bool MBPBProductAddrSet
//----------------------------------------------------------------------------
bool MBPBProductAddrSet(UCHAR ucAddr, U16 u16Timeout)
{
   bool bResult= FALSE;
   
   if (ucAddr >= MBPB_ADDR_MIN && ucAddr <= MBPB_ADDR_MAX)
   {
      if (SSEM_LOCKUSR(SSemLock, SMBPBApp.SDataRW, u16Timeout))
      {
         SMBPBApp.SDataRW.ucAddr= _umin(ucAddr, MBPB_ADDR_MAX);
   
         bResult= TRUE;
         
         SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW);
      }
   }
         
   return (bResult);
}

//----------------------------------------------------------------------------
// EMBPBErrorCode MBPBMsgCtrl
//----------------------------------------------------------------------------
EMBPBErrorCode MBPBMsgCtrl(struct _SMBPBPara *pSMBPBPara, bool bRx, bool bTx)
{
   EMBPBErrorCode EErrorCode= MBPB_ENORES;
   
   // Lock EMode
   if (SSEM_LOCK(SSemLock, pSMBPBPara->SPortRW))
   {
      if (bRx)
      {
         if (pSMBPBPara->SPortRO.SAux.PrepareRx) pSMBPBPara->SPortRO.SAux.PrepareRx(pSMBPBPara);
         LOCKED(pSMBPBPara->SPortRW.ulMsgMask |= (pSMBPBPara->SPortRO.ulMsgBitRx | pSMBPBPara->SPortRO.ulMsgBitTo));
         // TBD: There should be NO received characters at this time yet!
      }
      else
      {
         LOCKED(pSMBPBPara->SPortRW.ulMsgMask &= ~(pSMBPBPara->SPortRO.ulMsgBitRx | pSMBPBPara->SPortRO.ulMsgBitTo));
      }
      
      if (bTx)
      {
         if (pSMBPBPara->SPortRO.SAux.PrepareTx) pSMBPBPara->SPortRO.SAux.PrepareTx(pSMBPBPara);
         LOCKED(pSMBPBPara->SPortRW.ulMsgMask |= pSMBPBPara->SPortRO.ulMsgBitTxDone);
      }
      else
      {
         LOCKED(pSMBPBPara->SPortRW.ulMsgMask &= ~pSMBPBPara->SPortRO.ulMsgBitTxDone);
      }
      
      SSEM_UNLOCK(SSemLock, pSMBPBPara->SPortRW);
      
      EErrorCode= MBPB_ENOERR;
   }
   
   return (EErrorCode);
}


// Common tasks

//----------------------------------------------------------------------------
// __task void TskMBPBMsg
//----------------------------------------------------------------------------
__task void TskMBPBMsg(void *pvParam)
{
   ULONG ulMsgMask;
   
   ULONG ulMsgBit;
   USHORT usCount;
   UCHAR ucIndex;
   UCHAR ucTranslating;
   
   EMBPBMode EMode;
   
   struct _SMBPBPara *pSMBPBPara= ((struct _SMBPBTskPara *) pvParam)->pSMBPBPara;
     
   FOREVER
   {
#ifdef MBPB_DEBUG
      if (((struct _SMBPBTskPara *)pSMBPBPara) == &SMBPBTskPara[MBPB_INDEX_SPI])
      {
         ucIndex= ucIndex;
      }
#endif

      // Pend on message samaphore (ready when messages are available)
      if (OS_SEM_TAKE_CHK(SSEM_TAKE(pSMBPBPara->SPortRO.SSemMsg)))
      {
         // Safely access ulMsgMask, EMode & ucTranslating and update temporary local copies
         LOCK;
         {
            ulMsgMask= pSMBPBPara->SPortRW.ulMsgMask;
            EMode= pSMBPBPara->SPortRW.EMode;
            ucTranslating= pSMBPBPara->SModeRW[EMode].ucTranslating;
         }
         UNLOCK;

         // Call driver message retrieve function for every set message bit in mask
         for (ucIndex= 0; ucIndex < 32; ucIndex++)
         {
            ulMsgBit= ulMsgMask & (1 << ucIndex);
            if (ulMsgBit)
            {
               // Read and reset counter for specified message bit
               usCount= pSMBPBPara->SPortRO.SDriver.Msg(pSMBPBPara->SPortRO.SDriver.SConfig.ucUnit, pSMBPBPara->SPortRO.SDriver.SConfig.ucChannel, ulMsgBit, USHRT_MAX, pSMBPBPara->SPortRO.SDriver.SConfig.u16To);
               
               // Call valid individual notification functions for Rx, TxDone and To bits ucCount times                  
               if (usCount)
               {
                  if (ulMsgBit == pSMBPBPara->SPortRO.ulMsgBitRx)
                  {
                     if (ucTranslating)
                     {
                        // Translating converts any input format to valid raw modbus data and writes it to the raw buffer
                        if (pSMBPBPara->SModeRO[EMode].STranslator.Rx) pSMBPBPara->SModeRO[EMode].STranslator.Rx(pSMBPBPara, usCount);                     
                     }
                     
                     if (pSMBPBPara->SModeRO[EMode].SMessage.NotifyBitRx)
                     {
                        if (SSEM_LOCK(SSemLock, pSMBPBPara->SModeRW[EMode].SPrevRW))
                        {
                           pSMBPBPara->SModeRO[EMode].SMessage.NotifyBitRx(pSMBPBPara, usCount, pSMBPBPara->SModeRW[EMode].SPrevRW.ucDataAr, &pSMBPBPara->SModeRW[EMode].SPrevRW.ucIdx);
                           SSEM_UNLOCK(SSemLock, pSMBPBPara->SModeRW[EMode].SPrevRW);
                        }
                     }
                  }
                  else if (ulMsgBit == pSMBPBPara->SPortRO.ulMsgBitTxDone)
                  {
                     if (pSMBPBPara->SModeRO[EMode].SMessage.NotifyBitTxDone) pSMBPBPara->SModeRO[EMode].SMessage.NotifyBitTxDone(pSMBPBPara, usCount);
                  }
                  else if (ulMsgBit == pSMBPBPara->SPortRO.ulMsgBitTo)
                  {
                     if (pSMBPBPara->SModeRO[EMode].SMessage.NotifyBitTo) pSMBPBPara->SModeRO[EMode].SMessage.NotifyBitTo(pSMBPBPara, usCount);
                  }  
               }
            }
            
            RESCHEDULE(0);
         }
      }
   }
   
   DETACH;
}

//----------------------------------------------------------------------------
// __task void TskMBPBData
//----------------------------------------------------------------------------
__task void TskMBPBData(void *pvParam)
{
   UINT uiIdx;
   USHORT usResult;
   UCHAR ucPortReqIdx;
   UCHAR ucPortReqID;
   UCHAR ucPortFwdIdx;
   UCHAR ucPortFwdID;
   UCHAR ucTranslating;
   UCHAR ucAddr;
   UCHAR ucCode;
   UCHAR ucIndex;
   UCHAR *pucData;
   bool bEventForward;
   
   EMBPBMode EMode;
   EMBPBMode EModeTemp;
   EMBPBEvent EEvent;
   EMBPBEvent EEventNext;
   EMBPBState EState;
   EMBPBException EException;

   struct _SMBPBTskPara *pSMBPBTskPara= (struct _SMBPBTskPara *) pvParam;
   struct _SMBPBPara *pSMBPBPara= ((struct _SMBPBTskPara *) pvParam)->pSMBPBPara;
   struct _SMBPBApp *pSMBPBApp= ((struct _SMBPBTskPara *) pvParam)->pSMBPBApp;
   
   EMBPBErrorCode EErrorCode= MBPB_ENOERR;
   
   FOREVER
   {
#ifdef MBPB_DEBUG
      if (((struct _SMBPBTskPara *)pSMBPBPara) == &SMBPBTskPara[MBPB_INDEX_SPI])
      {
         ucIndex= ucIndex;
      }
#endif

      LOCK;
      {
         // Safely access EMode and update temporary local copy
         EMode= pSMBPBPara->SPortRW.EMode;
         
         // Safely access ucState and update temporary local copy
         EState= pSMBPBPara->SModeRW[EMode].EState;
         
         // Safely access ucTranslating and update temporary local copy
         ucTranslating= pSMBPBPara->SModeRW[EMode].ucTranslating;
      }
      UNLOCK;
      
      // Easy debug watch: pSMBPBPara->SPortRO.ucPortBusID
      if (EState == MBPB_STATE_ENABLED)
      {
         // Pend on new event (change) message
         if (SMBX_WAIT(SMbxEvent, pSMBPBPara->SModeRO[EMode], EEventNext))
         {           
            if (/* std mode: local msg and not pending            */ ((EEventNext <= MBPB_EVT_FRAME_SENT) && (EEvent != MBPB_EVT_PENDING)) || \
                /* req mode: reply msg (nak/rtn) when pending     */ ((EEventNext >= MBPB_EVT_FRAME_FORWARD_NAK) && (EEvent == MBPB_EVT_PENDING)) || \
                /* fwd mode: timeout msg after frame send/sent    */ ((EEventNext == MBPB_EVT_FRAME_FORWARD_TO) && ((EEvent == MBPB_EVT_FRAME_SEND) || (EEvent == MBPB_EVT_FRAME_SENT))) || \
                /* std mode: forward request msg when idle        */ (((EEventNext >= MBPB_EVT_FRAME_FORWARD_REQ1) && (EEventNext <= MBPB_EVT_FRAME_FORWARD_REQ7)) && ((EEvent == MBPB_EVT_READY) || (EEvent == MBPB_EVT_IDLE) || (EEvent == MBPB_EVT_FRAME_SENT))))
            {
               // Change to new event for valid cases
               EEvent= EEventNext;
            }
            else
            {
               // Determine mode by checking frame forward storage
               if (!pSMBPBPara->SPortRW.SReq.ucPortBusID)
               {
                  // Standard frame mode
                  // Reject new event and force local exception reply
                  EEvent= MBPB_EVT_FRAME_FORWARD_NAK;
               }
               else
               {
                  // Frame dest/forward mode
                  // Reject new event and force exception reply to requester
                  EEvent= MBPB_EVT_FRAME_FORWARD_TO;               
               }
            }

            // Direct event forwarding happens only inside this do-while block
            bEventForward= FALSE;
            do
            {
               // Change to forward event and clear event forward flag
               if (bEventForward)
               {
                  EEvent= EEventNext;
                  bEventForward= FALSE;
               }
               
               switch (EEvent)
               {
                  case MBPB_EVT_READY:
                     // Initial timeout signals the end of the modbus-portbus startup delay
                                       
                     if (pSMBPBPara->SPortRO.SAux.PrepareRx) pSMBPBPara->SPortRO.SAux.PrepareRx(pSMBPBPara);
                     if (pSMBPBPara->SPortRO.SDriver.SMisc.PrepareRx) pSMBPBPara->SPortRO.SDriver.SMisc.PrepareRx(pSMBPBPara);
                     
                     break;

                  case MBPB_EVT_IDLE:
                     // After all forward/backward tasks are done (like ready after startup)
                     
                     break;

                  case MBPB_EVT_PENDING:
                     // Pend on dest/forward return message(s) and reject any other host request
                     
                     break;
                     
                  case MBPB_EVT_FRAME_FORWARD_TO:
                     // Timer or manual message for dest/forward port to trigger message MBPB_EVT_FRAME_FORWARD_NAK
                     // to request port to stop its pending to continue its normal operation
                     
                     // Determine mode by checking frame forward storage
                     // Check for valid current frame forward request port
                     LOCKED(ucPortReqID= pSMBPBPara->SPortRW.SReq.ucPortBusID);
                     if (ucPortReqID)
                     {
                        // Frame dest/forward mode
                                                
                        // Determine port index
                        ucPortReqIdx= ucPortReqID- 1;

                        LOCK;
                        {
                           // Send MBPB_EVT_FRAME_FORWARD_NAK reply to current pending request port to continue its normal operation
                           EModeTemp= SMBPBPara[ucPortReqIdx].SPortRW.EMode;
                           SMBX_SEND(SMbxEvent, SMBPBPara[ucPortReqIdx].SModeRO[EModeTemp], MBPB_EVT_FRAME_FORWARD_NAK);
                        
                           // Exit frame dest/forward mode
                           pSMBPBPara->SPortRW.SReq.ucPortBusID= 0;
                           pSMBPBPara->SPortRW.SFwd.ucPBAddr= 0;
                        }
                        UNLOCK;
                     }
                     
                     // Clear timer indicator (already fired - no chance to kill early anymore)
                     pSMBPBPara->SPortRW.SFwd.oiTmr= 0;
                     
                     // Set MBPB_EVT_IDLE
                     EEventNext= MBPB_EVT_IDLE;
                     bEventForward= TRUE;
                     
                     break;

                  case MBPB_EVT_FRAME_FORWARD_NAK:
                     // Previous forward request is rejected by last event or dest/forward port
                     
                     // Exit request mode
                     pSMBPBPara->SPortRW.SReq.ucPBAddr= 0;
                     
                     // Reply with I/O error exception frame because we're not ready for the requested event yet
                     EException= MBPB_EX_GATEWAY_PATH_FAILED;
                     
                     // Set MBPB_EVT_FRAME_SEND as new event directly to transmit the exception
                     EEventNext= MBPB_EVT_FRAME_SEND;
                     bEventForward= TRUE;
                     
                     break;

                  case MBPB_EVT_FRAME_FORWARD_RTN:
                     // Received return frame for previously forwarded request from dest/forward port
                     
                     // Exit request mode
                     pSMBPBPara->SPortRW.SReq.ucPBAddr= 0;

                     // Clear exception
                     EException= MBPB_EX_NONE;
                     
                     // Set MBPB_EVT_FRAME_SEND as new event directly to transmit the frame written into our buffer by dest/forward port
                     EEventNext= MBPB_EVT_FRAME_SEND;
                     bEventForward= TRUE;
                     
                     break;

                  case MBPB_EVT_FRAME_FORWARD_REQ1:
                  case MBPB_EVT_FRAME_FORWARD_REQ2:
                  case MBPB_EVT_FRAME_FORWARD_REQ3:
                  case MBPB_EVT_FRAME_FORWARD_REQ4:
                  case MBPB_EVT_FRAME_FORWARD_REQ5:
                  case MBPB_EVT_FRAME_FORWARD_REQ6:
                  case MBPB_EVT_FRAME_FORWARD_REQ7:
                     // Received forward request (we are the dest/forward port this time)
                     
                     // Determine request port's PortBusID from message
                     ucPortReqID= (_umin(EEvent, MBPB_EVT_FRAME_FORWARD_REQ7)- MBPB_EVT_FRAME_FORWARD_REQ1)+ 1;

                     // Set initial EException (will be cleared on success)
                     EException= MBPB_EX_GATEWAY_PATH_FAILED;

                     // Determine mode by checking frame forward storage
                     if (!pSMBPBPara->SPortRW.SReq.ucPortBusID)
                     {
                        // Standard mode - ready for dest/forward request
                     
                        // Check for valid forward request port
                        if ((ucPortReqID > 0) && (ucPortReqID <= MBPB_PORT_COUNT))
                        {
                           // Determine port index
                           ucPortReqIdx= ucPortReqID- 1;
                        
                           // Enter dest/forward mode by storing requester's port PortBusID (retrieved from message)
                           // Set translation flag depending on address translation bit
                           LOCK;
                           {
                              pSMBPBPara->SPortRW.SReq.ucPortBusID= ucPortReqID;
                              pSMBPBPara->SPortRW.SFwd.ucPBAddr= SMBPBPara[ucPortReqIdx].SPortRW.SReq.ucPBAddr;
                              
                              pSMBPBPara->SModeRW[EMode].ucTranslating= (((pSMBPBPara->SPortRW.SFwd.ucPBAddr & MBPB_TRANSLATE_MASK) != 0) && (pSMBPBPara->SModeRO[EMode].STranslator.Rx != 0) && (pSMBPBPara->SModeRO[EMode].STranslator.Tx != 0));
                              ucTranslating= pSMBPBPara->SModeRW[EMode].ucTranslating;
                           }
                           UNLOCK;

                           // Lock the raw buffer (not locked by NotifyRx function yet) - requester's raw buffer it's locked by itsself already
                           if (SSEM_LOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]))
                           {
                              // Copy requester's buffer into our (dest/forward) buffer (this time we're the dest/forward port)
                             
                              for (uiIdx= 0; uiIdx < SMBPBPara[ucPortReqIdx].SBufferRW[MBPB_BUFFER_RAW].usCnt; uiIdx++)
                              {
                                 *(pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData+ uiIdx)= *(SMBPBPara[ucPortReqIdx].SBufferRW[MBPB_BUFFER_RAW].pucData+ uiIdx);
                              }

                              pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos= SMBPBPara[ucPortReqIdx].SBufferRW[MBPB_BUFFER_RAW].usPos;
                              pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= SMBPBPara[ucPortReqIdx].SBufferRW[MBPB_BUFFER_RAW].usCnt;

                              // Re-set the fixed address (clear bus and translator bits to hide the forwarding from the end point device)
                              if (pSMBPBPara->SModeRO[EMode].SParser.AddrSet)
                              {
                                 pSMBPBPara->SModeRO[EMode].SParser.AddrSet(pSMBPBPara, (pSMBPBPara->SPortRW.SFwd.ucPBAddr & MBPB_ADDR_MASK));

                                 // (Re-)arm timeout timer
                                 LOCK;
                                 {
                                    // Kill any pending timer early
                                    if (pSMBPBPara->SPortRW.SFwd.oiTmr)
                                    {
                                       os_tmr_kill(pSMBPBPara->SPortRW.SFwd.oiTmr);
                                       pSMBPBPara->SPortRW.SFwd.oiTmr= 0;
                                    }
   
                                    // Create new pending timer
                                    if (pSMBPBPara->SPortRO.ucPortBusID)
                                    {
                                       pSMBPBPara->SPortRW.SFwd.oiTmr= os_tmr_create(OS_T_MS(MBPB_TIMEOUTMSFWD), TIMER((pSMBPBPara->SPortRO.ucPortBusID- 1)+ MBPB_TIMER_BASE));
                                       if (pSMBPBPara->SPortRW.SFwd.oiTmr)
                                       {
                                          // Clear exception
                                          EException= MBPB_EX_NONE;
                                       }
                                    }
                                 }
                                 UNLOCK;
                              }
                              else
                              {
                                 EException= MBPB_EX_ILLEGAL_FUNCTION;
                              }

                              // Set ucAddr for MBPB_EVT_FRAME_SEND
                              ucAddr= pSMBPBPara->SPortRW.SFwd.ucPBAddr;

                              // Set MBPB_EVT_FRAME_SEND as new event directly to transmit the copied buffer data to the end point
                              EEventNext= MBPB_EVT_FRAME_SEND;
                              bEventForward= TRUE;
                              
                              // Unlock the raw buffer
                              SSEM_UNLOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]);
                           }
                        }
                     }
                     
                     if (EException != MBPB_EX_NONE)
                     {
                        // Check for valid current forward request port
                        if ((ucPortReqID > 0) && (ucPortReqID <= MBPB_PORT_COUNT))
                        {
                           // Determine port index
                           ucPortReqIdx= ucPortReqID- 1;
                           
                           // Frame dest/forward mode is already active - send MBPB_EVT_FRAME_FORWARD_NAK reply to current request port
                           LOCK;
                           {
                              EModeTemp= SMBPBPara[ucPortReqIdx].SPortRW.EMode;
                              SMBX_SEND(SMbxEvent, SMBPBPara[ucPortReqIdx].SModeRO[EModeTemp], MBPB_EVT_FRAME_FORWARD_NAK);
                           }
                           UNLOCK;
                        }
                     }
                     
                     break;

                  case MBPB_EVT_FRAME_RECEIVED:
                     // Received a new data frame

                     // Determine mode by checking frame forward storage
                     if (!pSMBPBPara->SPortRW.SReq.ucPortBusID)
                     {
                        // Standard mode
                        
                        EErrorCode= MBPB_ENORES;
                        if (pSMBPBPara->SModeRO[EMode].SParser.Verify) EErrorCode= pSMBPBPara->SModeRO[EMode].SParser.Verify(pSMBPBPara);
                        
                        if (EErrorCode == MBPB_ENOERR)
                        {
                           if (pSMBPBPara->SModeRO[EMode].SParser.AddrGet) ucAddr= pSMBPBPara->SModeRO[EMode].SParser.AddrGet(pSMBPBPara);

                           // Determine dest/forward port from frame address byte
                           ucPortFwdID= (ucAddr & MBPB_PORTBUSID_MASK) >> MBPB_PORTBUSID_MASKSHIFT;

                           if ((ucPortFwdID == pSMBPBPara->SPortRO.ucPortBusID) || (!ucPortFwdID))
                           {
                              // Local execution request
                              
                              // Clear any translation/portbus information
                              ucAddr&= MBPB_ADDR_MASK;
                              
                              if ((ucAddr == pSMBPBApp->SDataRW.ucAddr) || (ucAddr == MBPB_ADDR_BROADCAST))
                              {
                                 // Our address - execute the frame

                                 // Set MBPB_EVT_EXECUTE as new event directly to execute the frame immediately
                                 EEventNext= MBPB_EVT_EXECUTE;
                                 bEventForward= TRUE;
                              }
                              else
                              {
                                 // Not our address - ignore the frame
                                 // Unlock the raw buffer (locked by NotifyRx function or MBPB_EVT_FRAME_FORWARD_REQx event)
                                 SSEM_UNLOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]);
                              }
                           }
                           else
                           {
                              // Forward request (remote execution)
                                                           
                              // Continue for valid port
                              if ((ucPortFwdID > 0) && (ucPortFwdID <= MBPB_PORT_COUNT))
                              {
                                 // Determine port index
                                 ucPortFwdIdx= ucPortFwdID- 1;
                                 
                                 // Check dest/forward port's activation to avoid pending forever on its timeout reply
                                 LOCK;
                                 {
                                    EModeTemp= SMBPBPara[ucPortFwdIdx].SPortRW.EMode;
                                    if ((!SMBPBPara[ucPortFwdIdx].SPortRO.bActive) || \
                                        ((ucAddr & MBPB_TRANSLATE_MASK) && ((!SMBPBPara[ucPortFwdIdx].SModeRO[EModeTemp].STranslator.Rx) || (!SMBPBPara[ucPortFwdIdx].SModeRO[EModeTemp].STranslator.Tx))))
                                    {
                                       // Dest/forward port is not active - set MBPB_EVT_FRAME_FORWARD_NAK as new event directly to transmit the exception locally
                                       EEventNext= MBPB_EVT_FRAME_FORWARD_NAK;
                                       bEventForward= TRUE;
                                    }
                                    else
                                    {                                   
                                       // Queue forward address for dest/forward port
                                       pSMBPBPara->SPortRW.SReq.ucPBAddr= ucAddr;

                                       // Send frame forward request to dest/forward port
                                       SMBX_SEND(SMbxEvent, SMBPBPara[ucPortFwdIdx].SModeRO[EModeTemp], (MBPB_EVT_FRAME_FORWARD_REQ1+ (_umax(pSMBPBPara->SPortRO.ucPortBusID, 1)- 1)));
                                       
                                       // Forward event to MBPB_EVT_PENDING to pend on dest/forward return message(s)
                                       EEventNext= MBPB_EVT_PENDING;
                                       bEventForward= TRUE;                              
                                    }
                                 }
                                 UNLOCK;
                              }
                              else
                              {
                                 // Unlock the raw buffer (locked by NotifyRx function or MBPB_EVT_FRAME_FORWARD_REQx event)
                                 SSEM_UNLOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]);
                                 
                                 // Dest/forward port is not valid - set MBPB_EVT_FRAME_FORWARD_NAK as new event directly to transmit the exception
                                 EEventNext= MBPB_EVT_FRAME_FORWARD_NAK;
                                 bEventForward= TRUE;
                              }                              
                           }
                        }
                        else
                        {
                           if (pSMBPBPara->SPortRO.SAux.PrepareRx) pSMBPBPara->SPortRO.SAux.PrepareRx(pSMBPBPara);
                           if (pSMBPBPara->SPortRO.SDriver.SMisc.PrepareRx) pSMBPBPara->SPortRO.SDriver.SMisc.PrepareRx(pSMBPBPara);
                        
                           // Unlock the raw buffer (locked by NotifyRx function or MBPB_EVT_FRAME_FORWARD_REQ event)
                           SSEM_UNLOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]);
                        }
                     }
                     else
                     {
                        // Frame dest/forward mode

                        // Cancel any timeout timer
                        LOCK;
                        {
                           if (pSMBPBPara->SPortRW.SFwd.oiTmr)
                           {
                              os_tmr_kill(pSMBPBPara->SPortRW.SFwd.oiTmr);
                              pSMBPBPara->SPortRW.SFwd.oiTmr= 0;
                           }
                        }
                        UNLOCK;

                        // Retrieve PortBusID to reply to
                        ucPortFwdID= pSMBPBPara->SPortRW.SReq.ucPortBusID;

                        // Continue for valid port
                        if ((ucPortFwdID > 0) && (ucPortFwdID <= MBPB_PORT_COUNT))
                        {
                           // Determine port index
                           ucPortFwdIdx= ucPortFwdID- 1;

                           // Lock the raw buffer (not locked by NotifyRx function yet) - requester's raw buffer it's locked by itsself already
                           if (SSEM_LOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]))
                           {                        
                              // Copy _our_ buffer to requester's buffer and send MBPB_EVT_FRAME_FORWARD_RTN to requester to wake up and continue
                             
                              for (uiIdx= 0; uiIdx < pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt; uiIdx++)
                              {
                                 *(SMBPBPara[ucPortFwdIdx].SBufferRW[MBPB_BUFFER_RAW].pucData+ uiIdx)= *(pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData+ uiIdx);
                              }
      
                              SMBPBPara[ucPortFwdIdx].SBufferRW[MBPB_BUFFER_RAW].usPos= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos;
                              SMBPBPara[ucPortFwdIdx].SBufferRW[MBPB_BUFFER_RAW].usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt;
   
                              LOCK;
                              {
                                 // Re-set the address (set bus and translator bits again to show the forwarding from the end point device)
                                 EModeTemp= SMBPBPara[ucPortFwdIdx].SPortRW.EMode;
                                 if (SMBPBPara[ucPortFwdIdx].SModeRO[EModeTemp].SParser.AddrSet) SMBPBPara[ucPortFwdIdx].SModeRO[EModeTemp].SParser.AddrSet(&SMBPBPara[ucPortFwdIdx], pSMBPBPara->SPortRW.SFwd.ucPBAddr);
                                 SMBX_SEND(SMbxEvent, SMBPBPara[ucPortFwdIdx].SModeRO[EModeTemp], MBPB_EVT_FRAME_FORWARD_RTN);
   
                                 // Exit Dest/forward mode
                                 pSMBPBPara->SPortRW.SReq.ucPortBusID= 0;
                                 pSMBPBPara->SPortRW.SFwd.ucPBAddr= 0;                           
                              }
                              UNLOCK;

                              // Unlock the raw buffer
                              SSEM_UNLOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]);
                           }
                        }
                        
                        // Clear exception
                        EException= MBPB_EX_NONE;

                        // Set MBPB_EVT_IDLE to change to idle state again (transmission is done by the request port, _not_ our port!!)
                        EEventNext= MBPB_EVT_IDLE;
                        bEventForward= TRUE;
                     }
                     
                     break;
                  
                  case MBPB_EVT_EXECUTE:
                     // Execute frame locally (code MBPB_CODE_NONE will be ignored for SPI usability)

                     EException= MBPB_EX_ILLEGAL_FUNCTION;
                     
                     if (pSMBPBPara->SModeRO[EMode].SParser.DataGet)
                     {
                        pucData= pSMBPBPara->SModeRO[EMode].SParser.DataGet(pSMBPBPara);                    
                        LOCKED(ucCode= pucData[MBPB_PDU_FUNC_OFF]);
                        if (ucCode != MBPB_CODE_NONE)
                        {
                           for (ucIndex= 0; (ucIndex < MBPB_HANDLERS_MAX) && (pSMBPBApp->SDataRO.SHandler[ucIndex].ucCode); ucIndex++)
                           {
                              if (ucCode == pSMBPBApp->SDataRO.SHandler[ucIndex].ucCode)
                              {
                                 if (pSMBPBApp->SDataRO.SHandler[ucIndex].Function)
                                 {
#ifdef _DEBUG
                                    // Copy raw to cooked buffer to safe the raw rx data for debugging
                                    memcpy(pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].pucData, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData, SMBPBPara->SBufferRO[MBPB_BUFFER_COOKED].usSize);
                                    pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usPos= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos;
                                    pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usCnt= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt;
#endif                        
                                    // Len fix because new send code requires len to include address and checksum bytes but function needs it without them and modifies it
                                    if (pSMBPBPara->SModeRO[EMode].SParser.LenGet)
                                    {
                                       LOCKED(pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= pSMBPBPara->SModeRO[EMode].SParser.LenGet(pSMBPBPara));
                                       EException= pSMBPBApp->SDataRO.SHandler[ucIndex].Function(pucData, &pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt, pSMBPBTskPara);
                                    }
                                    
                                    // Len fix because new send code requires len to include address and checksum bytes
                                    if (EException == MBPB_EX_NONE)
                                    {
                                       if (pSMBPBPara->SModeRO[EMode].SParser.LenFix)
                                       {
      												LOCK;
      												{
                                          	pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= pSMBPBPara->SModeRO[EMode].SParser.LenFix(pSMBPBPara);
                                          	pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usPos= pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt;
      												}
      												UNLOCK;
                                       }
                                       else
                                       {
                                          EException= MBPB_EX_ILLEGAL_FUNCTION;
                                       }
                                    }
                                    
                                    break;
                                 }
                              }
                           }
                        }
                        else
                        {
                           EException= MBPB_EX_NONE;
                        }
                     }

                     // __NO__ "break;" to continue with MBPB_EVT_FRAME_SEND directly !!!
                     
                  case MBPB_EVT_FRAME_SEND:
                     // If the request is not MBPB_CODE_NONE and it was not sent to the broadcast address we return a reply
                     // For syncronous drivers we always queue reply data to enable them to receive again

                     // Retrieve address and code for forward/backward (foreign) frame to send (there was no local MBPB_EVT_EXECUTE before!)
                     if (EEvent != MBPB_EVT_EXECUTE)
                     {                       
                        // ucCode is zero for foreign frames and needs to be retrieved from storage in that case
                        if (pSMBPBPara->SModeRO[EMode].SParser.DataGet)
                        {
                           pucData= pSMBPBPara->SModeRO[EMode].SParser.DataGet(pSMBPBPara);
                           LOCKED(ucCode= pucData[MBPB_PDU_FUNC_OFF]);
                        }
                     }
                     
                     if ((((ucAddr & MBPB_ADDR_MASK) != MBPB_ADDR_BROADCAST) && (ucCode != MBPB_CODE_NONE)) || (!pSMBPBPara->SPortRO.SDriver.SConfig.bAsync))
                     {
                        if (EException != MBPB_EX_NONE)
                        {
                           // An exception occured. Build an error frame
                           
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[MBPB_PDU_DATA_OFF   ]= (UCHAR)(ucCode | MBPB_CODE_ERROR);
                           pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].pucData[MBPB_PDU_DATA_OFF+ 1]= EException;
                           
                           // usCnt needs to include already address and checksum bytes
                           
   								LOCK;
   								{
                           	pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= 2;
                           	pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW].usCnt= pSMBPBPara->SModeRO[EMode].SParser.LenFix(pSMBPBPara);
   								}
   								UNLOCK;
                        }
                        
                        // Prepare raw data for transmission (cook it;-)) and send it

                        // Set initial EErrorCode (will be overwritten by valid callback function in the block below)
                        EErrorCode= MBPB_ENORES;
                        
                        if (!ucTranslating)
                        {
                           if (pSMBPBPara->SModeRO[EMode].SParser.Cook) EErrorCode= pSMBPBPara->SModeRO[EMode].SParser.Cook(pSMBPBPara);
                        }
                        else
                        {
                           // Translating uses raw modbus data and converts it to any output format it writes to the cooked buffer
                           if (pSMBPBPara->SModeRO[EMode].STranslator.Tx) EErrorCode= pSMBPBPara->SModeRO[EMode].STranslator.Tx(pSMBPBPara);
                        }
                        
                        if (EErrorCode == MBPB_ENOERR)
                        {
                           if (pSMBPBPara->SPortRO.SAux.PrepareTx) pSMBPBPara->SPortRO.SAux.PrepareTx(pSMBPBPara);
                           if (pSMBPBPara->SPortRO.SDriver.SMisc.PrepareTx) pSMBPBPara->SPortRO.SDriver.SMisc.PrepareTx(pSMBPBPara);
                           LOCKED(pSMBPBPara->SModeRW[EMode].EStateTx= MBPB_STATE_TX_XMIT);
                           
                           if (pSMBPBPara->SPortRO.SDriver.Tx) usResult= pSMBPBPara->SPortRO.SDriver.Tx(pSMBPBPara->SPortRO.SDriver.SConfig.ucUnit, pSMBPBPara->SPortRO.SDriver.SConfig.ucChannel, &pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].pucData[0], (pSMBPBPara->SPortRO.SDriver.SConfig.bAsync) ? (pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usCnt) : (SPI_PDCMAX), pSMBPBPara->SPortRO.SDriver.SConfig.u16To);
                           
                           // Debug
                           usResult= usResult;                           
                        }
                     }
                
                     // Unlock the raw buffer (locked by NotifyRx function or MBPB_EVT_FRAME_FORWARD_REQx event)
                     SSEM_UNLOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_RAW]);
                     
                     break;
   
                  case MBPB_EVT_FRAME_SENT:
                     // Frame transmission complete
                     
                     // Mark buffer as free
                     pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usCnt= 0;
                     pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED].usPos= 0;
                     
                     // Unlock the cooked buffer (locked by Cook function)
                     SSEM_UNLOCK(SSemLock, pSMBPBPara->SBufferRW[MBPB_BUFFER_COOKED]);

                     if (pSMBPBPara->SPortRO.SAux.PrepareRx) pSMBPBPara->SPortRO.SAux.PrepareRx(pSMBPBPara);
                     //-if (pSMBPBPara->SPortRO.SDriver.SMisc.PrepareRx) pSMBPBPara->SPortRO.SDriver.SMisc.PrepareRx(pSMBPBPara);
                     
                     break;
               }
               
            } while (bEventForward && (EEvent != EEventNext));
         }
      }
      else
      {
         RESCHEDULE(0);
      }
   }
   
   DETACH;
}

//----------------------------------------------------------------------------
// __task void TskMBPBTrgFilter
//----------------------------------------------------------------------------
__task void TskMBPBTrgFilter(void)
{
   USHORT usAddrBase[MBPB_TRGTYPE_COUNT]= { 0 };
   
   struct _SMBPBFilterItem *pSFilterItem;
   ULONG ulRaw;
   USHORT usAddr;
   USHORT usVal;
   UCHAR ucType;
   UCHAR ucIndex;
   bool bRW;
   bool bAddr;
   bool bVal;
   bool bAddrRange;
   bool bValRange;
     
   FOREVER
   {
      // Pend on new address/value trigger message and return decoded values (FALSE= including address base messages)
      if (MBPBTrgWaitDecode(&usAddrBase[0], FALSE, &ulRaw, &ucType, &bRW, &usAddr, &usVal, SMBPBApp.SDataRW.STrgMsg.SMbxTrg.oiID, SMBPBApp.SDataRW.STrgMsg.SMbxTrg.u16To))
      {     
         // Lock STrgMsg
         if (SSEM_LOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg))
         {                  
            // Browse thru all filter items of the list choosen by ucType
            for (ucIndex= 0; ucIndex < MBPB_FILTERLIST_COUNT; ucIndex++)
            {
               pSFilterItem= &SMBPBApp.SDataRW.STrgMsg.SFilter[ucType].STrgItem[ucIndex];
               
               // Item active?
               if (pSFilterItem->bActive)
               {
                  // Mailbox valid?
                  if (pSFilterItem->SMbxTsk.oiID && pSFilterItem->SMbxTsk.u16To)
                  {
                     // Access match?
                     if ((bRW == MBPB_TRGMSG_RW_READ) ? (pSFilterItem->ucRWMask & MBPB_TRGMSG_RWMASK_READ) : (pSFilterItem->ucRWMask & MBPB_TRGMSG_RWMASK_WRITE))
                     {
                        // Init flags to support skip of AddrRange and ValRange
                        bVal= FALSE;
                        bAddrRange= TRUE;
                        bValRange= TRUE;
                        
                        // Address and value match?
                        bAddr= (usAddr == (pSFilterItem->usAddrMask & usAddr));                        
                        if (bAddr)
                        {
                           bVal= ((!usVal) ? (pSFilterItem->ucRWMask & MBPB_TRGMSG_RWMASK_VALZERO) : (usVal == (pSFilterItem->usValMask & usVal)));
                           if (bVal)
                           {
                              // Address range match?
                              if (pSFilterItem->ucRWMask & MBPB_TRGMSG_RWMASK_RANGE_ADDR)
                              {
                                 bAddrRange= ((usAddr >= pSFilterItem->SAddrRange.usMin) && (usAddr <= pSFilterItem->SAddrRange.usMax));
                              }
   
                              if (bAddrRange)
                              {
                                 // Value range match?
                                 if (pSFilterItem->ucRWMask & MBPB_TRGMSG_RWMASK_RANGE_VAL)
                                 {
                                    bValRange= ((usVal >= pSFilterItem->SValRange.usMin) && (usVal <= pSFilterItem->SValRange.usMax));
                                 }
                              }
                           }
                        }
                                                
                        if (bAddr && bVal && bAddrRange && bValRange)
                        {
                           LOCK;
                           {
                              // Lock is set above and a task switch may occur (with lock kept set!!!) for a write attempt to full mbx!!!
                              if (SMBX_CHECK(SMbxTsk, (*pSFilterItem)) > 0)
                              {
                                 // Send copy of msg to specified msgbox
                                 SMBX_SEND(SMbxTsk, (*pSFilterItem), ulRaw);
                              }
                           }
                           UNLOCK;
                        }
                     }
                  }
               }
            }
            
            SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
         }
      }
   }
   
   DETACH;
}

//----------------------------------------------------------------------------
// bool MBPBTrgWaitDecode
//----------------------------------------------------------------------------
bool MBPBTrgWaitDecode(USHORT *pusAddrBase, bool bHideBaseType, ULONG *pulRaw, UCHAR *pucType, bool *pbRW, USHORT *pusAddr, USHORT *pusVal, OS_ID oiID, U16 u16To)
{
   bool bResult= FALSE;
   
   ULONG ulValue;
   USHORT usAddr;
   USHORT usVal;
   UCHAR ucId;
   UCHAR ucType;
   bool bRW;
      
   // Address base is required
   if (pusAddrBase)
   {
      // At least one return storage is required
      if ((pucType != 0)+ (pbRW != 0)+ (pusAddr != 0)+ (pusVal != 0))
      {
         do
         {
            // Pend on new address/value trigger message
            if (OS_MBX_VALUE_CHK(os_mbx_wait(oiID, OS_MBX_VALUE_GET(ulValue), u16To)))
            {
               bResult= TRUE;
               ucType= MBPB_TRGMSG_GETTYPE(ulValue);
                              
               // Process valid type only
               if (ucType < MBPB_TRGTYPE_COUNT)
               {
                  ucId= MBPB_TRGMSG_GETID(ulValue);
                  bRW= MBPB_TRGMSG_GETRW(ulValue);
                  
                  // Determine address and value
                  switch (ucId)
                  {
                     case MBPB_TRGMSG_ID_ABS_ADDR8:
                        usAddr= MBPB_TRGMSG_GETVAL8(ulValue);
                        usVal= MBPB_TRGMSG_GETVAL16(ulValue);
                        break;
                        
                     case MBPB_TRGMSG_ID_ABS_ADDR16:
                        usAddr= MBPB_TRGMSG_GETVAL16(ulValue);
                        usVal= MBPB_TRGMSG_GETVAL8(ulValue);
                        break;
                        
                     case MBPB_TRGMSG_ID_BASE_ADDR16:
                        *(pusAddrBase+ ucType)= MBPB_TRGMSG_GETVAL16(ulValue);
                        usAddr= *(pusAddrBase+ ucType);
                        usVal= MBPB_TRGMSG_GETVAL8(ulValue);
                        break;
                        
                     case MBPB_TRGMSG_ID_REL_ADDR8:
                        usAddr= (*(pusAddrBase+ ucType))+ MBPB_TRGMSG_GETVAL8(ulValue);
                        usVal= MBPB_TRGMSG_GETVAL16(ulValue);
                        break;
                  }
               }
            }
      
         } while ((bHideBaseType && (ucId == MBPB_TRGMSG_ID_BASE_ADDR16)) && (ucType < MBPB_TRGTYPE_COUNT));
         
         if (ucType < MBPB_TRGTYPE_COUNT)
         {
            // Return decoded type, access, address and value
            if (pucType) *pucType= ucType;
            if (pbRW) *pbRW= bRW;
            if (pusAddr) *pusAddr= usAddr;
            if (pusVal) *pusVal= usVal;            
         }
         else
         {
            bResult= FALSE;
         }
         
         // Return always the raw message
         if (pulRaw) *pulRaw= ulValue;
      }
   }
        
   return (bResult);
}

//----------------------------------------------------------------------------
// bool MBPBTrgRebuild
//----------------------------------------------------------------------------
bool MBPBTrgRebuild(UCHAR ucType, U16 u16To)
{
   bool bResult= FALSE;
   
   struct _SMBPBTriggerItem *pSTriggerItem;
   struct _SMBPBFilterItem *pSFilterItem;
   USHORT usFilterIdx;
   
   if (ucType < MBPB_TRGTYPE_COUNT)
   {
      // Lock STrgMsg
      if (SSEM_LOCKUSR(SSemLock, SMBPBApp.SDataRW.STrgMsg, u16To))
      {
         pSTriggerItem= &SMBPBApp.SDataRW.STrgMsg.STrg[ucType];
         
         // Clear trigger masks
         pSTriggerItem->usAddrMask= 0;
         pSTriggerItem->ucR= 0;
         pSTriggerItem->ucW= 0;
         
         for (usFilterIdx= 0; usFilterIdx < SMBPBApp.SDataRW.STrgMsg.SFilter[ucType].usCount; usFilterIdx++)
         {
            pSFilterItem= &SMBPBApp.SDataRW.STrgMsg.SFilter[ucType].STrgItem[usFilterIdx];
            
            if (pSFilterItem->bActive && pSFilterItem->SMbxTsk.oiID && pSFilterItem->SMbxTsk.u16To)
            {
               pSTriggerItem->usAddrMask |= pSFilterItem->usAddrMask;
               pSTriggerItem->ucR |= MBPB_TRGMSG_RBITS(pSFilterItem->ucRWMask);
               pSTriggerItem->ucW |= MBPB_TRGMSG_WBITS(pSFilterItem->ucRWMask);
               
               bResult= TRUE;
            }
         }
         
         SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
      }
   }
   
   return (bResult);
}

//----------------------------------------------------------------------------
// USHORT MBPBTrgFilterSet
//----------------------------------------------------------------------------
USHORT MBPBTrgFilterSet(UCHAR ucType, USHORT usIndex, bool bActive, UCHAR ucRWMask, USHORT usAddrMask, USHORT usValMask, OS_ID oiMbx, U16 u16ToMbx, U16 u16To)
{
   return (MBPBTrgFilterSetEx(ucType, usIndex, bActive, ucRWMask, usAddrMask, usValMask, 0, 0, 0, 0, oiMbx, u16ToMbx, u16To));
}

//----------------------------------------------------------------------------
// USHORT MBPBTrgFilterSetEx
//----------------------------------------------------------------------------
USHORT MBPBTrgFilterSetEx(UCHAR ucType, USHORT usIndex, bool bActive, UCHAR ucRWMask, USHORT usAddrMask, USHORT usValMask, USHORT usAddrMin, USHORT usAddrMax, USHORT usValMin, USHORT usValMax, OS_ID oiMbx, U16 u16ToMbx, U16 u16To)
{
   USHORT usResult= 0;
   
   struct _SMBPBTriggerItem *pSTriggerItem;
   struct _SMBPBFilterItem *pSFilterItem;
   USHORT usFilterIdx;
   UCHAR ucRWMaskTemp;
   
   if (ucType < MBPB_TRGTYPE_COUNT)
   {
      if ((ucRWMask & MBPB_TRGMSG_RWMASK_RWBITS) && usAddrMask && (usValMask || (ucRWMask & MBPB_TRGMSG_RWMASK_VALZERO)) && oiMbx && u16ToMbx)
      {
         // Lock STrgMsg
         if (SSEM_LOCKUSR(SSemLock, SMBPBApp.SDataRW.STrgMsg, u16To))
         {                  
            if (!usIndex)
            {
               // Add filter at next free index
               usFilterIdx= SMBPBApp.SDataRW.STrgMsg.SFilter[ucType].usCount;
            }
            else
            {
               // Set/overwrite filter
               usFilterIdx= usIndex- 1;
            }
            
            if (usFilterIdx < (MBPB_FILTERLIST_COUNT- 1))
            {
               pSTriggerItem= &SMBPBApp.SDataRW.STrgMsg.STrg[ucType];
               pSFilterItem= &SMBPBApp.SDataRW.STrgMsg.SFilter[ucType].STrgItem[usFilterIdx];
               
               // Deactivate filter item
               pSFilterItem->bActive= FALSE;
      
               // Setup filter item

               ucRWMaskTemp= (usAddrMin || usAddrMax) ? (ucRWMask | MBPB_TRGMSG_RWMASK_RANGE_ADDR) : (ucRWMask & ~MBPB_TRGMSG_RWMASK_RANGE_ADDR);
               ucRWMaskTemp= (usValMin || usValMax) ? (ucRWMaskTemp | MBPB_TRGMSG_RWMASK_RANGE_VAL) : (ucRWMaskTemp & ~MBPB_TRGMSG_RWMASK_RANGE_VAL);
               pSFilterItem->ucRWMask= ucRWMaskTemp;

               pSFilterItem->usAddrMask= usAddrMask;
               pSFilterItem->usValMask= usValMask;
               pSFilterItem->SMbxTsk.oiID= oiMbx;
               pSFilterItem->SMbxTsk.u16To= u16ToMbx;
               
               // Setup filter item's ranges (require activation my ucRWMask flags MBPB_TRGMSG_RWMASK_RANGE_ADDR / VAL)
               pSFilterItem->SAddrRange.usMin= usAddrMin;
               pSFilterItem->SAddrRange.usMax= usAddrMax;
               pSFilterItem->SValRange.usMin= (ucRWMask & MBPB_TRGMSG_RWMASK_VALZERO) ? (0) : (usValMin);
               pSFilterItem->SValRange.usMax= usValMax;
               
               // Update trigger
               pSTriggerItem->usAddrMask |= usAddrMask;
               pSTriggerItem->ucR |= MBPB_TRGMSG_RBITS(ucRWMask);
               pSTriggerItem->ucW |= MBPB_TRGMSG_WBITS(ucRWMask);
               
               // Activate/Deactivate filter item
               pSFilterItem->bActive= bActive;
               
               // Update count
               SMBPBApp.SDataRW.STrgMsg.SFilter[ucType].usCount++;
               
               // Prepare result
               usResult= usFilterIdx+ 1;
            }
            
            SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
         }
      }
   }
            
   return (usResult);
}

//----------------------------------------------------------------------------
// bool MBPBTrgFilterStateSet
//----------------------------------------------------------------------------
bool MBPBTrgFilterStateSet(UCHAR ucType, USHORT usIndex, bool bActive, U16 u16To)
{
   bool bResult= FALSE;

   if (ucType < MBPB_TRGTYPE_COUNT)
   {
      if (usIndex)
      {
         if (usIndex <= (SMBPBApp.SDataRW.STrgMsg.SFilter[ucType].usCount))
         {
            // Lock STrgMsg
            if (SSEM_LOCKUSR(SSemLock, SMBPBApp.SDataRW.STrgMsg, u16To))
            {
               // Activate/Deactivate filter item
               SMBPBApp.SDataRW.STrgMsg.SFilter[ucType].STrgItem[usIndex- 1].bActive= bActive;
            
               bResult= TRUE;
               
               SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
            }
         }
      }         
   }

   return (bResult);
}

//----------------------------------------------------------------------------
// bool MBPBTrgFilterStateGet
//----------------------------------------------------------------------------
bool MBPBTrgFilterStateGet(UCHAR ucType, USHORT usIndex, U16 u16To)
{
   bool bResult= FALSE;

   if (ucType < MBPB_TRGTYPE_COUNT)
   {
      if (usIndex)
      {   
         if (usIndex <= (SMBPBApp.SDataRW.STrgMsg.SFilter[ucType].usCount))
         {
            // Lock STrgMsg
            if (SSEM_LOCKUSR(SSemLock, SMBPBApp.SDataRW.STrgMsg, u16To))
            {         
               // Return filter item state
               bResult= SMBPBApp.SDataRW.STrgMsg.SFilter[ucType].STrgItem[usIndex- 1].bActive;
               
               SSEM_UNLOCK(SSemLock, SMBPBApp.SDataRW.STrgMsg);
            }
         }
      }
   }

   return (bResult);
}

// Suppress warning for "#186-D: pointless comparision of unsigned integer with zero"
#pragma diag_remark 186
   
//----------------------------------------------------------------------------
// void MBPBTmrHandler
//----------------------------------------------------------------------------
void MBPBTmrHandler(U16 u16Info)
{
   // CAUTION!!! ISR CONTEXT!!!

   register U16 u16InfoTemp;
   register EMBPBMode EModeTemp;
   
   if (u16Info >= MBPB_TIMER_BASE)
   {
      u16InfoTemp= u16Info- MBPB_TIMER_BASE;
      
      if (u16InfoTemp < MBPB_PORT_COUNT)
      {    
         // Send timer message to dest/forward port
         EModeTemp= SMBPBPara[u16InfoTemp].SPortRW.EMode;
         SMBX_ISR_SEND(SMbxEvent, SMBPBPara[u16InfoTemp].SModeRO[EModeTemp], MBPB_EVT_FRAME_FORWARD_TO);
      }
   }
}

