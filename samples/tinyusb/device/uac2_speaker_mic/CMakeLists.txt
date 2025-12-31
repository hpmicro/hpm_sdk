# Copyright (c) 2021-2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_TINYUSB 1)
set(CONFIG_USB_DEVICE 1)
set(CONFIG_USB_DEVICE_AUDIO 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(tinyusb_device_uac2_speaker_mic_example)

sdk_inc(src)

sdk_app_src(src/main.c)
sdk_app_src(src/usb_descriptors.c)

generate_ide_projects()
