# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_TRACE_RECORDER 1)
# set(CONFIG_TRACE_RECORDER_SNAPSHOT_MODE 1)
set(CONFIG_TRACE_RECORDER_STREAM_MODE "JLink RTT")

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(trace_recorder_freertos)

sdk_inc(src)
sdk_compile_definitions(-D__freertos_irq_stack_top=_stack)
sdk_compile_definitions(-DCONFIG_FREERTOS=1)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_app_src(src/trace_recorder_freertos.c)
generate_ide_projects()
