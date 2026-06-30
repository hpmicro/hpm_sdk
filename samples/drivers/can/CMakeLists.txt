# Copyright (c) 2021-2026 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(can_example)

sdk_app_src(src/can_app_common.c)
sdk_app_src(src/can_loopback_demo.c)
sdk_app_src(src/can_echo_demo.c)
sdk_app_src(src/can_txrx_demo.c)
sdk_app_src(src/can_error_demo.c)
sdk_app_src(src/can_filter_demo.c)
sdk_app_src(src/can_timestamp_demo.c)
generate_ide_projects()
