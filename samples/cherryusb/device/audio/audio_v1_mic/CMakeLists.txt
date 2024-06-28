# Copyright (c) 2021 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_AUDIO 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(cherryusb_device_audio_v1_mic)

sdk_inc(../../../config)
sdk_inc(src)
sdk_app_src(src/main.c)
sdk_app_src(src/audio_v1_mic.c)

generate_ide_projects()
