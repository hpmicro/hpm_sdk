# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_DMA_MGR 1)
set(CONFIG_CODEC "wm8978")
set(CONFIG_HPM_I2S_OVER_SPI 1)
set(CONFIG_HPM_SPI 1)
find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(i2s_emulation_demo)

sdk_inc(src)
sdk_app_src_glob(src/*.c)
generate_ide_projects()
