# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CUSTOM_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/linker.ld)
set(CUSTOM_STARTUP_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/func_table.S)

set(CMAKE_BUILD_TYPE RELEASE)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(openocd_flash_algo_example)

sdk_app_src(src/openocd_flash_algo.c)
sdk_compile_options(-fpic)

