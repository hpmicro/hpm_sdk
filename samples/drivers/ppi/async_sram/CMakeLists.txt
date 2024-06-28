# Copyright (c) 2022 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PPI 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ppi_async_sram_example)

sdk_app_src(src/async_sram.c)
generate_ide_projects()
