# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_ERPC 1)
set(CONFIG_ERPC_CLIENT 1)
set(CONFIG_ERPC_RPMSG_MASTER 1)
set(CONFIG_IPC_EVENT_MGR 1)
set(CONFIG_IPC_EVENT_MGR_MBX 1)
set(CONFIG_FREERTOS 1)

set(CONFIG_FREERTOS 1)
set(CONFIG_CMSIS_RTOS2 1)
set(CONFIG_OPENER 1)
set(CONFIG_LWIP 1)
set(CONFIG_LWIP_HTTPSRV 1)
set(CONFIG_LWIP_STRERR 1)
set(CONFIG_LWIP_SOCKET_API 1)

set(CONFIG_ENET_PHY 1)
set(APP_USE_ENET_PORT_COUNT 1)
#set(APP_USE_ENET_ITF_RGMII 1)
#set(APP_USE_ENET_ITF_RMII 1)
#set(APP_USE_ENET_PHY_DP83867 1)
#set(APP_USE_ENET_PHY_RTL8211 1)
#set(APP_USE_ENET_PHY_DP83848 1)
#set(APP_USE_ENET_PHY_RTL8201 1)
if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(erpc_motor_core0)

## Erpc files
sdk_inc(src)
sdk_inc(../../../common)

sdk_app_src(../../../common/multicore_common.c)
sdk_app_src(src/main_core0.c)
sdk_app_src(src/sec_core_img.c)

sdk_inc(./config)
sdk_inc(../generated)
sdk_src(../generated/erpc_motor_client.cpp)
## Lwip files

sdk_inc(./lwip)
sdk_inc(./lwip/single)
sdk_inc(./lwip/single/arch)
sdk_inc(./lwip/common/single)

sdk_app_src(./lwip/single/arch/sys_arch.c)
sdk_app_src(./lwip/single/ethernetif.c)
sdk_app_src(./lwip/common/single/common.c)
sdk_app_src(./lwip/common/single/netconf.c)

## OpENer application files
sdk_inc(./opener)
sdk_inc(./opener/sample_application)

sdk_app_src(./opener/sample_application/ethlinkcbs.c)
sdk_app_src(./opener/sample_application/sampleapplication.c)
sdk_app_src(./opener/networkconfig.c)
sdk_app_src(./opener/networkhandler.c)
sdk_app_src(./opener/opener_error.c)
sdk_app_src(./opener/opener.c)

sdk_compile_definitions(-DHPM_FEATURE_MBX_SIDE_A)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_options(-fno-exceptions)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_definitions(-DLWIP_DHCP=0)
sdk_compile_definitions(-D__DISABLE_AUTO_NEGO=0)
sdk_compile_definitions(-DOPENER_WITH_TRACES=1)
sdk_compile_definitions(-DLWIP_NETBUF_RECVINFO=1)
sdk_compile_definitions(-DMEMP_OVERFLOW_CHECK=3)

sdk_ld_options("--std=c++11")
generate_ide_projects()
