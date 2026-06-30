# Copyright (c) 2021,2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CODEC 1) # audio codec
#set(CONFIG_CODEC_NAME "wm8960") # specify audio codec name, default is auto-detected by board.yaml

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(audio_loopback)

sdk_app_src(src/audio_codec.c)
generate_ide_projects()
