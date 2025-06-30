/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_soc_feature.h"
#include "hpm_filter.h"

static void plb_filter_rapid_change(filter_config_t *cfg)
{
    trgm_output_t trgm_config;
    plb_type_a_truth_t plb_type_a_lut;

    /* TYPE B */
    trgm_config.invert = false;
    trgm_config.type = trgm_output_pulse_at_input_both_edge;
    trgm_config.input = cfg->input_trgmux_pin;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4), &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEB_TRGM_OUTPUT0 + (cfg->plb_type_b_chn * 4);
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 1, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEB_TRGM_OUTPUT0 + (cfg->plb_type_b_chn * 4) + 1;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 2, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = HPM_TRGM0_INPUT_SRC_VSS;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 3, &trgm_config);

    /* true value table */
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_0, plb_slice_opt_keep);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_1, plb_slice_opt_add_one);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_2, plb_slice_opt_add_one);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_3, plb_slice_opt_add_one);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_4, plb_slice_opt_get_cmp0_val);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_5, plb_slice_opt_get_cmp0_val);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_6, plb_slice_opt_get_cmp0_val);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_7, plb_slice_opt_get_cmp0_val);

    plb_type_b_set_cmp_mode(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp0, plb_cmp_mode_gt);
    plb_type_b_set_cmp_mode(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp1, plb_cmp_mode_ge);
    plb_type_b_set_cmp_val(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp0, 0);
    plb_type_b_set_cmp_val(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp1, cfg->filter_len);
    plb_type_b_inject_by_sw(cfg->plb, cfg->plb_type_b_chn, 0);

    /* TYPE A */
    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = cfg->input_trgmux_pin;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEA_TRGM_INPUT0 + (cfg->plb_type_a_chn * 4), &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_pulse_at_input_both_edge;
    trgm_config.input = cfg->input_trgmux_pin;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEA_TRGM_INPUT0 + (cfg->plb_type_a_chn * 4) + 1, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEB_TRGM_OUTPUT0 + (cfg->plb_type_b_chn * 4);
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEA_TRGM_INPUT0 + (cfg->plb_type_a_chn * 4) + 2, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEA_TRGM_OUTPUT0 + (cfg->plb_type_a_chn * 4);
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEA_TRGM_INPUT0 + (cfg->plb_type_a_chn * 4) + 3, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEA_TRGM_OUTPUT0 + (cfg->plb_type_a_chn * 4);
    trgm_output_config(cfg->trgm, cfg->output_trgmux_pin, &trgm_config);

    plb_type_a_lut.val = 0b1111101000001010;
    plb_type_a_set_lut(cfg->plb, cfg->plb_type_a_chn, plb_type_a_table0, &plb_type_a_lut);
}

static void plb_filter_delay(filter_config_t *cfg)
{
    trgm_output_t trgm_config;

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = cfg->input_trgmux_pin;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4), &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEB_TRGM_OUTPUT0 + (cfg->plb_type_b_chn * 4);
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 1, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_pulse_at_input_both_edge;
    trgm_config.input = cfg->input_trgmux_pin;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 2, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = HPM_TRGM0_INPUT_SRC_VSS;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 3, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEB_TRGM_OUTPUT0 + (cfg->plb_type_b_chn * 4);
    trgm_output_config(cfg->trgm, cfg->output_trgmux_pin, &trgm_config);

    /* true value table */
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_0, plb_slice_opt_keep);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_1, plb_slice_opt_add_one);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_2, plb_slice_opt_sub_one);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_3, plb_slice_opt_keep);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_4, plb_slice_opt_add_one);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_5, plb_slice_opt_get_cmp1_val);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_6, plb_slice_opt_get_cmp2_val);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_7, plb_slice_opt_sub_one);

    plb_type_b_set_cmp_mode(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp0, plb_cmp_mode_ge);
    plb_type_b_set_cmp_val(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp0, cfg->filter_len);
    plb_type_b_set_cmp_val(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp1, 0);
    plb_type_b_set_cmp_val(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp2, 2 * cfg->filter_len);
    plb_type_b_inject_by_sw(cfg->plb, cfg->plb_type_b_chn, 0);
}

static void plb_filter_stable_low(filter_config_t *cfg)
{
    trgm_output_t trgm_config;

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = cfg->input_trgmux_pin;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4), &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEB_TRGM_OUTPUT0 + (cfg->plb_type_b_chn * 4);
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 1, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = HPM_TRGM0_INPUT_SRC_VSS;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 2, &trgm_config);
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 3, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEB_TRGM_OUTPUT0 + (cfg->plb_type_b_chn * 4);
    trgm_output_config(cfg->trgm, cfg->output_trgmux_pin, &trgm_config);

    /* true value table */
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_0, plb_slice_opt_keep);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_1, plb_slice_opt_get_cmp1_val);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_2, plb_slice_opt_add_one);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_3, plb_slice_opt_get_cmp1_val);

    plb_type_b_set_cmp_mode(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp0, plb_cmp_mode_lt);
    plb_type_b_set_cmp_val(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp0, cfg->filter_len);
    plb_type_b_set_cmp_val(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp1, 0);
    plb_type_b_inject_by_sw(cfg->plb, cfg->plb_type_b_chn, cfg->filter_len);
}

static void plb_filter_stable_high(filter_config_t *cfg)
{
    trgm_output_t trgm_config;

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = cfg->input_trgmux_pin;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4), &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEB_TRGM_OUTPUT0 + (cfg->plb_type_b_chn * 4);
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 1, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = HPM_TRGM0_INPUT_SRC_VSS;
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 2, &trgm_config);
    trgm_output_config(cfg->trgm, PLB_SOC_TYPEB_TRGM_INPUT0 + (cfg->plb_type_b_chn * 4) + 3, &trgm_config);

    trgm_config.invert = false;
    trgm_config.type = trgm_output_same_as_input;
    trgm_config.input = PLB_SOC_TYPEB_TRGM_OUTPUT0 + (cfg->plb_type_b_chn * 4);
    trgm_output_config(cfg->trgm, cfg->output_trgmux_pin, &trgm_config);

    /* true value table */
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_0, plb_slice_opt_get_cmp1_val);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_1, plb_slice_opt_add_one);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_2, plb_slice_opt_get_cmp1_val);
    plb_type_b_set_lut(cfg->plb, cfg->plb_type_b_chn, plb_type_b_slice_3, plb_slice_opt_keep);

    plb_type_b_set_cmp_mode(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp0, plb_cmp_mode_ge);
    plb_type_b_set_cmp_val(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp0, cfg->filter_len);
    plb_type_b_set_cmp_val(cfg->plb, cfg->plb_type_b_chn, plb_type_b_cmp1, 0);
    plb_type_b_inject_by_sw(cfg->plb, cfg->plb_type_b_chn, 0);
}

void plb_filter_init(filter_config_t *cfg)
{
    switch (cfg->mode) {
    case filter_mode_delay:
        plb_filter_delay(cfg);
        break;

    case filter_mode_stable_low:
        plb_filter_stable_low(cfg);
        break;

    case filter_mode_stable_high:
        plb_filter_stable_high(cfg);
        break;

    case filter_mode_rapid_change:
    default:
        plb_filter_rapid_change(cfg);
        break;
    }
}
