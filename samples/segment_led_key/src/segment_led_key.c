/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_segment_led.h"
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

static uint8_t s_disp_mode;
static uint8_t s_disp_data[3];
static uint8_t s_data_sel;

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

static void seg_led_disp_config(void)
{
    segment_led_config_eui_instance(BOARD_EUI, s_eui_clock_freq);
    segment_led_config_blink_period(500, 500);
    segment_led_config_disp_blink(0, BOARD_EUI_SEG_DP_BIT_MASK);
    segment_led_config_disp_blink(1, BOARD_EUI_SEG_DP_BIT_MASK);
    segment_led_config_disp_blink(2, BOARD_EUI_SEG_DP_BIT_MASK);
    segment_led_config_disp_blink(3, BOARD_EUI_SEG_DP_BIT_MASK);
    segment_led_config_disp_blink(4, BOARD_EUI_SEG_DP_BIT_MASK);
    s_disp_mode = 1;
    s_data_sel = 0;
}

static void update_seg_led_blink(void)
{
    if (s_disp_mode == 1) {        /* Data No Blink */
        segment_led_config_disp_blink(2, BOARD_EUI_SEG_DP_BIT_MASK);
        segment_led_config_disp_blink(3, BOARD_EUI_SEG_DP_BIT_MASK);
        segment_led_config_disp_blink(4, BOARD_EUI_SEG_DP_BIT_MASK);
    } else if (s_disp_mode == 2) { /* Data0 Blink */
        segment_led_config_disp_blink(2, 0xFF);
        segment_led_config_disp_blink(3, BOARD_EUI_SEG_DP_BIT_MASK);
        segment_led_config_disp_blink(4, BOARD_EUI_SEG_DP_BIT_MASK);
    } else if (s_disp_mode == 3) { /* Data1 Blink */
        segment_led_config_disp_blink(2, BOARD_EUI_SEG_DP_BIT_MASK);
        segment_led_config_disp_blink(3, 0xFF);
        segment_led_config_disp_blink(4, BOARD_EUI_SEG_DP_BIT_MASK);
    } else if (s_disp_mode == 4) { /* Data2 Blink */
        segment_led_config_disp_blink(2, BOARD_EUI_SEG_DP_BIT_MASK);
        segment_led_config_disp_blink(3, BOARD_EUI_SEG_DP_BIT_MASK);
        segment_led_config_disp_blink(4, 0xFF);
    } else if (s_disp_mode == 5) { /* All Data Blink */
        segment_led_config_disp_blink(2, 0xFF);
        segment_led_config_disp_blink(3, 0xFF);
        segment_led_config_disp_blink(4, 0xFF);
    } else if (s_disp_mode == 6) { /* All Data Some Bits Blink */
        segment_led_config_disp_blink(2, 0x63 | BOARD_EUI_SEG_DP_BIT_MASK);
        segment_led_config_disp_blink(3, 0x5C | BOARD_EUI_SEG_DP_BIT_MASK);
        segment_led_config_disp_blink(4, 0x3F | BOARD_EUI_SEG_DP_BIT_MASK);
    } else {
        ;
    }
}

static void update_seg_led_disp_data(void)
{
    segment_led_set_disp_data(0, s_disp_code_8_seg[s_disp_mode] | ((s_data_sel == 0) ? BOARD_EUI_SEG_DP_BIT_MASK : 0));
    segment_led_set_disp_data(1, (s_data_sel == 1) ? BOARD_EUI_SEG_DP_BIT_MASK : 0);    /* OFF */
    segment_led_set_disp_data(2, s_disp_code_8_seg[s_disp_data[0]] | ((s_data_sel == 2) ? BOARD_EUI_SEG_DP_BIT_MASK : 0));
    segment_led_set_disp_data(3, s_disp_code_8_seg[s_disp_data[1]] | ((s_data_sel == 3) ? BOARD_EUI_SEG_DP_BIT_MASK : 0));
    segment_led_set_disp_data(4, s_disp_code_8_seg[s_disp_data[2]] | ((s_data_sel == 4) ? BOARD_EUI_SEG_DP_BIT_MASK : 0));
}

int main(void)
{
    board_init();
    init_eui_pins(BOARD_EUI);

    printf("segment led key example\n\n");

    clock_add_to_group(BOARD_EUI_CLOCK_NAME, 0);
    s_eui_clock_freq = clock_get_frequency(BOARD_EUI_CLOCK_NAME);
    init_eui_config();
    seg_led_disp_config();

    while (1) {
        if (s_key_changed) {
            s_key_changed = false;
            if (s_esc_key_pressed || s_up_key_pressed || s_enter_key_pressed || s_left_key_pressed || s_down_key_pressed || s_right_key_pressed) {
                if (s_right_key_pressed) {
                    if (s_data_sel < 4) {
                        s_data_sel++;
                    } else {
                        s_data_sel = 0;
                    }
                }

                if (s_left_key_pressed) {
                    if (s_data_sel > 0) {
                        s_data_sel--;
                    } else {
                        s_data_sel = 4;
                    }
                }

                if (s_up_key_pressed) {
                    if (s_data_sel == 0) {
                        if (s_disp_mode < 6) {
                            s_disp_mode++;
                        } else {
                            s_disp_mode = 1;
                        }
                    } else if ((s_data_sel >= 2) && (s_data_sel <= 4)) {
                        if (s_disp_data[s_data_sel - 2] < 0x0F) {
                            s_disp_data[s_data_sel - 2]++;
                        } else {
                            s_disp_data[s_data_sel - 2] = 0;
                        }
                    } else {
                        ;
                    }
                }

                if (s_down_key_pressed) {
                    if (s_data_sel == 0) {
                        if (s_disp_mode > 1) {
                            s_disp_mode--;
                        } else {
                            s_disp_mode = 6;
                        }
                    } else if ((s_data_sel >= 2) && (s_data_sel <= 4)) {
                        if (s_disp_data[s_data_sel - 2] > 0) {
                            s_disp_data[s_data_sel - 2]--;
                        } else {
                            s_disp_data[s_data_sel - 2] = 0x0F;
                        }
                    } else {
                        ;
                    }
                }

                update_seg_led_blink();

                printf(" esc: %d, up: %d, enter: %d\n", s_esc_key_pressed, s_up_key_pressed, s_enter_key_pressed);
                printf("left: %d, dn: %d, right: %d\n\n", s_left_key_pressed, s_down_key_pressed, s_right_key_pressed);
            }
        }

        update_seg_led_disp_data();
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
