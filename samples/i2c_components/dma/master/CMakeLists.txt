# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_I2C 1)
set(CONFIG_DMA_MGR 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(i2c_components_dma_master_demo)

sdk_app_src(src/main.c)
generate_ide_projects()
