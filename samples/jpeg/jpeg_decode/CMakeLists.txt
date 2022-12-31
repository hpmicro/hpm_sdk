# Copyright (c) 2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

# target storage
set(JPEG_USE_SDCARD 1)
#set(JPEG_USE_UDISK 1)

# set JPEG_HW_MODE to 0 to use software library
set(JPEG_HW_MODE 1)

# target storage
set(CONFIG_FATFS 1)
if(DEFINED JPEG_USE_SDCARD)
  set(CONFIG_SDMMC 1)
elseif(DEFINED JPEG_USE_UDISK)
  set(CONFIG_TINYUSB 1)
  set(CONFIG_USB_HOST 1)
  set(CONFIG_USB_FATFS 1)
  set(CONFIG_USB_FATFS_TINYUSB 1)
else()
  message(FATAL_ERROR "Please select target storaget, set JPEG_USE_SDCARD or JPEG_USE_UDISK in CMakeLists.txt")
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE flash_sdram_xip)
endif()

if(${JPEG_HW_MODE})
  set(HEAP_SIZE 0x40000)
else()
  set(CONFIG_LIBJPEG 1)
  set(HEAP_SIZE 0x100000)
endif()
set(STACK_SIZE 0x40000)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(jpeg_dec_example)

sdk_compile_definitions(INIT_EXT_RAM_FOR_DATA=1)

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

if(${JPEG_HW_MODE})
  sdk_compile_definitions(-DJPEG_HW_MODE=1)
  sdk_app_src(src/jpeg_hw.c)
else()
  sdk_compile_definitions(DECOMPRESS_MODE)
  sdk_compile_definitions(-DJPEG_HW_MODE=0)
  sdk_app_src(src/jpeg_turbo.c)
endif()

sdk_compile_options(-Os)

sdk_inc(../common/inc)
sdk_inc(inc)
sdk_app_src(src/djpeg.c)

generate_ses_project()
