# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(lobs_example)

sdk_compile_definitions(-DLOBS_USE_TWO_GROUP=1)

sdk_app_src(src/lobs.c)

generate_ide_projects()
