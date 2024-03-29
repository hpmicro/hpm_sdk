# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.5)
set(CONFIG_TINYUSB 1)
set(CONFIG_TINYUSB_HOST 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
project(usb_host_hid_mouse_keyboard_example)
sdk_compile_definitions(-DCFG_TUSB_MCU=OPT_MCU_HPM)
sdk_compile_definitions(-DUSB_HOST_MCU_CORE=HPM_CORE0)
#sdk_compile_options("-Os")

sdk_inc(inc)
sdk_app_src(src/main.c)
sdk_app_src(src/hid_app.c)
generate_ide_projects()
