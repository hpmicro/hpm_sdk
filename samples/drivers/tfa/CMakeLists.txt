# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")

set(RV_ARCH "rv32imafc")
set(RV_ABI "ilp32f")

set(CONFIG_HPM_MATH 1)

set(CONFIG_TFA_ENABLE 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(tfa_example)
sdk_compile_definitions(-DCONFIG_TFA_ENABLE=1)

sdk_app_src(src/tfa_demo.c)
generate_ide_projects()
