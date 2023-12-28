/*
 * Copyright (c) 2021-2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SDMMC_PORT_H
#define SDMMC_PORT_H

#include "hpm_sdmmc_host.h"

hpm_stat_t board_init_sd_host_params(sdmmc_host_t *host, SDMMCHOST_Type *base);
hpm_stat_t board_init_emmc_host_params(sdmmc_host_t *host, SDMMCHOST_Type *base);

#endif /* SDMMC_PORT_H */
