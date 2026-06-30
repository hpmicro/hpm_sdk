# Copyright (c) 2023-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

#use dma for pwm measure, but only support XDMA
#if the soc not support XMDA, the defined value is useless
sdk_compile_definitions(-DCONFIG_USE_DMA=1)
# Disable board timer callback to avoid IRQ conflict on SOCs with limited GPTMR channels
# shares the same IRQ as the sample timer, causing duplicate ISR definition)
sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)

project(gptmr_pwm_measure)

sdk_app_src(src/pwm_measure.c)
generate_ide_projects()
