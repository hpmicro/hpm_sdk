# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_USB_HOST 1)

set(CONFIG_AZURE_RTOS_THREADX 1)
set(CONFIG_AZURE_RTOS_USBX 1)
set(CONFIG_AZURE_RTOS_USBX_HOST 1)
set(CONFIG_AZURE_RTOS_USBX_CLASS_HID 1)
set(CONFIG_AZURE_RTOS_USBX_CLASS_HUB 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__threadx_irq_stack_top=_stack)
sdk_compile_definitions(-DCONFIG_THREADX=1)
sdk_compile_definitions(-DUX_HOST_SIDE_ONLY=1)
sdk_compile_definitions(-DUSB_HOST_MCU_CORE=HPM_CORE0)
sdk_compile_definitions(-DUX_HCD_EHCI_SPLIT_TRANSFER_ENABLE=1)

project(usbx_host_hid_mouse)
sdk_inc(inc)
sdk_app_src(src/demo_host_hid_mouse.c)
generate_ses_project()
