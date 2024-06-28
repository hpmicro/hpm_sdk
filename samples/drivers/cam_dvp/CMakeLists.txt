# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED CONFIG_CAMERA)
set(CONFIG_CAMERA "ov7725")
endif()
set(CONFIG_HPM_CAMERA 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

set(CONFIG_HPM_PANEL 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cam_dvp_example)

sdk_app_src(src/cam_dvp.c)
generate_ide_projects()
