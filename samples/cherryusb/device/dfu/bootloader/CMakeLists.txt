# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_DFU 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
  set(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(dfu)
sdk_inc(../common)
sdk_src(../common/hpm_dfu_trigger.c)
sdk_inc(../config)
sdk_app_src(src/main.c src/dfu.c)
sdk_compile_definitions(USBD_DFU_APP_DEFAULT_ADD=0x80020000)

# Generate IDE projects
generate_ide_projects()
