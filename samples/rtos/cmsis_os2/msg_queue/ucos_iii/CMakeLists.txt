# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_UCOS_III 1)
set(CONFIG_CMSIS_RTOS2 1)

set(HEAP_SIZE 0x8000)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(cmsis_os2_msg_queue_ucos_iii)

sdk_compile_definitions(-D__ucos_iii_irq_stack_top=_stack)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_compile_definitions(-DCONFIG_UCOS_III=1)

sdk_inc(./ucos_cfg)
sdk_app_src(./ucos_cfg/os_app_hooks.c)
sdk_app_src(../src/main.c)

generate_ide_projects()

