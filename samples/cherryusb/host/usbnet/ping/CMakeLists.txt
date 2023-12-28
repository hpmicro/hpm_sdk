# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_HOST_RNDIS 1)
set(CONFIG_USB_HOST_CDC_ECM 1)
set(CONFIG_LWIP 1)
set(CONFIG_LWIP_STRERR 1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherryusb_host_rndis_ping)

sdk_compile_definitions(-D__freertos_irq_stack_top=_stack)
sdk_compile_definitions(-DCONFIG_FREERTOS=1)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_definitions(-DCONFIG_PING_COUNT=5)
sdk_compile_definitions(-DconfigTOTAL_HEAP_SIZE=30720)
sdk_inc(../../../config)
sdk_inc(src)
sdk_inc(../common)

sdk_app_src(../../../../../middleware/lwip/src/api/netdb.c)
sdk_app_src(../../../../../middleware/lwip/src/api/sockets.c)
sdk_app_src(../common/arch/sys_arch.c)
sdk_app_src(../common/feature/rndis_host.c)
sdk_app_src(../common/feature/ecm_host.c)
sdk_app_src(src/main.c)
sdk_app_src(src/ping.c)
generate_ses_project()
