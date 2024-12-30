/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SDMMC_PORT_H
#define SDMMC_PORT_H

/**
 *
 * @brief HPM SDMMC portable APIs
 * @defgroup hpm_sdmmc HPM SDMMC stack
 *  @ingroup hpm_sdmmc_interfaces
 * @{
 *
 */

#include "hpm_sdmmc_host.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize board-level SD Host parameters
 * @param [in,out] host Host context
 * @param [in] base SD Host base address
 *
 * @return Operation status
 */
hpm_stat_t board_init_sd_host_params(sdmmc_host_t *host, SDMMCHOST_Type *base);

/**
 * @brief Initialize board-level eMMC Host parameters
 * @param [in,out] host Host context
 * @param [in] base Host base address
 *
 * @return Operation status
 */
hpm_stat_t board_init_emmc_host_params(sdmmc_host_t *host, SDMMCHOST_Type *base);

/**
 * @brief Initialize board-level SDIO Host parameters
 * @param [in,out] host Host context
 * @param [in] base Host base address
 *
 * @return Operation status
 */
hpm_stat_t board_init_sdio_host_params(sdmmc_host_t *host, SDMMCHOST_Type *base);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* SDMMC_PORT_H */
