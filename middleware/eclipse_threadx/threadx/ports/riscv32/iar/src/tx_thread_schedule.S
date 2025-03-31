/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   Thread                                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/* #define TX_SOURCE_CODE  */


/* Include necessary system files.  */

/*  #include "tx_api.h"
    #include "tx_thread.h"
    #include "tx_timer.h"  */
#include "hpm_csr_regs.h"

RISCV_MSTATUS_MIE   DEFINE  0x08
RISCV_MEIE_MTIE     DEFINE  0x880

    EXTERN      _tx_thread_execute_ptr
    EXTERN      _tx_thread_current_ptr
    EXTERN      _tx_timer_time_slice
#if defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE)
    EXTERN      _tx_execution_thread_enter
#endif
#ifdef TX_LOW_POWER
    EXTERN      tx_low_power_enter
    EXTERN      tx_low_power_exit
#endif

    SECTION `.isr_vector`:CODE:REORDER:NOROOT(2)
    CODE
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_schedule                                RISC-V32/IAR     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*    Tom van Leeuwen, Technolution B.V.                                  */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function waits for a thread control block pointer to appear in */
/*    the _tx_thread_execute_ptr variable.  Once a thread pointer appears */
/*    in the variable, the corresponding thread is resumed.               */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    None                                                                */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    _tx_initialize_kernel_enter          ThreadX entry function         */
/*    _tx_thread_system_return             Return to system from thread   */
/*    _tx_thread_context_restore           Restore thread's context       */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  01-24-2024     Zihan Xu                 Add D extend support          */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_schedule(VOID)
{  */
    PUBLIC  _tx_thread_schedule
_tx_thread_schedule:
; Enable mchtmr interrupts
    li     t0, RISCV_MEIE_MTIE
    csrrs  zero, mie, t0
    /* Enable interrupts.  */
    csrsi   mstatus, 0x08                               ; Enable interrupts

    /* Wait for a thread to execute.  */
    /* do
    {  */

    la      t0, _tx_thread_execute_ptr                  ; Pickup address of execute ptr
_tx_thread_schedule_loop:
    lw      t1, 0(t0)                                   ; Pickup next thread to execute
#ifndef TX_LOW_POWER
    beqz    t1, _tx_thread_schedule_loop                ; If NULL, wait for thread to execute
#else
    bnez    t1, _tx_thread_ready                        ; If NULL, wait for thread to execute
    csrci   mstatus, 0x08                               ; Lockout interrupts
    addi    sp, sp, -4
    sw      ra, 0(sp)
    call    tx_low_power_enter                      // Possibly enter low power mode
    wfi                                             // Wait for interrupt
    call    tx_low_power_exit                       // Exit low power mode
    lw      ra, 0(sp)
    addi    sp, sp, 4
    csrsi   mstatus, 0x08                               ; Enable interrupts
    /* Repick the address of execute ptr, so we don't need to save context before call tx_low_power_enter or tx_low_power_exit */
    la      t0, _tx_thread_execute_ptr                  ; Repickup address of execute ptr
    j       _tx_thread_schedule_loop
#endif

_tx_thread_ready:
    /* }
    while(_tx_thread_execute_ptr == TX_NULL);  */

    /* Yes! We have a thread to execute.  Lockout interrupts and
       transfer control to it.  */
    csrci   mstatus, 0x08                               ; Lockout interrupts

    /* Setup the current thread pointer.  */
    /* _tx_thread_current_ptr =  _tx_thread_execute_ptr;  */

    la      t0, _tx_thread_current_ptr                  ; Pickup current thread pointer address
    sw      t1, 0(t0)                                   ; Set current thread pointer

    /* Increment the run count for this thread.  */
    /* _tx_thread_current_ptr -> tx_thread_run_count++;  */

    lw      t2, 4(t1)                                   ; Pickup run count
    lw      t3, 24(t1)                                  ; Pickup time slice value
    addi    t2, t2, 1                                   ; Increment run count
    sw      t2, 4(t1)                                   ; Store new run count

    /* Setup time-slice, if present.  */
    /* _tx_timer_time_slice =  _tx_thread_current_ptr -> tx_thread_time_slice;  */

    la      t2, _tx_timer_time_slice                    ; Pickup time-slice variable address

    /* Switch to the thread's stack.  */
    /* SP =  _tx_thread_execute_ptr -> tx_thread_stack_ptr;  */

    lw      sp, 8(t1)                                   ; Switch to thread's stack
    sw      t3, 0(t2)                                   ; Store new time-slice*/

#if defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE)
    mv      s0, ra
    call    _tx_execution_thread_enter                  ; Call the thread execution enter function
    mv      ra, s0
#endif

    /* Determine if an interrupt frame or a synchronous task suspension frame
       is present.  */

    lw      t2, 0(sp)                                   ; Pickup stack type
    beqz    t2, _tx_thread_synch_return                 ; If 0, solicited thread return

    /* Determine if floating point registers need to be recovered.  */

#ifdef __riscv_flen
#if __riscv_flen == 32
    flw     f0, 0x90(sp)                                ; Recover ft0
    flw     f1, 0x94(sp)                                ; Recover ft1
    flw     f2, 0x98(sp)                                ; Recover ft2
    flw     f3, 0x9C(sp)                                ; Recover ft3
    flw     f4, 0xA0(sp)                                ; Recover ft4
    flw     f5, 0xA4(sp)                                ; Recover ft5
    flw     f6, 0xA8(sp)                                ; Recover ft6
    flw     f7, 0xAC(sp)                                ; Recover ft7
    flw     f8, 0xB0(sp)                                ; Recover fs0
    flw     f9, 0xB4(sp)                                ; Recover fs1
    flw     f10,0xB8(sp)                                ; Recover fa0
    flw     f11,0xBC(sp)                                ; Recover fa1
    flw     f12,0xC0(sp)                                ; Recover fa2
    flw     f13,0xC4(sp)                                ; Recover fa3
    flw     f14,0xC8(sp)                                ; Recover fa4
    flw     f15,0xCC(sp)                                ; Recover fa5
    flw     f16,0xD0(sp)                                ; Recover fa6
    flw     f17,0xD4(sp)                                ; Recover fa7
    flw     f18,0xD8(sp)                                ; Recover fs2
    flw     f19,0xDC(sp)                                ; Recover fs3
    flw     f20,0xE0(sp)                                ; Recover fs4
    flw     f21,0xE4(sp)                                ; Recover fs5
    flw     f22,0xE8(sp)                                ; Recover fs6
    flw     f23,0xEC(sp)                                ; Recover fs7
    flw     f24,0xF0(sp)                                ; Recover fs8
    flw     f25,0xF4(sp)                                ; Recover fs9
    flw     f26,0xF8(sp)                                ; Recover fs10
    flw     f27,0xFC(sp)                                ; Recover fs11
    flw     f28,0x100(sp)                               ; Recover ft8
    flw     f29,0x104(sp)                               ; Recover ft9
    flw     f30,0x108(sp)                               ; Recover ft10
    flw     f31,0x10C(sp)                               ; Recover ft11
    lw      t0, 0x110(sp)                               ; Recover fcsr
    csrw    fcsr, t0                                    ;
#elif __riscv_flen == 64
    flw     f0, 0x90(sp)                                ; Recover ft0
    flw     f1, 0x98(sp)                                ; Recover ft1
    flw     f2, 0xA0(sp)                                ; Recover ft2
    flw     f3, 0xA8(sp)                                ; Recover ft3
    flw     f4, 0xB0(sp)                                ; Recover ft4
    flw     f5, 0xB8(sp)                                ; Recover ft5
    flw     f6, 0xC0(sp)                                ; Recover ft6
    flw     f7, 0xC8(sp)                                ; Recover ft7
    flw     f8, 0xD0(sp)                                ; Recover fs0
    flw     f9, 0xD8(sp)                                ; Recover fs1
    flw     f10,0xE0(sp)                                ; Recover fa0
    flw     f11,0xE8(sp)                                ; Recover fa1
    flw     f12,0xF0(sp)                                ; Recover fa2
    flw     f13,0xF8(sp)                                ; Recover fa3
    flw     f14,0x100(sp)                               ; Recover fa4
    flw     f15,0x108(sp)                               ; Recover fa5
    flw     f16,0x110(sp)                               ; Recover fa6
    flw     f17,0x118(sp)                               ; Recover fa7
    flw     f18,0x120(sp)                               ; Recover fs2
    flw     f19,0x128(sp)                               ; Recover fs3
    flw     f20,0x130(sp)                               ; Recover fs4
    flw     f21,0x138(sp)                               ; Recover fs5
    flw     f22,0x140(sp)                               ; Recover fs6
    flw     f23,0x148(sp)                               ; Recover fs7
    flw     f24,0x150(sp)                               ; Recover fs8
    flw     f25,0x158(sp)                               ; Recover fs9
    flw     f26,0x160(sp)                               ; Recover fs10
    flw     f27,0x168(sp)                               ; Recover fs11
    flw     f28,0x170(sp)                               ; Recover ft8
    flw     f29,0x178(sp)                               ; Recover ft9
    flw     f30,0x180(sp)                               ; Recover ft10
    flw     f31,0x188(sp)                               ; Recover ft11
    lw      t0, 0x190(sp)                               ; Recover fcsr
    csrw    fcsr, t0                                    ;    
#endif
#endif

    lw      t0, 0x88(sp)                                ; Recover mcctlbeginaddr
    csrw    CSR_MCCTLBEGINADDR, t0                      ; Setup mcctlbeginaddr
    lw      t0, 0x8C(sp)                                ; Recover mcctldata
    csrw    CSR_MCCTLDATA, t0                           ; Setup mcctldata

    /* Recover standard registers.  */

    lw      t0, 0x78(sp)                                ; Recover mepc
    csrw    mepc, t0                                    ; Store mepc
#ifdef __riscv_flen
    li      t0, 0x3880                                  ; Prepare MPIP
#else
    li      t0, 0x1880                                  ; Prepare MPIP
#endif
    csrw    mstatus, t0                                 ; Enable MPIP

    lw      x1, 0x70(sp)                                ; Recover RA
    lw      x5, 0x4C(sp)                                ; Recover t0
    lw      x6, 0x48(sp)                                ; Recover t1
    lw      x7, 0x44(sp)                                ; Recover t2
    lw      x8, 0x30(sp)                                ; Recover s0
    lw      x9, 0x2C(sp)                                ; Recover s1
    lw      x10, 0x6C(sp)                               ; Recover a0
    lw      x11, 0x68(sp)                               ; Recover a1
    lw      x12, 0x64(sp)                               ; Recover a2
    lw      x13, 0x60(sp)                               ; Recover a3
    lw      x14, 0x5C(sp)                               ; Recover a4
    lw      x15, 0x58(sp)                               ; Recover a5
    lw      x16, 0x54(sp)                               ; Recover a6
    lw      x17, 0x50(sp)                               ; Recover a7
    lw      x18, 0x28(sp)                               ; Recover s2
    lw      x19, 0x24(sp)                               ; Recover s3
    lw      x20, 0x20(sp)                               ; Recover s4
    lw      x21, 0x1C(sp)                               ; Recover s5
    lw      x22, 0x18(sp)                               ; Recover s6
    lw      x23, 0x14(sp)                               ; Recover s7
    lw      x24, 0x10(sp)                               ; Recover s8
    lw      x25, 0x0C(sp)                               ; Recover s9
    lw      x26, 0x08(sp)                               ; Recover s10
    lw      x27, 0x04(sp)                               ; Recover s11
    lw      x28, 0x40(sp)                               ; Recover t3
    lw      x29, 0x3C(sp)                               ; Recover t4
    lw      x30, 0x38(sp)                               ; Recover t5
    lw      x31, 0x34(sp)                               ; Recover t6

#ifdef __riscv_flen
#if __riscv_flen == 32
    addi    sp, sp, 288                                 ; Recover stack frame - with floating point registers
#elif __riscv_flen == 64
    addi    sp, sp, 416                                 ; Recover stack frame - with floating point registers
#endif
#else
    addi    sp, sp, 160                                 ; Recover stack frame - without floating point registers
#endif
    mret                                                ; Return to point of interrupt

_tx_thread_synch_return:

#ifdef __riscv_flen
#if __riscv_flen == 32
    flw     f8, 0x50(sp)                                ; Recover fs0
    flw     f9, 0x54(sp)                                ; Recover fs1
    flw     f18,0x58(sp)                                ; Recover fs2
    flw     f19,0x5C(sp)                                ; Recover fs3
    flw     f20,0x60(sp)                                ; Recover fs4
    flw     f21,0x64(sp)                                ; Recover fs5
    flw     f22,0x68(sp)                                ; Recover fs6
    flw     f23,0x6C(sp)                                ; Recover fs7
    flw     f24,0x70(sp)                                ; Recover fs8
    flw     f25,0x74(sp)                                ; Recover fs9
    flw     f26,0x78(sp)                                ; Recover fs10
    flw     f27,0x7C(sp)                                ; Recover fs11
    lw      t0, 0x80(sp)                                ; Recover fcsr
    csrw    fcsr, t0                                    ;
#elif __riscv_flen == 64
    fld     f8, 0x50(sp)                                ; Recover fs0
    fld     f9, 0x58(sp)                                ; Recover fs1
    fld     f18,0x60(sp)                                ; Recover fs2
    fld     f19,0x68(sp)                                ; Recover fs3
    fld     f20,0x70(sp)                                ; Recover fs4
    fld     f21,0x78(sp)                                ; Recover fs5
    fld     f22,0x80(sp)                                ; Recover fs6
    fld     f23,0x88(sp)                                ; Recover fs7
    fld     f24,0x90(sp)                                ; Recover fs8
    fld     f25,0x98(sp)                                ; Recover fs9
    fld     f26,0xA0(sp)                                ; Recover fs10
    fld     f27,0xA8(sp)                                ; Recover fs11
    lw      t0, 0xB0(sp)                                ; Recover fcsr
    csrw    fcsr, t0                                    ;
#endif
#endif

    lw      t0, 0x48(sp)                                ; Recover mcctlbeginaddr
    csrw    CSR_MCCTLBEGINADDR, t0
    lw      t0, 0x4C(sp)                                ; Recover mcctldata
    csrw    CSR_MCCTLDATA, t0

    /* Recover standard preserved registers.  */
    /* Recover standard registers.  */

    lw      x1, 0x34(sp)                                ; Recover RA
    lw      x8, 0x30(sp)                                ; Recover s0
    lw      x9, 0x2C(sp)                                ; Recover s1
    lw      x18, 0x28(sp)                               ; Recover s2
    lw      x19, 0x24(sp)                               ; Recover s3
    lw      x20, 0x20(sp)                               ; Recover s4
    lw      x21, 0x1C(sp)                               ; Recover s5
    lw      x22, 0x18(sp)                               ; Recover s6
    lw      x23, 0x14(sp)                               ; Recover s7
    lw      x24, 0x10(sp)                               ; Recover s8
    lw      x25, 0x0C(sp)                               ; Recover s9
    lw      x26, 0x08(sp)                               ; Recover s10
    lw      x27, 0x04(sp)                               ; Recover s11
    lw      t0, 0x38(sp)                                ; Recover mstatus
    csrw    mstatus, t0                                 ; Store mstatus, enables interrupt
#ifdef __riscv_flen
#if __riscv_flen == 32
    addi    sp, sp, 136                                 ; Recover stack frame
#elif __riscv_flen == 64
    addi    sp, sp, 188                                 ; Recover stack frame
#endif
#else
    addi    sp, sp, 84                                  ; Recover stack frame
#endif
    ret                                                 ; Return to thread

/* }  */
    END

