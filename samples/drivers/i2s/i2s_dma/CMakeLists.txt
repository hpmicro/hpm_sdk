# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED CONFIG_CODEC)
set(CONFIG_CODEC "sgtl5000")
endif()
set(CONFIG_HPM_CODEC 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(i2s_dma)

sdk_app_src(src/audio_codec.c)
generate_ses_project()
