# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_ERPC 1)
set(CONFIG_ERPC_CLIENT 1)
set(CONFIG_ERPC_RPMSG_MASTER 1)
set(CONFIG_IPC_EVENT_MGR 1)
set(CONFIG_IPC_EVENT_MGR_MBX 1)
set(CONFIG_FREERTOS 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(erpc_rpmsg_rtos_core0)

sdk_compile_definitions(-DHPM_FEATURE_MBX_SIDE_A)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_options(-fno-exceptions)

sdk_inc(src)
sdk_inc(../../../common)

sdk_app_src(../../../common/multicore_common.c)
sdk_app_src(src/main_core0.c)
sdk_app_src(src/sec_core_img.c)

sdk_inc(../config)
sdk_inc(../generated)
sdk_src(../generated/erpc_matrix_multiply_client.cpp)

sdk_ld_options("--std=c++11")
generate_ide_projects()
