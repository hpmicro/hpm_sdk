# Copyright 2024 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PANEL 1)
set(CONFIG_VGLITE 1)

# ENABLE LVGL
set(CONFIG_LVGL 1)
set(CONFIG_LV_DEMO 1)

if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt9xx")
endif()
set(CONFIG_HPM_TOUCH 1)
set(CONFIG_FREERTOS 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(lvgl_vglite)
sdk_ld_options("-lm")
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

# LVGL DEMOS
sdk_compile_definitions(-DLV_USE_DEMO_MUSIC=1)
sdk_compile_definitions(-DLV_USE_DEMO_WIDGETS=1)
sdk_compile_definitions(-DLV_USE_DEMO_BENCHMARK=1)
sdk_compile_definitions(-DLV_USE_DEMO_RENDER=1)
sdk_compile_definitions(-DLV_USE_DEMO_STRESS=1)
sdk_compile_definitions(-DLV_USE_DEMO_FLEX_LAYOUT=1)

# LVGL CONF
sdk_compile_definitions(-DLV_USE_OS=LV_OS_FREERTOS)
sdk_compile_definitions(-DLV_USE_DRAW_VG_LITE=1)
sdk_compile_definitions(-DCONFIG_LV_HAS_EXTRA_CONFIG="lv_app_conf.h")

sdk_inc(src)
sdk_app_src(src/main.c)
sdk_app_src(../common/lvgl.c)
sdk_app_src(../common/hpmlogo.c)

generate_ide_projects()