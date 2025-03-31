# Copyright (c) 2024-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

set(CONFIG_LWIP 1)
set(CONFIG_LWIP_IPERF 1)
set(CONFIG_ENET_PHY 1)
set(APP_USE_ENET_PORT_COUNT 2)

if(NOT DEFINED APP_USE_ENET_PORT_COUNT)
    message(FATAL_ERROR "APP_USE_ENET_PORT_COUNT is undefined!")
endif()

if(NOT APP_USE_ENET_PORT_COUNT EQUAL 2)
    message(FATAL_ERROR "This sample must use two Ethernet ports!")
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
sdk_compile_definitions(-D__ENABLE_ENET_RECEIVE_INTERRUPT=0)
sdk_compile_definitions(-DENET_MULTIPLE_PORT=1)
sdk_compile_definitions(-DLWIP_DHCP=0)
sdk_compile_definitions(-DUSE_LWIPOPTS_APP_H=1)

project(lwip_iperf_multi_ports_example)
sdk_inc(../ports/baremetal/multiple)
sdk_inc(../ports/baremetal/multiple/arch)
sdk_inc(../common/multiple)
sdk_inc(inc)

sdk_app_src(../ports/baremetal/multiple/arch/sys_arch.c)
sdk_app_src(../ports/baremetal/multiple/ethernetif.c)
sdk_app_src(../common/multiple/common.c)
sdk_app_src(../common/multiple/netconf.c)
sdk_app_src(src/lwip.c)
generate_ide_projects()
