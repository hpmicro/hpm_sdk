# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_MSC 1)

set(CONFIG_SDMMC 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(msc_sdcard)

sdk_compile_definitions(-DCONFIG_USBDEV_MSC_MAX_BUFSIZE=65536)

sdk_inc(../../../config)
sdk_app_src(src/main.c)
sdk_app_src(src/msc_sdcard.c)

generate_ide_projects()
