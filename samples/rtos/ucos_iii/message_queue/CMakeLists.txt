# Copyright (c) 2023-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_UCOS_III 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(ucos_iii_message_queue)

sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_inc(inc)
sdk_inc(../ucos_cfg)

sdk_app_src(src/main.c)
sdk_app_src(src/led_task.c)
sdk_app_src(src/start_task.c)
sdk_app_src(src/float_task.c)
sdk_app_src(../ucos_cfg/os_app_hooks.c)

generate_ide_projects()
