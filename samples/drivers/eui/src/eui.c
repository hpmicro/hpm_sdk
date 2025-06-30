/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_eui_drv.h"
#include "hpm_interrupt.h"
#include "hpm_clock_drv.h"

static const uint8_t s_disp_code_8_seg[] = BOARD_EUI_SEG_ENCODE_DATA;
static uint32_t s_eui_clock_freq;
static volatile bool s_key_changed;
static volatile bool s_esc_key_pressed;
static volatile bool s_up_key_pressed;
static volatile bool s_enter_key_pressed;
static volatile bool s_left_key_pressed;
static volatile bool s_down_key_pressed;
static volatile bool s_right_key_pressed;

static void init_eui_config(void)
{
    eui_ctrl_config_t ctrl_config;

    eui_get_default_ctrl_config(BOARD_EUI, &ctrl_config);
    ctrl_config.work_mode = eui_work_mode_8x8;
    ctrl_config.clko_freq_khz = 100;
    ctrl_config.key_filter_ms = 50;
    ctrl_config.disp_data_invert = 0xFF;
    ctrl_config.scan_invert = 0x00;
    ctrl_config.dedicate_out_cfg = BOARD_EUI_DEDICATE_OUT_LINES;
    ctrl_config.dedicate_in_cfg = 0x0000;
    eui_config_ctrl(BOARD_EUI, s_eui_clock_freq, &ctrl_config);

    printf("clko_tm_us:%d, slot_tm_us:%d, hold_tm_us:%d, disp_tm_us:%d, filter_tm_us:%d\n\n",
            eui_get_time_us(BOARD_EUI, s_eui_clock_freq, eui_clko_time), eui_get_time_us(BOARD_EUI, s_eui_clock_freq, eui_slot_time),
            eui_get_time_us(BOARD_EUI, s_eui_clock_freq, eui_hold_time), eui_get_time_us(BOARD_EUI, s_eui_clock_freq, eui_disp_time),
            eui_get_time_us(BOARD_EUI, s_eui_clock_freq, eui_filter_time));

    eui_set_irq_enable(BOARD_EUI, eui_irq_area_mask);
    intc_m_enable_irq_with_priority(BOARD_EUI_IRQ, 1);

    eui_set_enable(BOARD_EUI, true);
}

static void led_disp_config(void)
{
    eui_scan_disp_data_t disp_data;
    eui_disp_config_t disp_config;

    disp_data.data_8x8[0] = s_disp_code_8_seg[0];
    disp_data.data_8x8[1] = s_disp_code_8_seg[1];
    disp_data.data_8x8[2] = s_disp_code_8_seg[2];
    disp_data.data_8x8[3] = s_disp_code_8_seg[3];
    disp_data.data_8x8[4] = s_disp_code_8_seg[4];
    eui_set_scan_disp_data(BOARD_EUI, eui_disp_data_idx_a, &disp_data);
    disp_data.data_8x8[0] = 0;
    disp_data.data_8x8[1] = 0;
    disp_data.data_8x8[2] = 0;
    disp_data.data_8x8[3] = 0;
    disp_data.data_8x8[4] = 0;
    eui_set_scan_disp_data(BOARD_EUI, eui_disp_data_idx_b, &disp_data);
    disp_config.b_data_format = eui_b_disp_b;
    disp_config.ab_loop_cnt = 9;      /* a and b loop 10 times  */
    disp_config.disp_a_time_ms = 500;
    disp_config.disp_b_time_ms = 500;
    eui_config_disp(BOARD_EUI, s_eui_clock_freq, eui_disp_ctrl_idx_ab, &disp_config);

    disp_data.data_8x8[0] = s_disp_code_8_seg[0];
    disp_data.data_8x8[1] = s_disp_code_8_seg[1];
    disp_data.data_8x8[2] = s_disp_code_8_seg[2];
    disp_data.data_8x8[3] = s_disp_code_8_seg[3];
    disp_data.data_8x8[4] = s_disp_code_8_seg[4];
    eui_set_scan_disp_data(BOARD_EUI, eui_disp_data_idx_c, &disp_data);
    disp_data.data_8x8[0] = 0xFF;
    disp_data.data_8x8[1] = 0;
    disp_data.data_8x8[2] = 0xFF;
    disp_data.data_8x8[3] = 0;
    disp_data.data_8x8[4] = 0xFF;
    eui_set_scan_disp_data(BOARD_EUI, eui_disp_data_idx_d, &disp_data);
    disp_config.b_data_format = eui_b_disp_a_and_b;
    disp_config.ab_loop_cnt = 9;
    disp_config.disp_a_time_ms = 500;
    disp_config.disp_b_time_ms = 500;
    eui_config_disp(BOARD_EUI, s_eui_clock_freq, eui_disp_ctrl_idx_cd, &disp_config);

    disp_data.data_8x8[0] = s_disp_code_8_seg[0];
    disp_data.data_8x8[1] = s_disp_code_8_seg[1];
    disp_data.data_8x8[2] = s_disp_code_8_seg[2];
    disp_data.data_8x8[3] = s_disp_code_8_seg[3];
    disp_data.data_8x8[4] = s_disp_code_8_seg[4];
    eui_set_scan_disp_data(BOARD_EUI, eui_disp_data_idx_e, &disp_data);
    disp_data.data_8x8[0] = 0;
    disp_data.data_8x8[1] = 0xFF;
    disp_data.data_8x8[2] = 0;
    disp_data.data_8x8[3] = 0xFF;
    disp_data.data_8x8[4] = 0;
    eui_set_scan_disp_data(BOARD_EUI, eui_disp_data_idx_f, &disp_data);
    disp_config.b_data_format = eui_b_disp_a_and_b;
    disp_config.ab_loop_cnt = 9;
    disp_config.disp_a_time_ms = 500;
    disp_config.disp_b_time_ms = 500;
    eui_config_disp(BOARD_EUI, s_eui_clock_freq, eui_disp_ctrl_idx_ef, &disp_config);
}

int main(void)
{
    board_init();
    init_eui_pins(BOARD_EUI);

    printf("eui example\n\n");

    clock_add_to_group(BOARD_EUI_CLOCK_NAME, 0);
    s_eui_clock_freq = clock_get_frequency(BOARD_EUI_CLOCK_NAME);
    init_eui_config();
    led_disp_config();

    while (1) {
        if (s_key_changed) {
            s_key_changed = false;
            if (s_esc_key_pressed || s_up_key_pressed || s_enter_key_pressed || s_left_key_pressed || s_down_key_pressed || s_right_key_pressed) {
                printf(" esc: %d, up: %d, enter: %d\n", s_esc_key_pressed, s_up_key_pressed, s_enter_key_pressed);
                printf("left: %d, dn: %d, right: %d\n\n", s_left_key_pressed, s_down_key_pressed, s_right_key_pressed);
            }
        }
    }

    return 0;
}

SDK_DECLARE_EXT_ISR_M(BOARD_EUI_IRQ, eui_isr)
void eui_isr(void)
{
    if ((eui_get_irq_status(BOARD_EUI) & eui_irq_area_mask) == eui_irq_area_mask) {
        eui_clear_irq_flag(BOARD_EUI, eui_irq_area_mask);
        s_esc_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_ESC_KEY_ROW, BOARD_EUI_ESC_KEY_COL);
        s_up_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_UP_KEY_ROW, BOARD_EUI_UP_KEY_COL);
        s_enter_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_ENTER_KEY_ROW, BOARD_EUI_ENTER_KEY_COL);
        s_left_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_LEFT_KEY_ROW, BOARD_EUI_LEFT_KEY_COL);
        s_down_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_DOWN_KEY_ROW, BOARD_EUI_DOWN_KEY_COL);
        s_right_key_pressed = eui_get_scan_key_by_row_col(BOARD_EUI, BOARD_EUI_RIGHT_KEY_ROW, BOARD_EUI_RIGHT_KEY_COL);
        s_key_changed = true;
    }
}
