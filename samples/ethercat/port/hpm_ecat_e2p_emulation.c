/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "hpm_ecat_e2p_emulation.h"
#include "hpm_romapi.h"
#include "hpm_ecat_hw.h"

extern unsigned char aEepromData[]; /* EEPROM data is created in eeprom.h by SSC Tool */

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

/* check EtherCAT Slave Controller Configuration Area is first 8 words(1 word = 2 bytes) */
static hpm_stat_t ecat_flash_eeprom_check_configuration_area(void)
{
    hpm_stat_t stat;
    uint16_t config_data[8];
    uint8_t checksum;
    for (uint8_t i = 0; i < 8; i++) {
        /* read Configuration Area data from e2p */
        stat = e2p_read(i, EEPROM_WRITE_SIZE, (uint8_t *)&config_data[i]);
        if (stat != E2P_STATUS_OK) {
            return status_fail;
        }
    }

    checksum = ecat_calculate_eeprom_config_data_checksum((uint8_t *)config_data, 14); /* calculate checksum value for word0 - word6 */

    /* Low byte contains remainder of division of word 0 to word 6 as unsigned number divided by the polynomial x8+x2+x+1(initial value 0xFF). */
    if (checksum != config_data[7]) {
        return status_invalid_argument; /* checksum error */
    }

    return status_success;
}

hpm_stat_t ecat_flash_eeprom_init(void)
{
    /* initial EEPROM content */
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


    /* If the value of aEepromData is the initial value in weak declaration, it means there is no valid data in aEepromData */
    if ((aEepromData[0] == 0xa5) && (aEepromData[1] == 0xa5) && (aEepromData[2] == 0xa5) && (aEepromData[3] == 0xa5)) {
        printf("No EEPROM content in PROGRAM.\n");
    } else {
        bool require_init_eeprom = false;
        stat = ecat_flash_eeprom_check_configuration_area();
        if (status_success == stat) {
            /* if eeprom configuration data checksum pass, then check Product Code and Revision Number */
#if defined(ECAT_EEPROM_CHECK_PRODUCT_CODE_AND_REVISION) && ECAT_EEPROM_CHECK_PRODUCT_CODE_AND_REVISION
            uint32_t product_code, revision_num;
            uint16_t product_code_low, product_code_high, revision_num_low, revision_num_high;
            stat = e2p_read(ESC_EEPROM_PRODUCT_CODE_WORD_INDEX, EEPROM_WRITE_SIZE, (uint8_t *)&product_code_low);
            if (stat != E2P_STATUS_OK) {
                printf("Read Product Code in EEPROM failed.\n");
                return status_fail;
            }

            stat = e2p_read(ESC_EEPROM_PRODUCT_CODE_WORD_INDEX + 1, EEPROM_WRITE_SIZE, (uint8_t *)&product_code_high);
            if (stat != E2P_STATUS_OK) {
                printf("Read Product Code in EEPROM failed.\n");
                return status_fail;
            }
            product_code = (uint32_t)product_code_high << 16 | product_code_low;

            stat = e2p_read(ESC_EEPROM_REVISION_NUM_WORD_INDEX, EEPROM_WRITE_SIZE, (uint8_t *)&revision_num_low);
            if (stat != E2P_STATUS_OK) {
                printf("Read Revision Number in EEPROM failed.\n");
                return status_fail;
            }

            stat = e2p_read(ESC_EEPROM_REVISION_NUM_WORD_INDEX + 1, EEPROM_WRITE_SIZE, (uint8_t *)&revision_num_high);
            if (stat != E2P_STATUS_OK) {
                printf("Read Revision Number in EEPROM failed.\n");
                return status_fail;
            }
            revision_num = (uint32_t)revision_num_high << 16 | revision_num_low;

            /* if Procde Code not match or aEepromData[ESC_EEPROM_REVISION_NUM_WORD_INDEX] > stored revesion, require to update eeprom by aEepromData */
            if ((product_code != ((uint32_t *)aEepromData)[ESC_EEPROM_PRODUCT_CODE_WORD_INDEX / 2]) || (revision_num < ((uint32_t *)aEepromData)[ESC_EEPROM_REVISION_NUM_WORD_INDEX / 2])) {
                require_init_eeprom = true;
            }
#endif
        } else {
            require_init_eeprom = true;
        }

        if (require_init_eeprom) {
            printf("Init EEPROM content.\n");
            disable_global_irq(CSR_MSTATUS_MIE_MASK);
            /* init eeprom content */
            for (uint32_t i = 0; i < ESC_EEPROM_SIZE / EEPROM_WRITE_SIZE; i++) {
                stat = e2p_write(i, EEPROM_WRITE_SIZE, &aEepromData[2*i]);
                if (stat != E2P_STATUS_OK) {
                    printf("Init EEPROM content failed.\n");
                    return status_fail;
                }
            }
            enable_global_irq(CSR_MSTATUS_MIE_MASK);
            printf("Init EEPROM content successful.\n");
        } else {
            printf("No need to init EEPROM content.\n");
        }
    }

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

