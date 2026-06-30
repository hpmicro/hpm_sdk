# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")

set(RV_ARCH "rv32imafc")
set(RV_ABI "ilp32f")
set(CONFIG_HPM_MATH 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(fast_math_func_test)
sdk_app_src(src/fast_math_func_test.c)
sdk_compile_options("-O3")
sdk_compile_options("-flto")
sdk_ld_options("-O3")

generate_ide_projects()