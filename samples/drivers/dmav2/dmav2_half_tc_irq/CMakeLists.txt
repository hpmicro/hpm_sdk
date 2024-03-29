# Copyright (c) 2021 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dmav2_irq_example)

sdk_app_src(src/dmav2_half_tc_irq.c)
generate_ide_projects()
