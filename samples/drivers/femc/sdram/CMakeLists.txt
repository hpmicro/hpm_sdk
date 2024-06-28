# Copyright (c) 2021 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(sdram_example)

sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA)
sdk_app_src(src/sdram.c)
generate_ide_projects()
