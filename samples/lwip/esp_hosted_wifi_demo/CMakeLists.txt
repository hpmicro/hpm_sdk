# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

set(CONFIG_FREERTOS 1)
set(CONFIG_ESP_HOST_MCU 1)
set(CONFIG_ESP_HOSTED_SPI 1)
set(CONFIG_HPM_SPI 1)
set(CONFIG_DMA_MGR 1)
set(CONFIG_LWIP 1)
set(CONFIG_LWIP_STRERR 1)
set(CONFIG_LWIP_SOCKET_API 1)
set(CONFIG_LWIP_NETDB 1)

# Enable Cherry Shell
set(CONFIG_CHERRYSH 1)
set(CONFIG_CHERRYSH_INTERFACE "uart")

set(STACK_SIZE 0x2000)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(esp_hosted_wifi_demo)

sdk_compile_definitions(-DCHERRYSH_TASK_REPL_STACK_SIZE=512)
sdk_compile_definitions(-DCHERRYSH_TASK_EXEC_STACK_SIZE=512)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_compile_definitions(-DconfigTOTAL_HEAP_SIZE=61440)
sdk_compile_definitions(-DLWIP_TIMEVAL_PRIVATE=0)

sdk_compile_definitions(-DLWIP_DHCP=1)

sdk_compile_definitions(-DCONFIG_SLAVE_IDF_TARGET_ESP32=1)
sdk_compile_definitions(-DCONFIG_ESP_HOSTED_RESET_GPIO_PIN_RESET=BOARD_APP_ESP_HOSTED_GPIO_RESET_PIN)
sdk_compile_definitions(-DCONFIG_ESP_HOSTED_SPI_GPIO_HANDSHAKE_PIN=BOARD_APP_ESP_HOSTED_GPIO_HANDSHAKE_PIN)
sdk_compile_definitions(-DCONFIG_ESP_HOSTED_SPI_GPIO_DATA_READY_PIN=BOARD_APP_ESP_HOSTED_GPIO_DATA_READY_PIN)
sdk_compile_definitions(-DCONFIG_ESP_HOSTED_SPI_CS_GPIO_PIN=BOARD_SPI_CS_PIN)
sdk_compile_definitions(-DCONFIG_ESP_HOSTED_SPI_DATAREADY_ACTIVE_HIGH=1)
sdk_compile_definitions(-DCONFIG_ESP_HOSTED_SPI_HANDSHAKE_ACTIVE_HIGH=1)
sdk_compile_definitions(-DCONFIG_ESP_HOSTED_RESET_GPIO_ACTIVE_LOW=0)
sdk_compile_definitions(-DCONFIG_ESP_HOSTED_SPI_MODE=2)
sdk_compile_definitions(-DCONFIG_ESP_HOSTED_SPI_CLOCK_FREQ_MHZ=10)

sdk_compile_definitions(-DCONFIG_ESP_HOSTED_DFLT_TASK_STACK=512)
sdk_compile_definitions(-DCONFIG_ESP_HOSTED_RPC_TASK_STACK_SIZE=1024)


sdk_inc(inc)
sdk_inc(lwip_port)
sdk_app_src(lwip_port/arch/sys_arch.c)
sdk_app_src(src/esp_wlan_ops.c)
sdk_app_src(src/esp_hosted_wifi_lwip.c)
sdk_app_src(src/esp_hosted_wifi_demo.c)
sdk_app_src(src/app/ping_cmd.c)
sdk_app_src(src/app/iperf_cmd.c)

generate_ide_projects()
