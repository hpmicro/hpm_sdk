# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_SDMMC 1)
set(CONFIG_HPM_SPI_SDCARD 1)
set(CONFIG_HPM_SPI 1)
set(CONFIG_DMA_MGR 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(spi_sdcard_sample)

sdk_compile_definitions(-DUSE_DMA_TRANSFER=1)
sdk_compile_definitions(-DSPI_SD_LOG=printf)
sdk_compile_definitions(-DSPI_SD_SPEED_MAX_HZ=40000000)

sdk_inc(../common/adapt)

sdk_app_src(../common/adapt/spi_sd_adapt.c)
sdk_app_src(src/spi_sdcard.c)

generate_ide_projects()



