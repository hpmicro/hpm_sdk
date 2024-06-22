# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")

set(CONFIG_HPM_MATH 1)
set(CONFIG_HPM_MATH_NN_RVP32_ONLY_SES 1)
set(CONFIG_TFLM 1)
set(STACK_SIZE 0x10000)

set(RV_ABI "ilp32d")
set(RV_ARCH "rv32gc")
set(CONFIG_HPM_MATH_NN_RVP32 1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(tflm-keyword)

sdk_app_src(src/keyword_benchmark.cc)
sdk_app_src(src/keyword_scrambled_model_data.cc)

sdk_app_inc(src)

sdk_ld_options("-lm")
sdk_ld_options("--std=c++11")
sdk_compile_definitions(__HPMICRO__)
sdk_ses_compile_options("-mabi=ilp32d")
sdk_ses_compile_options("-march=rv32gcv")
#sdk_ses_compile_options("-march=rv32gcxandes")
sdk_compile_options("-O3")
generate_ses_project()
