/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_qei_encoder.h"
#include "board.h"

#define PLB_BASE_NUM (1)

hpm_stat_t plb_qei_encoder_init(PLB_Type *plb, TRGM_Type *trgm, plb_qei_encoder_channel_t qei_chn, plb_qei_encoder_cfg_t *cfg)
{
    uint8_t chn;
    plb_type_a_truth_t plb_type_a_lut = {0};
    trgm_output_t trgm_output_cfg = {0};
    trgm_filter_t trgm_filter_cfg = {0};

    chn = qei_chn;
    if (cfg->mode == plb_abz_mode) {
        if (chn == plb_qei_encoder_channel_7) {
            return status_fail;
        }
    }

    trgm_filter_cfg.sync = true;
    trgm_filter_cfg.invert = false;
    trgm_filter_cfg.mode = trgm_filter_mode_rapid_change;
    trgm_filter_cfg.filter_length = cfg->filter_length;
    trgm_filter_config(trgm, HPM_TRGM0_FILTER_SRC_TRGM0_P00 + (cfg->trgm_input.qei_a - HPM_TRGM0_INPUT_SRC_TRGM0_P00), &trgm_filter_cfg);
    trgm_filter_config(trgm, HPM_TRGM0_FILTER_SRC_TRGM0_P00 + (cfg->trgm_input.qei_b - HPM_TRGM0_INPUT_SRC_TRGM0_P00), &trgm_filter_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_both_edge;
    trgm_output_cfg.input  = cfg->trgm_input.qei_a;
    trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_00 + chn * 4, &trgm_output_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_both_edge;
    trgm_output_cfg.input  = cfg->trgm_input.qei_b;
    trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_01 + chn * 4, &trgm_output_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = cfg->trgm_input.qei_a;
    trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_02 + chn * 4, &trgm_output_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_same_as_input;
    trgm_output_cfg.input  = cfg->trgm_input.qei_b;
    trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_03 + chn * 4, &trgm_output_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT00 + chn * 4;
    trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_32 + chn * 4, &trgm_output_cfg);

    trgm_output_cfg.invert = false;
    trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
    trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT01 + chn * 4;
    trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_33 + chn * 4, &trgm_output_cfg);

    /**
     * @brief pwm
     *
     */
    plb_type_a_lut.val = 0x2442;
    plb_type_a_set_lut(plb, chn, plb_type_a_table0, &plb_type_a_lut);
    /**
     * @brief pulse
     *
     */
    plb_type_a_lut.val = 0x4224;
    plb_type_a_set_lut(plb, chn, plb_type_a_table1, &plb_type_a_lut);

    plb_type_b_set_all_slice(plb, chn, plb_slice_opt_keep);
    plb_type_b_set_lut(plb, chn, plb_type_b_slice_1, plb_slice_opt_add_one);
    plb_type_b_set_lut(plb, chn, plb_type_b_slice_2, plb_slice_opt_sub_one);

    if (cfg->enable_phase) {
        plb_type_b_set_lut_range_mask(plb, chn, 0x00f0, plb_slice_opt_get_cmp1_val);
        plb_type_b_set_lut_range_mask(plb, chn, 0x0f00, plb_slice_opt_get_cmp0_val);

        plb_type_b_set_cmp_mode(plb, chn, plb_type_b_cmp2, plb_cmp_mode_eq);
        plb_type_b_set_cmp_mode(plb, chn, plb_type_b_cmp3, plb_cmp_mode_eq);
        plb_type_b_set_cmp_val(plb, chn, plb_type_b_cmp0, cfg->phase_cnt - 1);
        plb_type_b_set_cmp_val(plb, chn, plb_type_b_cmp1, 0);
        plb_type_b_set_cmp_val(plb, chn, plb_type_b_cmp2, cfg->phase_cnt);
        plb_type_b_set_cmp_val(plb, chn, plb_type_b_cmp3, -1);
        if (cfg->phase_cnt < 4) {
            return status_fail;
        }
    } else {
        if (cfg->mode == plb_abz_mode) {
            return status_fail;
        }
    }
    switch (cfg->mode) {
    case plb_ab_mode:
        trgm_output_cfg.invert = false;
        trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
        trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT34 + chn * 4;
        trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_34 + chn * 4, &trgm_output_cfg);

        trgm_output_cfg.invert = false;
        trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
        trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT35 + chn * 4;
        trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_35 + chn * 4, &trgm_output_cfg);
        break;
    case plb_abz_mode:
        if (cfg->enable_z_pin) {
            trgm_output_cfg.invert = false;
            trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
            trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT01 + (chn + 1) * 4;
            trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_32 + (chn + 1) * 4, &trgm_output_cfg);

            trgm_output_cfg.invert = false;
            trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
            trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT02 + (chn + 1) * 4;
            trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_33 + (chn + 1) * 4, &trgm_output_cfg);

            trgm_output_cfg.invert = false;
            trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
            trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT01 + (chn + 1) * 4;
            trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_34 + chn * 4, &trgm_output_cfg);

            trgm_output_cfg.invert = false;
            trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
            trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT02 + (chn + 1) * 4;
            trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_35 + chn * 4, &trgm_output_cfg);

        } else {
            trgm_output_cfg.invert = false;
            trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
            trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT34 + chn * 4;
            trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_32 + (chn + 1) * 4, &trgm_output_cfg);

            trgm_output_cfg.invert = false;
            trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
            trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT35 + chn * 4;
            trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_33 + (chn + 1) * 4, &trgm_output_cfg);

            trgm_output_cfg.invert = false;
            trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
            trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT34 + chn * 4;
            trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_34 + chn * 4, &trgm_output_cfg);

            trgm_output_cfg.invert = false;
            trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
            trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT35 + chn * 4;
            trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_35 + chn * 4, &trgm_output_cfg);
        }

        trgm_output_cfg.invert = false;
        trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
        trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT00 + chn * 4;
        trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_00 + (chn + 1) * 4, &trgm_output_cfg);

        trgm_output_cfg.invert = false;
        trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
        trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT01 + chn * 4;
        trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_01 + (chn + 1) * 4, &trgm_output_cfg);

        trgm_output_cfg.invert = false;
        trgm_output_cfg.type   = trgm_output_same_as_input;
        trgm_output_cfg.input  = HPM_TRGM0_INPUT_SRC_PLB_OUT00 + (chn + 1) * 4;
        trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_02 + (chn + 1) * 4, &trgm_output_cfg);

        trgm_output_cfg.invert = false;
        trgm_output_cfg.type   = trgm_output_pulse_at_input_rising_edge;
        trgm_output_cfg.input  = cfg->trgm_input.qei_z;
        trgm_output_config(trgm, TRGM_TRGOCFG_PLB_IN_03 + (chn + 1) * 4, &trgm_output_cfg);

        /**
         * @brief z phase
         *
         */
        plb_type_b_set_all_slice(plb, chn + 1, plb_slice_opt_keep);
        if (cfg->enable_z_pin) {
            plb_type_a_lut.val = 0xb2b2;
            plb_type_a_set_lut(plb, chn + 1, plb_type_a_table0, &plb_type_a_lut);
            plb_type_a_inject_by_sw(plb, chn + 1, 0);
            plb_type_a_lut.val = 0xf000;
            plb_type_a_set_lut(plb, chn + 1, plb_type_a_table1, &plb_type_a_lut);
            plb_type_a_inject_by_sw(plb, chn + 1, 0);
            plb_type_a_lut.val = 0x0f00;
            plb_type_a_set_lut(plb, chn + 1, plb_type_a_table2, &plb_type_a_lut);
            plb_type_a_inject_by_sw(plb, chn + 1, 0);

            plb_type_b_set_lut(plb, chn + 1, plb_type_b_slice_1, plb_slice_opt_add_one);
            plb_type_b_set_lut(plb, chn + 1, plb_type_b_slice_2, plb_slice_opt_sub_one);
        } else {
            plb_type_b_set_lut(plb, chn + 1, plb_type_b_slice_1, plb_slice_opt_add_one);
            plb_type_b_set_lut(plb, chn + 1, plb_type_b_slice_2, plb_slice_opt_sub_one);
        }

        break;

    default:
        return status_invalid_argument;
    }
    /**
     * @brief Wait for the circuit to stabilize
     *
     */
    board_delay_ms(1);
    /**
     * @brief Zeroing registers, because of the timing logic, requires multiple zeroes to completely erase the data
     *
     */
    plb_type_a_inject_by_sw(plb, chn, 0);
    plb_type_b_inject_by_sw(plb, chn, 0);
    plb_type_a_inject_by_sw(plb, chn, 0);
    plb_type_b_inject_by_sw(plb, chn, 0);
    if (cfg->mode == plb_abz_mode) {
        plb_type_a_inject_by_sw(plb, chn + 1, 0);
        plb_type_b_inject_by_sw(plb, chn + 1, 0);
        plb_type_a_inject_by_sw(plb, chn + 1, 0);
        plb_type_b_inject_by_sw(plb, chn + 1, 0);
    }

    return status_success;
}

uint32_t plb_qei_encoder_get_phase_count(PLB_Type *plb, plb_qei_encoder_channel_t qei_chn)
{
    return plb_type_b_get_counter(plb, (plb_chn_t)qei_chn);
}

void plb_qei_encoder_set_phase_count(PLB_Type *plb, plb_qei_encoder_channel_t qei_chn, uint32_t val)
{
    plb_type_b_inject_by_sw(plb, (plb_chn_t)qei_chn, val);
}

uint32_t plb_qei_encoder_get_laps_number(PLB_Type *plb, plb_qei_encoder_channel_t qei_chn)
{
    return plb_type_b_get_counter(plb, qei_chn + 1);
}

void plb_qei_encoder_set_laps_number(PLB_Type *plb, plb_qei_encoder_channel_t qei_chn, int32_t val)
{
    plb_type_b_inject_by_sw(plb, qei_chn + 1, val);
}

