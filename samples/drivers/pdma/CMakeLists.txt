# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CUSTOM_LINKER_FILE $ENV{HPM_SDK_BASE}/samples/littlevgl/linkers/gcc/lvgl.ld)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(pdma_example)

sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)
sdk_app_src(src/pdma.c)
generate_ses_project()
