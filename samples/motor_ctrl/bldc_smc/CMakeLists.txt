# Copyright (c) 2023,2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)
set(CONFIG_MOTORCTRL 1)
set(CONFIG_HPM_ADC_V2 1)

# Enable segger j-scope
# set(CONFIG_SEGGER_RTT 1)
set(RV_ABI "ilp32f")
set(RV_ARCH "rv32imafc")
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(bldc_smc)
# Disable board timer callback to avoid IRQ conflict when
# BOARD_CALLBACK_TIMER and BOARD_BLDC_TMR both use the same GPTMR,
# which would cause duplicate default_isr definition at link time.
sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)
sdk_app_inc(src)
sdk_app_src(src/bldc_smc_callback.c)
sdk_app_src(src/bldc_smc.c)
sdk_ld_options("-lm")

generate_ide_projects()
