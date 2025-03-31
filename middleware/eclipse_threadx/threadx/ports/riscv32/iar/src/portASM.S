/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#if __riscv_xlen == 64
    #define portWORD_SIZE 8
    #define store_x sd
    #define load_x ld
#elif __riscv_xlen == 32
    #define store_x sw
    #define load_x lw
    #define portWORD_SIZE 4
#else
    #error Assembler did not define __riscv_xlen
#endif

#define portCONTEXT_SIZE ( 32 * portWORD_SIZE )

    EXTERN __threadx_irq_stack_top
    EXTERN _tx_thread_execute_ptr
    EXTERN _tx_thread_current_ptr
    EXTERN _tx_thread_context_save
    EXTERN _tx_thread_context_restore
    EXTERN ulPortTrapHandler
    EXTERN _tx_timer_interrupt
    EXTERN p_mchtmr_cmp_reg
    EXTERN p_os_next_tic
    EXTERN timer_increment_per_tick /* size_t type so 32-bit on 32-bit core and 64-bits on 64-bit core. */
    EXTERN portasmHANDLE_INTERRUPT
    EXTERN irq_handler_trap

/*-----------------------------------------------------------*/

    PUBLIC tx_risc_v_trap_handler
    SECTION `.isr_vector`:CODE(2)
tx_risc_v_trap_handler:
#ifdef __riscv_flen
#if __riscv_flen == 32
    addi    sp, sp, -288                            ; Allocate space for all registers - with floating point enabled
#elif __riscv_flen == 64
    addi    sp, sp, -416                            ; Allocate space for all registers - with floating point enabled
#else
#error "__riscv_flen is wrong! Please check it!"
#endif
#else
    addi    sp, sp, -160                            ; Allocate space for all registers - without floating point enabled
#endif
    sw      x1, 0x70(sp)                            ; Store RA
    call    _tx_thread_context_save                 ; Call ThreadX context save

    csrr    a0, mcause

test_if_asynchronous:
    srli    a2, a0, __riscv_xlen - 1       /* MSB of mcause is 1 if handing an asynchronous interrupt - shift to LSB to clear other bits. */
    beq     a2, x0, handle_synchronous      /* Branch past interrupt handing if not asynchronous. */

handle_asynchronous:
#if defined(portasmHAS_MTIME) && (portasmHAS_MTIME != 0)
test_if_mtimer:                         /* If there is a CLINT then the mtimer is used to generate the tick interrupt. */
        addi t0, x0, 1
        slli t0, t0, __riscv_xlen - 1   /* LSB is already set, shift into MSB.  Shift 31 on 32-bit or 63 on 64-bit cores. */
        addi t1, t0, 7                  /* 0x8000[]0007 == machine timer interrupt. */
        bne  a0, t1, other_interrupts

        load_x t0, p_mchtmr_cmp_reg     /* Load address of compare register into t0. */
        load_x t1, p_os_next_tic        /* Load the address of ullNextTime into t1. */

        #if( __riscv_xlen == 32 )

            /* Update the 64-bit mtimer compare match value in two 32-bit writes. */
            li t4, -1
            lw t2, 0(t1)                /* Load the low word of ullNextTime into t2. */
            lw t3, 4(t1)                /* Load the high word of ullNextTime into t3. */
            sw t4, 0(t0)                /* Low word no smaller than old value to start with - will be overwritten below. */
            sw t3, 4(t0)                /* Store high word of ullNextTime into compare register.  No smaller than new value. */
            sw t2, 0(t0)                /* Store low word of ullNextTime into compare register. */
            lw t0, timer_increment_per_tick    /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to p_os_next_tic?). */
            add t4, t0, t2              /* Add the low word of ullNextTime to the timer increments for one tick (assumes timer increment for one tick fits in 32-bits). */
            sltu t5, t4, t2             /* See if the sum of low words overflowed (what about the zero case?). */
            add t6, t3, t5              /* Add overflow to high word of ullNextTime. */
            sw t4, 0(t1)                /* Store new low word of ullNextTime. */
            sw t6, 4(t1)                /* Store new high word of ullNextTime. */

        #endif /* __riscv_xlen == 32 */

        #if( __riscv_xlen == 64 )

            /* Update the 64-bit mtimer compare match value. */
            ld t2, 0(t1)                /* Load ullNextTime into t2. */
            sd t2, 0(t0)                /* Store ullNextTime into compare register. */
            ld t0, timer_increment_per_tick  /* Load the value of ullTimerIncrementForOneTick into t0 (could this be optimized by storing in an array next to p_os_next_tic?). */
            add t4, t0, t2              /* Add ullNextTime to the timer increments for one tick. */
            sd t4, 0(t1)                /* Store ullNextTime. */

        #endif /* __riscv_xlen == 64 */

        call _tx_timer_interrupt
        j processed_source
#endif /* portasmHAS_MTIME */

other_interrupts:
    call irq_handler_trap               /* Jump to the interrupt handler if there is no CLINT or if there is a CLINT and it has been determined that anEXTERNal interrupt is pending. */
    j processed_source

handle_synchronous:
    addi a1, a1, 4                      /* Synchronous so updated exception return address to the instruction after the instruction that generated the exeption. */
    store_x a1, 0( sp )                 /* Save updated exception return address. */

test_if_environment_call:
    li t0, 11                           /* 11 == environment call. */
    bne a0, t0, is_exception            /* Not an M environment call, so some other exception. */

    /* Disable task scheduler, if it's already in ISR */
    addi t1, t0, -1
    bgtz t1, processed_source

    j processed_source

is_exception:
    csrr t0, mcause                      /* For viewing in the debugger only. */
    csrr t1, mepc                        /* For viewing in the debugger only */
    csrr t2, mstatus
    j is_exception                       /* No other exceptions handled yet. */

processed_source:
    j _tx_thread_context_restore     ;Jump to ThreadX context restore function. Note: this does not return!
    
