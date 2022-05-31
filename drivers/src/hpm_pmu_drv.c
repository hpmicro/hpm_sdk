/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_pcfg_drv.h"
#include "hpm_soc_feature.h"

#define PCFG_CURRENT_MEASUREMENT_STEP (50U)

hpm_stat_t pcfg_ldo1p1_set_voltage(PCFG_Type *ptr, uint16_t mv)
{
    if ((mv < PCFG_SOC_LDO1P1_MIN_VOLTAGE_IN_MV)
            || (mv > PCFG_SOC_LDO1P1_MAX_VOLTAGE_IN_MV)) {
        return status_pcfg_ldo_out_of_range;
    }
    ptr->LDO1P1 &= ~PCFG_LDO1P1_ENABLE_MASK;
    ptr->LDO1P1 = PCFG_LDO1P1_ENABLE_MASK | PCFG_LDO1P1_VOLT_SET(mv); 
    return status_success;
}

hpm_stat_t pcfg_ldo2p5_set_voltage(PCFG_Type *ptr, uint16_t mv)
{
    if ((mv < PCFG_SOC_LDO2P5_MIN_VOLTAGE_IN_MV)
            || (mv > PCFG_SOC_LDO2P5_MAX_VOLTAGE_IN_MV)) {
        return status_pcfg_ldo_out_of_range;
    }
    ptr->LDO2P5 &= ~PCFG_LDO2P5_ENABLE_MASK;
    ptr->LDO2P5 = PCFG_LDO2P5_ENABLE_MASK | PCFG_LDO2P5_VOLT_SET(mv); 
    while (!(ptr->LDO2P5 & PCFG_LDO2P5_READY_MASK));

    return status_success;
}

uint16_t pcfg_dcdc_get_current_level(PCFG_Type *ptr)
{
    while(!pcfg_dcdc_is_measure_current_valid(ptr)){}
    return PCFG_DCDC_CURRENT_LEVEL_GET(ptr->DCDC_CURRENT) * PCFG_CURRENT_MEASUREMENT_STEP;
}

#define PCFG_RC24M_FREQ (24000000UL)
void pcfg_irc24m_config_track(PCFG_Type *ptr, pcfg_irc24m_config_t *config)
{
    uint32_t calculated_freq;
    uint16_t mul = 1;
    uint16_t div = 1;

    if (!(config->freq_in_hz < PCFG_RC24M_FREQ)) {
        /* calculate div */
        div = PCFG_RC24M_FREQ / config->freq_in_hz;
    } 
    calculated_freq = PCFG_RC24M_FREQ / div;
    while (calculated_freq < config->freq_in_hz) {
        calculated_freq *= (mul++);
    }
    ptr->TRACK_TARGET = PCFG_TRACK_TARGET_PRE_DIV_SET(div - 1)
        | PCFG_TRACK_TARGET_TARGET_SET(mul - 1);
    ptr->RC24M_TRACK = PCFG_RC24M_TRACK_SEL24M_SET(config->reference)
        | PCFG_RC24M_TRACK_RETURN_SET(config->return_to_default_on_xtal_loss)
        | PCFG_RC24M_TRACK_TRACK_SET(config->free_run);
}
