# Copyright 2023 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(STACK_SIZE 0x10000)
set(CONFIG_VGLITE 1)
set(CONFIG_FREERTOS 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

set(CONFIG_HPM_PANEL 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(vglite_tiger)
sdk_ld_options("-lm")
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)


sdk_inc(src)
sdk_app_src(src/main.c)
sdk_app_src(src/lcdc_panel.c)

generate_ide_projects()