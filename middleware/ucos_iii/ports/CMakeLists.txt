# Copyright (c) 2023-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_gcc_inc(riscv32/gcc)
sdk_ses_inc(riscv32/gcc)
sdk_iar_inc(riscv32/iar)

sdk_ses_src(riscv32/gcc/port.c)
sdk_gcc_src(riscv32/gcc/port.c)
sdk_iar_src(riscv32/iar/port.c)
sdk_ses_src(riscv32/gcc/portASM.S)
sdk_gcc_src(riscv32/gcc/portASM.S)
sdk_iar_src(riscv32/iar/portASM.s)