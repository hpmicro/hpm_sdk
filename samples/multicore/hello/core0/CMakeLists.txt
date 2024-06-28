# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(multicore_core0)

sdk_app_inc(src)
sdk_app_inc(../../common)

sdk_app_src(../../common/multicore_common.c)
sdk_app_src(src/demo.c src/sec_core_img.c)
generate_ide_projects()
