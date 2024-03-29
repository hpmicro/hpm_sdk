# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_ECLIPSE_THREADX 1)
set(CONFIG_ECLIPSE_THREADX_GUIX 1)
set(CONFIG_ECLIPSE_THREADX_GUIX_565RGB 1)
set(CONFIG_ECLIPSE_THREADX_GUIX_DEMO car_infotainment)
set(STACK_SIZE 0x10000)

set(CONFIG_HPM_PANEL 1)
set(CONFIG_HPM_TOUCH 1)
if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt911")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(demo_guix_car_infotainment)

sdk_compile_definitions(-D__threadx_irq_stack_top=_stack)
sdk_compile_definitions(-DCONFIG_THREADX=1)
sdk_compile_options("-O3")

sdk_inc(src)
sdk_app_src(src/guix_demo.c)

generate_ide_projects()
