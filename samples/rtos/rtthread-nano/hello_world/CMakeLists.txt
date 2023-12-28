# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_RTTHREAD_NANO 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(demo)

sdk_inc(src)
sdk_inc(common)
sdk_app_src(src/demo.c)
generate_ses_project()
