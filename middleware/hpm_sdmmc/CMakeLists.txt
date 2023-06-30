# Copyright (c) 2021-2022 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

sdk_inc(lib)

if(NOT DEFINED CONFIG_HPM_SPI_SDCARD)
sdk_inc(port/${HPM_SOC})
add_subdirectory(port/${HPM_SOC})
else()
sdk_compile_definitions(-DSD_SPI_ENABLE=1)
endif()

add_subdirectory(lib)
