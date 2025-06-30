/*
 * Copyright (c) 2024-2025 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <assert.h>
#include "hpm_eui_drv.h"

void eui_get_default_ctrl_config(EUI_Type *eui, eui_ctrl_config_t *config)
{
    (void)eui;

    config->work_mode = eui_work_mode_8x8;
    config->colum_loop_mode = false;
    config->clko_freq_khz = 100;
    config->hold_tm_cnt = 0;
    config->key_filter_ms = 50;
    config->clk_invert = false;
    config->sh_invert = false;
    config->di_invert = false;
    config->do_invert = false;
    config->disp_data_invert = 0x0000;
    config->scan_invert = 0x00;
    config->dedicate_out_cfg = eui_dedicate_output_0_line;
    config->dedicate_in_cfg = 0x0000;
}

void eui_config_ctrl(EUI_Type *eui, uint32_t eui_clk_freq, eui_ctrl_config_t *config)
{
    uint32_t freq_khz = eui_clk_freq / 1000u;
    uint32_t disp_time;
    uint16_t div;
    uint16_t filter_len;
    uint8_t hold_tm = config->hold_tm_cnt;

    assert(config->clko_freq_khz > 0);

    eui->CTRL = EUI_CTRL_DISP_MODE_SET(config->work_mode)
              | EUI_CTRL_COL_LOOP_MODE_SET(config->colum_loop_mode);

    div = (uint16_t)((freq_khz / ((uint32_t)config->clko_freq_khz * 2u)) - 1u);
    if (hold_tm > (EUI_DIV_CFG_HOLD_TIME_MASK >> EUI_DIV_CFG_HOLD_TIME_SHIFT)) {
        hold_tm = (EUI_DIV_CFG_HOLD_TIME_MASK >> EUI_DIV_CFG_HOLD_TIME_SHIFT);
    }
    eui->DIV_CFG = (eui->DIV_CFG & ~(EUI_DIV_CFG_CLK_DIV_MASK | EUI_DIV_CFG_HOLD_TIME_MASK))
                 | EUI_DIV_CFG_CLK_DIV_SET(div) | EUI_DIV_CFG_HOLD_TIME_SET(hold_tm);

    eui->POL_CFG = EUI_POL_CFG_DATA_POL_SET((config->disp_data_invert << 8) | config->scan_invert)
                 | EUI_POL_CFG_SH_POL_SET(config->sh_invert)
                 | EUI_POL_CFG_CK_POL_SET(config->clk_invert)
                 | EUI_POL_CFG_DO_POL_SET(config->do_invert)
                 | EUI_POL_CFG_DI_POL_SET(config->di_invert);
    eui->OUT_CFG = EUI_OUT_CFG_DO_CFG_SET(config->dedicate_out_cfg);
    eui->IN_CFG = EUI_IN_CFG_DI_CFG_SET(config->dedicate_in_cfg);

    disp_time = eui_get_time_us(eui, eui_clk_freq, eui_disp_time);
    filter_len = (uint16_t)(((uint32_t)config->key_filter_ms * 1000u) / disp_time + 1u);
    if (filter_len > (EUI_PAD_CFG_FILT_LEN_MASK >> EUI_PAD_CFG_FILT_LEN_SHIFT)) {
        filter_len = (EUI_PAD_CFG_FILT_LEN_MASK >> EUI_PAD_CFG_FILT_LEN_SHIFT);
    }
    eui->PAD_CFG = EUI_PAD_CFG_FILT_LEN_SET(filter_len);
}

void eui_config_disp(EUI_Type *eui, uint32_t eui_clk_freq, eui_disp_ctrl_idx_t idx, eui_disp_config_t *config)
{
    uint32_t disp_time_us;
    uint32_t time_a = 0, time_b = 0;

    disp_time_us = eui_get_time_us(eui, eui_clk_freq, eui_disp_time) * 16u;
    if (config->disp_a_time_ms > 0) {
        time_a = (config->disp_a_time_ms * 1000) / disp_time_us + 1u;
        if (time_a > (EUI_DAB_CTRL_PRDA_MASK >> EUI_DAB_CTRL_PRDA_SHIFT)) {
            time_a = (EUI_DAB_CTRL_PRDA_MASK >> EUI_DAB_CTRL_PRDA_SHIFT);
        }
    }
    if (config->disp_b_time_ms > 0) {
        time_b = (config->disp_b_time_ms * 1000) / disp_time_us + 1u;
        if (time_b > (EUI_DAB_CTRL_PRDB_MASK >> EUI_DAB_CTRL_PRDB_SHIFT)) {
            time_b = (EUI_DAB_CTRL_PRDB_MASK >> EUI_DAB_CTRL_PRDB_SHIFT);
        }
    }

    switch (idx) {
    case eui_disp_ctrl_idx_ab:
        eui->DAB_CTRL = EUI_DAB_CTRL_DB_CTRL_SET(config->b_data_format)
                      | EUI_DAB_CTRL_LOOPAB_SET(config->ab_loop_cnt)
                      | EUI_DAB_CTRL_PRDB_SET(time_b)
                      | EUI_DAB_CTRL_PRDA_SET(time_a);
        break;
    case eui_disp_ctrl_idx_cd:
        eui->DCD_CTRL = EUI_DCD_CTRL_DD_CTRL_SET(config->b_data_format)
                      | EUI_DCD_CTRL_LOOPCD_SET(config->ab_loop_cnt)
                      | EUI_DCD_CTRL_PRDD_SET(time_b)
                      | EUI_DCD_CTRL_PRDC_SET(time_a);
        break;
    case eui_disp_ctrl_idx_ef:
        eui->DEF_CTRL = EUI_DEF_CTRL_DF_CTRL_SET(config->b_data_format)
                      | EUI_DEF_CTRL_LOOPEF_SET(config->ab_loop_cnt)
                      | EUI_DEF_CTRL_PRDF_SET(time_b)
                      | EUI_DEF_CTRL_PRDE_SET(time_a);
        break;
    case eui_disp_ctrl_idx_gh:
        eui->DGH_CTRL = EUI_DGH_CTRL_DH_CTRL_SET(config->b_data_format)
                      | EUI_DGH_CTRL_LOOPGH_SET(config->ab_loop_cnt)
                      | EUI_DGH_CTRL_PRDH_SET(time_b)
                      | EUI_DGH_CTRL_PRDG_SET(time_a);
        break;
    default:
        break;
    }
}

void eui_set_scan_disp_data(EUI_Type *eui, eui_disp_data_idx_t idx, eui_scan_disp_data_t *data)
{
    switch (idx) {
    case eui_disp_data_idx_a:
        eui->DA_00 = data->data_u8[0] | (data->data_u8[1] << 8u) | (data->data_u8[2] << 16u) | (data->data_u8[3] << 24u);
        eui->DA_01 = data->data_u8[4] | (data->data_u8[5] << 8u) | (data->data_u8[6] << 16u) | (data->data_u8[7] << 24u);
        break;
    case eui_disp_data_idx_b:
        eui->DB_00 = data->data_u8[0] | (data->data_u8[1] << 8u) | (data->data_u8[2] << 16u) | (data->data_u8[3] << 24u);
        eui->DB_01 = data->data_u8[4] | (data->data_u8[5] << 8u) | (data->data_u8[6] << 16u) | (data->data_u8[7] << 24u);
        break;
    case eui_disp_data_idx_c:
        eui->DC_00 = data->data_u8[0] | (data->data_u8[1] << 8u) | (data->data_u8[2] << 16u) | (data->data_u8[3] << 24u);
        eui->DC_01 = data->data_u8[4] | (data->data_u8[5] << 8u) | (data->data_u8[6] << 16u) | (data->data_u8[7] << 24u);
        break;
    case eui_disp_data_idx_d:
        eui->DD_00 = data->data_u8[0] | (data->data_u8[1] << 8u) | (data->data_u8[2] << 16u) | (data->data_u8[3] << 24u);
        eui->DD_01 = data->data_u8[4] | (data->data_u8[5] << 8u) | (data->data_u8[6] << 16u) | (data->data_u8[7] << 24u);
        break;
    case eui_disp_data_idx_e:
        eui->DE_00 = data->data_u8[0] | (data->data_u8[1] << 8u) | (data->data_u8[2] << 16u) | (data->data_u8[3] << 24u);
        eui->DE_01 = data->data_u8[4] | (data->data_u8[5] << 8u) | (data->data_u8[6] << 16u) | (data->data_u8[7] << 24u);
        break;
    case eui_disp_data_idx_f:
        eui->DF_00 = data->data_u8[0] | (data->data_u8[1] << 8u) | (data->data_u8[2] << 16u) | (data->data_u8[3] << 24u);
        eui->DF_01 = data->data_u8[4] | (data->data_u8[5] << 8u) | (data->data_u8[6] << 16u) | (data->data_u8[7] << 24u);
        break;
    case eui_disp_data_idx_g:
        eui->DG_00 = data->data_u8[0] | (data->data_u8[1] << 8u) | (data->data_u8[2] << 16u) | (data->data_u8[3] << 24u);
        eui->DG_01 = data->data_u8[4] | (data->data_u8[5] << 8u) | (data->data_u8[6] << 16u) | (data->data_u8[7] << 24u);
        break;
    case eui_disp_data_idx_h:
        eui->DH_00 = data->data_u8[0] | (data->data_u8[1] << 8u) | (data->data_u8[2] << 16u) | (data->data_u8[3] << 24u);
        eui->DH_01 = data->data_u8[4] | (data->data_u8[5] << 8u) | (data->data_u8[6] << 16u) | (data->data_u8[7] << 24u);
        break;
    default:
        break;
    }
}

void eui_get_scan_disp_data(EUI_Type *eui, eui_disp_data_idx_t idx, uint32_t *data0, uint32_t *data1)
{
    switch (idx) {
    case eui_disp_data_idx_a:
        *data0 = eui->DA_00;
        *data1 = eui->DA_01;
        break;
    case eui_disp_data_idx_b:
        *data0 = eui->DB_00;
        *data1 = eui->DB_01;
        break;
    case eui_disp_data_idx_c:
        *data0 = eui->DC_00;
        *data1 = eui->DC_01;
        break;
    case eui_disp_data_idx_d:
        *data0 = eui->DD_00;
        *data1 = eui->DD_01;
        break;
    case eui_disp_data_idx_e:
        *data0 = eui->DE_00;
        *data1 = eui->DE_01;
        break;
    case eui_disp_data_idx_f:
        *data0 = eui->DF_00;
        *data1 = eui->DF_01;
        break;
    case eui_disp_data_idx_g:
        *data0 = eui->DG_00;
        *data1 = eui->DG_01;
        break;
    case eui_disp_data_idx_h:
        *data0 = eui->DH_00;
        *data1 = eui->DH_01;
        break;
    default:
        break;
    }
}

uint16_t eui_get_scan_key_by_row(EUI_Type *eui, uint8_t row)
{
    assert(row < 8);
    uint16_t ret = 0;

    switch (row) {
    case 0:
        ret = EUI_DATA_IN0_DATA_GET(eui->DATA_IN0);
        break;
    case 1:
        ret = EUI_DATA_IN1_DATA_GET(eui->DATA_IN1);
        break;
    case 2:
        ret = EUI_DATA_IN2_DATA_GET(eui->DATA_IN2);
        break;
    case 3:
        ret = EUI_DATA_IN3_DATA_GET(eui->DATA_IN3);
        break;
    case 4:
        ret = EUI_DATA_IN4_DATA_GET(eui->DATA_IN4);
        break;
    case 5:
        ret = EUI_DATA_IN5_DATA_GET(eui->DATA_IN5);
        break;
    case 6:
        ret = EUI_DATA_IN6_DATA_GET(eui->DATA_IN6);
        break;
    case 7:
        ret = EUI_DATA_IN7_DATA_GET(eui->DATA_IN7);
        break;
    default:
        break;
    }

    return ret;
}

bool eui_get_scan_key_by_row_col(EUI_Type *eui, uint8_t row, uint8_t col)
{
    assert(row < 8);
    assert(col < 16);

    uint16_t row_data = eui_get_scan_key_by_row(eui, row);

    return ((row_data & (1u << col)) != 0) ? true : false;
}

uint32_t eui_get_time_us(EUI_Type *eui, uint32_t eui_clk_freq, eui_time_id_t id)
{
    uint32_t ret = 0;
    uint32_t freq = eui_clk_freq;
    uint16_t clk_div = EUI_DIV_CFG_CLK_DIV_GET(eui->DIV_CFG);
    uint8_t hold_tm = EUI_DIV_CFG_HOLD_TIME_GET(eui->DIV_CFG);
    uint8_t do_cfg = EUI_OUT_CFG_DO_CFG_GET(eui->OUT_CFG);
    bool col_loop_mode = EUI_CTRL_COL_LOOP_MODE_GET(eui->CTRL);
    uint8_t work_mode = EUI_CTRL_DISP_MODE_GET(eui->CTRL);
    uint16_t filter_len = EUI_PAD_CFG_FILT_LEN_GET(eui->PAD_CFG);
    uint8_t dig_cnt;
    uint32_t clko_freq;
    uint32_t clko_time_us, slot_time_us, hold_time_us, disp_time_us, disp_filter_us;

    clko_freq = freq / ((clk_div + 1) * 2);
    clko_time_us = 1000000 / clko_freq;
    slot_time_us = clko_time_us * 32u;
    hold_time_us = slot_time_us * hold_tm;
    dig_cnt = 8 - do_cfg;
    if (work_mode == eui_work_mode_16x4) {
        if (dig_cnt > 4) {
            dig_cnt = 4;
        }
        if (col_loop_mode) {
            disp_time_us = (slot_time_us + hold_time_us) * dig_cnt * 16;
        } else {
            disp_time_us = (slot_time_us + hold_time_us) * dig_cnt;
        }
    } else {
        if (col_loop_mode) {
            disp_time_us = (slot_time_us + hold_time_us) * dig_cnt * 8;
        } else {
            disp_time_us = (slot_time_us + hold_time_us) * dig_cnt;
        }
    }
    disp_filter_us = disp_time_us * filter_len;

    switch (id) {
    case eui_clko_time:
        ret = clko_time_us;
        break;
    case eui_slot_time:
        ret = slot_time_us;
        break;
    case eui_hold_time:
        ret = hold_time_us;
        break;
    case eui_disp_time:
        ret = disp_time_us;
        break;
    case eui_filter_time:
        ret = disp_filter_us;
        break;
    default:
       break;
    }

    return ret;
}
