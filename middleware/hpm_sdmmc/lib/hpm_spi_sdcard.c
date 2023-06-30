/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "stdio.h"
#include "hpm_spi_sdcard.h"
#include "board.h"

/* CMD wait response timeout*/
#define CMD_WAIT_RESP_TIMEOUT               (1500U)
/* wait idle timeout*/
#define WAIT_IDLE_TIMEOUT                   (100U)
/* spi clk frequency in init */
#define SPI_SPEED_INIT_HZ                   (400000U)
/* spi clk frequency in running */
#define SPI_SPEED_MAX_HZ                    (25000000U)
#define SPI_AUTO_PROBE_COUNT          (2)
#define SPI_SPEED_PROBE_FREQ          (12000000U)

static uint8_t wait_sdcard_ready(void);
static uint8_t send_sdcard_command(uint8_t cmd, uint32_t arg, uint8_t crc);
static uint8_t send_sdcard_command_recv_response(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t *data, uint32_t size);
static uint8_t send_sdcard_command_hold(uint8_t cmd, uint32_t arg, uint8_t crc);
static hpm_stat_t read_sdcard_buffer(uint8_t *buf, uint32_t len);
static hpm_stat_t read_sdcard_info(spi_sdcard_info_t *cardinfo);

static sdcard_type_t g_card_type;
static spi_sdcard_info_t g_card_info;
static bool sdcard_init_status;
static sdcard_spi_interface_t *g_spi_dev = NULL;
static uint32_t g_spi_max_speed;

hpm_stat_t sdcard_spi_status(void)
{
    return (sdcard_init_status) ? status_success : status_fail;
}

hpm_stat_t sdcard_spi_init(sdcard_spi_interface_t *spi_io)
{
    assert(spi_io);
    hpm_stat_t sta = status_success;
    uint8_t buffer[4];
    uint32_t timeout = WAIT_IDLE_TIMEOUT;
    uint8_t response = 0;
    sdcard_init_status = false;
    g_spi_dev = spi_io;

    if (!g_spi_dev->sdcard_is_present()) {
        printf("sdcard no detected !\r\n");
        return status_fail;
    }

    g_spi_dev->cs_relese();
    g_spi_dev->set_spi_speed(SPI_SPEED_INIT_HZ);

    /* Start send 74 clocks at least, it means send 10 byte at least*/
    for (uint32_t i = 0; i < 20; i++) {
        g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
    }

    do {
        response = send_sdcard_command((uint8_t)sdmmc_cmd_go_idle_state, 0, 0x95);
        timeout--;
    } while ((response != SPISD_R1_IDLE_FLAG) && (timeout > 0));

    response = send_sdcard_command_recv_response((uint8_t)sd_cmd_send_if_cond, 0x1AA, 0x87, buffer, sizeof(buffer));

    if (response == SPISD_R1_IDLE_FLAG) {

        /* Check voltage range be 2.7-3.6V    */
        if ((buffer[2] == 0X01) && (buffer[3] == 0xAA)) {
            for (uint16_t i = 0; i < 0xFFF; i++) {

                /* should be return 0x01 */
                response = send_sdcard_command((uint8_t)sdmmc_cmd_app_cmd, 0, 0);

                if (response != 0x01) {
                    printf("send CMD55 should return 0x01, but response is 0x%02x\n", response);
                    return status_timeout;
                }

                /* should be return 0x00 */
                response = send_sdcard_command((uint8_t)sd_acmd_sd_send_op_cond, 0x40000000, 1);

                if (response == 0x00) {
                    break;
                }
            }

            if (response != 0x00) {
                printf("send ACMD41 should return 0x00, but response is 0x%02x\n", response);
                return status_timeout;
            }

             /* Read OCR by CMD58 */
            response = send_sdcard_command_recv_response((uint8_t)sdmmc_cmd_read_ocr, 0, 0, buffer, sizeof(buffer));

            if (response != 0x00) {
                printf("Send CMD58 should return 0x00, response=0x%02x\r\n", response);
                return status_timeout;
            }

            /* OCR -> CCS(bit30)  1: SDV2HC     0: SDV2 */
            g_card_type = (buffer[0] & 0x40) ? card_type_sd_v2_hc : card_type_sd_v2;

            response = send_sdcard_command((uint8_t)sd_cmd_crc_option, 0, 0);

            if (response != 0x00) {
                printf("Send CMD59 should return 0x00, response=0x%02x\r\n", response);
                return status_timeout;
            }

            /* using the module with multiple buffers, maybe cause abnormal communication in spi clk high frequency */
            g_spi_max_speed = SPI_SPEED_MAX_HZ;
            for (uint8_t i = 0; i < SPI_AUTO_PROBE_COUNT; i++) {
                g_spi_dev->set_spi_speed(g_spi_max_speed);
                sta = read_sdcard_info(&g_card_info);
                if (sta == status_fail) {
                    g_spi_max_speed = SPI_SPEED_PROBE_FREQ;
                    continue;
                }
                break;
            }
        }
    } else {
        return status_fail;
    }
    if (sta == status_success) {
        sdcard_init_status = true;
    }
    return sta;
}

hpm_stat_t sdcard_spi_get_card_info(spi_sdcard_info_t *cardinfo)
{
    assert(cardinfo);
    if (sdcard_init_status) {
        memcpy(cardinfo, &g_card_info, sizeof(spi_sdcard_info_t));
        return status_success;
    }
    return status_fail;
}

hpm_stat_t sdcard_spi_read_block(uint32_t sector, uint8_t *buffer)
{
    hpm_stat_t sta = status_fail;
    assert(g_spi_dev);
    if (g_card_type != card_type_sd_v2_hc) {
        sector = sector << 9;
    }
    g_spi_dev->cs_select();
    if (send_sdcard_command_hold((uint8_t)sdmmc_cmd_read_single_block, sector, 0x00) == 0x00) {
        sta = read_sdcard_buffer(buffer, SPI_SD_BLOCK_SIZE);
    }
    g_spi_dev->cs_relese();
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
    return sta;
}

hpm_stat_t sdcard_spi_write_block(uint32_t sector, uint8_t *buffer)
{
    assert(g_spi_dev);
    hpm_stat_t ret = status_fail;

    if (g_card_type != card_type_sd_v2_hc) {
        sector = sector << 9;
    }
    if (send_sdcard_command((uint8_t)sdmmc_cmd_write_single_block, sector, 0) != 0x00) {
        return ret;
    }

    g_spi_dev->cs_select();
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);

    /* Start data write token: 0xFE */
    g_spi_dev->write_read_byte(0xFE);

    ret = g_spi_dev->write(buffer, SPI_SD_BLOCK_SIZE);

    /* 2Bytes dummy CRC */
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);

    /* MSD card accept the data */
    uint8_t response = g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);

    if ((response & 0x1F) == 0x05) {

        /* Wait all the data program finished */
        for (uint32_t i = 0; i < 0x40000; i++) {
            if (g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE) != 0x00) {
                ret = status_success;
                break;
            }
        }
    }

    g_spi_dev->cs_relese();
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);

    return ret;
}

hpm_stat_t sdcard_spi_read_multi_block(uint8_t *buffer, uint32_t start_sector, uint32_t num_sectors)
{
    assert(g_spi_dev);
    if (g_card_type != card_type_sd_v2_hc) {
        start_sector = start_sector << 9;
    }
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
    g_spi_dev->cs_select();

    if (send_sdcard_command((uint8_t)sdmmc_cmd_read_multiple_block, start_sector, 0xff) != 0x00) {
        return status_fail;
    }

    for (uint32_t i = 0; i < num_sectors; i++) {

        hpm_stat_t ret = read_sdcard_buffer(&buffer[i * SPI_SD_BLOCK_SIZE], SPI_SD_BLOCK_SIZE);

        if (ret != status_success) {
            /* Send stop data transmit command - CMD12    */
            send_sdcard_command((uint8_t)sdmmc_cmd_stop_transmission, 0, 0xff);
            return status_fail;
        }
    }

    /* Send stop data transmit command - CMD12 */
    send_sdcard_command((uint8_t)sdmmc_cmd_stop_transmission, 0, 0xff);

    g_spi_dev->cs_relese();
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);

    return status_success;
}

hpm_stat_t sdcard_spi_write_multi_block(uint8_t *buffer, uint32_t sector, uint32_t num_sectors)
{
    assert(g_spi_dev);

    if (g_card_type != card_type_sd_v2_hc) {
        sector = sector << 9;
    }
    hpm_stat_t sta;
    for (uint32_t i = 0; i < num_sectors; i++) {
        sta = sdcard_spi_write_block(sector + i, &buffer[i * SPI_SD_BLOCK_SIZE]);
        if (sta != status_success) {
            break;
        }
    }
    return sta;
}

static uint8_t wait_sdcard_ready(void)
{
    uint8_t response = 0;
    for (uint32_t i = 0; i < CMD_WAIT_RESP_TIMEOUT; i++) {
        g_spi_dev->cs_select();
        response = g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
        g_spi_dev->cs_relese();
        if (response == 0xFF) {
            break;
        }
    }
    return response;
}

static uint8_t send_sdcard_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t response = 0xFF;
    uint8_t _cmd = cmd | 0x40;
    uint8_t packet[] = {arg >> 24, arg >> 16, arg >> 8, arg, crc};
    uint8_t dummy = SPISD_DUMMY_BYTE;
    if (wait_sdcard_ready() != 0xFF) {
        return 0xFF;
    }
    g_spi_dev->cs_select();
    g_spi_dev->write_cmd_data(_cmd, packet, sizeof(packet));
    /* Wait response, quit till timeout */
    for (uint32_t i = 0; i < CMD_WAIT_RESP_TIMEOUT; i++) {
        response = g_spi_dev->write_read_byte(dummy);
        if (response != 0xFF) {
            break;
        }
    }
    /* Chip disable */
    g_spi_dev->cs_relese();

    return response;
}

static uint8_t send_sdcard_command_recv_response(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t *data, uint32_t size)
{
    /* Dummy byte and chip enable */
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
    if (wait_sdcard_ready() != 0xFF) {
        return 0xFF;
    }
    uint8_t _cmd = cmd | 0x40;
    uint8_t packet[] = {arg >> 24, arg >> 16, arg >> 8, arg, crc};
    uint8_t response = SPISD_DUMMY_BYTE;
    g_spi_dev->cs_select();

    g_spi_dev->write_cmd_data(_cmd, packet, sizeof(packet));

     /* Wait response, quit till timeout */
    for (uint32_t i = 0; i < CMD_WAIT_RESP_TIMEOUT; i++) {
        response = g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
        if (response != SPISD_DUMMY_BYTE) {
            g_spi_dev->read(data, size);
            break;
        }
    }
    /* Chip disable and dummy byte */
    g_spi_dev->cs_relese();
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);

    return response;
}

static uint8_t send_sdcard_command_hold(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t _cmd = cmd | 0x40;
    uint8_t packet[] = {arg >> 24, arg >> 16, arg >> 8, arg, crc};
    uint8_t response = SPISD_DUMMY_BYTE;

    /* Dummy byte and chip enable */
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
    if (wait_sdcard_ready() != 0xFF) {
        return 0xFF;
    }
    g_spi_dev->cs_select();
    g_spi_dev->write_cmd_data(_cmd, packet, sizeof(packet));

    /* Wait response, quit till timeout */
    for (uint32_t i = 0; i < CMD_WAIT_RESP_TIMEOUT; i++) {

        response = g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);

        if (response != 0xFF) {
            break;
        }
    }
    g_spi_dev->cs_relese();
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);

    return response;
}

static hpm_stat_t read_sdcard_buffer(uint8_t *buf, uint32_t len)
{
    uint8_t response = 0;
    g_spi_dev->cs_select();
        /* Wait start-token 0xFE */
    for (uint32_t i = 0; i < CMD_WAIT_RESP_TIMEOUT; i++) {
        response = g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
        if (response == 0xFE) {
            break;
        }
    }

    if (response != 0xFE) {
        return status_fail;
    }

    g_spi_dev->read(buf, len);

    /* 2bytes dummy CRC */
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
    g_spi_dev->cs_relese();

    return status_success;
}

static hpm_stat_t read_sdcard_info(spi_sdcard_info_t *cardinfo)
{
    uint8_t temp[16];
    assert(g_spi_dev);

    /* Start send 74 clocks at least, it means send 10 byte at least*/
    for (uint32_t i = 0; i < 20; i++) {
        g_spi_dev->cs_select();
        g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);
        g_spi_dev->cs_relese();
    }

    /* Send CMD9, Read CSD */
    uint8_t response = send_sdcard_command((uint8_t)sdmmc_cmd_send_csd, 0, 0xFF);

    if (response != 0x00) {
        return status_fail;
    }

    g_spi_dev->cs_select();

    hpm_stat_t ret = read_sdcard_buffer(temp, sizeof(temp));

    /* chip disable and dummy byte */
    g_spi_dev->cs_relese();
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);

    if (ret != status_success) {
        return ret;
    }

    /* Byte 0 */
    cardinfo->csd.csd_structure              = (temp[0] & 0xC0) >> 6;
    /* Byte 1 */
    cardinfo->csd.data_read_access_time1     = temp[1] ;
    /* Byte 2 */
    cardinfo->csd.data_read_access_time2     = temp[2];
    /* Byte 3 */
    cardinfo->csd.transfer_speed             = temp[3];
    /* Byte 4 */
    cardinfo->csd.card_command_class         = temp[4] << 4;
    /* Byte 5 */
    cardinfo->csd.card_command_class        |= (temp[5] & 0xF0) >> 4;
    cardinfo->csd.read_block_len             = temp[5] & 0x0F;
    /* Byte 6 */
    if (((temp[6] & 0x80) >> 7) != 0) {
        cardinfo->csd.support_read_block_partial = true;
    }
    if (((temp[6] & 0x40) >> 6) != 0) {
        cardinfo->csd.support_write_block_misalignment = true;
    }
    if (((temp[6] & 0x20) >> 5) != 0) {
        cardinfo->csd.support_read_block_misalignment = true;
    }
    if (((temp[6] & 0x10) >> 4) != 0) {
        cardinfo->csd.is_dsr_implemented = true;
    }
    cardinfo->csd.device_size               = (temp[6] & 0x03) << 10;
    /* Byte 7 */
    cardinfo->csd.device_size              |= (temp[7]) << 2;
    /* Byte 8 */
    cardinfo->csd.device_size              |= (temp[8] & 0xC0) >> 6;
    cardinfo->csd.read_current_vdd_min      = (temp[8] & 0x38) >> 3;
    cardinfo->csd.read_current_vdd_max      = (temp[8] & 0x07);
    /* Byte 9 */
    cardinfo->csd.write_current_vdd_min     = (temp[9] & 0xE0) >> 5;
    cardinfo->csd.write_current_vdd_max     = (temp[9] & 0x1C) >> 2;
    cardinfo->csd.device_size_multiplier    = (temp[9] & 0x03) << 1;
    /* Byte 10 */
    if ((uint8_t) ((temp[10] & 0x80) >> 7) != 0U) {
        cardinfo->csd.is_erase_block_enabled = true;
    }
    cardinfo->csd.erase_sector_size = (temp[10] & 0x7f);
    /* Byte 11 */
    cardinfo->csd.write_protect_group_size   = (temp[11] & 0x1F);
    /* Byte 12 */
    if ((uint8_t) ((temp[12] & 0x80) >> 7) != 0U) {
        cardinfo->csd.is_write_protection_group_enabled = true;
    }
    cardinfo->csd.write_speed_factor       = (temp[12] & 0x1C) >> 2;
    cardinfo->csd.max_write_block_len      = (temp[12] & 0x03) << 2;
    /* Byte 13 */
    if ((uint8_t) ((temp[13] & 0x20) >> 5) != 0U) {
        cardinfo->csd.support_write_block_partial = true;
    }
    cardinfo->csd.max_write_block_len     |= (temp[13] & 0xc0) >> 6;
    /* Byte 14 */
    if ((uint8_t) ((temp[14] & 0x80) >> 7) != 0U) {
        cardinfo->csd.support_file_format_group = true;
    }
    if ((uint8_t) ((temp[14] & 0x40) >> 6) != 0U) {
        cardinfo->csd.support_copy = true;
    }
    if ((uint8_t) ((temp[14] & 0x20) >> 5) != 0U) {
        cardinfo->csd.support_permanent_write_protect = true;
    }
    if ((uint8_t) ((temp[14] & 0x10) >> 4) != 0U) {
        cardinfo->csd.support_temporary_write_protect = true;
    }
    cardinfo->csd.file_format             = (temp[14] & 0x0C) >> 2;
    cardinfo->card_type = g_card_type;
    if (cardinfo->card_type == card_type_sd_v2_hc) {
        /* Byte 7 */
        cardinfo->csd.device_size         = temp[9] + (temp[8] << 8) + 1;
        /* Byte 8 */
        cardinfo->csd.device_size         = cardinfo->csd.device_size << 10;
    }

    cardinfo->capacity                    = (uint64_t)(cardinfo->csd.device_size) * SPI_SD_BLOCK_SIZE;
    cardinfo->block_size                  = SPI_SD_BLOCK_SIZE;

    /* Send CMD10, Read CID */
    response = send_sdcard_command((uint8_t)sdmmc_cmd_send_cid, 0, 0xFF);

    if (response != 0x00) {
        return status_fail;
    }

    g_spi_dev->cs_select();
    ret = read_sdcard_buffer(temp, sizeof(temp));

    /* chip disable and dummy byte */
    g_spi_dev->cs_relese();
    g_spi_dev->write_read_byte(SPISD_DUMMY_BYTE);

    if (ret != status_success) {
        return ret;
    }


    /* Byte 0 */
    cardinfo->cid.mid   = temp[0];
    /* Byte 1 */
    cardinfo->cid.oid   = temp[1] << 8;
    /* Byte 2 */
    cardinfo->cid.oid  |= temp[2];
    /* Byte 3 */
    cardinfo->cid.pnm   = temp[3] << 24;
    /* Byte 4 */
    cardinfo->cid.pnm  |= temp[4] << 16;
    /* Byte 5 */
    cardinfo->cid.pnm  |= temp[5] << 8;
    /* Byte 6 */
    cardinfo->cid.pnm  |= temp[6];
    /* Byte 7 */
    cardinfo->cid.pnm   = temp[7];
    /* Byte 8 */
    cardinfo->cid.prv   = temp[8];
    /* Byte 9 */
    cardinfo->cid.psn   = temp[9] << 24;
    /* Byte 10 */
    cardinfo->cid.psn  |= temp[10] << 16;
    /* Byte 11 */
    cardinfo->cid.psn  |= temp[11] << 8;
    /* Byte 12 */
    cardinfo->cid.psn  |= temp[12];
    /* Byte 13 */
    /* Byte 14 */
    cardinfo->cid.mdt   = (temp[13] & 0x0F) << 8;
    /* Byte 15 */
    cardinfo->cid.mdt  |= temp[14];
    /* Byte 16 */
    cardinfo->cid.crc7  = (temp[15] & 0xFE) >> 1;

    return status_success;
}
