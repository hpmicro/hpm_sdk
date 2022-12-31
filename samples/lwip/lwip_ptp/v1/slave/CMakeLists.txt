# Copyright (c) 2021-2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_xip)
endif()

set(HPM_SDK_NO_NANO_SPECS 1)
set(CONFIG_LWIP 1)
#set(CONFIG_ENET_PHY_DP83867 1)
#set(CONFIG_ENET_PHY_RTL8211 1)
set(CONFIG_ENET_PHY_DP83848 1)
#set(CONFIG_ENET_PHY_RTL8201 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(lwip_ptp_slave_example)
sdk_inc(../../../common)
sdk_inc(../../../common/arch)
sdk_inc(../../../common/feature)
sdk_inc(inc)
sdk_inc(inc/eth)

sdk_inc(src/ptpd/src)
sdk_inc(src/ptpd/src/ptp)
sdk_inc(src/ptpd/src/ptp/dep)

sdk_app_src(../../../common/arch/ethernetif.c)
sdk_app_src(../../../common/feature/common.c)
sdk_app_src(src/eth/netconf.c)
sdk_app_src(src/lwip.c)

sdk_app_src(src/ptpd/src/arith.c)
sdk_app_src(src/ptpd/src/bmc.c)
sdk_app_src(src/ptpd/src/protocol.c)
sdk_app_src(src/ptpd/src/ptpd.c)

sdk_app_src(src/ptpd/src/dep/msg.c)
sdk_app_src(src/ptpd/src/dep/net.c)
sdk_app_src(src/ptpd/src/dep/servo.c)
sdk_app_src(src/ptpd/src/dep/startup.c)
sdk_app_src(src/ptpd/src/dep/sys.c)
sdk_app_src(src/ptpd/src/dep/timer.c)

generate_ses_project()
