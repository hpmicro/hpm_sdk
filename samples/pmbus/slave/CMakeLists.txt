# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PMBUS  1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(pmbus_slave)

sdk_app_src(src/pmbus_slave.c)
generate_ide_projects()
