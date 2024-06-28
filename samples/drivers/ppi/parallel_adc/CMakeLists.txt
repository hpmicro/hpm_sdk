# Copyright (c) 2024 HPMicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PPI 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(ppi_parallel_adc_example)

sdk_app_src(src/parallel_adc.c)
generate_ide_projects()
