# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_USB_DEVICE 1)

set(CONFIG_AZURE_RTOS_THREADX 1)
set(CONFIG_AZURE_RTOS_FILEX 1)
set(CONFIG_AZURE_RTOS_USBX 1)
set(CONFIG_AZURE_RTOS_USBX_DEVICE 1)
set(CONFIG_AZURE_RTOS_USBX_CLASS_STORAGE 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__threadx_irq_stack_top=_stack)
sdk_compile_definitions(-DCONFIG_THREADX=1)
sdk_compile_definitions(-DUX_DEVICE_SIDE_ONLY=1)
project(threadx_usbx_msc)
sdk_inc(inc)
sdk_app_src(src/demo_msc.c)
generate_ses_project()
