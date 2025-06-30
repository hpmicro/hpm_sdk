/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "wmhd_api.h"
#include "hpm_soc.h"

#ifdef __SEGGER_RTL_VERSION
#include <ctype.h>
__attribute__((used)) int _impure_ptr; /* Workaround for fixing compiling error with SES */
/* Workaround for fixing compiling error that the _ctype_ array is missing in SES toolchain */
const char _ctype_[] = { 00,
                         20, 20, 20, 20, 20, 20, 20, 20, 20, 28, 28, 28, 28, 28, 20, 20,
                         20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
                         88, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
                         04, 04, 04, 04, 04, 04, 04, 04, 04, 04, 10, 10, 10, 10, 10, 10,
                         10, 41, 41, 41, 41, 41, 41, 01, 01, 01, 01, 01, 01, 01, 01, 01,
                         01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 01, 10, 10, 10, 10, 10,
                         10, 42, 42, 42, 42, 42, 42, 02, 02, 02, 02, 02, 02, 02, 02, 02,
                         02, 02, 02, 02, 02, 02, 02, 02, 02, 02, 02, 10, 10, 10, 10, 20
};
#endif
#if defined(__zcc__)
__attribute__((used)) int _impure_ptr; /* Workaround for fixing compiling error with zcc */
#endif

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
    while (index < (int)hpm_sdio_instances) {
        if (sdio_base == hpm_sdio_base_table[index]) {
            break;
        }
        ++index;
    }
    if (index >= (int)hpm_sdio_instances) {
        index = -1;
    }
    return index;
}