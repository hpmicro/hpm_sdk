/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_dma_mgr.h"
#include "hpm_spi.h"
#include "board.h"

#define TEST_SPI                    BOARD_APP_SPI_BASE
#define TEST_SPI_DATA_LEN_BITS      (3U)

#ifndef SENT_ENABLE_PAUSE_NIBBLE
#define SENT_ENABLE_PAUSE_NIBBLE     (1U)
#endif

#ifndef SENT_IDLE_POLARITY
#define SENT_IDLE_POLARITY          (0U)
#endif

#if SENT_IDLE_POLARITY
#define SENT_PULSE_WIDTH_BITS       (0x00U)
#else
#define SENT_PULSE_WIDTH_BITS       (0xFFU)
#endif

#define SENT_MIN_PERIOD_TICKS         (12U)
#ifndef SENT_CLCOK_TICK_US
#define SENT_CLCOK_TICK_US          (3U)
#endif
#define SENT_CLOCK_BIT_US           ((float)SENT_CLCOK_TICK_US / (float)TEST_SPI_DATA_LEN_BITS)
#define SENT_DATA_NIBBLE_MAX_COUNT  (8U)

/* use J2716 recommends*/
static uint8_t crc4_calculate(uint8_t *data, uint8_t len);
static uint8_t sent_data_nibble_encode(uint8_t data, uint8_t *endcode_data);
static uint16_t sent_data_encode(uint8_t status, uint8_t *data, uint8_t len, uint8_t *endcode_data);
static void sent_data_send(uint8_t status, uint8_t *data, uint8_t len);

const uint8_t crc4_table_byte[16] = {
    0, 13, 7, 10, 14, 3, 9, 4, 1, 12, 6, 11, 15, 2, 8, 5
};

ATTR_PLACE_AT_NONCACHEABLE  uint8_t send_buffer[1024];

int main(void)
{
    char buffer[100];
    uint16_t buf_len = 0;
    uint16_t len = 0;
    uint8_t idle_dummy[10];
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    uint8_t status_nibble_value = 0x00;
    spi_initialize_config_t init_config;
    float frequency;
    board_init();
    board_init_spi_clock(TEST_SPI);
    printf("sent signal encode demo\n");
    /* Alternatively, only the MOSI pin needs to be initialized for pinmux */
    board_init_spi_pins(TEST_SPI);
    dma_mgr_init();
    hpm_spi_get_default_init_config(&init_config);
    init_config.mode = spi_master_mode;
    init_config.clk_phase = spi_sclk_sampling_odd_clk_edges;
    init_config.clk_polarity = spi_sclk_high_idle;
    init_config.data_len = TEST_SPI_DATA_LEN_BITS;
    /* step.1  initialize spi */
    if (hpm_spi_initialize(TEST_SPI, &init_config) != status_success) {
        printf("hpm_spi_initialize fail\n");
        while (1) {
        }
    }
    /* step.2  set spi sclk frequency for master */
    frequency = 1000000 / SENT_CLOCK_BIT_US;
    if (hpm_spi_set_sclk_frequency(TEST_SPI, (uint32_t)frequency) != status_success) {
        printf("hpm_spi_set_sclk_frequency fail\n");
        while (1) {
        }
    }

    /* step.3 install dma callback if want use dma */
    if (hpm_spi_dma_mgr_install_callback(TEST_SPI, NULL, NULL) != status_success) {
        printf("hpm_spi_dma_mgr_install_callback fail\n");
        while (1) {
        }
    }
    memset(idle_dummy, (uint8_t)~SENT_PULSE_WIDTH_BITS, sizeof(idle_dummy));
    hpm_spi_transmit_nonblocking(TEST_SPI, (uint8_t *)idle_dummy, sizeof(idle_dummy));
    while (spi_is_active(TEST_SPI) == true) {
    };
    while (1) {
        sent_data_send(status_nibble_value, (uint8_t *)data, sizeof(data));
        buf_len = 0;
        len = sprintf(&buffer[buf_len], "stat:%02x data:", status_nibble_value);
        buf_len += len;
        for (uint8_t k = 0; k < sizeof(data); k++) {
            len = sprintf(&buffer[buf_len], "%02x ", data[k]);
            buf_len += len;
        }
        sprintf(&buffer[buf_len], "crc:%02x ", crc4_calculate(data, sizeof(data)));
        printf("%s pause:%s\n\n", buffer, SENT_ENABLE_PAUSE_NIBBLE ? "true" : "false");
        board_delay_ms(2000);
    }
    return 0;
}

static uint8_t crc4_calculate(uint8_t *data, uint8_t len)
{
    uint8_t result  = 0x03;
    uint8_t tableNo = 0;
    int i = 0;
    if (data == NULL || len == 0) {
        return 0;
    }
    for (i = 0; i < len; i++) {
        tableNo = result ^ data[i];
        result  = crc4_table_byte[tableNo];
    }
    return result;
}

static uint8_t sent_data_nibble_encode(uint8_t data, uint8_t *endcode_data)
{
    uint8_t i = 0;
    uint8_t remain_ticks = 0;
    uint16_t index = 0;
    if (data > 0x0F) {
        return 0;
    }
    for (i = 0; i < 5; i++) {
        endcode_data[index++] = SENT_PULSE_WIDTH_BITS;
    }
    if (data > 0x0F) {
        return 0;
    }
    remain_ticks = (SENT_MIN_PERIOD_TICKS + data) - 5;
    for (i = 0; i < remain_ticks; i++) {
        endcode_data[index++] = (uint8_t)~SENT_PULSE_WIDTH_BITS;
    }
    return index;
}

static uint16_t sent_data_encode(uint8_t status, uint8_t *data, uint8_t len, uint8_t *endcode_data)
{
    uint8_t i = 0;
    uint16_t index = 0;
    uint8_t crc = 0;
    /*sync nibble 56 ticks (56 * SENT_CLCOK_TICK_US) */
    for (i = 0; i < 5; i++) {
        endcode_data[index++] = SENT_PULSE_WIDTH_BITS;
    }
    for (i = 0; i < 51; i++) {
        endcode_data[index++] = (uint8_t)~SENT_PULSE_WIDTH_BITS;
    }
    /* status nibble */
    index += sent_data_nibble_encode(status, &endcode_data[index]);
    /* data nibble */
    for (i = 0; i < len; i++) {
        index += sent_data_nibble_encode(data[i], &endcode_data[index]);
    }
    /* crc nibble*/
    crc = crc4_calculate(data, len);
    index += sent_data_nibble_encode(crc, &endcode_data[index]);
    /* pause nibble*/
    /* The fixed tick is set to the minimum value of a data nibble, which is 0, */
#if SENT_ENABLE_PAUSE_NIBBLE
    index += sent_data_nibble_encode(0x00, &endcode_data[index]);
#endif
    endcode_data[index++] = SENT_PULSE_WIDTH_BITS;
    /* idle */
    endcode_data[index++] = (uint8_t)~SENT_PULSE_WIDTH_BITS;
    return index;
}

static void sent_data_send(uint8_t status, uint8_t *data, uint8_t len)
{
    uint32_t length;
    if (len > SENT_DATA_NIBBLE_MAX_COUNT) {
        printf("data len is too long\n");
        return;
    }
    length = sent_data_encode(status, data, len, send_buffer);
    if (hpm_spi_transmit_nonblocking(TEST_SPI, (uint8_t *)send_buffer, length) != status_success) {
        printf("hpm_spi_transmit_nonblocking fail\n");
        return;
    }
    while (spi_is_active(TEST_SPI) == true) {
    };
}
