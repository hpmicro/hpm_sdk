# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(include)
sdk_inc(libcpu/risc-v)

sdk_gcc_src(
    libcpu/risc-v/cpuport.c
    libcpu/risc-v/gcc/context_gcc.S
    libcpu/risc-v/gcc/interrupt_gcc.S
)
sdk_ses_src(
    libcpu/risc-v/cpuport.c
    libcpu/risc-v/gcc/context_gcc.S
    libcpu/risc-v/gcc/interrupt_gcc.S
)
sdk_iar_src(
    libcpu/risc-v/cpuport.c
    libcpu/risc-v/iar/context_iar.s
    libcpu/risc-v/iar/interrupt_iar.s
)

sdk_compile_definitions(-DCONFIG_RTTHREAD=1)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_definitions(-DMAIN_ENTRY=entry)

file(GLOB_RECURSE RTT_NANO_SRC src/*.c)
sdk_src(${RTT_NANO_SRC})

add_subdirectory(bsp)
add_subdirectory(components)
