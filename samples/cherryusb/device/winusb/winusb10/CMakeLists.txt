# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_CHERRYUSB_DEVICE 1)
set(CONFIG_USB_DEVICE 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherryusb_device_winusb10)

sdk_compile_definitions(-DDOUBLE_WINUSB=1)

sdk_inc(../../../config)
sdk_app_inc(src)
sdk_app_src(src/main.c)
sdk_app_src(src/winusb10.c)
generate_ide_projects()
