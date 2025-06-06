# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_FREERTOS_TICKLESS 1)
set(CONFIG_FREERTOS_TICKLESS_USE_STOP_MODE 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(freertos_tickless_stop_mode)

sdk_inc(src)

sdk_app_src(src/freertos_tickless.c)
sdk_app_src(src/${HPM_SOC}/low_power_example.c)

sdk_compile_definitions(-DBOARD_CONSOLE_UART_BASE=HPM_PUART)
sdk_compile_definitions(-DBOARD_CONSOLE_UART_CLK_NAME=clock_puart)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
generate_ide_projects()
