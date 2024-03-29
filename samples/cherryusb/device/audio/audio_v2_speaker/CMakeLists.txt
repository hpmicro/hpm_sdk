# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_AUDIO 1)
#set(CONFIG_CODEC "sgtl5000")
#set(CONFIG_CODEC "wm8960")

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(cherryusb_device_audio_v2_speaker)

if(DEFINED CONFIG_CODEC)
sdk_compile_definitions(-DUSING_CODEC=1)
else()
sdk_compile_definitions(-DUSING_DAO=1)
endif()

sdk_compile_definitions(-DCONFIG_USBDEV_AUDIO_VERSION=0x0200)

sdk_inc(../../../config)
sdk_inc(src)
sdk_app_src(src/main.c)
sdk_app_src(src/audio_v2_speaker.c)

generate_ide_projects()
