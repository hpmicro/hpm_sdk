# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(xpi_nor_api_demo)

sdk_app_inc(src)
sdk_app_src(src/xpi_util.c)

sdk_app_src(src/xpi_nor_api_demo.c)

sdk_compile_options(-fno-inline)

generate_ide_projects()
