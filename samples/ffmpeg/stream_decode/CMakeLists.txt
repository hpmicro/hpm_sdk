# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

set(HPM_SDK_NO_NANO_SPECS 1)
# set(SES_TOOLCHAIN_VARIANT "Andes")

cmake_minimum_required(VERSION 3.13)
string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "release")
    message("** Project set custom linker file and startup file **")
    set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linker/ram.ld)
endif()




string(FIND ${build_type} "sdram_xip" found)
if (${found} GREATER_EQUAL 0)
    message("** Project set custom linker file and startup file **")
    set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linker/sdram_xip.ld)
    set(LINK_TO_FLASH 1)
    set(INCLUDE_BOOTHEADER 1)
else()

endif()

set(HEAP_SIZE 0x4000)
set(CONFIG_FFMPEG 1)
set(CONFIG_SDMMC 1)
set(CONFIG_FATFS 1)
set(INIT_EXT_RAM_FOR_DATA 1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE debug)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

if ("${CMAKE_BUILD_TYPE}" STREQUAL "sdram_xip")
    sdk_compile_definitions("-DINIT_EXT_RAM_FOR_DATA=1")
endif()

sdk_ld_options("-lm")
#sdk_compile_options("-O2")

string(FIND ${build_type} "release" found)
if (${found} GREATER_EQUAL 0)
    message("****  O2  ****")
    sdk_compile_options("-Ofast")
else()
    message("****  -g  ****")
    sdk_compile_options("-g")
endif()

sdk_nds_compile_options(-mcpu=d45)
sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)

project(ffmpeg_h264_stream_decode)

sdk_inc(../common/inc)
sdk_inc(./src)

sdk_app_src(../common/src/sd_fatfs_common.c)
sdk_app_src(src/main.c)
sdk_app_src(src/disp.c)
sdk_app_src(src/convert.c)
sdk_app_src(src/osd.c)
sdk_app_src(src/fount24x48.c)
sdk_app_src(src/c_startup.c)
sdk_app_src(src/picture.c)
sdk_app_src(src/h264_stream.c)
sdk_app_src(src/uart.c)

generate_ses_project()
