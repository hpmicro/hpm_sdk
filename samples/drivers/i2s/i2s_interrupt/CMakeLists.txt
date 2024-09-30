# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

if(NOT DEFINED CONFIG_CODEC)
# set(CONFIG_CODEC "sgtl5000")
set(CONFIG_CODEC "wm8960")
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(i2s_interrupt)

sdk_inc(src)
sdk_app_src(src/i2s_interrupt.c)
generate_ide_projects()
