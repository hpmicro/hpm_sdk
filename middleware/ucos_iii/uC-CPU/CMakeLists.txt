# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_definitions(-DCONFIG_DISABLE_GLOBAL_IRQ_ON_STARTUP=1)

sdk_ses_inc(.)
sdk_ses_inc(./RISC-V/GCC)
sdk_ses_src(RISC-V/GCC/cpu_a.S)

sdk_gcc_inc(.)
sdk_gcc_inc(./RISC-V/GCC)
sdk_gcc_src(RISC-V/GCC/cpu_a.S)

sdk_iar_inc(.)
sdk_iar_inc(./RISC-V/IAR)
sdk_iar_src(RISC-V/IAR/cpu_a.s)

sdk_src(cpu_core.c)