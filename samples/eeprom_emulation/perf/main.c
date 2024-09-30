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
#define DEMO_WRITE_CYCLE    (5)
#define DEMO_ERASE_SIZE     (4096)
#define DEMO_SECTOR_CNT     (32)
#define DEMO_MANAGE_SIZE    (DEMO_ERASE_SIZE * DEMO_SECTOR_CNT)
#define DEMO_MANAGE_OFFSET  (BOARD_FLASH_SIZE - DEMO_MANAGE_SIZE)

#define PERF_CONFIG_TEST     ('1')
#define PERF_FLUSH_TEST      ('2')
#define PERF_READ_TEST       ('3')
#define PERF_SHOW_INFO       ('4')
#define PERF_SHOW_INDEX      ('i')

e2p_t e2p_demo;

uint8_t _read_buf[DEMO_RX_SIZE];

uint64_t delta_time;

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

static void start_time(void)
{
    delta_time = hpm_csr_get_core_mcycle();
}

static uint32_t get_end_time(void)
{
    delta_time = hpm_csr_get_core_mcycle() - delta_time;
    return delta_time;
}

static void eeprom_perf_write_data(void)
{
    uint32_t blob = 0;
    uint32_t blob_data[2];
    int i = 0;
    int count_per_blob;
    memset(blob_data, 0x00, sizeof(blob_data));
    
    blob_data[0] = E2P_MAGIC_ID;
    while (i < EEPROM_MAX_VAR_CNT) {
        count_per_blob = 0;
        blob++;
        while (count_per_blob < DEMO_WRITE_CYCLE) {
            blob_data[1] += 10;
            e2p_write(blob, sizeof(blob_data), (uint8_t *)blob_data);
            count_per_blob++;        
        }
        i++;
    }    
}

static uint32_t eeprom_perf_config_time(void)
{
    uint32_t run_time;
    uint32_t run_us;
    uint32_t tick_per_us = clock_get_frequency(clock_cpu0) / 1000 / 1000;

    e2p_clear();
    e2p_config(&e2p_demo);

    eeprom_perf_write_data();
    start_time();
    e2p_config(&e2p_demo);
    run_time = get_end_time();

    run_us = run_time / tick_per_us;
    printf("eeprom config run time=(%u)us\n", run_us);
    return run_us;
}

static uint32_t eeprom_perf_flush_time(void)
{
    uint32_t run_time;
    uint32_t run_us;
    uint32_t tick_per_us = clock_get_frequency(clock_cpu0) / 1000 / 1000;

    e2p_config(&e2p_demo);

    eeprom_perf_write_data();
    start_time();
    e2p_flush(E2P_FLUSH_BEGIN);
    run_time = get_end_time();

    run_us = run_time / tick_per_us;
    printf("eeprom flush run time=(%u)us\n", run_us);
    return run_us;
}

static uint32_t eeprom_perf_read_time(void)
{
    uint32_t run_time;
    uint32_t run_us;
    uint32_t tick_per_us = clock_get_frequency(clock_cpu0) / 1000 / 1000;

    e2p_config(&e2p_demo);

    eeprom_perf_write_data();
    start_time();
    e2p_read(EEPROM_MAX_VAR_CNT, DEMO_BLOCK_SIZE_MAX, _read_buf);
    run_time = get_end_time();

    run_us = run_time / tick_per_us;
    printf("eeprom read run time=(%u)us\n", run_us);
    return run_us;
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

static void eeprom_show_info(void)
{
    e2p_show_info();
}

static void eeprom_show_index(void)
{
    printf(" eeprom emulation perf test\r\n----------------------------------------\r\n");
    printf(" 1 - Test config perf\r\n");
    printf(" 2 - Test flush perf\r\n");
    printf(" 3 - Test read perf\r\n");
    printf(" 4 - show area base info\r\n");
    printf(" Others - Show index menu\r\n");
}

int main(void)
{
    board_init();
    eeprom_init();
    eeprom_show_index();
    char click;

    while(1)
    {
        click = getchar();
        putchar(click);
        putchar('\n');
        
        switch (click) {
            case PERF_CONFIG_TEST:
                eeprom_perf_config_time();
                break;
            case PERF_FLUSH_TEST:
                eeprom_perf_flush_time();
                break;
            case PERF_READ_TEST:
                eeprom_perf_read_time();
                break;
            case PERF_SHOW_INFO:
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
