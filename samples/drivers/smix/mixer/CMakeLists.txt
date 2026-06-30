# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CODEC 1) # audio codec

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(mixer)

sdk_inc(src)
sdk_app_src(src/mixer.c)
generate_ide_projects()
