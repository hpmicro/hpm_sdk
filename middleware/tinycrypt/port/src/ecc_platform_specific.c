/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_common.h"
#include "hpm_soc.h"

#ifdef HPM_RNG
#include "hpm_rng_drv.h"
#include "hpm_clock_drv.h"
#else
#include <stdlib.h>

#endif

#ifdef HPM_RNG
static bool is_trng_active;
static void tinycrypt_trng_init(void);

static void tinycrypt_trng_init(void)
{
    clock_add_to_group(clock_rng, 0);
    rng_init(HPM_RNG);
}

#endif

int default_CSPRNG(uint8_t *dest, unsigned int size)
{
    /* input sanity check: */
    if (dest == NULL || (size < 1UL)) {
        return 0;
    }
#ifdef HPM_RNG
    if (!is_trng_active) {
        tinycrypt_trng_init();
        is_trng_active = true;
    }
    rng_rand_wait(HPM_RNG, dest, size);
#else
    /* Note: Below codes are not cryptographically random, they are for demonstration purposes only */
    srand((uint32_t)dest);
    for (uint32_t i = 0; i < size; i++) {
        dest[i] = (uint8_t)(rand() & 0xFFU);
    }
#endif
    return 1;
}
