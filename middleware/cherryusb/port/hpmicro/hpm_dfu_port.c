/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_dfu_trigger.h"

#include <stdio.h>
#include "board.h"
#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_romapi.h"
#include "boot/hpm_bootheader.h"
#include "hpm_l1c_drv.h"
#ifdef HPM_BCFG_BASE
#include "hpm_bgpr_drv.h"
#endif
#ifdef HPM_PDGO_BASE
#include "hpm_pdgo_drv.h"
#endif
#include "hpm_ppor_drv.h"
#include "usb_dfu.h"

#define DFU_TRIGGER_MAGIC (0x55464455UL)
#define DFU_TRIGGER_BGPR_INDEX (0U)
#ifndef HPM_DFU_BGPR
#define HPM_DFU_BGPR BOARD_BGPR
#endif

#ifndef USBD_DFU_APP_DEFAULT_ADD
#define USBD_DFU_APP_DEFAULT_ADD 0x80020000
#endif
#ifndef APP_START_OFFSET
#define APP_START_OFFSET    0x20000
#endif

#ifndef DFU_XPI_NOR_BASE
#define DFU_XPI_NOR_BASE BOARD_APP_XPI_NOR_XPI_BASE
#endif

#ifndef DFU_XPI_NOR_CFG_OPT_HDR
#define DFU_XPI_NOR_CFG_OPT_HDR BOARD_APP_XPI_NOR_CFG_OPT_HDR
#endif

#ifndef DFU_XPI_NOR_CFG_OPT_OPT0
#define DFU_XPI_NOR_CFG_OPT_OPT0 BOARD_APP_XPI_NOR_CFG_OPT_OPT0
#endif

#ifndef DFU_XPI_NOR_CFG_OPT_OPT1
#define DFU_XPI_NOR_CFG_OPT_OPT1 BOARD_APP_XPI_NOR_CFG_OPT_OPT1
#endif

#ifndef DFU_XPI_FLASH_ERASE_SIZE
#define DFU_XPI_FLASH_ERASE_SIZE (16 * 1024)
#endif

#ifndef DFU_FLASH_SIZE
#define DFU_FLASH_SIZE BOARD_FLASH_SIZE
#endif

#if DFU_XPI_FLASH_ERASE_SIZE % 4096 != 0
#error "DFU_XPI_FLASH_ERASE_SIZE should be a multiple of 4096"
#endif

static xpi_nor_config_t s_dfu_xpi_nor_config;
static bool s_dfu_xpi_nor_initialized = false;
static uint32_t s_dfu_xpi_nor_size = 0;
volatile uint32_t flash_start_address;

static void dfu_flash_init(void)
{
    if (s_dfu_xpi_nor_initialized) {
        return;
    }

    xpi_nor_config_option_t option;
    option.header.U = DFU_XPI_NOR_CFG_OPT_HDR;
    option.option0.U = DFU_XPI_NOR_CFG_OPT_OPT0;
    option.option1.U = DFU_XPI_NOR_CFG_OPT_OPT1;

    XPI_Type *base = (XPI_Type *)DFU_XPI_NOR_BASE;

    hpm_stat_t status = rom_xpi_nor_auto_config(base, &s_dfu_xpi_nor_config, &option);
    if (status == status_success) {
        s_dfu_xpi_nor_initialized = true;
        rom_xpi_nor_get_property(base, &s_dfu_xpi_nor_config, xpi_nor_property_total_size, &s_dfu_xpi_nor_size);
    }
}

static int dfu_erase_flash(uint32_t add)
{
    if (s_dfu_xpi_nor_size == 0) {
        printf("DFU: Flash size not initialized\r\n");
        return 1;
    }

    XPI_Type *base = (XPI_Type *)DFU_XPI_NOR_BASE;
    xpi_xfer_channel_t chn = xpi_xfer_channel_auto;

    uint32_t sector_size;
    rom_xpi_nor_get_property(base, &s_dfu_xpi_nor_config, xpi_nor_property_sector_size, &sector_size);

    if ((add < USBD_DFU_APP_DEFAULT_ADD) || (add > BOARD_FLASH_BASE_ADDRESS + (uint32_t)DFU_FLASH_SIZE)) {
        printf("ERROR!Address is out of range 0x%08x(0x%08x-0x%08x)\n", add, USBD_DFU_APP_DEFAULT_ADD, BOARD_FLASH_BASE_ADDRESS + (uint32_t)DFU_FLASH_SIZE);
        return 1;
    }
    uint32_t addr_offset = add - (uint32_t)BOARD_FLASH_BASE_ADDRESS;
    uint32_t erase_start = addr_offset & (~(sector_size - 1));

    hpm_stat_t status = rom_xpi_nor_erase(base, chn, &s_dfu_xpi_nor_config, erase_start, DFU_XPI_FLASH_ERASE_SIZE);
    if (status == status_success) {
    } else {
        printf("DFU: Flash erase failed, status=%d\r\n", status);
    }
    __asm("fence.i");

    return (status == status_success) ? 0 : 1;
}

int usbd_dfu_read(uint16_t value, const uint8_t *data, uint16_t length, uint16_t *actual_length)
{
    uint32_t i = 0;
    __asm("fence.i");
    l1c_dc_invalidate_all();
    if (value == 0) {
        if (data[0] == DFU_SPECIAL_CMD_SET_ADDRESS_POINTER) {
            memcpy((uint8_t *)&flash_start_address, &data[1], 4);
            return 0;
        }
    } else if (value > 1) {
        uint32_t addr = (value - 2) * USBD_DFU_XFER_SIZE + flash_start_address;
        if (addr - (uint32_t)BOARD_FLASH_BASE_ADDRESS + length > s_dfu_xpi_nor_size) {
            return 1;
        }
        uint8_t *p = (uint8_t *)addr;
        uint8_t *buf = (uint8_t *)data;
        for (i = 0; i < length; i++) {
            buf[i] = p[i];
        }
        *actual_length = length;
        return length;
    }

    return -1;
}

int usbd_dfu_write(uint16_t value, const uint8_t *data, uint16_t length)
{
    dfu_flash_init();

    if (!s_dfu_xpi_nor_initialized || s_dfu_xpi_nor_size == 0) {
        return 1;
    }
    if (value == 0) {
        if (data[0] == DFU_SPECIAL_CMD_SET_ADDRESS_POINTER) {
            memcpy((uint8_t *)&flash_start_address, &data[1], 4);
            return 0;
        } else if (data[0] == DFU_SPECIAL_CMD_ERASE) {
            memcpy((uint8_t *)&flash_start_address, &data[1], 4);
            return dfu_erase_flash(flash_start_address);
        }
    } else if (value > 1) {
        uint32_t addr_offset = (value - 2) * USBD_DFU_XFER_SIZE + flash_start_address - (uint32_t)BOARD_FLASH_BASE_ADDRESS;
        if (addr_offset + length > s_dfu_xpi_nor_size) {
            return 1;
        }

        XPI_Type *base = (XPI_Type *)DFU_XPI_NOR_BASE;
        xpi_xfer_channel_t chn = xpi_xfer_channel_auto;

        disable_global_irq(CSR_MSTATUS_MIE_MASK);
        hpm_stat_t status = rom_xpi_nor_program(base, chn, &s_dfu_xpi_nor_config, (const uint32_t *)data, addr_offset, length);
        enable_global_irq(CSR_MSTATUS_MIE_MASK);
        __asm("fence.i");

        return (status == status_success) ? 0 : 1;
    }
    return -1;
}

void usbd_dfu_reset(void)
{
    ppor_sw_reset(HPM_PPOR, 24);
    while (1) {
    }
}