# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(uart_software_rx_idle)

sdk_inc(src)
sdk_app_src(src/uart.c)
generate_ide_projects()
