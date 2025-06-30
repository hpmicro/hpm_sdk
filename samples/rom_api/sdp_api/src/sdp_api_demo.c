/*
 * Copyright (c) 2021-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_romapi.h"
#include "hpm_l1c_drv.h"
#include "hpm_soc_feature.h"

ATTR_PLACE_AT_NONCACHEABLE static sdp_aes_ctx_t s_aes_ctx;

ATTR_PLACE_AT_NONCACHEABLE static sdp_hash_ctx_t s_hash_ctx;

ATTR_PLACE_AT_NONCACHEABLE static sdp_dma_ctx_t s_dma_ctx;

static void sdp_api_init(void);
static hpm_stat_t sdp_api_aes_ecb_demo(void);
static hpm_stat_t sdp_api_aes_cbc_demo(void);
static hpm_stat_t sdp_api_sha256_demo(void);
static hpm_stat_t sdp_api_sha1_demo(void);
static hpm_stat_t sdp_api_crc32_demo(void);
static hpm_stat_t sdp_api_memcpy_demo(void);
static hpm_stat_t sdp_api_memset_demo(void);

#if defined(SDP_HAS_SM3_SUPPORT) && (SDP_HAS_SM3_SUPPORT == 1)
static hpm_stat_t sdp_api_sm3_demo(void);
#endif

#if defined(SDP_HAS_SM4_SUPPORT) && (SDP_HAS_SM4_SUPPORT == 1)
static hpm_stat_t sdp_api_sm4_ecb_demo(void);
static hpm_stat_t sdp_api_sm4_cbc_demo(void);
#endif

static void app_cache_flush(uint32_t addr, uint32_t size);

int main(void)
{
    board_init();

    clock_add_to_group(clock_sdp, 0);

    sdp_api_init();

    hpm_stat_t status;
    printf("SDP API demo\nThis demo will test the Crypto algorithms, HASH algorithms and the memory operations\n");

    status = sdp_api_aes_ecb_demo();
    printf("SDP AES ECB demo execution %s\n", (status == status_success) ? "passed" : "failed");
    status = sdp_api_aes_cbc_demo();
    printf("SDP AES CBC demo execution %s\n", (status == status_success) ? "passed" : "failed");
#if defined(SDP_HAS_SM4_SUPPORT) && (SDP_HAS_SM4_SUPPORT == 1)
    status = sdp_api_sm4_ecb_demo();
    printf("SDP SM4 ECB demo execution %s\n", (status == status_success) ? "passed" : "failed");
    status = sdp_api_sm4_cbc_demo();
    printf("SDP SM4 CBC demo execution %s\n", (status == status_success) ? "passed" : "failed");
#endif

    status = sdp_api_sha256_demo();
    printf("SDP SHA256 demo execution %s\n", (status == status_success) ? "passed" : "failed");
#if defined(SDP_HAS_SM3_SUPPORT) && (SDP_HAS_SM3_SUPPORT == 1)
    status = sdp_api_sm3_demo();
    printf("SDP SM3 demo execution %s\n", (status == status_success) ? "passed" : "failed");
#endif
    status = sdp_api_sha1_demo();
    printf("SDP SHA1 demo execution %s\n", (status == status_success) ? "passed" : "failed");
    status = sdp_api_crc32_demo();
    printf("SDP CRC32 demo execution %s\n", (status == status_success) ? "passed" : "failed");
    status = sdp_api_memcpy_demo();
    printf("SDP MEMCPY demo execution %s\n", (status == status_success) ? "passed" : "failed");
    status = sdp_api_memset_demo();
    printf("SDP MEMSET demo execution %s\n", (status == status_success) ? "passed" : "failed");

    while (1) {
    }

    return 0;
}

static void sdp_api_init(void)
{
    rom_sdp_init();
}

static hpm_stat_t sdp_api_aes_ecb_demo(void)
{
    uint32_t error_cnt = 0;
    const uint8_t gold_ct128[16] = {
        0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30, 0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a
    };
    const uint8_t gold_ct256[16] = {
        0x8e, 0xa2, 0xb7, 0xca, 0x51, 0x67, 0x45, 0xbf, 0xea, 0xfc, 0x49, 0x90, 0x4b, 0x49, 0x60, 0x89
    };

    // AES-128 IV test
    // AES key = 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
    uint8_t aes_key[32];
    for (uint32_t i = 0; i < 32; i++) {
        aes_key[i] = i;
    }

    sdp_aes_ctx_t *p_sys_sdp_ctx = (sdp_aes_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_aes_ctx);

    printf("rom_sdp_aes_crypt_ecb, 128-bit key test.\n");
    (void) rom_sdp_aes_set_key(p_sys_sdp_ctx, aes_key, sdp_aes_keybits_128, 0);

    ATTR_PLACE_AT_NONCACHEABLE static uint8_t input[16];
    ATTR_PLACE_AT_NONCACHEABLE static uint8_t output[16];
    // Test data = 00 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff
    for (uint32_t i = 0; i < 16; i++) {
        input[i] = i | (i << 4);
    }

    uint8_t *p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &input);
    uint8_t *p_sys_output = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &output);
    hpm_stat_t status = rom_sdp_aes_crypt_ecb(p_sys_sdp_ctx, sdp_aes_op_encrypt, 16, p_sys_input, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("rom_sdp_aes_crypt_ecb() encryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, gold_ct128, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("rom_sdp_aes_crypt_ecb encryption test passed\n");
        }
    }

    status = rom_sdp_aes_crypt_ecb(p_sys_sdp_ctx, sdp_aes_op_decrypt, 16, p_sys_output, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("rom_sdp_aes_crypt_ecb() decryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, input, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("rom_sdp_aes_crypt_ecb decryption test passed\n");
        }
    }


    // AES-256 ECB test
    printf("rom_sdp_aes_crypt_ecb, 256-bit key test.\n");
    // Test data = 00 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff
    for (uint32_t i = 0; i < 16; i++) {
        input[i] = i | (i << 4);
    }

    // AES key = 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f
    for (uint32_t i = 0; i < 32; i++) {
        aes_key[i] = i;
    }

    (void) rom_sdp_aes_set_key(&s_aes_ctx, aes_key, sdp_aes_keybits_256, 1);
    status = rom_sdp_aes_crypt_ecb(p_sys_sdp_ctx, sdp_aes_op_encrypt, 16, p_sys_input, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("rom_sdp_aes_crypt_ecb() encryption failed, status=%d\n", status);
    } else {
        printf("rom_sdp_aes_crypt_ecb() encryption completed\n");
        if (memcmp(output, gold_ct256, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("rom_sdp_aes_crypt_ecb encryption test passed\n");
        }
    }

    status = rom_sdp_aes_crypt_ecb(p_sys_sdp_ctx, sdp_aes_op_decrypt, 16, p_sys_output, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("rom_sdp_aes_crypt_ecb(), decryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, input, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("rom_sdp_aes_crypt_ecb decryption test passed\n");
        }
    }

    return (error_cnt == 0U) ? status_success : status_fail;
}

hpm_stat_t sdp_api_aes_cbc_demo(void)
{
    hpm_stat_t status;
    uint32_t error_cnt = 0;
    sdp_aes_ctx_t *p_sys_sdp_ctx = (sdp_aes_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_aes_ctx);
    ATTR_PLACE_AT_NONCACHEABLE_INIT static uint8_t input[16] = {
        0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
    };
    ATTR_PLACE_AT_NONCACHEABLE static uint8_t output[16];
    const uint8_t gold_ct128[16] = {
        0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d
    };
    const uint8_t gold_ct256[16] = {
        0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba, 0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6
    };
    const uint8_t def_key128[16] = {
        0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
    };
    const uint8_t def_key256[32] = {
        0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81, 0x1f, 0x35,
        0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
    };
    uint8_t iv[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    uint8_t *p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &input);
    uint8_t *p_sys_output = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &output);

    // AES-128 CBC test
    printf("AES-128 CBC test...\n");
    (void) rom_sdp_aes_set_key(&s_aes_ctx, def_key128, sdp_aes_keybits_128, 0);
    status = rom_sdp_aes_crypt_cbc(p_sys_sdp_ctx, sdp_aes_op_encrypt, sizeof(input), iv, p_sys_input, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_aes_crypt_cbc() encryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, gold_ct128, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("sdp_aes_crypt_cbc() encryption passed\n");
        }
    }

    status = rom_sdp_aes_crypt_cbc(p_sys_sdp_ctx, sdp_aes_op_decrypt, sizeof(input), iv, p_sys_output, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("Decryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, input, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("sdp_aes_crypt_cbc() decryption passed\n");
        }
    }


    // AES-256 CBC test
    printf("AES-256 CBC test...\n");
    (void) rom_sdp_aes_set_key(&s_aes_ctx, def_key256, sdp_aes_keybits_256, 0);
    status = rom_sdp_aes_crypt_cbc(p_sys_sdp_ctx, sdp_aes_op_encrypt, sizeof(input), iv, p_sys_input, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("Encryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, gold_ct256, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("sdp_aes_crypt_cbc() encryption passed\n");
        }
    }

    status = rom_sdp_aes_crypt_cbc(p_sys_sdp_ctx, sdp_aes_op_decrypt, sizeof(input), iv, p_sys_output, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("Decryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, input, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("sdp_aes_crypt_cbc() decryption passed\n");
        }
    }

    return (error_cnt == 0U) ? status_success : status_fail;
}

hpm_stat_t sdp_api_sha256_demo(void)
{
    uint32_t error_cnt = 0;
    hpm_stat_t status;
    sdp_hash_ctx_t *p_sys_sdp_ctx = (sdp_hash_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_hash_ctx);
    uint8_t digest[32];
    uint8_t *p_sys_input;

    const char m0[] = "abc";
    const uint8_t expected0[32] = {
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23, 0xb0, 0x03,
        0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
    };
    p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) m0);
    status = rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_sha256);
    if (status != status_success) {
        ++error_cnt;
        printf("rom_sdp_hash_init failed, status =%d\n", status);
    }
    app_cache_flush((uint32_t) p_sys_input, sizeof(m0));
    status = rom_sdp_hash_update(p_sys_sdp_ctx, p_sys_input, strlen(m0));
    if (status != status_success) {
        ++error_cnt;
        printf("rom_sdp_hash_update failed, status =%d\n", status);
    }
    status = rom_sdp_hash_finish(p_sys_sdp_ctx, digest);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_finish failed, status =%d\n", status);
    } else {
        if (memcmp(digest, expected0, sizeof(digest)) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("HASH calculation for %s passed\n", m0);
        }
    }


    const char m1[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\0\0\0\0";
    const uint8_t expected1[32] = {
        0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8, 0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39, 0xa3, 0x3c,
        0xe4, 0x59, 0x64, 0xff, 0x21, 0x67, 0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
    };
    p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) m1);

    status = rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_sha256);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }
    app_cache_flush((uint32_t) p_sys_input, sizeof(m1));
    status = rom_sdp_hash_update(p_sys_sdp_ctx, p_sys_input, strlen(m1));
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }
    status = rom_sdp_hash_finish(p_sys_sdp_ctx, digest);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_finish failed, status =%d\n", status);
    } else {
        if (memcmp(digest, expected1, sizeof(digest)) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("HASH calculation for %s passed\n", m1);
        }
    }

    char m2[1000];
    (void) memset(m2, 0x41, sizeof(m2));
    const uint8_t expected2[32] = {
        0xc2, 0xe6, 0x86, 0x82, 0x34, 0x89, 0xce, 0xd2, 0x01, 0x7f, 0x60, 0x59, 0xb8, 0xb2, 0x39, 0x31, 0x8b, 0x63,
        0x64, 0xf6, 0xdc, 0xd8, 0x35, 0xd0, 0xa5, 0x19, 0x10, 0x5a, 0x1e, 0xad, 0xd6, 0xe4
    };

    p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) m2);
    status = rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_sha256);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }

    app_cache_flush((uint32_t) p_sys_input, sizeof(m2));
    status = rom_sdp_hash_update(p_sys_sdp_ctx, p_sys_input, sizeof(m2));
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }
    status = rom_sdp_hash_finish(p_sys_sdp_ctx, digest);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_finish failed, status =%d\n", status);
    } else {
        if (memcmp(digest, expected2, sizeof(digest)) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("HASH calculation for 1000 bytes passed\n");
        }
    }

    return (error_cnt == 0U) ? status_success : status_fail;
}

hpm_stat_t sdp_api_sha1_demo(void)
{
    uint32_t error_cnt = 0;
    hpm_stat_t status;
    sdp_hash_ctx_t *p_sys_sdp_ctx = (sdp_hash_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_hash_ctx);
    uint8_t digest[20];
    uint8_t *p_sys_input;

    const char m0[] = "abc";
    const uint8_t expected0[20] = {
        0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a, 0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0,
        0xd8, 0x9d
    };
    p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &m0);

    status = rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_sha1);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }
    app_cache_flush((uint32_t) p_sys_input, sizeof(m0));
    status = rom_sdp_hash_update(p_sys_sdp_ctx, p_sys_input, strlen(m0));
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }
    status = rom_sdp_hash_finish(p_sys_sdp_ctx, digest);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_final failed, status =%d\n", status);
    } else {
        if (memcmp(digest, expected0, sizeof(digest)) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        }
    }


    const char m1[] = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
    const uint8_t expected1[20] = {
        0x84, 0x98, 0x3e, 0x44, 0x1c, 0x3b, 0xd2, 0x6e, 0xba, 0xae, 0x4a, 0xa1, 0xf9, 0x51, 0x29, 0xe5, 0xe5, 0x46,
        0x70, 0xf1
    };
    p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &m1);

    status = rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_sha1);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }

    app_cache_flush((uint32_t) p_sys_input, sizeof(m1));
    status = rom_sdp_hash_update(p_sys_sdp_ctx, p_sys_input, strlen(m1));
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }
    status = rom_sdp_hash_finish(p_sys_sdp_ctx, digest);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_final failed, status =%d\n", status);
    } else {
        if (memcmp(digest, expected1, sizeof(digest)) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        }
    }

    return (error_cnt == 0U) ? status_success : status_fail;
}

hpm_stat_t sdp_api_crc32_demo(void)
{
    uint32_t error_cnt = 0;
    hpm_stat_t status;
    sdp_hash_ctx_t *p_sys_sdp_ctx = (sdp_hash_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_hash_ctx);
    uint8_t digest[4];
    uint8_t *p_sys_input;

    const char m0[] = "abcd";
    const uint8_t expected0[4] = {
        0xb4, 0xcc, 0xb3, 0xc2
    };
    p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &m0);

    status = rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_crc32);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }

    app_cache_flush((uint32_t) p_sys_input, sizeof(m0));
    status = rom_sdp_hash_update(p_sys_sdp_ctx, p_sys_input, strlen(m0));
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }
    status = rom_sdp_hash_finish(p_sys_sdp_ctx, digest);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_final failed, status =%d\n", status);
    } else {
        if (memcmp(digest, expected0, sizeof(digest)) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        }
    }


    const char m1[] = "abc";
    const uint8_t expected1[4] = {
        0xff, 0x7c, 0x12, 0x7d
    };
    p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &m1);

    status = rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_crc32);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }

    app_cache_flush((uint32_t) p_sys_input, sizeof(m1));
    status = rom_sdp_hash_update(p_sys_sdp_ctx, p_sys_input, strlen(m1));
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }
    status = rom_sdp_hash_finish(p_sys_sdp_ctx, digest);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_final failed, status =%d\n", status);
    } else {
        if (memcmp(digest, expected1, sizeof(digest)) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        }
    }

    return (error_cnt == 0U) ? status_success : status_fail;
}

hpm_stat_t sdp_api_memcpy_demo(void)
{
    const char hello_src[] = "hello world\n";
    ATTR_PLACE_AT_NONCACHEABLE static char hello_dst[sizeof(hello_src)];

    sdp_dma_ctx_t *p_sdp_ctx = (sdp_dma_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_dma_ctx);
    const void *p_sys_src = (const void *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) hello_src);
    void *p_sys_dst = (void *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) hello_dst);
    printf("%s, desc_addr=%x, src_addr=%x, dst_addr=%x\n",
           __func__,
           (uint32_t) &p_sdp_ctx->sdp_pkt,
           (uint32_t) p_sys_src,
           (uint32_t) p_sys_dst);

    app_cache_flush((uint32_t) &hello_src, sizeof(hello_src));
    hpm_stat_t status = rom_sdp_memcpy(p_sdp_ctx, p_sys_dst, p_sys_src, sizeof(hello_dst));
    if (status != status_success) {
        printf("sdp_memcpy failed, error code=%d\n", status);
        return status_fail;
    } else {
        if (memcmp(hello_dst, hello_src, sizeof(hello_src)) == 0) {
            printf("sdp_memcpy succeeded\n");
        } else {
            printf("sdp_memcpy failed, the content in destination memory didn't match the one in source memory\n");
            return status_fail;
        }
    }

    return status_success;
}

hpm_stat_t sdp_api_memset_demo(void)
{
    ATTR_PLACE_AT_NONCACHEABLE static uint8_t dst[20];

    (void) memset(dst, 0xFF, sizeof(dst));
    uint8_t pattern = 0xaa;

    sdp_dma_ctx_t *p_sdp_ctx = (sdp_dma_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_dma_ctx);
    void *p_sys_dst = (void *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) dst);
    printf("%s, desc_addr=%x, dst_addr=%x\n", __func__, (uint32_t) &p_sdp_ctx->sdp_pkt, (uint32_t) p_sys_dst);
    hpm_stat_t status = rom_sdp_memset(p_sdp_ctx, p_sys_dst, pattern, sizeof(dst));
    app_cache_flush((uint32_t) &dst, sizeof(dst));
    if (status != status_success) {
        printf("sdp_memset failed, error code=%d\n", status);
        return status_fail;
    } else {
        bool match = true;
        for (uint32_t i = 0; i < sizeof(dst); i++) {
            if (dst[i] != pattern) {
                match = false;
                break;
            }
        }
        if (match) {
            printf("sdp_memset succeeded\n");
        } else {
            printf("sdp_memset failed, the content in destination memory didn't match the pattern\n");
            return status_fail;
        }
    }
    return status_success;
}

static void app_cache_flush(uint32_t addr, uint32_t size)
{
    if (l1c_dc_is_enabled()) {
        uint32_t aligned_start = addr & ~(HPM_L1C_CACHELINE_SIZE - 1U);
        uint32_t aligned_size =
            ((addr + size + HPM_L1C_CACHELINE_SIZE - 1U) & ~(HPM_L1C_CACHELINE_SIZE - 1U)) - aligned_start;

        l1c_dc_flush(aligned_start, aligned_size);
    }
}

#if defined(SDP_HAS_SM3_SUPPORT) && (SDP_HAS_SM3_SUPPORT == 1)
static hpm_stat_t sdp_api_sm3_demo(void)
{
    uint32_t error_cnt = 0;
    hpm_stat_t status;
    sdp_hash_ctx_t *p_sys_sdp_ctx = (sdp_hash_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_hash_ctx);
    uint8_t digest[32];
    uint8_t *p_sys_input;

    const char m0[] = { 'a', 'b', 'c' };
    const uint8_t expected0[32] = {
        0x66, 0xc7, 0xf0, 0xf4, 0x62, 0xee, 0xed, 0xd9, 0xd1, 0xf2, 0xd4, 0x6b, 0xdc, 0x10, 0xe4, 0xe2, 0x41, 0x67,
        0xc4, 0x87, 0x5c, 0xf2, 0xf7, 0xa2, 0x29, 0x7d, 0xa0, 0x2b, 0x8f, 0x4b, 0xa8, 0xe0
    };
    p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) m0);
    status = rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_sm3);
    if (status != status_success) {
        ++error_cnt;
        printf("rom_sdp_hash_init failed, status =%d\n", status);
    }
    app_cache_flush((uint32_t) p_sys_input, sizeof(m0));
    status = rom_sdp_hash_update(p_sys_sdp_ctx, p_sys_input, sizeof(m0));
    if (status != status_success) {
        ++error_cnt;
        printf("rom_sdp_hash_update failed, status =%d\n", status);
    }
    status = rom_sdp_hash_finish(p_sys_sdp_ctx, digest);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_finish failed, status =%d\n", status);
    } else {
        if (memcmp(digest, expected0, sizeof(digest)) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        }
    }


    const char m1[64] = {
        'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b',
        'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd',
        'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd', 'a', 'b', 'c', 'd'
    };
    const uint8_t expected1[32] = {
        0xde, 0xbe, 0x9f, 0xf9, 0x22, 0x75, 0xb8, 0xa1, 0x38, 0x60, 0x48, 0x89, 0xc1, 0x8e, 0x5a, 0x4d, 0x6f, 0xdb,
        0x70, 0xe5, 0x38, 0x7e, 0x57, 0x65, 0x29, 0x3d, 0xcb, 0xa3, 0x9c, 0x0c, 0x57, 0x32
    };
    p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) m1);

    status = rom_sdp_hash_init(p_sys_sdp_ctx, sdp_hash_alg_sm3);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }
    app_cache_flush((uint32_t) p_sys_input, sizeof(m1));
    status = rom_sdp_hash_update(p_sys_sdp_ctx, p_sys_input, sizeof(m1));
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_init failed, status =%d\n", status);
    }
    status = rom_sdp_hash_finish(p_sys_sdp_ctx, digest);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_hash_finish failed, status =%d\n", status);
    } else {
        if (memcmp(digest, expected1, sizeof(digest)) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        }
    }


    return (error_cnt == 0U) ? status_success : status_fail;
}
#endif

#if defined(SDP_HAS_SM4_SUPPORT) && (SDP_HAS_SM4_SUPPORT == 1)
static hpm_stat_t sdp_api_sm4_ecb_demo(void)
{
    ATTR_PLACE_AT_NONCACHEABLE static uint8_t input[16];
    ATTR_PLACE_AT_NONCACHEABLE static uint8_t output[16];

    uint32_t error_cnt = 0;
    const uint8_t gold_ct128[16] = { 0x68, 0x1e, 0xdf, 0x34, 0xd2, 0x06, 0x96, 0x5e, 0x86, 0xb3, 0xe9, 0x4f, 0x53, 0x6e, 0x42, 0x46 };
    /*
     * AES-128 IV test
     * AES key = 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f
     */
    uint8_t key[16];
    for (uint32_t i = 0; i < 8; i++) {
        uint32_t nibble0, nibble1;
        nibble0 = 2 * i;
        nibble1 = 2 * i + 1;

        input[i] = (nibble0 << 4) | nibble1;
        key[i] = (nibble0 << 4) | nibble1;

        input[15 - i] = (nibble0) | (nibble1 << 4);
        key[15 - i] = (nibble0) | (nibble1 << 4);
    }

    sdp_sm4_ctx_t *p_sys_sdp_ctx = (sdp_sm4_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_aes_ctx);

    printf("rom_sdp_sm4_crypt_ecb, 128-bit key test.\n");
    (void) rom_sdp_sm4_set_key(p_sys_sdp_ctx, key, sdp_sm4_keybits_128, 0);

    uint8_t *p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &input);
    uint8_t *p_sys_output = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &output);
    hpm_stat_t status = rom_sdp_sm4_crypt_ecb(p_sys_sdp_ctx, sdp_sm4_op_encrypt, 16, p_sys_input, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("rom_sdp_sm4_crypt_ecb() encryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, gold_ct128, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("rom_sdp_sm4_crypt_ecb encryption test passed\n");
        }
    }

    status = rom_sdp_sm4_crypt_ecb(p_sys_sdp_ctx, sdp_sm4_op_decrypt, 16, p_sys_output, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("rom_sdp_sm4_crypt_ecb() decryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, input, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("rom_sdp_sm4_crypt_ecb decryption test passed\n");
        }
    }


    return (error_cnt == 0U) ? status_success : status_fail;
}

static hpm_stat_t sdp_api_sm4_cbc_demo(void)
{
    ATTR_PLACE_AT_NONCACHEABLE_INIT static uint8_t input[16];
    ATTR_PLACE_AT_NONCACHEABLE static uint8_t output[16];

    hpm_stat_t status;
    uint32_t error_cnt = 0;
    sdp_sm4_ctx_t *p_sys_sdp_ctx = (sdp_aes_ctx_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &s_aes_ctx);
    const uint8_t gold_ct128[16] = {
        0x5d, 0x1f, 0xee, 0x63, 0xf5, 0xeb, 0x8b, 0xb5, 0x03, 0x58, 0x0a, 0xb8, 0x23, 0x92, 0x5d, 0x55
    };

    uint8_t key[16];
    uint8_t iv[16];

    for (uint32_t i = 0; i < 8; i++) {
        uint32_t nibble0, nibble1;
        nibble0 = 2 * i;
        nibble1 = 2 * i + 1;

        input[i] = (nibble0 << 4) | nibble1;
        key[i] = (nibble0 << 4) | nibble1;

        input[15 - i] = (nibble0) | (nibble1 << 4);
        key[15 - i] = (nibble0) | (nibble1 << 4);
    }

    for (uint32_t i = 0; i < sizeof(iv); i++) {
        iv[i] = (i << 4) | i;
    }


    uint8_t *p_sys_input = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &input);
    uint8_t *p_sys_output = (uint8_t *) core_local_mem_to_sys_address(HPM_CORE0, (uint32_t) &output);

    /* SM4 CBC test */
    printf("SM4 CBC test...\n");
    (void) rom_sdp_sm4_set_key(&s_aes_ctx, key, sdp_sm4_keybits_128, 0);
    status = rom_sdp_sm4_crypt_cbc(p_sys_sdp_ctx, sdp_sm4_op_encrypt, sizeof(input), iv, p_sys_input, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("sdp_sm4_crypt_cbc() encryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, gold_ct128, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("sdp_sm4_crypt_cbc() encryption passed\n");
        }
    }

    status = rom_sdp_sm4_crypt_cbc(p_sys_sdp_ctx, sdp_sm4_op_decrypt, sizeof(input), iv, p_sys_output, p_sys_output);
    if (status != status_success) {
        ++error_cnt;
        printf("Decryption failed, status=%d\n", status);
    } else {
        if (memcmp(output, input, 16) != 0) {
            ++error_cnt;
            printf("Opps, result doesn't match the golden data\n");
        } else {
            printf("sdp_sm4_crypt_cbc() decryption passed\n");
        }
    }


    return (error_cnt == 0U) ? status_success : status_fail;
}
#endif
