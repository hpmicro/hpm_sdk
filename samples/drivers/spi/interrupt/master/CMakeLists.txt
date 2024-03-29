# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(spi_interrupt_master)

# The frequency of SCLK should adapt to SPI interrupt response time, otherwise it may cause overrun or underrun
sdk_compile_definitions(-DEXAMPLE_SPI_SCLK_FREQ=2000000)
sdk_app_src(src/spi.c)
generate_ide_projects()
