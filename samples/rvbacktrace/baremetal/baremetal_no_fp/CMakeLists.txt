# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_RV_BACKTRACE true)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(bt_no_fp)

sdk_inc(src)
sdk_app_src(../baremetal.c)
generate_ide_projects()
