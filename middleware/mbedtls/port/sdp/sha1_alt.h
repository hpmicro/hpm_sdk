/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SHA1_ALT_H
#define SHA1_ALT_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "hpm_common.h"
#include "hpm_sdp_drv.h"
#include "hpm_romapi.h"
#include "sha_common.h"

#ifdef __cplusplus
extern "C" {
#endif
#if defined(MBEDTLS_SHA1_ALT)
/**
 * \brief          The SHA-1 context structure.
 *
 * \warning        SHA-1 is considered a weak message digest and its use
 *                 constitutes a security risk. We recommend considering
 *                 stronger message digests instead.
 *
 */
typedef struct mbedtls_sha1_context
{
    uint32_t total[2];          /*!< The number of Bytes processed.  */
    uint32_t state[5];          /*!< The intermediate digest state.  */
    unsigned char buffer[64];   /*!< The data block being processed. */
}
mbedtls_sha1_context;
#endif /* defined(MBEDTLS_SHA1_ALT) */

#ifdef __cplusplus
}
#endif

#endif /* SHA1_ALT_H */
