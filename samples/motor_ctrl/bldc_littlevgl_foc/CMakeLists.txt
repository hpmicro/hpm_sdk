# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PANEL 1)

set(CONFIG_MOTORCTRL 1)
# set(CONFIG_HPM_MATH 1)
if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt9xx")
endif()
set(CONFIG_HPM_TOUCH 1)
set(CONFIG_HPM_ADC 1)
# set(HPM_MATH_DSP_SES_LIB "libdspf")
set(STACK_SIZE 0x10000)
set(RV_ABI "ilp32f")
set(RV_ARCH "rv32imafc")

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(bldc_littlevgl_foc)
sdk_compile_definitions(-DCONFIG_LV_TOUCH=1)
sdk_compile_definitions(-DCONFIG_LV_HAS_EXTRA_CONFIG="lv_app_conf.h")
sdk_compile_definitions(-DHPM_LVGL_FRAMEBUFFER_NONCACHEABLE)

sdk_inc(lvgl_v835)
sdk_inc(lvgl_v835/lvgl)
sdk_inc(lvgl_v835/porting)
sdk_app_src(lvgl_v835/porting/lv_adapter.c)
sdk_app_src_glob(lvgl_v835/lvgl/src/*.c)

sdk_inc(src)

sdk_app_src(src/bldc_foc_callback.c)
sdk_app_src(src/bldc_littlevgl_foc.c)
sdk_app_src(src/bldc_littlevgl_disp.c)
sdk_app_src(src/hp_micro_logo_alpha_181x100.c)
sdk_ld_options("-lm")
generate_ide_projects()
