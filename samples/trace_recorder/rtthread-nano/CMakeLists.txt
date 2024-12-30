# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_RTTHREAD_NANO 1)
set(CONFIG_TRACE_RECORDER 1)
set(CONFIG_TRACE_RECORDER_STREAM_MODE "JLink RTT")

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(rtt-trace_recorder)

sdk_inc(src)
sdk_inc(common)
sdk_app_src(src/demo.c)
generate_ide_projects()
