# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")

set(CONFIG_HPM_PANEL 1)
set(CONFIG_HPM_MATH 1)
set(CONFIG_HPM_MATH_NN_RVP32_ONLY_SES 1)
set(CONFIG_HPM_MATH_DSP 1)
set(CONFIG_TINYENGINE 1)
set(HPM_MATH_DSP_SES_LIB "libdspf")
set(RV_ABI "ilp32f")
set(RV_ARCH "rv32imafc")

if(NOT DEFINED CONFIG_CAMERA)
set(CONFIG_CAMERA "ov5640")
endif()
set(CONFIG_HPM_CAMERA 1)
set(STACK_SIZE 0x10000)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(person_obj_example)

sdk_app_inc(src)
sdk_app_inc(tinyengine/codegen/Include)

sdk_app_src(src/person_obj.c)
aux_source_directory(tinyengine/codegen/Source SRC_LIST_TINY_ENGINE)
sdk_app_src(${SRC_LIST_TINY_ENGINE})

sdk_compile_definitions(__HPMICRO__)
sdk_compile_options("-O3")
generate_ide_projects()
