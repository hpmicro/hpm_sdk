# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.5)

set(CONFIG_FREERTOS 1)
set(CONFIG_TINYUSB 1)
set(CONFIG_TINYUSB_HOST 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(tinyusb_host_cdc_msc_hid_freertos_example)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_inc(src)

sdk_app_src(src/main.c)
sdk_app_src(src/hid_app.c)
sdk_app_src(src/cdc_app.c)
sdk_app_src(src/msc_app.c)

generate_ide_projects()
