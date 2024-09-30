/*
 * Copyright (c) 2023 HPMicro
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
#define DEMO_ERASE_SIZE     (4096)
#define DEMO_SECTOR_CNT     (32)
#define DEMO_MANEGE_SIZE    (DEMO_ERASE_SIZE * DEMO_SECTOR_CNT)
#define DEMO_MANAGE_OFFSET  (BOARD_FLASH_SIZE - DEMO_MANEGE_SIZE * 2)
#define DEMO_RET_CHECK(foo) do { \
                    int ret = foo; \
                    if (ret < 0) { \
                        printf ("test "#foo" failed: %d\n", ret); \
                    } \
                } while (0)

#define DEMO_WRITE_TEST     ('1')
#define DEMO_READ_TEST      ('2')
#define DEMO_UPDATE_TEST    ('3')
#define DEMO_FLUSH_TEST     ('4')
#define DEMO_SHOW_INFO      ('5')
#define DEMO_SHOW_INDEX     ('i')

e2p_t e2p_demo;
const char *blob1 = "HPMC";
const char *blob2 = "A";
const char *blob3 = "AB";
const char *blob4 = "ABC";

uint8_t read_buf[DEMO_RX_SIZE];
uint32_t var1, var2, var3,var4;

static uint32_t demo_read(uint8_t *buf, uint32_t addr, uint32_t size)
{
    return nor_flash_read(&e2p_demo.nor_config, buf, addr, size);
}

static uint32_t demo_write(uint8_t *buf, uint32_t addr, uint32_t size)
{
    return nor_flash_write(&e2p_demo.nor_config, buf, addr, size);
}

static void demo_erase(uint32_t start_addr, uint32_t size)
{
    nor_flash_erase(&e2p_demo.nor_config, start_addr, size);
}

static void eeprom_init(void)
{
    e2p_demo.nor_config.xpi_base = BOARD_APP_XPI_NOR_XPI_BASE;
    e2p_demo.nor_config.base_addr = BOARD_FLASH_BASE_ADDRESS;
    e2p_demo.config.start_addr = e2p_demo.nor_config.base_addr + DEMO_MANAGE_OFFSET;
    e2p_demo.config.erase_size = DEMO_ERASE_SIZE;
    e2p_demo.config.sector_cnt = DEMO_SECTOR_CNT;
    e2p_demo.config.version = 0x4553; /* 'E' 'S' */
    e2p_demo.nor_config.opt_header = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    e2p_demo.nor_config.opt0 = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    e2p_demo.nor_config.opt1 = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;
    e2p_demo.config.flash_read = demo_read;
    e2p_demo.config.flash_write = demo_write;
    e2p_demo.config.flash_erase = demo_erase;

    nor_flash_init(&e2p_demo.nor_config);
    e2p_config(&e2p_demo);
}

static void eeprom_write_test(void)
{
    DEMO_RET_CHECK(e2p_write(var1, sizeof("abcdef"), (uint8_t *)"abcdef"));
    DEMO_RET_CHECK(e2p_write(var2, sizeof("1234"), (uint8_t *)"1234"));
    DEMO_RET_CHECK(e2p_write(var3, sizeof("hello,world"), (uint8_t *)"hello,world"));
    DEMO_RET_CHECK(e2p_write(var4, sizeof("eeprom_demo"), (uint8_t *)"eeprom_demo"));
}

static void eeprom_read_test(void)
{
    DEMO_RET_CHECK(e2p_read(var1, DEMO_BLOCK_SIZE_MAX, read_buf));
    printf("var1 = %s\n", read_buf);

    DEMO_RET_CHECK(e2p_read(var2, DEMO_BLOCK_SIZE_MAX, read_buf));
    printf("var2 = %s\n", read_buf);

    DEMO_RET_CHECK(e2p_read(var3, DEMO_BLOCK_SIZE_MAX, read_buf));
    printf("var3 = %s\n", read_buf);
    if (0 != memcmp(read_buf, "hello,world", sizeof("hello,world"))) {
        printf("test var3 failed\n");
    }

    DEMO_RET_CHECK(e2p_read(var4, DEMO_BLOCK_SIZE_MAX, read_buf));
    printf("var4 = %s\n", read_buf);
    if (0 != memcmp(read_buf, "eeprom_demo", sizeof("eeprom_demo"))) {
        printf("test var4 failed\n");
    }
}

static void eeprom_update_test(void)
{
    eeprom_write_test();
    DEMO_RET_CHECK(e2p_write(var1, sizeof("qwe"), (uint8_t *)"qwe"));
    DEMO_RET_CHECK(e2p_write(var2, sizeof("5678"), (uint8_t *)"5678"));
    DEMO_RET_CHECK(e2p_read(var1, DEMO_BLOCK_SIZE_MAX, read_buf));
    if (0 != memcmp(read_buf, "qwe", sizeof("qwe"))) {
        printf("update var1 failed\n");
    }

    DEMO_RET_CHECK(e2p_read(var2,  DEMO_BLOCK_SIZE_MAX, read_buf));
    if (0 != memcmp(read_buf, "5678", sizeof("5678"))) {
        printf("update var2 failed\n");
    }
}

static void eeprom_flush_test(void)
{
    e2p_flush(E2P_FLUSH_BEGIN);
    e2p_show_info();
}

static void eeprom_show_info(void)
{
    e2p_show_info();
}

static void eeprom_show_index(void)
{
    printf(" eeprom emulation demo\r\n----------------------------------------\r\n");
    printf(" 1 - Test eeprom write\r\n");
    printf(" 2 - Test eeprom read\r\n");
    printf(" 3 - Test eeprom update data\r\n");
    printf(" 4 - Test eeprom flush whole area\r\n");
    printf(" 5 - show area base info\r\n");
    printf(" Others - Show index menu\r\n");
}

int main(void)
{
    board_init();
    eeprom_init();
    char click;
    var1 = e2p_generate_id(blob1);
    var2 = e2p_generate_id(blob2);
    var3 = e2p_generate_id(blob3);
    var4 = e2p_generate_id(blob4);
    eeprom_show_index();

    while(1)
    {
        click = getchar();
        putchar(click);
        putchar('\n');
        
        switch (click) {
            case DEMO_WRITE_TEST:
                eeprom_write_test();
                break;
            case DEMO_READ_TEST:
                eeprom_read_test();
                break;
            case DEMO_UPDATE_TEST:
                eeprom_update_test();
                break;
            case DEMO_FLUSH_TEST:
                eeprom_flush_test();
                break;
            case DEMO_SHOW_INFO:
                eeprom_show_info();
                break;
            case '\r':
                break;
            case '\n':
                break;
            default:
                eeprom_show_index();
                break;
        }
    }
    return 0;
}
