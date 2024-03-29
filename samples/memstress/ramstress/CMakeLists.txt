# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ramstress)

sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA)
sdk_app_src(src/ramstress.c)
sdk_app_src(src/ramstress_lib.c)
generate_ide_projects()
