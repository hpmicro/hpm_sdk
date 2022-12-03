# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CMAKE_BUILD_TYPE sec_core_img)
set(IS_SEC_CORE_IMG true)
set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../linker/gcc/core1.ld)
set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../core0/src/sec_core_img.c)
set(CONFIG_ERPC 1)
set(CONFIG_ERPC_SERVER 1)
set(CONFIG_IPC_EVENT_MGR 1)
set(CONFIG_IPC_EVENT_MGR_MBX 1)

set(HEAP_SIZE 0x4000)
set(CONFIG_FFMPEG 1)
set(CONFIG_SDMMC 1)
set(CONFIG_FATFS 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(multcore_erpc_rpmsg_core1)

set(CMAKE_CXX_STANDARD 11)

sdk_compile_definitions(-DHPM_FEATURE_MBX_SIDE_B)
sdk_compile_definitions(-DBOARD_RUNNING_CORE=1)
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)

sdk_inc(src)
sdk_app_src(src/main_core1.c)

sdk_inc(../generated)
sdk_src(../generated/erpc_matrix_multiply_server.cpp)

sdk_inc(../common/inc)
sdk_inc(./src)

sdk_app_src(../common/src/sd_fatfs_common.c)
sdk_app_src(src/c_startup.c)
sdk_app_src(src/h264_stream.c)
sdk_app_src(src/erpc.c)


sdk_compile_options("-Os")
sdk_ld_options("--std=c++11")
generate_ses_project()


