# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")

set(CONFIG_HPM_MATH 1)
set(HPM_MATH_SES_LIB "libdspf")
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dsp_demo)

sdk_ses_compile_options(-mabi=ilp32f)
sdk_ses_compile_options(-march=rv32imafc)

sdk_app_src(src/dsp_demo.c)
generate_ses_project()
