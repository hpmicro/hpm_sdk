/*
 * Copyright (c) 2026 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include "board.h"
#include "hpm_pdgo_drv.h"

static const uint32_t gpr_value_list[DGO_GPR_WORD_COUNT] = { 0x33221100, 0x77665544, 0xbbaa9988, 0xffeeddcc, 0x12345678, 0x5AA555AA, 0x98765432, 0xFF0000FF };

static void init_pdgo_wuio(void)
{
    dgo_wuio_filter_cfg_t wuio_config;

    wuio_config.enable = true;
    wuio_config.output_invert = false;
    wuio_config.output_init_value = false;
    wuio_config.filter_length = 2000;
    wuio_config.filter_mode = dgo_filter_mode_stable_low;
    wuio_config.irq_mode = dgo_wuio_irq_rise_edge_mode;
    pdgo_config_wuio_filter(HPM_PDGO, dgo_wuio_pin_0, &wuio_config);
    pdgo_config_wuio_filter(HPM_PDGO, dgo_wuio_pin_1, &wuio_config);
    pdgo_config_wuio_filter(HPM_PDGO, dgo_wuio_pin_2, &wuio_config);
    pdgo_config_wuio_filter(HPM_PDGO, dgo_wuio_pin_3, &wuio_config);

    pdgo_disable_all_irq(HPM_PDGO);
    pdgo_enable_irq0_by_bit_mask(HPM_PDGO, dgo_irq0_wuio0_pin | dgo_irq0_wuio1_pin
                                         | dgo_irq0_wuio2_pin | dgo_irq0_wuio3_pin);
}

static void init_pdgo_pcap(void)
{
    dgo_wuio_filter_cfg_t wuio_config;
    dgo_pcap_cfg_t pcap_config;

    wuio_config.enable = true;
    wuio_config.output_invert = false;
    wuio_config.output_init_value = false;
    wuio_config.filter_length = 2;
    wuio_config.filter_mode = dgo_filter_mode_delay;
    wuio_config.irq_mode = dgo_wuio_irq_disable;
    pdgo_config_wuio_filter(HPM_PDGO, dgo_wuio_pin_1, &wuio_config);

    pcap_config.enable = true;
    pcap_config.wuio_select = dgo_wuio_pin_1;
    pcap_config.match_mode = dgo_pcap_match_mode_cnt_ge_cmp;
    pcap_config.dump_mode = dgo_pcap_dump_wuio_rise_edge;
    pcap_config.cnt_dir = dgo_pcap_cnt_up;
    pcap_config.cnt_mode = dgo_pcap_cnt_oneshot;
    pcap_config.cnt_event = dgo_pcap_cnt_wuio_rise_edge;
    pcap_config.cmp_value = 5;
    pcap_config.period_value = 6;
    pdgo_config_pcap(HPM_PDGO, dgo_pcap_0, &pcap_config);

    pdgo_disable_all_irq(HPM_PDGO);
    pdgo_enable_irq1_by_bit_mask(HPM_PDGO, dgo_irq1_pcap0_match);
}

static void init_pdgo_pcnt(void)
{
    dgo_wuio_filter_cfg_t wuio_config;
    dgo_pcnt_cfg_t pcnt_config;

    wuio_config.enable = true;
    wuio_config.output_invert = false;
    wuio_config.output_init_value = false;
    wuio_config.filter_length = 2;
    wuio_config.filter_mode = dgo_filter_mode_delay;
    wuio_config.irq_mode = dgo_wuio_irq_disable;
    pdgo_config_wuio_filter(HPM_PDGO, dgo_wuio_pin_0, &wuio_config);
    pdgo_config_wuio_filter(HPM_PDGO, dgo_wuio_pin_1, &wuio_config);
    pdgo_config_wuio_filter(HPM_PDGO, dgo_wuio_pin_2, &wuio_config);
    pdgo_config_wuio_filter(HPM_PDGO, dgo_wuio_pin_3, &wuio_config);

    pdgo_get_pcnt_defconfig(HPM_PDGO, dgo_pcnt_0, &pcnt_config);
    pcnt_config.enable = true;
    pcnt_config.a_sel = dgo_wuio_pin_0;
    pcnt_config.b_sel = dgo_wuio_pin_1;
    pcnt_config.z_sel = dgo_wuio_pin_2;
    pcnt_config.h_sel = dgo_wuio_pin_3;
    pcnt_config.decode_mode = dgo_pcnt_ab_4x;
    pcnt_config.total_lines = 32;
    pcnt_config.z_mode = dgo_pcnt_z_disable;
    pcnt_config.z_state.edge_mode.rise_forward = 1;
    pcnt_config.z_state.edge_mode.rise_reverse = 1;
    pcnt_config.h_mode = dgo_pcnt_h_disable;
    pcnt_config.h_state.edge_mode.rise_forward = 1;
    pcnt_config.dump_mode = dgo_pcnt_dump_h_edge_mode;
    pcnt_config.dump_state.edge_mode.rise_forward = 1;
    pcnt_config.tacho_mode = dgo_pcnt_tacho_travel_mode;
    pcnt_config.tacho_len = 6;
    pcnt_config.match0_mode = dgo_pcnt_match_range_cmp0_cmp1_mode;
    pcnt_config.range_cmp_src = dgo_pcnt_cmp_src_revolution_line_phase;
    pdgo_config_pcnt(HPM_PDGO, dgo_pcnt_0, &pcnt_config);

    pdgo_disable_all_irq(HPM_PDGO);
    pdgo_enable_irq1_by_bit_mask(HPM_PDGO, dgo_irq1_pcnt0_cmp_match0);
}

void dgo_enable_wakeup_src(void)
{
    pdgo_enable_wkup_pin_wakeup(HPM_PDGO);
}

void dgo_check_wakeup_src(void)
{
    dgo_pcnt_counter_t pcnt_value;

    if (pdgo_get_wakeup_status(HPM_PDGO) & dgo_wakeup_wkup_pin) {
        pdgo_clear_wakeup_status(HPM_PDGO, dgo_wakeup_wkup_pin);
        printf("The System was waken up by Wakeup pin\n");
    }

    if (pdgo_get_wakeup_status(HPM_PDGO) & dgo_wakeup_software) {
        pdgo_clear_wakeup_status(HPM_PDGO, dgo_wakeup_software);
        printf("The System was waken up by software\n");
    }

    if (pdgo_get_wakeup_status(HPM_PDGO) & dgo_wakeup_wuio_pins) {
        pdgo_clear_wakeup_status(HPM_PDGO, dgo_wakeup_wuio_pins);
        printf("The System was waken up by WUIO pins\n");
    }

    if (pdgo_get_wakeup_status(HPM_PDGO) & dgo_wakeup_pcap0) {
        pdgo_clear_wakeup_status(HPM_PDGO, dgo_wakeup_pcap0);
        printf("The System was waken up by PCAP\n");
    }

    if (pdgo_get_wakeup_status(HPM_PDGO) & dgo_wakeup_pcnt0) {
        pdgo_clear_wakeup_status(HPM_PDGO, dgo_wakeup_pcnt0);
        printf("The System was waken up by PCNT\n");
    }

    pdgo_get_pcnt_counter(HPM_PDGO, dgo_pcnt_0, &pcnt_value);
    printf("Current PCNT counter value is phase=%d, line=%d, revolution=%d\n", pcnt_value.phase, pcnt_value.line, pcnt_value.revolution);
}

void dgo_check_gpr_retention(void)
{
    if (pdgo_is_retention_mode_enabled(HPM_PDGO)) {
        bool equal = true;
        for (uint32_t i = 0; i < DGO_GPR_WORD_COUNT; i++) {
            if (pdgo_read_gpr(HPM_PDGO, i) != gpr_value_list[i]) {
                equal = false;
                break;
            }
        }
        printf("DGO GPR register values are %s\n", equal ? "as expected" : "not as expected");
    }
    pdgo_enable_retention_mode(HPM_PDGO);
}

void dgo_turn_off(uint32_t turnoff_in_us)
{
    uint32_t turnoff_counter = pdgo_get_turnoff_counter_from_us(turnoff_in_us);
    pdgo_clear_wakeup_status(HPM_PDGO, dgo_wakeup_wkup_pin | dgo_wakeup_software);
    pdgo_set_turnoff_counter(HPM_PDGO, turnoff_counter);
    printf("Actual Turn-off time is %dus, counter is %d\n", pdgo_get_us_from_turnoff_counter(turnoff_counter), turnoff_counter);
    printf("Wait until DGO turns off\n");
    while (true) {
    }
}

void dgo_turn_off_sys_oneshot_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us)
{
    uint32_t wakeup_counter;

    pdgo_enable_software_wakeup(HPM_PDGO);
    wakeup_counter = pdgo_get_wakeup_counter_from_us(wakeup_in_us);
    printf("Actual Wake-up time is %dus, counter=%d\n", pdgo_get_us_from_wakeup_counter(wakeup_counter), wakeup_counter);
    pdgo_set_wakeup_counter(HPM_PDGO, wakeup_counter);
    dgo_turn_off(turnoff_in_us);
}

void dgo_turn_off_sys_auto_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us)
{
    uint32_t wakeup_counter;

    pdgo_enable_software_wakeup(HPM_PDGO);
    wakeup_counter = pdgo_get_wakeup_counter_from_us(wakeup_in_us);
    printf("Actual Wake-up time is %dus, counter=%d\n", pdgo_get_us_from_wakeup_counter(wakeup_counter), wakeup_counter);
    pdgo_set_cycle_wakeup_counter(HPM_PDGO, wakeup_counter);
    dgo_turn_off(turnoff_in_us);
}

void dgo_gpr_retention_oneshot_wakeup(uint32_t turnoff_in_us, uint32_t wakeup_in_us)
{
    uint32_t wakeup_counter;

    pdgo_enable_software_wakeup(HPM_PDGO);

    for (uint32_t i = 0; i < DGO_GPR_WORD_COUNT; i++) {
        pdgo_write_gpr(HPM_PDGO, i, gpr_value_list[i]);
    }

    wakeup_counter = pdgo_get_wakeup_counter_from_us(wakeup_in_us);
    printf("Actual Wake-up time is %dus, counter=%d\n", pdgo_get_us_from_wakeup_counter(wakeup_counter), wakeup_counter);
    pdgo_set_wakeup_counter(HPM_PDGO, wakeup_counter);
    dgo_turn_off(turnoff_in_us);
}

void dgo_turn_off_wkup_pin_wakeup(uint32_t turnoff_in_us)
{
    printf("Press WKUP or RESET pin to wake up from the shutdown mode\n");

    pdgo_enable_wkup_pin_wakeup(HPM_PDGO);
    dgo_turn_off(turnoff_in_us);
}

void dgo_turn_off_wuio_wakeup(uint32_t turnoff_in_us)
{
    printf("Use WUIO pins or RESET pin to wake up from the shutdown mode\n");

    init_pdgo_wuio();
    pdgo_enable_wuio_pins_wakeup(HPM_PDGO);
    dgo_turn_off(turnoff_in_us);
}

void dgo_turn_off_pcap_wakeup(uint32_t turnoff_in_us)
{
    printf("Use pulse capture or RESET pin to wake up from the shutdown mode\n");

    init_pdgo_pcap();
    pdgo_set_pcap_counter_value(HPM_PDGO, dgo_pcap_0, 0);
    pdgo_enable_pulse_capture0_wakeup(HPM_PDGO);
    dgo_turn_off(turnoff_in_us);
}

void dgo_turn_off_pcnt_wakeup(uint32_t turnoff_in_us)
{
    uint32_t cmp0, cmp1;
    dgo_pcnt_counter_t pcnt_counter;

    init_pdgo_pcnt();

    printf("Use pulse counter or RESET pin to wake up from the shutdown mode\n");

    pcnt_counter.phase = 0;
    pcnt_counter.line = 8;
    pcnt_counter.revolution = 1;
    cmp0 = pdgo_convert_pcnt_counter(HPM_PDGO, dgo_pcnt_0, &pcnt_counter);
    pcnt_counter.phase = 0;
    pcnt_counter.line = 24;
    pcnt_counter.revolution = 2;
    cmp1 = pdgo_convert_pcnt_counter(HPM_PDGO, dgo_pcnt_0, &pcnt_counter);
    pdgo_config_pcnt_range_compare(HPM_PDGO, dgo_pcnt_0, cmp0, cmp1);

    pdgo_enable_pulse_counter0_wakeup(HPM_PDGO);
    dgo_turn_off(turnoff_in_us);
}