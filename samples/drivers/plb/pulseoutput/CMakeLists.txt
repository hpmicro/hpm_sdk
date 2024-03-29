# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(pulseoutput)

sdk_app_src(src/pulseoutput.c)
generate_ide_projects()
