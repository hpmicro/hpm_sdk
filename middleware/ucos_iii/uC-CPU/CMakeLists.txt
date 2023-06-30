# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_compile_definitions(-DCONFIG_DISABLE_GLOBAL_IRQ_ON_STARTUP=1)

sdk_inc(.)
sdk_inc(./RISC-V/GCC)

sdk_src(RISC-V/GCC/cpu_a.S)
sdk_src(cpu_core.c)