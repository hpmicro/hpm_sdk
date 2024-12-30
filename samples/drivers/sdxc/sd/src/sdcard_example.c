/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_sdmmc_sd.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_clock_drv.h"

ATTR_PLACE_AT_NONCACHEABLE_BSS sdmmc_host_t g_sdmmc_host;
static sd_card_t g_sd = { .host = &g_sdmmc_host };

/***********************************************************************************
 *
 *  NOTE: To achieve maximum read/write performance, user needs to increase the
 *        MAX_BUF_SIZE_DEFAULT, for example 128*1024U
 *
 ************************************************************************************/
#ifdef INIT_EXT_RAM_FOR_DATA
#define MAX_BUF_SIZE_DEFAULT (512U * 1024U)
#else
#define MAX_BUF_SIZE_DEFAULT (32U * 1024U)
#endif

ATTR_PLACE_AT_NONCACHEABLE uint32_t s_write_buf[MAX_BUF_SIZE_DEFAULT / sizeof(uint32_t)];
ATTR_PLACE_AT_NONCACHEABLE uint32_t s_read_buf[MAX_BUF_SIZE_DEFAULT / sizeof(uint32_t)];

#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
SDK_DECLARE_EXT_ISR_M(BOARD_APP_SDCARD_SDXC_IRQ, sdxc_isr)
void sdxc_isr(void)
{
    sdmmchost_irq_handler(&g_sdmmc_host);
}
#endif

static void show_card_info(const sd_card_t *card);

void show_help(void);

void test_write_read_last_block(void);

void test_write_read_last_1024_blocks(void);

void test_hot_plug(void);

void test_sd_stress_test(void);


static void show_card_info(const sd_card_t *card)
{
    printf("SD Card initialization succeeded\n");
    printf("Card Info:\n-----------------------------------------------\n");
    printf("Card Size in GBytes:    %.2fGB\n", card->card_size_in_bytes * 1.0f / 1024UL / 1024UL / 1024UL);
    printf("Total Block Counts: %u\n", card->block_count);
    printf("Block Size: %d Bytes\n", card->block_size);
    printf("Card class: %d\n", card->status.speed_class);
    printf("Maximum Card Supported Frequency: %uMHz\n", card->max_freq / 1000000UL);
    printf("Current Host Clock Frequency:     %uMHz\n", card->host->clock_freq / 1000000UL);

    if (card->operation_voltage == sdmmc_operation_voltage_1v8) {
        switch (card->status.uhs_speed_grade) {
        case 0:
            printf("UHS Speed Grade: Less than 10MB/sec\n");
            break;
        case 1:
            printf("UHS Speed Grade: 10MB/sec and above\n");
            break;
        case 3:
            printf("UHS Speed Grade: 30MB/sec and above\n");
            break;
        default:
            break;
        }
    }

    if (card->operation_voltage == sdmmc_operation_voltage_1v8) {
        printf("SD operation voltage is 1.8V\n");
    } else {
        printf("SD operation voltage is 3V\n");
    }
}

int main(void)
{
    sd_card_t *card = &g_sd;
    board_init();
    hpm_stat_t status;
    do {
        printf("Please insert the SD card to SD slot...\n");
        status = board_init_sd_host_params(&g_sdmmc_host, BOARD_APP_SDCARD_SDXC_BASE);
        if (status != status_success) {
            break;
        }

#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
        intc_m_enable_irq_with_priority(BOARD_APP_SDCARD_SDXC_IRQ, 1);
#endif

        status = sd_init(card);
        if (status != status_success) {
            board_delay_ms(1000);
        }
    } while (status != status_success);

    show_card_info(card);

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
        case '3':
            test_hot_plug();
            break;
        case '4':
            test_sd_stress_test();
            break;
        }
    }

    return 0;
}


void show_help(void)
{
    const char help_info[] = "\n"
                             "-----------------------------------------------------------------------------------\n"
                             "*                                                                                 *\n"
                             "*                   SD Card Low-level test demo                                   *\n"
                             "*                                                                                 *\n"
                             "*        1. Write & Read the last block                                           *\n"
                             "*        2. Write & Read the last 1024 blocks                                     *\n"
                             "*        3. Hot plug test                                                         *\n"
                             "*        4. SD Stress test (Write / Read 200MBytes)                               *\n"
                             "*                                                                                 *\n"
                             "*---------------------------------------------------------------------------------*\n";
    printf("%s", help_info);
}

void test_write_read_last_block(void)
{
    bool result = false;
    hpm_stat_t status;
    uint32_t sector_addr = g_sd.block_count - 1U;

    uint8_t *buf_8 = (uint8_t *) &s_write_buf;
    for (uint32_t i = 0; i < g_sd.block_size; i++) {
        buf_8[i] = (uint8_t) (i & 0xFFU);
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

    srand((unsigned int) HPM_MCHTMR->MTIME);
    for (uint32_t i = 0; i < ARRAY_SIZE(s_write_buf); i++) {
        s_write_buf[i] = ((uint32_t) rand() << 16) | rand();
    }

    status = sd_erase_blocks(&g_sd, sector_addr, 1024);
    if (status != status_success) {
        printf("SD Card Erase operation failed, status=%d\n", status);
    }
    bool result = false;
    uint32_t step = sizeof(s_write_buf) / g_sd.block_size;
    uint64_t write_ticks = 0;
    uint64_t read_ticks = 0;
    if (step > 1024) {
        step = 1024;
    }
    for (uint32_t i = 0; i < 1024; i += step) {
        result = false;
        if ((i + step) > 1024) {
            step = 1024 - i;
        }
        uint64_t start_ticks = mchtmr_get_count(HPM_MCHTMR);
        status = sd_write_blocks(&g_sd, (uint8_t *) s_write_buf, sector_addr + i, step);
        if (status != status_success) {
            break;
        }
        uint64_t end_ticks = mchtmr_get_count(HPM_MCHTMR);
        write_ticks += (end_ticks - start_ticks);

        start_ticks = mchtmr_get_count(HPM_MCHTMR);
        status = sd_read_blocks(&g_sd, (uint8_t *) s_read_buf, sector_addr + i, step);
        if (status != status_success) {
            break;
        }
        end_ticks = mchtmr_get_count(HPM_MCHTMR);
        read_ticks += (end_ticks - start_ticks);
        result = (memcmp(s_write_buf, s_read_buf, sizeof(s_write_buf)) == 0);
        printf("SD write-read-verify block range 0x%08x-0x%08x %s\n",
               sector_addr + i,
               sector_addr + i + step - 1U,
               result ? "PASSED" : "FAILED");
        if (!result) {
            break;
        }
    }

    if (status != status_success) {
        printf("Error code: %d\n", status);
    }
    printf("Test completed, %s\n", result ? "PASSED" : "FAILED");

    if (result) {
        uint32_t xfer_bytes = 1024 * g_sd.block_size;
        float write_speed = 1.0f * xfer_bytes / (1.0f * write_ticks / clock_get_frequency(clock_mchtmr0));
        float read_speed = 1.0f * xfer_bytes / (1.0f * read_ticks / clock_get_frequency(clock_mchtmr0));

        printf("Write Speed: %.2fMB/s, Read Speed: %.2fMB/s\n", write_speed / 1024 / 1024, read_speed / 1024 / 1024);
        printf("NOTE: Increasing the MAX_BUF_SIZE_DEFAULT can achieve higher Read/write performance\n");
    }
}

void test_hot_plug(void)
{
    hpm_stat_t status;

    if (sdmmchost_is_card_detected(g_sd.host)) {
        show_card_info(&g_sd);
    }

    while (true) {
        if ((!sdmmchost_is_card_detected(g_sd.host)) || !g_sd.host->card_init_done) {
            printf("Please insert an SD card...\n");
            status = sd_init(&g_sd);
            if (status == status_success) {
                printf("SD Card initialization succeeded\n");
                show_card_info(&g_sd);
                test_write_read_last_1024_blocks();
            }
        }
    }
}

void test_sd_stress_test(void)
{
    hpm_stat_t status;

    printf("SD card stress test...\n");
    srand((unsigned int) HPM_MCHTMR->MTIME);
    for (uint32_t i = 0; i < ARRAY_SIZE(s_write_buf); i++) {
        s_write_buf[i] = ((uint32_t) rand() << 16) | rand();
    }

    /* Erase, Write, read 200MB */
    uint32_t max_test_blocks = MIN(200 * 1024 * 1024 / g_sd.block_size, g_sd.block_count);

    uint32_t sector_addr;
    uint32_t start_sector = g_sd.block_count - max_test_blocks;
    uint64_t write_ticks = 0;
    uint64_t read_ticks = 0;
    bool result = false;
    uint32_t step = sizeof(s_write_buf) / g_sd.block_size;
    uint32_t blocks_per_loop;
    for (uint32_t offset = 0; offset < max_test_blocks; offset += step) {
        sector_addr = start_sector + offset;
        blocks_per_loop = MIN(step, (max_test_blocks - offset));
        result = false;
        uint64_t start_ticks = mchtmr_get_count(HPM_MCHTMR);
        status = sd_write_blocks(&g_sd, (uint8_t *) s_write_buf, sector_addr, blocks_per_loop);
        if (status != status_success) {
            break;
        }
        uint64_t end_ticks = mchtmr_get_count(HPM_MCHTMR);
        write_ticks += (end_ticks - start_ticks);

        start_ticks = mchtmr_get_count(HPM_MCHTMR);
        status = sd_read_blocks(&g_sd, (uint8_t *) s_read_buf, sector_addr, blocks_per_loop);
        if (status != status_success) {
            break;
        }
        end_ticks = mchtmr_get_count(HPM_MCHTMR);
        read_ticks += (end_ticks - start_ticks);
        result = (memcmp(s_write_buf, s_read_buf, sizeof(s_write_buf)) == 0);
        if (!result) {
            printf("SD write-read-verify block range 0x%08x-0x%08x %s\n",
                   sector_addr,
                   sector_addr + blocks_per_loop - 1U,
                   result ? "PASSED" : "FAILED");
            break;
        } else {
            printf(".");
#if !defined(__ICCRISCV__) || (defined(_DLIB_FILE_DESCRIPTOR) && (_DLIB_FILE_DESCRIPTOR == 1))
            fflush(stdout);
#endif
        }
    }
    printf("\n");
    printf("Test completed, %s\n", result ? "PASSED" : "FAILED");

    if (result) {
        uint32_t xfer_bytes = max_test_blocks * g_sd.block_size;
        float write_speed = 1.0f * xfer_bytes / (1.0f * write_ticks / clock_get_frequency(clock_mchtmr0));
        float read_speed = 1.0f * xfer_bytes / (1.0f * read_ticks / clock_get_frequency(clock_mchtmr0));

        printf("Write Speed: %.2fMB/s, Read Speed: %.2fMB/s\n", write_speed / 1024 / 1024, read_speed / 1024 / 1024);
        printf("NOTE: Increasing the MAX_BUF_SIZE_DEFAULT can achieve higher Read/write performance\n");
    }
}
