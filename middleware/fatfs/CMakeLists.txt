# Copyright (c) 2021,2025 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(src/common)
sdk_src(src/common/ff.c)
sdk_src(src/common/ffunicode.c)

add_subdirectory_ifndef(CONFIG_FATFS_CUSTOM_PORTABLE src/portable)

if(DEFINED CONFIG_SDMMC)
sdk_compile_definitions(-DSD_FATFS_ENABLE=1)
endif()
