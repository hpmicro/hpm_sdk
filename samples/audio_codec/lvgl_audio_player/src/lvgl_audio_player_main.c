/*
 * Copyright (c) 2022-2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "board.h"
#include "lvgl.h"
#include "lv_adapter.h"
#include "lv_demo_music.h"
#include "audio_codec_common.h"
#include "sd_fatfs_common.h"
#include "hpm_gptmr_drv.h"

#define LV_TICK 5

static void init_gptmr0(uint32_t ms)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(HPM_GPTMR0, &config);

    clock_add_to_group(clock_gptmr0, 0);
    gptmr_freq = clock_get_frequency(clock_gptmr0);

    config.reload = gptmr_freq / 1000 * ms;
    gptmr_channel_config(HPM_GPTMR0, 0, &config, false);
    gptmr_enable_irq(HPM_GPTMR0, GPTMR_CH_RLD_IRQ_MASK(0));
    intc_m_enable_irq_with_priority(IRQn_GPTMR0, 3);

    gptmr_start_counter(HPM_GPTMR0, 0);
}

static void lv_tick_isr(void)
{
    if (gptmr_check_status(HPM_GPTMR0, GPTMR_CH_RLD_STAT_MASK(0))) {
        gptmr_clear_status(HPM_GPTMR0, GPTMR_CH_RLD_STAT_MASK(0));
        lv_tick_inc(LV_TICK);
    }
}
SDK_DECLARE_EXT_ISR_M(IRQn_GPTMR0, lv_tick_isr);


static void init_gptmr1(uint32_t ms)
{
    uint32_t gptmr_freq;
    gptmr_channel_config_t config;

    gptmr_channel_get_default_config(HPM_GPTMR1, &config);

    clock_add_to_group(clock_gptmr1, 0);
    gptmr_freq = clock_get_frequency(clock_gptmr1);

    config.reload = gptmr_freq / 1000 * ms;
    gptmr_channel_config(HPM_GPTMR1, 0, &config, false);
    gptmr_enable_irq(HPM_GPTMR1, GPTMR_CH_RLD_IRQ_MASK(0));
    intc_m_enable_irq_with_priority(IRQn_GPTMR1, 2);

    gptmr_start_counter(HPM_GPTMR1, 0);
}

static void lv_audio_codec_task_isr(void)
{
    if (gptmr_check_status(HPM_GPTMR1, GPTMR_CH_RLD_STAT_MASK(0))) {
        gptmr_clear_status(HPM_GPTMR1, GPTMR_CH_RLD_STAT_MASK(0));
        lv_audio_codec_task();
    }
}
SDK_DECLARE_EXT_ISR_M(IRQn_GPTMR1, lv_audio_codec_task_isr);


extern void _lv_demo_music_album_next(bool next);

int main(void)
{
    FRESULT ff_result;

    board_init();
    board_init_cap_touch();
    board_init_lcd();

#if defined(USING_CODEC) && USING_CODEC
    init_codec();
#elif defined(USING_DAO) && USING_DAO
    init_dao();
#endif

    lv_init();
    lv_adapter_init();

    printf("lvgl music\n");
    printf("%d color depth @%d\n", LV_COLOR_SIZE, clock_get_frequency(clock_display));

    init_wav_ctrl_func();
    ff_result = sd_mount_fs();
    if (ff_result != FR_OK) {
        lv_disp_no_sd_card_info();
    } else {
        ff_result = lv_generate_music_info_table();
        if (ff_result == FR_OK) {
            lv_demo_music();
        }
    }

    init_gptmr0(5U);
    init_gptmr1(25U);

    while (1) {
        lv_task_handler();
        if (has_switch_songs_req()) {
            _lv_demo_music_album_next(true);
            set_switch_songs_req(false);
        }
    }

    return 0;
}
