# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(GCC/RISC-V)
sdk_src(
    GCC/RISC-V/port.c
    GCC/RISC-V/chip_specific_extensions/HPMicro/hpm_port.c
    GCC/RISC-V/portASM.S
    )
sdk_src(MemMang/heap_4.c)
