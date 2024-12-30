# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)


set(CONFIG_SDMMC 1)
set(CONFIG_FATFS 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(sd_fatfs)

sdk_compile_definitions(-DSD_FATFS_ENABLE=1)
sdk_compile_definitions(-DFF_CODE_PAGE=437)

# Note: enable the following definition to enable interrupt-based transfer
# sdk_compile_definitions(-DHPM_SDMMC_HOST_ENABLE_IRQ=1)

sdk_compile_options(-Os)

sdk_inc(src)

sdk_app_src(src/sd_fatfs.c)
generate_ide_projects()
