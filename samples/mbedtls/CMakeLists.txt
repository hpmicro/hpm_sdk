# Copyright (c) 2021-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)
set(CONFIG_MBEDTLS 1)
set(CONFIG_MBEDTLS_USE_HPM_SDP 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

sdk_inc(inc)
sdk_compile_definitions(-DMBEDTLS_CONFIG_FILE="hpm_sdk_mbedtls_config.h")

project(mebedtls)
sdk_app_src(src/mbedtls_test.c)

generate_ide_projects()
