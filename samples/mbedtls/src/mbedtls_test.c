/*
 * Copyright (c) 2023-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "mbedtls/aes.h"
#include <stdio.h>
#define LED_FLASH_PERIOD_IN_MS 300
#include "hpm_romapi.h"
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "board.h"
#include "sha1_alt.h"
#include "sha256_alt.h"
#include "sha_common.h"
#include <stdio.h>
#include <string.h>

#define mbedtls_printf printf

extern sdp_aes_ctx_t s_aes_ctx;
static void mbedtls_hmac_demo(void)
{
    int ret;
    unsigned char secret[] = "a secret";
    unsigned char buffer[] = "some data to hash";
    unsigned char digest[32];
    mbedtls_md_context_t sha_ctx;

    mbedtls_md_init(&sha_ctx);
    memset(digest, 0x00, sizeof(digest));

    ret = mbedtls_md_setup(&sha_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 1);
    if (ret != 0) {
        mbedtls_printf("[ERROR] mbedtls_md_setup() returned -0x%04x\n", -ret);
        goto exit;
    }

    mbedtls_md_hmac_starts(&sha_ctx, secret, sizeof(secret) - 1);
    mbedtls_md_hmac_update(&sha_ctx, buffer, sizeof(buffer) - 1);
    mbedtls_md_hmac_finish(&sha_ctx, digest);

    mbedtls_printf("- HMAC: ");
    for (uint32_t i = 0; i < sizeof(digest); i++)
        mbedtls_printf("%02X", digest[i]);
    mbedtls_printf("\n");

exit:
    mbedtls_md_free(&sha_ctx);

    return;
}

static void mbedtls_aes_ecb_demo(void)
{
    int i = 0;

    uint8_t plaintext[16] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P'};
    const uint8_t passwd2[32] = "AAAAAAAAAABBBBBBBBBBCCCCCCCCCCDD";
    ATTR_PLACE_AT_NONCACHEABLE static uint8_t encrypt[16];
    ATTR_PLACE_AT_NONCACHEABLE static uint8_t decrypt[16];
    mbedtls_aes_context ctx;

    mbedtls_aes_init(&ctx);

    mbedtls_aes_setkey_enc(&ctx, passwd2, 128);
    mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_ENCRYPT, plaintext, encrypt);

    mbedtls_aes_setkey_dec(&ctx, passwd2, 128);
    mbedtls_aes_crypt_ecb(&ctx, MBEDTLS_AES_DECRYPT, encrypt, decrypt);

    mbedtls_aes_free(&ctx);

    mbedtls_printf("- AES ECB encrypt: [");
    for (i = 0; i < 16; i++) {
        mbedtls_printf("%02X", encrypt[i]);
    }
    mbedtls_printf("]\r\n");

    mbedtls_printf("- AES ECB decrypt: [");
    for (i = 0; i < 16; i++) {
        mbedtls_printf("%c", decrypt[i]);
    }
    mbedtls_printf("]\r\n");
    if (memcmp(decrypt, plaintext, sizeof(plaintext)) != 0) {
        mbedtls_printf("[ERROR] Opps, result doesn't match the golden data %s\n", __func__);
    } else {
        mbedtls_printf("[OK] %s calculation for passed\n", __func__);
    }
}

static void mbedtls_aes_cbc_demo(void)
{
    uint32_t i = 0;
    ATTR_PLACE_AT_NONCACHEABLE_INIT static uint8_t plaintext[16] = {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a};
    ATTR_PLACE_AT_NONCACHEABLE static uint8_t encrypt[16];
    ATTR_PLACE_AT_NONCACHEABLE static uint8_t decrypt[16];
    const uint8_t def_key128[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    const uint8_t iv[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    uint8_t iv_input[16];

    memcpy(iv_input, iv, 16);

    mbedtls_aes_context ctx;

    mbedtls_aes_setkey_enc(&ctx, def_key128, 128);
    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, sizeof(plaintext), iv_input, plaintext, encrypt);

    memcpy(iv_input, iv, 16);
    mbedtls_aes_setkey_dec(&ctx, def_key128, 128);
    mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, sizeof(plaintext), iv_input, encrypt, decrypt);

    mbedtls_aes_free(&ctx);

    mbedtls_printf("- AES CBC encrypt: [");
    for (i = 0; i < sizeof(encrypt); i++) {
        mbedtls_printf("%02X", encrypt[i]);
    }
    mbedtls_printf("]\r\n");
    mbedtls_printf("- AES CBC decrypt: [");
    for (i = 0; i < sizeof(decrypt); i++) {
        mbedtls_printf("%02X", decrypt[i]);
    }
    mbedtls_printf("]\r\n");
    if (memcmp(decrypt, plaintext, sizeof(plaintext)) != 0) {
        mbedtls_printf("[ERROR] Opps, result doesn't match the golden data %s\n", __func__);
    } else {
        mbedtls_printf("[OK] %s calculation for passed\n", __func__);
    }
}

static void mbedtls_sha1_demo(void)
{
    int ret;
    static const unsigned char m0[] = "abc";
    const uint8_t expected0[20] = {
        0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a, 0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0,
        0xd8, 0x9d};
    unsigned char digest[20] = {0};
    mbedtls_sha1_context sha1_ctx;

    memset(digest, 0x00, sizeof(digest));

    mbedtls_sha1_init(&sha1_ctx);

    ret = mbedtls_sha1_starts_ret(&sha1_ctx);
    if (ret) {
        mbedtls_printf("[ERROR] mbedtls_sha1_starts_ret\n");
        return;
    }
    ret = mbedtls_sha1_update_ret(&sha1_ctx, m0, sizeof(m0) - 1);
    if (ret) {
        mbedtls_printf("[ERROR] mbedtls_sha1_update_ret\n");
        return;
    }
    ret = mbedtls_sha1_finish_ret(&sha1_ctx, digest);
    if (ret) {
        mbedtls_printf("[ERROR] mbedtls_sha1_finish_ret\n");
        return;
    }

    int i = 0;
    mbedtls_printf("- SHA1: [");
    while (i < 20) {
        mbedtls_printf("%02X", digest[i]);
        i++;
    }
    mbedtls_printf("]\n");
    if (memcmp(digest, expected0, sizeof(digest)) != 0) {
        mbedtls_printf("[ERROR] Opps, result doesn't match the golden data\n");
    } else {
        mbedtls_printf("[OK] HASH calculation for %s passed\n", m0);
    }
}

static void mbedtls_sha256_demo(void)
{
    int ret;
    const uint8_t expected0[32] = {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23, 0xb0, 0x03,
        0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad};
    static const unsigned char m0[] = "abc";
    unsigned char digest[32] = {0};
    mbedtls_sha256_context sha256_ctx;

    memset(digest, 0x00, sizeof(digest));

    mbedtls_sha256_init(&sha256_ctx);

    ret = mbedtls_sha256_starts_ret(&sha256_ctx, 0);
    if (ret) {
        mbedtls_printf("[ERROR] mbedtls_sha256_starts_ret\n");
        return;
    }
    ret = mbedtls_sha256_update_ret(&sha256_ctx, m0, sizeof(m0) - 1);
    if (ret) {
        mbedtls_printf("[ERROR] mbedtls_sha256_update_ret\n");
        return;
    }
    ret = mbedtls_sha256_finish_ret(&sha256_ctx, digest);
    if (ret) {
        mbedtls_printf("[ERROR] mbedtls_sha256_finish_ret\n");
        return;
    }

    int i = 0;
    mbedtls_printf("- SHA256: [");
    while (i < 32) {
        mbedtls_printf("%02X", digest[i]);
        i++;
    }
    mbedtls_printf("]\n");
    if (memcmp(digest, expected0, sizeof(digest)) != 0) {
        mbedtls_printf("[ERROR] Opps, result doesn't match the golden data\n");
    } else {
        mbedtls_printf("[OK] HASH calculation for %s passed\n", m0);
    }
}

int main(void)
{
    int u;
    board_init();

#if defined(CONFIG_MBEDTLS_USE_HPM_SDP) && CONFIG_MBEDTLS_USE_HPM_SDP
    clock_add_to_group(clock_sdp, BOARD_RUNNING_CORE & 0x1);
#endif

    board_init_led_pins();
    board_timer_create(LED_FLASH_PERIOD_IN_MS, board_led_toggle);

    mbedtls_printf("begin to test mbedtls\n");

    mbedtls_aes_ecb_demo();
    mbedtls_aes_cbc_demo();
    mbedtls_sha1_demo();
    mbedtls_sha256_demo();
    mbedtls_hmac_demo();
    mbedtls_printf("All Test Finished\n");
    while (1) {
        u = getchar();
        if (u == '\r') {
            u = '\n';
        }
        mbedtls_printf("%c", u);
    }
    return 0;
}