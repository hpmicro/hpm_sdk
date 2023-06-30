/*
 * Copyright (c) 2022 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "pinmux.h"
#include "audio_v2_speaker_sync.h"


int main(void)
{
    board_init();
    board_init_usb_pins();

#if defined(USING_CODEC) && USING_CODEC
    board_init_i2c(CODEC_I2C);
    init_i2s_pins(TARGET_I2S);
#elif defined(USING_DAO) && USING_DAO
    init_dao_pins();
#else
    #error define USING_CODEC or USING_DAO
#endif

    intc_set_irq_priority(IRQn_USB0, 2);
    i2s_enable_dma_irq_with_priority(1);

    printf("cherry usb audio v2 speaker sample.\n");

    cherryusb_audio_init();
    speaker_init_i2s_dao_codec();

    while (1) {
        cherryusb_audio_main_task();
    }
    return 0;
}
