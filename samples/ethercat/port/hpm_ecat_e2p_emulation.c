/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "hpm_ecat_e2p_emulation.h"
#include "hpm_romapi.h"

#if defined(ECAT_INIT_EEPROM_CONTEXT_IN_PROGRAM) && ECAT_INIT_EEPROM_CONTEXT_IN_PROGRAM
extern unsigned char aEepromData[];

/* if eeprom_init_flag[0] == EEPROM_INIT_CODE, init eeprom context from aEepromData, then erase eeprom_init_flag sector */
extern const uint32_t option[4]; /* ref to nor_cfg_option in board.c */
#endif

e2p_t ecat_e2p_emulation;

static uint32_t flash_read(uint8_t *buf, uint32_t addr, uint32_t size)
{
    return nor_flash_read(&ecat_e2p_emulation.nor_config, buf, addr, size);
}

static uint32_t flash_write(uint8_t *buf, uint32_t addr, uint32_t size)
{
    return nor_flash_write(&ecat_e2p_emulation.nor_config, buf, addr, size);
}

static void flash_erase(uint32_t start_addr, uint32_t size)
{
    nor_flash_erase(&ecat_e2p_emulation.nor_config, start_addr, size);
}

hpm_stat_t flash_erase_sector(uint32_t start_addr)
{
    nor_flash_config_t *cfg = &ecat_e2p_emulation.nor_config;
    start_addr > cfg->base_addr ? (start_addr -= cfg->base_addr) : start_addr;
    hpm_stat_t status = rom_xpi_nor_erase_sector(cfg->xpi_base, xpi_xfer_channel_auto, &cfg->nor_config, start_addr);

    return status;
}

hpm_stat_t ecat_flash_eeprom_init(void)
{
    /* initial EEPROM context */
    hpm_stat_t stat;
    uint16_t data, dummy_data = 0xFFFF;
    ecat_e2p_emulation.nor_config.xpi_base = BOARD_APP_XPI_NOR_XPI_BASE;
    ecat_e2p_emulation.nor_config.base_addr = BOARD_FLASH_BASE_ADDRESS;
    ecat_e2p_emulation.config.start_addr = ecat_e2p_emulation.nor_config.base_addr + ECAT_EEPROM_FLASH_OFFSET;
    ecat_e2p_emulation.config.erase_size = ECAT_EEPROM_FLASH_SECTOR_SIZE;
    ecat_e2p_emulation.config.sector_cnt = ECAT_EEPROM_FLASH_SECTOR_CNT;
    ecat_e2p_emulation.config.version = 0x4553; /* 'E' 'S' */
    ecat_e2p_emulation.nor_config.opt_header = BOARD_APP_XPI_NOR_CFG_OPT_HDR;
    ecat_e2p_emulation.nor_config.opt0 = BOARD_APP_XPI_NOR_CFG_OPT_OPT0;
    ecat_e2p_emulation.nor_config.opt1 = BOARD_APP_XPI_NOR_CFG_OPT_OPT1;
    ecat_e2p_emulation.config.flash_read = flash_read;
    ecat_e2p_emulation.config.flash_write = flash_write;
    ecat_e2p_emulation.config.flash_erase = flash_erase;

    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    stat = nor_flash_init(&ecat_e2p_emulation.nor_config);
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
    if (status_success != stat) {
        return status_fail;
    }

    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    stat = e2p_config(&ecat_e2p_emulation);
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
    if (E2P_STATUS_OK != stat) {
        return status_fail;
    }

#if defined(ECAT_INIT_EEPROM_CONTEXT_IN_PROGRAM) && ECAT_INIT_EEPROM_CONTEXT_IN_PROGRAM
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    uint32_t eeprom_init_flag_addr = (uint32_t)&option[0] + sizeof(option);
    uint32_t flag = 0x5a5a5a5a;
    /* printf("addr %x\n", eeprom_init_flag_addr); */
    /* printf("value %x\n", *(uint32_t *)eeprom_init_flag_addr); */ /* cache */

    if (*(uint32_t *)eeprom_init_flag_addr == 0xFFFFFFFF) {
        for (uint32_t i = 0; i < ESC_EEPROM_SIZE / EEPROM_WRITE_SIZE; i++) {
            stat = e2p_write(i, EEPROM_WRITE_SIZE, &aEepromData[2*i]);
            if (stat != E2P_STATUS_OK) {
                break;
            }
        }

        stat = flash_write((uint8_t *)&flag, eeprom_init_flag_addr, 4);
        if (stat == status_success) {
            printf("Init EEPROM context successful.\n");
        }

    } else {
        printf("No need to init EEPROM context.\n");
    }
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
    if (E2P_STATUS_OK != stat) {
        return status_fail;
    }
#endif

    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    for (uint32_t i = 0; i < ESC_EEPROM_SIZE / EEPROM_WRITE_SIZE; i++) {
        stat = e2p_read(i, EEPROM_WRITE_SIZE, (uint8_t *)&data);
        if (stat != E2P_STATUS_OK) {
            stat = e2p_write(i, EEPROM_WRITE_SIZE, (uint8_t *)&dummy_data);
            if (stat != E2P_STATUS_OK) {
                break;
            }
        }
    }
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
    if (E2P_STATUS_OK != stat) {
        return status_fail;
    }

    return status_success;
}

hpm_stat_t ecat_flash_eeprom_write(uint32_t addr, uint8_t *data)
{
    hpm_stat_t stat;
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    stat = e2p_write(addr, EEPROM_WRITE_SIZE, data);
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
    if (stat == status_success) {
        e2p_info("\n WRITE success: addr[%d] - val[0x%x]\n", addr, *((uint16_t *)data));
    } else {
        e2p_info("\n WRITE failed: error code[%d]\n", stat);
    }
    return stat;
}

hpm_stat_t ecat_flash_eeprom_read(uint32_t addr, uint8_t *data)
{
    hpm_stat_t stat;
    stat = e2p_read(addr, EEPROM_WRITE_SIZE, data);
    if (stat == status_success) {
        e2p_info("\n READ success: addr[%d] - val[0x%x]\n", addr, *((uint16_t *)data));
    } else {
        e2p_info("\n READ failed: error code[%d]\n", stat);
    }
    return stat;
}

void ecat_flash_eeprom_format(void)
{
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    e2p_clear();
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
}

void ecat_flash_eeprom_flush(void)
{
    disable_global_irq(CSR_MSTATUS_MIE_MASK);
    e2p_flush(E2P_FLUSH_BEGIN);
    enable_global_irq(CSR_MSTATUS_MIE_MASK);
}

void ecat_flash_eeprom_show_info(void)
{
    e2p_show_info();
}

