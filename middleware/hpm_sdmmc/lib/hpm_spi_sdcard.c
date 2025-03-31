/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "stdio.h"
#include "hpm_spi_sdcard.h"
#include "board.h"

#ifndef SPI_SD_LOG
#define SPI_SD_LOG(...)
#endif

#define  CSD_VERSION_V1_0             (0U)
#define  CSD_VERSION_V2_0             (1U)
#define  CSD_VERSION_V3_0             (2U)

/* CMD wait response timeout*/
#define CMD_WAIT_RESP_TIMEOUT         (0xFFFFFFFFU)
/* spi clk frequency in init */
#define SPI_SPEED_INIT_HZ             (400000U)
/* spi clk frequency in running */
#if defined(USE_DMA_TRANSFER) && (USE_DMA_TRANSFER == 1)
#ifndef SPI_SD_SPEED_MAX_HZ
#define SPI_SD_SPEED_MAX_HZ           (25000000U)
#endif
#else
#ifndef SPI_SD_SPEED_MAX_HZ
#define SPI_SD_SPEED_MAX_HZ           (20000000U)
#endif
#endif
#ifndef SPI_SD_AUTO_PROBE_COUNT
#define SPI_SD_AUTO_PROBE_COUNT       (5U)
#endif
#define SPI_SPEED_PROBE_FREQ          (12000000U)

#define CRC16_CCITT_SEED              0
#define CRC16_CCITT_POLY16            0x1021

#ifndef SPI_SD_RETRY_COUNT
#define SPI_SD_RETRY_COUNT            (5U)
#endif

#ifndef SPI_SD_DELAY_DEFAULT_US
#define SPI_SD_DELAY_DEFAULT_US      (100U)
#endif

static hpm_stat_t send_sdcard_command(uint8_t cmd, uint32_t arg, uint8_t crc);
static hpm_stat_t read_sdcard_buffer(uint8_t *buf, uint32_t len);
static hpm_stat_t read_sdcard_info(spi_sdcard_info_t *cardinfo);
static uint8_t crc7_calc(uint8_t *data, uint8_t len);
static bool check_cid_data(bool check_crc);
static hpm_stat_t sdcard_wait_not_busy(void);
static uint8_t sdcard_read_r1_status(void);
static hpm_stat_t sdcard_wait_data_token(uint8_t token);

static volatile sdcard_type_t g_card_type;
static spi_sdcard_info_t g_card_info;
static volatile bool sdcard_init_status;
static volatile sdcard_spi_interface_t *g_spi_dev = NULL;
static volatile uint32_t g_spi_max_speed;
static uint8_t dummy_write_byte = SPISD_DUMMY_BYTE;
static uint8_t dummy_read_byte;

hpm_stat_t sdcard_spi_status(void)
{
    return (sdcard_init_status) ? status_success : status_fail;
}

hpm_stat_t sdcard_spi_init(sdcard_spi_interface_t *spi_io)
{
    assert(spi_io);
    uint32_t i;
    hpm_stat_t sta = status_success;
    bool probe_cid_check = false;
    uint8_t buffer[4];
    uint8_t response = 0;
    uint8_t count = 0;
    sdcard_init_status = false;
    g_spi_dev = spi_io;

    if (!g_spi_dev->sdcard_is_present()) {
        SPI_SD_LOG("[spi_sdcard] [spi_sdcard] sdcard no detected !\r\n");
        return status_fail;
    }

    sta = g_spi_dev->set_spi_speed(SPI_SPEED_INIT_HZ);
    if (sta != status_success) {
        return sta;
    }

    /* Step 1. Start send 80 clocks at least, it means send 10 byte at least*/
    g_spi_dev->cs_relese();
    for (uint32_t i = 0; i < 10; i++) {
        g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);
    }

    g_spi_dev->cs_select();
    /* Step 2. Send CMD0 (GO_IDLE_STATE): Reset the SD card. */
    /* It is possible that the card has been abnormal before initialization. You can reset the SD card multiple times to ensure that it enters the Idle state. */
    for (count = 0; count < SPI_SD_RETRY_COUNT; count++) {
        sta = sdcard_wait_not_busy();
        if (sta != status_success) {
            sta = status_fail;
            continue;
        }
        sta = send_sdcard_command((uint8_t)sdmmc_cmd_go_idle_state, 0, 0x95) ;
        if (sta != status_success) {
            SPI_SD_LOG("[spi_sdcard] reset SDcard fail\n");
            sta = status_fail;
            continue;
        }
        if ((sdcard_read_r1_status() & SPISD_R1_IDLE_FLAG) != SPISD_R1_IDLE_FLAG) {
            SPI_SD_LOG("[spi_sdcard] not go idle status\n");
            sta = status_fail;
            continue;
        }
    }
    if (sta != status_success) {
        g_spi_dev->cs_relese();
        SPI_SD_LOG("[spi_sdcard] not go idle status\n");
        return sta;
    }

    /* Step 3. read CMD8 */
    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    if (send_sdcard_command((uint8_t)sd_cmd_send_if_cond, 0x1AA, 0x86) != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    if (sdcard_read_r1_status() != SPISD_R1_IDLE_FLAG) {
        g_spi_dev->cs_relese();
        SPI_SD_LOG("[spi_sdcard] not support V1.X SDcard or not SDcard\\n");
        return status_fail;
    }
    sta = g_spi_dev->read(buffer, sizeof(buffer));
    if (sta != status_success) {
        g_spi_dev->cs_relese();
        return sta;
    }
    /* Check voltage range be 2.7-3.6V    */
    if (((buffer[2] & 0x01) != 1) || (buffer[3] != 0xAA)) {
        g_spi_dev->cs_relese();
        return status_fail;
    }

    /* Step 3. init CMD41  with HCS flag (bit 30) */
    for (i = 0; i < 0xFFFFFFFF; i++) {
        if (sdcard_wait_not_busy() != status_success) {
            g_spi_dev->cs_relese();
            return status_fail;
        }
        if (send_sdcard_command((uint8_t)sdmmc_cmd_app_cmd, 0, (0x7F << 1) | 1) != status_success) {
            g_spi_dev->cs_relese();
            return status_fail;
        }
        response = sdcard_read_r1_status();
        if ((response != SPISD_R1_IDLE_FLAG) && (response > 0)) {
            g_spi_dev->cs_relese();
            SPI_SD_LOG("[spi_sdcard] send CMD55 should return 0x01, but response is 0x%02x\n", response);
            return status_fail;
        }
        if (sdcard_wait_not_busy() != status_success) {
            g_spi_dev->cs_relese();
            return status_fail;
        }
        if (send_sdcard_command((uint8_t)sd_acmd_sd_send_op_cond, 0x40000000, 1) != status_success) {
            g_spi_dev->cs_relese();
            return status_fail;
        }
        response = sdcard_read_r1_status();
        if (response == 0x00) {
            break;
        }
        if (response != 0x01) {
            g_spi_dev->cs_relese();
            SPI_SD_LOG("[spi_sdcard] send ACMD41 should return 0x00, but response is 0x%02x\n", response);
            return status_timeout;
        }
    }

    /* Step 5. read OCR(CMD58) and check CCS flag to check he card is a high-capacity card known as SDHC/SDXC */
    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    /* Read OCR by CMD58 */
    if (send_sdcard_command((uint8_t)sdmmc_cmd_read_ocr, 0, (0x7F << 1) | 1) != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    response = sdcard_read_r1_status();
    if (response != 0x00) {
        g_spi_dev->cs_relese();
        SPI_SD_LOG("[spi_sdcard] Send CMD58 should return 0x00, response=0x%02x\r\n", response);
        return status_timeout;
    }
    sta = g_spi_dev->read(buffer, sizeof(buffer));
    if (sta != status_success) {
        g_spi_dev->cs_relese();
        return sta;
    }
    /* OCR -> CCS(bit30)  1: SDV2HC     0: SDV2 */
    g_card_type = (buffer[0] & 0x40) ? card_type_sd_v2_hc : card_type_sd_v2;

    /* Step 6. read CSD info */
    sta = read_sdcard_info(&g_card_info);
    if (sta != status_success) {
        SPI_SD_LOG("[spi_sdcard] read sdcard info fail\n");
        return sta;
    }
    /* using the module with multiple buffers, maybe cause abnormal communication in spi clk high frequency */
    g_spi_max_speed = SPI_SD_SPEED_MAX_HZ;
    for (uint8_t i = 0; i < SPI_SD_AUTO_PROBE_COUNT; i++) {
        g_spi_dev->set_spi_speed(g_spi_max_speed);
        probe_cid_check = check_cid_data(false);
        if (probe_cid_check == false) {
            g_spi_max_speed = SPI_SPEED_PROBE_FREQ;
            continue;
        }
        break;
    }
    sdcard_init_status = true;
    g_spi_dev->cs_relese();
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
    hpm_stat_t sta = status_success;
    uint8_t response;
    assert(g_spi_dev);
    if (g_card_type != card_type_sd_v2_hc) {
        sector = sector << 9;
    }
    g_spi_dev->cs_select();
    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    if (send_sdcard_command((uint8_t)sdmmc_cmd_read_single_block, sector, (0x7F << 1) | 1) != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    response = sdcard_read_r1_status();
    if (response != 0x00) {
        g_spi_dev->cs_relese();
        return status_timeout;
    }
    if (sdcard_wait_data_token(SPISD_START_TOKEN) != status_success) {
        g_spi_dev->cs_relese();
        return status_timeout;
    }
    if (read_sdcard_buffer(buffer, SPI_SD_BLOCK_SIZE) != status_success) {
        g_spi_dev->cs_relese();
        return status_timeout;
    }
    g_spi_dev->cs_relese();
    return sta;
}

hpm_stat_t sdcard_spi_write_block(uint32_t sector, uint8_t *buffer)
{
    assert(g_spi_dev);
    hpm_stat_t ret = status_success;
    uint8_t response;
    uint8_t data;
    if (g_card_type != card_type_sd_v2_hc) {
        sector = sector << 9;
    }
    g_spi_dev->cs_select();
    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    /* Considering SD card compatibility, add busy waiting time */
#if (SPI_SD_DELAY_DEFAULT_US > 0)
    if (g_spi_dev->delay_us != NULL) {
        g_spi_dev->delay_us(SPI_SD_DELAY_DEFAULT_US);
    }
#endif
    if (send_sdcard_command((uint8_t)sdmmc_cmd_write_single_block, sector, (0x7F << 1) | 1) != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    response = sdcard_read_r1_status();
    if (response != 0x00) {
        g_spi_dev->cs_relese();
        return status_timeout;
    }
   /* Start data write token: 0xFE */
    data = SPISD_START_TOKEN;
    g_spi_dev->write_read_byte(&data, &dummy_read_byte);
    g_spi_dev->write(buffer, SPI_SD_BLOCK_SIZE);
    /* 2Bytes dummy CRC */
    g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);
    g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);


    /* MSD card accept the data */
    ret = g_spi_dev->write_read_byte(&dummy_write_byte, &response);
    if (ret != status_success) {
        g_spi_dev->cs_relese();
        return status_timeout;
    }
    if ((response & 0x1F) != 0x05) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    g_spi_dev->cs_relese();
    return ret;
}

hpm_stat_t sdcard_spi_read_multi_block(uint8_t *buffer, uint32_t start_sector, uint32_t num_sectors)
{
    uint8_t response;
    hpm_stat_t stat = status_success;
    assert(g_spi_dev);
    if (g_card_type != card_type_sd_v2_hc) {
        start_sector = start_sector << 9;
    }
    /* step1. read begin */
    g_spi_dev->cs_select();
    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    if (send_sdcard_command((uint8_t)sdmmc_cmd_read_multiple_block, start_sector, (0x7F << 1) | 1) != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    response = sdcard_read_r1_status();
    if (response != 0x00) {
        g_spi_dev->cs_relese();
        return status_timeout;
    }

    /* step2. read data */
    for (uint32_t i = 0; i < num_sectors; i++) {
        if (sdcard_wait_data_token(SPISD_START_TOKEN) != status_success) {
            g_spi_dev->cs_relese();
            return status_timeout;
        }
        if (read_sdcard_buffer(&buffer[i * SPI_SD_BLOCK_SIZE], SPI_SD_BLOCK_SIZE) != status_success) {
            send_sdcard_command((uint8_t)sdmmc_cmd_stop_transmission, 0, (0x7F << 1) | 1);
            g_spi_dev->cs_relese();
            return status_timeout;
        }
    }

    /* step3. read end */
    if (send_sdcard_command((uint8_t)sdmmc_cmd_stop_transmission, 0, (0x7F << 1) | 1) != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    stat = g_spi_dev->write_read_byte(&dummy_write_byte, &response);
    if (stat != status_success) {
        g_spi_dev->cs_relese();
        return status_timeout;
    }
    g_spi_dev->cs_relese();
    return status_success;
}

hpm_stat_t sdcard_spi_write_multi_block(uint8_t *buffer, uint32_t sector, uint32_t num_sectors)
{
    hpm_stat_t sta = status_success;
    uint8_t start_token = SPISD_START_MULTI_WRITE_TOKEN;
    uint8_t end_token = SPISD_END_MULTI_WRITE_TOKEN;
    uint8_t response;
    uint32_t i;
    if (g_card_type != card_type_sd_v2_hc) {
        sector = sector << 9;
    }
    g_spi_dev->cs_select();
    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    /* Considering SD card compatibility, add busy waiting time */
#if (SPI_SD_DELAY_DEFAULT_US > 0)
    if (g_spi_dev->delay_us != NULL) {
        g_spi_dev->delay_us(SPI_SD_DELAY_DEFAULT_US);
    }
#endif
    if (send_sdcard_command((uint8_t)sdmmc_cmd_write_multiple_block, sector, (0x7F << 1) | 1) != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    response = sdcard_read_r1_status();
    if (response != 0x00) {
        g_spi_dev->cs_relese();
        return status_timeout;
    }

    for (i = 0; i < num_sectors; i++) {
        /* Start multi block write token: 0xFC */
        g_spi_dev->write_read_byte(&start_token, &dummy_read_byte);
        g_spi_dev->write(&buffer[i * SPI_SD_BLOCK_SIZE], SPI_SD_BLOCK_SIZE);
        /* 2Bytes dummy CRC */
        g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);
        g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);

        /* MSD card accept the data */
        g_spi_dev->write_read_byte(&dummy_write_byte, &response);
        if ((response & 0x1F) != 0x05) {
            g_spi_dev->cs_relese();
            return status_fail;
        }
        if (sdcard_wait_not_busy() != status_success) {
            g_spi_dev->cs_relese();
            return status_fail;
        }
    }

    /* Send end of transmit token: 0xFD */
    sta = g_spi_dev->write_read_byte(&end_token, &response);
    if (sta != status_success) {
        g_spi_dev->cs_relese();
        return status_timeout;
    }
    g_spi_dev->write_read_byte(&dummy_write_byte, &response);
    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    g_spi_dev->cs_relese();
    return sta;
}

static uint8_t sdcard_read_r1_status(void)
{
    uint8_t r1 = 0xFF;
    uint32_t repeat = CMD_WAIT_RESP_TIMEOUT;
    while (repeat--) {
        if (g_spi_dev->write_read_byte(&dummy_write_byte, &r1) != status_success) {
            return dummy_write_byte;
        }
        if ((r1 & 0x80) == 0) {
            break;
        }
    }
    return r1;
}

static hpm_stat_t sdcard_wait_not_busy(void)
{
    hpm_stat_t stat = status_success;
    uint8_t busy;
    uint32_t repeat = CMD_WAIT_RESP_TIMEOUT;
    while (1) {
        stat = g_spi_dev->write_read_byte(&dummy_write_byte, &busy);
        if (stat != status_success) {
            return stat;
        }
        if (busy == SPISD_DUMMY_BYTE) {
            break;
        }
        repeat--;
        if (repeat == 0) {
            return status_timeout;
        }
    }
    return stat;
}

static hpm_stat_t sdcard_wait_data_token(uint8_t token)
{
    hpm_stat_t stat = status_success;
    uint8_t read_byte;
    uint32_t repeat = CMD_WAIT_RESP_TIMEOUT;
    while (1) {
        stat = g_spi_dev->write_read_byte(&dummy_write_byte, &read_byte);
        if (stat != status_success) {
            return stat;
        }
        if (read_byte == token) {
            break;
        }
        if (read_byte != 0xFF) {
            return status_fail;
        }
        repeat--;
        if (repeat == 0) {
            return status_timeout;
        }
    }
    return stat;
}
static uint8_t crc7_calc(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0x00;
    uint8_t crc_polynomial = 0x89;
    uint8_t i, j;
    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc ^= crc_polynomial;
            }
            crc = crc << 1;
        }
    }
    crc = crc >> 1;
    return crc;
}

static bool check_cid_data(bool check_crc)
{
    uint8_t temp[16];
    uint8_t response;
    hpm_stat_t sta;
    uint32_t i;
    uint8_t crc = 0;
    /* Send CMD10, Read CID */
    g_spi_dev->cs_select();
    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return false;
    }
    if (send_sdcard_command((uint8_t)sdmmc_cmd_send_csd, 0, (0x7F << 1) | 1) != status_success) {
        g_spi_dev->cs_relese();
        return false;
    }
    response = sdcard_read_r1_status();
    if (response != 0x00) {
        g_spi_dev->cs_relese();
        SPI_SD_LOG("[spi_sdcard] Send CMD9 should return 0x00, response=0x%02x\r\n", response);
        return false;
    }
    sta = sdcard_wait_data_token(SPISD_START_TOKEN);
    if (sta != status_success) {
        g_spi_dev->cs_relese();
        return false;
    }
    for (i = 0; i < sizeof(temp); i++) {
        sta = g_spi_dev->write_read_byte(&dummy_write_byte, &temp[i]);
        if (sta != status_success) {
            g_spi_dev->cs_relese();
            return false;
        }
    }
    g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);
    g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);
    if (check_crc == true) {
        crc = crc7_calc(temp, sizeof(temp) - 1);
        if (crc == (temp[15] >> 1)) {
            return true;
        } else {
            return false;
        }
    }
    return true;
}

static hpm_stat_t send_sdcard_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{;
    uint8_t _cmd = cmd | 0x40;
    uint8_t packet[] = { arg >> 24, arg >> 16, arg >> 8, arg, crc};
    return g_spi_dev->write_cmd_data(_cmd, packet, sizeof(packet));
}

static hpm_stat_t read_sdcard_buffer(uint8_t *buf, uint32_t len)
{
    hpm_stat_t stat = status_success;
    HPM_CHECK_RET(g_spi_dev->read(buf, len));

    /* 2bytes dummy CRC */
    g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);
    g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);
    return stat;
}

static hpm_stat_t read_sdcard_info(spi_sdcard_info_t *cardinfo)
{
    uint8_t crc7 = 0;
    uint8_t temp[16];
    uint8_t response;
    hpm_stat_t sta;
    uint8_t i = 0;
    assert(g_spi_dev);
    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    if (send_sdcard_command((uint8_t)sdmmc_cmd_send_csd, 0, (0x7F << 1) | 1) != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    response = sdcard_read_r1_status();
    if (response != 0x00) {
        g_spi_dev->cs_relese();
        SPI_SD_LOG("[spi_sdcard] Send CMD9 should return 0x00, response=0x%02x\r\n", response);
        return status_timeout;
    }
    sta = sdcard_wait_data_token(SPISD_START_TOKEN);
    if (sta != status_success) {
        g_spi_dev->cs_relese();
        return sta;
    }
    for (i = 0; i < sizeof(temp); i++) {
        sta = g_spi_dev->write_read_byte(&dummy_write_byte, &temp[i]);
        if (sta != status_success) {
            g_spi_dev->cs_relese();
            return sta;
        }
    }
    g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);
    g_spi_dev->write_read_byte(&dummy_write_byte, &dummy_read_byte);

    crc7 = crc7_calc(temp, sizeof(temp) - 1);
    if (crc7 != (temp[15] >> 1)) {
        SPI_SD_LOG("[spi_sdcard] not support read CSD info, because CRC7 check error\n");
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
    if (cardinfo->csd.csd_structure == CSD_VERSION_V1_0) {
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
        cardinfo->csd.device_size_multiplier   |= (temp[10] & 0x80) >> 7;

        /* memory capacity= BLOCKNR * BLOCK_LEN
         * MULT = 2 ^ (c_size_mult + 2)
         * BLOCKNR = (device_size + 1) * MULT = (device_size + 1) * (2 ^ (c_size_mult + 2))
         * BLOCK_LEN = 2 ^ (read_block_len)
         */
        uint32_t c_size_mult = 1UL << (cardinfo->csd.device_size_multiplier + 2);
        cardinfo->block_count = (cardinfo->csd.device_size + 1U) * c_size_mult;
        cardinfo->block_size = (1UL << (cardinfo->csd.read_block_len));
        if (cardinfo->block_size != SPI_SD_BLOCK_SIZE) {
            cardinfo->block_count *= cardinfo->block_size;
            cardinfo->block_size = SPI_SD_BLOCK_SIZE;
            cardinfo->block_count /= cardinfo->block_size;
        }
        cardinfo->capacity = (uint64_t) cardinfo->block_size * cardinfo->block_count;
    } else if (cardinfo->csd.csd_structure == CSD_VERSION_V2_0) {
        cardinfo->block_size = SPI_SD_BLOCK_SIZE;
        /* Byte 7 */
        cardinfo->csd.device_size              |= (((temp[7]) & 0x3F) << 16);
        /* Byte 8 */
        cardinfo->csd.device_size              |= ((temp[8]) << 8);
        cardinfo->csd.device_size              |= (temp[9]);
        if (cardinfo->csd.device_size >= 0xFFFFU) {
            cardinfo->csd.support_sdxc = true;
            SPI_SD_LOG("[spi_sdcard] Unsupported SDXC: > 32G\n");
            return status_invalid_argument;
        }
        cardinfo->block_count = ((cardinfo->csd.device_size + 1U) * 1024U);
        cardinfo->capacity = (uint64_t) (cardinfo->csd.device_size + 1U) * 512UL * 1024UL;
    } else {
        SPI_SD_LOG("[spi_sdcard] Unsupported csd version V3.0\n");
        return status_invalid_argument;
    }

    /* Byte 10 */
    if ((uint8_t) ((temp[10] & 0x40) >> 6) != 0U) {
        cardinfo->csd.is_erase_block_enabled = true;
    }
    cardinfo->csd.erase_sector_size         = (temp[10] & 0x3F) << 1;

    /* Byte 11 */
    cardinfo->csd.erase_sector_size        |= (temp[11] & 0x80) >> 7;
    cardinfo->csd.write_protect_group_size  = (temp[11] & 0x7F);

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


    if (sdcard_wait_not_busy() != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    /* Send CMD10, Read CID */
    if (send_sdcard_command((uint8_t)sdmmc_cmd_send_cid, 0, (0x7F << 1) | 1) != status_success) {
        g_spi_dev->cs_relese();
        return status_fail;
    }
    response = sdcard_read_r1_status();
    if (response != 0x00) {
        g_spi_dev->cs_relese();
        SPI_SD_LOG("[spi_sdcard] Send CMD10 should return 0x00, response=0x%02x\r\n", response);
        return status_timeout;
    }
    if (sta != sdcard_wait_data_token(SPISD_START_TOKEN)) {
        g_spi_dev->cs_relese();
        return sta;
    }
    sta = g_spi_dev->read(temp, sizeof(temp));
    if (sta != status_success) {
        g_spi_dev->cs_relese();
        return sta;
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
