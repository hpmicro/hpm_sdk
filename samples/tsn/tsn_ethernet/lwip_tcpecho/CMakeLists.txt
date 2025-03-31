# Copyright (c) 2024-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_LWIP 1)
set(CONFIG_TSW_PHY 1)
set(APP_USE_TSW_PORT_COUNT 1)
#set(APP_USE_TSW_ITF_RGMII 1)

if(NOT DEFINED APP_USE_TSW_PORT_COUNT)
    message(FATAL_ERROR "APP_USE_TSW_PORT_COUNT is undefined!")
endif()

if(NOT APP_USE_TSW_PORT_COUNT EQUAL 1)
    message(FATAL_ERROR "This sample supports only one TSW port!")
endif()

if (APP_USE_TSW_ITF_RGMII AND APP_USE_TSW_ITF_RMII)
    message(FATAL_ERROR "This sample doesn't supported more than one TSW phy!")
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
sdk_compile_definitions(-DLWIP_DHCP=0)
#sdk_compile_definitions(-DENABLE_TSW_RECEIVE_INTERRUPT=1)

project(tsn_lwip_tcpecho_example)
sdk_inc(../ports/baremetal/single)
sdk_inc(../ports/baremetal/single/arch)
sdk_inc(../common/single)
sdk_inc(inc)
sdk_inc(inc/app)

sdk_app_src(../ports/baremetal/single/arch/sys_arch.c)
sdk_app_src(../ports/baremetal/single/ethernetif.c)
sdk_app_src(../common/single/common.c)
sdk_app_src(../common/single/netconf.c)
sdk_app_src(src/app/tcp_echo.c)
sdk_app_src(src/lwip.c)
generate_ide_projects()
