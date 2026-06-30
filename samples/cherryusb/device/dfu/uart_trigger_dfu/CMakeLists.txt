# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)


if("${HPM_BUILD_TYPE}" STREQUAL "")
    set(HPM_BUILD_TYPE flash_dfu)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(dfu_uart_trigger)

sdk_inc(../common)
sdk_src(../common/hpm_dfu_trigger.c)
sdk_app_src(src/main.c)
generate_ide_projects()
