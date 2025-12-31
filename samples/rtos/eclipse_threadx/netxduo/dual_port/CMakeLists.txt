# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_ECLIPSE_THREADX 1)
set(CONFIG_ECLIPSE_THREADX_NETXDUO 1)

# Select the phy on the board
set(CONFIG_ENET_PHY 1)
set(APP_USE_ENET_PORT_COUNT 2)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(netx_duo_multiport)
sdk_compile_definitions(-DENET_MULTIPLE_PORT=1)

# use sdk_compile_definitions(-DMAC{0-1}_CONFIG=98:2c:bc:b1:9f:27) to specify the mac address
sdk_inc(src)
sdk_app_src(src/dual_port.c)
generate_ide_projects()
