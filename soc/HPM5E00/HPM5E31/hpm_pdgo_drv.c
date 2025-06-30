/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_common.h"
#include "hpm_pdgo_drv.h"

void pdgo_config_wuio_filter(PDGO_Type *ptr, dgo_wuio_pin_num_t num, dgo_wuio_filter_cfg_t *cfg)
{
    uint32_t tmp;
    uint16_t filter_len = cfg->filter_length;

    if (filter_len > PDGO_IOFILTER_FILTER_LEN_MASK) {
        filter_len = PDGO_IOFILTER_FILTER_LEN_MASK;
    }
    tmp = PDGO_IOFILTER_EN_SET(cfg->enable)
        | PDGO_IOFILTER_IRQMODE_SET(cfg->irq_mode)
        | PDGO_IOFILTER_INV_SET(cfg->output_invert)
        | PDGO_IOFILTER_NORM_SET(cfg->output_init_value)
        | PDGO_IOFILTER_FILTER_LEN_SET(filter_len);
    if (cfg->filter_mode == dgo_filter_mode_bypass) {
        tmp |= PDGO_IOFILTER_BYPASS_MASK;
    } else {
        tmp |= PDGO_IOFILTER_MODE_SET(cfg->filter_mode);
    }

    ptr->IOFILTER[num] = tmp;
}

void pdgo_config_pcap(PDGO_Type *ptr, dgo_pcap_num_t num, dgo_pcap_cfg_t *cfg)
{
    uint32_t tmp;

    ptr->PCAP[num].CTRL &= ~PDGO_PCAP_CTRL_EN_MASK;

    tmp = PDGO_PCAP_CP_CFG_COMPARE_SET(cfg->cmp_value) | PDGO_PCAP_CP_CFG_PERIOD_SET(cfg->period_value);
    ptr->PCAP[num].CP_CFG = tmp;
    while (ptr->PCAP[num].CP_CFG != tmp) {
        ;
    }

    tmp = PDGO_PCAP_CTRL_COUNT_MODE_SET(cfg->cnt_dir | (cfg->cnt_mode << 1) | (cfg->cnt_event << 2))
        | PDGO_PCAP_CTRL_DUMP_MODE_SET(cfg->dump_mode)
        | PDGO_PCAP_CTRL_MATCH_MODE_SET(cfg->match_mode)
        | PDGO_PCAP_CTRL_SELECT_SET(cfg->wuio_select)
        | PDGO_PCAP_CTRL_EN_SET(cfg->enable);
    ptr->PCAP[num].CTRL = tmp;
}

void pdgo_get_pcnt_defconfig(PDGO_Type *ptr, dgo_pcnt_num_t num, dgo_pcnt_cfg_t *cfg)
{
    (void)ptr;
    (void)num;

    cfg->enable = false;
    cfg->dir_invert_en = false;
    cfg->phase_bias = 0;
    cfg->a_sel = dgo_wuio_pin_0;
    cfg->b_sel = dgo_wuio_pin_1;
    cfg->z_sel = dgo_wuio_pin_2;
    cfg->h_sel = dgo_wuio_pin_3;
    cfg->decode_mode = dgo_pcnt_ab_4x;
    cfg->resolution = 1000;
    cfg->z_mode = dgo_pcnt_z_disable;
    cfg->z_state.state = 1;
    cfg->h_mode = dgo_pcnt_h_disable;
    cfg->h_state.state = 0;
    cfg->dump_mode = dgo_pcnt_dump_disable;
    cfg->dump_state.state = 0;
    cfg->tacho_mode = dgo_pcnt_tacho_disable;
    cfg->tacho_len = 0;
    cfg->cmp0_mode = dgo_pcnt_cmp_disable;
    cfg->range_cmp_src = dgo_pcnt_cmp_src_line_phase;
    cfg->cmp0_value = 0;
    cfg->cmp1_value = 0;
}

void pdgo_config_pcnt(PDGO_Type *ptr, dgo_pcnt_num_t num, dgo_pcnt_cfg_t *cfg)
{
    assert(cfg->resolution > 0);

    uint32_t tmp;

    ptr->PCNT[num].CNT_CFG &= ~PDGO_PCNT_CNT_CFG_EN_MASK;

    if (cfg->resolution != 0xFFFFFFFF) {
        ptr->PCNT[num].RESOLUTION = (cfg->resolution - 1);
    } else {
        ptr->PCNT[num].RESOLUTION = 0xFFFFFFFF;
    }

    tmp = PDGO_PCNT_CALIB_CFG_ZMODE_SET(cfg->z_mode)
        | PDGO_PCNT_CALIB_CFG_ZSTATE_SET(cfg->z_state.state)
        | PDGO_PCNT_CALIB_CFG_H_SEL_SET(cfg->h_sel)
        | PDGO_PCNT_CALIB_CFG_Z_SEL_SET(cfg->z_sel)
        | PDGO_PCNT_CALIB_CFG_B_SEL_SET(cfg->b_sel)
        | PDGO_PCNT_CALIB_CFG_A_SEL_SET(cfg->a_sel);
    ptr->PCNT[num].CALIB_CFG = tmp;

    tmp = PDGO_PCNT_HOMING_CFG_HMODE_SET(cfg->h_mode)
        | PDGO_PCNT_HOMING_CFG_HSTATE_SET(cfg->h_state.state)
        | PDGO_PCNT_HOMING_CFG_DMODE_SET(cfg->dump_mode)
        | PDGO_PCNT_HOMING_CFG_DSTATE_SET(cfg->dump_state.state)
        | PDGO_PCNT_HOMING_CFG_CMP0_MODE_SET(cfg->cmp0_mode)
        | PDGO_PCNT_HOMING_CFG_RNG_CMP_SRC_SET(cfg->range_cmp_src);
    ptr->PCNT[num].HOMING_CFG = tmp;

    tmp = PDGO_PCNT_ANALYSE_CFG_TACHO_MODE_SET(cfg->tacho_mode)
        | PDGO_PCNT_ANALYSE_CFG_TACHO_LEN_SET(cfg->tacho_len);
    ptr->PCNT[num].ANALYSE_CFG = tmp;
    while (ptr->PCNT[num].ANALYSE_CFG != tmp) {
        ;
    }

    ptr->PCNT[num].CMP0 = cfg->cmp0_value;
    while (ptr->PCNT[num].CMP0 != cfg->cmp0_value) {
        ;
    }
    ptr->PCNT[num].CMP1 = cfg->cmp1_value;
    while (ptr->PCNT[num].CMP1 != cfg->cmp1_value) {
        ;
    }

    tmp = PDGO_PCNT_CNT_CFG_EN_SET(cfg->enable)
        | PDGO_PCNT_CNT_CFG_DECODE_MODE_SET(cfg->decode_mode)
        | PDGO_PCNT_CNT_CFG_BIAS_SET(cfg->phase_bias)
        | PDGO_PCNT_CNT_CFG_SEQ_SET(cfg->dir_invert_en);
    ptr->PCNT[num].CNT_CFG = tmp;
}

void pdgo_get_pcnt_counter(PDGO_Type *ptr, dgo_pcnt_num_t num, dgo_pcnt_counter_t *counter)
{
    uint32_t counter_val = pdgo_get_pcnt_raw_counter_value(ptr, num);
    dgo_pcnt_decode_mode_t mode = PDGO_PCNT_CNT_CFG_DECODE_MODE_GET(ptr->PCNT[num].CNT_CFG);
    uint32_t resolution = ptr->PCNT[num].RESOLUTION;
    uint32_t target_lines;
    uint8_t phase_bits;
    uint8_t line_bits;
    uint8_t rev_bits;

    switch (mode) {
    case dgo_pcnt_ab_1x:
    case dgo_pcnt_pd_1x:
    case dgo_pcnt_pd_f_1x:
    case dgo_pcnt_ud_1x:
    case dgo_pcnt_ud_f_1x:
        phase_bits = 0;
        break;

    case dgo_pcnt_ab_2x:
    case dgo_pcnt_pd_2x:
    case dgo_pcnt_pd_f_2x:
    case dgo_pcnt_ud_2x:
    case dgo_pcnt_ud_f_2x:
        phase_bits = 1;
        break;

    case dgo_pcnt_ab_4x:
        phase_bits = 2;
        break;

    case dgo_pcnt_ud_uvw_6x:
    case dgo_pcnt_ud_uvw_6y:
        phase_bits = 3;
        break;

    default:
        phase_bits = 0;
        break;
    }

    target_lines = resolution >> phase_bits;
    line_bits = get_first_set_bit_from_msb(target_lines) + 1u;
    rev_bits = 32 - line_bits - phase_bits;

    counter->phase = counter_val & ((1 << phase_bits) - 1);
    counter->line = (counter_val >> phase_bits) & ((1 << line_bits) - 1);
    counter->revolution = (counter_val >> (line_bits + phase_bits)) & ((1 << rev_bits) - 1);
}

uint32_t pdgo_convert_pcnt_counter(PDGO_Type *ptr, dgo_pcnt_num_t num, dgo_pcnt_counter_t *counter)
{
    uint32_t counter_val;
    dgo_pcnt_decode_mode_t mode = PDGO_PCNT_CNT_CFG_DECODE_MODE_GET(ptr->PCNT[num].CNT_CFG);
    uint32_t resolution = ptr->PCNT[num].RESOLUTION;
    uint32_t target_lines;
    uint8_t phase_bits;
    uint8_t line_bits;
    uint8_t rev_bits;

    switch (mode) {
    case dgo_pcnt_ab_1x:
    case dgo_pcnt_pd_1x:
    case dgo_pcnt_pd_f_1x:
    case dgo_pcnt_ud_1x:
    case dgo_pcnt_ud_f_1x:
        phase_bits = 0;
        break;

    case dgo_pcnt_ab_2x:
    case dgo_pcnt_pd_2x:
    case dgo_pcnt_pd_f_2x:
    case dgo_pcnt_ud_2x:
    case dgo_pcnt_ud_f_2x:
        phase_bits = 1;
        break;

    case dgo_pcnt_ab_4x:
        phase_bits = 2;
        break;

    case dgo_pcnt_ud_uvw_6x:
    case dgo_pcnt_ud_uvw_6y:
        phase_bits = 3;
        break;

    default:
        phase_bits = 0;
        break;
    }

    target_lines = resolution >> phase_bits;
    line_bits = get_first_set_bit_from_msb(target_lines) + 1u;
    rev_bits = 32 - line_bits - phase_bits;

    counter_val = (counter->phase & ((1 << phase_bits) - 1));
    counter_val |= (counter->line & ((1 << line_bits) - 1)) << phase_bits;    /* NOLINT */
    counter_val |= (counter->revolution & ((1 << rev_bits) - 1)) << (line_bits + phase_bits);    /* NOLINT */

    return counter_val;
}

void pdgo_config_pcnt_range_compare(PDGO_Type *ptr, dgo_pcnt_num_t num, uint32_t cmp0, uint32_t  cmp1)
{
    pdgo_set_pcnt_cmp_mode(ptr, num, dgo_pcnt_cmp_disable);
    pdgo_set_pcnt_cmp0_value(ptr, num, cmp0);
    pdgo_set_pcnt_cmp1_value(ptr, num, cmp1);
    pdgo_set_pcnt_cmp_mode(ptr, num, dgo_pcnt_cmp_range_match_mode);
}

float pdgo_calc_pcnt_tacho_cycles_per_line(PDGO_Type *ptr, dgo_pcnt_num_t num)
{
    dgo_pcnt_tacho_mode_t mode = PDGO_PCNT_ANALYSE_CFG_TACHO_MODE_GET(ptr->PCNT[num].ANALYSE_CFG);
    uint32_t tacho = ptr->PCNT[num].TACHO;
    uint32_t cycles = PDGO_PCNT_TACHO_TIME_GET(tacho);
    uint32_t lines = PDGO_PCNT_TACHO_LINE_GET(tacho);
    bool reverse = (lines & 0x8000u) ? true : false;
    float cycles_per_line;

    if (reverse) {
        lines = 0x10000 - lines;
    }
    if (!reverse || (mode == dgo_pcnt_tacho_period_mode)) {
        lines++;
    }

    cycles_per_line = ((float)cycles / (float)lines);
    if (reverse) {
        cycles_per_line = -cycles_per_line;
    }

    return cycles_per_line;
}

