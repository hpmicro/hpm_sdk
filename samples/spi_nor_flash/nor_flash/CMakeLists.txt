# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_SPI_NOR_FLASH 1)
set(CONFIG_DMA_MGR 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(spi_nor_flash_demo)

sdk_inc(../common/port)

sdk_app_src(../common/port/hpm_serial_nor_host_port.c)
sdk_app_src(src/main.c)

sdk_compile_definitions(-DPORT_SPI_CLK_FREQUENCY=80000000)
sdk_compile_definitions(-DPORT_SPI_IO_MODE=SERIAL_NOR_HOST_SUPPORT_QUAD_IO_MODE)

sdk_compile_options("-O3")

generate_ide_projects()
