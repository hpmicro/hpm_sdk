# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_EEPROM_EMULATION 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(eeprom_perf)

sdk_inc(.)
sdk_app_src(main.c)
generate_ide_projects()
