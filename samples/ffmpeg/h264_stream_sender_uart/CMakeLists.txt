# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

set(HPM_SDK_NO_NANO_SPECS 1)

cmake_minimum_required(VERSION 3.13)

set(HEAP_SIZE 0x700000)
set(CONFIG_SDMMC 1)
set(CONFIG_FATFS 1)
set(INIT_EXT_RAM_FOR_DATA 1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE debug)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)

project(h264_stream_sender_uart)

sdk_inc(../common/inc)
sdk_inc(./src)

sdk_app_src(../common/src/sd_fatfs_common.c)
sdk_app_src(src/main.c)

