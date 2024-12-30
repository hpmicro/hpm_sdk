/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "wmhd_api.h"
#include "hpm_soc.h"


/* Prepare the SDIO base table */
static const SDXC_Type *hpm_sdio_base_table[] = {
#ifdef HPM_SDXC0
    HPM_SDXC0,
#endif
#ifdef HPM_SDXC1
    HPM_SDXC1,
#endif
};

static const uint32_t hpm_sdio_instances = ARRAY_SIZE(hpm_sdio_base_table);

void wmhd_config_hpm_sdio_instances(void)
{
    wmhd_sdio_config((void **)hpm_sdio_base_table, (uint8_t) hpm_sdio_instances);
}

int wmhd_lookup_hpm_sdio_index(void *sdio_base)
{
    int index = 0;
    while (index < hpm_sdio_instances) {
        if (sdio_base == hpm_sdio_base_table[index]) {
            break;
        }
        ++index;
    }
    if (index >= hpm_sdio_instances) {
        index = -1;
    }
    return index;
}