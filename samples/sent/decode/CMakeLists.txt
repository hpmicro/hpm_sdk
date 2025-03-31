# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

# the collection of sent signal is using DMA
# so should be have interval to check whether the remaining transfer size of DMA has changed
# if there is no change, it means that a collection of sent signal is complete
sdk_compile_definitions(-DCONFIG_SENT_IDLE_INTERVAL_US=1000)

project(gptmr_sent_decode_demo)
sdk_ld_options("-lm")
sdk_app_src(src/sent_signal_decode.c)
generate_ide_projects()
