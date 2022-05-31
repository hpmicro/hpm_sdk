# Copyright 2022 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED CONFIG_CODEC)
set(CONFIG_CODEC "sgtl5000")
endif()
set(CONFIG_SDMMC 1)
set(CONFIG_FATFS 1)

set(CONFIG_AUDIO_CODEC 1)
set(CONFIG_AUDIO_DECODER_WAV 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(audio_codec)

sdk_compile_definitions(-DSD_FATFS_ENABLE=1)

sdk_app_src(src/audio_codec.c)
generate_ses_project()
