/*
 * Copyright (c) 2023,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <string.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "eeprom_emulation.h"
#include "hpm_nor_flash.h"

#define DEMO_RX_SIZE        (512)
#define DEMO_BLOCK_SIZE_MAX (32)
#define DEMO_ERASE_SIZE     (4096)
#define DEMO_SECTOR_CNT     (32)
#define DEMO_MANAGE_SIZE    (DEMO_ERASE_SIZE * DEMO_SECTOR_CNT)
#define DEMO_MANAGE_OFFSET  (BOARD_FLASH_SIZE - DEMO_MANAGE_SIZE * 2)

#define DEMO_WRITE_TEST     ('1')
#define DEMO_READ_TEST      ('2')
#define DEMO_UPDATE_TEST    ('3')
#define DEMO_DELETE_TEST    ('4')
#define DEMO_FLUSH_TEST     ('5')
#define DEMO_CLEAR_TEST     ('6')
#define DEMO_SHOW_INFO      ('7')
#define DEMO_SHOW_INDEX     ('i')

static nor_flash_config_t g_nor_cfg;
static e2p_t g_e2p_ctx;

static uint8_t g_read_buf[DEMO_RX_SIZE];

static uint32_t g_var1;
static uint32_t g_var2;
static uint32_t g_var3;
static uint32_t g_var4;

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

static void eeprom_show_menu(void)
{
    printf("\n");
    printf("========================================\n");
    printf("  EEPROM Emulation Demo Menu\n");
    printf("========================================\n");
    printf("  1 - Write test data\n");
    printf("  2 - Read test data\n");
    printf("  3 - Update test data\n");
    printf("  4 - Delete a variable\n");
    printf("  5 - Flush (defragment)\n");
    printf("  6 - Clear all data\n");
    printf("  7 - Show status info\n");
    printf("  i - Show this menu\n");
    printf("========================================\n");
}

static void eeprom_write_test(void)
{
    printf("Writing 4 variables...\n");

    if (e2p_write(g_var1, sizeof("abcdef"), (uint8_t *)"abcdef") != E2P_STATUS_OK) {
        printf("Write var1 failed\n");
        return;
    }
    if (e2p_write(g_var2, sizeof("1234"), (uint8_t *)"1234") != E2P_STATUS_OK) {
        printf("Write var2 failed\n");
        return;
    }
    if (e2p_write(g_var3, sizeof("hello,world"), (uint8_t *)"hello,world") != E2P_STATUS_OK) {
        printf("Write var3 failed\n");
        return;
    }
    if (e2p_write(g_var4, sizeof("eeprom_demo"), (uint8_t *)"eeprom_demo") != E2P_STATUS_OK) {
        printf("Write var4 failed\n");
        return;
    }

    printf("Write completed successfully\n");
}

static void eeprom_read_test(void)
{
    printf("Reading variables...\n");

    if (e2p_read(g_var1, DEMO_BLOCK_SIZE_MAX, g_read_buf) == E2P_STATUS_OK) {
        printf("var1 = %s\n", g_read_buf);
    } else {
        printf("Read var1 failed\n");
    }

    if (e2p_read(g_var2, DEMO_BLOCK_SIZE_MAX, g_read_buf) == E2P_STATUS_OK) {
        printf("var2 = %s\n", g_read_buf);
    } else {
        printf("Read var2 failed\n");
    }

    if (e2p_read(g_var3, DEMO_BLOCK_SIZE_MAX, g_read_buf) == E2P_STATUS_OK) {
        printf("var3 = %s\n", g_read_buf);
        if (memcmp(g_read_buf, "hello,world", sizeof("hello,world")) != 0) {
            printf("var3 data mismatch!\n");
        }
    } else {
        printf("Read var3 failed\n");
    }

    if (e2p_read(g_var4, DEMO_BLOCK_SIZE_MAX, g_read_buf) == E2P_STATUS_OK) {
        printf("var4 = %s\n", g_read_buf);
        if (memcmp(g_read_buf, "eeprom_demo", sizeof("eeprom_demo")) != 0) {
            printf("var4 data mismatch!\n");
        }
    } else {
        printf("Read var4 failed\n");
    }
}

static void eeprom_update_test(void)
{
    printf("Updating var1 and var2...\n");

    if (e2p_write(g_var1, sizeof("qwe"), (uint8_t *)"qwe") != E2P_STATUS_OK) {
        printf("Update var1 failed\n");
        return;
    }

    if (e2p_write(g_var2, sizeof("5678"), (uint8_t *)"5678") != E2P_STATUS_OK) {
        printf("Update var2 failed\n");
        return;
    }

    if (e2p_read(g_var1, DEMO_BLOCK_SIZE_MAX, g_read_buf) == E2P_STATUS_OK) {
        printf("var1 = %s\n", g_read_buf);
        if (memcmp(g_read_buf, "qwe", sizeof("qwe")) != 0) {
            printf("var1 update mismatch!\n");
        }
    }

    if (e2p_read(g_var2, DEMO_BLOCK_SIZE_MAX, g_read_buf) == E2P_STATUS_OK) {
        printf("var2 = %s\n", g_read_buf);
        if (memcmp(g_read_buf, "5678", sizeof("5678")) != 0) {
            printf("var2 update mismatch!\n");
        }
    }

    printf("Update completed successfully\n");
}

static void eeprom_delete_test(void)
{
    printf("Deleting var4...\n");

    hpm_stat_t ret = e2p_delete(g_var4);
    if (ret == E2P_STATUS_OK) {
        printf("Delete successful\n");
    } else if (ret == E2P_ERROR_NOT_FOUND) {
        printf("var4 not found\n");
    } else {
        printf("Delete failed, error=%d\n", ret);
    }
}

static void eeprom_flush_test(void)
{
    printf("Flushing (defragmenting)...\n");

    hpm_stat_t ret = e2p_flush(E2P_FLUSH_FORCE);
    if (ret == E2P_STATUS_OK) {
        printf("Flush completed successfully\n");
        e2p_show_info();
    } else {
        printf("Flush failed, error=%d\n", ret);
    }
}

static void eeprom_clear_test(void)
{
    printf("Clearing all data...\n");
    e2p_clear();
    printf("Clear completed\n");
    e2p_show_info();
}

static void eeprom_show_info(void)
{
    e2p_show_info();
}

int main(void)
{
    board_init();

    printf("========================================\n");
    printf("  EEPROM Emulation Base Test\n");
    printf("========================================\n");

    g_nor_cfg.xpi_base = BOARD_APP_XPI_NOR_XPI_BASE;
    g_nor_cfg.base_addr = BOARD_FLASH_BASE_ADDRESS;
    g_nor_cfg.opt_header = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    g_nor_cfg.opt0 = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    g_nor_cfg.opt1 = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;

    g_var1 = e2p_generate_id("HPMC");
    g_var2 = e2p_generate_id("A");
    g_var3 = e2p_generate_id("AB");
    g_var4 = e2p_generate_id("ABC");

    printf("Generated block IDs:\n");
    printf("  var1 = 0x%08x\n", g_var1);
    printf("  var2 = 0x%08x\n", g_var2);
    printf("  var3 = 0x%08x\n", g_var3);
    printf("  var4 = 0x%08x\n", g_var4);

    eeprom_init();
    eeprom_show_menu();

    while (1) {
        char ch = getchar();
        putchar(ch);
        putchar('\n');

        switch (ch) {
            case DEMO_WRITE_TEST:
                eeprom_write_test();
                break;
            case DEMO_READ_TEST:
                eeprom_read_test();
                break;
            case DEMO_UPDATE_TEST:
                eeprom_update_test();
                break;
            case DEMO_DELETE_TEST:
                eeprom_delete_test();
                break;
            case DEMO_FLUSH_TEST:
                eeprom_flush_test();
                break;
            case DEMO_CLEAR_TEST:
                eeprom_clear_test();
                break;
            case DEMO_SHOW_INFO:
                eeprom_show_info();
                break;
            case DEMO_SHOW_INDEX:
                eeprom_show_menu();
                break;
            case '\r':
            case '\n':
                break;
            default:
                printf("Invalid input\n");
                eeprom_show_menu();
                break;
        }
    }

    return 0;
}
