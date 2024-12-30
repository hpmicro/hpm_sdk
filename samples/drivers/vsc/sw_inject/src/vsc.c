/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_synt_drv.h"
#include "hpm_vsc_drv.h"

const int32_t value_a[] = {
#include "value_a.txt"
};

const int32_t value_b[] = {
#include "value_b.txt"
};

const int32_t value_c[] = {
#include "value_c.txt"
};

const uint32_t value_pos[] = {
#include "value_pos.txt"
};

const int32_t value_alpha_pos_gold[] = {
#include "value_alpha_pos.txt"
};

const int32_t value_beta_pos_gold[] = {
#include "value_beta_pos.txt"
};

const int32_t value_d_pos_gold[] = {
#include "value_d_pos.txt"
};

const int32_t value_q_pos_gold[] = {
#include "value_q_pos.txt"
};

const int32_t value_alpha_neg_gold[] = {
#include "value_alpha_neg.txt"
};

const int32_t value_beta_neg_gold[] = {
#include "value_beta_neg.txt"
};

const int32_t value_d_neg_gold[] = {
#include "value_d_neg.txt"
};

const int32_t value_q_neg_gold[] = {
#include "value_q_neg.txt"
};

static volatile bool vsc_convert_busy;
static volatile uint32_t vsc_data_idx;
static volatile uint32_t vsc_conver_cnt;
static volatile bool vsc_convert_err;

int main(void)
{
    vsc_config_t vsc_config;

    board_init();
    printf("VSC software inject sample\n");

    synt_enable_timestamp(HPM_SYNT, true);

    vsc_get_default_config(BOARD_VSC, &vsc_config);
    vsc_config.phase_mode = vsc_ab_phase;
    vsc_config.pole_pairs = 16;
    vsc_config_init(BOARD_VSC, &vsc_config);
    vsc_set_adc_tolerate(BOARD_VSC, 0xFF);
    vsc_set_enable(BOARD_VSC, true);

    /* Boundary Software Inject Check */
    vsc_clear_irq_status(BOARD_VSC, vsc_irq_convert_done);
    vsc_sw_inject_pos_value(BOARD_VSC, 0); /* theta:0 deg */
    vsc_sw_inject_abc_value(BOARD_VSC, 0xe1100000, 0x9f500000, 0);
    while (!vsc_get_irq_flag(BOARD_VSC, vsc_irq_convert_done)) {
        ;
    }
    if (vsc_get_beta_axis_value(BOARD_VSC, true) != (int32_t)0x80000000) {
        vsc_convert_err = true;
        printf("beta-axis postive value compare failure. cal_value: %#x, gold_value:%#x\n", vsc_get_beta_axis_value(BOARD_VSC, true), 0x80000000);
    }
    if (vsc_get_beta_axis_value(BOARD_VSC, false) != (int32_t)0x7FFFFFFF) {
        vsc_convert_err = true;
        printf("beta-axis postive value compare failure. cal_value: %#x, gold_value:%#x\n", vsc_get_beta_axis_value(BOARD_VSC, true), 0x7FFFFFFF);
    }
    if (vsc_get_q_axis_value(BOARD_VSC, true) != (int32_t)0x80000000) {
        vsc_convert_err = true;
        printf("q-axis postive value compare failure. cal_value: %#x, gold_value:%#x\n", vsc_get_q_axis_value(BOARD_VSC, true), 0x80000000);
    }
    if (vsc_get_q_axis_value(BOARD_VSC, false) != (int32_t)0x7FFFFFFF) {
        vsc_convert_err = true;
        printf("q-axis negtive value compare failure. cal_value: %#x, gold_value:%#x\n", vsc_get_q_axis_value(BOARD_VSC, false), 0x7FFFFFFF);
    }

    vsc_clear_irq_status(BOARD_VSC, vsc_irq_convert_done);
    vsc_sw_inject_pos_value(BOARD_VSC, 0x8000000); /* theta:180 deg */
    vsc_sw_inject_abc_value(BOARD_VSC, 0x8700000, 0x89a00000, 0);
    while (!vsc_get_irq_flag(BOARD_VSC, vsc_irq_convert_done)) {
        ;
    }
    if (vsc_get_beta_axis_value(BOARD_VSC, true) != (int32_t)0x80000000) {
        vsc_convert_err = true;
        printf("beta-axis postive value compare failure. cal_value: %#x, gold_value:%#x\n", vsc_get_beta_axis_value(BOARD_VSC, true), 0x80000000);
    }
    if (vsc_get_beta_axis_value(BOARD_VSC, false) != (int32_t)0x7FFFFFFF) {
        vsc_convert_err = true;
        printf("beta-axis postive value compare failure. cal_value: %#x, gold_value:%#x\n", vsc_get_beta_axis_value(BOARD_VSC, true), 0x7FFFFFFF);
    }
    if (vsc_get_q_axis_value(BOARD_VSC, true) != (int32_t)0x7FFFFFFF) {
        vsc_convert_err = true;
        printf("q-axis postive value compare failure. cal_value: %#x, gold_value:%#x\n", vsc_get_q_axis_value(BOARD_VSC, true), 0x7FFFFFFF);
    }
    if (vsc_get_q_axis_value(BOARD_VSC, false) != (int32_t)0x80000000) {
        vsc_convert_err = true;
        printf("q-axis negtive value compare failure. cal_value: %#x, gold_value:%#x\n", vsc_get_q_axis_value(BOARD_VSC, false), 0x80000000);
    }

    /* Normal Software Inject Check */
    vsc_clear_irq_status(BOARD_VSC, vsc_irq_convert_done | vsc_irq_abc_over_tolerate | vsc_irq_convert_conflict);
    vsc_set_irq_enable(BOARD_VSC, vsc_irq_convert_done | vsc_irq_abc_over_tolerate | vsc_irq_convert_conflict, true);
    intc_m_enable_irq_with_priority(BOARD_VSC_IRQn, 1);
    for (uint32_t i = 0; i < 1080; i++) {
        vsc_convert_busy = true;
        vsc_sw_inject_pos_value(BOARD_VSC, value_pos[i]);
        vsc_sw_inject_abc_value(BOARD_VSC, value_a[i], value_b[i], value_c[i]);
        vsc_data_idx = i;
        while (vsc_convert_busy) {
            ;
        }
    }
    printf("VSC software inject calculation finish! Convert Count: %d\n", vsc_conver_cnt);

    if (vsc_convert_err) {
        printf("VSC software inject calculation error occurred, failure!\n");
    } else {
        printf("VSC software inject calculation all matched, success!\n");
    }

    while (1) {
        ;
    }
}

SDK_DECLARE_EXT_ISR_M(BOARD_VSC_IRQn, isr_vsc)
void isr_vsc(void)
{
    uint32_t i;

    if (vsc_get_irq_flag(BOARD_VSC, vsc_irq_convert_done)) {
        vsc_clear_irq_status(BOARD_VSC, vsc_irq_convert_done);
        vsc_convert_busy = false;

        i = vsc_data_idx;
        vsc_conver_cnt++;

        if (abs(vsc_get_alpha_axis_value(BOARD_VSC, true) - value_alpha_pos_gold[i]) > 0xFFF) {
            vsc_convert_err = true;
            printf("alpha-axis postive value compare failure. index: %d, cal_value: %#x, gold_value:%#x\n", i, vsc_get_alpha_axis_value(BOARD_VSC, true),
                   value_alpha_pos_gold[i]);
        }

        if (abs(vsc_get_beta_axis_value(BOARD_VSC, true) - value_beta_pos_gold[i]) > 0xFFF) {
            vsc_convert_err = true;
            printf("beta-axis postive value compare failure. index: %d, cal_value: %#x, gold_value:%#x\n", i, vsc_get_beta_axis_value(BOARD_VSC, true),
                   value_beta_pos_gold[i]);
        }

        if (abs(vsc_get_d_axis_value(BOARD_VSC, true) - value_d_pos_gold[i]) > 0xFFF) {
            vsc_convert_err = true;
            printf("d-axis postive value compare failure. index: %d, cal_value: %#x, gold_value:%#x\n", i, vsc_get_d_axis_value(BOARD_VSC, true),
                   value_d_pos_gold[i]);
        }

        if (abs(vsc_get_q_axis_value(BOARD_VSC, true) - value_q_pos_gold[i]) > 0xFFF) {
            vsc_convert_err = true;
            printf("q-axis postive value compare failure. index: %d, cal_value: %#x, gold_value:%#x\n", i, vsc_get_q_axis_value(BOARD_VSC, true),
                   value_q_pos_gold[i]);
        }

        if (abs(vsc_get_alpha_axis_value(BOARD_VSC, false) - value_alpha_neg_gold[i]) > 0xFFF) {
            vsc_convert_err = true;
            printf("alpha-axis negtive value compare failure. index: %d, cal_value: %#x, gold_value:%#x\n", i, vsc_get_alpha_axis_value(BOARD_VSC, false),
                   value_alpha_neg_gold[i]);
        }

        if (abs(vsc_get_beta_axis_value(BOARD_VSC, false) - value_beta_neg_gold[i]) > 0xFFF) {
            vsc_convert_err = true;
            printf("beta-axis negtive value compare failure. index: %d, cal_value: %#x, gold_value:%#x\n", i, vsc_get_beta_axis_value(BOARD_VSC, false),
                   value_beta_neg_gold[i]);
        }

        if (abs(vsc_get_d_axis_value(BOARD_VSC, false) - value_d_neg_gold[i]) > 0xFFF) {
            vsc_convert_err = true;
            printf("d-axis negtive value compare failure. index: %d, cal_value: %#x, gold_value:%#x\n", i, vsc_get_d_axis_value(BOARD_VSC, false),
                   value_d_neg_gold[i]);
        }

        if (abs(vsc_get_q_axis_value(BOARD_VSC, false) - value_q_neg_gold[i]) > 0xFFF) {
            vsc_convert_err = true;
            printf("q-axis negtive value compare failure. index: %d, cal_value: %#x, gold_value:%#x\n", i, vsc_get_q_axis_value(BOARD_VSC, false),
                   value_q_neg_gold[i]);
        }
    }

    if (vsc_get_irq_flag(BOARD_VSC, vsc_irq_abc_over_tolerate)) {
        vsc_clear_irq_status(BOARD_VSC, vsc_irq_abc_over_tolerate);
        vsc_convert_err = true;
        printf("abc value over tolerate!\n");
    }

    if (vsc_get_irq_flag(BOARD_VSC, vsc_irq_convert_conflict)) {
        vsc_clear_irq_status(BOARD_VSC, vsc_irq_convert_conflict);
        vsc_convert_err = true;
        printf("vsc convert conflict!\n");
    }
}
