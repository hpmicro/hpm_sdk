# Copyright (c) 2023-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_ECLIPSE_THREADX 1)
set(CONFIG_ECLIPSE_THREADX_FILEX 1)
set(CONFIG_ECLIPSE_THREADX_NETXDUO 1)
set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_HTTP 1)
set(CONFIG_ECLIPSE_THREADX_NETXDUO_ADDONS_WEB 1)
set(CONFIG_ECLIPSE_THREADX_NETXDUO_UTILITY_IPERF 1)

# Select the phy on the board
set(CONFIG_ENET_PHY 1)
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
    message(FATAL_ERROR "This sample doesn't support more than one Ethernet phy!")
endif()

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-DNETX_TX_DATA_COPY_ALGORITHM=1)
sdk_compile_definitions(-DNETX_RX_DATA_COPY_ALGORITHM=1)
sdk_compile_definitions(-DNX_DRIVER_ENABLE_DEFERRED=1)

# Increse buffer count to improve performance
sdk_compile_definitions(-DENET_TX_BUFF_COUNT=60)
sdk_compile_definitions(-DENET_RX_BUFF_COUNT=50)

project(netx_duo_iperf)

sdk_inc(src)
sdk_app_src(src/iperf.c)
generate_ide_projects()
