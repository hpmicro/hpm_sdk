# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYSH 1)
set(CONFIG_FREERTOS 1)
set(CONFIG_CHERRYSH_INTERFACE "uart")
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherrysh_freertos_uart_demo)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_inc(inc)
sdk_inc(inc)
sdk_app_src_glob(src/*.c)

generate_ide_projects()
