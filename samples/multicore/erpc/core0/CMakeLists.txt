# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../linker/gcc/core0.ld)
set(CMAKE_BUILD_TYPE debug)
set(INCLUDE_BOOTHEADER 1)

set(CONFIG_ERPC 1)
set(CONFIG_ERPC_CLIENT 1)
set(CONFIG_IPC_EVENT_MGR 1)
set(CONFIG_IPC_EVENT_MGR_MBX 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(multcore_erpc_rpmsg_core0)

set(CMAKE_CXX_STANDARD 11)

sdk_compile_definitions("-DFLASH_XIP=1")
sdk_compile_definitions(-DHPM_FEATURE_MBX_SIDE_A)

sdk_inc(src)

sdk_app_src(src/main_core0.c)
sdk_app_src(src/sec_core_img.c)

sdk_inc(../generated)
sdk_src(../generated/erpc_matrix_multiply_client.cpp)

sdk_ld_options("--std=c++11")
generate_ses_project()
