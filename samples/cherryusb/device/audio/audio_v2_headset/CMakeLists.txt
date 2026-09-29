# Copyright (c) 2026 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_AUDIO 1)

# CONFIG_CODEC is auto-detected by application.cmake from board feature board_codec_*.
# User can override by adding the following before find_package:
#   set(CONFIG_CODEC 1)   # force codec

# specify audio codec name, default is auto-detected by board.yaml
#   set(CONFIG_CODEC_NAME "wm8960")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(cherryusb_device_audio_v2_headset)

sdk_compile_definitions(-DCONFIG_USBDEV_AUDIO_VERSION=0x0200)
sdk_compile_definitions(-DBOARD_USE_AUDIO_PLL_FOR_EXACT_44100=1)

sdk_inc(../../../config)
sdk_inc(src)
sdk_app_src(src/main.c)
sdk_app_src(src/audio_v2_headset.c)

generate_ide_projects()
