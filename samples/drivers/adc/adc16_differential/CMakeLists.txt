# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(adc16_differential_example)

#sdk_compile_definitions(-DAPP_ADC16_TRIG_SRC_FREQUENCY=20000)
sdk_app_src(src/adc16_differential.c)
generate_ide_projects()
