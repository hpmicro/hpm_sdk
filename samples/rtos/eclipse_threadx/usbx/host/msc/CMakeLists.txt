# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_ECLIPSE_THREADX 1)
set(CONFIG_ECLIPSE_THREADX_USBX 1)
set(CONFIG_ECLIPSE_THREADX_FILEX 1)
set(CONFIG_ECLIPSE_THREADX_USBX_HOST 1)
set(CONFIG_ECLIPSE_THREADX_USBX_CLASS_STORAGE 1)
set(CONFIG_ECLIPSE_THREADX_USBX_CLASS_HUB 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(usbx_host_msc)

sdk_compile_definitions(-DUX_HOST_SIDE_ONLY=1)
sdk_compile_definitions(-DUSB_HOST_MCU_CORE=HPM_CORE0)
sdk_compile_definitions(-DUX_HCD_EHCI_SPLIT_TRANSFER_ENABLE=1)

sdk_inc(src)
sdk_app_src(src/demo_host_msc.c)
generate_ide_projects()
