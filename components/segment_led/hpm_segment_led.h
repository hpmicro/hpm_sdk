/*
 * Copyright (c) 2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_SEGMENT_LED_H
#define _HPM_SEGMENT_LED_H

/*---------------------------------------------------------------------
 * Includes
 *---------------------------------------------------------------------
 */
#include "hpm_eui_drv.h"

/**
 * @brief segment led disp config structure
 *
 */
typedef struct {
    EUI_Type *eui_instance;
    uint32_t eui_clock_freq;
    uint32_t blink_period_on_ms;
    uint32_t blink_period_off_ms;
    uint16_t blink_bit_sel[8];    /* Max 8 segment leds, each led max has 16 segments. */
                                  /* When blink_bit_sel is 0, no segment will be blink; When blink_bit_sel is 1, segment num 0 will be blink;
                                   * ... ; When blink_bit_sel is 0xFF, segment num 0~7 will be blink.
                                   */
} segment_led_disp_config_t;


#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief config segment led eui instance and clock frequency.
 *
 * @param[in] eui EUI base address
 * @param[in] eui_clock_freq eui clock frequency
 */
void segment_led_config_eui_instance(EUI_Type *eui, uint32_t eui_clock_freq);

/**
 * @brief config segment led blink on and off time.
 *
 * @param[in] on_ms on time in ms
 * @param[in] off_ms off time in ms
 */
void segment_led_config_blink_period(uint32_t on_ms, uint32_t off_ms);

/**
 * @brief config segment led blink bit selection.
 *
 * @param[in] index index of segment leds
 * @param[in] blink_bit_sel blink bit selection
 */
void segment_led_config_disp_blink(uint8_t index, uint16_t blink_bit_sel);

/**
 * @brief config segment led display data.
 *
 * @param[in] index index of segment leds
 * @param[in] data data to display
 */
void segment_led_set_disp_data(uint8_t index, uint16_t data);


#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_SEGMENT_LED_H */