# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_RTTHREAD_NANO 1)
set(CONFIG_CMSIS_RTOS2 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(cmsis_os2_blinky_rtthread)

sdk_inc(.)
sdk_app_src(../src/blinky.c)
generate_ide_projects()

