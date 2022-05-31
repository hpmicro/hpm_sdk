# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_TINYCRYPT 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_compile_definitions(-DENABLE_TESTS)

project(ecc_dh_example)

sdk_app_src(src/ecc_dh_example.c)
sdk_app_src(src/ecc_utils.c)
generate_ses_project()
