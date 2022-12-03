# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(HEAP_SIZE 0x900000)
set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../linker/gcc/core0.ld)
set(CMAKE_BUILD_TYPE debug)
set(INCLUDE_BOOTHEADER 1)

set(CONFIG_ERPC 1)
set(CONFIG_ERPC_CLIENT 1)
set(CONFIG_IPC_EVENT_MGR 1)
set(CONFIG_IPC_EVENT_MGR_MBX 1)
set(CONFIG_SDMMC 1)
set(CONFIG_FATFS 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(INIT_EXT_RAM_FOR_DATA=1)

project(multcore_erpc_rpmsg_core0)

set(CMAKE_CXX_STANDARD 11)

sdk_compile_definitions("-DFLASH_XIP=1")
sdk_compile_definitions(-DHPM_FEATURE_MBX_SIDE_A)
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)

sdk_inc(src)
sdk_inc(../core1/src)
sdk_inc(../common/inc)

sdk_app_src(src/main_core0.c)
sdk_app_src(src/sec_core_img.c)
sdk_app_src(../common/src/sd_fatfs_common.c)
sdk_app_src(src/disp.c)
sdk_app_src(src/convert.c)
sdk_app_src(src/measure.c)
sdk_app_src(src/osd.c)
sdk_app_src(src/fount24x48.c)
sdk_app_src(src/picture.c)
sdk_app_src(src/uart.c)

sdk_inc(../generated)
sdk_src(../generated/erpc_matrix_multiply_client.cpp)

sdk_ld_options("--std=c++11")
# generate_ses_project()
