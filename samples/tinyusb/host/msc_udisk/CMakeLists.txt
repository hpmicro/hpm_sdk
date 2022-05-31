# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_TINYUSB 1)
set(CONFIG_USB_HOST 1)
set(CONFIG_FATFS 1)
set(CONFIG_USB_FATFS 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
sdk_compile_definitions(-DCFG_TUSB_MCU=OPT_MCU_HPM)
sdk_compile_definitions(-DUSB_HOST_MCU_CORE=HPM_CORE0)
sdk_compile_definitions(-DUSB_FATFS_ENABLE)
sdk_compile_options("-Os")

project(usb_host_example)
sdk_inc(inc)
sdk_app_src(src/main.c)
sdk_app_src(src/msc_app.c)
sdk_app_src(src/file_op.c)
generate_ses_project()
