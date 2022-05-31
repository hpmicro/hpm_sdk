/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_sdmmc_sd.h"

static sd_card_t g_sd;

ATTR_PLACE_AT_NONCACHEABLE static uint32_t s_write_buf[4096];
ATTR_PLACE_AT_NONCACHEABLE static uint32_t s_read_buf[4096];


void show_help(void);
void test_write_read_last_block(void);
void test_write_read_last_1024_blocks(void);


int main(void)
{
    board_init();

    if (sd_init(&g_sd) != status_success) {
        printf("SD Card initialization failed\n");
        return status_fail;
    }
    printf("SD Card initialization succeeded\n");
    printf("Card Info:\n-----------------------------------------------\n");
    printf("Card Size in GBytes:    %.2fGB\n", g_sd.card_size_in_bytes * 1.0f / 1024UL / 1024UL / 1024UL);
    printf("Total Block Counts: %u\n", g_sd.block_count);
    printf("Block Size: %d Bytes\n", g_sd.block_size);
    printf("Maximum supported frequency: %uMHz\n", g_sd.max_freq / 1000000UL);


    show_help();
    while (true) {
        char opt_char = getchar();
        putchar(opt_char);
        putchar('\n');

        switch (opt_char) {
        default:
            show_help();
            break;
        case '1':
            test_write_read_last_block();
            break;
        case '2':
            test_write_read_last_1024_blocks();
            break;
        }
    }

    return 0;
}



void show_help(void)
{
    static const char help_info[] = "\n"
                             "-----------------------------------------------------------------------------------\n"
                             "*                                                                                 *\n"
                             "*                   SD Card Low-level test demo                                   *\n"
                             "*                                                                                 *\n"
                             "*        1. Write & Read the last block                                           *\n"
                             "*        2. Write & Read the last 1024 blocks                                     *\n"
                             "*                                                                                 *\n"
                             "*---------------------------------------------------------------------------------*\n";
    printf("%s", help_info);
}

void test_write_read_last_block(void)
{
    bool result = false;
    hpm_stat_t status = status_success;
    uint32_t sector_addr = g_sd.block_count - 1U;

    uint8_t *buf_8 = (uint8_t *)&s_write_buf;
    for (uint32_t i = 0; i < g_sd.block_size; i++) {
        buf_8[i] = (uint8_t)(i & 0xFFU);
    }


    do {
        result = false;
        status = sd_write_blocks(&g_sd, (uint8_t *) s_write_buf, sector_addr, 1);
        if (status != status_success) {
            break;
        }
        status = sd_read_blocks(&g_sd, (uint8_t *) s_read_buf, sector_addr, 1);
        if (status != status_success) {
            break;
        }
        result = (memcmp(s_write_buf, s_read_buf, g_sd.block_size) == 0);
        printf("SD write-read-verify block 0x%08x %s\n", sector_addr, result ? "PASSED" : "FAILED");
        if (!result) {
            break;
        }
    } while (false);
    printf("Test completed, %s\n", result ? "PASSED" : "FAILED");
}

void test_write_read_last_1024_blocks(void)
{
    uint32_t sector_addr = g_sd.block_count - 1024U;
    hpm_stat_t status;

    for (uint32_t i = 0; i < ARRAY_SIZE(s_write_buf); i++) {
        s_write_buf[i] = i << 16 | i;
    }


    bool result = false;
    uint32_t step = sizeof(s_write_buf) / 512;
    for (uint32_t i = 0; i < 1024; i += step) {
        result = false;
        status = sd_write_blocks(&g_sd, (uint8_t *) s_write_buf, sector_addr + i, step);
        if (status != status_success) {
            break;
        }
        status = sd_read_blocks(&g_sd, (uint8_t *) s_read_buf, sector_addr + i, step);
        if (status != status_success) {
            break;
        }
        result = (memcmp(s_write_buf, s_read_buf, sizeof(s_write_buf)) == 0);
        printf("SD write-read-verify block range 0x%08x-0x%08x %s\n", sector_addr + i, sector_addr + i + step - 1U, result ? "PASSED" : "FAILED");
        if (!result) {
            break;
        }
    }
    printf("Test completed, %s\n", result ? "PASSED" : "FAILED");
}