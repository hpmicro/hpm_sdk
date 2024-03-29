# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(qeo_pwm_example)

sdk_inc(src)
sdk_app_src(src/mmc.c)
sdk_app_src(src/pwm.c)
sdk_app_src(src/qeo_pwm.c)
generate_ide_projects()
