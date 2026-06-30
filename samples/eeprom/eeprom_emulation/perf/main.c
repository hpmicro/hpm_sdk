/*
 * Copyright (c) 2023,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "eeprom_emulation.h"
#include "hpm_nor_flash.h"

#define DEMO_RX_SIZE        (512)
#define DEMO_BLOCK_SIZE_MAX (32)
#define DEMO_WRITE_CYCLE    (5)
#define DEMO_TEST_VAR_CNT   (50)
#define DEMO_ERASE_SIZE     (4096)
#define DEMO_SECTOR_CNT     (32)
#define DEMO_MANAGE_SIZE    (DEMO_ERASE_SIZE * DEMO_SECTOR_CNT)
#define DEMO_MANAGE_OFFSET  (BOARD_FLASH_SIZE - DEMO_MANAGE_SIZE * 2)

#define PERF_CONFIG_TEST     ('1')
#define PERF_WRITE_TEST      ('2')
#define PERF_FLUSH_TEST      ('3')
#define PERF_READ_TEST       ('4')
#define PERF_DELETE_TEST     ('5')
#define PERF_SHOW_INFO       ('6')
#define PERF_SHOW_INDEX      ('i')

static nor_flash_config_t g_nor_cfg;
static e2p_t g_e2p_ctx;

static uint8_t g_read_buf[DEMO_RX_SIZE];

static uint64_t g_perf_start_time;

static hpm_stat_t demo_flash_read(uint8_t *buf, uint32_t addr, uint32_t size)
{
    return nor_flash_read(&g_nor_cfg, buf, addr, size);
}

static hpm_stat_t demo_flash_write(const uint8_t *buf, uint32_t addr, uint32_t size)
{
    return nor_flash_write(&g_nor_cfg, buf, addr, size);
}

static hpm_stat_t demo_flash_erase(uint32_t start_addr, uint32_t size)
{
    return nor_flash_erase(&g_nor_cfg, start_addr, size);
}

static void perf_timer_start(void)
{
    g_perf_start_time = hpm_csr_get_core_mcycle();
}

static uint32_t perf_timer_stop_us(void)
{
    uint64_t elapsed = hpm_csr_get_core_mcycle() - g_perf_start_time;
    uint32_t tick_per_us = clock_get_frequency(clock_cpu0) / 1000 / 1000;
    return (uint32_t)(elapsed / tick_per_us);
}

static void eeprom_init(void)
{
    nor_flash_init(&g_nor_cfg);

    g_e2p_ctx.config.start_addr = g_nor_cfg.base_addr + DEMO_MANAGE_OFFSET;
    g_e2p_ctx.config.erase_size = DEMO_ERASE_SIZE;
    g_e2p_ctx.config.sector_cnt = DEMO_SECTOR_CNT;
    g_e2p_ctx.config.version = 0x4553; /* 'E' 'S' */
    g_e2p_ctx.config.flash_read = demo_flash_read;
    g_e2p_ctx.config.flash_write = demo_flash_write;
    g_e2p_ctx.config.flash_erase = demo_flash_erase;

    e2p_config(&g_e2p_ctx);
}

static void perf_write_data(void)
{
    uint32_t var_count = 0;
    uint32_t blob_data[2] = {0, 0};
    int cycle_count;

    printf("Writing %d variables, %d updates each...\n", DEMO_TEST_VAR_CNT, DEMO_WRITE_CYCLE);

    while (var_count < DEMO_TEST_VAR_CNT) {
        cycle_count = 0;
        blob_data[0] = E2P_MAGIC_ID;

        while (cycle_count < DEMO_WRITE_CYCLE) {
            blob_data[1] += 10;
            if (e2p_write(var_count, sizeof(blob_data), (uint8_t *)blob_data) != E2P_STATUS_OK) {
                printf("Write failed at var %d, cycle %d\n", var_count, cycle_count);
            }
            cycle_count++;
        }
        var_count++;
    }

    printf("Write completed: %d variables x %d updates = %d total writes\n",
           DEMO_TEST_VAR_CNT, DEMO_WRITE_CYCLE, DEMO_TEST_VAR_CNT * DEMO_WRITE_CYCLE);
}

static uint32_t perf_test_config(void)
{
    uint32_t time_us;

    printf("\n=== Test: Configuration (Init) ===\n");

    e2p_clear();
    g_e2p_ctx.config.sector_cnt = DEMO_SECTOR_CNT;
    perf_timer_start();
    if (e2p_config(&g_e2p_ctx) != E2P_STATUS_OK) {
        printf("Config failed\n");
        return 0;
    }
    time_us = perf_timer_stop_us();

    printf("  Config time: %u us\n", time_us);
    return time_us;
}

static uint32_t perf_test_write(void)
{
    uint32_t time_us;

    g_e2p_ctx.config.sector_cnt = DEMO_SECTOR_CNT;
    printf("\n=== Test: Write Performance ===\n");

    e2p_clear();
    e2p_config(&g_e2p_ctx);

    perf_timer_start();
    perf_write_data();
    time_us = perf_timer_stop_us();

    uint32_t total_writes = DEMO_TEST_VAR_CNT * DEMO_WRITE_CYCLE;
    printf("  Total writes: %u\n", total_writes);
    printf("  Total time: %u us\n", time_us);
    printf("  Avg per write: %u us\n", time_us / total_writes);
    return time_us;
}

static uint32_t perf_test_flush(void)
{
    uint32_t time_us;

    g_e2p_ctx.config.sector_cnt = DEMO_SECTOR_CNT;
    printf("\n=== Test: Flush Performance ===\n");

    e2p_clear();
    e2p_config(&g_e2p_ctx);
    perf_write_data();

    perf_timer_start();
    if (e2p_flush(E2P_FLUSH_FORCE) != E2P_STATUS_OK) {
        printf("Flush failed\n");
        return 0;
    }
    time_us = perf_timer_stop_us();

    printf("  Flush time: %u us\n", time_us);
    return time_us;
}

static uint32_t perf_test_read(void)
{
    uint32_t time_us;
    uint32_t success_reads = 0;

    g_e2p_ctx.config.sector_cnt = DEMO_SECTOR_CNT;
    printf("\n=== Test: Read Performance ===\n");

    e2p_clear();
    e2p_config(&g_e2p_ctx);
    perf_write_data();

    perf_timer_start();
    for (uint32_t i = 0; i < DEMO_TEST_VAR_CNT; i++) {
        if (e2p_read(i, sizeof(g_read_buf), g_read_buf) == E2P_STATUS_OK) {
            success_reads++;
        }
    }
    time_us = perf_timer_stop_us();

    printf("  Success reads: %u / %u\n", success_reads, DEMO_TEST_VAR_CNT);
    printf("  Total time: %u us\n", time_us);
    if (success_reads > 0) {
        printf("  Avg per read: %u us\n", time_us / success_reads);
    }
    return time_us;
}

static uint32_t perf_test_delete(void)
{
    uint32_t time_us;
    uint32_t success_deletes = 0;

    g_e2p_ctx.config.sector_cnt = DEMO_SECTOR_CNT;
    printf("\n=== Test: Delete Performance ===\n");

    e2p_clear();
    e2p_config(&g_e2p_ctx);
    perf_write_data();

    perf_timer_start();
    for (uint32_t i = 0; i < DEMO_TEST_VAR_CNT; i++) {
        if (e2p_delete(i) == E2P_STATUS_OK) {
            success_deletes++;
        }
    }
    time_us = perf_timer_stop_us();

    printf("  Success deletes: %u / %u\n", success_deletes, DEMO_TEST_VAR_CNT);
    printf("  Total time: %u us\n", time_us);
    if (success_deletes > 0) {
        printf("  Avg per delete: %u us\n", time_us / success_deletes);
    }
    return time_us;
}

static void perf_show_menu(void)
{
    printf("\n");
    printf("========================================\n");
    printf("  EEPROM Emulation Perf Test\n");
    printf("========================================\n");
    printf("  1 - Test config/init time\n");
    printf("  2 - Test write performance\n");
    printf("  3 - Test flush performance\n");
    printf("  4 - Test read performance\n");
    printf("  5 - Test delete performance\n");
    printf("  6 - Show status info\n");
    printf("  i - Show this menu\n");
    printf("========================================\n");
}

static void perf_show_info(void)
{
    e2p_show_info();
}

int main(void)
{
    board_init();

    printf("========================================\n");
    printf("  EEPROM Emulation Performance Test\n");
    printf("========================================\n");
    printf("  Test variables: %d\n", DEMO_TEST_VAR_CNT);
    printf("  Updates per var: %d\n", DEMO_WRITE_CYCLE);
    printf("  Erase size: %d bytes\n", DEMO_ERASE_SIZE);
    printf("  Sector count: %d\n", DEMO_SECTOR_CNT);
    printf("========================================\n");

    g_nor_cfg.xpi_base = BOARD_APP_XPI_NOR_XPI_BASE;
    g_nor_cfg.base_addr = BOARD_FLASH_BASE_ADDRESS;
    g_nor_cfg.opt_header = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    g_nor_cfg.opt0 = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    g_nor_cfg.opt1 = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;

    eeprom_init();
    perf_show_menu();

    while (1) {
        char ch = getchar();
        putchar(ch);
        putchar('\n');

        switch (ch) {
        case PERF_CONFIG_TEST:
            perf_test_config();
            break;
        case PERF_WRITE_TEST:
            perf_test_write();
            break;
        case PERF_FLUSH_TEST:
            perf_test_flush();
            break;
        case PERF_READ_TEST:
            perf_test_read();
            break;
        case PERF_DELETE_TEST:
            perf_test_delete();
            break;
        case PERF_SHOW_INFO:
            perf_show_info();
            break;
        case PERF_SHOW_INDEX:
            perf_show_menu();
            break;
        case '\r':
        case '\n':
            break;
        default:
            printf("Invalid input\n");
            perf_show_menu();
            break;
        }
    }

    return 0;
}
