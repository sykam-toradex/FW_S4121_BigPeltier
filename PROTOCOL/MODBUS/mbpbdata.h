//!	data definitions of MBPB (ModBusPortBus)
/**
		\file		mbpbdata.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBDATA.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-07-22 initial version
// 2009-07-23 added own struct for semaphores, adjusted (un)lock macro
// 2009-07-29 added portbusID and translate masks
// 2009-07-30 extrated file contents from mpbp.h
// 2009-07-31 changed FPTR for driver message function
// 2009-08-04 added state enums for rx and tx state
// 2009-08-06 added aux function pointers
// 2009-08-07 added count parameter for notify functions
// 2009-08-11 added comments to state enums
// 2009-08-12 changed translation mode to translation flag
// 2009-08-19 added application data structures
// 2009-08-19 added EMBPBException enum
// 2009-08-20 added EMBPBRegisterMode enum
// 2009-08-26 added ucMaxRx and ucMaxTx to parser structure
// 2009-08-26 added _SMBPBTskPara
// 2009-08-27 added void pointer to code function calls
// 2009-09-03 added TimeOutCalc function pointer definition
// 2009-09-04 changed TimeOutCalc function pointer definition
// 2009-09-04 added rx and tx functions to _SMBPBFPTRsTranslator
// 2009-09-09 removed ucPending and ucForward of _SMBPBPortRW
// 2009-09-09 added ucAddMaskTx and ucAddrForward to _SMBPBPortRW
// 2009-09-09 added MBPBSerAddrSet and MBPBSerAddrFix to _SMBPBFPTRsParser
// 2009-09-09 changed argument list of rx of _SMBPBFPTRsTranslator
// 2009-09-25 changed to fit single linked reentrant usart driver model
// 2009-10-01 added _SMBPBPrevRW, changed arguments for (*NotifyBitRx)
// 2009-10-23 changed driver rx and tx to support ushort block transfer
// 2009-11-06 changed driver msg return type to support ushort msg counters
// 2009-12-18 changed msg function to support true msg counter decrement
// 2010-04-14 added EMBPBTrgType
// 2010-04-14 added _SMBPBTrigger, _SMBPBFilterItem and _SMBPBFilterList
// 2010-04-14 added _SMBPBTrigger & _SMBPBFilterList to _SMBPBAppDataRW
// 2010-04-15 moved tidTskMBPBData/Msg to SMBPBApp.SDataRW
// 2010-04-22 moved trg msg params into STrgMsg structure
// 2010-04-22 added semaphore to lock STrgMsg structure during update
// 2010-04-30 added struct _SReg and added _SReg to _SMBPBAppDataRW
// 2010-04-30 moved SProduct to SMBPBApp.SDataRW
// 2010-05-05 added ulBaud & ulOption to _SMBPBFPTRsDriver
// 2010-05-05 created _SMBPBCfgDriver and added to _SMBPBPortRO
// 2010-05-05 moved params of SDriver to SDriver.SConfig
// 2010-06-16 added EMBPBREGType and EMBPBRegRW
// 2010-06-18 added _SMBPBRegAccess for wrapper function MBPBRegAccessStruct
// 2010-06-18 removed ucMaxRx and ucMaxTx of _SMBPBCfgDriver
// 2010-08-12 added bAsync flag to structure _SMBPBCfgDriver
// 2010-08-12 added void pointer arg to all functions of _SMBPBFPTRsAux
// 2010-08-12 changed return type to USHORT of all functions of _SMBPBFPTRsAux
// 2010-08-12 added SMBPBFPTRsAux as SMisc to _SMBPBFPTRsDriver
// 2010-08-27 added new state MBPB_STATE_RX_COMPLETE to EMBPBStateRx
// 2011-07-14 added user validate function pointer to _SMBPBAppDataRO
// 2011-07-28 added new event states to EMBPBEvent for forwarding feature
// 2011-08-03 fixed several comments, also for _SMBPBCfgDriver
// 2011-08-04 removed ucAddrMaskTx in _SMBPBPortRW
// 2011-08-04 renamed ucAddrForward to ucPBAddrForwardFwd in _SMBPBPortRW
// 2011-08-04 added ucPortBusIDFwd to _SMBPBPortRW
// 2011-08-04 added ucPBAddrForwardReq to _SMBPBPortRW
// 2011-08-10 added bRTU and bDisabled data fields to _SMBPBCfgDriver
// 2011-08-11 tied requester mode data of _SMBPBPortRW in _SMBPBPortRequester
// 2011-08-11 tied forwarder mode data of _SMBPBPortRW in _SMBPBPortForwarder
// 2011-08-11 added _SMBPBFPTRsTranslator (STranslator) to _SMBPBCfgDriver
// 2011-08-17 added _SMBPBFilterRange structure for use by _SMBPBFilterItem
// 2011-08-17 added SAddrRange & SValRange (_SMBPBFilterRange) to _SMBPBFilterItem
// 2011-10-27 added public doxygen documentation
// 2011-11-24 added public doxygen documentation
// 2011-11-24 removed obsolete ucDummy of structure _SMBPBFPTRsParser
// 2011-11-24 added internal doxygen documentation
// 2011-12-16 changed item ucClass -> ulClass of struct _SMBPBProduct 
// 2011-12-16 added item ucDeviceLen to struct _SMBPBProduct
// 2013-01-24 added _SMBPBMaxItemsRO, _SMBPBMaxItemsRW(Combo) and _SMBPBMaxsItem
// 2013-01-24 added _SMBPBMaxItems to _SMBPBAppDataRO
// 2013-01-24 changed count to USHORT in _SMBPBFPTRsMsg function pointers
// 2013-01-30 added MBPB_REGRW_READWRITE & MBPB_REGRW_COUNT to EMBPBRegRW
// 2013-04-24 added general reference type to EMBPBRegType & _SMBPBMaxItems
// 2013-04-25 changed address argument of validate function pointer to U32
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBDATA_H
#define SYKAM_PROTOCOL_MODBUS_MBPBDATA_H 1


// Forward declarations
struct _SMBPBPara;
struct _SMBPBApp;

//! Slave device run state
typedef enum
{
   MBPB_RUNSTATE_OFF= 0x00,                                             //!< Slave device not initialized, not running or unrecoverable error
   MBPB_RUNSTATE_ON= 0xFF                                               //!< Slave device initialized and running
} EMBPBRunState;

//! Parser state
typedef enum
{
   MBPB_STATE_ENABLED,                                                  //!< Parser enabled
   MBPB_STATE_DISABLED,                                                 //!< Parser disabled
   MBPB_STATE_NOT_INITIALIZED                                           //!< Parser not initialized yet
} EMBPBState;

//! Port receive state
typedef enum
{
   MBPB_STATE_RX_INIT,                                                  //!< Receiver is in initial state (ASC, RTU)
   MBPB_STATE_RX_IDLE,                                                  //!< Receiver is in idle state (ASC, RTU)
   MBPB_STATE_RX_RCV,                                                   //!< Frame is beeing received (ASC, RTU)
   MBPB_STATE_RX_WAIT_EOF,                                              //!< Wait for End of Frame (ASC)
   MBPB_STATE_RX_COMPLETE,                                              //!< End of Frame is received (ASC)
   MBPB_STATE_RX_ERROR                                                  //!< If the frame is invalid (RTU)
} EMBPBStateRx;

//! Port transmit state
typedef enum
{
   MBPB_STATE_TX_INIT,                                                  //!< Transmitter is in initial state (ASC)
   MBPB_STATE_TX_IDLE,                                                  //!< Transmitter is in idle state (ASC, RTU)
   MBPB_STATE_TX_START,                                                 //!< Starting transmission (':' sent) (ASC)
   MBPB_STATE_TX_DATA,                                                  //!< Sending of data (Address, Data, LRC) (ASC)
   MBPB_STATE_TX_END,                                                   //!< End of transmission (ASC)
   MBPB_STATE_TX_NOTIFY,                                                //!< Notify sender that the frame has been sent (ASC)
   MBPB_STATE_TX_XMIT                                                   //!< Transmitter is in transfer state (RTU)
} EMBPBStateTx;

//! State change event
typedef enum
{
   MBPB_EVT_READY,                                                      //!< Startup finished (trigger)
   MBPB_EVT_IDLE,                                                       //!< Idle for new events
   MBPB_EVT_FRAME_RECEIVED,                                             //!< Frame received (trigger)
   MBPB_EVT_EXECUTE,                                                    //!< Execute function (trigger)
   MBPB_EVT_FRAME_SEND,                                                 //!< Frame send (trigger)
   MBPB_EVT_FRAME_SENT,                                                 //!< Frame sent (trigger)
   MBPB_EVT_PENDING,                                                    //!< Pending for reply message from other port (state)
   MBPB_EVT_FRAME_FORWARD_REQ1,                                         //!< Frame forward request from port 1 to other port (trigger)
   MBPB_EVT_FRAME_FORWARD_REQ2,                                         //!< Frame forward request from port 2 to other port (trigger)
   MBPB_EVT_FRAME_FORWARD_REQ3,                                         //!< Frame forward request from port 3 to other port (trigger)
   MBPB_EVT_FRAME_FORWARD_REQ4,                                         //!< Frame forward request from port 4 to other port (trigger)
   MBPB_EVT_FRAME_FORWARD_REQ5,                                         //!< Frame forward request from port 5 to other port (trigger)
   MBPB_EVT_FRAME_FORWARD_REQ6,                                         //!< Frame forward request from port 6 to other port (trigger)
   MBPB_EVT_FRAME_FORWARD_REQ7,                                         //!< Frame forward request from port 7 to other port (trigger)
   MBPB_EVT_FRAME_FORWARD_NAK,                                          //!< Frame forward request is rejected from other port (trigger)
   MBPB_EVT_FRAME_FORWARD_RTN,                                          //!< Frame backward message from other port (trigger)
   MBPB_EVT_FRAME_FORWARD_TO                                            //!< Frame message for other port to stop its forwarding mode and release request port
} EMBPBEvent;

//! Error reply code
typedef enum
{
   MBPB_ENOERR,                                                         //!< No error
   MBPB_ENOREG,                                                         //!< Illegal register address
   MBPB_EINVAL,                                                         //!< Illegal argument
   MBPB_EPORTERR,                                                       //!< Porting layer error
   MBPB_ENORES,                                                         //!< Insufficient resources
   MBPB_EIO,                                                            //!< I/O error
   MBPB_EILLSTATE,                                                      //!< Protocol stack in illegal state
   MBPB_ETIMEDOUT                                                       //!< Timeout error occurred
} EMBPBErrorCode;

//! Exception reply code
typedef enum
{
    MBPB_EX_NONE= 0x00,                                                 //!< No exception
    MBPB_EX_ILLEGAL_FUNCTION= 0x01,                                     //!< Illegal function code
    MBPB_EX_ILLEGAL_DATA_ADDRESS= 0x02,                                 //!< Illegal data address
    MBPB_EX_ILLEGAL_DATA_VALUE= 0x03,                                   //!< Illegal data value
    MBPB_EX_SLAVE_DEVICE_FAILURE= 0x04,                                 //!< Slave device failure
    MBPB_EX_ACKNOWLEDGE= 0x05,                                          //!< Acknowledge missing
    MBPB_EX_SLAVE_BUSY= 0x06,                                           //!< Slave device busy
    MBPB_EX_MEMORY_PARITY_ERROR= 0x08,                                  //!< Memory parity error
    MBPB_EX_GATEWAY_PATH_FAILED= 0x0A,                                  //!< Gateway/forward path failure
    MBPB_EX_GATEWAY_TGT_FAILED= 0x0B                                    //!< Gateway/forward target failure
} EMBPBException;

//! Register access mode
typedef enum
{
   MBPB_REG_READ,                                                       //!< Read register values and pass to protocol stack
   MBPB_REG_WRITE                                                       //!< Update register values
} EMBPBRegisterMode;

//! Telegram style
typedef enum
{
   MBPB_MODE_ASC,                                                       //!< ASCII telegram style
   MBPB_MODE_RTU                                                        //!< Binary (RemoteTerminalUnit) telegram style
} EMBPBMode;

//! <b>Internal data:</b> ASC character position/state
typedef enum
{
   MBPB_BYTE_HIGH_NIBBLE,                                               //!< Character for high nibble of byte
   MBPB_BYTE_LOW_NIBBLE                                                 //!< Character for low nibble of byte
} EMBPBBytePos;

//! Access trigger
typedef enum
{
   MBPB_TRGTYPE_INPUT_READ,                                             //!< Read of 16 bit read ONLY register(s)
   MBPB_TRGTYPE_HOLDING_READ,                                           //!< Read of 16 bit read/write register(s)
   MBPB_TRGTYPE_HOLDING_WRITE,                                          //!< Write to 16 bit read/write register(s)
   MBPB_TRGTYPE_COILS_READ,                                             //!< Read of single read/write bit(s)
   MBPB_TRGTYPE_COILS_WRITE,                                            //!< Write to single read/write bit(s)
   MBPB_TRGTYPE_DISCRETE_READ,                                          //!< Read of single read ONLY bit(s)
   MBPB_TRGTYPE_COUNT
} EMBPBTrgType;

//! Register type
typedef enum
{
   MBPB_REGTYPE_INPUT,                                                  //!< Read ONLY 16 bit register
   MBPB_REGTYPE_HOLDING,                                                //!< Read/write 16 bit register
   MBPB_REGTYPE_COILS,                                                  //!< Read/write single bit
   MBPB_REGTYPE_DISCRETE,                                               //!< Read ONLY single bit
   MBPB_REGTYPE_GENERAL,                                                //!< Read/write 16 bit system memory data
   MBPB_REGTYPE_COUNT
} EMBPBRegType;

//! Register access mode
typedef enum
{
   MBPB_REGRW_WRITE,                                                    //!< Write access
   MBPB_REGRW_READ,                                                     //!< Read access
   MBPB_REGRW_READWRITE,                                                //!< Combined read+write access
   MBPB_REGRW_COUNT
} EMBPBRegRW;

//! Param struct for MBPBRegAccessStruct
struct _SMBPBRegAccess
{
	UCHAR 	ucType;                                                     //!< Type of register (#EMBPBRegType)
   bool     bRW;                                                        //!< Access mode (#EMBPBRegRW)
	USHORT 	usAddr;                                                     //!< Base adress
	USHORT 	usAddrOffs;                                                 //!< Adress offset (set 0 for default)
	USHORT 	usCount;                                                    //!< Count of register addresses
	void 		*pvData;                                                    //!< Application data storage
	void 		*pvIntern;                                                  //!< INTERNAL storage address (CAUTION!!!)
	U16 		u16To;                                                      //!< Maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
};

//! Auxiliary function pointers
struct _SMBPBFPTRsAux
{
   USHORT (*Prepare) (void *pvParam);                                   //!< Prepare hardware (optional, set NULL for default)
   USHORT (*PrepareRx) (void *pvParam);                                 //!< Prepare hardware for every new receive (optional, set NULL for default)
   USHORT (*PrepareTx) (void *pvParam);                                 //!< Prepare hardware for every new transmit (optional, set NULL for default)
};

//!   Translation function pointers
/**
      Translation is only used for destination addresses having the bit #MBPB_TRANSLATE_MASK set (you may address a port WITHOUT and WITH translation one after each other)
*/
struct _SMBPBFPTRsTranslator
{
   EMBPBErrorCode (*Rx) (struct _SMBPBPara  *pSMBPBPara, UCHAR ucCount);//!< (optional, set NULL for default) Translate ANY received data into ModBusPortBus like (only address and command code is verified) binary
   EMBPBErrorCode (*Tx) (struct _SMBPBPara  *pSMBPBPara);               //!< (optional, set NULL for default) Translate ModBusPortBus like (only address and command code is verified) binary into ANY transmit data
};

//! Port driver options
struct _SMBPBCfgDriver
{
   struct _SMBPBFPTRsTranslator STranslator[MBPB_MODE_COUNT];           //!< Translator function pointer(s)
   UCHAR ucUnit;                                                        //!< Unit of the driver
   UCHAR ucChannel;                                                     //!< Channel of the driver
   USHORT usMaxTx;                                                      //!< Maximum transfer size (in bytes) of the driver
   ULONG ulBaud;                                                        //!< Baud rate for the driver
   ULONG ulOption;                                                      //!< Driver dependent options (e.g. refer to usartext.h for RS232 options)
   U16 u16To;                                                           //!< Maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
   bool bAsync;                                                         //!< Type of driver (TRUE == asyncronous, FALSE == syncronous)
   bool bRTU;                                                           //!< Type of telegram style (TRUE == RTU / binary, FALSE == ASCII)
   bool bDisabled;                                                      //!< State of the driver (TRUE == disabled, FALSE == enabled)
};

//! \internal \brief <b>Internal data:</b> Port driver functions
struct _SMBPBFPTRsDriver
{
   USHORT (*Rx) (UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout);     //!< Receive function
   USHORT (*Tx) (UCHAR ucUnit, UCHAR ucChannel, void *pvData, USHORT usCount, U16 u16Timeout);     //!< Transmit function
   USHORT (*Msg) (UCHAR ucUnit, UCHAR ucChannel, ULONG ulMsg, USHORT usDecrement, U16 u16Timeout); //!< Message retrieve function
   struct _SMBPBFPTRsAux SMisc;                                                                    //!< Auxiliary function pointer(s)
   struct _SMBPBCfgDriver SConfig;                                                                 //!< Options
};

//! \internal \brief <b>Internal data:</b> Forward requester (originating) port
struct _SMBPBPortRequester
{
   UCHAR ucPBAddr;                                                      //!< Req mode: complete requester address of received frame (set by requester in its own data) (0x00 = req mode _in_active)
   UCHAR ucPortBusID;                                                   //!< Fwd mode: PortBusID the forwarding port will forward the reply to after a sent forwarded request (determined from the forward request message) (0x00 = fwd mode _in_active)
};

//! \internal \brief <b>Internal data:</b> Forward communicater/forwarder port
struct _SMBPBPortForwarder
{
   OS_ID oiTmr;                                                         //!< Fwd mode: OS_ID of running (timeout) timer (0= none) (set by forwarder)
   UCHAR ucPBAddr;                                                      //!< Fwd mode: complete portbus address storage (set by forwarder by reading requester's ucPBAddr)
};

//! \internal \brief <b>Internal data:</b> Port read only data
struct _SMBPBPortRO
{
   struct _SMBPBFPTRsDriver SDriver;
   struct _SMBPBFPTRsAux SAux;
   ULONG ulMsgBitRx;                                                    //!< Bit for message Rx (character received)
   ULONG ulMsgBitTxDone;                                                //!< Bit for message Tx (transmit complete)   
   ULONG ulMsgBitTo;                                                    //!< Bit for message To (time out)   
   struct _SSemaphore SSemMsg;                                          //!< OS_ID of semaphore to signal new message(s)
   EMBPBMode EModeMax;                                                  //!< Highest mode supported (and configured)
   UCHAR ucPortBusID;                                                   //!< PortBusID (covered by MBPB_PORTBUSID_MASK bits) of address byte
   bool bActive;                                                        //!< Flag indicating port is active
};

//! \internal \brief <b>Internal data:</b> Port read/write data
struct _SMBPBPortRW
{
   struct _SSemaphore SSemLock;                                         //!< OS_ID of semaphore to avoid data write interruption
   struct _SMBPBPortRequester SReq;                                     //!< Data storage in requester mode (of forwarding/backwarding)
   struct _SMBPBPortForwarder SFwd;                                     //!< Data storage in forwarder mode (of forwarding/backwarding)
   ULONG ulMsgMask;                                                     //!< Bit mask for incoming driver messages to recognize
   EMBPBMode EMode;                                                     //!< Current mode of this modbus port
};

//! \internal \brief <b>Internal data:</b> Parser function pointers
struct _SMBPBFPTRsParser
{
   EMBPBErrorCode (*Enable) (struct _SMBPBPara *pSMBPBPara);            //!< Enable the parser (active state) (e.g. #MBPBSerParserEnable)
   EMBPBErrorCode (*Start) (struct _SMBPBPara *pSMBPBPara);             //!< Prepare port for active parser (e.g. #MBPBSerParserStart)
   EMBPBErrorCode (*Stop) (struct _SMBPBPara *pSMBPBPara);              //!< Prepare port for inactive parser (e.g. #MBPBSerParserStop)
   EMBPBErrorCode (*Verify) (struct _SMBPBPara *pSMBPBPara);            //!< Verify decoded RAW binary data (e.g. #MBPBSerParserVerify)
   EMBPBErrorCode (*Cook) (struct _SMBPBPara *pSMBPBPara);              //!< Encode full buffer at once to active telegram style (#EMBPBMode) for block type transfer (e.g. #MBPBSerParserCook)
   UCHAR (*AddrGet) (struct _SMBPBPara *SMBPBPara);                     //!< Retrieve device address of current internal/raw (binary) buffer (e.g. #MBPBSerAddrGet)
   UCHAR (*AddrSet) (struct _SMBPBPara *pSMBPBPara, UCHAR ucAddr);      //!< Set device address of current internal/raw (binary) buffer (e.g. #MBPBSerAddrSet)
   UCHAR (*AddrFix) (struct _SMBPBPara *pSMBPBPara);                    //!< Determine, limit and set address of current internal/raw (binary) buffer (e.g. #MBPBSerAddrFix)
   USHORT (*LenGet) (struct _SMBPBPara *pSMBPBPara);                    //!< Determine length of current internal/raw (binary) buffer (e.g. #MBPBSerLenGet)
   USHORT (*LenFix) (struct _SMBPBPara *pSMBPBPara);                    //!< Fix length of current buffer (new send code requires length to include address and checksum bytes) (e.g. #MBPBSerLenFix)
   UCHAR* (*DataGet) (struct _SMBPBPara *pSMBPBPara);                   //!< Get address of data of current internal/raw (binary) buffer (skips frame prefix) (e.g. #MBPBSerDataGet)
   ULONG (*TimeOutCalc) (EMBPBMode EMode, ULONG ulBaudRate);            //!< Determine time-out (us!) for specified mode (and baud rate) (e.g. #MBPBSerTimeOutCalc)
};

//! <b>Internal data:</b> Function pointers for message bits
struct _SMBPBFPTRsMsg
{
   void (*NotifyBitRx) (struct _SMBPBPara  *pSMBPBPara, USHORT usCount, UCHAR *pucPrevAr, UCHAR *pucPrevIdx);//!< Function to call for message rx (receive)
   void (*NotifyBitTxDone) (struct _SMBPBPara  *pSMBPBPara, USHORT usCount);                                 //!< Function to call for message tx done (transmit complete)
   void (*NotifyBitTo) (struct _SMBPBPara  *pSMBPBPara, USHORT usCount);                                     //!< Function to call for message to (time out)
};

//! \internal \brief <b>Internal data:</b> Parser mode read only data
struct _SMBPBModeRO
{
   struct _SMBPBFPTRsParser SParser;                                    //!< Parser function pointers
   struct _SMBPBFPTRsTranslator STranslator;                            //!< Translation function pointers
   struct _SMBPBFPTRsMsg SMessage;                                      //!< Function pointers for message bits
   struct _SMailbox SMbxEvent;                                          //!< OS_ID of mailbox for state handling of this modbus port
};

//! \internal \brief <b>Internal data:</b> Previous received data storage (for auto replacement of '??' with valid checksum)
struct _SMBPBPrevRW
{
   struct _SSemaphore SSemLock;                                         //!< OS_ID of semaphore to avoid data write interruption
   UBYTE ucDataAr[MBPB_ASC_PDU_SIZE_LRC* 2];                            //!< Data storage
   UBYTE ucIdx;                                                         //!< Storage index
};

//! \internal \brief <b>Internal data:</b> Parser mode read/write data
struct _SMBPBModeRW
{
   struct _SSemaphore SSemLock;                                         //!< OS_ID of semaphore to avoid data write interruption
   EMBPBStateRx EStateRx;                                               //!< State of the receiver of this modbus port
   EMBPBStateTx EStateTx;                                               //!< State of the transmitter of this modbus port
   EMBPBState EState;                                                   //!< State of this modbus port
   UCHAR ucTranslating;                                                 //!< Flag for enabling translation function pointers
   ULONG ulTimeOutMS;                                                   //!< Time out for data in ms   
   EMBPBBytePos EBytePos;                                               //!< ASC: Digit position within number
   UCHAR ucCR;                                                          //!< ASC: Current CR character
   UCHAR ucLF;                                                          //!< ASC: Current LF character
   struct _SMBPBPrevRW SPrevRW;                                         //!< Previous received data storage (for auto replacement of '??' with valid checksum)
};

//! \internal \brief <b>Internal data:</b> Buffer read only data
struct _SMBPBBufferRO
{
   USHORT usSize;                                                       //!< Size of this buffer in bytes
};

//! <b>Internal data:</b> Buffer read/write data
struct _SMBPBBufferRW
{
   struct _SSemaphore SSemLock;                                         //!< OS_ID of semaphore to avoid data write interruption
   UCHAR *pucData;                                                      //!< Pointer to memory[usSize] for this buffer
   USHORT usPos;                                                        //!< Cursor position inside this buffer
   USHORT usCnt;                                                        //!< Number of bytes allocated inside this buffer
};

//! <b>Internal data:</b> Param structure for task(s)
struct _SMBPBPara
{
   struct _SMBPBPortRO SPortRO;                                         //!< Parser mode read only data
   struct _SMBPBPortRW SPortRW;                                         //!< Parser mode read/write data
   struct _SMBPBModeRO SModeRO[MBPB_MODE_COUNT];                        //!< Parser mode read only data
   struct _SMBPBModeRW SModeRW[MBPB_MODE_COUNT];                        //!< Parser mode read/write data
   struct _SMBPBBufferRO SBufferRO[MBPB_BUFFER_COUNT];                  //!< Buffer read only data (#MBPB_BUFFER_COUNT)
   struct _SMBPBBufferRW SBufferRW[MBPB_BUFFER_COUNT];                  //!< Buffer read/write data (#MBPB_BUFFER_COUNT)
};


// Application data structures

typedef EMBPBException (*MBPBFunctionHandler) (UCHAR *pucFrame, USHORT *pusLength, void *pvData);

//! \internal \brief <b>Internal data:</b> ModBusPortBus command code handler
struct _SMBPBFPTRsHandler
{
   UCHAR ucCode;                                                        //!< Command code
   MBPBFunctionHandler Function;                                        //!< Command code handler
};

//! \internal \brief <b>Internal data:</b> Maximum data item count for register type with read-only access
struct _SMBPBMaxItemsRO
{
   USHORT usRead;                                                                                           //!< Maximum data item count for read access
};

//! \internal \brief <b>Internal data:</b> Maximum data item count for register type with read/write access
struct _SMBPBMaxItemsRW
{
   USHORT usRead;                                                                                           //!< Maximum data item count for read access
   USHORT usWrite;                                                                                          //!< Maximum data item count for write access
};

//! \internal \brief <b>Internal data:</b> Maximum data item count for register type with read/write and combined read+write access
struct _SMBPBMaxItemsRWCombo
{
   USHORT usRead;                                                                                           //!< Maximum data item count for read access
   USHORT usReadCombo;                                                                                      //!< Maximum read data item count for combined read+write access
   USHORT usWrite;                                                                                          //!< Maximum data item count for write access
   USHORT usWriteCombo;                                                                                     //!< Maximum write data item count for combined read+write access
};

//! \internal \brief <b>Internal data:</b> Maximum item counts for all register types
struct _SMBPBMaxItems
{
   struct _SMBPBMaxItemsRWCombo SHolding;                                                                   //!< Maximum data item counts for holding register type
   struct _SMBPBMaxItemsRW SCoils;                                                                          //!< Maximum data item counts for coils register type
   struct _SMBPBMaxItemsRO SInput;                                                                          //!< Maximum data item counts for input register type
   struct _SMBPBMaxItemsRO SDiscrete;                                                                       //!< Maximum data item counts for discrete register type
   struct _SMBPBMaxItemsRW SGeneral;                                                                        //!< Maximum data item counts for general reference register type
};

//! \internal \brief <b>Internal data:</b> ModBusPortBus device indentification
struct _SMBPBProduct
{
   ULONG ulClass;                                                       //!< Device class
   UCHAR ucDeviceLen;                                                   //!< Device identification string length
   UCHAR ucDevice[MBPB_PRODUCT_DEVICE_LENGTH];                          //!< Device identification string
};

//! Trigger item
struct _SMBPBTriggerItem
{
   USHORT usAddrMask;                                                   //!< Adress mask (address & mask == address)
   UCHAR ucR;                                                           //!< Enable read trigger
   UCHAR ucW;                                                           //!< Enable write trigger
};

//! Value filter
struct _SMBPBFilterRange
{
   USHORT usMin;                                                        //!< Minimum value
   USHORT usMax;                                                        //!< Maximum value
};

//! Filter item
struct _SMBPBFilterItem
{
   bool bActive;                                                        //!< State (TRUE == active, FALSE == inactive)
   UCHAR ucRWMask;                                                      //!< Read/write mask (refer to 'MBPB_TRGMSG_RWMASK_...' of mbpbmsg.h)
   USHORT usAddrMask;                                                   //!< Adress mask (address & mask == address)
   USHORT usValMask;                                                    //!< Value mask (value & mask == value)
   struct _SMBPBFilterRange SAddrRange;                                 //!< Address filter (range)
   struct _SMBPBFilterRange SValRange;                                  //!< Value filter (range)
   struct _SMailbox SMbxTsk;                                            //!< Mailbox to write matches to
};

//! List of filter items
struct _SMBPBFilterList
{
   struct _SMBPBFilterItem STrgItem[MBPB_FILTERLIST_COUNT];             //!< Filter items
   USHORT usCount;                                                      //!< Count of filter items
};

//! <b>Internal data:</b> Trigger data
struct _SMBPBTriggerMessage
{
   struct _SSemaphore SSemLock;                                         //!< OS_ID of semaphore to avoid data write interruption
   struct _SMailbox SMbxTrg;                                            //!< Mailbox for trigger filter task
   struct _SMBPBTriggerItem STrg[MBPB_TRGTYPE_COUNT];                   //!< List of trigger items
   struct _SMBPBFilterList SFilter[MBPB_TRGTYPE_COUNT];                 //!< List List of filter items
};

//! \internal \brief <b>Internal data:</b> Register access semaphores
struct _SReg
{
   struct _SSemaphore SSemLockHolding;                                  //!< 16 bit read/write register type
   struct _SSemaphore SSemLockInput;                                    //!< 16 bit read ONLY register type
   struct _SSemaphore SSemLockCoils;                                    //!< single bit read/write register type
   struct _SSemaphore SSemLockDiscrete;                                 //!< single bit read ONLY register type
};

//! \internal \brief <b>Internal data:</b> ModBusPortBus read only data
struct _SMBPBAppDataRO
{
   struct _SMBPBFPTRsHandler SHandler[MBPB_HANDLERS_MAX];               //!< List of command code handlers
   bool (*Validate) (UCHAR ucType, U32 u32Address, USHORT *pusValue);   //!< Validation function for received (and already to binary converted) telegrams with write requests
   struct _SMBPBMaxItems SMaxItems[MBPB_MODE_COUNT];                    //!< Maximum item counts for all register types
};

//! \internal \brief <b>Internal data:</b> ModBusPortBus read/write data
struct _SMBPBAppDataRW
{
   struct _SSemaphore SSemLock;                                         //!< OS_ID of semaphore to avoid data write interruption
   UCHAR ucAddr;                                                        //!< ModBusPortBus address
   EMBPBRunState ERunState;                                             //!< Slave device run state
   OS_TID tidTskTrg;                                                    //!< OS_TID of trigger task
   OS_TID tidTskMsg[MBPB_PORT_COUNT];                                   //!< OS_TIDs of message tasks
   OS_TID tidTskData[MBPB_PORT_COUNT];                                  //!< OS_TIDs of data tasks
   struct _SMBPBProduct SProduct;                                       //!< ModBusPortBus device indentification
   struct _SMBPBTriggerMessage STrgMsg;                                 //!< Trigger data
   struct _SReg SReg;                                                   //!< Register access semaphores
};

//! \internal \brief <b>Internal data:</b> ModBusPortBus data
struct _SMBPBApp
{
   struct _SMBPBAppDataRO SDataRO;                                      //!< ModBusPortBus read only data
   struct _SMBPBAppDataRW SDataRW;                                      //!< ModBusPortBus read/write data
};

//! \internal \brief <b>Internal data:</b> ModBusPortBus data for tasks
struct _SMBPBTskPara
{
   struct _SMBPBPara *pSMBPBPara;                                       //!< Param structure for task(s)
   struct _SMBPBApp *pSMBPBApp;                                         //!< ModBusPortBus data
};

#endif
