# Copyright 2022 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

#Read file mode:
#Only one of them can be selected to be defined(1->CONFIG_SDFATFS_MODE:sd card;2->CONFIG_TINYUSBHOST_MODE:tinyusb)
set(CONFIG_SDFATFS_MODE 1)
#set(CONFIG_TINYUSBHOST_MODE 1)

#Decoding mode:
#Only one of them can be selected to be defined(1->CONFIG_JPEG_TURBO_MODE:libjpeg_turbo;2->CONFIG_JPEG_HARDWARE_MODE:Hardware JPEG)
set(CONFIG_JPEG_TURBO_MODE 1)
#set(CONFIG_JPEG_HARDWARE_MODE 1)

if(DEFINED CONFIG_SDFATFS_MODE)
set(CONFIG_SDMMC 1)
endif()

if(DEFINED CONFIG_TINYUSBHOST_MODE)
set(CONFIG_TINYUSB 1)
set(CONFIG_USB_HOST 1)
set(CONFIG_USB_FATFS 1)
endif()

set(CONFIG_FATFS 1)
set(CONFIG_LIBJPEG 1)
set(CUSTOM_LINKER_FILE $ENV{HPM_SDK_BASE}/samples/jpeg/linkers/gcc/jpeg.ld)

set(HEAP_SIZE 0x40000)
set(STACK_SIZE 0x40000)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})

project(libjpeg_djpeg_example)

sdk_compile_definitions(DECOMPRESS_MODE)

if(DEFINED CONFIG_SDFATFS_MODE)
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)
sdk_compile_definitions(SD_FATFS_MODE)
endif()

if(DEFINED CONFIG_TINYUSBHOST_MODE)
sdk_compile_definitions(-DCFG_TUSB_MCU=OPT_MCU_HPM)
sdk_compile_definitions(-DUSB_HOST_MCU_CORE=BOARD_RUNNING_CORE)
sdk_compile_definitions(-DUSB_FATFS_ENABLE)
sdk_compile_definitions(TINYUSB_HOST_MODE)
endif()

if(DEFINED CONFIG_JPEG_TURBO_MODE)
sdk_compile_definitions(JPEG_TURBO_MODE)
endif()

if(DEFINED CONFIG_JPEG_HARDWARE_MODE)
sdk_compile_definitions(JPEG_HARDWARE_MODE)
endif()

sdk_compile_options(-Os)

sdk_inc(../common/inc)
sdk_inc(inc)

if(DEFINED CONFIG_SDFATFS_MODE)
sdk_app_src(../common/src/sd_fatfs.c)
endif()

if(DEFINED CONFIG_TINYUSBHOST_MODE)
sdk_app_src(../common/src/msc_app.c)
endif()

if(DEFINED CONFIG_JPEG_TURBO_MODE)
sdk_app_src(src/jpeg_turbo.c)
endif()

if(DEFINED CONFIG_JPEG_HARDWARE_MODE)
sdk_app_src(src/jpeg_hw.c)
endif()

sdk_app_src(src/djpeg.c)
sdk_app_src(../common/src/file_op.c)

generate_ses_project()
