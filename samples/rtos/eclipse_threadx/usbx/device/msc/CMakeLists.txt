# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_USB_DEVICE 1)

set(CONFIG_ECLIPSE_THREADX 1)
set(CONFIG_ECLIPSE_THREADX_FILEX 1)
set(CONFIG_ECLIPSE_THREADX_USBX 1)
set(CONFIG_ECLIPSE_THREADX_USBX_DEVICE 1)
set(CONFIG_ECLIPSE_THREADX_USBX_CLASS_STORAGE 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(threadx_usbx_msc)

sdk_compile_definitions(-DUX_DEVICE_SIDE_ONLY=1)

sdk_inc(src)
sdk_app_src(src/demo_msc.c)
generate_ide_projects()
