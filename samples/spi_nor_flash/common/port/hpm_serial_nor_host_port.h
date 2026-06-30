/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _BOARD_SPI_OPS_H
#define _BOARD_SPI_OPS_H

#include "hpm_serial_nor_host.h"
#include "hpm_spi_drv.h"

hpm_stat_t serial_nor_get_board_host(hpm_serial_nor_host_t *host);
void serial_nor_spi_pins_init(SPI_Type *spi);
#endif
