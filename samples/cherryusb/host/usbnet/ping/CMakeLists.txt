# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_HOST_RNDIS 1)
set(CONFIG_USB_HOST_CDC_ECM 1)
set(CONFIG_LWIP 1)
set(CONFIG_LWIP_STRERR 1)
set(CONFIG_LWIP_NETDB 1)
set(CONFIG_LWIP_SOCKET_API 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherryusb_host_usbnet_ping)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_definitions(-DCONFIG_PING_COUNT=5)
sdk_compile_definitions(-DconfigTOTAL_HEAP_SIZE=30720)
sdk_compile_definitions(-DLWIP_TIMEVAL_PRIVATE=0)

sdk_iar_cc_preinclude(../../../../../drivers/inc/hpm_common.h)

sdk_inc(../../../config)
sdk_inc(src)
sdk_inc(../common)

sdk_app_src(../common/arch/sys_arch.c)
sdk_app_src(src/main.c)
sdk_app_src(src/ping.c)
generate_ide_projects()
