# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_MICROROS 1)
set(CONFIG_MICROROS_FOXY 1)
set(CONFIG_FREERTOS_HEAP custom)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(publisher)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_inc(inc)
sdk_inc(../common/inc)
sdk_app_src(src/app.c)
sdk_app_src(../common/src/main.c)
