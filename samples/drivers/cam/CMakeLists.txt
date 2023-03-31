# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED CONFIG_CAMERA)
set(CONFIG_CAMERA "ov7725")
endif()
set(CONFIG_HPM_CAMERA 1)

if(NOT "${CONFIG_CAMERA}" STREQUAL "ov7725")
    string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
    string(FIND ${build_type} "release" found)
    string(FIND ${build_type} "flash_sdram_xip" warn_message)

    if (${warn_message})
        message(WARNING "<${CONFIG_CAMERA}> need more ram on chip, so switch to <flash_sdram_xip> linker!")
    endif()

    if (${found} GREATER_EQUAL 0)
        SET(CMAKE_BUILD_TYPE flash_sdram_xip_release)
    else()
        SET(CMAKE_BUILD_TYPE flash_sdram_xip)
    endif()
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cam_example)

sdk_app_src(src/cam.c)
generate_ses_project()
