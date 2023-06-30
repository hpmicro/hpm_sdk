/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_SPI_SDCARD_H
#define HPM_SPI_SDCARD_H

#include "hpm_common.h"
#include "hpm_sdmmc_card.h"

typedef struct {
    sd_csd_t csd;
    sd_cid_t cid;
    uint64_t         capacity;           /* Card Capacity */
    uint32_t         block_size;         /* Card Block Size */
    uint8_t          card_type;
} spi_sdcard_info_t;

typedef struct {
    void       (*set_spi_speed)     (uint32_t freq);
    void       (*cs_select)         (void);
    void       (*cs_relese)         (void);
    bool       (*sdcard_is_present) (void);
    uint8_t    (*write_read_byte)   (uint8_t byte);
    hpm_stat_t (*write_cmd_data)    (uint8_t cmd, uint8_t *buffer, uint32_t size);
    hpm_stat_t (*write)             (uint8_t *buffer, uint32_t size);
    hpm_stat_t (*read)              (uint8_t *buffer, uint32_t size);
} sdcard_spi_interface_t;

#ifdef __cplusplus
extern "C" {
#endif

hpm_stat_t sdcard_spi_status(void);
hpm_stat_t sdcard_spi_init(sdcard_spi_interface_t *spi_io);
hpm_stat_t sdcard_spi_get_card_info(spi_sdcard_info_t *cardinfo);
hpm_stat_t sdcard_spi_read_block(uint32_t sector, uint8_t *buffer);
hpm_stat_t sdcard_spi_write_block(uint32_t sector, uint8_t *buffer);
hpm_stat_t sdcard_spi_read_multi_block(uint8_t *buffer, uint32_t start_sector, uint32_t num_sectors);
hpm_stat_t sdcard_spi_write_multi_block(uint8_t *buffer, uint32_t sector, uint32_t num_sectors);

#ifdef __cplusplus
}
#endif

#endif /* HPM_SPI_SDCARD_H */
