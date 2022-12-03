# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(include)
sdk_inc(portable/GCC/RISC-V)
sdk_inc(portable/GCC/RISC-V/chip_specific_extensions/HPMicro)


sdk_src(heap_5.c)
