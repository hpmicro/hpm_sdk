/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_lobs_drv.h"
#include "hpm_interrupt.h"

#ifndef LOBS_USE_TWO_GROUP
#define LOBS_USE_TWO_GROUP 0
#endif

#define TRACE_CNT 1024
#define TRIG_POINT 64

ATTR_PLACE_AT(".ahb_sram") lobs_trace_data_t lobs_buffer[TRACE_CNT];

#if defined(LOBS_USE_TWO_GROUP) && LOBS_USE_TWO_GROUP
static void init_lobs_two_group_config(void)
{
    lobs_ctrl_config_t ctrl_config;
    lobs_two_group_mode_config_t group_config;
    lobs_state_config_t state_config = { 0 };

    lobs_unlock(HPM_LOBS);

    lobs_deinit(HPM_LOBS);

    ctrl_config.group_mode = lobs_two_group_8_bits;
    ctrl_config.sample_rate = lobs_sample_1_per_7;
    ctrl_config.start_addr = (uint32_t)&lobs_buffer[0];
    ctrl_config.end_addr = (uint32_t)&lobs_buffer[0] + sizeof(lobs_buffer);
    lobs_ctrl_config(HPM_LOBS, &ctrl_config);

    group_config.group_enable = true;
    group_config.sig_group_num = lobs_signal_group_PA;
    group_config.sample_sig_bit[0] = LOBS_PIN_DI(0);    /* PA00 DI */
    group_config.sample_sig_bit[1] = LOBS_PIN_DO(2);    /* PA02 DO */
    group_config.sample_sig_bit[2] = LOBS_PIN_OE(5);    /* PA05 OE */
    group_config.sample_sig_bit[3] = LOBS_PIN_DI(8);    /* PA08 DI */
    group_config.sample_sig_en[0] = true;
    group_config.sample_sig_en[1] = true;
    group_config.sample_sig_en[2] = true;
    group_config.sample_sig_en[3] = true;
    lobs_two_group_mode_config(HPM_LOBS, lobs_two_group_1, &group_config);

    group_config.group_enable = true;
    group_config.sig_group_num = lobs_signal_group_PB;
    group_config.sample_sig_bit[0] = LOBS_PIN_DI(0);    /* PB00 DI */
    group_config.sample_sig_bit[1] = LOBS_PIN_DO(2);    /* PB02 DO */
    group_config.sample_sig_bit[2] = LOBS_PIN_OE(5);    /* PB05 OE */
    group_config.sample_sig_bit[3] = LOBS_PIN_DI(8);    /* PB08 DI */
    group_config.sample_sig_en[0] = true;
    group_config.sample_sig_en[1] = true;
    group_config.sample_sig_en[2] = true;
    group_config.sample_sig_en[3] = true;
    lobs_two_group_mode_config(HPM_LOBS, lobs_two_group_2, &group_config);

    state_config.sig_group_num = BOARD_LOBS_TRIG_GROUP;
    state_config.cmp_mode = lobs_sig_cmp_mode;
    state_config.state_chg_condition = lobs_sig_equal_golden;
    state_config.next_state = lobs_next_state_1;
    state_config.cmp_sig_bit[0] = LOBS_PIN_DI(BOARD_LOBS_TRIG_PIN_0);    /* Trig Pin */
    state_config.cmp_sig_bit[1] = LOBS_PIN_DI(BOARD_LOBS_TRIG_PIN_1);    /* Trig Pin */
    state_config.cmp_sig_en[0] = true;
    state_config.cmp_sig_en[1] = true;
    state_config.cmp_sig_en[2] = false;
    state_config.cmp_sig_en[3] = false;
    state_config.cmp_golden_value[0] = true;
    state_config.cmp_golden_value[1] = false;
#if defined(HPM_IP_FEATURE_LOBS_COMP_LOGIC) && (HPM_IP_FEATURE_LOBS_COMP_LOGIC)
    state_config.cmp_logic = lobs_cmp_logic_and;
#endif
    lobs_state_config(HPM_LOBS, lobs_state_0, &state_config);

    state_config.cmp_mode = lobs_cnt_cmp_mode;
    state_config.state_chg_condition = lobs_cnt_matched;
    state_config.next_state = lobs_next_state_finish;
    state_config.cmp_counter = (TRACE_CNT - TRIG_POINT);
    state_config.cmp_sig_en[0] = false;
    state_config.cmp_sig_en[1] = false;
    state_config.cmp_sig_en[2] = false;
    state_config.cmp_sig_en[3] = false;
    lobs_state_config(HPM_LOBS, lobs_state_1, &state_config);

    lobs_set_pre_trig_enable(HPM_LOBS, true);
    lobs_set_state_enable(HPM_LOBS, lobs_state_0, true);
    lobs_set_state_enable(HPM_LOBS, lobs_state_1, true);

#if defined(HPM_IP_FEATURE_LOBS_IRQ_CTRL) && (HPM_IP_FEATURE_LOBS_IRQ_CTRL)
    lobs_set_irq_enable(HPM_LOBS, lobs_irq_final_mask | lobs_irq_full_mask);
#endif
    intc_m_enable_irq_with_priority(IRQn_LOBS, 1);

    lobs_set_enable(HPM_LOBS, true);

    lobs_lock(HPM_LOBS);
}
#else
static void init_lobs_one_group_config(void)
{
    lobs_ctrl_config_t ctrl_config;
    lobs_state_config_t state_config = { 0 };

    lobs_unlock(HPM_LOBS);

    lobs_deinit(HPM_LOBS);

    ctrl_config.group_mode = lobs_one_group_96_bits;
    ctrl_config.sample_rate = lobs_sample_1_per_7;
    ctrl_config.start_addr = (uint32_t)&lobs_buffer[0];
    ctrl_config.end_addr = (uint32_t)&lobs_buffer[0] + sizeof(lobs_buffer);
    lobs_ctrl_config(HPM_LOBS, &ctrl_config);

    state_config.sig_group_num = BOARD_LOBS_TRIG_GROUP;
    state_config.cmp_mode = lobs_sig_cmp_mode;
    state_config.state_chg_condition = lobs_sig_equal_golden;
    state_config.next_state = lobs_next_state_1;
    state_config.cmp_sig_bit[0] = LOBS_PIN_DI(BOARD_LOBS_TRIG_PIN_0);      /* Trig Pin */
    state_config.cmp_sig_bit[1] = LOBS_PIN_DI(BOARD_LOBS_TRIG_PIN_1);      /* Trig Pin */
    state_config.cmp_sig_en[0] = true;
    state_config.cmp_sig_en[1] = true;
    state_config.cmp_sig_en[2] = false;
    state_config.cmp_sig_en[3] = false;
    state_config.cmp_golden_value[0] = true;
    state_config.cmp_golden_value[1] = false;
#if defined(HPM_IP_FEATURE_LOBS_COMP_LOGIC) && (HPM_IP_FEATURE_LOBS_COMP_LOGIC)
    state_config.cmp_logic = lobs_cmp_logic_and;
#endif
    lobs_state_config(HPM_LOBS, lobs_state_0, &state_config);

    state_config.cmp_mode = lobs_cnt_cmp_mode;
    state_config.state_chg_condition = lobs_cnt_matched;
    state_config.next_state = lobs_next_state_finish;
    state_config.cmp_counter = (TRACE_CNT - TRIG_POINT);
    state_config.cmp_sig_en[0] = false;
    state_config.cmp_sig_en[1] = false;
    state_config.cmp_sig_en[2] = false;
    state_config.cmp_sig_en[3] = false;
    lobs_state_config(HPM_LOBS, lobs_state_1, &state_config);

    lobs_set_pre_trig_enable(HPM_LOBS, true);
    lobs_set_state_enable(HPM_LOBS, lobs_state_0, true);
    lobs_set_state_enable(HPM_LOBS, lobs_state_1, true);

#if defined(HPM_IP_FEATURE_LOBS_IRQ_CTRL) && (HPM_IP_FEATURE_LOBS_IRQ_CTRL)
    lobs_set_irq_enable(HPM_LOBS, lobs_irq_final_mask | lobs_irq_full_mask);
#endif
    intc_m_enable_irq_with_priority(IRQn_LOBS, 1);

    lobs_set_enable(HPM_LOBS, true);

    lobs_lock(HPM_LOBS);
}
#endif

int main(void)
{
    board_init();

    clock_add_to_group(clock_lobs, 0);

    printf("lobs example\n\n");

#if defined(LOBS_USE_TWO_GROUP) && LOBS_USE_TWO_GROUP
    init_lobs_two_group_config();
#else
    init_lobs_one_group_config();
#endif

    while (!lobs_is_trace_finish(HPM_LOBS)) {
        ;
    }
    printf("lobs trace finish!\n");
    printf("trace memory base addr: %#x, trace data final addr: %#x\n\n", (uint32_t)lobs_buffer, lobs_get_final_address(HPM_LOBS));

#if defined(HPM_IP_FEATURE_LOBS_TRIG_ADDR) && (HPM_IP_FEATURE_LOBS_TRIG_ADDR)
     printf("trig addr1: %#x, trig addr2: %#x, trig addr3: %#x, trig addr4: %#x\n\n",
            lobs_get_trig_address(HPM_LOBS, lobs_trig_addr_idx_1), lobs_get_trig_address(HPM_LOBS, lobs_trig_addr_idx_2),
            lobs_get_trig_address(HPM_LOBS, lobs_trig_addr_idx_3), lobs_get_trig_address(HPM_LOBS, lobs_trig_addr_idx_4));
#endif

    while (1) {
        ;
    }

    return 0;
}

SDK_DECLARE_EXT_ISR_M(IRQn_LOBS, lobs_isr)
void lobs_isr(void)
{
    lobs_unlock(HPM_LOBS);
#if defined(HPM_IP_FEATURE_LOBS_IRQ_CTRL) && (HPM_IP_FEATURE_LOBS_IRQ_CTRL)
    if ((lobs_get_irq_status(HPM_LOBS) & lobs_irq_final_mask) == lobs_irq_final_mask) {
        lobs_clear_irq_flag(HPM_LOBS, lobs_irq_final_mask);
        printf("lobs trace finish! isr info.\n");
    }

    if ((lobs_get_irq_status(HPM_LOBS) & lobs_irq_full_mask) == lobs_irq_full_mask) {
        lobs_clear_irq_flag(HPM_LOBS, lobs_irq_full_mask);
        printf("lobs fifo full!\n");
    }
#else
    lobs_clear_fifo_overflow_flag(HPM_LOBS);
    printf("lobs fifo full!\n");
#endif
    lobs_lock(HPM_LOBS);
}
