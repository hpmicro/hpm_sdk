# Copyright (c) 2024-2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PANEL 1)

# select target storage
set(PNG_USE_SDCARD 1)
# set(PNG_USE_UDISK 1)

set(CONFIG_LODEPNG 1)
set(CONFIG_HPM_CAMERA 1)

# camera
if(NOT DEFINED CONFIG_CAMERA)
  set(CONFIG_CAMERA "ov7725")
endif()

set(CONFIG_FATFS 1)
if(DEFINED PNG_USE_SDCARD)
  set(CONFIG_SDMMC 1)
elseif(DEFINED PNG_USE_UDISK)
  set(CONFIG_TINYUSB 1)
  set(CONFIG_TINYUSB_HOST 1)
  set(CONFIG_USB_FATFS 1)
  set(CONFIG_USB_FATFS_TINYUSB 1)
else()
  message(FATAL_ERROR "Please select target storaget, set PNG_USE_SDCARD or PNG_USE_UDISK in CMakeLists.txt")
endif()

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

set(HEAP_SIZE 0x600000)
set(STACK_SIZE 0x20000)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(png_enc_example)

if(DEFINED PNG_USE_SDCARD)
  sdk_compile_definitions(-DPNG_USE_SDCARD=1)
  sdk_compile_definitions(-DSD_FATFS_ENABLE=1)
  sdk_app_src(../common/src/sd_fatfs.c)
elseif(DEFINED PNG_USE_UDISK)
  sdk_compile_definitions(-DPNG_USE_UDISK=1)
  sdk_compile_definitions(-DCFG_TUSB_MCU=OPT_MCU_HPM)
  sdk_compile_definitions(-DUSB_HOST_MCU_CORE=BOARD_RUNNING_CORE)
  sdk_compile_definitions(-DUSB_FATFS_ENABLE=1)
  sdk_app_src(../common/src/msc_app.c)
endif()

sdk_inc(../common/inc)
sdk_app_src(../common/src/file_op.c)
sdk_app_src(../common/src/format_convert.c)
sdk_app_src(src/png_encode.c)

generate_ide_projects()
