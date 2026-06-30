/*
 * Copyright (c) 2021,2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_pcfg_drv.h"
#include "hpm_soc_feature.h"

#define PCFG_CURRENT_MEASUREMENT_STEP (50U)
#define HPM_PMU_DRV_RETRY_COUNT (5000U)
#define PCFG_RC24M_FREQ (24000000UL)

hpm_stat_t pcfg_ldo1p1_set_voltage(PCFG_Type *ptr, uint16_t mv)
{
    if ((mv < PCFG_SOC_LDO1P1_MIN_VOLTAGE_IN_MV)
            || (mv > PCFG_SOC_LDO1P1_MAX_VOLTAGE_IN_MV)) {
        return status_pcfg_ldo_out_of_range;
    }
    ptr->LDO1P1 = (ptr->LDO1P1 & ~PCFG_LDO1P1_VOLT_MASK) | PCFG_LDO1P1_VOLT_SET(mv);
    return status_success;
}

hpm_stat_t pcfg_ldo2p5_set_voltage(PCFG_Type *ptr, uint16_t mv)
{
    uint32_t retry = 0;
    if ((mv < PCFG_SOC_LDO2P5_MIN_VOLTAGE_IN_MV)
            || (mv > PCFG_SOC_LDO2P5_MAX_VOLTAGE_IN_MV)) {
        return status_pcfg_ldo_out_of_range;
    }
    ptr->LDO2P5 &= ~PCFG_LDO2P5_ENABLE_MASK;
    ptr->LDO2P5 = PCFG_LDO2P5_ENABLE_MASK | PCFG_LDO2P5_VOLT_SET(mv);

    while (!PCFG_LDO2P5_READY_GET(ptr->LDO2P5)) {
        if (retry > HPM_PMU_DRV_RETRY_COUNT) {
            break;
        }
        retry++;
    }
    if (retry > HPM_PMU_DRV_RETRY_COUNT) {
        return status_timeout;
    }

    return status_success;
}

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

#if !defined(HPM_IP_FEATURE_PCFG_NO_DCDC) || (HPM_IP_FEATURE_PCFG_NO_DCDC == 0)
uint16_t pcfg_dcdc_get_current_level(PCFG_Type *ptr)
{
    uint32_t retry = 0;
    while (!pcfg_dcdc_is_measure_current_valid(ptr)) {
        if (retry > HPM_PMU_DRV_RETRY_COUNT) {
            break;
        }
        retry++;
    }
    if (retry > HPM_PMU_DRV_RETRY_COUNT) {
        return 0;
    }

    return PCFG_DCDC_CURRENT_LEVEL_GET(ptr->DCDC_CURRENT) * PCFG_CURRENT_MEASUREMENT_STEP;
}

hpm_stat_t pcfg_dcdc_set_voltage(PCFG_Type *ptr, uint16_t mv)
{
    bool dcm_mode = ((ptr->DCDC_ADVMODE & PCFG_DCDC_ADVMODE_EN_DCM_MASK) != 0) ? true : false;

    hpm_stat_t stat = status_success;
    if ((mv < PCFG_SOC_DCDC_MIN_VOLTAGE_IN_MV) || (mv > PCFG_SOC_DCDC_MAX_VOLTAGE_IN_MV)) {
        return status_invalid_argument;
    }

    if (dcm_mode) {
        pcfg_dcdc_set_voltage_dcm_mode(ptr, mv);
    } else {
        pcfg_dcdc_set_voltage_ccm_mode(ptr, mv);
    }

    return stat;
}

hpm_stat_t pcfg_dcdc_set_lpmode_voltage(PCFG_Type *ptr, uint16_t mv)
{
    hpm_stat_t stat = status_success;
    if ((mv < PCFG_SOC_DCDC_MIN_VOLTAGE_IN_MV) || (mv > PCFG_SOC_DCDC_MAX_VOLTAGE_IN_MV)) {
        return status_invalid_argument;
    }
    ptr->DCDC_LPMODE = (ptr->DCDC_LPMODE & ~PCFG_DCDC_LPMODE_STBY_VOLT_MASK) | PCFG_DCDC_LPMODE_STBY_VOLT_SET(mv);
    return stat;
}

void pcfg_dcdc_switch_to_dcm_mode(PCFG_Type *ptr)
{
    const uint8_t pcfc_dcdc_min_duty_cycle[] = {
        0x6E, 0x6E, 0x70, 0x70, 0x70, 0x70, 0x72, 0x72,
        0x72, 0x72, 0x74, 0x74, 0x74, 0x74, 0x76, 0x76,
        0x76, 0x78, 0x78, 0x78, 0x78, 0x7A, 0x7A, 0x7A,
        0x7A, 0x7C, 0x7C, 0x7C, 0x7E, 0x7E, 0x7E, 0x7E
    };
    uint16_t voltage;

    ptr->DCDC_MODE |= 0x77000u;
    ptr->DCDC_ADVMODE = (ptr->DCDC_ADVMODE & ~0x73F0067u) | 0x4120067u;
    ptr->DCDC_PROT &= ~PCFG_DCDC_PROT_SHORT_CURRENT_MASK;
    ptr->DCDC_PROT |= PCFG_DCDC_PROT_DISABLE_SHORT_MASK;
    ptr->DCDC_MISC = 0x100000u;
    voltage = PCFG_DCDC_MODE_VOLT_GET(ptr->DCDC_MODE);
    voltage = (voltage - 600) / 25;
    ptr->DCDC_ADVPARAM = (ptr->DCDC_ADVPARAM & ~PCFG_DCDC_ADVPARAM_MIN_DUT_MASK) | PCFG_DCDC_ADVPARAM_MIN_DUT_SET(pcfc_dcdc_min_duty_cycle[voltage]);
    while (!pcfg_dcdc_is_stable(ptr)) {
        NOP();
    }
}

void pcfg_dcdc_switch_to_ccm_mode(PCFG_Type *ptr)
{
    /* Attention: Need first switch to dcm */
    pcfg_dcdc_switch_to_dcm_mode(ptr);

    ptr->DCDC_MODE = (ptr->DCDC_MODE & ~0x77000u) | 0x11000u;
    ptr->DCDC_ADVPARAM = (ptr->DCDC_ADVPARAM & ~PCFG_DCDC_ADVPARAM_MIN_DUT_MASK) | PCFG_DCDC_ADVPARAM_MIN_DUT_SET(0x6A);
    while (!pcfg_dcdc_is_stable(ptr)) {
        NOP();
    }
}

void pcfg_dcdc_set_voltage_dcm_mode(PCFG_Type *ptr, uint16_t voltage)
{
    uint8_t mode = PCFG_DCDC_MODE_MODE_GET(ptr->DCDC_MODE);
    ptr->DCDC_MODE = (ptr->DCDC_MODE & ~(PCFG_DCDC_MODE_VOLT_MASK | 0xF000)) | PCFG_DCDC_MODE_VOLT_SET(voltage) | (mode << 12u);

    pcfg_dcdc_switch_to_dcm_mode(ptr);
}

void pcfg_dcdc_set_voltage_ccm_mode(PCFG_Type *ptr, uint16_t voltage)
{
    uint8_t mode = PCFG_DCDC_MODE_MODE_GET(ptr->DCDC_MODE);
    ptr->DCDC_MODE = (ptr->DCDC_MODE & ~(PCFG_DCDC_MODE_VOLT_MASK | 0xF000)) | PCFG_DCDC_MODE_VOLT_SET(voltage) | (mode << 12u);

    if ((ptr->DCDC_ADVMODE & PCFG_DCDC_ADVMODE_EN_DCM_MASK) != 0) {
        pcfg_dcdc_switch_to_ccm_mode(ptr);
    } else {
        ptr->DCDC_ADVPARAM = (ptr->DCDC_ADVPARAM & ~PCFG_DCDC_ADVPARAM_MIN_DUT_MASK) | PCFG_DCDC_ADVPARAM_MIN_DUT_SET(0x6A);
        while (!pcfg_dcdc_is_stable(ptr)) {
            NOP();
        }
    }
}

#endif
