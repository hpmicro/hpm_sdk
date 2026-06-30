# Copyright (c) 2021-2024,2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(adc16_example)

#sdk_compile_definitions(-D__ADC16_USE_SW_TRIG)
#sdk_compile_definitions(-DAPP_ADC16_TRIG_SRC_FREQUENCY=20000)
#sdk_compile_definitions(-DAPP_ADC16_CH_ADC_LOOP_EXP=0)
#sdk_compile_definitions(-DAPP_ADC16_ENABLE_LOOP_EXP_LOG=1)
#sdk_compile_definitions(-DAPP_ADC16_CAPTURE_MAX=1000)
#sdk_compile_definitions(-DAPP_ADC16_ENABLE_SEQ_DMA_16BIT=1)
sdk_app_src(src/adc16.c)
generate_ide_projects()
