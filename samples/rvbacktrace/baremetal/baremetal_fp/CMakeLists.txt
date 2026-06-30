# Copyright (c) 2024-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_RV_BACKTRACE true)
set(RV_BACKTRACE_USE_FP true)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(backtrace_bm)

# Disable board timer callback to avoid IRQ conflict on SOCs with limited GPTMR channels
# shares the same IRQ as the sample timer, causing duplicate ISR definition)
sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)
sdk_inc(src)
sdk_app_src(../baremetal.c)
generate_ide_projects()
