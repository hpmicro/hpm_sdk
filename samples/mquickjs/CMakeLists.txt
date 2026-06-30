# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
#
# mquickjs example for HPM SDK

cmake_minimum_required(VERSION 3.13)

# Enable mquickjs middleware
set(CONFIG_MQUICKJS 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(mquickjs_example)

# Link math library for mquickjs (fabs, etc.)
sdk_ld_options(-lm)

# Add application source
sdk_app_src(src/main.c)

# Generate IDE project files
generate_ide_projects()

