/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SHA_COMMON_H
#define SHA_COMMON_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "hpm_common.h"
#include "hpm_sdp_drv.h"
#include "hpm_romapi.h"

#ifdef __cplusplus
extern "C" {
#endif

extern sdp_hash_ctx_t s_hash_ctx;
void mbedtls_zeroize(void *v, size_t n);
void hpm_sdp_api_init(void);
#ifdef __cplusplus
}
#endif

#endif /* SHA_COMMON_H */
