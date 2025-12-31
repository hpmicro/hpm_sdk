# Copyright (c) 2022-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)


set(CONFIG_TINYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_CDC 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(tinyusb_device_cdc_dual_ports_example)

sdk_inc(src)

sdk_app_src(src/main.c)
sdk_app_src(src/usb_descriptors.c)

generate_ide_projects()
