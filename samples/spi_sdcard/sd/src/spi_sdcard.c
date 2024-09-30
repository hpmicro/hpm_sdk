/*
 * Copyright (c) 2021-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "spi_sd_adapt.h"
#include "hpm_spi_sdcard.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_dma_mgr.h"
#define WRITE_READ_BLOCK_COUNT     (20U)

static void display_sdcard_info(void);
static void test_write_read_last_block(void);

ATTR_PLACE_AT_NONCACHEABLE uint8_t s_write_buf[SPI_SD_BLOCK_SIZE * WRITE_READ_BLOCK_COUNT];
ATTR_PLACE_AT_NONCACHEABLE uint8_t s_read_buf[SPI_SD_BLOCK_SIZE * WRITE_READ_BLOCK_COUNT];

static spi_sdcard_info_t g_sd_info;

int main(void)
{
    board_init();
    dma_mgr_init();
    if (spi_sd_init() == status_success) {
        display_sdcard_info();
        test_write_read_last_block();
    } else {
        printf("spi sdcard init fail!\n");
    }
    while (1) {
    }
}

static void display_sdcard_info(void)
{
    if (sdcard_spi_get_card_info(&g_sd_info) == status_success) {
        printf("SD Card initialization succeeded\n");
        printf("Card Info:\n-----------------------------------------------\n");
        printf("Card Size in GBytes:    %.2fGB\n", g_sd_info.capacity * 1.0f / 1024UL / 1024UL / 1024UL);
        printf("Block count: %d block\n", (uint32_t)g_sd_info.block_count);
        printf("Block Size: %d Bytes\n", g_sd_info.block_size);
        printf("\n-----------------------------------------------\n");
    }
}

static void test_write_read_last_block(void)
{
    bool result = false;
    uint32_t i;
    hpm_stat_t status = status_success;
    uint64_t write_ticks = 0;
    uint64_t read_ticks = 0;
    uint32_t start_sector_addr = g_sd_info.block_count - WRITE_READ_BLOCK_COUNT;
    uint32_t end_sector_addr   = g_sd_info.block_count - 1U;
    uint8_t *buf_8 = (uint8_t *)&s_write_buf;
    for (uint32_t i = 0; i < g_sd_info.block_size; i++) {
        buf_8[i] = (uint8_t)(i & 0xFFU);
    }

    do {
        result = false;
        printf("sdcard wiite/read verify ....\n");
        for (i = start_sector_addr; i < end_sector_addr; i++) {
            memset(s_write_buf, 0, sizeof(s_write_buf));
            memset(s_read_buf, 0, sizeof(s_read_buf));
            status = sdcard_spi_write_block(i, (uint8_t *)s_write_buf);
            if (status != status_success) {
                break;
            }
            printf("sd block_%u write PASSED\n", i);
            status = sdcard_spi_read_block(i, (uint8_t *) s_read_buf);
            if (status != status_success) {
                break;
            }
            printf("sd block_%u read PASSED\n", i);
            result = (memcmp(s_write_buf, s_read_buf, g_sd_info.block_size) == 0);
            printf("SD write-read-verify block 0x%08x %s\n", i, result ? "PASSED" : "FAILED");
            if (!result) {
                break;
            }
        }
        if (i < end_sector_addr) {
            printf("\nSD write-read-verify can't reach the total count:%d  FAILED!!!!\n\n", i - start_sector_addr);
        } else {
            printf("\nSD write-read-verify reach the total count:%d  PASSED!!!!\n\n", i - start_sector_addr);
        }

        printf("sdcard wiite/read speed test....\n", i);
         for (i = 0; i < ARRAY_SIZE(s_write_buf); i++) {
            s_write_buf[i] = i << 16 | i;
        }
        uint64_t start_ticks = mchtmr_get_count(HPM_MCHTMR);
        status = sdcard_spi_write_multi_block(s_write_buf, start_sector_addr, WRITE_READ_BLOCK_COUNT);
        if (status != status_success) {
            printf("sdcard multi write failed\n");
            break;
        }
        uint64_t end_ticks = mchtmr_get_count(HPM_MCHTMR);
        write_ticks += (end_ticks - start_ticks);

        start_ticks = mchtmr_get_count(HPM_MCHTMR);
        status = sdcard_spi_read_multi_block(s_read_buf, start_sector_addr, WRITE_READ_BLOCK_COUNT);
        if (status != status_success) {
            printf("sdcard multi read failed\n");
            break;
        }
        end_ticks = mchtmr_get_count(HPM_MCHTMR);
        read_ticks += (end_ticks - start_ticks);
        result = (memcmp(s_write_buf, s_read_buf, sizeof(s_write_buf)) == 0);
        printf("SD write-read-verify block range 0x%08x-0x%08x %s\n",
                   start_sector_addr,
                   end_sector_addr,
                   result ? "PASSED" : "FAILED");
        if (result) {
            uint32_t xfer_bytes = WRITE_READ_BLOCK_COUNT * 512U;
            float write_speed = 1.0f * xfer_bytes / (1.0f * write_ticks / clock_get_frequency(clock_mchtmr0));
            float read_speed = 1.0f * xfer_bytes / (1.0f * read_ticks / clock_get_frequency(clock_mchtmr0));

            printf("Write Speed: %.2fKB/s, Read Speed: %.2fKB/s\n", write_speed / 1024, read_speed / 1024);
        }
    } while (false);
    printf("Test completed, %s\n", result ? "PASSED" : "FAILED");
}


