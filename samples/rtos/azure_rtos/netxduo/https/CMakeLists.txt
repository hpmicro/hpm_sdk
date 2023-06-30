# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_AZURE_RTOS_THREADX 1)
set(CONFIG_AZURE_RTOS_FILEX 1)
set(CONFIG_AZURE_RTOS_NETXDUO 1)
set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_HTTP 1)
set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_WEB 1)
# This should be defined to include DHCP source code
set(CONFIG_AZURE_RTOS_NETXDUO_ADDONS_DHCP 1)


#set(CONFIG_ENET_PHY_DP83867 1)
#set(CONFIG_ENET_PHY_RTL8211 1)
#set(CONFIG_ENET_PHY_DP83848 1)
set(CONFIG_ENET_PHY_RTL8201 1)

set(CONFIG_AZURE_RTOS_NETXDUO_SECURE 1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-D__threadx_irq_stack_top=_stack)
# This must be defined to select the right irq handler
sdk_compile_definitions(-DCONFIG_THREADX=1)
# Comment this if there is no need to enable DHCP
sdk_compile_definitions(-DCONFIG_NETX_ENABLE_DHCP=1)

sdk_compile_definitions(-DNX_DRIVER_ENABLE_DEFERRED=1)
sdk_compile_definitions(-DNX_WEB_HTTPS_ENABLE=1)
sdk_compile_definitions(-DNX_SECURE_ENABLE_PSK_CIPHERSUITES=1)
sdk_compile_definitions(-DNX_SECURE_TLS_ENABLE_TLS_1_0=1)
sdk_compile_definitions(-DNX_SECURE_TLS_TLS_1_2_ENABLED=1)
sdk_compile_definitions(-DNX_SECURE_TLS_REQUIRE_RENEGOTIATION_EXT=1)
sdk_compile_definitions(-DNX_SECURE_ENABLE_AEAD_CIPHER=1)

project(netx_duo_https)


sdk_app_src(src/https.c)
generate_ses_project()
