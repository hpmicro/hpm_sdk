# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYSH_INTERFACE "uart")
set(CONFIG_CHERRYSH 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(cherrysh_barebone_uart_demo)

sdk_inc(inc)
sdk_app_src_glob(src/*.c)

generate_ide_projects()
