# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(plic_example)

# Uncomment the following line to enable IRQ handling at supervisor mode
#sdk_compile_definitions(-DUSE_S_MODE_IRQ=1)

# Uncomment the following line to enable IRQ handling at non-vector mode
#sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)

sdk_app_src(src/plic.c)
generate_ide_projects()
