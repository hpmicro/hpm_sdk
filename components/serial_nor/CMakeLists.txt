# Copyright (c) 2023 HPMicro
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory_ifdef(CONFIG_SPI_NOR_FLASH interface/spi)

sdk_inc(.)
sdk_src(hpm_serial_nor.c)

