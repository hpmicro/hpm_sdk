# Copyright 2021 hpmicro
# SPDX-Liceense-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dma_example)

sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)
sdk_app_src(src/dma.c)
generate_ses_project()
