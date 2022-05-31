# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREEMASTER 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(freemaster)
sdk_inc(src)
sdk_inc(src/common)

sdk_app_src(src/common/freemaster_example.c)
sdk_app_src(src/freemaster_uart.c)
generate_ses_project()
