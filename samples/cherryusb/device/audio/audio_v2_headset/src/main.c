/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "pinmux.h"
#include "audio_v2_headset.h"
#include "usb_config.h"

int main(void)
{
    board_init();

    printf("CherryUSB audio v2 headset sample.\n");

    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

    /* Initialize I2C for codec control */
    board_init_i2c(CODEC_I2C);

    /* Initialize I2S pins and clock for codec */
    init_i2s_pins(CODEC_I2S);
    board_config_i2s_clock(CODEC_I2S, 48000);

    /* Initialize codec and I2S */
    audio_v2_init_i2s_codec();

    intc_m_enable_irq_with_priority(BOARD_APP_DMA1_IRQ, 2);
    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 1);

    /* Initialize USB audio */
    audio_v2_init(0, CONFIG_HPM_USBD_BASE);

    while (1) {
        audio_v2_task(0);
    }
    return 0;
}
