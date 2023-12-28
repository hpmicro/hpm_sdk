# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(include)
sdk_inc(libcpu)

sdk_src(
    libcpu/cpuport.c
    libcpu/startup.c
    libcpu/context_gcc.S
    libcpu/interrupt_gcc.S
)

sdk_compile_definitions(-DCONFIG_RTTHREAD=1)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
file(GLOB_RECURSE RTT_NANO_SRC src/*.c)
sdk_src(${RTT_NANO_SRC})

add_subdirectory(bsp)
add_subdirectory(components)
