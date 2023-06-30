# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CUSTOM_GCC_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/linker.ld)
set(CUSTOM_GCC_STARTUP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/func_table.S)

set(CMAKE_BUILD_TYPE RELEASE)
set(HPM_SDK_LD_NO_NANO_SPECS 1)
set(HPM_SDK_LD_NO_WHOLE_SDK 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(openocd_flash_algo_example)

sdk_app_src(src/openocd_flash_algo.c)
sdk_compile_options(-fpic)

