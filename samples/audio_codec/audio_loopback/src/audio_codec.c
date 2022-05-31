/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_i2s_drv.h"
#include "hpm_sgtl5000.h"

#define CODEC_I2C            BOARD_APP_I2C_BASE
#define CODEC_I2S            BOARD_APP_I2S_BASE
#define CODEC_I2S_CLK_NAME   BOARD_APP_I2S_CLK_NAME
#define CODEC_I2S_DATA_LINE  BOARD_APP_I2S_DATA_LINE

#define CODEC_I2C_ADDRESS    SGTL5000_I2C_ADDR
#define CODEC_I2S_MCLK_HZ    24576000UL
#define CODEC_SAMPLE_RATE_HZ 48000U
#define CODEC_BIT_WIDTH      32U

sgtl_config_t sgtl5000_config = {
    .route = sgtl_route_playback_record,  /*!< Audio data route.*/
    .bus = sgtl_bus_left_justified,       /*!< Audio transfer protocol */
    .master = false,                      /*!< Master or slave. True means master, false means slave. */
    .format = {.mclk_hz = CODEC_I2S_MCLK_HZ,
               .sample_rate = CODEC_SAMPLE_RATE_HZ,
               .bit_width = CODEC_BIT_WIDTH,
               .sclk_edge = sgtl_sclk_valid_edge_rising}, /*!< audio format */
};

sgtl_context_t sgtl5000_context = {
    .ptr = CODEC_I2C,
    .slave_address = CODEC_I2C_ADDRESS, /* I2C address */
};

void test_sgtl5000_playback_record(void)
{
    uint32_t data_rx, data_tx;
    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;

    /* Config I2S interface to CODEC */ 
    i2s_get_default_config(CODEC_I2S, &i2s_config);
    i2s_config.enable_mclk_out = true;
    i2s_init(CODEC_I2S, &i2s_config);

    i2s_get_default_transfer_config(&transfer);
    transfer.data_line = I2S_DATA_LINE_2;
    transfer.sample_rate = CODEC_SAMPLE_RATE_HZ;
    transfer.master_mode = true;
    /* configure I2S RX and TX */
    if (status_success != i2s_config_transfer(CODEC_I2S, CODEC_I2S_MCLK_HZ, &transfer))
    {
        printf("I2S config failed for CODEC\n");
        while(1);
    }

    sgtl5000_config.route = sgtl_route_playback_record;
    sgtl_init(&sgtl5000_context, &sgtl5000_config);

    printf("Test Codec playback and record\n");
    while(1) {
        /* record from codec and play by codec */
        while(i2s_get_rx_line_fifo_level(CODEC_I2S, CODEC_I2S_DATA_LINE) <= 1);
        i2s_receive_data(CODEC_I2S, CODEC_I2S_DATA_LINE, &data_rx, 1);

        data_tx = data_rx;
        while(i2s_get_tx_line_fifo_level(CODEC_I2S, CODEC_I2S_DATA_LINE) >= 1);
        if (i2s_get_tx_line_fifo_level(CODEC_I2S, CODEC_I2S_DATA_LINE) <= 2){
            i2s_send_data(CODEC_I2S, CODEC_I2S_DATA_LINE, &data_tx, 1); /* Left channel */
            i2s_send_data(CODEC_I2S, CODEC_I2S_DATA_LINE, &data_tx, 1); /* Right channel */
        }
    }
}

int main(void)
{
    board_init();
    board_init_i2c(CODEC_I2C);

    init_i2s_pins(CODEC_I2S);
    board_init_i2s_clock(CODEC_I2S);

    printf("audio codec example\n");
    test_sgtl5000_playback_record();

    while(1);
    return 0;
}
