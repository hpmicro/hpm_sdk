# Copyright 2022 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")

set(CONFIG_HPM_MATH 1)
set(CONFIG_HPM_MATH_NN_RVP32_ONLY_SES 1)
set(CONFIG_HPM_MATH_NN_NORMAL 1)

set(CONFIG_TFLM 1)
set(STACK_SIZE 0x10000)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(mlperf_person_dection)
set(CMAKE_CXX_STANDARD 11)
sdk_app_src(src/main.cc)
sdk_app_src(src/submitter_implemented.cpp)
sdk_app_src(src/api/internally_implemented.cpp)
sdk_app_src(src/vww/vww_inputs.cc)
sdk_app_src(src/vww/vww_model_data.cc)
sdk_app_src(src/vww/vww_model_settings.cc)

sdk_app_inc(src)
sdk_app_inc(src/api)
sdk_app_inc(src/util)
sdk_app_inc(src/vww)
sdk_ld_options("-lm")
sdk_ld_options("--std=c++11")
sdk_compile_definitions(__HPMICRO__)
sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)
sdk_ses_compile_options("-mabi=ilp32d")
sdk_ses_compile_options("-march=rv32gc")
# sdk_compile_options("-mabi=ilp32d")
# sdk_compile_options("-march=rv32gc")
sdk_compile_options("-O3")
set(SEGGER_LEVEL_O3 1)
generate_ses_project()
