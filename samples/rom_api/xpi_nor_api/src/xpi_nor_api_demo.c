/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_debug_console.h"
#include "hpm_romapi.h"


#define OPTION_LAST_SECTOR_DEMO ('1')
#define OPTION_STRESS_TEST ('2')
#define OPTION_EXIT ('x')

static xpi_nor_config_t s_xpi_nor_config;

static uint32_t s_write_buf[1024];
static uint32_t s_read_buf[1024];


void show_menu(void);

static hpm_stat_t xpi_nor_test_last_sector(void);

static hpm_stat_t xpi_nor_stress_test(void);

int main(void)
{
    board_init();

    show_menu();

    char option;
    bool exit_loop = false;
    while (!exit_loop) {
        hpm_stat_t status;
        option = getchar();
        putchar(option);
        putchar('\n');

        switch (option) {
        case OPTION_LAST_SECTOR_DEMO:
            status = xpi_nor_test_last_sector();
            printf("XPI NOR Test Last Sector %s\n", status == status_success ? "PASSED" : "FAILED");
            break;
        case OPTION_STRESS_TEST:
            status = xpi_nor_stress_test();
            printf("XPI NOR Stress Test %s\n", status == status_success ? "PASSED" : "FAILED");
            break;
        case OPTION_EXIT:
            exit_loop = true;
            break;
        default:
            show_menu();
            break;
        }
    }
    return 0;
}

static hpm_stat_t xpi_nor_test_last_sector(void)
{
    xpi_nor_config_option_t option;
    option.header.U = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    option.option0.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    option.option1.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;

    hpm_stat_t status = rom_xpi_nor_auto_config(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, &option);
    if (status != status_success) {
        return status;
    }
    uint32_t flash_size;
    uint32_t sector_size;
    uint32_t page_size;
    rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_total_size,
                             &flash_size);
    rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_sector_size,
                             &sector_size);
    rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_page_size, &page_size);
    uint32_t erase_start = flash_size - sector_size;
    uint32_t erase_size = sector_size;
    uint32_t program_start = erase_start;
    uint32_t program_size = sizeof(s_write_buf);

    printf("Flash Size:%dMBytes\nFlash Sector Size:%dKBytes\nFlash Page Size:%dBytes\n",
           flash_size / 1024U / 1024U, sector_size / 1024U, page_size);

    status = rom_xpi_nor_erase(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,
                               erase_start, erase_size);
    if (status != status_success) {
        return status;
    }

    uint32_t buffer[128];
    uint32_t start = erase_start;
    for (uint32_t i = 0; i < erase_size; i += sizeof(buffer)) {
        status = rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, buffer, start,
                                  sizeof(buffer));
        if (status != status_success) {
            return status;
        }
        for (uint32_t j = 0; j < ARRAY_SIZE(buffer); j++) {
            if (buffer[j] != 0xFFFFFFFFUL) {
                return status_fail;
            }
        }

        start += sizeof(buffer);
    }

    uint8_t *write_buf_8 = (uint8_t *) s_write_buf;
    for (uint32_t i = 0; i < sizeof(s_write_buf); i++) {
        *write_buf_8++ = (uint8_t) i & 0xFFU;
    }

    status = rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config,
                                 s_write_buf, program_start, program_size);
    if (status != status_success) {
        return status;
    }

    status = rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, s_read_buf,
                              program_start, program_size);
    if (status != status_success) {
        return status;
    }

    if (memcmp(s_read_buf, s_write_buf, sizeof(s_write_buf)) != 0) {
        return status_fail;
    }

    status = rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, s_read_buf,
                              program_start, sizeof(s_read_buf));
    if (status != status_success) {
        return status;
    }

    uint8_t *read_buf_8 = (uint8_t *) s_read_buf;
    for (uint32_t i = 0; i < sizeof(s_read_buf); i += 16) {
        printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
               read_buf_8[0], read_buf_8[1], read_buf_8[2], read_buf_8[3], read_buf_8[4], read_buf_8[5], read_buf_8[6],
               read_buf_8[7],
               read_buf_8[8], read_buf_8[9], read_buf_8[10], read_buf_8[11], read_buf_8[12], read_buf_8[13],
               read_buf_8[14], read_buf_8[15]);
        read_buf_8 += 16;
    }

    return status_success;
}

static hpm_stat_t xpi_nor_stress_test(void)
{
    uint32_t flash_size;
    uint32_t sector_size;
    uint32_t block_size;
    uint32_t page_size;

    xpi_nor_config_option_t option;
    option.header.U = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    option.option0.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    option.option1.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;

    uint32_t err_cnt = 0;

    uint8_t *write_8 = (uint8_t *) s_write_buf;
    for (uint32_t i = 0; i < sizeof(s_write_buf); i++) {
        *write_8++ = i & 0xFFU;
    }

    const char *freq_str_list[] = {
            "Reserved", "30MHz", "50MHz", "66MHz", "80MHz", "100MHz", "120MHz", "133MHz", "166MHz"
    };

    hpm_stat_t status;
    do {
        status = rom_xpi_nor_auto_config(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, &option);
        if (status != status_success) {
            ++err_cnt;
            break;
        }
        if (option.option0.freq_opt >= ARRAY_SIZE(freq_str_list)) {
            ++err_cnt;
            break;


        }
        printf("Frequency:%s\n", freq_str_list[option.option0.freq_opt]);

        rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_total_size,
                                 &flash_size);
        rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_sector_size,
                                 &sector_size);
        rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_block_size,
                                 &block_size);
        rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, xpi_nor_property_page_size, &page_size);

        printf("Flash Size:%dMBytes\nFlash Sector Size:%dKBytes\nFlash Block Size:%dKBytes\nFlash Page Size:%dBytes\n",
               flash_size / 1024U / 1024U, sector_size / 1024U, block_size / 1024U, page_size);

        printf("\nSector Erase test...\n");
        for (uint32_t j = 0; j < flash_size;) {
            status = rom_xpi_nor_erase_sector(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, j);
            if (status != status_success) {
                ++err_cnt;
                break;
            }
            j += sector_size;
            printf("\r%3d%%", j * 100 / flash_size);
        }
        printf("\n");

        printf("\nBlock Erase test ...\n");
        for (uint32_t j = 0; j < flash_size;) {
            status = rom_xpi_nor_erase_block(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, j);
            if (status != status_success) {
                ++err_cnt;
                break;
            }
            j += block_size;
            printf("\r%3d%%", j * 100 / flash_size);
        }
        printf("\n");
        printf("\nFlash full-chip program...\n");
        for (uint32_t j = 0; j < flash_size;) {
            status = rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, s_write_buf,
                                         j, sizeof(s_write_buf));
            if (status != status_success) {
                ++err_cnt;
                break;
            }
            j += sizeof(s_write_buf);
            printf("\r%3d%%", j * 100 / flash_size);
        }
        printf("\n");
        printf("XPI Frequency=%dMHz\n", clock_get_frequency(clock_xpi0) / 1000000UL);
        printf("\nFlash verify program...\n");
        for (uint32_t j = 0; j < flash_size;) {
            status = rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, s_read_buf,
                                      j, sizeof(s_read_buf));
            if (status != status_success) {
                ++err_cnt;
                break;
            }
            if (memcmp(s_read_buf, s_write_buf, sizeof(s_read_buf)) != 0) {
                printf("Read failed at 0x%08x\n", j);
                ++err_cnt;
                break;
            }
            j += sizeof(s_read_buf);
            printf("\r%3d%%", j * 100 / flash_size);
        }
        printf("\n");
    } while (false);

    status = (err_cnt > 0) ? status_fail : status_success;

    return status;
}

void show_menu(void)
{
    printf("XPI NOR demo\r\n----------------------------------------\r\n");
    printf(" 1 - Test FLASH at the last Sector\r\n");
    printf(" 2 - Stress test on the FLASH\r\n     NOTE: This case cannot be tested on flash_xip build\r\n");
    printf(" Others - Show menu\r\n");
}
