//!	extended data definitions for RTL
/**
		\file		support.h
*/

//----------------------------------------------------------------------------
// R T L   K E R N E L   S U P P O R T
//----------------------------------------------------------------------------
// Name:    SYKAM/RTL/SUPPORT.H
// Purpose: RTL KERNEL SUPPORT
// Author:  hschoettner
//----------------------------------------------------------------------------
// 2008-11-14 extracted from evaluation.c; reorganized and enhanced
// 2008-11-19 added bit and event definitions
// 2008-11-19 added name wrappers for unusual named semaphore functions of RTL
// 2008-11-20 added name warppers for unusual named mutex functions of RTL
// 2008-11-20 added RESCHEDULE macro, added size verification to type defs
// 2008-11-20 added os_tmr wrappers to offer signal feature
// 2008-11-21 changed BIT defs to avoid conflicts with RTL definitions
// 2008-11-21 changed type definitions to true typedefs incl. RTL BOOL notice
// 2008-11-21 changed bool typedef to be still safe also when cpp is used
// 2008-12-04 initial version of circular buffer tool
// 2008-12-05 circular buffer tool completed
// 2008-12-05 initial version of memcpyflex
// 2008-12-05 completed type independence of circular buffer
// 2008-12-12 added AT91F_US_ReadPDC(...)
// 2008-12-17 added name wrapper isr_sem_give for unusual name isr semaphore
// 2008-12-18 additional typesize circular buffer to allow buffer > itemsize
// 2008-12-19 removed buggy AT91F_US_ReadPDC(...)
// 2008-12-22 added debug mode for memcpyflex; fixed sizeof calcs in circular
// 2008-12-23 added 2nd destination memory limit check for circular_write
// 2008-12-23 macro OS_MBX_INIT added
// 2009-01-08 moved non inline functions to support.c
// 2009-06-11 added macro section for semaphore
// 2009-06-26 added MSG macros for count+message combo handling
// 2009-06-26 renamed MSG macros and made MSG macro bit width configurable
// 2009-07-29 added warning 550 ignore when DEBUG is set
// 2009-07-30 added _SSemaphore struct and macros
// 2009-08-04 added _SSemaphore macros for easy atomic read and write access
// 2009-08-05 added _SMailbox struct and macros
// 2009-08-06 added critical section macros
// 2009-08-13 added SSEM_LOCKUSR and SSEM_TAKEUSR to force user given timeout
// 2009-09-02 fixed atomic access macros
// 2009-09-24 enhanced circular buffer tool for pointer referenced access
// 2009-09-24 added os_mbx_declare_ex to support arrays of mailboxes
// 2009-09-25 changed param order of os_mbx_declare_ex
// 2009-09-25 changed circular buffer to support arrays of circular buffers
// 2009-09-30 added macro OS_MBX_INIT_EX (required for working arrays of mbx)
// 2009-10-08 changed circular_declare_ex to support x-dimensional arrays yet
// 2009-10-08 changed os_mbx_declare_ex to support x-dimensional arrays yet
// 2009-10-15 added macro RAISEMAX
// 2009-10-21 added BITS_UCHARH and BITS_UCHARL
// 2009-12-23 added circular_wipe(p) to read from buffer without target memory
// 2009-12-23 added OS_TSKSTACK_USR and OS_TSKSTACK_USR_EX for user stack def.
// 2010-01-07 renamed OS_TSKSTACK_USR(_EX) to OS_TSK_STACK_USER(_EX)
// 2010-01-07 added OS_TSK_CREATE_USER and OS_TSK_CREATE_USER_EX
// 2010-01-20 added OS_SYS_INIT_USER
// 2010-01-21 added ISR_MBX_VALUE..., BITS_ULONGHX/BITS_ULONGLX
// 2010-01-29 added safety check for os_sem_give and os_sem_take
// 2010-01-29 added OS_MUT_TAKE_CHK
// 2010-01-29 added safety check for os_mut_give and os_mut_take
// 2010-02-05 added OS_MBX_CHECK
// 2010-02-11 added isr_mbx_write (immediate update version of isr_mbx_send)
// 2010-02-11 added isr_mbx_read (task free version of isr_mbx_receive)
// 2010-02-18 added ABS macro (extension for c55x.h)
// 2010-04-15 added SMBX_CHECK
// 2010-04-30 added BITS_OF() macro
// 2010-05-21 added SWAP_USHORT and SWAP_ULONG macros
// 2010-05-27 changed RESCHEDULE macro and task priorities
// 2010-07-01 fixed item init of circular to support multi single dimensions
// 2010-07-02 modified typedefs CHAR, SHORT, INT, LONG & LONGLONG to signed
// 2010-07-08 fixed typedef ULONGLONG
// 2010-07-28 added lower case data types (suggestion of rradtke)
// 2010-07-28 fixed OS_T_MS() macro (based on tests & suggestion of rradtke)
// 2010-07-29 completed lower case data types
// 2010-09-02 added LOCKED macro (supersedes old ATOMIC macros)
// 2010-09-02 removed obsolete ATOMIC_READ, ATOMIC_WRITE and ATOMIC macro
// 2010-09-22 added _u(l)min & _u(l)max intrinsics to extend available c55x.h
// 2010-09-23 changed circular buffer macros to use _ulmin instead of _min
// 2010-09-23 changed BITX(), SIGNAL() & EVENT() to use _umin instead of _min
// 2010-09-23 changed OS_T_MS() macro to use _ulmin/_ulmax
// 2010-10-27 added os_sem_check (non-destructive version of os_sem_wait)
// 2010-10-28 added _OS_T_MS for constant initialization/definition
// 2010-12-09 renamed param ticks to ostickrate in macro _OS_T_MS
// 2011-01-27 added OS_P_MAX priority define
// 2011-01-27 added external force_irq_enable flag
// 2011-01-27 added (UN)LOCK_FORCECTRL to control forced runs of scheduler
// 2011-02-16 added external irq_lock flag
// 2011-02-16 added setting of irq_lock to LOCK/UNLOCK to avoid scheduler runs
// 2011-02-17 fixed _OS_T_MS macro for constant initialization
// 2011-02-24 enhanced os_sem_check to sem_check & added wrappers for os_/isr_
// 2011-02-24 added macros ISR_SEM_TAKE_CHK and ISR_MUT_TAKE_CHK
// 2011-05-02 rr: fixed circular_declare macro primary initialization
// 2011-05-02 rr: removed double init in macro circular_reset
// 2011-05-02 rr: added ringbuffer macros (declare, reset, write, read...)
// 2011-05-04 fixed primary init in macro circular_declare_ex
// 2011-05-04 added macro ringbuffer_declare_ex and circular_size
// 2011-05-04 changed anonymous param data to source & dest in circular macros
// 2011-05-04 replaced ringbuffer macros of rr by a version based on circular
// 2011-05-11 undo fix of primary init in macro circular_declare_ex
// 2011-06-01 added forced ULONG type casting to message combo macros (MSG_)
// 2011-07-20 renamed SWAP_... macros to BSWAP_...
// 2011-07-20 added BSWAP_USHORT32 (two ushorts within an ulong) macro
// 2011-07-20 fixed BSWAP_ULONG macro and added forced type cast to argument
// 2011-08-04 added tmr & TIMER as synonyms for previously defines SIGNAL & sig
// 2011-08-10 added SMBX_ISR_SEND and SMBX_ISR_CHECK macros
// 2011-08-10 added SSEM_ISR_GIVE and SSEM_ISR_UNLOCK macros
// 2011-08-17 added _TIMER(x) and _SIGNAL(x) macros for constant initialization
// 2011-09-15 added GET_BITS_ULONGHX/LX macros supplied by rradtke
// 2011-09-16 added new prototypes os_mbx_clear, os_mbx_read & os_mbx_write
// 2011-09-16 modified RESCHEDULE macro to call os_tsk_pass for zero argument
// 2011-09-28 added new prototypes os_mbx_count and isr_mbx_count
// 2011-10-26 added public doxygen documentation
// 2011-11-30 added public doxygen documentation
// 2013-06-13 added macros for safe SoftWareInterrupts (SWI) usage
// 2013-06-20 added public doxygen documentation for SoftWareInterrupts (SWI)
// 2013-08-21 added RESCHEDULE_TICKS for tick argument support
// 2013-08-21 changed _OS_T_MS to return zero for given zero argument(s)
// 2013-08-21 optimized OS_T_MS and _OS_T_MS
// 2013-08-21 changed behaviour of optimized OS_T_MS to style of _OS_T_MS (zero args return zero now)
// 2014-11-26 updated public doxygen documentation for os_sem_check
// 2014-12-18 added missing ATOMIC_LOWERMIN to complete macro section
//----------------------------------------------------------------------------
// Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
//----------------------------------------------------------------------------

#ifndef SYKAM_RTL_SUPPORT_H
#define SYKAM_RTL_SUPPORT_H 1


// Includes
#include <c55x.h>
#include <string.h>

#include <RTL.h>

// Suppress warning for "#111-D: statement is unreachable" to allow ALWAYS the macro DETACH at tasks end
#pragma diag_remark 111

// Suppress warning for "550-D: variable was set but never used" for debugging
#ifdef DEBUG
#pragma diag_remark 550
#endif


// Externals

extern U32 const os_clockrate;
extern U32 os_timernum;
extern BIT irq_lock;
extern BIT force_irq_enable;


// Definitions


// Task priorities

//!   <b>Task priority:</b>: maximum (maximum available)
#define OS_P_MAX           255

//!   <b>Task priority:</b>: realtime
#define OS_P_REALTIME      240

//!   <b>Task priority:</b>: above high
#define OS_P_ABOVEHIGH     210

//!   <b>Task priority:</b>: high
#define OS_P_HIGH          180

//!   <b>Task priority:</b>: above normal
#define OS_P_ABOVENORMAL   150

//!   <b>Task priority:</b>: normal
#define OS_P_NORMAL        120

//!   <b>Task priority:</b>: below normal
#define OS_P_BELOWNORMAL   90

//!   <b>Task priority:</b>: low
#define OS_P_LOW           60

//!   <b>Task priority:</b>: below low
#define OS_P_BELOWLOW      30

//!   <b>Task priority:</b>: idle (minimum available)
#define OS_P_IDLE          0


// Booleans

//!   <b>Boolean:</b> true
#ifndef TRUE
   #define TRUE 1
#endif

//!   <b>Boolean: </b> false
#ifndef FALSE
   #define FALSE 0
#endif


// Types

// CAUTION: RTL already defined "BOOL" and "BIT" (all upercase)

//!   <b>Data type:</b> boolean (8 bit) (refer also to #BIT)
#ifndef _BOOL
   typedef unsigned char bool;
#endif

//!   <b>Data type:</b> signed char (8 bit)
typedef signed char CHAR;

//!   <b>Data type:</b> unsigned char (8 bit)
typedef unsigned char UCHAR;

//!   <b>Data type:</b> unsigned char (8 bit)
typedef unsigned char uchar;


//!   <b>Data type:</b> signed short (16 bit)
typedef signed short SHORT;

//!   <b>Data type:</b> unsigned short (16 bit)
typedef unsigned short USHORT;

//!   <b>Data type:</b> unsigned short (16 bit)
typedef unsigned short ushort;


#if __sizeof_int == 4
   //!   <b>Data type:</b> signed int (32 bit)
   typedef signed long INT;
   
   //!   <b>Data type:</b> unsigned int (32 bit)
   typedef unsigned long UINT;
   
   //!   <b>Data type:</b> un signed int (32 bit)
	typedef unsigned int uint;
#endif
 
#if __sizeof_long == 4
   //!   <b>Data type:</b> signed long (32 bit)
   typedef signed long LONG;
   
   //!   <b>Data type:</b> unsigned long (32 bit)
   typedef unsigned long ULONG;
   
   //!   <b>Data type:</b> unsigned long (32 bit)
	typedef unsigned long ulong;
   
   
   //!   <b>Data type:</b> signed longlong (64 bit)
   typedef signed long long LONGLONG;
   
   //!   <b>Data type:</b> signed longlong (64 bit)
	typedef long long longlong;
   
   //!   <b>Data type:</b> signed longlong (64 bit)
	typedef long long llong;
   
   //!   <b>Data type:</b> unsigned longlong (64 bit)
   typedef unsigned long long ULONGLONG;
   
   //!   <b>Data type:</b> unsigned longlong (64 bit)
	typedef unsigned long long ulonglong;
   
   //!   <b>Data type:</b> unsigned longlong (64 bit)
	typedef unsigned long long ullong;
#endif

//!   <b>Data type:</b> signed byte (8 bit)
typedef signed char BYTE;

//!   <b>Data type:</b> signed byte (8 bit)
typedef signed char byte;

//!   <b>Data type:</b> unsigned byte (8 bit)
typedef unsigned char UBYTE;

//!   <b>Data type:</b> unsigned byte (8 bit)
typedef unsigned char ubyte;


#if __sizeof_short == 2
   //!   <b>Data type:</b> signed word (16 bit)
   typedef signed short WORD;
   
   //!   <b>Data type:</b> signed word (16 bit)
   typedef signed short word;
   
   //!   <b>Data type:</b> unsigned word (16 bit)
   typedef unsigned short UWORD;
   
   //!   <b>Data type:</b> unsigned word (16 bit)
   typedef unsigned short uword;
#endif

#if __sizeof_long == 4
   //!   <b>Data type:</b> signed dword (32 bit)
   typedef signed long DWORD;
   
   //!   <b>Data type:</b> signed dword (32 bit)
   typedef signed long dword;
   
   //!   <b>Data type:</b> unsigned dword (32 bit)
   typedef unsigned long UDWORD;
   
   //!   <b>Data type:</b> unsigned dword (32 bit)
   typedef unsigned long udword;
   
   
   //!   <b>Data type:</b> signed qword (64 bit)
   typedef signed long long QWORD;
   
   //!   <b>Data type:</b> signed qword (64 bit)
   typedef signed long long qword;
   
   //!   <b>Data type:</b> unsigned qword (64 bit)
   typedef unsigned long long UQWORD;
   
   //!   <b>Data type:</b> unsigned qword (64 bit)
   typedef unsigned long long uqword;
#endif


// Alternatives for c55x.h

//!   <b>Macro:</b> classic typeless abs
#define ABS(a, b) ((a) >= (b)) ? ((a)- (b)) : ((b)- (a))


// Extensions for c55x.h

//!   <b>Inline:</b> max for USHORTs
__ARM_INTRINSIC USHORT _umax   (USHORT src1, USHORT src2)   { return (src1 > src2) ? src1 : src2; }

//!   <b>Inline:</b> max for ULONGs
__ARM_INTRINSIC ULONG  _ulmax  (ULONG src1, ULONG src2)     { return (src1 > src2) ? src1 : src2; }

//!   <b>Inline:</b> min for USHORTs
__ARM_INTRINSIC USHORT _umin   (USHORT src1, USHORT src2)   { return (src1 < src2) ? src1 : src2; }

//!   <b>Inline:</b> min for ULONGs
__ARM_INTRINSIC ULONG  _ulmin  (ULONG src1, ULONG src2)     { return (src1 < src2) ? src1 : src2; }


// Typeless extensions

//!   <b>Macro:</b> classic typeless max
#define MAX(a, b) ((a) >= (b)) ? (a) : (b)

//!   <b>Macro:</b> classic typeless min
#define MIN(a, b) ((a) <= (b)) ? (a) : (b)


// Time constants (CAUTION! OS_T_FOREVER synchronizes pending tasks!!!)

//!   <b>Time constant:</b> NO time
#define OS_T_NONE (0x0000)

//!   <b>Time constant:</b> MAX time
#define OS_T_MAX  (0XFFFE)

//!   <b>Time constant:</b> FOREVER time
#define OS_T_FOREVER (0XFFFF)

//!   <b>Time macro/inline:</b> convert given time (ms) to ticks (auto determination of tick's frequency) and returns always at least "(t != 0)"
#define OS_T_MS(t) ((t)) ? (_ulmax(((ULONG)(t))/ _ulmin(((ULONG)os_clockrate/ 1000UL), OS_T_MAX), 1)) : (0)

//!   <b>Time macro:</b> convert given time (ms) and tick frequency to ticks <b>[constant init safe]</b> and returns at least "(t != 0)"
#define _OS_T_MS(t, ostickrate) ((t) && (ostickrate)) ? (MAX(((((ULONG)(t)))/ (MIN(((ULONG)ostickrate/ 1000UL), OS_T_MAX))), 1)) : (0)


// Bit masks

//!   <b>Mask:</b> bit 0
#define BIT0 (1<<0)

//!   <b>Mask:</b> bit 1
#define BIT1 (1<<1)

//!   <b>Mask:</b> bit 2
#define BIT2 (1<<2)

//!   <b>Mask:</b> bit 3
#define BIT3 (1<<3)

//!   <b>Mask:</b> bit 4
#define BIT4 (1<<4)

//!   <b>Mask:</b> bit 5
#define BIT5 (1<<5)

//!   <b>Mask:</b> bit 6
#define BIT6 (1<<6)

//!   <b>Mask:</b> bit 7
#define BIT7 (1<<7)

//!   <b>Mask:</b> bit 8
#define BIT8 (1<<8)

//!   <b>Mask:</b> bit 9
#define BIT9 (1<<9)

//!   <b>Mask:</b> bit 10
#define BIT10 (1<<10)

//!   <b>Mask:</b> bit 11
#define BIT11 (1<<11)

//!   <b>Mask:</b> bit 12
#define BIT12 (1<<12)

//!   <b>Mask:</b> bit 13
#define BIT13 (1<<13)

//!   <b>Mask:</b> bit 14
#define BIT14 (1<<14)

//!   <b>Mask:</b> bit 15
#define BIT15 (1<<15)

//!   <b>Mask:</b> bit 16
#define BIT16 (1<<16)

//!   <b>Mask:</b> bit 17
#define BIT17 (1<<17)

//!   <b>Mask:</b> bit 18
#define BIT18 (1<<18)

//!   <b>Mask:</b> bit 19
#define BIT19 (1<<19)

//!   <b>Mask:</b> bit 20
#define BIT20 (1<<20)

//!   <b>Mask:</b> bit 21
#define BIT21 (1<<21)

//!   <b>Mask:</b> bit 22
#define BIT22 (1<<22)

//!   <b>Mask:</b> bit 23
#define BIT23 (1<<23)

//!   <b>Mask:</b> bit 24
#define BIT24 (1<<24)

//!   <b>Mask:</b> bit 25
#define BIT25 (1<<25)

//!   <b>Mask:</b> bit 26
#define BIT26 (1<<26)

//!   <b>Mask:</b> bit 27
#define BIT27 (1<<27)

//!   <b>Mask:</b> bit 28
#define BIT28 (1<<28)

//!   <b>Mask:</b> bit 29
#define BIT29 (1<<29)

//!   <b>Mask:</b> bit 30
#define BIT30 (1<<30)

//!   <b>Mask:</b> bit 31
#define BIT31 (1<<31)


//!   <b>Mask:</b> bit x ["BIT" is already occupied by RTL via typedef]
#define BITX(x) (1<<(_umin((x), 63)))


//!   <b>Mask:</b> all bits of UCHAR set
#define BITS_UCHAR (0xFF)

//!   <b>Mask:</b> all bits of USHORT set
#define BITS_USHORT (0xFFFF)


//!   <b>Mask:</b> all 4 most significant bits of UCHAR set
#define BITS_UCHARH  (0xF0)

//!   <b>Mask:</b> all 4 less significant bits of UCHAR set
#define BITS_UCHARL  (0x0F)

//!   <b>Mask:</b> all 8 most significant bits of USHORT set
#define BITS_USHORTH (0xFF00)

//!   <b>Mask:</b> all 8 less significant bits of USHORT set
#define BITS_USHORTL (0x00FF)


//!   <b>Mask:</b> all 8 most significant bits of ULONG set
#define BITS_ULONGHH (0xFF000000)

//!   <b>Mask:</b> all 8 below most significant bits of ULONG set
#define BITS_ULONGHL (0x00FF0000)

//!   <b>Mask:</b> all 8 above less significant bits of ULONG set
#define BITS_ULONGLH (0x0000FF00)

//!   <b>Mask:</b> all 8 less significant bits of ULONG set
#define BITS_ULONGLL (0x000000FF)

//!   <b>Mask:</b> all 16 most significant bits of ULONG set
#define BITS_ULONGHX (0xFFFF0000)

//!   <b>Mask:</b> all 16 less significant bits of ULONG set
#define BITS_ULONGLX (0x0000FFFF)

//!   <b>Macro:</b> transfer 16 most significant bits of U32 to U16 <b>[no return value]</b>
#define GET_BITS_ULONGHX(u32_value, result) (result = ((USHORT)((u32_value & 0xFFFF0000) >> 16)))

//!   <b>Macro:</b> transfer 16 less significant bits of U32 to U16 <b>[no return value]</b>
#define GET_BITS_ULONGLX(u32_value, result) (result = ((USHORT)((u32_value & 0x0000FFFF))))

//!   <b>Constant:</b> bit count of UCHAR
#define BITS_OF_UCHAR (8)

//!   <b>Constant:</b> bit count of given data type
#define BITS_OF(type) (sizeof(type)* (BITS_OF_UCHAR))


// Byte swaps

//!   <b>Macro:</b> swap bytes [little/big endian conversion] of USHORT (16 bit)
#define BSWAP_USHORT(us)      ((((USHORT)(us)) & BITS_USHORTH) >> 8 | (((USHORT)(us)) & BITS_USHORTL) << 8)

//!   <b>Macro:</b> swap bytes [little/big endian conversion] of two packed USHORTs in a ULONG (16+16 bit)
#define BSWAP_USHORT32(us32)  ((((ULONG)(us32)) & BITS_ULONGHH) >> 8 | (((ULONG)(us32)) & BITS_ULONGHL) << 8 | (((ULONG)(us32)) & BITS_ULONGLH) >> 8 | (((ULONG)(us32)) & BITS_ULONGLL) << 8)

//!   <b>Macro:</b> swap bytes [little/big endian conversion] of ULONG (32 bit)
#define BSWAP_ULONG(ul)       ((((ULONG)(ul)) & BITS_ULONGHH) >> 24 | (((ULONG)(ul)) & BITS_ULONGHL) >> 8 | (((ULONG)(ul)) & BITS_ULONGLH) << 8 | (((ULONG)(ul)) & BITS_ULONGLL) << 24)


// RTL os_tmr used for signals

//!   <b>Constant:</b> timer 0
#define TIMER0 (0)

//!   <b>Constant:</b> timer 1
#define TIMER1 (1)

//!   <b>Constant:</b> timer 2
#define TIMER2 (2)

//!   <b>Constant:</b> timer 3
#define TIMER3 (3)

//!   <b>Constant:</b> timer 4
#define TIMER4 (4)

//!   <b>Constant:</b> timer 5
#define TIMER5 (5)

//!   <b>Constant:</b> timer 6
#define TIMER6 (6)

//!   <b>Constant:</b> timer 7
#define TIMER7 (7)

//!   <b>Constant:</b> timer 8
#define TIMER8 (8)

//!   <b>Constant:</b> timer 9
#define TIMER9 (9)

//!   <b>Constant:</b> timer 10
#define TIMER10 (10)

//!   <b>Constant:</b> timer 11
#define TIMER11 (11)

//!   <b>Constant:</b> timer 12
#define TIMER12 (12)

//!   <b>Constant:</b> timer 13
#define TIMER13 (13)

//!   <b>Constant:</b> timer 14
#define TIMER14 (14)

//!   <b>Constant:</b> timer 15
#define TIMER15 (15)

//!   <b>Constant:</b> count of available timers (configured in RTX_Config.c)
#define TIMERS (os_timernum & BITS_USHORTL)

//!   <b>Constant:</b> timer x (limited to maximum available timer number)
#define TIMER(x) (_umin((x), TIMERS))

//!   <b>Constant:</b> timer x (<b>NOT limited</b> to maximum available timer number)
#define _TIMER(x) (x)

//!   <b>Prototype:</b> timer handler
#define TIMER_HANDLER(a) os_tmr_call (a)


//!   <b>Constant:</b> signal (timer) 0
#define SIGNAL0 (TIMER0)

//!   <b>Constant:</b> signal (timer) 1
#define SIGNAL1 (TIMER1)

//!   <b>Constant:</b> signal (timer) 2
#define SIGNAL2 (TIMER2)

//!   <b>Constant:</b> signal (timer) 3
#define SIGNAL3 (TIMER3)

//!   <b>Constant:</b> signal (timer) 4
#define SIGNAL4 (TIMER4)

//!   <b>Constant:</b> signal (timer) 5
#define SIGNAL5 (TIMER5)

//!   <b>Constant:</b> signal (timer) 6
#define SIGNAL6 (TIMER6)

//!   <b>Constant:</b> signal (timer) 7
#define SIGNAL7 (TIMER7)

//!   <b>Constant:</b> signal (timer) 8
#define SIGNAL8 (TIMER8)

//!   <b>Constant:</b> signal (timer) 9
#define SIGNAL9 (TIMER9)

//!   <b>Constant:</b> signal (timer) 10
#define SIGNAL10 (TIMER10)

//!   <b>Constant:</b> signal (timer) 11
#define SIGNAL11 (TIMER11)

//!   <b>Constant:</b> signal (timer) 12
#define SIGNAL12 (TIMER12)

//!   <b>Constant:</b> signal (timer) 13
#define SIGNAL13 (TIMER13)

//!   <b>Constant:</b> signal (timer) 14
#define SIGNAL14 (TIMER14)

//!   <b>Constant:</b> signal (timer) 15
#define SIGNAL15 (TIMER15)

//!   <b>Constant:</b> count of available signals (timers) (configured in RTX_Config.c)
#define SIGNALS (TIMERS)

//!   <b>Constant:</b> signal (timer) x (limited to maximum available timer number)
#define SIGNAL(x) (TIMER(x))

//!   <b>Constant:</b> signal (timer) x (<b>NOT limited</b> to maximum available timer number)
#define _SIGNAL(x) (_TIMER(x))

//!   <b>Prototype:</b> signal (timer) handler
#define SIGNAL_HANDLER(a) TIMER_HANDLER(a)


// RTL os_evt

//!   <b>Constant:</b> event bit 0
#define EVENT0 BIT0

//!   <b>Constant:</b> event bit 1
#define EVENT1 BIT1

//!   <b>Constant:</b> event bit 2
#define EVENT2 BIT2

//!   <b>Constant:</b> event bit 3
#define EVENT3 BIT3

//!   <b>Constant:</b> event bit 4
#define EVENT4 BIT4

//!   <b>Constant:</b> event bit 5
#define EVENT5 BIT5

//!   <b>Constant:</b> event bit 6
#define EVENT6 BIT6

//!   <b>Constant:</b> event bit 7
#define EVENT7 BIT7

//!   <b>Constant:</b> event bit 8
#define EVENT8 BIT8

//!   <b>Constant:</b> event bit 9
#define EVENT9 BIT9

//!   <b>Constant:</b> event bit 10
#define EVENT10 BIT10

//!   <b>Constant:</b> event bit 11
#define EVENT11 BIT11

//!   <b>Constant:</b> event bit 12
#define EVENT12 BIT12

//!   <b>Constant:</b> event bit 13
#define EVENT13 BIT13

//!   <b>Constant:</b> event bit 14
#define EVENT14 BIT14

//!   <b>Constant:</b> event bit 15
#define EVENT15 BIT15

//!   <b>Constant:</b> count of available event bits
#define EVENTS (BITS_USHORT)

//!   <b>Constant:</b> event bit x (limited to maximum available event bit number)
#define EVENT(x) (1<<(_umin((x), 15)))


// Critical section

//!   <b>System control:</b> lock system interrupt tick
#define LOCK irq_lock= __TRUE; tsk_lock()

//!   <b>System control:</b> unlock system interrupt tick
#define UNLOCK irq_lock= __FALSE; tsk_unlock()

//!   <b>System control:</b> lock system interrupt tick, perform expr(ession) and unlock system interrupt tick
#define LOCKED(expr) LOCK, (expr), UNLOCK

//!   <b>System control:</b> lock system force interrupt trigger (additional forced scheduler runs)
#define LOCK_FORCECTRL force_irq_enable= __FALSE

//!   <b>System control:</b> unlock system force interrupt trigger (additional forced scheduler runs)
#define UNLOCK_FORCECTRL force_irq_enable= __TRUE


// RTL task user stack

//!   <b>System macro:</b> define and allocate user stack with given size for given task
#define OS_TSK_STACK_USER(task, size) static U64 task[(size)/ 8]

//!   <b>System macro:</b> define and allocate user stack array with given size for given task array 'name'
#define OS_TSK_STACK_USER_EX(name, size, dim) static U64 name##dim##[(size)/ 8]

//!   <b>System macro:</b> create task with previously defined and allocated user stack
#define OS_TSK_CREATE_USER(task, prio, stack) os_tsk_create_user(task, prio, stack, sizeof(stack))

//!   <b>System macro:</b> create task with argument pointer and previously defined and allocated user stack
#define OS_TSK_CREATE_USER_EX(task, prio, stack, arg) os_tsk_create_user_ex(task, prio, stack, sizeof(stack), arg)

//!   <b>System macro:</b> create initial user task with previously defined and allocated user stack
#define OS_SYS_INIT_USER(task, prio, stack) os_sys_init_user(task, prio, stack, sizeof(stack))


// RTL os_mbx

//!   <b>System macro:</b> put ULONG as direct mailbox value (instead of the default pointer to data)
#define OS_MBX_VALUE_PUT(v) (void *)((ULONG)(v))

//!   <b>System macro:</b> get ULONG as direct mailbox value (instead of the default pointer to data)
#define OS_MBX_VALUE_GET(v) (void *)((ULONG)(&v))

//!   <b>System macro:</b> easy success checking for os_mbx_wait
#define OS_MBX_VALUE_CHK(v) ((v) != OS_R_TMO)

//!   <b>System macro:</b> easy success checking for os_mbx_wait
#define OS_MBX_CHECK(v) OS_SEM_TAKE_CHK(v)

//!   <b>System macro:</b> create a mailbox easy and safe
#define OS_MBX_INIT(v) os_mbx_init((v), sizeof((v)))

//!   <b>System macro:</b> create an array of mailboxes easy and safe
#define OS_MBX_INIT_EX(v) os_mbx_init((&##v), sizeof((v)))

//!   <b>System macro:</b> define and allocate an array of mailboxes
#define os_mbx_declare_ex(name, size, dim)    U32 name##dim##[4+ size]


// RTL isr_mbx

//!   <b>System macro:</b> put ULONG as direct mailbox value (instead of the default pointer to data)
#define ISR_MBX_VALUE_PUT(v) OS_MBX_VALUE_PUT(v)

//!   <b>System macro:</b> get ULONG as direct mailbox value (instead of the default pointer to data)
#define ISR_MBX_VALUE_GET(v) OS_MBX_VALUE_GET(v)

//!   <b>System macro:</b> easy success checking for os_mbx_wait
#define ISR_MBX_VALUE_CHK(v) ((v) == OS_R_MBX)


// SMailbox

//!   <b>System macro:</b> easy os_mbx_wait for a mailbox embedded in a structure
#define SMBX_WAIT(mbx, loc, var) OS_MBX_VALUE_CHK(os_mbx_wait(loc.mbx.oiID, OS_MBX_VALUE_GET(var), loc.mbx.u16To))

//!   <b>System macro:</b> easy os_mbx_send_safe for a mailbox embedded in a structure
#define SMBX_SEND(mbx, loc, var) os_mbx_send_safe(loc.mbx.oiID, OS_MBX_VALUE_PUT(var), loc.mbx.u16To)

//!   <b>System macro:</b> easy os_mbx_check for a mailbox embedded in a structure
#define SMBX_CHECK(mbx, loc) os_mbx_check(loc.mbx.oiID)


//!   <b>System macro:</b> easy isr_mbx_send for a mailbox embedded in a structure
#define SMBX_ISR_SEND(mbx, loc, var) isr_mbx_send(loc.mbx.oiID, OS_MBX_VALUE_PUT(var))

//!   <b>System macro:</b> easy isr_mbx_check for a mailbox embedded in a structure
#define SMBX_ISR_CHECK(mbx, loc) isr_mbx_check(loc.mbx.oiID)

//!   Mailbox embedded in a structure
struct _SMailbox
{
   OS_ID oiID;       //!< OS_ID of mailbox
   U16 u16To;        //!< Time-Out (ticks) of any mailbox access
};


// RTL os/isr_sem

//!   <b>Macro:</b> easy and safe checking of success of os_sem_take (os_sem_wait)
/**   <b>Example:</b> if (OS_SEM_TAKE_CHK(os_sem_take(SSpiRW[ucUnit].SemMsg, u16Timeout))) { ... } */
#define OS_SEM_TAKE_CHK(v) ((v) != OS_R_TMO)

//!   <b>Macro:</b> easy and safe checking of success of isr_sem_check
/**   <b>Example:</b> if (!ISR_SEM_TAKE_CHK(isr_sem_check(SemNotifyMcp2317[ucUnit]))) { ... } */
#define ISR_SEM_TAKE_CHK(v) OS_SEM_TAKE_CHK(v)


// RTL os/isr_mut

//!   <b>Macro:</b> easy and safe checking of success of os_mut_take (os_mut_wait)
#define OS_MUT_TAKE_CHK(v) OS_SEM_TAKE_CHK(v)

//!   <b>Macro:</b> easy and safe checking of success of isr_mut_check
#define ISR_MUT_TAKE_CHK(v) OS_MUT_TAKE_CHK(v)


// SSemaphore

//!   <b>Macro:</b> os_sem_take (os_sem_wait) for #_SSemaphore (time-out of #_SSemaphore) (result == result of os_sem_take)
#define SSEM_TAKE(s) os_sem_take(s.oiID, s.u16To)

//!   <b>Macro:</b> os_sem_take (os_sem_wait) for #_SSemaphore (user specified time-out) (result == result of os_sem_take)
#define SSEM_TAKEUSR(s, to) os_sem_take(s.oiID, to)

//!   <b>Macro:</b> os_sem_give (os_sem_semd) for #_SSemaphore
#define SSEM_GIVE(s) os_sem_give(s.oiID)


//!   <b>Macro:</b> take semaphore at loc, copy loc.var to var and give semaphore (result == #TRUE for success)
#define SSEM_READ(sem, loc, var) SSEM_READVAR(sem, loc, var, var)

//!   <b>Macro:</b> take semaphore at loc, copy loc.var to dst and give semaphore (result == #TRUE for success)
#define SSEM_READVAR(sem, loc, var, dst) OS_SEM_TAKE_CHK(SSEM_TAKE(loc.sem)) ? (dst)= loc.var, SSEM_GIVE(loc.sem), TRUE : FALSE


//!   <b>Macro:</b> take semaphore at loc, copy var to loc.var and give semaphore (result == #TRUE for success)
#define SSEM_WRITE(sem, loc, var) SSEM_WRITEVAR(sem, loc, var, var)

//!   <b>Macro:</b> take semaphore at loc, copy src to loc.var and give semaphore (result == #TRUE for success)
#define SSEM_WRITEVAR(sem, loc, var, src) OS_SEM_TAKE_CHK(SSEM_TAKE(loc.sem)) ? loc.var= (src), SSEM_GIVE(loc.sem), TRUE : FALSE


//!   <b>Macro:</b> os_sem_take (os_sem_wait) for #_SSemaphore (time-out of #_SSemaphore) (result == #TRUE for success)
#define SSEM_LOCK(sem, loc) OS_SEM_TAKE_CHK(SSEM_TAKE(loc.sem))

//!   <b>Macro:</b> os_sem_take (os_sem_wait) for #_SSemaphore (user specified time-out) (result == #TRUE for success)
#define SSEM_LOCKUSR(sem, loc, to) OS_SEM_TAKE_CHK(SSEM_TAKEUSR(loc.sem, to))

//!   <b>Macro:</b> os_sem_give (os_sem_semd) for #_SSemaphore
#define SSEM_UNLOCK(sem, loc) SSEM_GIVE(loc.sem)


//!   <b>Macro:</b> copy loc.var to var
#define SBLK_READ(loc, var) (var)= loc.var

//!   <b>Macro:</b> copy var to loc.var
#define SBLK_WRITE(loc, var) loc.var= (var)


//!   <b>Macro:</b> isr_sem_give (isr_send_send) for #_SSemaphore
#define SSEM_ISR_GIVE(s) isr_sem_give(s.oiID)

//!   <b>Macro:</b> isr_sem_give (isr_send_send) for #_SSemaphore
#define SSEM_ISR_UNLOCK(sem, loc) SSEM_ISR_GIVE(loc.sem)


//!   Semaphore embedded in a structure (incl. time-out value for easy access by SSEM... macros)
struct _SSemaphore
{
   OS_ID oiID;       //!< OS_ID of semaphore
   U16 u16To;        //!< Time-Out (ticks) for any semaphore access
};


// Message combo (count & value packed in 32bits)

//!   <b>Packed message macro:</b> create mask with first x bits set
#define MSG_MASKMSG(bits) ((1UL << (bits))- 1)

//!   <b>Packed message macro:</b> get message part from packed message
#define MSG_GETMSG(bits, msg) (((ULONG)(msg)) &  MSG_MASKMSG((bits)))

//!   <b>Packed message macro:</b> get count part from packed message
#define MSG_GETCNT(bits, msg) ((((ULONG)(msg)) & ~MSG_MASKMSG((bits))) >> (bits))

//!   <b>Packed message macro:</b> add count part to packed message
#define MSG_ADDCNT(bits, msg, cnt) (MSG_GETMSG((bits), (msg)) | (((ULONG)(cnt)) << (bits)))


// Values

//!   <b>Macro:</b> raise 'val' by 'step' limited to 'max'
#define RAISEMAX(val, step, max) (val)+= ((val)+ (step) <= ((max)- 1)) ? ((step)) : ((max)- (val))

//!   <b>Macro:</b> lower 'val' by 'step' limited to 'min'
#define LOWERMIN(val, step, min) (val)-= ((val)- (step) > ((min))) ? ((step)) : ((val)- (min))

//!   <b>Macro:</b> raise 'val' by 'step' limited to 'max' with system tick locked during action
#define ATOMIC_RAISEMAX(val, step, max) LOCK, RAISEMAX(val, step, max), UNLOCK

//!   <b>Macro:</b> lower 'val' by 'step' limited to 'min' with system tick locked during action
#define ATOMIC_LOWERMIN(val, step, max) LOCK, LOWERMIN(val, step, max), UNLOCK


// SoftWareInterrupts

//!   <b>Macro:</b> SoftwareInterrupt (SWI) prototype attribute helper [<i>e.g.</i> void <b>SWI_PROTO(8)</b> inc_5bit (U32 *pu32Value);]
#define SWI_PROTO(id) __swi(id)

//!   <b>Macro:</b> SoftwareInterrupt (SWI) function header helper [<i>e.g.</i> void <b>SWI_FUNC(8)</b> /* inc_5bit */ (U32 *pu32Value)]
#define SWI_FUNC(id) __SWI_##id


// Other

//!   <b>Macro:</b> task frame
#define FOREVER for(;;)

//!   <b>Macro:</b> task quitting (should never happen)
#define DETACH os_tsk_delete_self()

//!   <b>Macro:</b> acknowledge isr to return to normal cpu state
#define EOI AT91F_AIC_AcknowledgeIt(AT91C_BASE_AIC)

//!   <b>Macro:</b> cast interrupt function pointers (removes special isr function attributes for standard assigment)
#define IRQ_CAST(f) ((void (*)(void))(f))

//!   <b>Macro:</b> delay in milliseconds (for x > 0) and reschedule other pending tasks (force scheduler run)
#define RESCHEDULE(x) (x) ? (os_dly_wait(OS_T_MS((x)))) : (os_tsk_pass())

//!   <b>Macro:</b> delay in ticks (for x > 0) and reschedule other pending tasks (force scheduler run)
#define RESCHEDULE_TICKS(x) (x) ? (os_dly_wait((x))) : (os_tsk_pass())


// Prototypes

//!   <b>System function:</b> check semaphore <b>WITHOUT changing</b> it
/**
      \param	semaphore					   OS_ID of the semaphore
      \param	bUnlock					      #TRUE == scheduler unlock after check, #FALSE == keep scheduler locked after check
      
      \return										> 0 (!= #OS_R_TMO) for available semaphore
      
      \note
               - <b>you need to unlock the scheduler again by yourself for bUnlock == #FALSE using the 'UNLOCK;' macro!!!</b>

      <b>Example code:</b><br>
      \include sem_check.c
*/
OS_RESULT os_sem_check (OS_ID semaphore, bool bUnlock);

//!   <b>System function:</b> check semaphore within isr <b>WITHOUT changing</b> it
/**
      \param	semaphore					   OS_ID of the semaphore
      
      \return										> 0 (!= #OS_R_TMO) for available semaphore
*/
OS_RESULT isr_sem_check (OS_ID semaphore);

//!   <b>System function:</b> check semaphore <b>WITHOUT changing</b> it
/**
      \param	semaphore					   OS_ID of the semaphore
      \param	bUnlock					      #TRUE == scheduler unlock after check, #FALSE == keep scheduler locked after check
      \param   bIsr                       context flag: #TRUE == isr, #FALSE == task
      
      \return										> 0 (!= #OS_R_TMO) for available semaphore
      
      \note
               - use the wrapper functions #os_sem_check and #isr_sem_check for safety
               - <b>you need to unlock the scheduler again by yourself for bUnlock == #FALSE using the 'UNLOCK;' macro!!!</b>
      
      <b>Example code:</b><br>
      \include sem_check.c
*/
OS_RESULT sem_check (OS_ID semaphore, bool bUnlock, bool bIsr);

//!   <b>System function:</b> read mailbox within isr <b>WITHOUT triggering</b> any further scheduler runs
/**
      \param	mailbox					      OS_ID of the mailbox
      \param	**message					   storage for message data
      
      \return										> 0 (!= #OS_R_TMO) for available message(s)

      \note
               - use this function instead for high frequency mailbox access <b>WITHOUT</b> forced scheduler runs and <b>WITHOUT</b> any pending tasks
*/
OS_RESULT isr_mbx_read (OS_ID mailbox, void **message);

//!   <b>System function:</b> write to mailbox within isr <b>WITHOUT triggering</b> any further scheduler runs
/**
      \param	mailbox					      OS_ID of the mailbox
      \param	*p_msg					      pointer to message data
      
      \note
               - you should check the mailbox's free space by <b>isr_mbx_check</b> before any write attempt to avoid data loss!
               - use this function instead of <b>isr_mbx_send</b> for high frequency mailbox access <b>WITHOUT</b> forced extra scheduler runs and <b>WITHOUT</b> any pending tasks
*/
void isr_mbx_write (OS_ID mailbox, void *p_msg);

//!   <b>System function:</b> read mailbox <b>WITHOUT triggering</b> any further scheduler runs
/**
      \param	mailbox					      OS_ID of the mailbox
      \param	*p_msg					      storage for message data
      \param   u16Timeout                 <b>unused</b>, just present for easy replacement of <b>os_mbx_send</b>
      
      \return										> 0 (!= #OS_R_TMO) for available message(s)

      \note
               - use this function instead of <b>os_mbx_wait</b> for high frequency mailbox access <b>WITHOUT</b> forced extra scheduler runs and <b>WITHOUT</b> any pending tasks
*/
OS_RESULT os_mbx_read(OS_ID mailbox, void *p_msg, U16 u16Timeout);

//!   <b>System function:</b> write to mailbox <b>WITHOUT triggering</b> any further scheduler runs
/**
      \param	mailbox					      OS_ID of the mailbox
      \param	*p_msg					      pointer to message data
      \param   u16Timeout                 <b>unused</b>, just present for easy replacement of <b>os_mbx_send</b>
      
      \return										> 0 (!= #OS_R_TMO) for successful write of message(s)     

      \note
               - use this function instead of <b>os_mbx_send</b> for high frequency mailbox access <b>WITHOUT</b> forced extra scheduler runs and <b>WITHOUT</b> any pending tasks
*/
OS_RESULT os_mbx_write(OS_ID mailbox, void *p_msg, U16 u16Timeout);

//!   <b>System function:</b> write to mailbox safely <b>(avoid receiving task is suspended on mailbox full state)</b>
/**
      \param	oiMbx					         OS_ID of the mailbox
      \param	*pvMbx					      pointer to message data
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 (!= #OS_R_TMO) for successful write of message(s)
      
      \note
               - use this function instead of <b>os_mbx_send</b> to avoid receiving task is suspended on mailbox full state
*/
OS_RESULT os_mbx_send_safe (OS_ID oiMbx, void *pvMbx, U16 u16Timeout);

//!   <b>System function:</b> check mailbox <b>WITHOUT changing</b> it
/**
      \param	oiMbx 				         OS_ID of the mailbox
      
      \return										count of entries within the mailbox
*/
OS_RESULT os_mbx_count (OS_ID oiMbx);

//!   <b>System function:</b> check mailbox within isr <b>WITHOUT changing</b> it
/**
      \param	oiMbx					         OS_ID of the mailbox
      
      \return										count of entries within the mailbox
*/
OS_RESULT isr_mbx_count (OS_ID oiMbx);

//!   <b>System function:</b> fast mailbox clearing <b>(skips for mailbox still empty after a single system tick)</b>
/**
      \param	oiMbx					         OS_ID of the mailbox
      \param   u16Timeout                 maximum cpu-load free resource pending time before any resource claim is canceled by the rtos (in ticks)
      
      \return										> 0 for success, == 0 for failure
      
      \remarks
               - this function skips for mailbox still empty after a single system tick <b>but will pend for more up to specified usTimeOut once</b>
*/
USHORT os_mbx_clear (OS_ID oiMbx, U16 u16Timeout);

//!   <b>System function:</b> type size optimized version of <b>memcpy</b> with "no source" (*source= NULL) support (to replace <b>memset</b> too)
/**
      \param	*pvDestination					pointer to destination
      \param   *cpvSource                 pointer to source <b>(set NULL to just init destination with ZERO like memset)</b>
      \param   num                        number of items to transfer / write
      \param   size                       size of item in bytes (for faster native memory access)
      
      \return										> NULL (pointer to pvDestination) for success, == NULL for failure
      
      \remarks
               - this is a <b>optimized version of memcpy</b> by using faster native (spedified) memory access
               - you may use this function as a <b>faster replacement for memset by setting pvSource to NULL</b>
*/
void * memcpyflex (void *pvDestination, const void *cpvSource, size_t num, size_t size);


// Utility


//----------------------------------------------------------------------------
// os_sem_give: name wrapper for unusual named semaphore function of RTL
//----------------------------------------------------------------------------
__inline OS_RESULT os_sem_give (OS_ID oiSemaphore)
{
   return ((oiSemaphore) ? (os_sem_send(oiSemaphore)) : (OS_R_TMO));
}

//----------------------------------------------------------------------------
// os_sem_take: name wrapper for unusual named semaphore function of RTL
//----------------------------------------------------------------------------
__inline OS_RESULT os_sem_take (OS_ID oiSemaphore, U16 u16Timeout)
{
   return ((oiSemaphore) ? (os_sem_wait(oiSemaphore, u16Timeout)) : (OS_R_TMO));
}

//----------------------------------------------------------------------------
// isr_sem_take: name wrapper for unusual named semaphore function of RTL
//----------------------------------------------------------------------------
__inline void isr_sem_give (OS_ID oiSemaphore)
{
   isr_sem_send(oiSemaphore);
}

//----------------------------------------------------------------------------
// os_mut_give: name wrapper for unusual named mutex function of RTL
//----------------------------------------------------------------------------
__inline OS_RESULT os_mut_give (OS_ID oiMutex)
{
   return ((oiMutex) ? (os_mut_release(oiMutex)) : (OS_R_TMO));
}

//----------------------------------------------------------------------------
// os_mut_take: name wrapper for unusual named mutex function of RTL
//----------------------------------------------------------------------------
__inline OS_RESULT os_mut_take (OS_ID oiMutex, U16 u16Timeout)
{
   return ((oiMutex) ? (os_mut_wait(oiMutex, u16Timeout)) : (OS_R_TMO));
}

//----------------------------------------------------------------------------
// os_tmr_...: wrapper for custom os_sig... functions
//----------------------------------------------------------------------------

#define os_tmr_send os_sig_send
#define os_tmr_send_delay os_sig_send_delay

//----------------------------------------------------------------------------
// os_sig_send: wrapper to use user timers as signals 
//----------------------------------------------------------------------------
__inline OS_ID os_sig_send (U16 u16Info)
{
   return (os_tmr_create(1, u16Info));
}

//----------------------------------------------------------------------------
// os_sig_send_delay: wrapper to use user timers as signals 
//----------------------------------------------------------------------------
__inline OS_ID os_sig_send_delay (U16 u16Info, U16 u16Delay)
{
   return (os_tmr_create(u16Delay, u16Info));
}

//----------------------------------------------------------------------------
// os_sig_kill: wrapper to use user timers as signals 
//----------------------------------------------------------------------------
__inline OS_ID os_sig_kill (OS_ID oiSignal)
{
   return ((oiSignal) ? (os_tmr_kill(oiSignal)) : (oiSignal));
}


//----------------------------------------------------------------------------
// circular buffer macros (lock sema required by access of multiple tasks !!!)
// creates and handles fifo buffers that _don't_ wipe unread data automatically
//----------------------------------------------------------------------------
// !!! explicit casts needed in args when different/other datatypes mixed  !!!
// !!! ANSI-C pointer offsetting works always with size of argument pointer!!!
//----------------------------------------------------------------------------

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> define and allocate a single buffer
/**
      \param	type					         data type of the buffer items (CHAR, SHORT, LONG, UCHAR, USHORT, ULONG ...)
      \param	typesize					      data type of the buffer indizes (CHAR, SHORT, LONG, UCHAR, USHORT, ULONG ...)
      \param	name   	                  name of the buffer
      \param	count				            count of items within the buffer

      \note
               - <b>No array element is initialized!!! You have to initialize the size (to value of count) and the pointer by yourself for all items!!!</b><br>
               
      <b>Example code:</b><br>
      \include Circular.c
*/
#define circular_declare(type, typesize, name, count) struct _##name {type Data[count]; typesize idxRead; typesize idxWrite; typesize cntEntries; typesize Size;} name = {{0}, 0, 0, 0, count}; struct _##name *p##name= &##name

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> define and allocate an array of buffer
/**
      \param	type					         data type of the buffer items (CHAR, SHORT, LONG, UCHAR, USHORT, ULONG ...)
      \param	typesize					      data type of the buffer indizes (CHAR, SHORT, LONG, UCHAR, USHORT, ULONG ...)
      \param	name   	                  name of the buffer
      \param	count				            count of items within the buffer
      \param	dim				            dimension definition (eg. [2] or [2][4] ...)

      \note
               - <b>No array element is initialized!!! You have to initialize the size (to value of count) and the pointer by yourself for all items!!!</b><br>
               
      <b>Example code:</b><br>
      \include CircularEx.c
*/
#define circular_declare_ex(type, typesize, name, count, dim) struct _##name {type Data[count]; typesize idxRead; typesize idxWrite; typesize cntEntries; typesize Size;}; struct _##name name##dim = { {0, 0, 0, 0, 0} }; struct _##name *p##name##dim= { NULL }
   
// Pointer macros

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> empty buffer
/**
      \param	name   	                  name of the buffer

      \note
               - only indizes are cleared, data items are overwritten by next use<br>
*/
#define circular_resetp(name) p##name->idxRead= 0; p##name->idxWrite= 0; p##name->cntEntries= 0; p##name->cntEntries= 0

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> return size (NOT remaining space) of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             size (NOT remaining space) of buffer
*/
#define circular_sizep(name) (p##name->Size)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> return remaining READ space of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             remaining READ space of buffer
*/
#define circular_freeReadp(name) (p##name->Size- p##name->cntEntries)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> return remaining WRITE space of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             remaining WRITE space of buffer
*/
#define circular_freeWritep(name) (p##name->Size- p##name->cntEntries)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> return allocated items (entries) in buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             allocated items (entries) in buffer
*/
#define circular_countEntriesp(name) (p##name->cntEntries)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> re-allocate already read items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   count                      number of read items (entries) to re-allocate
      
      \return                             number of re-allocated items (entries)
*/
#define circular_unReadp(name, count) _ulmin((count), p##name->Size); \
                                      ((_ulmin((count), p##name->Size)) <= p##name->idxRead) ? \
                                         (p##name->idxRead-= (_ulmin((count), p##name->Size)), p##name->cntEntries+= _ulmin((count), p##name->Size)) : \
                                         (p##name->idxRead= (p##name->Size - (((_ulmin((count), p##name->Size)))- p##name->idxRead)), p##name->cntEntries+= _ulmin((count), p##name->Size))

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> write items (entries) to buffer
/**
      \param	name   	                  name of the buffer
      \param   source                     data to write into buffer
      \param   count                      number items (entries) to read
      
      \return                             number of written items (entries)
      
      \note
               - <b>source must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define circular_writep(name, source, count) (((count) <= circular_freeWrite(name))* (count)); \
                                             ((count) <= circular_freeWrite(name)) ? \
                                                ((count) <= p##name->Size- p##name->idxWrite) ? \
                                                   (memcpyflex((void *)(&p##name->Data[p##name->idxWrite]), (const void *)(source), (count), sizeof(p##name->Data[0])), p##name->idxWrite= ((p##name->idxWrite+ (count)) % p##name->Size), p##name->cntEntries+= (count)) : \
                                                   (memcpyflex((void *)(&p##name->Data[p##name->idxWrite]), (const void *)(source), (p##name->Size- p##name->idxWrite), sizeof(p##name->Data[0])), memcpyflex((void *)(&p##name->Data[0]), (const void *)((source)+ (p##name->Size- p##name->idxWrite)), _ulmin(((count)- (p##name->Size- p##name->idxWrite)), (circular_freeWrite(name)- (p##name->Size- p##name->idxWrite))), sizeof(p##name->Data[0])), p##name->idxWrite= ((count)- (((p##name->Size- p##name->idxWrite)))), p##name->cntEntries+= (count)) : \
                                                (0)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> read items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   dest                       data to write buffer items (entries) to
      \param   count                      number of items (entries) to write
      
      \return                             number of read items (entries)
      
      \note
               - <b>dest must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define circular_readp(name, dest, count) _ulmin((count), p##name->cntEntries); \
                                          (_ulmin((count), p##name->cntEntries) > 0) ? \
                                             ((p##name->idxRead + _ulmin((count), p##name->cntEntries) <= p##name->Size)) ? \
                                                (memcpyflex((void *)(dest), (const void *)(&p##name->Data[p##name->idxRead]), _ulmin((count), p##name->cntEntries), sizeof(p##name->Data[0])), p##name->idxRead= ((p##name->idxRead+ _ulmin((count), p##name->cntEntries)) % p##name->Size), p##name->cntEntries-= _ulmin((count), p##name->cntEntries)) : \
                                                (memcpyflex((void *)(dest), (const void *)(&p##name->Data[p##name->idxRead]), p##name->Size- p##name->idxRead, sizeof(p##name->Data[0])), memcpyflex((void *)((dest)+ (p##name->Size- p##name->idxRead)), (const void *)(&p##name->Data[0]), _ulmin((count), p##name->cntEntries)- (p##name->Size- p##name->idxRead), sizeof(p##name->Data[0])), p##name->idxRead= _ulmin((count), p##name->cntEntries)- (p##name->Size- p##name->idxRead), p##name->cntEntries-= _ulmin((count), p##name->cntEntries)) : \
                                             (0)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> wipe (read without transfer) items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   count                      number of items (entries) to wipe
      
      \return                             number of wiped items (entries)
      
      \note
               - <b>dest must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define circular_wipep(name, count) _ulmin((count), p##name->cntEntries); \
                                    (_ulmin((count), p##name->cntEntries) > 0) ? \
                                       ((p##name->idxRead + _ulmin((count), p##name->cntEntries) <= p##name->Size)) ? \
                                          (p##name->idxRead= ((p##name->idxRead+ _ulmin((count), p##name->cntEntries)) % p##name->Size), p##name->cntEntries-= _ulmin((count), p##name->cntEntries)) : \
                                          (p##name->idxRead= _ulmin((count), p##name->cntEntries)- (p##name->Size- p##name->idxRead), p##name->cntEntries-= _ulmin((count), p##name->cntEntries)) : \
                                       (0)
                                            

// Generated value macros (identical to pointer macros but replaced "p##name->" with "name." plus removed suffix "p" at names end)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> empty buffer
/**
      \param	name   	                  name of the buffer

      \note
               - only indizes are cleared, data items are overwritten by next use<br>
*/
#define circular_reset(name) name.idxRead= 0; name.idxWrite= 0; name.cntEntries= 0

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> return size (NOT remaining space) of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             size (NOT remaining space) of buffer
*/
#define circular_size(name) (name.Size)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> return remaining READ space of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             remaining READ space of buffer
*/
#define circular_freeRead(name) (name.Size- name.cntEntries)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> return remaining WRITE space of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             remaining WRITE space of buffer
*/
#define circular_freeWrite(name) (name.Size- name.cntEntries)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> return allocated items (entries) in buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             allocated items (entries) in buffer
*/
#define circular_countEntries(name) (name.cntEntries)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> re-allocate already read items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   count                      number of read items (entries) to re-allocate
      
      \return                             number of re-allocated items (entries)
*/
#define circular_unRead(name, count) _ulmin((count), name.Size); \
                                     ((_ulmin((count), name.Size)) <= name.idxRead) ? \
                                        (name.idxRead-= (_ulmin((count), name.Size)), name.cntEntries+= _ulmin((count), name.Size)) : \
                                        (name.idxRead= (name.Size- (((_ulmin((count), name.Size)))- name.idxRead)), name.cntEntries+= _ulmin((count), name.Size))

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> write items (entries) to buffer
/**
      \param	name   	                  name of the buffer
      \param   source                     data to write into buffer
      \param   count                      number items (entries) to read
      
      \return                             number of written items (entries)
      
      \note
               - <b>source must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define circular_write(name, source, count) (((count) <= circular_freeWrite(name))* (count)); \
                                            ((count) <= circular_freeWrite(name)) ? \
                                               ((count) <= name.Size- name.idxWrite) ? \
                                                  (memcpyflex((void *)(&name.Data[name.idxWrite]), (const void *)(source), (count), sizeof(name.Data[0])), name.idxWrite= ((name.idxWrite+ (count)) % name.Size), name.cntEntries+= (count)) : \
                                                  (memcpyflex((void *)(&name.Data[name.idxWrite]), (const void *)(source), (name.Size- name.idxWrite), sizeof(name.Data[0])), memcpyflex((void *)(&name.Data[0]), (const void *)((source)+ (name.Size- name.idxWrite)), _ulmin(((count)- (name.Size- name.idxWrite)), (circular_freeWrite(name)- (name.Size- name.idxWrite))), sizeof(name.Data[0])), name.idxWrite= ((count)- (((name.Size- name.idxWrite)))), name.cntEntries+= (count)) : \
                                               (0)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> read items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   dest                       data to write buffer items (entries) to
      \param   count                      number of items (entries) to write
      
      \return                             number of read items (entries)
      
      \note
               - <b>dest must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define circular_read(name, dest, count) _ulmin((count), name.cntEntries); \
                                         (_ulmin((count), name.cntEntries) > 0) ? \
                                            ((name.idxRead + _ulmin((count), name.cntEntries) <= name.Size)) ? \
                                               (memcpyflex((void *)(dest), (const void *)(&name.Data[name.idxRead]), _ulmin((count), name.cntEntries), sizeof(name.Data[0])), name.idxRead= ((name.idxRead+ _ulmin((count), name.cntEntries)) % name.Size), name.cntEntries-= _ulmin((count), name.cntEntries)) : \
                                               (memcpyflex((void *)(dest), (const void *)(&name.Data[name.idxRead]), name.Size- name.idxRead, sizeof(name.Data[0])), memcpyflex((void *)((dest)+ (name.Size- name.idxRead)), (const void *)(&name.Data[0]), _ulmin((count), name.cntEntries)- (name.Size- name.idxRead), sizeof(name.Data[0])), name.idxRead= _ulmin((count), name.cntEntries)- (name.Size- name.idxRead), name.cntEntries-= _ulmin((count), name.cntEntries)) : \
                                            (0)

//!   <b>Circular (fifo buffer that _don't_ wipe unread data automatically):</b> wipe (read without transfer) items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   count                      number of items (entries) to wipe
      
      \return                             number of wiped items (entries)
      
      \return                             number of wiped items (entries)
*/
#define circular_wipe(name, count) _ulmin((count), name.cntEntries); \
                                   (_ulmin((count), name.cntEntries) > 0) ? \
                                      ((name.idxRead + _ulmin((count), name.cntEntries) <= name.Size)) ? \
                                         (name.idxRead= ((name.idxRead+ _ulmin((count), name.cntEntries)) % name.Size), name.cntEntries-= _ulmin((count), name.cntEntries)) : \
                                         (name.idxRead= _ulmin((count), name.cntEntries)- (name.Size- name.idxRead), name.cntEntries-= _ulmin((count), name.cntEntries)) : \
                                      (0)

                                      
//----------------------------------------------------------------------------
// ring buffer macros (lock sema required by access of multiple tasks !!!)
// creates and handles fifo buffers that wipe unread data automatically
//----------------------------------------------------------------------------
// !!! explicit casts needed in args when different/other datatypes mixed  !!!
// !!! ANSI-C pointer offsetting works always with size of argument pointer!!!
//----------------------------------------------------------------------------

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> define and allocate a single buffer
/**
      \param	type					         data type of the buffer items (CHAR, SHORT, LONG, UCHAR, USHORT, ULONG ...)
      \param	typesize					      data type of the buffer indizes (CHAR, SHORT, LONG, UCHAR, USHORT, ULONG ...)
      \param	name   	                  name of the buffer
      \param	count				            count of items within the buffer
      
      <b>Example code:</b><br>
      \include RingBuffer.c
*/
#define ringbuffer_declare(type, typesize, name, count) circular_declare(type, typesize, name, count)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> define and allocate an array of buffer
/**
      \param	type					         data type of the buffer items (CHAR, SHORT, LONG, UCHAR, USHORT, ULONG ...)
      \param	typesize					      data type of the buffer indizes (CHAR, SHORT, LONG, UCHAR, USHORT, ULONG ...)
      \param	name   	                  name of the buffer
      \param	count				            count of items within the buffer
      \param	dim				            dimension definition (eg. [2] or [2][4] ...)

      \note
               - <b>No array element is initialized!!! You have to initialize the size (to value of count) and the pointer by yourself for all items!!!</b><br>
*/
#define ringbuffer_declare_ex(type, typesize, name, count, dim) circular_declare_ex(type, typesize, name, count, dim)


// Pointer macros

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> empty buffer
/**
      \param	name   	                  name of the buffer

      \note
               - only indizes are cleared, data items are overwritten by next use<br>
*/
#define ringbuffer_resetp(name) circular_resetp(name)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> return size (NOT remaining space) of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             size (NOT remaining space) of buffer
*/
#define ringbuffer_sizep(name) (name.Size)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> return remaining READ space of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             remaining READ space of buffer
*/
#define ringbuffer_freeReadp(name) circular_freeReadp(name)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> return remaining WRITE space of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             remaining WRITE space of buffer
*/
#define ringbuffer_freeWritep(name) ringbuffer_sizep(name)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> return allocated items (entries) in buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             allocated items (entries) in buffer
*/
#define ringbuffer_countEntriesp(name) circular_countEntriesp(name)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> re-allocate already read items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   count                      number of read items (entries) to re-allocate
      
      \return                             number of re-allocated items (entries)
*/
#define ringbuffer_unReadp(name, count) _ulmin((count), p##name->Size); \
                                       ((_ulmin((count), p##name->Size)) <= p##name->idxRead) ? \
                                          (p##name->idxRead-= (_ulmin((count), p##name->Size)), p##name->cntEntries= _ulmin(p##name->Size, p##name->cntEntries+ _ulmin((count), p##name->Size))) : \
                                          (p##name->idxRead= (p##name->Size- (((_ulmin((count), p##name->Size)))- p##name->idxRead)), p##name->cntEntries= _ulmin(p##name->Size, p##name->cntEntries+ _ulmin((count), p##name->Size)))

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> write items (entries) to buffer
/**
      \param	name   	                  name of the buffer
      \param   source                     data to write into buffer
      \param   count                      number items (entries) to read
      
      \return                             number of written items (entries)
      
      \note
               - <b>source must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define ringbuffer_writep(name, source, count) (((count) <= p##name->Size)* (count)); \
                                              ((count) <= p##name->Size) ? \
                                                 ((count) <= p##name->Size- p##name->idxWrite) ? \
                                                    (memcpyflex((void *)(&p##name->Data[p##name->idxWrite]), (const void *)(source), (count), sizeof(p##name->Data[0])), p##name->idxWrite= ((p##name->idxWrite+ (count)) % p##name->Size), p##name->idxRead= (p##name->cntEntries >= p##name->Size) ? ((p##name->idxRead+ (count)) % p##name->Size) : (p##name->idxRead), p##name->cntEntries= _ulmin(p##name->Size, p##name->cntEntries+ _ulmin((count), p##name->Size))) : \
                                                    (memcpyflex((void *)(&p##name->Data[p##name->idxWrite]), (const void *)(source), (p##name->Size- p##name->idxWrite), sizeof(p##name->Data[0])), memcpyflex((void *)(&p##name->Data[0]), (const void *)((source)+ (p##name->Size- p##name->idxWrite)), _ulmin(((count)- (p##name->Size- p##name->idxWrite)), (ringbuffer_freeWrite(name)- (p##name->Size- p##name->idxWrite))), sizeof(p##name->Data[0])), p##name->idxWrite= ((count)- (((p##name->Size- p##name->idxWrite)))), p##name->idxRead= (p##name->cntEntries >= p##name->Size) ? ((p##name->idxRead+ (count)) % p##name->Size) : (p##name->idxRead), p##name->cntEntries= _ulmin(p##name->Size, p##name->cntEntries+ _ulmin((count), p##name->Size))) : \
                                                 (0)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> read items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   dest                       data to write buffer items (entries) to
      \param   count                      number of items (entries) to write
      
      \return                             number of read items (entries)
      
      \note
               - <b>dest must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define ringbuffer_readp(name, dest, count) circular_readp(name, dest, count)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> wipe (read without transfer) items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   count                      number of items (entries) to wipe
      
      \return                             number of wiped items (entries)
      
      \note
               - <b>dest must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define ringbuffer_wipep(name, count) circular_wipep(name, count)


// Generated value macros (identical to pointer macros but replaced "p##name->" with "name." plus removed suffix "p" at names end)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> empty buffer
/**
      \param	name   	                  name of the buffer

      \note
               - only indizes are cleared, data items are overwritten by next use<br>
*/
#define ringbuffer_reset(name) circular_reset(name)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> return size (NOT remaining space) of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             size (NOT remaining space) of buffer
*/
#define ringbuffer_size(name) (name.Size)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> return remaining READ space of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             remaining READ space of buffer
*/
#define ringbuffer_freeRead(name) circular_freeRead(name)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> return remaining WRITE space of buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             remaining WRITE space of buffer
*/
#define ringbuffer_freeWrite(name) ringbuffer_size(name)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> return allocated items (entries) in buffer
/**
      \param	name   	                  name of the buffer
      
      \return                             allocated items (entries) in buffer
*/
#define ringbuffer_countEntries(name) circular_countEntries(name)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> re-allocate already read items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   count                      number of read items (entries) to re-allocate
      
      \return                             number of re-allocated items (entries)
*/
#define ringbuffer_unRead(name, count) _ulmin((count), name.Size); \
                                       ((_ulmin((count), name.Size)) <= name.idxRead) ? \
                                          (name.idxRead-= (_ulmin((count), name.Size)), name.cntEntries= _ulmin(name.Size, name.cntEntries+ _ulmin((count), name.Size))) : \
                                          (name.idxRead= (name.Size- (((_ulmin((count), name.Size)))- name.idxRead)), name.cntEntries= _ulmin(name.Size, name.cntEntries+ _ulmin((count), name.Size)))

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> write items (entries) to buffer
/**
      \param	name   	                  name of the buffer
      \param   source                     data to write into buffer
      \param   count                      number items (entries) to read
      
      \return                             number of written items (entries)
      
      \note
               - <b>source must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define ringbuffer_write(name, source, count) (((count) <= name.Size)* (count)); \
                                              ((count) <= name.Size) ? \
                                                 ((count) <= name.Size- name.idxWrite) ? \
                                                    (memcpyflex((void *)(&name.Data[name.idxWrite]), (const void *)(source), (count), sizeof(name.Data[0])), name.idxWrite= ((name.idxWrite+ (count)) % name.Size), name.idxRead= (name.cntEntries >= name.Size) ? ((name.idxRead+ (count)) % name.Size) : (name.idxRead), name.cntEntries= _ulmin(name.Size, name.cntEntries+ _ulmin((count), name.Size))) : \
                                                    (memcpyflex((void *)(&name.Data[name.idxWrite]), (const void *)(source), (name.Size- name.idxWrite), sizeof(name.Data[0])), memcpyflex((void *)(&name.Data[0]), (const void *)((source)+ (name.Size- name.idxWrite)), _ulmin(((count)- (name.Size- name.idxWrite)), (ringbuffer_freeWrite(name)- (name.Size- name.idxWrite))), sizeof(name.Data[0])), name.idxWrite= ((count)- (((name.Size- name.idxWrite)))), name.idxRead= (name.cntEntries >= name.Size) ? ((name.idxRead+ (count)) % name.Size) : (name.idxRead), name.cntEntries= _ulmin(name.Size, name.cntEntries+ _ulmin((count), name.Size))) : \
                                                 (0)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> read items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   dest                       data to write buffer items (entries) to
      \param   count                      number of items (entries) to write
      
      \return                             number of read items (entries)
      
      \note
               - <b>dest must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define ringbuffer_read(name, dest, count) circular_read(name, dest, count)

//!   <b>Ringbuffer (fifo buffer that wipe unread data automatically):</b> wipe (read without transfer) items (entries) of buffer
/**
      \param	name   	                  name of the buffer
      \param   count                      number of items (entries) to wipe
      
      \return                             number of wiped items (entries)
      
      \note
               - <b>dest must MATCH (or being casted to) buffer item (entry) data type, otherwise unexspected pointer arithmetics will occur!!!</b><br>
*/
#define ringbuffer_wipe(name, count) circular_wipe(name, count)

//----------------------------------------------------------------------------
// 
//----------------------------------------------------------------------------

#endif
