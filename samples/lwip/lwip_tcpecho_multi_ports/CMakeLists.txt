# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(HPM_SDK_LD_NO_NANO_SPECS 1)
set(CONFIG_LWIP 1)

if(${BOARD} STREQUAL "hpm6750evk")
set(CONFIG_ENET_PHY_DP83867 1)
set(CONFIG_ENET_PHY_DP83848 1)
elseif(${BOARD} STREQUAL "hpm6750evk2")
set(CONFIG_ENET_PHY_RTL8211 1)
set(CONFIG_ENET_PHY_RTL8201 1)
else()
message("This board doesn't support multiple enet ports!")
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
sdk_compile_definitions(-D__ENABLE_ENET_RECEIVE_INTERRUPT=0)
sdk_compile_definitions(-DENET_MULTIPLE_PORT=1)

project(lwip_tcpecho_multi_ports_example)

sdk_inc(inc)
sdk_inc(inc/arch)
sdk_inc(inc/feature)
sdk_inc(inc/eth)
sdk_inc(inc/app)

sdk_app_src(src/arch/sys_arch.c)
sdk_app_src(src/arch/ethernetif.c)
sdk_app_src(src/feature/common_lwip.c)
sdk_app_src(src/eth/netconf.c)
sdk_app_src(src/app/tcp_echo.c)
sdk_app_src(src/lwip.c)
generate_ses_project()
