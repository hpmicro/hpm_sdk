# Copyright (c) 2023-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_SDMMC 1)
set(CONFIG_HPM_SPI_SDCARD 1)
set(CONFIG_FATFS 1)
set(CONFIG_HPM_SPI 1)
set(CONFIG_DMA_MGR 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(spi_sd_fatfs)

sdk_compile_definitions(-DUSE_DMA_TRANSFER=1)
sdk_compile_definitions(-DFF_CODE_PAGE=437)
sdk_compile_definitions(-DSPI_SD_LOG=printf)
sdk_compile_definitions(-DSPI_SD_SPEED_MAX_HZ=40000000)

sdk_inc(src)
sdk_inc(../common/adapt)

sdk_app_src(../common/adapt/spi_sd_adapt.c)
sdk_app_src(src/spi_sd_fatfs.c)

sdk_iar_ld_options(--disable_relaxation) # A workaround to fix EWRISCV3.30 linking issue

generate_ide_projects()
