# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_gcc_inc(GCC/RISC-V)
sdk_ses_inc(GCC/RISC-V)
sdk_iar_inc(IAR/RISC-V)

sdk_gcc_src(
    GCC/RISC-V/port.c
    GCC/RISC-V/chip_specific_extensions/HPMicro/hpm_port.c
    GCC/RISC-V/portASM.S
    )
sdk_ses_src(
    GCC/RISC-V/port.c
    GCC/RISC-V/chip_specific_extensions/HPMicro/hpm_port.c
    GCC/RISC-V/portASM.S
    )
sdk_iar_src(
    IAR/RISC-V/port.c
    IAR/RISC-V/chip_specific_extensions/HPMicro/hpm_port.c
    IAR/RISC-V/portASM.S
    IAR/RISC-V/default_isr.S
    )

# Use PTMR to generate system tic if tickless stop mode is set
if(DEFINED CONFIG_FREERTOS_TICKLESS_USE_STOP_MODE)
    set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)
endif()
# If CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR and DportTIMER_SOURCE_PWM is not defined, the mtimer is used to generate systic
# Otherwise use the gptimer to generate systic
if(DEFINED CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR)
    sdk_compile_definitions("-DportasmHAS_MTIME=0")
    sdk_compile_definitions("-DportTIMER_SOURCE_GPTMR=1")
elseif(DEFINED CONFIG_FREERTOS_TIMER_RESOURCE_PWM)
    sdk_compile_definitions("-DportasmHAS_MTIME=0")
    sdk_compile_definitions("-DportTIMER_SOURCE_PWM=1")
else()
    sdk_compile_definitions("-DportasmHAS_MTIME=1")
endif()

if(DEFINED CONFIG_FREERTOS_TICKLESS)
    sdk_compile_definitions(-DconfigUSE_TICKLESS_IDLE=1)
endif()
add_subdirectory_ifdef(CONFIG_FREERTOS_TICKLESS tickless)   

if(DEFINED CONFIG_FREERTOS_HEAP)
    if("${CONFIG_FREERTOS_HEAP}" STREQUAL "")
        sdk_src(MemMang/heap_4.c)
    elseif("${CONFIG_FREERTOS_HEAP}" STREQUAL "heap_4")
        sdk_src(MemMang/heap_4.c)
    elseif("${CONFIG_FREERTOS_HEAP}" STREQUAL "custom")
        message(STATUS "FreeRTOS use custom heap allocation")
    elseif("${CONFIG_FREERTOS_HEAP}" STREQUAL "heap_1")
        sdk_src(MemMang/heap_1.c)
    elseif("${CONFIG_FREERTOS_HEAP}" STREQUAL "heap_2")
        sdk_src(MemMang/heap_2.c)
    elseif("${CONFIG_FREERTOS_HEAP}" STREQUAL "heap_3")
        sdk_src(MemMang/heap_3.c)
    elseif("${CONFIG_FREERTOS_HEAP}" STREQUAL "heap_5")
        sdk_src(MemMang/heap_5.c)
    endif()
else()
    sdk_src(MemMang/heap_4.c)
endif()
