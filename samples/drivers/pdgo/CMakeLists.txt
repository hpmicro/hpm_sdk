# Copyright (c) 2023-2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)


if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_xip)
endif()


find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(dgo_demo)

sdk_app_inc(./src)
sdk_app_src(src/main.c)

if("${HPM_SOC}" MATCHES "^HPM53")
    sdk_app_src(src/hpm53xx/dgo_demo.c)
elseif("${HPM_SOC}" STREQUAL "HPM6E80")
    sdk_app_src(src/hpm6exx/dgo_demo.c)
elseif("${HPM_SOC}" STREQUAL "HPM6P81")
    sdk_app_src(src/hpm6pxx/dgo_demo.c)
elseif("${HPM_SOC}" STREQUAL "HPM5E31")
    sdk_app_src(src/hpm5exx/dgo_demo.c)
endif()

generate_ide_projects()
