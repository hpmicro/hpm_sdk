# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_RTTHREAD_NANO 1)
set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_HOST_HID 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherryusb_hid_host)

sdk_inc(../../config)
sdk_app_src(src/main.c)
sdk_app_src(src/hid_host.c)
generate_ide_projects()
