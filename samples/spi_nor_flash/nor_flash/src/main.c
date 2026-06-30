/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_l1c_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_serial_nor.h"
#include "hpm_serial_nor_host_port.h"
#if SERIAL_NOR_USE_DMA_MGR
#include "hpm_dma_mgr.h"
#endif

#define TRANSFER_SIZE (15360U)
ATTR_PLACE_AT_WITH_ALIGNMENT(".ahb_sram", HPM_L1C_CACHELINE_SIZE) uint8_t wbuff[TRANSFER_SIZE];
ATTR_PLACE_AT_WITH_ALIGNMENT(".ahb_sram", HPM_L1C_CACHELINE_SIZE) uint8_t rbuff[TRANSFER_SIZE];

static uint32_t timer_freq_in_hz;
hpm_serial_nor_t nor_flash_dev = {0};

int main(void)
{
    hpm_stat_t stat;
    hpm_serial_nor_info_t flash_info;
    uint32_t transfer_len = 0;
    uint32_t addr = 0;
    uint32_t i = 0, j = 0;
    uint64_t elapsed = 0, now;
    double write_speed, read_speed;

    board_init();
#if SERIAL_NOR_USE_DMA_MGR
    dma_mgr_init();
#endif
    serial_nor_get_board_host(&nor_flash_dev.host);
    board_init_spi_clock(nor_flash_dev.host.host_param.param.host_base);
    serial_nor_spi_pins_init(nor_flash_dev.host.host_param.param.host_base);

    memset(rbuff, 0, sizeof(rbuff));
    for (i = 0; i < sizeof(wbuff); i++) {
        wbuff[i] = i % 0xFF;
    }

    timer_freq_in_hz = clock_get_frequency(clock_mchtmr0);
    stat = hpm_serial_nor_init(&nor_flash_dev, &flash_info);
    if (stat != status_success) {
        printf("spi nor flash init error\n");
    } else {
        printf("spi nor flash init ok\n");
        if (hpm_serial_nor_get_info(&nor_flash_dev, &flash_info) == status_success) {
            printf("the flash sfdp version:%d\n", flash_info.sfdp_version);
            printf("the flash size:%d KB\n", flash_info.size_in_kbytes);
            printf("the flash page_size:%d Byte\n", flash_info.page_size);
            printf("the flash sector_size:%d KB\n", flash_info.sector_size_kbytes);
            printf("the flash block_size:%d KB\n", flash_info.block_size_kbytes);
            printf("the flash sector_erase_cmd:0x%02x\n", flash_info.sector_erase_cmd);
            printf("the flash block_erase_cmd:0x%02x\n", flash_info.block_erase_cmd);
            transfer_len = TRANSFER_SIZE;
            hpm_serial_nor_erase_blocking(&nor_flash_dev, addr, transfer_len);
            now = mchtmr_get_count(HPM_MCHTMR);
            hpm_serial_nor_program_blocking(&nor_flash_dev, wbuff, transfer_len, addr);
            elapsed = (mchtmr_get_count(HPM_MCHTMR) - now);
            write_speed = (double) transfer_len * (timer_freq_in_hz / 1000) / elapsed;
            now = mchtmr_get_count(HPM_MCHTMR);
            stat = hpm_serial_nor_read(&nor_flash_dev, rbuff, transfer_len, addr);
            if (stat != status_success) {
                printf("read error\n");
            }
            elapsed = (mchtmr_get_count(HPM_MCHTMR) - now);
            read_speed = (double) transfer_len * (timer_freq_in_hz / 1000) / elapsed;
            for (i = 0; i < transfer_len; i++) {
                if (wbuff[i] != rbuff[i]) {
                    j++;
                }
            }
            if (!j) {
                printf("wbuff and rbuff compare finsh ok\n");
                printf("write_speed:%.2f KB/s read_speed:%.2f KB/s\n", write_speed, read_speed);
            } else {
                printf("wbuff and rbuff compare finsh fail %d\n", j);
            }
            addr = 0;
            while (hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
            };
            hpm_serial_nor_erase_sector_nonblocking(&nor_flash_dev, addr);
            while (hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
            };
            for (i = 0; i < flash_info.page_size; i++) {
                wbuff[i] = (i + 10) % 0xFF;
            }
            hpm_serial_nor_page_program_nonblocking(&nor_flash_dev, wbuff, flash_info.page_size, addr);
            while (hpm_serial_nor_is_busy(&nor_flash_dev) == status_spi_nor_flash_is_busy) {
            };
            board_delay_ms(10);
            stat = hpm_serial_nor_read(&nor_flash_dev, rbuff, flash_info.page_size, addr);
            if (stat != status_success) {
                printf("read error\n");
            }
            for (i = 0; i < flash_info.page_size; i++) {
                if (wbuff[i] != rbuff[i]) {
                    j++;
                }
            }
            if (!j) {
                printf("page program: wbuff and rbuff compare finsh ok\n");
            } else {
                printf("page program: wbuff and rbuff compare finsh fail %d\n", j);
            }
        }
    }
    while (1) {
    };
}
