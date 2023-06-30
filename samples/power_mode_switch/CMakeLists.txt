# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(power_mode_switch)

sdk_compile_definitions(-DBOARD_CONSOLE_BASE=HPM_PUART)
sdk_compile_definitions(-DBOARD_CONSOLE_CLK_NAME=clock_puart)
sdk_app_src(src/power_mode_switch.c)

if("${HPM_SOC}" STREQUAL "HPM6750")
    sdk_app_src(src/hpm67xx/low_power.c)
elseif("${HPM_SOC}" STREQUAL "HPM6360")
    sdk_app_src(src/hpm63xx/low_power.c)
elseif("${HPM_SOC}" STREQUAL "HPM6280")
    sdk_app_src(src/hpm62xx/low_power.c)
elseif("${HPM_SOC}" STREQUAL "HPM5300")
    sdk_app_src(src/hpm53xx/low_power.c)
endif()
generate_ses_project()
