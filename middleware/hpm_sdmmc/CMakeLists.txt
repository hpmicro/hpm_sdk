# Copyright (c) 2021-2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(lib)

if(DEFINED CONFIG_HPM_SPI_SDCARD)
sdk_compile_definitions(-DSD_SPI_ENABLE=1)
endif()

add_subdirectory(lib)

if(NOT DEFINED CONFIG_HPM_SPI_SDCARD)
add_subdirectory(port)
endif()
