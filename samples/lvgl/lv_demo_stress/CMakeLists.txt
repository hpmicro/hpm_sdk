# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

# include lvgl
set(CONFIG_LVGL 1)
set(CONFIG_LV_DEMO stress)
set(STACK_SIZE 0x10000)
set(CONFIG_HPM_PANEL 1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()


find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(lv_demo_stress)
sdk_compile_definitions(-DLV_USE_DEMO_STRESS=1)
sdk_inc(src)
sdk_app_src(src/lvgl.c)

generate_ide_projects()
