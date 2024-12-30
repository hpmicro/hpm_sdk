# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_SDMMC 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(sdcard_example)

sdk_compile_definitions(-DHPM_SDMMC_ENABLE_CACHE_MAINTENANCE=0)

# Note: enable the following definition to enable interrupt-based transfer
# sdk_compile_definitions(-DHPM_SDMMC_HOST_ENABLE_IRQ=1)

sdk_inc(src)

sdk_app_src(src/sdcard_example.c)
generate_ide_projects()
