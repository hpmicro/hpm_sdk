# Copyright (c) 2022,2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

set(CONFIG_CODEC 1) # audio codec
#set(CONFIG_CODEC_NAME "wm8960") # specify audio codec name, default is auto-detected by board.yaml

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(i2s_interrupt)

sdk_inc(src)
sdk_app_src(src/i2s_interrupt.c)
generate_ide_projects()
