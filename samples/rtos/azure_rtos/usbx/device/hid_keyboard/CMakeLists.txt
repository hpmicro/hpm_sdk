# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_USB_DEVICE 1)

set(CONFIG_AZURE_RTOS_THREADX 1)
set(CONFIG_AZURE_RTOS_USBX 1)
set(CONFIG_AZURE_RTOS_USBX_DEVICE 1)
set(CONFIG_AZURE_RTOS_USBX_CLASS_HID 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__threadx_irq_stack_top=_stack)
sdk_compile_definitions(-DCONFIG_THREADX=1)
sdk_compile_definitions(-DUX_DEVICE_SIDE_ONLY=1)
sdk_compile_definitions(-DCONFIG_USB_DBG_LEVEL=1)

sdk_inc(inc)
project(threadx_usbx_hid_keyboard)
sdk_app_src(src/demo_hid_keyboard.c)

generate_ses_project()
