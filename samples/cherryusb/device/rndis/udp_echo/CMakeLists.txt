# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_RNDIS 1)
set(CONFIG_LWIP 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherryusb_device_rndis_udp_echo)

sdk_inc(../../../config)
sdk_inc(src)
sdk_inc(../common)
sdk_inc(../common/dhcp-server)
sdk_app_src(src/main.c)
sdk_app_src(src/udp_echo.c)
sdk_app_src(src/cdc_rndis_device.c)
sdk_app_src(../common/dhcp-server/dhserver.c)
generate_ide_projects()
