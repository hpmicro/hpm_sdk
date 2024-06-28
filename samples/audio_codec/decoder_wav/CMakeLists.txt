# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED CONFIG_CODEC)
# set(CONFIG_CODEC "sgtl5000")
# set(CONFIG_CODEC "wm8960")
endif()
set(CONFIG_SDMMC 1)
set(CONFIG_FATFS 1)

set(CONFIG_AUDIO_CODEC 1)
set(CONFIG_AUDIO_DECODER_WAV 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(decoder_wav)

if(DEFINED CONFIG_CODEC)
sdk_compile_definitions(-DUSING_CODEC=1)
else()
sdk_compile_definitions(-DUSING_DAO=1)
endif()
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)
sdk_compile_definitions(-DFF_CODE_PAGE=437)

sdk_app_src(src/audio_codec.c)

sdk_inc(../common/inc)
sdk_app_src(../common/src/audio_codec_common.c)
sdk_app_src(../common/src/sd_fatfs_common.c)
sdk_app_src(../common/src/wav_ctrl_common.c)

generate_ide_projects()
