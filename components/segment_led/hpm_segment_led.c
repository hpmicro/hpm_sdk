
/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/*---------------------------------------------------------------------
 * Include
 *---------------------------------------------------------------------
 */
#include "hpm_segment_led.h"

static segment_led_disp_config_t _seg_led_disp_config;

static void segment_led_update_blink_time(void)
{
    eui_disp_config_t eui_disp_config;

    eui_disp_config.b_data_format = eui_b_disp_a_and_b;
    eui_disp_config.ab_loop_cnt = 0;
    eui_disp_config.disp_a_time_ms = _seg_led_disp_config.blink_period_on_ms;
    eui_disp_config.disp_b_time_ms = _seg_led_disp_config.blink_period_off_ms;
    eui_config_disp(_seg_led_disp_config.eui_instance, _seg_led_disp_config.eui_clock_freq, eui_disp_ctrl_idx_ab, &eui_disp_config);
}

void segment_led_config_eui_instance(EUI_Type *eui, uint32_t eui_clock_freq)
{
    _seg_led_disp_config.eui_instance = eui;
    _seg_led_disp_config.eui_clock_freq = eui_clock_freq;

    segment_led_update_blink_time();
}

void segment_led_config_blink_period(uint32_t on_ms, uint32_t off_ms)
{
    _seg_led_disp_config.blink_period_on_ms = on_ms;
    _seg_led_disp_config.blink_period_off_ms = off_ms;

    segment_led_update_blink_time();
}

void segment_led_config_disp_blink(uint8_t index, uint16_t blink_bit_sel)
{
    _seg_led_disp_config.blink_bit_sel[index] = blink_bit_sel;
}

void segment_led_set_disp_data(uint8_t index, uint16_t data)
{
    eui_work_mode_t work_mode = eui_get_work_mode(_seg_led_disp_config.eui_instance);
    eui_scan_disp_data_t disp_a_data;
    eui_scan_disp_data_t disp_b_data;

    eui_get_scan_disp_data(_seg_led_disp_config.eui_instance, eui_disp_data_idx_a, &disp_a_data.data_u32[0], &disp_a_data.data_u32[1]);
    eui_get_scan_disp_data(_seg_led_disp_config.eui_instance, eui_disp_data_idx_b, &disp_b_data.data_u32[0], &disp_b_data.data_u32[1]);
    if (work_mode == eui_work_mode_8x8) {
        disp_a_data.data_8x8[index] = data;
        disp_b_data.data_8x8[index] = ~_seg_led_disp_config.blink_bit_sel[index];
    } else {
        disp_a_data.data_16x4[index] = data;
        disp_b_data.data_16x4[index] = ~_seg_led_disp_config.blink_bit_sel[index];
    }
    eui_set_scan_disp_data(_seg_led_disp_config.eui_instance, eui_disp_data_idx_a, &disp_a_data);
    eui_set_scan_disp_data(_seg_led_disp_config.eui_instance, eui_disp_data_idx_b, &disp_b_data);
}
