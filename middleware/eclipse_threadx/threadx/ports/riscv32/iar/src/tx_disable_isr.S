/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

CPU_MSTATUS_MIE  DEFINE    0x08

    PUBLIC __disable_interrupt
    SECTION `.isr_vector`:CODE:REORDER:NOROOT(2)
    CODE
__disable_interrupt:
; DisablePUBLIC interupt
    li     t0, CPU_MSTATUS_MIE
    csrrc  zero, mstatus, t0
    ret
    

    PUBLIC __get_interrupt_state
    SECTION `.isr_vector`:CODE:REORDER:NOROOT(2)
    CODE
__get_interrupt_state:
; Save the Machine status register
    csrr   a0, mstatus
    ret
    

    PUBLIC __set_interrupt_state
    SECTION `.isr_vector`:CODE:REORDER:NOROOT(2)
    CODE
__set_interrupt_state:
; restore the Machine status register previous state
    csrw   mstatus, a0
    ret
    END