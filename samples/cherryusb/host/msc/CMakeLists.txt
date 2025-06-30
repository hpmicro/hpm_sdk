# Copyright (c) 2022-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_FATFS 1)
set(CONFIG_USB_FATFS 1)
set(CONFIG_USB_FATFS_CHERRYUSB 1)
set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_HOST_MSC 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherryusb_msc_host)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_definitions(-DUSB_FATFS_ENABLE=1)
# sdk_compile_definitions(-DFATFS_ONLY_NONCACHEABLE_BUF=1)
sdk_inc(../../config)
sdk_app_src(src/main.c)
sdk_app_src(src/msc_host.c)
generate_ide_projects()
