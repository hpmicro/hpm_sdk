# Copyright (c) 2023-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_AGILE_MODBUS 1)
set(CONFIG_AGILE_MODBUS_TCP 1)

if (CONFIG_AGILE_MODBUS_TCP)
set(CONFIG_MODBUS_NETWORK 1)
set(CONFIG_LWIP 1)
set(CONFIG_ENET_PHY 1)
set(APP_USE_ENET_PORT_COUNT 1)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(modbus_tcp_master)

sdk_inc(../../common)
sdk_inc(../../common/timer)
sdk_app_src(../../common/timer/timer.c)

if (CONFIG_MODBUS_NETWORK)

sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
sdk_compile_definitions(-D__ENABLE_ENET_RECEIVE_INTERRUPT=0)
sdk_compile_definitions(-DLWIP_DHCP=0)

sdk_inc(../../common/network/eth)
sdk_inc(../../common/network/lwip)
sdk_inc(../../common/network/lwip/arch)
sdk_inc(../../common/network/lwip/feature)
sdk_inc(../../common/network/lwip/inc)
sdk_inc(../../common/network/tcp)

sdk_app_src(../../common/network/lwip/arch/ethernetif.c)
sdk_app_src(../../common/network/lwip/arch/sys_arch.c)
sdk_app_src(../../common/network/lwip/feature/common_lwip.c)
sdk_app_src(../../common/network/eth/netconf.c)
sdk_app_src(../../common/network/eth/network.c)
sdk_app_src(../../common/network/tcp/tcp_client.c)
endif()

sdk_app_src(src/main.c)
generate_ide_projects()
