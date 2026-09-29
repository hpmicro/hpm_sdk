/*
 * Copyright (c) 2022-2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "pinmux.h"
#include "audio_v2_mic_adaptive.h"
#include "usb_config.h"


int main(void)
{
    board_init();

    printf("CherryUSB audio v2 mic sample.\n");

    board_init_usb((USB_Type *)CONFIG_HPM_USBD_BASE);

#if defined(USING_CODEC) && USING_CODEC
    board_init_i2c(CODEC_I2C);
    init_i2s_pins(TARGET_I2S);
    board_config_i2s_clock(TARGET_I2S, 48000);
#else
    board_init_pdm_clock();
    init_pdm_pins();
#endif

    init_mic_i2s_pdm_codec();

    i2s_enable_dma_irq_with_priority(2);
    intc_set_irq_priority(CONFIG_HPM_USBD_IRQn, 1);
    audio_v2_init(0, CONFIG_HPM_USBD_BASE);

    while (1) {
        audio_v2_task(0);
    }
}
