# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_UART_LIN 1) # use hpm_uart_lin component
set(CONFIG_HPM_PLB_LIN_CLOCK_DETECTION 1) # plb detect lin baudrate
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(uart_lin_slave_baudrate_adaptive)

sdk_app_src(src/uart_lin_slave_baudrate_adaptive.c)
generate_ide_projects()
