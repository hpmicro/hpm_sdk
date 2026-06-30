# Copyright (c) 2025-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_SEGGER_RTT 1)
set(CONFIG_SEGGER_SYSVIEW 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(segger_sysview_test)

#sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)

# SEGGER_SYSVIEW_SECTION_FAST can't support in HPM6700
#sdk_compile_definitions(-DSEGGER_SYSVIEW_SECTION_FAST)

# Disable board timer callback to avoid IRQ conflict on SOCs with limited GPTMR channels
# shares the same IRQ as the sample timer, causing duplicate ISR definition)
sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)
sdk_compile_definitions(-DCONFIG_NDEBUG_CONSOLE=1)
sdk_ses_opt_lib_io_type(RTT)
sdk_ses_opt_debug_connection(J-Link)
sdk_app_src(src/main.c)
generate_ide_projects()