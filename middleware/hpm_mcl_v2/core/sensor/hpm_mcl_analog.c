/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_mcl_analog.h"

hpm_mcl_stat_t hpm_mcl_analog_init(mcl_analog_t *analog, mcl_analog_cfg_t *cfg, mcl_cfg_t *mcl_cfg)
{
    uint8_t current_num = 0;

    analog->status = analog_status_init;
    MCL_ASSERT(analog != NULL, mcl_invalid_pointer);
    analog->status = analog_status_null;
    MCL_ASSERT(cfg != NULL, mcl_invalid_pointer);
    MCL_ASSERT(mcl_cfg != NULL, mcl_invalid_pointer);
    MCL_ASSERT(cfg->callback.get_value != NULL, mcl_invalid_pointer);
    MCL_ASSERT(cfg->callback.init != NULL, mcl_invalid_pointer);
    MCL_ASSERT(cfg->callback.update_sample_location != NULL, mcl_invalid_pointer);
    MCL_ASSERT(((cfg->enable_process_by_user == false) || (cfg->callback.process_by_user != NULL)), mcl_invalid_pointer);
    analog->cfg = cfg;
    analog->board_parameter = &mcl_cfg->physical.board.analog[0];
    analog->num_current_sample_res = &mcl_cfg->physical.board.num_current_sample_res;
    MCL_ASSERT(cfg->callback.init() == mcl_success, mcl_analog_init_error);
    if (analog->cfg->enable_a_current) {
        current_num++;
        MCL_ASSERT(cfg->callback.update_sample_location(analog_a_current, 0) == mcl_success, mcl_analog_update_location_error);
    }
    if (analog->cfg->enable_b_current) {
        current_num++;
        MCL_ASSERT(cfg->callback.update_sample_location(analog_b_current, 0) == mcl_success, mcl_analog_update_location_error);
    }
    if (analog->cfg->enable_c_current) {
        current_num++;
        MCL_ASSERT(cfg->callback.update_sample_location(analog_c_current, 0) == mcl_success, mcl_analog_update_location_error);
    }
    if (analog->cfg->enable_a_voltage) {
        MCL_ASSERT(cfg->callback.update_sample_location(analog_a_voltage, 0) == mcl_success, mcl_analog_update_location_error);
    }
    if (analog->cfg->enable_b_voltage) {
        MCL_ASSERT(cfg->callback.update_sample_location(analog_b_voltage, 0) == mcl_success, mcl_analog_update_location_error);
    }
    if (analog->cfg->enable_c_voltage) {
        MCL_ASSERT(cfg->callback.update_sample_location(analog_c_voltage, 0) == mcl_success, mcl_analog_update_location_error);
    }
    if (analog->cfg->enable_vbus) {
        MCL_ASSERT(cfg->callback.update_sample_location(analog_vbus, 0) == mcl_success, mcl_analog_update_location_error);
    }

    MCL_ASSERT(*analog->num_current_sample_res == current_num, mcl_invalid_argument);
    /**
     * @brief @todo only two res sample
     *
     */
    MCL_ASSERT(*analog->num_current_sample_res == 2, mcl_in_development);

    analog->status = analog_status_run;

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_analog_get_value(mcl_analog_t *analog, mcl_analog_chn_t chn, float *value)
{
    int32_t adc_val;
    float val0;

    MCL_ASSERT_OPT(analog != NULL, mcl_invalid_pointer);
    MCL_ASSERT_OPT(value != NULL, mcl_invalid_pointer);
    MCL_ASSERT(analog->status == analog_status_run, mcl_analog_not_ready);
    MCL_ASSERT_EXEC_CODE_AND_RETURN(analog->cfg->callback.get_value(chn, &adc_val) == mcl_success,
        analog->status = analog_status_fail, mcl_analog_get_value_error);
    if (analog->cfg->enable_process_by_user) {
        MCL_ASSERT(analog->cfg->callback.process_by_user(chn, adc_val, &val0) == mcl_success, mcl_fail);
    } else {
        val0 = adc_val;
        val0 = (val0 * analog->board_parameter[chn].adc_reference_vol / analog->board_parameter[chn].sample_precision) /
                analog->board_parameter[chn].opamp_gain / analog->board_parameter[chn].sample_res;
        if (analog->cfg->enable_filter[chn]) {
            val0 = hpm_mcl_filter_iir_df1(analog->iir[chn], val0);
        }
    }
    *value = val0;

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_analog_iir_filter_init(mcl_analog_t *analog, mcl_analog_chn_t chn, mcl_filter_iir_df1_t *iir)
{
    MCL_ASSERT(analog != NULL, mcl_invalid_pointer);
    MCL_ASSERT(iir != NULL, mcl_invalid_pointer);
    analog->iir[chn] = iir;
    analog->cfg->enable_filter[chn] = true;
    memset(iir->mem, 0, iir->cfg->section * sizeof(mcl_filter_iir_df1_memory_t));

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_analog_disable_iir_filter(mcl_analog_t *analog, mcl_analog_chn_t chn)
{
    MCL_ASSERT(analog != NULL, mcl_invalid_pointer);
    analog->cfg->enable_filter[chn] = false;
    memset(analog->iir[chn]->mem, 0, analog->iir[chn]->cfg->section * sizeof(mcl_filter_iir_df1_memory_t));

    return mcl_success;
}

hpm_mcl_stat_t hpm_mcl_analog_step_convert(mcl_analog_t *analog, float value, mcl_analog_chn_t chn, float angle, float *output)
{
    (void)analog;
    MCL_ASSERT_OPT(output != NULL, mcl_invalid_pointer);
    switch (chn) {
    case analog_a_current:
        if ((angle >= (0.75 * MCL_PI)) && (angle <= (1.75 * MCL_PI))) {
            *output = -value;
        } else {
            *output = value;
        }
        break;
    case analog_b_current:
        if ((angle <= (0.25 * MCL_PI)) || (angle >= (1.25 * MCL_PI))) {
            *output = -value;
        } else {
            *output = value;
        }
        break;
    default:
        return mcl_invalid_argument;
    }
    return mcl_success;
}
