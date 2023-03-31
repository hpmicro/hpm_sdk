/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef SHA256_ALT_H
#define SHA256_ALT_H

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
#define MBEDTLS_ERROR_DEFAULT    -1
#define MBEDTLS_ERROR_HW_NOT_SUPPORTED    -2
#if defined(MBEDTLS_SHA256_ALT)
/**
 * \brief          The SHA-256 context structure.
 *
 *                 The structure is used both for SHA-256 and for SHA-224
 *                 checksum calculations. The choice between these two is
 *                 made in the call to mbedtls_sha256_starts_ret().
 */
typedef struct mbedtls_sha256_context
{
    uint32_t total[2];          /*!< The number of Bytes processed.  */
    uint32_t state[8];          /*!< The intermediate digest state.  */
    unsigned char buffer[64];   /*!< The data block being processed. */
    int is224;                  /*!< Determines which function to use:
                                     0: Use SHA-256, or 1: Use SHA-224. */
}
mbedtls_sha256_context;
#endif /* defined(MBEDTLS_SHA256_ALT) */

#ifdef __cplusplus
}
#endif

#endif /* SHA256_ALT_H */
