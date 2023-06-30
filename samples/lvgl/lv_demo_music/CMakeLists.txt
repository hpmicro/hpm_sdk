# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_LVGL 1)
if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt911")
endif()
set(CONFIG_HPM_TOUCH 1)
set(CONFIG_LV_INDEV 1)
set(CONFIG_LV_DEMO music)
set(STACK_SIZE 0x10000)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(lv_demo_music)
sdk_compile_definitions(-DCONFIG_LV_TOUCH=1)
sdk_compile_definitions(-DLV_USE_DEMO_MUSIC=1)
sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)
sdk_compile_definitions(-DCONFIG_LV_HAS_EXTRA_CONFIG="lv_app_conf.h")
sdk_inc(src)
sdk_app_src(src/lvgl.c)

generate_ses_project()
