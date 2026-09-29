# Copyright (c) 2021,2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_AUDIO 1)
# CONFIG_CODEC is auto-detected when app.yaml declares board_audio_codec and the
# board declares both board_audio_codec and board_codec_*.
# User can override by adding one of the following before find_package:
#   set(CONFIG_CODEC 1)   # force codec
#   set(CONFIG_CODEC 0)   # force PDM

# specify audio codec name, default is auto-detected by board.yaml
#   set(CONFIG_CODEC_NAME "wm8960")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(cherryusb_device_audio_v2_mic_adaptive)

if(CONFIG_CODEC)
sdk_compile_definitions(-DUSING_CODEC=1)
else()
sdk_compile_definitions(-DUSING_PDM=1)
endif()

sdk_compile_definitions(-DCONFIG_USBDEV_AUDIO_VERSION=0x0200)

sdk_inc(../../../config)
sdk_inc(src)
sdk_app_src(src/main.c)
sdk_app_src(src/audio_v2_mic_adaptive.c)

generate_ide_projects()
