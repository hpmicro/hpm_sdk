# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

# EtherCAT EEPROM
set(CONFIG_EEPROM_EMULATION 1) # using eeprom emulation compenent

# LwIP
set(CONFIG_LWIP 1)
set(CONFIG_LWIP_STRERR 1)
set(CONFIG_LWIP_SOCKET_API 1)
set(APP_USE_ENET_PORT_COUNT 1)

# FreeRTOS
set(CONFIG_FREERTOS 1)
set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)

# ModBus
set(CONFIG_AGILE_MODBUS 1)
set(CONFIG_AGILE_MODBUS_TCP 1)
set(CONFIG_AGILE_MODBUS_SLAVE_UTIL 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

# Demo definition
sdk_compile_definitions(-DUSE_LWIPOPTS_APP_H=1)
sdk_compile_definitions(-DLWIP_TIMEVAL_PRIVATE=0)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=0)
sdk_compile_definitions(-DUSE_SYSCALL_INTERRUPT_PRIORITY=1)
sdk_compile_definitions(-DMODBUS_TCP_SLAVE)
sdk_compile_definitions(-DECAT_SSC_TIMER_USE_RTOS_TICK=1) # SSC code use rtos tick as timer

project(ecat_eoe_modbus_tcp_server_freertos)

# ECAT SSC code
add_subdirectory(../SSC ${CMAKE_BINARY_DIR}/SSC_build)

sdk_inc(../../port)
sdk_src(../../port/hpm_ecat_hw.c)
sdk_src(../../port/hpm_ecat_phy.c)
if(CONFIG_EEPROM_EMULATION)
sdk_src(../../port/hpm_ecat_e2p_emulation.c)
endif()

# LwIP port code
sdk_inc(../../port/lwip/freertos/single)
sdk_inc(../../port/lwip/freertos/single/arch)
sdk_app_src(../../port/lwip/freertos/single/arch/sys_arch.c)
sdk_app_src(../../port/lwip/freertos/single/eoe_ethernetif.c)

# ModBus port code
sdk_inc(../../port/modbus_tcp/freertos)
sdk_app_src(../../port/modbus_tcp/freertos/modbus_tcp_server.c)


# Application code
sdk_inc(inc)
sdk_app_src(src/main.c)
sdk_app_src(src/modbus_application.c)
sdk_app_src(src/modbus_mem.c)
sdk_app_src(src/ecat_application.c)
generate_ide_projects()
