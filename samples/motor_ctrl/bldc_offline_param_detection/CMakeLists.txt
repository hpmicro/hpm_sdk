# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(CONFIG_MOTORCTRL_V2 1)
set(CONFIG_HPM_ADC 1)

set(RV_ABI "ilp32f")
set(RV_ARCH "rv32imafc")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(bldc_offline_param_detection)
sdk_compile_definitions(-DCONFIG_MCL_HAS_EXTRA_CONFIG="mcl_app_config.h")
sdk_inc(src)
sdk_app_src(src/bldc_offline_param_detection.c)
generate_ide_projects()
