# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_VIDEO 1)
set(CONFIG_HPM_JPEG 1)

set(CONFIG_CAMERA "ov7725")
set(CONFIG_HPM_CAMERA 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

set(HEAP_SIZE 0x200000)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(cherryusb_device_video_mjpeg)

sdk_inc(../../../config)
sdk_inc(./src)
sdk_app_src(src/main.c)
sdk_app_src(src/video_mjpeg.c)
generate_ide_projects()
