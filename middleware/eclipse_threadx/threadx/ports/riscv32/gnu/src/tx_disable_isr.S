/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

.equ  CPU_MSTATUS_MIE,         0x08

.global __disable_interrupt
.global __get_interrupt_state
.global __set_interrupt_state

.section .isr_vector, "ax"
.type __disable_interrupt, function
__disable_interrupt:
# Disable global interupt
    li     t0, CPU_MSTATUS_MIE
    csrrc  zero, mstatus, t0
    ret

.section .isr_vector, "ax"
.type __get_interrupt_state, function
__get_interrupt_state:
# Save the Machine status register
    csrr   a0, mstatus
    ret

.section .isr_vector, "ax"
.type __set_interrupt_state, function
__set_interrupt_state:
# restore the Machine status register previous state
    csrw   mstatus, a0
    ret
