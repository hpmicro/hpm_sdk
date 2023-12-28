/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sha_common.h"
#include "hpm_l1c_drv.h"

ATTR_PLACE_AT_NONCACHEABLE sdp_hash_ctx_t s_hash_ctx;
static volatile bool flag_sdp_inited = false;

#if defined(MBEDTLS_SHA1_ALT) || defined(MBEDTLS_SHA256_ALT)
/* Implementation that should never be optimized out by the compiler */
void mbedtls_zeroize(void *v, size_t n)
{
    volatile unsigned char *p = v;
    while (0U != n--)
    {
        *p++ = 0;
    }
}

void hpm_sdp_api_init(void)
{
    if (!flag_sdp_inited)
    {
        flag_sdp_inited = true;
        rom_sdp_init();
    }
}

#endif /* MBEDTLS_SHA1_ALT || MBEDTLS_SHA256_ALT */