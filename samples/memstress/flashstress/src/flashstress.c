/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include <stdio.h>
#include "board.h"
#include "hpm_debug_console.h"
#include "hpm_l1c_drv.h"
#include "hpm_romapi.h"
#include "hpm_clock_drv.h"
#include "flashstress_lib.h"

static xpi_nor_config_t s_xpi_nor_config;
int norflash_init(void)
{
    xpi_nor_config_option_t option;
    option.header.U = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    option.option0.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    option.option1.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;

    hpm_stat_t status = rom_xpi_nor_auto_config(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, &option);
    if (status != status_success) {
        printf("Error: rom_xpi_nor_auto_config\n");
        while (1);
    }
    return 0;
}

int norflash_read(uint32_t offset, void *buf, uint32_t size_bytes)
{
    hpm_stat_t status = rom_xpi_nor_read(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto,
                        &s_xpi_nor_config, (uint32_t *)buf, offset, size_bytes);
    if (status != status_success) {
        return -1;
    }

    return 0;
}

int norflash_read_mem(uint32_t offset, void *buf, uint32_t size_bytes)
{
    uint32_t flash_addr = 0x80000000 + offset;
    uint32_t aligned_start = HPM_L1C_CACHELINE_ALIGN_DOWN(flash_addr);
    uint32_t aligned_end = HPM_L1C_CACHELINE_ALIGN_UP(flash_addr + size_bytes);
    uint32_t aligned_size = aligned_end - aligned_start;

    l1c_dc_invalidate(aligned_start, aligned_size);

    memcpy(buf, (void *)flash_addr, size_bytes);
    return 0;
}

int norflash_write(uint32_t offset, const void *buf, uint32_t size_bytes)
{
    hpm_stat_t status = rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto,
                                 &s_xpi_nor_config, (const uint32_t *)buf, offset, size_bytes);
    if (status != status_success) {
        return -1;
    }
    return 0;
}

int norflash_erase_chip(void)
{
    hpm_stat_t status = rom_xpi_nor_erase_chip(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config);
    if (status != status_success) {
        return -1;
    }
    return 0;
}

int norflash_erase_block(uint32_t offset)
{
    hpm_stat_t status = rom_xpi_nor_erase_block(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, offset);
    if (status != status_success) {
        return -1;
    }
    return 0;
}

uint32_t norflash_get_chip_size(void)
{
    uint32_t flash_size;
    rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, 
            xpi_nor_property_total_size, &flash_size);
    return flash_size;
}

uint32_t norflash_get_block_size(void)
{
    uint32_t block_size;
    rom_xpi_nor_get_property(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, 
            xpi_nor_property_block_size, &block_size);
    return block_size;
}

uint8_t norflash_get_erase_value(void)
{
    return 0xffu;
}

int main(void)
{
    struct flashstress_driver drv;
    struct flashstress_context *nor_ctx;
    board_init();
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    norflash_init();

    if (!l1c_dc_is_enabled()) {
        l1c_dc_enable();
        l1c_dc_invalidate_all();
    }
    __asm volatile("fence rw, rw");

    drv.get_flash_chip_size_bytes = norflash_get_chip_size;
    drv.get_flash_block_size_bytes = norflash_get_block_size;
    drv.get_flash_erase_value = norflash_get_erase_value;
    drv.erase_chip = norflash_erase_chip;
    drv.erase_block = norflash_erase_block;
    drv.read = norflash_read;
    drv.write = norflash_write;
    nor_ctx = flashstress_create(&drv, "read api(dcache enable)");
    if (flashstress_run(nor_ctx) < 0) {
        while (1);
    }
    flashstress_destroy(nor_ctx);


    drv.read = norflash_read_mem;
    nor_ctx = flashstress_create(&drv, "read mem(dcache enable)");
    if (flashstress_run(nor_ctx) < 0) {
        while (1);
    }
    flashstress_destroy(nor_ctx);

    if (l1c_dc_is_enabled()) {
        l1c_dc_writeback_all();
        __asm volatile("fence rw, rw");
        l1c_dc_disable();
    }
    __asm volatile("fence rw, rw");

    drv.read = norflash_read;
    nor_ctx = flashstress_create(&drv, "read api(dcache disable)");
    if (flashstress_run(nor_ctx) < 0) {
        while (1);
    }
    flashstress_destroy(nor_ctx);


    drv.read = norflash_read_mem;
    nor_ctx = flashstress_create(&drv, "read mem(dcache disable)");
    if (flashstress_run(nor_ctx) < 0) {
        while (1);
    }
    flashstress_destroy(nor_ctx);

    printf("\n=============================================\n");
    printf("All cases are PASSED\n");
    printf("=============================================\n");

    while (1);
    return 0;
}