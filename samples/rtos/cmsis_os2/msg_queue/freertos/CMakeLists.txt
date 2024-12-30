# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)
set(CONFIG_CMSIS_RTOS2 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(cmsis_os2_msg_queue_freertos)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_inc(.)
sdk_app_src(../src/main.c)

generate_ide_projects()

