# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory_ifdef(CONFIG_SPI_NOR_FLASH interface/spi)

if(CONFIG_DMA_MGR)
sdk_compile_definitions(-DSERIAL_NOR_USE_DMA_MGR=1)
endif()

sdk_inc(.)
sdk_src(hpm_serial_nor.c)

