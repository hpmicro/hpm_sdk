# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(mbx_example_core0)

sdk_app_inc(../../common)

sdk_app_src(../src/mbx.c)
sdk_app_src(../../common/multicore_common.c)
sdk_app_src(../src/sec_core_img.c)
generate_ide_projects()
