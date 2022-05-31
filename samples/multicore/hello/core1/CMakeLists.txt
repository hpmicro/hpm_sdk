# Copyright 2022 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CUSTOM_LINKER_FILE $ENV{HPM_SDK_BASE}/samples/multicore/hello/linker/gcc/core1.ld)

set(SEC_CORE_IMG_C_ARRAY_OUTPUT $ENV{HPM_SDK_BASE}/samples/multicore/hello/core0/src/sec_core_img.c)

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "")
    string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
    string(FIND ${build_type} "sec_core_img" found)
    if(${found} GREATER_EQUAL 0)
        set(IS_SEC_CORE_IMG true)
    endif()
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(multcore_core1)

sdk_compile_definitions(-DBOARD_SHOW_CLOCK=0)
sdk_app_src(src/demo.c)
generate_ses_project()
