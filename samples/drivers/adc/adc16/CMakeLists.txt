# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(adc16_example)

#sdk_compile_definitions(-D__ADC16_USE_SW_TRIG)
sdk_app_src(src/adc16.c)
generate_ide_projects()
