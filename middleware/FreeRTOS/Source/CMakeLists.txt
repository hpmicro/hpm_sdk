# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(include)
sdk_gcc_inc(portable/GCC/RISC-V)
sdk_gcc_inc(portable/GCC/RISC-V/chip_specific_extensions/HPMicro)
sdk_ses_inc(portable/GCC/RISC-V)
sdk_ses_inc(portable/GCC/RISC-V/chip_specific_extensions/HPMicro)
sdk_iar_inc(portable/IAR/RISC-V)
sdk_iar_inc(portable/IAR/RISC-V/chip_specific_extensions/HPMicro)
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
