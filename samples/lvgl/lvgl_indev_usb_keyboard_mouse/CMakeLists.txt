# Copyright (c) 2025  HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PANEL 1)
# ENABLE LVGL
set(CONFIG_LVGL 1)
# ENABLE FREERTOS
set(CONFIG_FREERTOS 1)
set(HEAP_SIZE 0x8000)
set(STACK_SIZE 0x10000)
# ENABLE USB HID FOR CHERRYUSB
set(CONFIG_CHERRYUSB 1)
set(CONFIG_USB_HOST_HID 1)
set(CONFIG_USB_HOST_HID_LVGL 1)

if(NOT DEFINED CONFIG_TOUCH)
set(CONFIG_TOUCH "gt9xx")
endif()
set(CONFIG_HPM_TOUCH 1)

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(lvgl_indev_usb_keyboard_mouse)

sdk_compile_definitions(-DUSE_NONVECTOR_MODE=1)
sdk_compile_definitions(-DDISABLE_IRQ_PREEMPTIVE=1)
sdk_ld_options("-lm")
sdk_compile_definitions(-DLVGL_MAJOR_VERSION=9)
# LVGL CONF
sdk_compile_definitions(-DLV_USE_OS=LV_OS_FREERTOS)
sdk_compile_definitions(-DLV_USE_SYSMON=1)
sdk_compile_definitions(-DLV_USE_HPM_MODE_DIRECT=0)
sdk_compile_definitions(-DLV_USE_HPM_PDMA_FLUSH=0)
sdk_compile_definitions(-DLV_USE_HPM_PDMA_WAIT_VSYNC=0)

sdk_compile_definitions(-DLVGL_USB_MOUSE=1)
sdk_compile_definitions(-DLVGL_USB_KEYBOARD=1)

sdk_inc(inc)
sdk_app_src(src/main.c)

generate_ide_projects()

