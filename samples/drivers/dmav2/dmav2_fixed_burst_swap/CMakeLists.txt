# Copyright (c) 2024 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dmav2_fixed_burst_swap_example)

sdk_app_src(src/dmav2_fixed_burst_swap.c)

generate_ide_projects()
