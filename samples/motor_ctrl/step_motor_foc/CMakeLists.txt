# Copyright (c) 2021-2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)
set(CONFIG_MOTORCTRL_V2 1)
set(CONFIG_HPM_ADC 1)

set(RV_ABI "ilp32f")
set(RV_ARCH "rv32imafc")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(step_motor_foc)
sdk_app_inc(src)
sdk_app_src(src/step_motor_foc.c)
sdk_ld_options("-lm")
generate_ide_projects()