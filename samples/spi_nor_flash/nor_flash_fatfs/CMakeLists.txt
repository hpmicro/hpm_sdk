# Copyright (c) 2026  HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(CONFIG_SPI_NOR_FLASH 1)
set(CONFIG_FATFS 1)
set(CONFIG_DMA_MGR 1)

set(USE_FREERTOS 1)

if (USE_FREERTOS)
    set(CONFIG_FREERTOS 1)
    #Set CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR to use GPTMR as system's tick source
    set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)
endif()

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(nor_flash_fatfs)

sdk_inc(../common/port)
sdk_inc(../common/port/${BOARD})
sdk_app_src(../common/port/hpm_serial_nor_host_port.c)

if (USE_FREERTOS)
    sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
    sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
    sdk_compile_definitions(-DUSE_FREERTOS=1)
    sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)
endif()

sdk_compile_definitions(-DPORT_SPI_CLK_FREQUENCY=80000000)
sdk_compile_definitions(-DPORT_SPI_IO_MODE=SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE)

sdk_inc(inc)
sdk_app_src_glob(src/*.c)
sdk_compile_options("-O3")
generate_ide_projects()
