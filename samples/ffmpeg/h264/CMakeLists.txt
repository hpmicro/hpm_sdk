# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

set(HPM_SDK_NO_NANO_SPECS 1)

cmake_minimum_required(VERSION 3.13)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "release")
    message("** Project set custom linker file and startup file **")
    set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linker/ram.ld)
endif()

set(HEAP_SIZE 0x900000)
set(CONFIG_FFMPEG 1)
set(CONFIG_SDMMC 1)
set(CONFIG_FATFS 1)
set(INIT_EXT_RAM_FOR_DATA 1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE debug)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_ld_options("-lm")
sdk_compile_options("-O2")
sdk_nds_compile_options(-mcpu=d45)
sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)

project(ffmpeg_h264_decode)

sdk_inc(../common/inc)
sdk_inc(./src)

sdk_app_src(../common/src/sd_fatfs_common.c)
sdk_app_src(src/h264_decode.c)
sdk_app_src(src/disp.c)
sdk_app_src(src/convert.c)
sdk_app_src(src/measure.c)


