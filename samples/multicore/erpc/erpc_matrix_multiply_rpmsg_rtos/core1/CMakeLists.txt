# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(HPM_BUILD_TYPE "sec_core_img")
set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../core0/src/sec_core_img.c)
set(CONFIG_FREERTOS 1)
set(CONFIG_ERPC 1)
set(CONFIG_ERPC_SERVER 1)
set(CONFIG_ERPC_RPMSG_REMOTE 1)
set(CONFIG_IPC_EVENT_MGR 1)
set(CONFIG_IPC_EVENT_MGR_MBX 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(erpc_rpmsg_rtos_core1)

sdk_compile_definitions(-DHPM_FEATURE_MBX_SIDE_B)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_options(-Os)
sdk_compile_options(-fno-exceptions)

sdk_inc(src)
sdk_app_src(src/main_core1.c)

sdk_inc(../config)
sdk_inc(../generated)
sdk_src(../generated/erpc_matrix_multiply_server.cpp)

sdk_ld_options("--std=c++11")
generate_ide_projects()


