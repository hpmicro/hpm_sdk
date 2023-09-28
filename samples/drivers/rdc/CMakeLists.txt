# Copyright (c) 2023 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_nds_compile_options("-mext-dsp")
sdk_ld_options("-lm")
project(rdc_example)

sdk_app_src(src/rdc.c)
generate_ses_project()
