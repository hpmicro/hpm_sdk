# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(enet_pps_capture_example)

sdk_app_src(src/pps_capture.c)

generate_ide_projects()
