# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(HEAP_SIZE 0x40000)
set(STACK_SIZE 0x10000)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()

set(CONFIG_LWIP 1)
set(CONFIG_LWIP_HTTPD_MBEDTLS 1)
set(CONFIG_LWIP_HTTPSSRV 1)

set(CONFIG_MBEDTLS 1)
set(CONFIG_MBEDTLS_USE_HPM_SDP 1)

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

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
sdk_compile_definitions(-D__USE_ENET_RECEIVE_INTERRUPT=0)
sdk_compile_definitions(-DLWIP_DHCP=0)

sdk_compile_definitions(-DMBEDTLS_CONFIG_FILE="hpm_sdk_mbedtls_config.h")
sdk_compile_definitions(-DCONFIG_MBEDTLS_USE_HPM_SDP=1)

project(lwip_https_server_example)
sdk_inc(../common)
sdk_inc(../common/baremetal)
sdk_inc(../common/baremetal/arch)
sdk_inc(../common/baremetal/feature)

sdk_inc(inc)
sdk_inc(inc/lwip_httpd_mbedtls)
sdk_inc(inc/eth)
sdk_inc(inc/app)

sdk_app_src(../common/baremetal/arch/ethernetif.c)
sdk_app_src(../common/baremetal/arch/sys_arch.c)
sdk_app_src(../common/baremetal/feature/common_lwip.c)
sdk_app_src(src/lwip_httpd_mbedtls/httpd_mbedtls.c)
sdk_app_src(src/eth/netconf.c)
sdk_app_src(src/lwip.c)
generate_ses_project()
