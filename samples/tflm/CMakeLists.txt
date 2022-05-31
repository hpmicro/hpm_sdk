# Copyright 2022 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED CONFIG_CAMERA)
set(CONFIG_CAMERA "ov5640")
endif()
set(CONFIG_HPM_CAMERA 1)
set(CONFIG_TFLM 1)
set(CUSTOM_LINKER_FILE $ENV{HPM_SDK_BASE}/samples/littlevgl/linkers/gcc/lvgl.ld)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(tensorflow_litemicro_example)
set(CMAKE_CXX_STANDARD 11)
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
sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)
# sdk_compile_options("-mabi=ilp32f")
# sdk_compile_options("-march=rv32imafc")
sdk_compile_options("-O3")
set(SEGGER_LEVEL_O3 1)
generate_ses_project()
