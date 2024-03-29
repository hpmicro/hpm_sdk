# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")
set(CONFIG_HPM_PANEL 1)
set(CONFIG_HPM_MATH 1)
set(CONFIG_HPM_MATH_NN_RVP32_ONLY_SES 1)

if(NOT DEFINED CONFIG_CAMERA)
set(CONFIG_CAMERA "ov5640")
endif()
set(CONFIG_HPM_CAMERA 1)
set(CONFIG_TFLM 1)
set(CONFIG_TFLM_MODELS_PERSON_DETECT 1)
set(STACK_SIZE 0x10000)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(tflm_detection_responder)
sdk_app_src(src/tflm.c)
sdk_app_src(src/fount24x48.c)
sdk_app_src(src/detection_responder.cc)
sdk_app_src(src/image_provider.cc)
sdk_app_src(src/main_functions.cc)
sdk_app_src(src/model_settings.cc)

sdk_app_inc(src)
sdk_ld_options("-lm")
sdk_ld_options("--std=c++11")
sdk_compile_definitions(__HPMICRO__)
sdk_ses_compile_options("-mabi=ilp32d")
sdk_ses_compile_options("-march=rv32gc")
# sdk_compile_options("-mabi=ilp32d")
# sdk_compile_options("-march=rv32gc")
sdk_compile_options("-O3")
generate_ide_projects()
