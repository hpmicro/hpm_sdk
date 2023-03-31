# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")

set(CONFIG_HPM_MATH 1)
set(CONFIG_HPM_MATH_DSP 1)
set(HPM_MATH_DSP_SES_LIB "libdspf")
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(rfft)
sdk_ses_compile_options(-mabi=ilp32f)
sdk_ses_compile_options(-march=rv32imafc)

sdk_compile_definitions(-DBOARD_SHOW_CLOCK=0)
sdk_app_src(src/rfft.c)
sdk_compile_options("-O3")
set(SEGGER_LEVEL_O3 1)

generate_ses_project()
