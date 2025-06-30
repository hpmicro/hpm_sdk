# Copyright (c) 2021-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_FREERTOS 1)

#Set CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR to use GPTMR as system's tick source
#set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)

#Set CONFIG_FREERTOS_TIMER_RESOURCE_PWM to use PWM/PWMV2 as system's tick source
#set(CONFIG_FREERTOS_TIMER_RESOURCE_PWM 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(freertos_hello)

sdk_inc(src)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_app_src(src/freertos_hello.c)
generate_ide_projects()
