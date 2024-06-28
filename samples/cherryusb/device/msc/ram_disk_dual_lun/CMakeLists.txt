# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_MSC 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherryusb_device_msc_ram_dual_lun)

sdk_compile_definitions(-DCONFIG_USBDEV_MSC_MAX_LUN=2)

sdk_inc(../../../config)
sdk_app_src(src/main.c)
sdk_app_src(src/msc_ram.c)
generate_ide_projects()
