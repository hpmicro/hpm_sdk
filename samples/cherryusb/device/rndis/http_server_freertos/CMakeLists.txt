# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_RNDIS 1)
set(CONFIG_LWIP 1)
set(CONFIG_LWIP_STRERR 1)
set(CONFIG_LWIP_NETDB 1)
set(CONFIG_LWIP_SOCKET_API 1)
set(CONFIG_LWIP_HTTPSRV 1)
set(CONFIG_FREERTOS 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherryusb_device_rndis_http_server_freertos)
sdk_inc(../../../config)
sdk_inc(src)
sdk_inc(../common)
sdk_inc(../common/arch)
sdk_inc(../common/dhcp-server)
sdk_inc(../common/dns-server)
sdk_iar_cc_preinclude(../../../../../drivers/inc/hpm_common.h)

sdk_app_src(../common/arch/sys_arch.c)
sdk_app_src(src/main.c)
sdk_app_src(src/cdc_rndis_device.c)
sdk_app_src(../common/dhcp-server/dhserver.c)
sdk_app_src(../common/dns-server/dnserver.c)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
generate_ide_projects()
