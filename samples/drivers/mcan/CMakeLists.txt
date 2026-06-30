# Copyright (c) 2023-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(mcan_example)

sdk_app_src(src/mcan_app_common.c)
sdk_app_src(src/mcan_loopback_demo.c)
sdk_app_src(src/mcan_echo_demo.c)
sdk_app_src(src/mcan_txrx_demo.c)
sdk_app_src(src/mcan_filter_demo.c)
sdk_app_src(src/mcan_error_demo.c)
sdk_app_src(src/mcan_rxbuf_demo.c)
sdk_app_src(src/mcan_tx_event_demo.c)
sdk_app_src(src/mcan_timestamp_demo.c)
sdk_app_src(src/mcan_timeout_demo.c)
sdk_app_src(src/mcan_txcancel_demo.c)
sdk_app_src(src/mcan_txqueue_demo.c)
generate_ide_projects()
