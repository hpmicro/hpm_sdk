/*
 * Copyright (c) 2021-2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_sdmmc_emmc.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_clock_drv.h"

#define SIZE_1GB (SIZE_1MB * SIZE_1KB)
ATTR_PLACE_AT_NONCACHEABLE_BSS sdmmc_host_t g_sdmmc_host;
static emmc_card_t g_emmc = {.host = &g_sdmmc_host};

/***********************************************************************************
 *
 *  NOTE: To achieve maximum read/write performance, user needs to increase the
 *        MAX_BUF_SIZE_DEFAULT, for example 128*1024U
 *
 ************************************************************************************/
#ifdef INIT_EXT_RAM_FOR_DATA
#define MAX_BUF_SIZE_DEFAULT (512*1024U)
#else
#define MAX_BUF_SIZE_DEFAULT (64*1024U)
#endif

ATTR_PLACE_AT_NONCACHEABLE_BSS uint32_t s_write_buf[MAX_BUF_SIZE_DEFAULT / sizeof(uint32_t)];
ATTR_PLACE_AT_NONCACHEABLE_BSS uint32_t s_read_buf[MAX_BUF_SIZE_DEFAULT / sizeof(uint32_t)];

#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
SDK_DECLARE_EXT_ISR_M(BOARD_APP_EMMC_SDXC_IRQ, sdxc_isr)
void sdxc_isr(void)
{
    sdmmchost_irq_handler(&g_sdmmc_host);
}
#endif

void show_help(void);

void show_emmc_info(emmc_card_t *card);

void test_write_read_last_block(void);

void test_write_read_last_1024_blocks(void);

void test_emmc_sleep_awake(void);


int main(void)
{
    hpm_stat_t status;

    board_init();

    do {
        status = board_init_emmc_host_params(&g_sdmmc_host, BOARD_APP_EMMC_SDXC_BASE);
        if (status != status_success) {
            break;
        }
#if defined(HPM_SDMMC_HOST_ENABLE_IRQ) && (HPM_SDMMC_HOST_ENABLE_IRQ == 1)
        intc_m_enable_irq_with_priority(BOARD_APP_EMMC_SDXC_IRQ, 1);
#endif

        status = emmc_init(&g_emmc);
    } while (false);

    if (status == status_success) {
        show_emmc_info(&g_emmc);

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
                test_emmc_sleep_awake();
                break;
            }
        }
    } else {
        printf("eMMC initialization failed, status=%d\n", status);
    }

    return 0;
}


void show_help(void)
{
    static const char help_info[] = "\n"
                                    "-----------------------------------------------------------------------------------\n"
                                    "*                                                                                 *\n"
                                    "*                   eMMC Low-level test demo                                      *\n"
                                    "*                                                                                 *\n"
                                    "*        1. Write & Read the last block                                           *\n"
                                    "*        2. Write & Read the last 1024 blocks                                     *\n"
                                    "*        3. Demonstrate eMMC sleep & awake                                        *\n"
                                    "*                                                                                 *\n"
                                    "*---------------------------------------------------------------------------------*\n";
    printf("%s", help_info);
}

void test_write_read_last_block(void)
{
    bool result = false;
    hpm_stat_t status = status_success;
    uint32_t sector_addr = g_emmc.device_attribute.sector_count - 1U;

    uint8_t *buf_8 = (uint8_t *) &s_write_buf;
    for (uint32_t i = 0; i < g_emmc.device_attribute.sector_size; i++) {
        buf_8[i] = (uint8_t) (i & 0xFFU);
    }

    do {
        result = false;
        status = emmc_write_blocks(&g_emmc, (uint8_t *) s_write_buf, sector_addr, 1);
        if (status != status_success) {
            break;
        }
        status = emmc_read_blocks(&g_emmc, (uint8_t *) s_read_buf, sector_addr, 1);
        if (status != status_success) {
            break;
        }
        result = (memcmp(s_write_buf, s_read_buf, g_emmc.device_attribute.sector_size) == 0);
        printf("eMMC write-read-verify block 0x%08x %s\n", sector_addr, result ? "PASSED" : "FAILED");
        if (!result) {
            break;
        }
    } while (false);
    printf("Test completed, %s, error code=%d\n", result ? "PASSED" : "FAILED", status);
}

void test_write_read_last_1024_blocks(void)
{
    uint32_t sector_addr = g_emmc.device_attribute.sector_count - 1024U;
    hpm_stat_t status;

    for (uint32_t i = 0; i < ARRAY_SIZE(s_write_buf); i++) {
        s_write_buf[i] = i << 16 | i;
    }

    status = emmc_erase_blocks(&g_emmc, sector_addr, 1024, emmc_erase_option_erase);
    if (status != status_success) {
        printf("eMMC Erase operation failed, status=%d\n", status);
    }
    bool result = false;
    uint32_t step = sizeof(s_write_buf) / 512;
    uint64_t write_ticks = 0;
    uint64_t read_ticks = 0;
    if (step > 1024) {
        step = 1024;
    }
    for (uint32_t i = 0; i < 1024; i += step) {
        result = false;
        uint64_t start_ticks = mchtmr_get_count(HPM_MCHTMR);
        status = emmc_write_blocks(&g_emmc, (uint8_t *) s_write_buf, sector_addr + i, step);
        if (status != status_success) {
            break;
        }
        uint64_t end_ticks = mchtmr_get_count(HPM_MCHTMR);
        write_ticks += (end_ticks - start_ticks);

        start_ticks = mchtmr_get_count(HPM_MCHTMR);
        status = emmc_read_blocks(&g_emmc, (uint8_t *) s_read_buf, sector_addr + i, step);
        if (status != status_success) {
            break;
        }
        end_ticks = mchtmr_get_count(HPM_MCHTMR);
        read_ticks += (end_ticks - start_ticks);
        result = (memcmp(s_write_buf, s_read_buf, sizeof(s_write_buf)) == 0);
        printf("eMMC write-read-verify block range 0x%08x-0x%08x %s\n", sector_addr + i, sector_addr + i + step - 1U,
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
        uint32_t xfer_bytes = 1024 * 512U;
        float write_speed = 1.0f * xfer_bytes / (1.0f * write_ticks / clock_get_frequency(clock_mchtmr0));
        float read_speed = 1.0f * xfer_bytes / (1.0f * read_ticks / clock_get_frequency(clock_mchtmr0));

        printf("Write Speed: %.2fMB/s, Read Speed: %.2fMB/s\n", write_speed / 1024 / 1024, read_speed / 1024 / 1024);
        printf("NOTE: Increasing the MAX_BUF_SIZE_DEFAULT can achieve higher Read/write performance\n");
    }
}

void show_emmc_info(emmc_card_t *card)
{
    printf("eMMC Info:\n--------------------------------------------------------\n");
    printf("CID:0x%08x 0x%08x 0x%08x 0x%08x\n", card->cid.cid_words[0], card->cid.cid_words[1], card->cid.cid_words[2],
           card->cid.cid_words[3]);
    printf("MID:0x%02x\n", card->cid.mid);
    printf("Product Name: ");
    for (int32_t i = 5; i >= 0; i--) {
        printf("%c", card->cid.pnm[i]);
        if (i == 0) {
            printf("\n");
        }
    }
    const char *timing_mode_str = NULL;
    switch (card->current_hs_timing) {
    case emmc_timing_legacy:
        timing_mode_str = "Legacy";
        break;
    case emmc_timing_high_speed:
        timing_mode_str = "High Speed";
        break;
    case emmc_timing_high_speed_ddr:
        timing_mode_str = "High Speed DDR";
        break;
    case emmc_timing_hs200:
        timing_mode_str = "HS200";
        break;
    case emmc_timing_hs400:
        timing_mode_str = "HS400";
        break;
    }
    uint32_t bus_wdith = 1;
    switch (card->current_bus_mode) {
    case emmc_bus_mode_x4_sdr:
    case emmc_bus_mode_x4_ddr:
        bus_wdith = 4;
        break;
    case emmc_bus_mode_x8_sdr:
    case emmc_bus_mode_x8_ddr:
    case emmc_bus_mode_x8_ddr_ds:
        bus_wdith = 8;
        break;
    default:
        bus_wdith = 1;
        break;
    }

    printf("eMMC Clock: %.1fMHz\n", card->host->clock_freq * 1.0 / 1000000);
    printf("eMMC Speed: %s\n", timing_mode_str);
    printf("eMMC Bus Width: %d\n", bus_wdith);
    printf("eMMC size: %ldMB\n", (uint32_t) (card->device_attribute.device_size_in_bytes / SIZE_1MB));
    printf("Boot partition size: %dKB\n", card->device_attribute.boot_partition_size / SIZE_1KB);
    printf("RPMB partition size: %dKB\n", card->device_attribute.rpmb_partition_size / SIZE_1KB);
    printf("Sectors: %d\n", card->device_attribute.sector_count);
    printf("Sector Size: %d\n", card->device_attribute.sector_size);
}

void test_emmc_sleep_awake(void)
{
    printf("Demonstrate the eMMC sleep/awake feature\n");

    static bool stay_in_sleep_mode = false;
    if (!stay_in_sleep_mode) {
        emmc_select_card(&g_emmc, false);
        emmc_enter_sleep_mode(&g_emmc);
        stay_in_sleep_mode = true;
        printf("Switched eMMC to sleep mode\n");
    } else {
        emmc_exit_sleep_mode(&g_emmc);
        emmc_select_card(&g_emmc, true);
        stay_in_sleep_mode = false;
        printf("Awoken eMMC from sleep mode\n");
    }
}
