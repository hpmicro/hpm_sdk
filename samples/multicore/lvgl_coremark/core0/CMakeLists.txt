# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(SES_TOOLCHAIN_VARIANT "Andes")

set(RV_ABI "ilp32d")
set(RV_ARCH "rv32gc")

set(CONFIG_HPM_PANEL 1)

set(CONFIG_LVGL 1)
if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt9xx")
endif()
set(CONFIG_HPM_TOUCH 1)
set(CONFIG_LV_DEMO widgets)
set(CONFIG_COREMARK 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(lv_demo_dualcore_coremark)

sdk_compile_definitions(-DCONFIG_LV_TOUCH=1)
sdk_compile_definitions(-DLV_USE_DEMO_WIDGETS=1)
sdk_compile_definitions(-DCONFIG_LV_HAS_EXTRA_CONFIG="lv_app_conf.h")
sdk_compile_definitions(-DMAIN_ENTRY=app_main)

sdk_inc(src/port)
sdk_inc(src)
sdk_app_src(src/demo_dualcore_coremark.c
        src/dualcore_widgets.c
        src/generated/hpmlogo_180x120.c
        src/sec_core_img.c
        src/port/core_portme.c
        )

generate_ide_projects()
