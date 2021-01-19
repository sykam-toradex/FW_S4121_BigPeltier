/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    RTX_CONFIG.C
 *      Purpose: Configuration of RTX Kernel for Atmel AT91SAM7X
 *      Rev.:    V4.13
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2009 KEIL - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------
 *      Copyright (c) 2008-2016 Sykam GmbH. All rights reserved.
 *----------------------------------------------------------------------------
 *      2008-11-20 hs: os_tmr_call now configurable for local code placement
 *      2010-07-15 rr: added important comments
 *      2010-10-13 hs: irq_sys_handler configurable for local code placement
 *      2010-10-14 hs: fixed WRONG setting by keil of AIC_SMR[AT91C_ID_SYS]
 *      2010-10-21 hs: set AIC_SMR[AT91C_ID_SYS] dependent of OS_SYSIRQ define
 *      2010-10-21 hs: fixed local default irq function irq_sys_handler
 *      2010-10-29 hs: added user option OS_SYSIRQ_FORCE for debugging
 *      2010-10-29 hs: moved irq priority setting to AT91C_AIC_SYSSRCTYPE
 *      2010-10-29 hs: raised irq priority to 7 for SRCTYPE_INT_HIGH_LEVEL
 *      2010-10-29 hs: fixed conditional exclusion by OS_SYSIRQ_FORCE
 *      2010-10-30 hs: changed OS_IACK to OS_TIACK in irq_sys_handler
 *      2010-12-16 hs: made OS_TFIRQ action OS_SYSIRQ dependend by AT91C_TFIRQ
 *      2010-12-16 hs: made OS_(UN)LOCK OS_SYSIRQ dependend by AT91C_(UN)LOCK
 *      2010-12-17 hs: removed OS_SYSIRQ_FORCE option define
 *      2011-01-20 hs: changed AT91C_TFIRQ to work in level mode of irq
 *      2011-01-26 hs: changed AT91C_TFIRQ, OS_TIACK() and OS_TINIT() for level
 *      2011-01-26 hs: updated to RTX_Config.c file version 4.01
 *      2011-01-27 hs: added force_irq_enable to control forced scheduler runs
 *      2011-01-27 hs: added CHK_IRQFLAG to query force_irq_enable on demand only
 *      2011-02-16 hs: defined force_irq_enable always to avoid app cases
 *      2011-02-16 hs: defined irq_lock to avoid unexspected unlocking by rtx
 *      2011-02-16 hs: changed rtx macro OS_UNLOCK() to unlock only by irq_lock
 *      2011-05-05 hs: updated to RTX_Config.c file version 4.13 (incl. older)
 *      2011-05-11 hs: added version configuration item for different MDK-ARMs
 *      2011-09-16 hs: added error cases to os_error function
 *      2013-07-25 hs: added automatic support of PMC & AIC for sys_ctrl_interrupt
 *---------------------------------------------------------------------------*/

#include <RTL.h>
#include <AT91SAM7X512.h>                /* AT91SAM7X512 definitions         */

/*----------------------------------------------------------------------------
 *      RTX User configuration part BEGIN
 *---------------------------------------------------------------------------*/

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <h>Version Configuration
// =====================
//
//   <o>MDK-ARM version number (without dots) <0-999999>
//   <i> Define the version number (without dots) of your MDK-ARM
//   <i> Default: 413 (for version 4.13)
#ifndef OS_MDKVER
 #define OS_MDKVER      413
#endif

// </h>
// <h>Task Configuration
// =====================
//
//   <o>Number of concurrent running tasks <0-250>
//   <i> Define max. number of tasks that will run at the same time.
//   <i> Default: 6
#ifndef OS_TASKCNT
 #define OS_TASKCNT     45
#endif

//   <o>Number of tasks with user-provided stack <0-250>
//   <i> Define the number of tasks that will use a bigger stack.
//   <i> The memory space for the stack is provided by the user.
//   <i> Default: 0     RR -> in final version adjust this value to number of tasks with user stack 
#ifndef OS_PRIVCNT
 #define OS_PRIVCNT     0
#endif

//   <o>Task stack size [bytes] <20-4096:8><#/4>
//   <i> Set the stack size for tasks which is assigned by the system.
//   <i> Default: 200
#ifndef OS_STKSIZE
 #define OS_STKSIZE     85
#endif

// <q>Check for the stack overflow
// ===============================
// <i> Include the stack checking code for a stack overflow.
// <i> Note that additional code reduces the RTX performance.
#ifndef OS_STKCHECK
 #define OS_STKCHECK    1
#endif

// </h>
// <h>Tick Timer Configuration
// =============================
//   <o>Hardware timer <0=> Timer 0 <1=> Timer 1 <2=> Timer 2 <3=> PIT
//   <i> Define the on-chip timer used as a time-base for RTX.
//   <i> Default: PIT
#ifndef OS_TIMER
 #define OS_TIMER       3
#endif

//   <q>Common IRQ System Handler for PIT timer
//   <i> Include a code for Common System Interrupt Handler
//   <i> when a PIT timer is used
#ifndef OS_SYSIRQ
 #define OS_SYSIRQ      1
#endif

//   <q>Local IRQ System Handler
//   <i> Check this option to place "__irq void irq_sys_handler (void)" function inside your own code (instead inside RTX_config.c)
#ifndef OS_SYSIRQ_CALL_LOCAL
 #define OS_SYSIRQ_CALL_LOCAL 1
#endif

//   <o>Timer clock value [Hz] <1-1000000000>
//   <i> Set the timer clock value for selected timer.
//   <i> Default PIT: 2995200  (2.99 MHz at 47.9 MHz MCK and PIT prescaler by 16)
//   <i> Default TCx: 1497600  (1.50 MHz at 47.9 MHz MCK and PIT prescaler by 32)
//   <i> RR: OS_CLOCK = PIT input clock = MCK/16 = 47,9232 MHz / 16 = 2,995200 MHz!
//   <i>     See AT91SAM7X.h for further information!
#ifndef OS_CLOCK
 #define OS_CLOCK       2995200
#endif

//   <o>Timer tick value [us] <1-1000000>
//   <i> Set the timer tick value for selected timer.
//   <i> Default: 10000  (10ms)
#ifndef OS_TICK
 #define OS_TICK        10000
#endif

// </h>

// <h>System Configuration
// =======================
// <e>Round-Robin Task switching
// =============================
// <i> Enable Round-Robin Task switching.
#ifndef OS_ROBIN
 #define OS_ROBIN       1
#endif

//   <o>Round-Robin Timeout [ticks] <1-1000>
//   <i> Define how long a task will execute before a task switch.
//   <i> Default: 5
#ifndef OS_ROBINTOUT
 #define OS_ROBINTOUT   5
#endif

// </e>

//   <o>Number of user timers <0-250>
//   <i> Define max. number of user timers that will run at the same time.
//   <i> Default: 0  (User timers disabled)
#ifndef OS_TIMERCNT
 #define OS_TIMERCNT    10
#endif

//   <q>Local os_tmr_call callback function
//   <i> Check this option to place "void os_tmr_call (U16 u16Info)" function inside your own code (instead inside RTX_config.c)
#ifndef OS_TMR_CALL_LOCAL
 #define OS_TMR_CALL_LOCAL 1
#endif

#if OS_MDKVER >= 413
//   <o>ISR FIFO Queue size (requires MDK-ARM 413 and later) <4=>   4 entries  <8=>   8 entries
//                                                           <12=> 12 entries  <16=> 16 entries
//                                                           <24=> 24 entries  <32=> 32 entries
//                                                           <48=> 48 entries  <64=> 64 entries
//                                                           <96=> 96 entries
//   <i> ISR functions store requests to this buffer,
//   <i> when they are called from the IRQ handler.
//   <i> Default: 16 entries (requires MDK-ARM 413 and later)
#ifndef OS_FIFOSZ
 #define OS_FIFOSZ      96
#endif
#endif
// </h>


//------------- <<< end of configuration section >>> -----------------------

// Standard library system mutexes
// ===============================
//  Define max. number system mutexes that are used to protect 
//  the arm standard runtime library. For microlib they are not used.
#ifndef OS_MUTEXCNT
 #define OS_MUTEXCNT    8
#endif

/*----------------------------------------------------------------------------
 *      RTX User configuration part END
 *---------------------------------------------------------------------------*/

#define AIC             AT91C_BASE_AIC                  /* Abbreviations    */
#define SYS             AT91C_BASE_SYS

#if   (OS_TIMER == 0)                                   /* Timer/Counter 0  */
 #define ID_TC          AT91C_ID_TC0
 #define TCx            AT91C_BASE_TC0
#elif (OS_TIMER == 1)                                   /* Timer/Counter 1  */
 #define ID_TC          AT91C_ID_TC1
 #define TCx            AT91C_BASE_TC1
#elif (OS_TIMER == 2)                                   /* Timer/Counter 2  */
 #define ID_TC          AT91C_ID_TC2
 #define TCx            AT91C_BASE_TC2
#elif (OS_TIMER == 3)                                   /* PIT              */
 #define ID_TC          AT91C_ID_SYS

 #if OS_SYSIRQ > 0
  /* Settings for stable sys irq usage */
  #define AT91C_AIC_SYSSRCTYPE (AT91C_AIC_SRCTYPE_HIGH_LEVEL| AT91C_AIC_PRIOR_LOWEST)
    
  /* Re-trigger os tick irq (initiate hw irq by software write access) */                          
  #define AT91C_TFIRQ          SYS->PMC_SCER = AT91C_PMC_MOSCS; /* HS: Enable re-triggering of sys irq by PMC_MOSCS */  \
                               SYS->PMC_IER = AT91C_PMC_MOSCS;  /* HS: Enable re-triggering of sys irq by PMC_MOSCS */
  
  #define AT91C_TIACK          SYS->PMC_IDR = AT91C_PMC_MOSCS;  /* HS: Disable re-triggering irq source */  \
                               SYS->PMC_SCER = AT91C_PMC_MOSCS; /* HS: Enable re-triggering of sys irq by PMC_MOSCS */  \
                               AIC->AIC_EOICR = SYS->PITC_PIVR; /* RR: Write any value to AIC_EOICR to ack that interrupt has finished */
                               
                               // AIC->AIC_ICCR  = OS_TIM_;        /* RR: Clear SYS interrupt (set 0x2 in AIC_ICCR), HS: works for edge mode only!! */   \  
 #else
  /* Original KEIL settings - not stable for full sys irq use because re-triggering irq by aic's iscr works for edge mode only */
  #define AT91C_AIC_SYSSRCTYPE (AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE| AT91C_AIC_PRIOR_LOWEST)
   
  /* Re-trigger os tick irq (initiate hw irq by software write access) */
  #define AT91C_TFIRQ          AIC->AIC_ISCR  = OS_TIM_         /* RR: Set SYS bit in AIC_ISCR, HS: works for edge mode only!! */
   
  #define AT91C_TIACK          AIC->AIC_ICCR  = OS_TIM_;        /* RR: Clear SYS interrupt (set 0x2 in AIC_ICCR), HS: works for edge mode only!! */   \
                               AIC->AIC_EOICR = SYS->PITC_PIVR; /* RR: Write any value to AIC_EOICR to ack that interrupt has finished */
   
 #endif
#else
 #error OS_TIMER invalid
#endif

#define OS_TIM_         (1 << ID_TC)                                       /* Interrupt Mask --> RR: If PIT selected OS_TIM_ = 1 << 1 = 0x2 */
#define OS_TRV          ((U32)(((double)OS_CLOCK*(double)OS_TICK)/1E6)-1)  /* RR: Internal timer value calculation */



#if (OS_TIMER < 3)  /* RR: selected timer = 0...2 */ 
 #define OS_TVAL        (TCx->TC_CV & 0x0000FFFF)       /*  Timer Value     */
 #define OS_TOVF        ((TCx->TC_SR >> 4) & 1)         /*  Reload Flag     */
 #define OS_TFIRQ()     CHK_IRQFLAG { AIC->AIC_ISCR  = OS_TIM_; }    /*  Force Interrupt */
 #define OS_TIACK()     AIC->AIC_ICCR  = OS_TIM_;       /*  Interrupt Ack   */ \
                        AIC->AIC_EOICR = TCx->TC_SR;
 #define OS_TINIT()     SYS->PMC_PCER  = OS_TIM_;       /*  Initialization  */ \
                        TCx->TC_CCR    = AT91C_TC_CLKEN | AT91C_TC_SWTRG;      \
                        TCx->TC_CMR    = 2 | AT91C_TC_CPCTRG;                  \
                        TCx->TC_RC     = OS_TRV;                               \
                        TCx->TC_IER    = AT91C_TC_CPCS;                        \
                        AIC->AIC_SPU   = (U32)os_def_interrupt;                \
                        AIC->AIC_SVR[ID_TC] = (U32)os_clock_interrupt;         \
                        AIC->AIC_SMR[ID_TC] =                                  \
                        AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE | AT91C_AIC_PRIOR_LOWEST;
                        
#else
 /* RR: selected timer = PIT = 3 */
 #define OS_TVAL        (SYS->PITC_PIIR & 0x000FFFFF)   /*  Timer Value --> RR: This is the CPIV value in PITC_PIIR; zero after reset */
 #define OS_TOVF        (SYS->PITC_PISR & 1)            /*  Overflow Flag   */

 #define OS_TFIRQ()     SET_IRQFLAG; CHK_IRQFLAG { AT91C_TFIRQ; }

 #define OS_TIACK()     CLR_IRQFLAG;                    /*  Interrupt Ack   */ \
                        AT91C_TIACK;

 #define OS_TINIT()     SYS->PITC_PIMR = OS_TRV |       /*  Initialization --> RR: function will be called in os_sys_init(); OS_TRV is the system clock set by user!! */ \
                        AT91C_PITC_PITIEN | AT91C_PITC_PITEN;                  \
                        AIC->AIC_SPU = (U32)os_def_interrupt;                  \
                        AIC->AIC_SVR[ID_TC] = (U32)sys_ctrl_interrupt;         \
                        AIC->AIC_SMR[ID_TC] =                                  \
                        AT91C_AIC_SYSSRCTYPE;
                        
#endif

#define OS_IACK()       AIC->AIC_EOICR = 0;             /* Interrupt Ack    */

#define OS_LOCK()       AIC->AIC_IDCR  = OS_TIM_;       /* Lock    RR: Set SYS bit in AIC_IDCR to disable system interrupts */
#define OS_UNLOCK()     if (!irq_lock) AIC->AIC_IECR  = OS_TIM_;       /* Unlock  RR: Set SYS bit in AIC_IECR to enable system interrupts */

/* WARNING: Using IDLE mode might cause you troubles while debugging. */
#define _idle_()        SYS->PMC_SCDR = AT91C_PMC_MOSCS;

BIT irq_lock = __FALSE;
BIT force_irq_enable = __TRUE;

#if (OS_TIMER == 3 && OS_SYSIRQ == 1)
 BIT force_irq;
 #define SET_IRQFLAG    force_irq = __TRUE;
 #define CLR_IRQFLAG    force_irq = __FALSE;
 #define CHK_IRQFLAG    if (force_irq_enable && !(SYS->PMC_IMR & AT91C_PMC_MOSCS))
#else
 #define SET_IRQFLAG
 #define CLR_IRQFLAG
 #define CHK_IRQFLAG    if (force_irq_enable)
#endif

/*----------------------------------------------------------------------------
 *      Global Functions
 *---------------------------------------------------------------------------*/

extern void os_clock_interrupt (void);

/*--------------------------- test sys_pmc_irq_handler ----------------------*/
__irq void irq_sysforce_handler (void) {
   /* Test System Interrupt Handler for: PMC  */

  for(;;);
}

#if (OS_TIMER == 3 && OS_SYSIRQ == 1)
/*--------------------------- sys_irq_handler -------------------------------*/
#if OS_SYSIRQ_CALL_LOCAL == 0
__irq void irq_sys_handler (void) {
   /* Common System Interrupt Handler for: DBGU, RSTC, RTT, WDT and PMC  */
   /* system peripheral interrupts.                                      */

  for(;;);
}
#else
__irq void irq_sys_handler (void) ;
#endif

/*--------------------------- sys_ctrl_interrupt ----------------------------*/

__asm void sys_ctrl_interrupt (void) {
   /* Common System Interrupt Handler entry. */
        PRESERVE8
        ARM
        
        STMDB   SP!,{R0}                        ; Save Work Register
        LDR     R0,=__cpp((U32)AT91C_PITC_PISR) ; PIT Status Register
        LDR     R0,[R0]                         ; Read PIT ISR

        TST     R0,#__cpp(AT91C_PITC_PITS)      ; Check for PIT interrupt
        LDMIANE SP!,{R0}                        ; Restore Work Register
        LDRNE   PC,=__cpp(os_clock_interrupt)   ; Jump to RTOS Clock IRQ

        IF (OS_TIMER == 3)                      ; Forced interrupt via PMC
        LDR     R0,=__cpp((U32)AT91C_PMC_IMR)   ; Check for forced interrupt
        LDR     R0,[R0]                         ; Read PMC Clock Status
        
        TST     R0,# __cpp(AT91C_PMC_MOSCS)

        ELSE                                    ; Forced interrupt via AIC
        LDR     R0,=__cpp(&force_irq)           ; Check for forced interrupt
        LDRB    R0,[R0]                         ; Read os_psh_flag

        CMP     R0,#__cpp(__TRUE)               ; Check if __TRUE
        ENDIF

        LDMIA   SP!,{R0}                        ; Restore Work Register

        IF (OS_TIMER < 3)                       ; After forced interrupt via AIC
        LDREQ   PC,=__cpp(os_clock_interrupt)   ; Jump to RTOS Clock IRQ

        ELSE                                    ; After forced interrupt via PMC
        //-LDRNE   PC,=__cpp(irq_sysforce_handler) ; Jump to PMC Test IRQ
        LDRNE   PC,=__cpp(os_clock_interrupt)   ; Jump to RTOS Clock IRQ
        ENDIF

        LDR     PC,=__cpp(irq_sys_handler)      ; Jump to SYS IRQ Handler
}
#else
 #define sys_ctrl_interrupt    	os_clock_interrupt
#endif


/*--------------------------- os_idle_demon ---------------------------------*/

__task void os_idle_demon (void) {
   /* The idle demon is a system task, running when no other task is ready */
   /* to run. The 'os_xxx' function calls are not allowed from this task.  */

   for (;;) {
   /* HERE: include optional user code to be executed when no task runs.*/
   }
}


/*--------------------------- os_tmr_call -----------------------------------*/

#if OS_TMR_CALL_LOCAL == 0
void os_tmr_call (U16 info) {
   /* This function is called when the user timer has expired. Parameter   */
   /* 'info' holds the value, defined when the timer was created.          */

   /* HERE: include optional user code to be executed on timeout. */
}
#endif

/*--------------------------- os_error --- MDK-ARM 413 and later-------------*/
#if OS_MDKVER >= 413
void os_error (U32 err_code) {
   /* This function is called when a runtime error is detected. Parameter */
   /* 'err_code' holds the runtime error code (defined in RTL.H).         */

   OS_TID err_task;

   switch (err_code) {
      case OS_ERR_STK_OVF:
         /* Identify the task with stack overflow. */
         err_task= isr_tsk_get();
         err_task= err_task;
         break;
         
      case OS_ERR_FIFO_OVF:
         break;
         
      case OS_ERR_MBX_OVF:
         break;
         
      default:
         err_code= err_code;
         break;
   }

   /* HERE: include optional code to be executed on runtime error. */
   for (;;);
}
#endif

/*--------------------------- os_stk_overflow --- MDK-ARM before 413---------*/
#if OS_MDKVER < 413
void os_stk_overflow (OS_TID task_id) {
   /* This function is called when a stack overflow is detected. Parameter */
   /* 'task_id' holds the id of this task. You can use 'RTX Kernel' dialog,*/
   /* page 'Active Tasks' to check, which task needs a bigger stack.       */

   /* HERE: include optional code to be executed on stack overflow. */
   for (;;);
}
#endif
/*----------------------------------------------------------------------------
 *      RTX Configuration Functions
 *---------------------------------------------------------------------------*/

static void os_def_interrupt (void) __irq  {
   /* Default Interrupt Function: may be called when timer ISR is disabled */
   OS_IACK();
}


#include <RTX_lib.c>

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
