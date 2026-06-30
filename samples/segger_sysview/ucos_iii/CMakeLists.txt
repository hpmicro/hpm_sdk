# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_SEGGER_RTT 1)
set(CONFIG_SEGGER_SYSVIEW 1)
set(CONFIG_UCOS_III 1)

#set(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR 1) to use gptmr as system's tick source
#set(CONFIG_UCOS_III_TIMER_RESOURCE_GPTMR 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(segger_sysview_ucos_iii)

# Disable board timer callback to avoid IRQ conflict on SOCs with limited GPTMR channels
# shares the same IRQ as the sample timer, causing duplicate ISR definition)
sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)
sdk_compile_definitions(-DCONFIG_NDEBUG_CONSOLE=1)
sdk_ses_opt_lib_io_type(RTT)
sdk_ses_opt_debug_connection(J-Link)

sdk_inc(src)
sdk_inc(inc)
sdk_inc(ucos_cfg)

sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)

sdk_app_src(src/main.c)
sdk_app_src(src/start_task.c)
sdk_app_src(src/user_tasks.c)
sdk_app_src(ucos_cfg/os_app_hooks.c)

generate_ide_projects()