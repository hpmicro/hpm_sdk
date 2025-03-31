# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_RTTHREAD_NANO 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(rtthread_nano_hello)

sdk_inc(src)
sdk_inc(common)
sdk_app_src(src/rtthread_nano_hello.c)
generate_ide_projects()
