# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(BUILD_FOR_SECONDARY_CORE 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(mbx_example_core1)

sdk_app_src(../src/mbx.c)
generate_ses_project()
