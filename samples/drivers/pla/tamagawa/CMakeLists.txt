# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)
set(SES_TOOLCHAIN_VARIANT "Andes")
set(CONFIG_HPM_MATH 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(pla_tamagawa)

sdk_app_inc(protocol)
sdk_app_src(src/tamagawa.c)
sdk_app_src(protocol/tamagawa_proc.c)
generate_ide_projects()
