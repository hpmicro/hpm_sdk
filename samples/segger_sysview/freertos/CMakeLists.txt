# Copyright (c) 2021-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_SEGGER_RTT 1)
set(CONFIG_SEGGER_SYSVIEW 1)
set(CONFIG_FREERTOS 1)

#Set CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR to use GPTMR as system's tick source
#set(CONFIG_FREERTOS_TIMER_RESOURCE_GPTMR 1)

#Set CONFIG_FREERTOS_TIMER_RESOURCE_PWM to use PWM/PWMV2 as system's tick source
#set(CONFIG_FREERTOS_TIMER_RESOURCE_PWM 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(segger_sysview_test)

# Disable board timer callback to avoid IRQ conflict on SOCs with limited GPTMR channels
# shares the same IRQ as the sample timer, causing duplicate ISR definition)
sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)
sdk_compile_definitions(-DCONFIG_NDEBUG_CONSOLE=1)
sdk_ses_opt_lib_io_type(RTT)
sdk_ses_opt_debug_connection(J-Link)

sdk_inc(src)
sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_app_src(src/main.c)
generate_ide_projects()
