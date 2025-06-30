# Copyright (c) 2024-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_VIDEO 1)

set(CONFIG_CAMERA "ov7725")
set(CONFIG_HPM_CAMERA 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(cherryusb_device_video_yuyv)

sdk_inc(../../../config)
sdk_inc(./src)
sdk_app_src(src/main.c)
sdk_app_src(src/video_yuyv.c)
generate_ide_projects()
