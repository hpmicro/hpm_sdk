# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(qeo_dac_example)

sdk_inc(src)
sdk_app_src(src/mmc.c)
sdk_app_src(src/dac.c)
sdk_app_src(src/qeo_dac.c)
generate_ide_projects()
