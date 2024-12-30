# Copyright (c) 2021 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CANOPEN 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(canopen_example)

sdk_inc(inc)

sdk_app_src(src/canopen_demo.c)
sdk_app_src(src/hpm_canopen_test.c)
generate_ide_projects()
