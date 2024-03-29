# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(inc)

#sdk_src(src/tx_initialize_low_level.S)
sdk_src(src/tx_thread_context_restore.S)
sdk_src(src/tx_thread_context_save.S)
sdk_src(src/tx_thread_interrupt_control.S)
sdk_src(src/tx_thread_schedule.S)
sdk_src(src/tx_thread_stack_build.S)
sdk_src(src/tx_thread_system_return.S)
sdk_src(src/tx_timer_interrupt.S)
sdk_src(src/tx_disable_isr.S)
sdk_src(src/portASM.S)
