# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

set(CONFIG_TINYUSB 1)
set(CONFIG_TINYUSB_HOST 1)
set(CONFIG_FATFS 1)
set(CONFIG_USB_FATFS 1)
set(CONFIG_USB_FATFS_TINYUSB 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-DUSB_FATFS_ENABLE)
# sdk_compile_options("-Os")

project(tinyusb_host_msc_file_explorer_example)

sdk_inc(src)

sdk_app_src(src/main.c)
sdk_app_src(src/msc_app.c)

generate_ide_projects()
