# Copyright (c) 2024-2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_DMA_MGR 1)
set(CONFIG_CODEC 1) # audio codec
set(CONFIG_CODEC_NAME "wm8978") # specify audio codec name, default is auto-detected by board.yaml
set(CONFIG_HPM_I2S_OVER_SPI 1)
set(CONFIG_HPM_SPI 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(i2s_emulation_demo)

# Disable board timer callback to avoid IRQ conflict on SOCs with limited GPTMR channels
# shares the same IRQ as the sample timer, causing duplicate ISR definition)
sdk_compile_definitions(-DNO_BOARD_TIMER_SUPPORT=1)

sdk_inc(src)
sdk_app_src_glob(src/*.c)
generate_ide_projects()
