# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ffa_demo)

sdk_app_src(src/ffa_demo.c
            src/ffa_test_data.c)
generate_ide_projects()
