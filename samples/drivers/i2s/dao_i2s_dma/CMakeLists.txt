# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-DTEST_AUDIO_MONO=0)

project(dao_i2s_dma)

sdk_inc(src)
sdk_app_src(src/dao_i2s_dma.c)
generate_ide_projects()
