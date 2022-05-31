# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.13)

#Only one of them can be selected to be defined(1->CONFIG_SDFATFS_MODE:sd card;2->CONFIG_TINYUSBHOST_MODE:tinyusb)
set(CONFIG_SDFATFS_MODE 1)
#set(CONFIG_TINYUSBHOST_MODE 1)

if(NOT DEFINED CONFIG_CAMERA)
set(CONFIG_CAMERA "ov7725")
endif()
set(CONFIG_HPM_CAMERA 1)

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

project(libjpeg_cjpeg_example)

sdk_compile_definitions(COMPRESS_MODE)

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

sdk_compile_options("-Os")

sdk_inc(../common/inc)

if(DEFINED CONFIG_SDFATFS_MODE)
sdk_app_src(../common/src/sd_fatfs.c)
endif()

if(DEFINED CONFIG_TINYUSBHOST_MODE)
sdk_app_src(../common/src/msc_app.c)
endif()

sdk_app_src(src/cjpeg.c)
sdk_app_src(../common/src/file_op.c)

generate_ses_project()
