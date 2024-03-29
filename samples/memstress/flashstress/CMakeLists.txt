# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(flashstress)

sdk_app_src(src/flashstress.c)
sdk_app_src(src/flashstress_lib.c)
generate_ide_projects()
