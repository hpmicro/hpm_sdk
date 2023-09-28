/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include <stdio.h>
#include "hpm_debug_console.h"
#include "hpm_sysctl_drv.h"
#include "hpm_clock_drv.h"
#include "hpm_i2s_drv.h"
#include "hpm_dao_drv.h"
#include "hpm_vad_drv.h"

/*
 * TODO: move these instance specific macros to board related config files
 */
#define TEST_VAD HPM_VAD
#define TEST_DAO HPM_DAO
#define I2S_MCLK_FREQ_IN_HZ (24576000UL)
#define DAO_I2S_CLK_NAME clock_dao

#define VOLUME_SCALE_SHIFT (2U)

uint32_t vad_in[50 << 10];

#define VAD_CALIBRATE_COUNT (8U)
#define VAD_MARGIN_LOW (0x1000U)
#define VAD_MARGIN_HIGH (0x4000U)
void test_vad_calibrate(vad_config_t *config)
{
    uint32_t actual = 0;
    for (uint32_t i = 0; i < VAD_CALIBRATE_COUNT; i++) {
        actual += vad_get_coef_value(TEST_VAD, VAD_COEF_STE_ACT);
    }

    actual = actual / VAD_CALIBRATE_COUNT * 20;

    if (actual > 0xFFFFU) {
        config->post_scale++;
        vad_reset(TEST_VAD);
        vad_set_post_scale(TEST_VAD, config->post_scale);
        vad_start(TEST_VAD);
        test_vad_calibrate(config);
    } else {
        vad_set_amplify(TEST_VAD, actual + VAD_MARGIN_HIGH, actual + VAD_MARGIN_LOW);
    }
}
void test_vad_to_dao(void)
{
    uint32_t i, j, data;
    bool recording;

    i2s_config_t i2s_config;
    i2s_transfer_config_t transfer;
    dao_config_t dao_config;
    vad_config_t vad_config;

    vad_get_default_config(TEST_VAD, &vad_config);
    vad_init(TEST_VAD, &vad_config);

    i2s_get_default_config(DAO_I2S, &i2s_config);
    i2s_init(DAO_I2S, &i2s_config);
    /*
     * config transfer for DAO
     */
    i2s_get_default_transfer_config_for_dao(&transfer);
    if (status_success != i2s_config_tx(DAO_I2S, I2S_MCLK_FREQ_IN_HZ, &transfer))
    {
        printf("I2S config failed for DAO\n");
        while(1);
    }

    dao_get_default_config(TEST_DAO, &dao_config);
    if (status_success != dao_init(TEST_DAO, &dao_config)) {
        printf("failed to initialize DAO\n");
        while(1);
    }

    vad_start(TEST_VAD);

    test_vad_calibrate(&vad_config);
    printf("vad calibration done\n");

    i2s_start(DAO_I2S);
    dao_start(TEST_DAO);

    recording = true;
    while(1) {
        i = 0;
        if (recording) {
            printf("Recording...\nPlease make some sound to mic0\n");

            test_vad_calibrate(&vad_config);
            vad_start(TEST_VAD);
            while(!(vad_get_status(TEST_VAD) & (VAD_EVENT_VAD))) {}
            vad_clear_status(TEST_VAD, VAD_EVENT_VAD);

            printf("Got something...\n");
            vad_enable_fifo(TEST_VAD);
            while(i < ARRAY_SIZE(vad_in)) {
                if (vad_get_status(TEST_VAD) & VAD_EVENT_FIFO_DATA_AVAILABLE){
                    vad_in[i] = vad_get_data(TEST_VAD);
                    i++;
                }
            }
            vad_disable_fifo(TEST_VAD);
            vad_stop(TEST_VAD);
            printf("Recorded %d bytes data\n", sizeof(vad_in));
        } else {
            i = 0;
            printf("Now, playing...\n");
            while(i < (ARRAY_SIZE(vad_in))) {
                if (i2s_get_tx_line_fifo_level(DAO_I2S, 0) <= 2){
                    data = *(vad_in + i);
                    data <<= VOLUME_SCALE_SHIFT;
                    for (j = 0; j < DAO_SOC_VAD_SAMPLE_RATE_RATIO; j++) {
                        i2s_send_data(DAO_I2S, 0, data); /* Left channel */
                        i2s_send_data(DAO_I2S, 0, data); /* Right channel */
                    }
                    i += 1;
                }
            }
        }
        if (!(i < ARRAY_SIZE(vad_in))) {
            recording = !recording;
        }
    }
}

int main(void)
{
    board_init();
    board_init_dao_clock();

    init_dao_pins();
    init_vad_pins();

    printf("vad example\n");

    test_vad_to_dao();

    while(1);
    return 0;
}
