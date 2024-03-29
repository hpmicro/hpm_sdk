# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
sdk_ld_options("-lm")
project(pwm_cap_example)

sdk_app_src(src/pwm.c)
generate_ide_projects()
