# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(pllctlv2_spreadspectrum)

sdk_app_src(src/pllctlv2.c)
generate_ide_projects()
