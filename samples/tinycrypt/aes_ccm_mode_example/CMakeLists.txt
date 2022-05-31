# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_TINYCRYPT 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(aes_ccm_mode_example)

sdk_app_src(src/aes_ccm_mode_example.c)
generate_ses_project()
