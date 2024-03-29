# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(uart_interrupt_example)

sdk_compile_definitions(-DCONFIG_UART_FIFO_MODE=1)

sdk_app_src(src/uart.c)
generate_ide_projects()
