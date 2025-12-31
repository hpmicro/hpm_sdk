# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(SES_TOOLCHAIN_VARIANT "Andes")

set(RV_ABI "ilp32d")
set(RV_ARCH "rv32gc")

set(CONFIG_COREMARK 1)
if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dualcore_console_coremark)

sdk_compile_definitions(-DMAIN_ENTRY=app_main)

sdk_inc(src/port)
sdk_inc(src)
sdk_inc(../../common)

sdk_app_src(../../common/multicore_common.c)
sdk_app_src(src/demo_dualcore_coremark.c
        src/sec_core_img.c
        src/port/core_portme.c
        )

generate_ide_projects()
