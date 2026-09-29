# Copyright (c) 2024-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")

set(RV_ARCH "rv32imafc")
set(RV_ABI "ilp32f")

set(CONFIG_HPM_MATH 1)
set(CONFIG_TFA_NOT_USE_IRQ_NEST_PROTECTION 0)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
if(CONFIG_TFA_NOT_USE_IRQ_NEST_PROTECTION)
    sdk_compile_definitions(CONFIG_TFA_NOT_USE_IRQ_NEST_PROTECTION)
endif()

project(tfa_example)

sdk_app_src(src/tfa_demo.c)

generate_ide_projects()
