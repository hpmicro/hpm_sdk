# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(hpm/hpm6000)
sdk_inc(GCC/RISC-V)
sdk_src(
    GCC/RISC-V/port.c
    GCC/RISC-V/portASM.S
    )
sdk_src(MemMang/heap_4.c)
