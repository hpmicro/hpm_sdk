# Copyright (c) 2023 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ewdg_example)

sdk_app_src(src/ewdg.c)
generate_ide_projects()
