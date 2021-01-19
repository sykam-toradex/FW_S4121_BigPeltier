;/*----------------------------------------------------------------------------
; *      RL-ARM - RTX
; *----------------------------------------------------------------------------
; * <<< Use Configuration Wizard in Context Menu >>>
; *----------------------------------------------------------------------------
; *      Name:    SWI_TABLE.S
; *      Purpose: Pre-defined SWI Table
; *      Rev.:    V4.13
; *----------------------------------------------------------------------------
; *      This code is part of the RealView Run-Time Library.
; *      Copyright (c) 2004-2010 KEIL - An ARM Company. All rights reserved.
; *----------------------------------------------------------------------------
; *      Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
; *----------------------------------------------------------------------------
; *      2013-06-06 hs: added configuration wizard control
; *      2013-06-12 hs: added configuration wizard documentation
; *      2013-06-20 hs: updated configuration wizard documentation
; *      2013-06-27 hs: fixed configuration wizard control
; *---------------------------------------------------------------------------*/

;/*
;// <e> Enable user SoftWare-Interrupt (SWI)
;// <i> EXAMPLE user Software-Interrupt function (refer to 'RL-ARM User's Guide: SWI Functions' step 5):
;// <i> // Note: step 4 of 'RL-ARM User's Guide: SWI Functions' is handled here automatically
;// <i> // Note: below the safe macros of support.h are used instead of the original attribute types
;// <i> PROTOTYPE:
;// <i> void SWI_PROTO(8) inc_5bit (U32 *pu32Value);
;// <i> FUNCTION CODE:
;// <i> // Note: there is NO explicit function name (only as comment) below!
;// <i> void SWI_FUNC(8) /* inc_5bit */ (U32 *pu32Value)
;// <i> {
;// <i>     /* A protected function (cpu supervisor state; all irqs except firq blocked) to increment a 5-bit counter. */
;// <i>      *pu32Value = (*pu32Value + 1) & 0x1F;
;// <i> }
;// <i> // Note: the function template and return type are absolutely free defineable
;//   <o1> Maximum SoftWare-Interrupt (SWI) number <8-32>
;//   <i> Default 8 (SWI 0..7 are used by RTX Kernel, maximum supported: 32)
;// </e>
;*/
SWI_USR         EQU     0
SWI_MAX         EQU     8

;/*----------------------------------------------------------------------------
; * <<< end of configuration section >>>
; *---------------------------------------------------------------------------*/

                AREA    SWI_TABLE, CODE, READONLY

                EXPORT  SWI_Count

SWI_Cnt         EQU    (SWI_End-SWI_Table)/4
SWI_Count       DCD     SWI_Cnt

                IMPORT  __SWI_0
                IMPORT  __SWI_1
                IMPORT  __SWI_2
                IMPORT  __SWI_3
                IMPORT  __SWI_4
                IMPORT  __SWI_5
                IMPORT  __SWI_6
                IMPORT  __SWI_7

; Import user SWI functions here.
   IF SWI_USR >= 1
      IF SWI_MAX >= 8
                   IMPORT  __SWI_8
      ENDIF
      IF SWI_MAX >= 9
                   IMPORT  __SWI_9
      ENDIF
      IF SWI_MAX >= 10
                   IMPORT  __SWI_10
      ENDIF
      IF SWI_MAX >= 11
                   IMPORT  __SWI_11
      ENDIF
      IF SWI_MAX >= 12
                   IMPORT  __SWI_12
      ENDIF
      IF SWI_MAX >= 13
                   IMPORT  __SWI_13
      ENDIF
      IF SWI_MAX >= 14
                   IMPORT  __SWI_14
      ENDIF
      IF SWI_MAX >= 15
                   IMPORT  __SWI_15
      ENDIF
      IF SWI_MAX >= 16
                   IMPORT  __SWI_16
      ENDIF
      IF SWI_MAX >= 17
                   IMPORT  __SWI_17
      ENDIF
      IF SWI_MAX >= 18
                   IMPORT  __SWI_18
      ENDIF
      IF SWI_MAX >= 19
                   IMPORT  __SWI_19
      ENDIF
      IF SWI_MAX >= 20
                   IMPORT  __SWI_20
      ENDIF
      IF SWI_MAX >= 21
                   IMPORT  __SWI_21
      ENDIF
      IF SWI_MAX >= 22
                   IMPORT  __SWI_22
      ENDIF
      IF SWI_MAX >= 23
                   IMPORT  __SWI_23
      ENDIF
      IF SWI_MAX >= 24
                   IMPORT  __SWI_24
      ENDIF
      IF SWI_MAX >= 25
                   IMPORT  __SWI_25
      ENDIF
      IF SWI_MAX >= 26
                   IMPORT  __SWI_26
      ENDIF
      IF SWI_MAX >= 27
                   IMPORT  __SWI_27
      ENDIF
      IF SWI_MAX >= 28
                   IMPORT  __SWI_28
      ENDIF
      IF SWI_MAX >= 29
                   IMPORT  __SWI_29
      ENDIF
      IF SWI_MAX >= 30
                   IMPORT  __SWI_30
      ENDIF
      IF SWI_MAX >= 31
                   IMPORT  __SWI_31
      ENDIF
      IF SWI_MAX >= 32
                   IMPORT  __SWI_32
      ENDIF
   ENDIF
      
                EXPORT  SWI_Table
SWI_Table
                DCD     __SWI_0                 ; SWI 0 used by RTX
                DCD     __SWI_1                 ; SWI 1 used by RTX
                DCD     __SWI_2                 ; SWI 2 used by RTX
                DCD     __SWI_3                 ; SWI 3 used by RTX
                DCD     __SWI_4                 ; SWI 4 used by RTX
                DCD     __SWI_5                 ; SWI 5 used by RTX
                DCD     __SWI_6                 ; SWI 6 used by RTX
                DCD     __SWI_7                 ; SWI 7 used by RTX

; Insert user SWI functions here. SWI 0..7 are used by RTX Kernel.
   IF SWI_USR >= 1
      IF SWI_MAX >= 8
                   DCD     __SWI_8                 ; user SWI function
      ENDIF
      IF SWI_MAX >= 9
                   DCD     __SWI_9                 ; user SWI function
      ENDIF
      IF SWI_MAX >= 10
                   DCD     __SWI_10                ; user SWI function
      ENDIF
      IF SWI_MAX >= 11
                   DCD     __SWI_11                ; user SWI function
      ENDIF
      IF SWI_MAX >= 12
                   DCD     __SWI_12                ; user SWI function
      ENDIF
      IF SWI_MAX >= 13
                   DCD     __SWI_13                ; user SWI function
      ENDIF
      IF SWI_MAX >= 14
                   DCD     __SWI_14                ; user SWI function
      ENDIF
      IF SWI_MAX >= 15
                   DCD     __SWI_15                ; user SWI function
      ENDIF
      IF SWI_MAX >= 16
                   DCD     __SWI_16                ; user SWI function
      ENDIF
      IF SWI_MAX >= 17
                   DCD     __SWI_17                ; user SWI function
      ENDIF
      IF SWI_MAX >= 18
                   DCD     __SWI_18                ; user SWI function
      ENDIF
      IF SWI_MAX >= 19
                   DCD     __SWI_19                ; user SWI function
      ENDIF
      IF SWI_MAX >= 20
                   DCD     __SWI_20                ; user SWI function
      ENDIF
      IF SWI_MAX >= 21
                   DCD     __SWI_21                ; user SWI function
      ENDIF
      IF SWI_MAX >= 22
                   DCD     __SWI_22                ; user SWI function
      ENDIF
      IF SWI_MAX >= 23
                   DCD     __SWI_23                ; user SWI function
      ENDIF
      IF SWI_MAX >= 24
                   DCD     __SWI_24                ; user SWI function
      ENDIF
      IF SWI_MAX >= 25
                   DCD     __SWI_25                ; user SWI function
      ENDIF
      IF SWI_MAX >= 26
                   DCD     __SWI_26                ; user SWI function
      ENDIF
      IF SWI_MAX >= 27
                   DCD     __SWI_27                ; user SWI function
      ENDIF
      IF SWI_MAX >= 28
                   DCD     __SWI_28                ; user SWI function
      ENDIF
      IF SWI_MAX >= 29
                   DCD     __SWI_29                ; user SWI function
      ENDIF
      IF SWI_MAX >= 30
                   DCD     __SWI_30                ; user SWI function
      ENDIF
      IF SWI_MAX >= 31
                   DCD     __SWI_31                ; user SWI function
      ENDIF
      IF SWI_MAX >= 32
                   DCD     __SWI_32                ; user SWI function
      ENDIF
   ENDIF
   
SWI_End

                END

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
