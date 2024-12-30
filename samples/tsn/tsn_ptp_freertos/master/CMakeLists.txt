# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_TSW_PHY 1)
set(CONFIG_TSN 1)
set(APP_USE_TSW_PORT_COUNT 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(tsn_ptp_master_freertos_example)
sdk_inc(src)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_definitions(-DTSN_PTP_MASTER=1)
sdk_app_src(src/tsn_ptp_master_freertos.c)
generate_ide_projects()
