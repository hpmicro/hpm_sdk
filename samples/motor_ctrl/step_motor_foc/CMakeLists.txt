# Copyright (c) 2021-2023,2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)
set(CONFIG_MOTORCTRL_V2 1)
set(CONFIG_HPM_ADC_V2 1)

set(RV_ABI "ilp32f")
set(RV_ARCH "rv32imafc")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(step_motor_foc)
# Disable board timer callback to avoid IRQ conflict when
# BOARD_CALLBACK_TIMER and BOARD_BLDC_TMR both use the same GPTMR,
# which would cause duplicate default_isr definition at link time.
sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)
sdk_compile_definitions(-DCONFIG_MCL_HAS_EXTRA_CONFIG="mcl_app_config.h")
sdk_inc(src)
sdk_app_src(src/step_motor_foc.c)
generate_ide_projects()