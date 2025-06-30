# Copyright (c) 2025 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CANOPEN 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(canopen_slave_example)

sdk_app_src(src/slave_canopen_demo.c)
generate_ide_projects()
