# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)


if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_xip)
endif()


find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dgo_demo)

sdk_app_src(src/dgo_demo.c)
generate_ide_projects()
