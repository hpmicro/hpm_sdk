# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(HPM_BUILD_TYPE "sec_core_img")
set(SEC_CORE_IMG_C_ARRAY_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/../core0/src/sec_core_img.c)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(multicore_core1)

sdk_app_src(src/demo.c)
generate_ide_projects()
