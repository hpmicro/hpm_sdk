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

    EXTERN      _tx_thread_current_ptr
    EXTERN      _tx_thread_system_state
    EXTERN      _tx_thread_system_stack_ptr
#if defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE)
    EXTERN      _tx_execution_isr_enter
#endif

    SECTION `.isr_vector`:CODE:REORDER:NOROOT(2)
    CODE
/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _tx_thread_context_save                            RISC-V32/IAR     */
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*    Tom van Leeuwen, Technolution B.V.                                  */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function saves the context of an executing thread in the       */
/*    beginning of interrupt processing.  The function also ensures that  */
/*    the system stack is used upon return to the calling ISR.            */
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
/*    ISRs                                                                */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  09-30-2020     William E. Lamie         Initial Version 6.1           */
/*  01-24-2024     Zihan Xu                 Add D extend support          */
/*                                                                        */
/**************************************************************************/
/* VOID   _tx_thread_context_save(VOID)
{  */
    PUBLIC      _tx_thread_context_save
_tx_thread_context_save:

    /* Upon entry to this routine, it is assumed that interrupts are locked
       out and the interrupt stack fame has been allocated and x1 (ra) has
       been saved on the stack. */

    sw      x5, 0x4C(sp)                                ; First store t0 and t1
    sw      x6, 0x48(sp)

    la      x5, _tx_thread_system_state                 ; Pickup address of system state
    lw      x6, 0(x5)                                   ; Pickup system state

    /* Check for a nested interrupt condition.  */
    /* if (_tx_thread_system_state++)
    {  */
    beqz    x6, _tx_thread_not_nested_save              ; If 0, first interrupt condition
    addi    x6, x6, 1                                   ; Increment the interrupt counter
    sw      x6, 0(x5)                                   ; Store the interrupt counter

    /* Nested interrupt condition.
       Save the reset of the scratch registers on the stack and return to the
       calling ISR.  */

    sw      x7, 0x44(sp)                                ; Store t2
    sw      x8, 0x30(sp)                                ; Store s0
    sw      x10, 0x6C(sp)                               ; Store a0
    sw      x11, 0x68(sp)                               ; Store a1
    sw      x12, 0x64(sp)                               ; Store a2
    sw      x13, 0x60(sp)                               ; Store a3
    sw      x14, 0x5C(sp)                               ; Store a4
    sw      x15, 0x58(sp)                               ; Store a5
    sw      x16, 0x54(sp)                               ; Store a6
    sw      x17, 0x50(sp)                               ; Store a7
    sw      x28, 0x40(sp)                               ; Store t3
    sw      x29, 0x3C(sp)                               ; Store t4
    sw      x30, 0x38(sp)                               ; Store t5
    sw      x31, 0x34(sp)                               ; Store t6
    csrr    t0, mepc                                    ; Load exception program counter
    sw      t0, 0x78(sp)                                ; Save it on the stack

    csrr    t0, CSR_MCCTLBEGINADDR                      ; Load mcctlbeginaddr
    sw      t0, 0x88(sp)                                ; Save it on the stack
    csrr    t0, CSR_MCCTLDATA                           ; Load mcctldata
    sw      t0, 0x8C(sp)                                ; Save it on the stack

#ifdef __riscv_flen
#if __riscv_flen == 32
    /* Save floating point scratch registers.  */

    fsw     f0, 0x90(sp)                                ; Store ft0
    fsw     f1, 0x94(sp)                                ; Store ft1
    fsw     f2, 0x98(sp)                                ; Store ft2
    fsw     f3, 0x9C(sp)                                ; Store ft3
    fsw     f4, 0xA0(sp)                                ; Store ft4
    fsw     f5, 0xA4(sp)                                ; Store ft5
    fsw     f6, 0xA8(sp)                                ; Store ft6
    fsw     f7, 0xAC(sp)                                ; Store ft7
    fsw     f10,0xB8(sp)                                ; Store fa0
    fsw     f11,0xBC(sp)                                ; Store fa1
    fsw     f12,0xC0(sp)                                ; Store fa2
    fsw     f13,0xC4(sp)                                ; Store fa3
    fsw     f14,0xC8(sp)                                ; Store fa4
    fsw     f15,0xCC(sp)                                ; Store fa5
    fsw     f16,0xD0(sp)                                ; Store fa6
    fsw     f17,0xD4(sp)                                ; Store fa7
    fsw     f28,0x100(sp)                               ; Store ft8
    fsw     f29,0x104(sp)                               ; Store ft9
    fsw     f30,0x108(sp)                               ; Store ft10
    fsw     f31,0x10C(sp)                               ; Store ft11
    csrr    t0, fcsr
    sw      t0, 0x110(sp)                               ; Store fcsr
#elif __riscv_flen == 64
    fsd     f0, 0x90(sp)                                ; Store ft0
    fsd     f1, 0x98(sp)                                ; Store ft1
    fsd     f2, 0xA0(sp)                                ; Store ft2
    fsd     f3, 0xA8(sp)                                ; Store ft3
    fsd     f4, 0xB0(sp)                                ; Store ft4
    fsd     f5, 0xB8(sp)                                ; Store ft5
    fsd     f6, 0xC0(sp)                                ; Store ft6
    fsd     f7, 0xC8(sp)                                ; Store ft7
    fsd     f10,0xE0(sp)                                ; Store fa0
    fsd     f11,0xE8(sp)                                ; Store fa1
    fsd     f12,0xF0(sp)                                ; Store fa2
    fsd     f13,0xF8(sp)                                ; Store fa3
    fsd     f14,0x100(sp)                               ; Store fa4
    fsd     f15,0x108(sp)                               ; Store fa5
    fsd     f16,0x110(sp)                               ; Store fa6
    fsd     f17,0x118(sp)                               ; Store fa7
    fsd     f28,0x170(sp)                               ; Store ft8
    fsd     f29,0x178(sp)                               ; Store ft9
    fsd     f30,0x180(sp)                               ; Store ft10
    fsd     f31,0x188(sp)                               ; Store ft11
    csrr    t0, fcsr
    sw      t0, 0x190(sp)                               ; Store fcsr
#endif
#endif

#if defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE)
    mv      s0, ra
    call    _tx_execution_isr_enter                     ; Call the ISR execution enter function
#endif

    ret                                                 ; Return to calling ISR

_tx_thread_not_nested_save:
    /* }  */

    /* Otherwise, not nested, check to see if a thread was running.  */
    /* else if (_tx_thread_current_ptr)
    {  */
    addi    x6, x6, 1                                   ; Increment the interrupt counter
    sw      x6, 0(x5)                                   ; Store the interrupt counter

    /* Not nested: Find the user thread that was running and load our SP */

    lw      x5, _tx_thread_current_ptr                  ; Pickup current thread pointer
    beqz    x5, _tx_thread_idle_system_save             ; If NULL, idle system was interrupted

    /* Save the standard scratch registers.  */

    sw      x7, 0x44(sp)                                ; Store t2
    sw      x8, 0x30(sp)                                ; Store s0
    sw      x10, 0x6C(sp)                               ; Store a0
    sw      x11, 0x68(sp)                               ; Store a1
    sw      x12, 0x64(sp)                               ; Store a2
    sw      x13, 0x60(sp)                               ; Store a3
    sw      x14, 0x5C(sp)                               ; Store a4
    sw      x15, 0x58(sp)                               ; Store a5
    sw      x16, 0x54(sp)                               ; Store a6
    sw      x17, 0x50(sp)                               ; Store a7
    sw      x28, 0x40(sp)                               ; Store t3
    sw      x29, 0x3C(sp)                               ; Store t4
    sw      x30, 0x38(sp)                               ; Store t5
    sw      x31, 0x34(sp)                               ; Store t6

    csrr    t0, mepc                                    ; Load exception program counter
    sw      t0, 0x78(sp)                                ; Save it on the stack

    csrr    t0, CSR_MCCTLBEGINADDR                      ; Load mcctlbeginaddr
    sw      t0, 0x88(sp)                                ; Save it on the stack
    csrr    t0, CSR_MCCTLDATA                           ; Load mcctldata
    sw      t0, 0x8C(sp)                                ; Save it on the stack

#ifdef __riscv_flen
#if __riscv_flen == 32
    /* Save floating point scratch registers.  */

    fsw     f0, 0x90(sp)                                ; Store ft0
    fsw     f1, 0x94(sp)                                ; Store ft1
    fsw     f2, 0x98(sp)                                ; Store ft2
    fsw     f3, 0x9C(sp)                                ; Store ft3
    fsw     f4, 0xA0(sp)                                ; Store ft4
    fsw     f5, 0xA4(sp)                                ; Store ft5
    fsw     f6, 0xA8(sp)                                ; Store ft6
    fsw     f7, 0xAC(sp)                                ; Store ft7
    fsw     f10,0xB8(sp)                                ; Store fa0
    fsw     f11,0xBC(sp)                                ; Store fa1
    fsw     f12,0xC0(sp)                                ; Store fa2
    fsw     f13,0xC4(sp)                                ; Store fa3
    fsw     f14,0xC8(sp)                                ; Store fa4
    fsw     f15,0xCC(sp)                                ; Store fa5
    fsw     f16,0xD0(sp)                                ; Store fa6
    fsw     f17,0xD4(sp)                                ; Store fa7
    fsw     f28,0x100(sp)                               ; Store ft8
    fsw     f29,0x104(sp)                               ; Store ft9
    fsw     f30,0x108(sp)                               ; Store ft10
    fsw     f31,0x10C(sp)                               ; Store ft11
    csrr    t0, fcsr
    sw      t0, 0x110(sp)                               ; Store fcsr
#elif __riscv_flen == 64
    fsd     f0, 0x90(sp)                                ; Store ft0
    fsd     f1, 0x98(sp)                                ; Store ft1
    fsd     f2, 0xA0(sp)                                ; Store ft2
    fsd     f3, 0xA8(sp)                                ; Store ft3
    fsd     f4, 0xB0(sp)                                ; Store ft4
    fsd     f5, 0xB8(sp)                                ; Store ft5
    fsd     f6, 0xC0(sp)                                ; Store ft6
    fsd     f7, 0xC8(sp)                                ; Store ft7
    fsd     f10,0xE0(sp)                                ; Store fa0
    fsd     f11,0xE8(sp)                                ; Store fa1
    fsd     f12,0xF0(sp)                                ; Store fa2
    fsd     f13,0xF8(sp)                                ; Store fa3
    fsd     f14,0x100(sp)                               ; Store fa4
    fsd     f15,0x108(sp)                               ; Store fa5
    fsd     f16,0x110(sp)                               ; Store fa6
    fsd     f17,0x118(sp)                               ; Store fa7
    fsd     f28,0x170(sp)                               ; Store ft8
    fsd     f29,0x178(sp)                               ; Store ft9
    fsd     f30,0x180(sp)                               ; Store ft10
    fsd     f31,0x188(sp)                               ; Store ft11
    csrr    t0, fcsr
    sw      t0, 0x190(sp)                               ; Store fcsr
#endif
#endif

    /* Save the current stack pointer in the thread's control block.  */
    /* _tx_thread_current_ptr -> tx_thread_stack_ptr =  sp;  */

    /* Switch to the system stack.  */
    /* sp =  _tx_thread_system_stack_ptr;  */

    lw      t1, _tx_thread_current_ptr                  ; Pickup current thread pointer
    sw      sp, 8(t1)                                   ; Save stack pointer*/

#if defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE)
    /* _tx_execution_isr_enter is called with thread stack pointer */
    mv      s0, ra
    call    _tx_execution_isr_enter                     ; Call the ISR execution enter function
    mv      ra, s0
#endif


    lw      sp, _tx_thread_system_stack_ptr             ; Switch to system stack
    ret                                                 ; Return to calling ISR

    /* }
    else
    {  */

_tx_thread_idle_system_save:


#if defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE)
    mv      s0, ra
    call    _tx_execution_isr_enter                     ; Call the ISR execution enter function
    mv      ra, s0
#endif

    /* Interrupt occurred in the scheduling loop.  */

    /* }
}  */
#ifdef __riscv_flen
#if __riscv_flen == 32
    addi    sp, sp, 288                                 ; Recover stack frame - with single floating point enabled
#elif __riscv_flen == 64
    addi    sp, sp, 416                                 ; Recover stack frame - with double floating point enabled
#endif
#else
    addi    sp, sp, 160                                 ; Recover the reserved stack space
#endif
    ret                                                 ; Return to calling ISR

    END
