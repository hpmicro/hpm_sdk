# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_TINYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_CDC 1)
set(CONFIG_USB_DEVICE_MSC 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(tinyusb_cdc_msc_example)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_inc(src)

sdk_app_src(src/main.c)
sdk_app_src(src/msc_disk.c)
sdk_app_src(src/usb_descriptors.c)

generate_ide_projects()
