/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_romapi.h"
#include "hpm_ppor_drv.h"
#include "ecat_foe_support.h"

#define DISABLE_GLOBAL_INTERRUPT() disable_global_irq(CSR_MSTATUS_MIE_MASK)
#define ENABLE_GLOBAL_INTERRUPT() enable_global_irq(CSR_MSTATUS_MIE_MASK)

bool foe_reset_request;
static xpi_nor_config_t s_xpi_nor_config;

extern uint32_t (*foe_get_file_addr)(uint16_t MBXMEM * name, uint16_t name_size);
extern uint32_t (*foe_get_file_size)(uint16_t MBXMEM * name, uint16_t name_size);
extern int8_t (*foe_prepare_data_space)(uint16_t MBXMEM * name, uint16_t name_size);
extern int8_t (*foe_store_data)(uint32_t offset, uint16_t MBXMEM * data, uint16_t size, uint8_t data_following);


uint32_t foe_support_get_file_addr(uint16_t MBXMEM * name, uint16_t name_size)
{
    (void) name;
    (void) name_size;

    return (FLASH_ADDR_BASE + FLASH_FILE_ADDR);
}

uint32_t foe_support_get_file_size(uint16_t MBXMEM * name, uint16_t name_size)
{
    (void) name;
    (void) name_size;

    uint32_t size = *(uint32_t *)(FLASH_ADDR_BASE + FLASH_FILE_SIZE_ADDR);

    if (size > FLASH_FILE_SIZE) {
        return 0;
    } else {
        return size;
    }

    return 0;
}

int8_t foe_support_prepare_data_space(uint16_t MBXMEM * name, uint16_t name_size)
{
    hpm_stat_t status;
    (void) name;
    (void) name_size;

    /* erase flash */
    DISABLE_GLOBAL_INTERRUPT();
    status = rom_xpi_nor_erase(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, FLASH_FILE_ADDR, FLASH_FILE_SIZE + 4);
    ENABLE_GLOBAL_INTERRUPT();

    if (status != status_success) {
        return -1;
    }

    return 0;
}

int8_t foe_support_store_data(uint32_t offset, uint16_t MBXMEM * data, uint16_t size, uint8_t data_following)
{
    hpm_stat_t status;

    if (size > 0) {
        DISABLE_GLOBAL_INTERRUPT();
        status = rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)data, (FLASH_FILE_ADDR + offset), size);
        ENABLE_GLOBAL_INTERRUPT();

        if (status !=  status_success) {
            return -1;
        }
    }


    if (!data_following) {
        /* write file size to flash */
        uint32_t file_size = offset + size;
        DISABLE_GLOBAL_INTERRUPT();
        status = rom_xpi_nor_program(BOARD_APP_XPI_NOR_XPI_BASE, xpi_xfer_channel_auto, &s_xpi_nor_config, (uint32_t *)&file_size, FLASH_FILE_SIZE_ADDR, 4);
        ENABLE_GLOBAL_INTERRUPT();

        if (status != status_success) {
            return -1;
        } else {
            /* Do data verification, if verify pass, return 1 */
            return 1;
        }
    }

    return 0;
}

void foe_support_soc_reset(void)
{
    DISABLE_GLOBAL_INTERRUPT();
    ppor_sw_reset(HPM_PPOR, 1000);
    while (1) {
    }
}

int8_t foe_support_init(void)
{
    hpm_stat_t status;
    xpi_nor_config_option_t option;
    option.header.U = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    option.option0.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    option.option1.U = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;

    DISABLE_GLOBAL_INTERRUPT();
    status = rom_xpi_nor_auto_config(BOARD_APP_XPI_NOR_XPI_BASE, &s_xpi_nor_config, &option);
    ENABLE_GLOBAL_INTERRUPT();

    if (status != status_success) {
        return -1;
    }

    foe_get_file_addr = foe_support_get_file_addr;
    foe_get_file_size = foe_support_get_file_size;
    foe_prepare_data_space = foe_support_prepare_data_space;
    foe_store_data = foe_support_store_data;

    return 0;
}

