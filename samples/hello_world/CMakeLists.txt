# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(hello_world)

sdk_compile_definitions(-DBOARD_SHOW_CLOCK=0)
sdk_app_src(src/hello_world.c)
generate_ses_project()
