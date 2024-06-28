# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PANEL 1)

# ENABLE LVGL
set(CONFIG_LVGL 1)
set(CONFIG_LV_DEMO 1)

if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt9xx")
endif()
set(CONFIG_HPM_TOUCH 1)
set(STACK_SIZE 0x10000)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(lvgl_baremetal)

# LVGL DEMOS
sdk_compile_definitions(-DLV_USE_DEMO_MUSIC=1)
sdk_compile_definitions(-DLV_USE_DEMO_WIDGETS=1)
sdk_compile_definitions(-DLV_USE_DEMO_BENCHMARK=1)
sdk_compile_definitions(-DLV_USE_DEMO_RENDER=0)
sdk_compile_definitions(-DLV_USE_DEMO_STRESS=1)
sdk_compile_definitions(-DLV_USE_DEMO_FLEX_LAYOUT=1)

# LVGL CONF
sdk_compile_definitions(-DLV_USE_SYSMON=1)
sdk_compile_definitions(-DLV_USE_HPM_MODE_DIRECT=0)
sdk_compile_definitions(-DLV_USE_HPM_PDMA_FLUSH=0)
sdk_compile_definitions(-DLV_USE_HPM_PDMA_WAIT_VSYNC=0)

sdk_inc(src)
sdk_app_src(src/main.c)
sdk_app_src(../common/lvgl.c)
sdk_app_src(../common/hpmlogo.c)

generate_ide_projects()
