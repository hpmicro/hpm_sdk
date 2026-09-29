# Copyright (c) 2024,2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

# CONFIG_CODEC is auto-detected when app.yaml declares board_audio_codec and the
# board declares both board_audio_codec and board_codec_*.
# User can override by adding one of the following before find_package:
#   set(CONFIG_CODEC 1)   # force codec
#   set(CONFIG_CODEC 0)   # force DAO

# specify audio codec name, default is auto-detected by board.yaml
#   set(CONFIG_CODEC_NAME "wm8960")
set(CONFIG_SDMMC 1)
set(CONFIG_FATFS 1)
set(CONFIG_MINIMP3 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

set(STACK_SIZE 0x10000)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(decoder_mp3)

if(CONFIG_CODEC)
sdk_compile_definitions(-DUSING_CODEC=1)
else()
sdk_compile_definitions(-DUSING_DAO=1)
endif()
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)
sdk_compile_definitions(-DFF_CODE_PAGE=437)
sdk_compile_definitions(-DBOARD_USE_AUDIO_PLL_FOR_EXACT_44100=1)

sdk_app_src(src/decoder_mp3.c)

sdk_inc(../common/inc)
sdk_app_src(../common/src/sd_fatfs_common.c)
sdk_app_src(../common/src/audio_codec_common.c)
sdk_app_src(../common/src/mp3_ctrl_common.c)

generate_ide_projects()
