# Copyright 2021 hpmicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dram_example)

sdk_app_src(src/dram.c)
generate_ses_project()
