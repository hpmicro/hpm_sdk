# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PLB_QEI_ENCODER 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(qei_encoder)

sdk_app_src(src/qei_encoder.c)
generate_ide_projects()
