# Copyright (c) 2024 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

set(CONFIG_HPM_PANEL 1)
set(CONFIG_HPM_JPEG 1)

# target storage
set(JPEG_USE_SDCARD 1)
#set(JPEG_USE_UDISK 1)

# target storage
set(CONFIG_FATFS 1)
if(DEFINED JPEG_USE_SDCARD)
  set(CONFIG_SDMMC 1)
elseif(DEFINED JPEG_USE_UDISK)
  set(CONFIG_TINYUSB 1)
  set(CONFIG_TINYUSB_HOST 1)
  set(CONFIG_USB_FATFS 1)
  set(CONFIG_USB_FATFS_TINYUSB 1)
else()
  message(FATAL_ERROR "Please select target storaget, set JPEG_USE_SDCARD or JPEG_USE_UDISK in CMakeLists.txt")
endif()

if("${HPM_BUILD_TYPE}" STREQUAL "")
    SET(HPM_BUILD_TYPE flash_sdram_xip)
endif()

set(HEAP_SIZE 0x400000)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(hpm_jpeg)

if(DEFINED JPEG_USE_SDCARD)
  sdk_compile_definitions(-DJPEG_USE_SDCARD=1)
  sdk_compile_definitions(-DSD_FATFS_ENABLE=1)
  sdk_app_src(../common/src/sd_fatfs.c)
elseif(DEFINED JPEG_USE_UDISK)
  sdk_compile_definitions(-DJPEG_USE_UDISK=1)
  sdk_compile_definitions(-DCFG_TUSB_MCU=OPT_MCU_HPM)
  sdk_compile_definitions(-DUSB_HOST_MCU_CORE=BOARD_RUNNING_CORE)
  sdk_compile_definitions(-DUSB_FATFS_ENABLE=1)
  sdk_app_src(../common/src/msc_app.c)
endif()


sdk_inc(../common/inc)
sdk_app_src(../common/src/file_op.c)
sdk_app_src(src/main.c)

generate_ide_projects()
