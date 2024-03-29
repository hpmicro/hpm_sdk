# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dhrystone)

sdk_clear_compile_options()
set(opt_string "-O3 -fno-inline")
separate_arguments(opt_list UNIX_COMMAND ${opt_string})

foreach(opt ${opt_list})
sdk_compile_options(${opt})
endforeach()
sdk_get_compile_options(all_opts)
sdk_compile_definitions(LOOP_COUNT=9000000)
sdk_iar_options("--diag_suppress Pe260")

sdk_inc(src)
sdk_app_src(src/dhry_1.c)
sdk_app_src(src/dhry_2.c)
generate_ide_projects()
