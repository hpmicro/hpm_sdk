# Copyright (c) 2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_EEPROM_HAL_EMULATION 1)

if (CONFIG_EEPROM_HAL_EMULATION)
set (CONFIG_EEPROM_EMULATION 1)
endif ()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(eeprom_hal_emulation)

sdk_inc(.)
sdk_app_src(main.c)
generate_ide_projects()
