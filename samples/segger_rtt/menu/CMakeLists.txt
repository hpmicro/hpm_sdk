# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_SEGGER_RTT 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(segger_rtt_menu)

sdk_compile_definitions(-DCONFIG_NDEBUG_CONSOLE=1)
sdk_ses_opt_lib_io_type(RTT)
sdk_ses_opt_debug_connection(J-Link)
sdk_ses_opt_debug_jlink_speed(50000)
sdk_app_src(src/main.c)
generate_ide_projects()
