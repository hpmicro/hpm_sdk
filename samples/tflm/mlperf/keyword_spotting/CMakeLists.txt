# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")
set(RV_ABI "ilp32f")
set(RV_ARCH "rv32imafc")

set(CONFIG_HPM_MATH 1)
set(CONFIG_HPM_ENABLE_DSP 1)
set(CONFIG_HPM_MATH_NN_RVP32_ONLY_SES 1)

set(CONFIG_TFLM 1)
set(STACK_SIZE 0x10000)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(mlperf_keyword_spotting)
if("${TOOLCHAIN_VARIANT}" STREQUAL "zcc" AND "${CMAKE_BUILD_TYPE}" STREQUAL "release")
    sdk_compile_options("-flate-loop-unroll")
endif()
sdk_app_src(src/main.cc)
sdk_app_src(src/submitter_implemented.cpp)
sdk_app_src(src/api/internally_implemented.cpp)
sdk_app_src(src/kws/kws_input_data.cc)
sdk_app_src(src/kws/kws_model_data.cc)
sdk_app_src(src/kws/kws_model_settings.cc)

sdk_app_inc(src)
sdk_app_inc(src/api)
sdk_app_inc(src/util)
sdk_app_inc(src/kws)
sdk_ld_options("-lm")
sdk_ld_options("--std=c++11")
sdk_compile_definitions(__HPMICRO__)
sdk_compile_options("-O3")
generate_ide_projects()
