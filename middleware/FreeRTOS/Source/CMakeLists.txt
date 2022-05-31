# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(include)
sdk_inc(portable/GCC/RISC-V)
sdk_inc(portable/GCC/RISC-V/chip_specific_extensions/HPMicro)
sdk_src(
    croutine.c
    list.c
    queue.c
    stream_buffer.c
    tasks.c
    timers.c
    event_groups.c
    )
add_subdirectory(portable)
