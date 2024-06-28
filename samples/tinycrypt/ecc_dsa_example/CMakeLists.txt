# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

set(CONFIG_TINYCRYPT 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-DENABLE_TESTS)

project(ecc_dsa_example)

sdk_app_src(src/ecc_dsa_example.c)
sdk_app_src(src/ecc_utils.c)
generate_ide_projects()
