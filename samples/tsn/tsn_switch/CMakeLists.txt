# Copyright (c) 2024-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_ENET_PHY 1)
set(APP_USE_TSW_PORT_COUNT 3)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(tsn_switch_example)
sdk_app_src(src/tsn_switch.c)
generate_ide_projects()
