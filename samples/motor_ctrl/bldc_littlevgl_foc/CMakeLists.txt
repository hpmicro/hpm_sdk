# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)

set(CONFIG_MOTORCTRL 1)
# set(CONFIG_HPM_MATH 1)
set(CONFIG_LVGL 1)
if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt911")
endif()
set(CONFIG_HPM_TOUCH 1)
set(CONFIG_LV_INDEV 1)
set(CONFIG_HPM_ADC 1)
# set(HPM_MATH_DSP_SES_LIB "libdspf")
set(STACK_SIZE 0x10000)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(bldc_littlevgl_foc)

sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)
sdk_compile_definitions(-DCONFIG_LV_HAS_EXTRA_CONFIG="lv_app_conf.h")

sdk_inc(src)

sdk_app_src(src/bldc_foc_callback.c)
sdk_app_src(src/bldc_littlevgl_foc.c)
sdk_app_src(src/bldc_littlevgl_disp.c)
sdk_app_src(src/hp_micro_logo_alpha_181x100.c)
sdk_ld_options("-lm")
sdk_ses_compile_options(-mabi=ilp32f)
sdk_ses_compile_options(-march=rv32imafc)
# sdk_compile_options("-mabi=ilp32d")
generate_ses_project()
