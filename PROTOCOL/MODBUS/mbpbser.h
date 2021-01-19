//!	<b>INTERNAL</b> interface of MBPB serial support (ModBusPortBus)
/**
		\file		mbpbser.h
*/

//----------------------------------------------------------------------------
// C O M M U N I C A T I O N   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/PROTOCOL/MODBUS/MBPBSER.H
// Purpose: COMMUNICATION SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2009-08-26 moved here from mbpb.c
// 2009-09-03 added MBPBTimeOutCalc prototype
// 2009-09-04 changed MBPBTimeOutCalc to MBPBSerTimeOutCalc and its params
// 2009-09-09 added MBPBSerAddrSet and MBPBSerAddrFix
// 2009-10-01 changed MBPBSerNotifyRx prototype
// 2010-07-28 fixed includes for "include cleanuped" framework by rradtke
// 2010-08-12 added prototype for MBPBSerSyncPrepareRx
// 2010-09-08 added prototypes for MBPBSerEncode and MBPBSerDecode
// 2010-09-15 added prototype for MBPBSerCookedReplyBytes
// 2011-10-27 added public doxygen documentation
// 2011-11-30 added internal doxygen documentation
// 2013-01-24 imported MBPBSerParserMaximum from VCPP implementation
// 2013-01-24 changed Count of MBPBSerNotifyRx to USHORT
// 2013-01-24 changed Count of MBPBSerNotifyTxDone to USHORT
// 2013-01-24 changed Count of MBPBSerNotifyBitTo to USHORT
// 2013-02-13 renamed MBPBSerNotifyTo to MBPBSerNotifyExTo and added param
// 2013-02-13 added MBPBSerNotifyTo wrapper for MBPBSerNotifyExTo
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_PROTOCOL_MODBUS_MBPBSER_H
#define SYKAM_PROTOCOL_MODBUS_MBPBSER_H 1


// Prototypes

//! \internal \brief <b>Internal function:</b> queue reply data for syncronous drivers to enable them to receive again
/**   
      \param	*pvParam					      param structure #_SMBPBPara for task(s) <b>[*pvParam == struct _SMBPBPara *pSMBPBPara]</b>

      \return                             > 0 for success, == 0 for failure (result of the driver's tx function)
*/
USHORT MBPBSerSyncPrepareRx(void *pvParam);

//! \internal \brief <b>Internal function:</b> determine time-out (us!) for specified mode (and baud rate)
/**   
      \param	EMode					         telegram style/mode (#EMBPBMode)
      \param	ulBaud					      baud rate

      \return                             time-out <b>(us!)</b>
*/
ULONG MBPBSerTimeOutCalc (EMBPBMode EMode, ULONG ulBaud);

//! \internal \brief <b>Internal function:</b> retrieve device address of current internal/raw (binary) buffer
UCHAR MBPBSerAddrGet(struct _SMBPBPara *pSMBPBPara);

//! \internal \brief <b>Internal function:</b> set device address of current internal/raw (binary) buffer
/**   
      \param	*pSMBPBPara					   param structure #_SMBPBPara for task(s)
      \param	ucAddr					      device address

      \return                             value of ucAddr
*/
UCHAR MBPBSerAddrSet(struct _SMBPBPara *pSMBPBPara, UCHAR ucAddr);

//! \internal \brief <b>Internal function:</b> determine, limit and set address of current internal/raw (binary) buffer
UCHAR MBPBSerAddrFix(struct _SMBPBPara *pSMBPBPara);

//! \internal \brief <b>Internal function:</b> determine length of current internal/raw (binary) buffer
USHORT MBPBSerLenGet(struct _SMBPBPara *pSMBPBPara);

//! \internal \brief <b>Internal function:</b> fix length of current internal/raw (binary) buffer (new send code requires length to include address and checksum bytes)
USHORT MBPBSerLenFix(struct _SMBPBPara *pSMBPBPara);

//! \internal \brief <b>Internal function:</b> get address of data of current internal/raw (binary) buffer (skips frame prefix)
UCHAR * MBPBSerDataGet(struct _SMBPBPara *pSMBPBPara);


//! \internal \brief <b>Internal function:</b> handle received bytes message with 'ucCount' (decode received bytes into raw binary by partially processing of 'ucCount' bytes per call)
/**   
      \param	*pSMBPBPara					   param structure #_SMBPBPara for task(s)
      \param	usCount					      count of characters received for this message (count of single character receive messages)
      \param	*pucPrevAr   	            port data storage (two bytes) for automatic checksum replacement of dummy "??" checksum
      \param	*pucPrevIdx				      index of port data storage (one byte) for automatic checksum replacement of dummy "??" checksum

      <b>Example code:</b>
      \include TskMBPBMsg.c
*/
void MBPBSerNotifyRx(struct _SMBPBPara *pSMBPBPara, USHORT usCount, UCHAR *pucPrevAr, UCHAR *pucPrevIdx);

//! \internal \brief <b>Internal function:</b> handle tx done with 'ucCount' message (all queued bytes are sent)
/**   
      \param	*pSMBPBPara					   param structure #_SMBPBPara for task(s)
      \param	usCount					      count of message

      <b>Example code:</b>
      \include TskMBPBMsg.c
*/
void MBPBSerNotifyTxDone(struct _SMBPBPara *pSMBPBPara, USHORT usCount);

//! \internal \brief <b>Internal function:</b> handle 'to' (time-out) with 'ucCount' message
/**   
      \param	*pSMBPBPara					   param structure #_SMBPBPara for task(s)
      \param	usCount					      count of message

      <b>Example code:</b>
      \include TskMBPBMsg.c
*/
void MBPBSerNotifyTo(struct _SMBPBPara *pSMBPBPara, USHORT usCount);

//! \internal \brief <b>Internal function:</b> handle 'to' (time-out) with 'ucCount' message with state-change control
/**   
      \param	*pSMBPBPara					   param structure #_SMBPBPara for task(s)
      \param	usCount					      count of message
      \param	bStateChange					true to enable state-change control (re-enable packet receive)

      <b>Example code:</b>
      \include TskMBPBMsg.c
*/
void MBPBSerNotifyExTo(struct _SMBPBPara *pSMBPBPara, USHORT usCount, bool bStateChange);

//! \internal \brief <b>Internal function:</b> enable the parser (active state)
EMBPBErrorCode MBPBSerParserEnable(struct _SMBPBPara *pSMBPBPara);

//! \internal \brief <b>Internal function:</b> disable the parser (will ignore requests)
EMBPBErrorCode MBPBSerParserDisable(struct _SMBPBPara *pSMBPBPara);


//! \internal \brief <b>Internal function:</b> prepare port for active parser
EMBPBErrorCode MBPBSerParserStart(struct _SMBPBPara *pSMBPBPara);

//! \internal \brief <b>Internal function:</b> prepare port for inactive parser
EMBPBErrorCode MBPBSerParserStop(struct _SMBPBPara *pSMBPBPara);

//! \internal \brief <b>Internal function:</b> verify decoded RAW binary data
EMBPBErrorCode MBPBSerParserVerify (struct _SMBPBPara *pSMBPBPara);

//! \internal \brief <b>Internal function:</b> encode full buffer at once to active telegram style for block type transfer
EMBPBErrorCode MBPBSerParserCook (struct _SMBPBPara *pSMBPBPara);


//!   encode RAW BINARY data to specified TELEGRAM STYLED (cooked) data
/**   
      \param	EMode					         telegram style to encode raw binary data to (#EMBPBMode)
      \param	ucCR					         binary byte value for "carriage return" (set #MBPB_ASC_DEFAULT_CR == 0x0a == \\r for default)
      \param	ucLF   	                  binary byte value for "line feed" (set #MBPB_ASC_DEFAULT_LF == 0x0d == \\n for default)
      \param	*pucRaw				         binary data to encode to telegram in specified style
      \param   usRawCnt                   size of the binary raw data <b>PLUS ONE byte for the checksum</b>
      \param   *pucCooked                 storage for the telegram in specified style to create
      \param   usCookedLen                size of the storage for the telegram in specified style to create

      \return										the byte count to transmit the complete telegram in the specified telegram style

      \note
               - <b>usRawCnt MUST BE INCREASED BY ONE to reserve the space for the checksum byte of the telegram!!!</b><br>
               - 'raw data' == RAW and 'telegram styled data' = COOKED here<br>

      \remarks - usual setting for ucCR is #MBPB_ASC_DEFAULT_CR == 0x0a == \\r and usual setting for ucLF is #MBPB_ASC_DEFAULT_LF == 0x0d == \\n (\\n)<br>

      <b>Example code:</b>
      \include MBPBSerEncode_MBPBSerCookedReplyBytes.c
*/
USHORT MBPBSerEncode(EMBPBMode EMode, CHAR ucCR, UCHAR ucLF, UCHAR *pucRaw, USHORT usRawCnt, UCHAR *pucCooked, USHORT usCookedLen);

//!   decode specified TELEGRAM STYLED (cooked) to RAW BINARY data
/**
      \param	EMode					         telegram style to encode raw binary data to (#EMBPBMode)
      \param	ucCR					         binary byte value for "carriage return" (set #MBPB_ASC_DEFAULT_CR == 0x0a == \\r for default)
      \param	ucLF   	                  binary byte value for "line feed" (set #MBPB_ASC_DEFAULT_LF == 0x0d == \\n for default)
      \param	*pucCooked				      telegram styled (cooked) data
      \param   usCookedCnt                size of the telegram styled (cooked) data
      \param   *pucRaw                    storage for the raw binary data to create
      \param   usRawLen                   size of the storage for the raw binary data to create

      \return										the byte count of the created (decoded) raw binary data

      \note
               - <b>EMode must fit the telegram style of your data!!!</b><br>
               - 'raw data' == RAW and 'telegram styled data' = COOKED here<br>

      \remarks - usual setting for ucCR is #MBPB_ASC_DEFAULT_CR == 0x0a == \\r and usual setting for ucLF is #MBPB_ASC_DEFAULT_LF == 0x0d == \\n (\\n)<br>
*/
USHORT MBPBSerDecode(EMBPBMode EMode, CHAR ucCR, UCHAR ucLF, UCHAR *pucCooked, USHORT usCookedCnt, UCHAR *pucRaw, USHORT usRawLen);

//!   determine the specified TELEGRAM STYLED (cooked) byte count to exspect for specified RAW BINARY REQUEST data <b>(required for synchronous ports)</b>
/**
      \param	EMode					         telegram style for calculation (#EMBPBMode)
      \param   *pucRaw                    binary data of the request
      \param   usRawLen                   the length of the binary data of the request to analyze

      \return										count of bytes for the specified TELEGRAM STYLED (cooked) reply for the specified raw binary request

      \note
               - <b>you specify the RAW BINARY request to get the specified TELEGRAM STYLED (cooked) byte count to exspect!!!</b><br>
               - 'raw data' == RAW and 'telegram styled data' = COOKED here<br>

      <b>Example code:</b>
      \include MBPBSerEncode_MBPBSerCookedReplyBytes.c
*/
USHORT MBPBSerCookedReplyBytes(EMBPBMode EMode, UCHAR *pucRaw, USHORT usRawLen);

//! \internal \brief <b>Internal function:</b> enable/disable the parser (active state)
/**   
      \param	*pSMBPBPara					   param structure #_SMBPBPara for task(s)
      \param	bEnable					      enable flag (#TRUE == enable)

      \return                             #EMBPBErrorCode
*/
EMBPBErrorCode MBPBSerParserChange(struct _SMBPBPara *pSMBPBPara, BOOL bEnable);

//! \internal \brief <b>Internal function:</b> determine maximum count of read and write registers of specified type (to keep request & reply within specified buffer size limits)
/**
      \param   EMode                      transmit mode (style) to set (#MBPB_MODE_ASC, #MBPB_MODE_RTU)
      \param   ERegType                   type of register (#EMBPBRegType)
      \param   usCountRegType             count of registers (read or write access)
      \param   usCountRegType2            count of registers (write access, for combined read and write access only, otherwise set to 0)
      \param   ERW                        mode of access (#EMBPBRegRW)
      \param   usRequestBytesMax          maximum byte count of request buffer (#MBPB_SER_PDU_SIZE_MAX)
      \param   usReplyBytesMax            maximum byte count of reply buffer (#MBPB_SER_PDU_SIZE_MAX)
      \param   *pusCountRegTypeMax        storage for maximum count of registers (read or write access)
      \param   *pusCountRegTypeMax2       storage for maximum count of registers (write access, for combined read and write access only, otherwise set to NULL)

      \return                             > 0 for success, == 0 for failure
*/
bool MBPBSerParserMaximum(EMBPBMode EMode, EMBPBRegType ERegType, USHORT usCountRegType, USHORT usCountRegType2, EMBPBRegRW ERW, USHORT usRequestBytesMax, USHORT usReplyBytesMax, USHORT *pusCountRegTypeMax, USHORT *pusCountRegTypeMax2);

#endif
