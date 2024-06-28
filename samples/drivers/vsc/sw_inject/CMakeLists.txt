# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(vsc_sw_inject_example)

sdk_inc(src)
sdk_inc(data)
sdk_app_src(src/vsc.c)

generate_ide_projects()
