# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)
set(CONFIG_MOTORCTRL 1)
set(CONFIG_HPM_ADC 1)

# Enable segger j-scope
# set(CONFIG_SEGGER_RTT 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(bldc_smc)
sdk_app_inc(src)
sdk_app_src(src/bldc_smc_callback.c)
sdk_app_src(src/bldc_smc.c)
sdk_ld_options("-lm")
sdk_ses_compile_options(-mabi=ilp32f)
sdk_ses_compile_options(-march=rv32imafc)

# sdk enable hardware floating point function
# sdk_compile_options("-mabi=ilp32f")
# sdk_compile_options("-march=rv32imafc")
generate_ses_project()
