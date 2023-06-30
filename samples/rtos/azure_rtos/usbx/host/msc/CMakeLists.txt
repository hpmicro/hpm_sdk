# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_USB_HOST 1)

set(CONFIG_AZURE_RTOS_THREADX 1)
set(CONFIG_AZURE_RTOS_USBX 1)
set(CONFIG_AZURE_RTOS_FILEX 1)
set(CONFIG_AZURE_RTOS_USBX_HOST 1)
set(CONFIG_AZURE_RTOS_USBX_CLASS_STORAGE 1)
set(CONFIG_AZURE_RTOS_USBX_CLASS_HUB 1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__threadx_irq_stack_top=_stack)
sdk_compile_definitions(-DCONFIG_THREADX=1)
sdk_compile_definitions(-DUX_HOST_SIDE_ONLY=1)
sdk_compile_definitions(-DUSB_HOST_MCU_CORE=HPM_CORE0)
sdk_compile_definitions(-DUX_HCD_EHCI_SPLIT_TRANSFER_ENABLE=1)

# Because of so many disks's volume is too large to use fat32, so exFat is needed.
# We need to enable this feature to support exFat U disk
sdk_compile_definitions(-DFX_ENABLE_EXFAT=1)

project(usbx_host_msc)
sdk_inc(inc)
sdk_app_src(src/demo_host_msc.c)
generate_ses_project()
