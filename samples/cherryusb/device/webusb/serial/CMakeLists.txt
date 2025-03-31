# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_CDC_ACM 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherryusb_device_webusb_serial)

sdk_inc(../../../config)
sdk_app_inc(src)
sdk_app_src(src/main.c)
sdk_app_src(src/webusb_serial.c)
generate_ide_projects()
