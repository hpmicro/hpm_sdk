/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "sha1_alt.h"
#if defined(MBEDTLS_SHA1_C)
#include "hpm_l1c_drv.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/sha1.h"
#include <string.h>

#if defined(MBEDTLS_PADLOCK_C)
#include "mbedtls/padlock.h"
#endif

#if defined(MBEDTLS_AESNI_C)
#include "mbedtls/aesni.h"
#endif

#if defined(MBEDTLS_THREADING_C)
#include "ksdk_mbedtls.h"
#include "mbedtls/threading.h"
#endif

/******************************************************************************/
/*************************** SHA1**********************************************/
/******************************************************************************/
#if defined(MBEDTLS_HPM_SDP_SHA1)

#include "mbedtls/sha1.h"
#include "board.h"

void mbedtls_sha1_init(mbedtls_sha1_context *ctx)
{
    sdp_hash_ctx_t *p_sys_sdp_ctx = (sdp_hash_ctx_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&s_hash_ctx);
    memset(ctx, 0, sizeof(mbedtls_sha1_context));
    hpm_sdp_api_init();
    (void)rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_sha1);
}

void mbedtls_sha1_free(mbedtls_sha1_context *ctx)
{
    if (ctx == NULL)
        return;

    mbedtls_zeroize(ctx, sizeof(mbedtls_sha1_context));
}

void mbedtls_sha1_clone(mbedtls_sha1_context *dst, const mbedtls_sha1_context *src)
{
    memcpy(dst, src, sizeof(mbedtls_sha1_context));
}

/*
 * SHA-1 context setup
 */
int mbedtls_sha1_starts_ret(mbedtls_sha1_context *ctx)
{
    hpm_stat_t ret = status_fail;
    (void)ctx;
    sdp_hash_ctx_t *p_sys_sdp_ctx = (sdp_hash_ctx_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&s_hash_ctx);
    ret = rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_sha1);
    if (ret != status_success)
    {
        return MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED;
    }
    return 0;
}

int mbedtls_internal_sha1_process(mbedtls_sha1_context *ctx, const unsigned char data[64])
{
    static uint8_t ATTR_PLACE_AT_NONCACHEABLE local[64];
    hpm_stat_t ret = status_fail;
    (void)ctx;
    sdp_hash_ctx_t *p_sys_sdp_ctx = (sdp_hash_ctx_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&s_hash_ctx);
    memcpy(local, data, 64);
    ret = rom_sdp_hash_update(p_sys_sdp_ctx, local, 64);
    if (ret != status_success)
    {
        return MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED;
    }
    return 0;
}

/*
 * SHA-1 process buffer
 */
int mbedtls_sha1_update_ret(mbedtls_sha1_context *ctx, const unsigned char *input, size_t ilen)
{
    static uint8_t ATTR_PLACE_AT_NONCACHEABLE local[64];
    hpm_stat_t ret = status_fail;

    for (uint32_t i = 0; i < ilen; i += 64)
    {
        size_t len = ilen - i > 64 ? 64 : ilen - i;
        if (len < 64)
        {
            sdp_hash_ctx_t *p_sys_sdp_ctx = (sdp_hash_ctx_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&s_hash_ctx);
            memcpy(local, input + i, len);
            ret = rom_sdp_hash_update(p_sys_sdp_ctx, local, len);
            if (ret != status_success)
            {
                return MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED;
            }

            return 0;
        }
        else
        {
            ret = mbedtls_internal_sha1_process(ctx, input + i);
            if (ret != status_success)
            {
                return MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED;
            }
        }
    }
    return 0;
}

/*
 * SHA-1 final digest
 */
int mbedtls_sha1_finish_ret(mbedtls_sha1_context *ctx, unsigned char output[20])
{
    static uint8_t ATTR_PLACE_AT_NONCACHEABLE local[20];
    hpm_stat_t ret = status_fail;
    (void)ctx;
    sdp_hash_ctx_t *p_sys_sdp_ctx = (sdp_hash_ctx_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)&s_hash_ctx);
    uint8_t *pout = (uint8_t *)core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)local);

    ret = rom_sdp_hash_finish(p_sys_sdp_ctx, pout);
    if (ret != status_success)
    {
        return MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED;
    }
    memcpy(output, pout, 20);
    return 0;
}
#endif /* MBEDTLS_HPM_SDP_SHA1 */
#endif /* MBEDTLS_SHA1_C */