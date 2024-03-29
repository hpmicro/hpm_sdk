# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_ECLIPSE_THREADX 1)
set(CONFIG_ECLIPSE_THREADX_NETXDUO 1)
# This should be defined to include DHCP source code
set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_DHCP 1)

set(APP_USE_ENET_PORT_COUNT 1)
#set(APP_USE_ENET_ITF_RGMII 1)
#set(APP_USE_ENET_ITF_RMII 1)
#set(APP_USE_ENET_PHY_DP83867 1)
#set(APP_USE_ENET_PHY_RTL8211 1)
#set(APP_USE_ENET_PHY_DP83848 1)
#set(APP_USE_ENET_PHY_RTL8201 1)

if(NOT DEFINED APP_USE_ENET_PORT_COUNT)
    message(FATAL_ERROR "APP_USE_ENET_PORT_COUNT is undefined!")
endif()

if(NOT APP_USE_ENET_PORT_COUNT EQUAL 1)
    message(FATAL_ERROR "This sample supports only one Ethernet port!")
endif()

if (APP_USE_ENET_ITF_RGMII AND APP_USE_ENET_ITF_RMII)
    message(FATAL_ERROR "This sample doesn't supported more than one Ethernet phy!")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__threadx_irq_stack_top=_stack)
# This must be defined to select the right irq handler
sdk_compile_definitions(-DCONFIG_THREADX=1)
# Comment this if there is no need to enable DHCP
sdk_compile_definitions(-DCONFIG_NETX_ENABLE_DHCP=1)

project(netx_duo_ping)

sdk_inc(src)
sdk_app_src(src/ping.c)
generate_ide_projects()
