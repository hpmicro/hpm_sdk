# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

if(NOT DEFINED CONFIG_CODEC)
# set(CONFIG_CODEC "sgtl5000")
set(CONFIG_CODEC "wm8960")
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(audio_loopback)

sdk_app_src(src/audio_codec.c)
generate_ide_projects()
