# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CUSTOM_LINKER_FILE $ENV{HPM_SDK_BASE}/samples/rom_api/otp_blow_plan/linkers/gcc/ram.ld)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(otp_api)

sdk_app_inc(src)
sdk_app_src(src/main.c)
generate_ses_project()
