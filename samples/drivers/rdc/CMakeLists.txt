# Copyright (c) 2023 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(RV_ABI "ilp32f")
set(RV_ARCH "rv32imafc")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(rdc_example)

sdk_app_src(src/rdc.c)
sdk_ld_options("-lm")
sdk_compile_options("-O3")
generate_ide_projects()
