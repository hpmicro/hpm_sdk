# Copyright 2024 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PANEL 1)

set(STACK_SIZE 0x10000)
set(CONFIG_VGLITE 1)
set(CONFIG_FREERTOS 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(vglite_rotate)
sdk_ld_options("-lm")
sdk_compile_definitions(-DINIT_EXT_RAM_FOR_DATA=1)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)


sdk_inc(src)
sdk_app_src(src/main.c)
sdk_app_src(src/vglite_dbuf.c)

generate_ide_projects()