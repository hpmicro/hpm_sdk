# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_DMA_MANAGER 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dma_manager)

sdk_compile_definitions(-DBOARD_SHOW_CLOCK=0)
sdk_app_src(src/dma_manager.c)
generate_ses_project()
