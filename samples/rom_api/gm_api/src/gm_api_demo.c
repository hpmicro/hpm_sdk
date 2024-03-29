/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_romapi.h"
#include "hpm_l1c_drv.h"

hpm_stat_t sm3_api_demo(void);
hpm_stat_t sm4_api_ecb_demo(void);
hpm_stat_t sm4_api_cbc_demo(void);

int main(void)
{
    board_init();

    hpm_stat_t status;

#if defined(ROMAPI_HAS_SW_SM4) && (ROMAPI_HAS_SW_SM4 > 0)
    status = sm4_api_ecb_demo();
    printf("SM4 ECB demo execution %s\n", (status == status_success) ? "passed" : "failed");
    status = sm4_api_cbc_demo();
    printf("SM4 CBC demo execution %s\n", (status == status_success) ? "passed" : "failed");
#endif

#if defined(ROMAPI_HAS_SW_SM3) && (ROMAPI_HAS_SW_SM3 > 0)
    status = sm3_api_demo();
    printf("SM3 demo execution %s\n", (status == status_success) ? "passed" : "failed");
#endif

    while (1) {
    }

    return 0;
}

#if defined(ROMAPI_HAS_SW_SM3) && (ROMAPI_HAS_SW_SM3 > 0)
hpm_stat_t sm3_api_demo(void)
{
    uint32_t error_cnt = 0;
    sm3_context_t sm3_ctx;

    printf("%s(), start...\n", __func__);

    uint8_t test1_content[3] = { 'a', 'b', 'c' };
    uint8_t digest[32];
    uint8_t test1_golden_data[32] = { 0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9, 0xd1, 0xf2, 0xd4, 0x6b, 0xdc,
                                      0x10, 0xe4, 0xe2, 0x41, 0x67, 0xc4, 0x87, 0x5c, 0xf2, 0xf7, 0xa2, 0x29, 0x7d,
                                      0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0 };
    printf("test1 started\n");
    rom_sm3_init(&sm3_ctx);
    rom_sm3_update(&sm3_ctx, test1_content, sizeof(test1_content));
    rom_sm3_finalize(&sm3_ctx, digest);
    printf("test1 finished\n");
    if (memcmp(digest, test1_golden_data, sizeof(digest)) != 0) {
        printf("test1 SM3 calculation failed\n");
        error_cnt++;
    } else {
        printf("test1 SM3 calculation passed\n");
    }

    uint8_t test2_content[64] = { 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd',
                                  'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd',
                                  'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd',
                                  'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd' };
    uint8_t test2_golden_data[32] = { 0xde, 0xbe, 0x9f, 0xf9, 0x22, 0x75, 0xb8, 0xa1, 0x38, 0x60, 0x48, 0x89, 0xc1,
                                      0x8e, 0x5a, 0x4d, 0x6f, 0xdb, 0x70, 0xe5, 0x38, 0x7e, 0x57, 0x65, 0x29, 0x3d,
                                      0xcb, 0xa3, 0x9c, 0x0c, 0x57, 0x32 };

    printf("test2 started\n");
    rom_sm3_init(&sm3_ctx);
    rom_sm3_update(&sm3_ctx, test2_content, sizeof(test2_content));
    rom_sm3_finalize(&sm3_ctx, digest);
    printf("test2 finished\n");
    if (memcmp(digest, test2_golden_data, sizeof(digest)) != 0) {
        printf("test2 SM3 calculation failed\n");
        error_cnt++;
    } else {
        printf("test2 SM3 calculation passed\n");
    }

    return (error_cnt > 0) ? status_fail : status_success;
}
#endif /* #if defined(ROMAPI_HAS_SW_SM3) && (ROMAPI_HAS_SW_SM3 > 0) */

#if defined(ROMAPI_HAS_SW_SM4) && (ROMAPI_HAS_SW_SM4 > 0)
hpm_stat_t sm4_api_ecb_demo(void)
{
    printf("%s, start...\n", __func__);
    uint8_t plain_text[16] = { 0 };
    uint8_t key[16];
    uint32_t error_cnt = 0;

    uint8_t cipher_text[16];


    const uint8_t golden_data[16] = { 0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e, 0x86, 0xb3, 0xe9, 0x4f, 0x53,
                                      0x6e, 0x42, 0x46 };

    for (uint32_t i = 0; i < 8; i++) {
        uint32_t nibble0, nibble1;
        nibble0 = 2 * i;
        nibble1 = 2 * i + 1;

        plain_text[i] = (nibble0 << 4) | nibble1;
        key[i] = (nibble0 << 4) | nibble1;

        plain_text[15 - i] = (nibble0) | (nibble1 << 4);
        key[15 - i] = (nibble0) | (nibble1 << 4);
    }

    sm4_context_t sm4_ctx;
    printf("SM4 ECB encryption start...\n");
    rom_sm4_setkey_enc(&sm4_ctx, key);
    rom_sm4_crypt_ecb(&sm4_ctx, SM4_ENCRYPT, sizeof(plain_text), plain_text, cipher_text);
    if (memcmp(cipher_text, golden_data, sizeof(cipher_text)) == 0) {
        printf("SM4 ECB encryption test passed!\n");
    } else {
        printf("SM4 ECB encryption test failed\n");
        error_cnt++;
    }

    uint8_t dec_text[16];
    rom_sm4_setkey_dec(&sm4_ctx, key);
    printf("SM4 ECB decryption start...\n");
    rom_sm4_crypt_ecb(&sm4_ctx, SM4_DECRYPT, sizeof(dec_text), cipher_text, dec_text);
    if (memcmp(dec_text, plain_text, sizeof(cipher_text)) == 0) {
        printf("SM4 ECB decryption test passed!\n");
    } else {
        printf("SM4 ECB decryption test failed\n");
        error_cnt++;
    }
    return (error_cnt > 0) ? status_fail : status_success;
}

hpm_stat_t sm4_api_cbc_demo(void)
{
    printf("%s, start...\n", __func__);
    uint8_t plain_text[16] = { 0 };
    uint8_t key[16];
    uint8_t iv[16];
    uint32_t error_cnt = 0;

    uint8_t cipher_text[16];


    const uint8_t golden_data[16] = { 0x5d, 0x1f, 0xee, 0x63, 0xf5, 0xeb, 0x8b, 0xb5,
                                      0x03, 0x58, 0x0a, 0xb8, 0x23, 0x92, 0x5d, 0x55 };

    for (uint32_t i = 0; i < 8; i++) {
        uint32_t nibble0, nibble1;
        nibble0 = 2 * i;
        nibble1 = 2 * i + 1;

        plain_text[i] = (nibble0 << 4) | nibble1;
        key[i] = (nibble0 << 4) | nibble1;

        plain_text[15 - i] = (nibble0) | (nibble1 << 4);
        key[15 - i] = (nibble0) | (nibble1 << 4);
    }

    for (uint32_t i = 0; i < sizeof(iv); i++) {
        iv[i] = (i << 4) | i;
    }

    sm4_context_t sm4_ctx;
    printf("SM4 CBC encryption start...\n");
    rom_sm4_setkey_enc(&sm4_ctx, key);
    rom_sm4_crypt_cbc(&sm4_ctx, SM4_ENCRYPT, sizeof(plain_text), iv, plain_text, cipher_text);
    if (memcmp(cipher_text, golden_data, sizeof(cipher_text)) == 0) {
        printf("SM4 CBC encryption test passed!\n");
    } else {
        printf("SM4 CBC encryption test failed\n");
        error_cnt++;
    }

    uint8_t dec_text[16];
    rom_sm4_setkey_dec(&sm4_ctx, key);
    printf("SM4 CBC decryption start...\n");
    rom_sm4_crypt_cbc(&sm4_ctx, SM4_DECRYPT, sizeof(dec_text), iv, cipher_text, dec_text);
    if (memcmp(dec_text, plain_text, sizeof(cipher_text)) == 0) {
        printf("SM4 CBC decryption test passed!\n");
    } else {
        printf("SM4 CBC decryption test failed\n");
        error_cnt++;
    }
    return (error_cnt > 0) ? status_fail : status_success;
}
#endif /* #if defined(ROMAPI_HAS_SW_SM4) && (ROMAPI_HAS_SW_SM4 > 0) */